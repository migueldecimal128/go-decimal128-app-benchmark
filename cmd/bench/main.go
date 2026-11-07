// SPDX-License-Identifier: MIT

// Command bench builds, gates and times the UW-Madison application benchmarks,
// appends one record per row to results/data/<app>.jsonl, and writes
// results/RESULTS.md. Run it from the repository root:
//
//	go run ./cmd/bench -lib all                                  # every library, every app
//	go run ./cmd/bench -lib miguel-decimal128-go -app banking    # one library, one app
//	go run ./cmd/bench -report-only                              # rewrite RESULTS.md from the records
//
// Two clocks per row. whole: process spawn -> exit for one gated run (fastest
// of -iters). compute: the program's own timed loop (D128_COMPUTE_ITERS /
// D128_COMPUTE_BATCH; markers on stdout), cross-checked against the runner's
// clock. Every whole run is gated against the golden.
package main

import (
	"bytes"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"sort"
	"strings"
	"time"
)

// library is one decimal library the apps can be built against. Its adapter
// file in internal/dec is selected by the build tag; the module path gives the
// version go.mod pins.
type library struct {
	name   string // shown in the report
	tag    string // go build tag that selects its adapter (there is no default)
	module string
}

var libraries = []library{
	{name: "miguel-decimal128-go", tag: "miguel_decimal128_go", module: "github.com/migueldecimal128/decimal128-go"},
}

// suite is one row's workload: a program, its arguments, and how to gate it.
type suite struct {
	app, sub, dataset string
	dir               string   // working directory, relative to the repo root
	exe               string   // program name under the build dir
	args              []string // arguments, relative to dir
	golden            string   // stderr golden, relative to dir ("" = file-gated); a .sha256 file holds its digest
	totals            string   // lines that must appear in stdout (telco's control totals)
	files             bool     // risk: gate one output file per stock
	records           int
	xcheckMax         float64
}

func suites() []suite {
	tax := func(sub string) suite {
		u := strings.ToUpper(sub)
		return suite{app: "tax_preparation", sub: sub, dataset: "Tax." + u + ".bin.BID128",
			dir: "data/tax_preparation", exe: "tax-" + sub,
			args:   []string{"testGen/bin/Tax." + u + ".bin.BID128", "/tmp/abtax_" + sub, "1"},
			golden: "golden/Tax." + u + ".stderr.golden", records: 1}
	}
	return []suite{
		{app: "euro_conversion", dataset: "Euro.bin.BID128", dir: "data/euro_conversion",
			exe: "euro", args: []string{"testGen/bin/Euro.bin.BID128", "euro_run.out"},
			golden: "golden/Euro.output.DECNUMBER_FIXED128", records: 100},
		{app: "risk_management", dataset: "sp500 + 20 tickers", dir: "data/risk_management",
			exe: "risk", args: []string{"1", "bench"}, files: true, records: 20},
		tax("fed"), tax("ca"), tax("nc"),
		// banking rebuilds its accounts between passes outside the timed region,
		// which the runner's clock sees, so its cross-check limit is wider.
		{app: "banking", dataset: "Banking.bin.BID128", dir: "data/banking",
			exe: "banking", args: []string{"testGen/bin/Banking.bin.BID128"},
			golden: "golden/Banking.output.DECNUMBER_FIXED128", records: 643, xcheckMax: 2.5},
		// telco's expected output is 6 MB, so the golden is its SHA-256; the
		// control totals are Cowlishaw's published values.
		{app: "telco", dataset: "expon180.1e6b", dir: "data/telco", exe: "telco",
			args: []string{"expon180.1e6b"}, golden: "golden/expon180.1e6.expected.sha256",
			totals: "golden/expon180.1e6.totals", records: 1_000_000},
	}
}

var appOrder = []string{"euro_conversion", "risk_management", "tax_preparation", "banking", "telco"}

// env is the inherited environment without any compute-mode variables, so a
// whole run is never accidentally a compute run.
func baseEnv() []string {
	var out []string
	for _, kv := range os.Environ() {
		if !strings.HasPrefix(kv, "D128_COMPUTE_") {
			out = append(out, kv)
		}
	}
	return out
}

// runInfo is what every record of one invocation shares.
type runInfo struct {
	ts, arch, cpu, goVersion string
	libVersions              map[string]string // library name -> pinned version
	benchCommit              string            // this repository's HEAD
	benchDirty               bool              // uncommitted changes outside results/
}

func main() {
	app := flag.String("app", "", "only this app (euro_conversion, risk_management, tax_preparation, banking, telco)")
	libName := flag.String("lib", "", "libraries to run: comma-separated names, or all (required)")
	iters := flag.Int("iters", 9, "whole runs per row (fastest kept; every run is gated)")
	reportOnly := flag.Bool("report-only", false, "rewrite results/RESULTS.md from the records only")
	flag.Parse()

	root, err := os.Getwd()
	check(err)
	if _, err := os.Stat(filepath.Join(root, "go.mod")); err != nil {
		fatal("run from the repository root (go run ./cmd/bench)")
	}
	if !*reportOnly {
		var names []string
		for _, l := range libraries {
			names = append(names, l.name)
		}
		if *libName == "" {
			fatal("name the libraries to run: -lib <name>[,<name>...] or -lib all; libraries: " + strings.Join(names, ", "))
		}
		var libs []library
		for _, want := range strings.Split(*libName, ",") {
			found := false
			for _, l := range libraries {
				if (want == "all") || (want == l.name) {
					libs, found = append(libs, l), true
				}
			}
			if !found {
				fatal("unknown library " + want + "; libraries: " + strings.Join(names, ", "))
			}
		}
		info := runInfo{ts: utcNow(), arch: archName(), cpu: cpuModel(),
			goVersion: goEnv("GOVERSION"), libVersions: map[string]string{}}
		info.benchCommit, info.benchDirty = benchStamp(root)
		for _, l := range libs {
			info.libVersions[l.name] = moduleVersion(l.module)
		}
		bin := filepath.Join(root, "results", ".build")
		dirty := ""
		if info.benchDirty {
			dirty = " + uncommitted changes"
		}
		fmt.Printf("[build] %s, bench %.7s%s\n", info.goVersion, info.benchCommit, dirty)
		build(root, bin, libs, info)
		verifyLibraries(root, bin, libs)
		st := &startup{exe: filepath.Join(bin, "noop"), dir: root, env: baseEnv()}
		ran := false
		for _, a := range appOrder {
			if (*app != "") && (*app != a) {
				continue
			}
			var jobs []*job
			for _, s := range suites() {
				if s.app != a {
					continue
				}
				for _, l := range libs {
					jobs = append(jobs, newJob(func(run runFunc) { measureRow(root, bin, s, l, *iters, info, run) }))
				}
			}
			jobs = append(jobs, newJob(func(run runFunc) { st.measure(*iters, run) }))
			fmt.Printf("[measure] %s\n", a)
			interleave(jobs)
			ran = true
		}
		if !ran {
			fatal("unknown app " + *app)
		}
		st.record(root, info)
	}
	check(writeReport(root))
	fmt.Println("[report] results/RESULTS.md")
}

// build compiles the startup baseline into bin, and every app once per
// library into bin/<library>/.
func build(root, bin string, libs []library, info runInfo) {
	goBuild := func(out string, tags string, pkgs ...string) {
		check(os.MkdirAll(out, 0o755))
		args := []string{"build", "-trimpath"}
		if tags != "" {
			args = append(args, "-tags", tags)
		}
		args = append(append(args, "-o", out+string(os.PathSeparator)), pkgs...)
		cmd := exec.Command("go", args...)
		cmd.Dir, cmd.Stdout, cmd.Stderr = root, os.Stdout, os.Stderr
		check(cmd.Run())
	}
	goBuild(bin, "", "./baseline/noop")
	for _, l := range libs {
		fmt.Printf("[build] %s %s\n", l.module, info.libVersions[l.name])
		goBuild(filepath.Join(bin, l.name), l.tag, "./apps/...")
	}
}

// verifyLibraries launches every built program once, without arguments, and
// checks the LIBRARY=<name> line it prints first against the library it was
// built for. (An unknown tag already fails to compile, via internal/dec's
// nolib.go; this catches a table entry carrying another library's tag, which
// would record that library's results under the wrong name.) The launch also
// absorbs a newly built program's slow first start.
func verifyLibraries(root, bin string, libs []library) {
	seen := map[string]bool{}
	for _, l := range libs {
		for _, s := range suites() {
			exe := filepath.Join(bin, l.name, s.exe)
			if seen[exe] {
				continue
			}
			seen[exe] = true
			p := runProc(root, baseEnv(), exe)
			got, _ := marker(p.stdout, "LIBRARY")
			if got != l.name {
				fatal(fmt.Sprintf("%s was built for library %q (build tag %q) but reports LIBRARY=%q: "+
					"check the tag in the libraries table and the adapter file's //go:build line", exe, l.name, l.tag, got))
			}
		}
	}
	fmt.Printf("[build] verified: every program reports the library it was built for\n")
}

// --- interleaving ----------------------------------------------------------

// A job is one row's measurement, run as a coroutine: it hands the scheduler
// one process run at a time and waits for the result.
type job struct {
	req chan func() any
	res chan any
}

type runFunc func(fn func() any) any

func newJob(body func(run runFunc)) *job {
	j := &job{req: make(chan func() any), res: make(chan any)}
	go func() {
		defer close(j.req)
		body(func(fn func() any) any { j.req <- fn; return <-j.res })
	}()
	return j
}

// interleave runs jobs round-robin, one process at a time: A, B, C, A, B, C...
// This spreads thermal and load drift across all rows of an app instead of
// letting it land on whichever row runs last. The scheduler waits for a job
// to ask for its next run before moving on, so no job's own work overlaps
// another job's process.
func interleave(jobs []*job) {
	type live struct {
		j  *job
		fn func() any
	}
	var cur []live
	for _, j := range jobs {
		if fn, ok := <-j.req; ok {
			cur = append(cur, live{j, fn})
		}
	}
	for len(cur) > 0 {
		var next []live
		for _, l := range cur {
			l.j.res <- l.fn()
			if fn, ok := <-l.j.req; ok {
				next = append(next, live{l.j, fn})
			}
		}
		cur = next
	}
}

// --- one row ---------------------------------------------------------------

type logEntry [3]string // kind, start UTC, stop UTC

type record struct {
	App                string     `json:"app"`
	Sub                *string    `json:"sub"`
	Impl               string     `json:"impl"`
	Library            string     `json:"library"`
	LibraryVersion     string     `json:"library_version"`
	Dataset            string     `json:"dataset"`
	Records            int        `json:"records"`
	Correct            bool       `json:"correct"`
	GateRunsPassed     int        `json:"gate_runs_passed"`
	GateRuns           int        `json:"gate_runs"`
	GoldenPassed       *int       `json:"golden_passed,omitempty"`
	GoldenTotal        *int       `json:"golden_total,omitempty"`
	WholeNs            int64      `json:"whole_ns"`
	WholeNsPerRecord   float64    `json:"whole_ns_per_record"`
	WholeRunsNs        []int64    `json:"whole_runs_ns"`
	ComputeNsPerRecord *float64   `json:"compute_ns_per_record"`
	ComputeP10         *float64   `json:"compute_p10"`
	ComputeMedian      *float64   `json:"compute_median"`
	ComputeP90         *float64   `json:"compute_p90"`
	ComputeNSamples    int        `json:"compute_n_samples"`
	ComputeProcessMin  []float64  `json:"compute_process_min"`
	ComputeStatus      string     `json:"compute_status"`
	ComputeError       string     `json:"compute_error,omitempty"`
	ComputeBatch       int        `json:"compute_batch"`
	ComputeSamples     int        `json:"compute_samples"`
	ComputeSampleNs    float64    `json:"compute_sample_ns"`
	XcheckNsPerRecord  *float64   `json:"compute_xcheck_ns_per_record"`
	XcheckRatio        *float64   `json:"compute_xcheck_ratio"`
	XcheckFlag         *bool      `json:"compute_xcheck_flag"`
	XcheckLimit        float64    `json:"compute_xcheck_limit"`
	Iterations         int        `json:"iterations"`
	Arch               string     `json:"arch"`
	CPU                string     `json:"cpu"`
	GoVersion          string     `json:"go_version"`
	BenchCommit        string     `json:"bench_commit"`
	BenchDirty         bool       `json:"bench_dirty"`
	TimestampUTC       string     `json:"timestamp_utc"`
	RunLog             []logEntry `json:"run_log"`
	WrittenUTC         string     `json:"written_utc"`
}

// startupRecord is one startup-baseline measurement.
type startupRecord struct {
	StartupNs     int64   `json:"startup_ns"`
	StartupRunsNs []int64 `json:"startup_runs_ns"`
	Iterations    int     `json:"iterations"`
	Arch          string  `json:"arch"`
	CPU           string  `json:"cpu"`
	GoVersion     string  `json:"go_version"`
	BenchCommit   string  `json:"bench_commit"`
	BenchDirty    bool    `json:"bench_dirty"`
	TimestampUTC  string  `json:"timestamp_utc"`
}

func measureRow(root, bin string, s suite, lib library, iters int, info runInfo, run runFunc) {
	dir := filepath.Join(root, s.dir)
	exe := filepath.Join(bin, lib.name, s.exe)
	env := baseEnv()
	var log []logEntry
	stamped := func(kind string, fn func() any) any {
		start := utcNow()
		r := run(fn)
		log = append(log, logEntry{kind, start, utcNow()})
		return r
	}

	var stocks []string
	resultDir := filepath.Join(dir, "testGen", "result")
	if s.files {
		gs, _ := filepath.Glob(filepath.Join(dir, "golden", "*.golden"))
		for _, g := range gs {
			stocks = append(stocks, strings.TrimSuffix(filepath.Base(g), ".golden"))
		}
		check(os.MkdirAll(resultDir, 0o755))
		defer os.RemoveAll(resultDir)
	}
	var golden []byte
	var goldenSum string // hex SHA-256, when the golden is a digest
	if !s.files {
		var err error
		golden, err = os.ReadFile(filepath.Join(dir, s.golden))
		check(err)
		if strings.HasSuffix(s.golden, ".sha256") {
			goldenSum, golden = strings.Fields(string(golden))[0], nil
		}
	}
	var wantLines []string
	if s.totals != "" {
		b, err := os.ReadFile(filepath.Join(dir, s.totals))
		check(err)
		wantLines = strings.Fields(string(b))
	}
	// gateTranscript: a clean exit, stderr matching the golden (or its
	// digest), and every required line present in stdout.
	gateTranscript := func(p procResult) bool {
		if (p.code != 0) || (p.err != nil) {
			return false
		}
		if goldenSum != "" {
			sum := sha256.Sum256(p.stderr)
			if hex.EncodeToString(sum[:]) != goldenSum {
				return false
			}
		} else if !bytes.Equal(p.stderr, golden) {
			return false
		}
		have := map[string]bool{}
		for _, ln := range strings.Split(string(p.stdout), "\n") {
			have[strings.TrimSpace(ln)] = true
		}
		for _, w := range wantLines {
			if !have[w] {
				return false
			}
		}
		return true
	}

	// Gate one run. Risk writes testGen/result/<stock>.log_0_<suffix>; each file
	// is compared and removed, so every run must write its own.
	gateFiles := func() int {
		n := 0
		for _, st := range stocks {
			out := filepath.Join(resultDir, st+".log_0_"+s.args[1])
			got, err1 := os.ReadFile(out)
			want, err2 := os.ReadFile(filepath.Join(dir, "golden", st+".golden"))
			if (err1 == nil) && (err2 == nil) && bytes.Equal(got, want) {
				n++
			}
			os.Remove(out)
		}
		return n
	}

	// Warm-up: one untimed, ungated run first. The first launch of a newly
	// built program is much slower (140-250 ms on macOS: a one-time check of
	// the new file), and it also brings the input into the file cache.
	stamped("warmup", func() any {
		runProc(dir, env, exe, s.args...)
		if s.files {
			gateFiles()
		}
		return nil
	})

	// Whole clock: spawn -> exit of one gated run, fastest of iters.
	type wholeRes struct {
		wall int64
		pass int // units matched (1 for a transcript)
	}
	best := int64(-1)
	var wholeRuns []int64
	minPass, allRuns := len(stocks), 0
	for i := 0; i < iters; i++ {
		w := stamped("whole", func() any {
			p := runProc(dir, env, exe, s.args...)
			if s.files {
				return wholeRes{p.wallNs, gateFiles()}
			}
			ok := 0
			if gateTranscript(p) {
				ok = 1
			}
			return wholeRes{p.wallNs, ok}
		}).(wholeRes)
		wholeRuns = append(wholeRuns, w.wall)
		if (best < 0) || (w.wall < best) {
			best = w.wall
		}
		full := 1
		if s.files {
			full = len(stocks)
			minPass = min(minPass, w.pass)
		}
		if w.pass == full {
			allRuns++
		}
	}

	// Compute clock.
	cs := measureCompute(func(kind string, samples, batch int) computeRun {
		return stamped(kind, func() any {
			cenv := append(append([]string{}, env...),
				fmt.Sprintf("D128_COMPUTE_ITERS=%d", samples), fmt.Sprintf("D128_COMPUTE_BATCH=%d", batch))
			p := runProc(dir, cenv, exe, s.args...)
			if s.files {
				gateFiles() // the compute run's gated pass writes the files too
			}
			return parseCompute(p, s.records)
		}).(computeRun)
	}, s.records, iters)

	limit := s.xcheckMax
	if limit == 0 {
		limit = xcheckMaxDefault
	}
	rec := record{
		App: s.app, Impl: lib.name, Library: lib.module, LibraryVersion: info.libVersions[lib.name],
		Dataset: s.dataset, Records: s.records, Correct: allRuns == iters,
		GateRunsPassed: allRuns, GateRuns: iters,
		WholeNs: best, WholeNsPerRecord: float64(best) / float64(s.records), WholeRunsNs: wholeRuns,
		ComputeStatus: "ok", XcheckLimit: limit, Iterations: iters,
		Arch: info.arch, CPU: info.cpu, GoVersion: info.goVersion,
		BenchCommit: info.benchCommit, BenchDirty: info.benchDirty, TimestampUTC: info.ts,
	}
	if s.sub != "" {
		sub := s.sub
		rec.Sub = &sub
	}
	if s.files {
		p, t := minPass, len(stocks)
		rec.GoldenPassed, rec.GoldenTotal = &p, &t
	}
	if cs.err != "" {
		rec.ComputeStatus, rec.ComputeError = "failed", cs.err
	} else {
		rec.ComputeNsPerRecord, rec.ComputeP10 = ptr(cs.npr), ptr(cs.p10)
		rec.ComputeMedian, rec.ComputeP90 = ptr(cs.median), ptr(cs.p90)
		rec.ComputeNSamples, rec.ComputeProcessMin = cs.n, cs.procMin
		rec.ComputeBatch, rec.ComputeSamples, rec.ComputeSampleNs = cs.batch, cs.samples, cs.sampleNs
		if ext, ratio, flag, ok := crossCheck(cs, s.records, limit); ok {
			rec.XcheckNsPerRecord, rec.XcheckRatio, rec.XcheckFlag = ptr(ext), ptr(ratio), &flag
		}
	}
	rec.RunLog = log
	rec.WrittenUTC = utcNow()
	appendRecord(root, s.app, rec)

	label := lib.name
	if s.sub != "" {
		label += "/" + s.sub
	}
	gate := "GOLDEN OK"
	if s.files {
		gate = fmt.Sprintf("GOLDEN %d/%d", minPass, len(stocks))
	} else if !rec.Correct {
		gate = "GOLDEN MISMATCH"
	}
	comp := "FAILED (" + cs.err + ")"
	if cs.err == "" {
		comp = fmt.Sprintf("%11.1f", cs.npr)
		if rec.XcheckRatio != nil {
			comp += fmt.Sprintf("  xcheck %4.2f", *rec.XcheckRatio)
			if *rec.XcheckFlag {
				comp += "  FLAG"
			}
		}
	}
	fmt.Printf("  %-26s whole %13.1f  %-16s compute %s\n", label, rec.WholeNsPerRecord, gate, comp)
}

// startup times the do-nothing baseline program, launched the same way as the
// apps, so `whole - startup` is a program's own work. It runs as one more job
// in every app's interleaved measurement: after one untimed warm-up launch,
// each round launches it once beside each row's whole run, so the baseline is
// taken under the same conditions as the times it is subtracted from (on this
// machine, back-to-back launches get faster and an idle pause undoes that).
// All launches of the run are pooled into one record.
type startup struct {
	exe, dir string
	env      []string
	runs     []int64
}

func (st *startup) measure(iters int, run runFunc) {
	run(func() any { runProc(st.dir, st.env, st.exe); return nil }) // untimed warm-up
	for i := 0; i < iters; i++ {
		st.runs = append(st.runs, run(func() any { return runProc(st.dir, st.env, st.exe).wallNs }).(int64))
	}
}

func (st *startup) record(root string, info runInfo) {
	if len(st.runs) == 0 {
		return
	}
	best := st.runs[0]
	for _, v := range st.runs {
		best = min(best, v)
	}
	appendRecord(root, "startup", startupRecord{StartupNs: best, StartupRunsNs: st.runs,
		Iterations: len(st.runs), Arch: info.arch, CPU: info.cpu, GoVersion: info.goVersion,
		BenchCommit: info.benchCommit, BenchDirty: info.benchDirty, TimestampUTC: info.ts})
	fmt.Printf("[measure] startup baseline %.2f ms (fastest of %d launches)\n", float64(best)/1e6, len(st.runs))
}

// --- records and environment ------------------------------------------------

func appendRecord(root, app string, rec any) {
	dir := filepath.Join(root, "results", "data")
	check(os.MkdirAll(dir, 0o755))
	b, err := json.Marshal(rec)
	check(err)
	f, err := os.OpenFile(filepath.Join(dir, app+".jsonl"), os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0o644)
	check(err)
	defer f.Close()
	_, err = f.Write(append(b, '\n'))
	check(err)
}

func ptr(v float64) *float64 { return &v }

func utcNow() string { return time.Now().UTC().Format("2006-01-02T15:04:05.000Z") }

func archName() string {
	if runtime.GOARCH == "amd64" {
		return "x86_64"
	}
	return runtime.GOARCH
}

func cpuModel() string {
	if runtime.GOOS == "darwin" {
		if out, err := exec.Command("sysctl", "-n", "machdep.cpu.brand_string").Output(); err == nil {
			return strings.TrimSpace(string(out))
		}
	}
	if b, err := os.ReadFile("/proc/cpuinfo"); err == nil {
		for _, ln := range strings.Split(string(b), "\n") {
			if strings.HasPrefix(ln, "model name") {
				if i := strings.Index(ln, ":"); i >= 0 {
					return strings.TrimSpace(ln[i+1:])
				}
			}
		}
	}
	return "unknown"
}

// benchStamp is this repository's HEAD commit, and whether anything outside
// results/ differs from it (the run itself writes results/).
func benchStamp(root string) (string, bool) {
	out, err := exec.Command("git", "-C", root, "rev-parse", "HEAD").Output()
	if err != nil {
		return "unknown", false
	}
	st, err := exec.Command("git", "-C", root, "status", "--porcelain", "--", ".", ":(exclude)results").Output()
	return strings.TrimSpace(string(out)), (err == nil) && (len(bytes.TrimSpace(st)) > 0)
}

func goEnv(key string) string {
	out, err := exec.Command("go", "env", key).Output()
	if err != nil {
		return "unknown"
	}
	return strings.TrimSpace(string(out))
}

// moduleVersion is the version go.mod pins for a module.
func moduleVersion(module string) string {
	out, err := exec.Command("go", "list", "-m", "-f", "{{.Version}}", module).Output()
	if err != nil {
		return "unknown"
	}
	return strings.TrimSpace(string(out))
}

func check(err error) {
	if err != nil {
		fatal(err.Error())
	}
}

func fatal(msg string) {
	fmt.Fprintln(os.Stderr, "bench:", msg)
	os.Exit(1)
}

// sortedKeys returns a map's keys in order.
func sortedKeys[V any](m map[string]V) []string {
	keys := make([]string, 0, len(m))
	for k := range m {
		keys = append(keys, k)
	}
	sort.Strings(keys)
	return keys
}
