// SPDX-License-Identifier: MIT

package main

import (
	"bytes"
	"fmt"
	"math"
	"os/exec"
	"sort"
	"strconv"
	"strings"
	"time"
)

// Compute sample sizing. One pass of a small workload (a tax return, or euro's
// 100 records) is too short to time reliably, so each timed sample covers
// `batch` back-to-back passes, chosen so a sample lasts at least minSampleNs.
// The number of samples is set by a time budget, not a record count.
const (
	minSampleNs      = 100_000     // >= 100 us per timed sample
	computeBudgetNs  = 200_000_000 // ~200 ms of timed samples per measurement
	computeProcesses = 3           // separate measured processes per row
	xcheckMin        = 0.9
	xcheckMaxDefault = 1.25
)

// procResult is one finished process run.
type procResult struct {
	stdout, stderr []byte
	code           int
	wallNs         int64 // spawn -> exit
	err            error // failed to start, or killed
}

// runProc runs one process to completion and times it from spawn to exit.
func runProc(dir string, env []string, exe string, args ...string) procResult {
	cmd := exec.Command(exe, args...)
	cmd.Dir = dir
	cmd.Env = env
	var so, se bytes.Buffer
	cmd.Stdout, cmd.Stderr = &so, &se
	t0 := time.Now()
	err := cmd.Run()
	wall := time.Since(t0).Nanoseconds()
	r := procResult{stdout: so.Bytes(), stderr: se.Bytes(), wallNs: wall}
	if ee, ok := err.(*exec.ExitError); ok {
		r.code = ee.ExitCode()
	} else if err != nil {
		r.err, r.code = err, -1
	}
	return r
}

// computeRun is one process run in compute mode.
type computeRun struct {
	npr     float64   // the harness's best sample, ns/record
	batch   int       // passes per sample, as the harness reports it
	samples []float64 // every sample, ns/record
	wallNs  int64
	err     string // "" = ok
}

// marker returns the value of the last `name=` line on stdout.
func marker(stdout []byte, name string) (string, bool) {
	val, found := "", false
	for _, ln := range strings.Split(string(stdout), "\n") {
		ln = strings.TrimSpace(ln)
		if strings.HasPrefix(ln, name+"=") {
			val, found = ln[len(name)+1:], true
		}
	}
	return val, found
}

// parseCompute reads the compute markers from one run.
func parseCompute(p procResult, records int) computeRun {
	r := computeRun{batch: 1, wallNs: p.wallNs}
	if s, ok := marker(p.stdout, "COMPUTE_BATCH"); ok {
		if k, err := strconv.Atoi(s); err == nil {
			r.batch = k
		}
	}
	npr := math.NaN()
	if s, ok := marker(p.stdout, "COMPUTE_NS_PER_RECORD"); ok {
		if v, err := strconv.ParseFloat(s, 64); err == nil {
			npr = v
		}
	}
	switch {
	case p.err != nil:
		r.err = p.err.Error()
	case p.code != 0:
		r.err = fmt.Sprintf("exit %d %s", p.code, lastLine(p.stdout, p.stderr))
	case math.IsNaN(npr) || (npr <= 0):
		r.err = "no COMPUTE_NS_PER_RECORD marker"
	default:
		r.npr = npr
	}
	if s, ok := marker(p.stdout, "COMPUTE_SAMPLES_NS"); ok {
		for _, f := range strings.Split(s, ",") {
			if v, err := strconv.ParseInt(f, 10, 64); err == nil {
				r.samples = append(r.samples, float64(v)/float64(r.batch*records))
			}
		}
	}
	return r
}

// lastLine is a short error text: the first ERROR line, else the last line.
func lastLine(outs ...[]byte) string {
	var lines []string
	for _, o := range outs {
		for _, ln := range strings.Split(string(o), "\n") {
			if ln = strings.TrimSpace(ln); ln != "" {
				lines = append(lines, ln)
			}
		}
	}
	if len(lines) == 0 {
		return ""
	}
	msg := lines[len(lines)-1]
	for _, ln := range lines {
		if strings.Contains(ln, "ERROR") {
			msg = ln
			break
		}
	}
	if len(msg) > 120 {
		msg = msg[:120]
	}
	return msg
}

// sizeCompute returns (samples, batch) for a measured run, from a per-record time.
func sizeCompute(records, base int, npr float64) (int, int) {
	passNs := npr * float64(max(records, 1))
	batch := max(1, int(math.Ceil(minSampleNs/passNs)))
	sampleNs := float64(batch) * passNs
	samples := max(base, int(math.Ceil(float64(base)/float64(batch))),
		int(math.Ceil(computeBudgetNs/sampleNs)))
	return samples, batch
}

// computeStats is the result of one compute measurement.
type computeStats struct {
	npr, p10, median, p90 float64
	n                     int
	procMin               []float64
	batch, samples        int
	sampleNs              float64
	passes                int
	wallNs, wall2Ns       int64
	err                   string
}

// measureCompute probes, sizes, then measures the in-process compute loop.
//
// A probe (one pass per sample) sizes the measured run, which is re-sized and
// repeated while its samples fall short of minSampleNs (at most twice: the
// probe's single-pass minimum overstates the per-pass time). The final
// configuration then runs in computeProcesses separate processes, each paired
// with a twice-as-long run for the external cross-check. The headline value is
// the fastest process's best sample; p10 / median / p90 cover the second half
// of each process's samples (the first half counts as warm-up).
func measureCompute(run func(kind string, samples, batch int) computeRun, records, base int) computeStats {
	fail := func(what string, r computeRun) computeStats {
		return computeStats{err: what + ": " + r.err}
	}
	r := run("probe", base, 1)
	if r.err != "" {
		return fail("probe", r)
	}
	npr := r.npr
	var samples, got int
	for i := 0; i < 3; i++ {
		var want int
		samples, want = sizeCompute(records, base, npr)
		r = run("measure", samples, want)
		if r.err != "" {
			return fail("measured run", r)
		}
		got = r.batch
		if got != want { // the harness does not batch: keep its result
			break
		}
		if r.npr*float64(records*got) >= minSampleNs {
			break
		}
		npr = r.npr
	}
	procs := []computeRun{r}
	wall, wall2 := r.wallNs, int64(-1)
	for i := 0; i < computeProcesses; i++ {
		if i > 0 {
			ri := run("repeat", samples, got)
			if ri.err != "" {
				return fail("repeat run", ri)
			}
			procs = append(procs, ri)
			wall = min(wall, ri.wallNs)
		}
		r2 := run("xcheck", 2*samples, got)
		if r2.err != "" {
			return fail("cross-check run", r2)
		}
		if (wall2 < 0) || (r2.wallNs < wall2) {
			wall2 = r2.wallNs
		}
	}
	var pooled, procMin []float64
	best := math.Inf(1)
	for _, p := range procs {
		if len(p.samples) > 0 {
			pooled = append(pooled, p.samples[len(p.samples)/2:]...)
		} else {
			pooled = append(pooled, p.npr)
		}
		procMin = append(procMin, p.npr)
		best = math.Min(best, p.npr)
	}
	sort.Float64s(pooled)
	return computeStats{
		npr: best, p10: percentile(pooled, 0.10), median: percentile(pooled, 0.50),
		p90: percentile(pooled, 0.90), n: len(pooled), procMin: procMin,
		batch: got, samples: samples, sampleNs: best * float64(records*got),
		passes: samples * got, wallNs: wall, wall2Ns: wall2,
	}
}

// percentile is the linear-interpolated percentile (q in [0, 1]) of an
// ascending slice.
func percentile(v []float64, q float64) float64 {
	if len(v) == 0 {
		return math.NaN()
	}
	k := float64(len(v)-1) * q
	lo := int(math.Floor(k))
	hi := min(lo+1, len(v)-1)
	return v[lo] + (v[hi]-v[lo])*(k-float64(lo))
}

// crossCheck is the runner's own per-record time: the wall-time difference
// between the measured run and the twice-as-long run is exactly `passes` more
// passes (startup and the gated pass cancel). It still includes per-pass work
// outside the harness's timed region, so it can sit above the internal value;
// a ratio outside [xcheckMin, limit] flags the row.
func crossCheck(cs computeStats, records int, limit float64) (ext, ratio float64, flag bool, ok bool) {
	if (cs.err != "") || (cs.passes == 0) || (cs.wall2Ns < 0) {
		return 0, 0, false, false
	}
	ext = float64(cs.wall2Ns-cs.wallNs) / float64(cs.passes*records)
	ratio = ext / cs.npr
	return ext, ratio, !((xcheckMin <= ratio) && (ratio <= limit)), true
}
