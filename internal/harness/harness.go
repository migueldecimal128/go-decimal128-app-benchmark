// SPDX-License-Identifier: MIT
//
// Package harness holds the small helpers shared by every app: env/arg
// handling, splitting a BID record into its 64-bit halves, the best-of-N
// compute clock, and the COMPUTE_* markers the runner parses. It contains no
// decimal arithmetic; the apps reach a decimal library only through
// internal/dec.
package harness

import (
	"encoding/binary"
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"
)

// ComputeIters returns D128_COMPUTE_ITERS (the runner's compute-mode switch),
// or 0 when unset/invalid — 0 means "whole mode: run the gated pass only".
func ComputeIters() int {
	s := os.Getenv("D128_COMPUTE_ITERS")
	if s == "" {
		return 0
	}
	n, err := strconv.Atoi(s)
	if (err != nil) || (n < 0) {
		return 0
	}
	return n
}

// ComputeBatch returns D128_COMPUTE_BATCH: how many passes one timed sample
// covers, so a sample of a small workload lasts long enough to time (the runner
// picks it so each sample is >= ~100 us). Unset or invalid means 1.
func ComputeBatch() int {
	n, err := strconv.Atoi(os.Getenv("D128_COMPUTE_BATCH"))
	if (err != nil) || (n < 1) {
		return 1
	}
	return n
}

// LE32 reads a little-endian int32 at b[off:].
func LE32(b []byte, off int) int32 { return int32(binary.LittleEndian.Uint32(b[off:])) }

// BID128 splits a 16-byte little-endian BID decimal128 at b[off:] into its
// (hi, lo) 64-bit halves, the argument order decimal128.FromBID128 takes.
func BID128(b []byte, off int) (hi, lo uint64) {
	lo = binary.LittleEndian.Uint64(b[off:])
	hi = binary.LittleEndian.Uint64(b[off+8:])
	return hi, lo
}

// samples holds every timed sample (ns, each covering `batch` passes), so the
// runner can report median and spread beside the minimum (Step 5).
var samples []int64

// AddSample records one timed sample taken outside BestOf.
func AddSample(dt int64) { samples = append(samples, dt) }

// BestOf times `iters` samples of `batch` back-to-back passes each, records
// every sample, and returns the fastest in nanoseconds (divide by batch for
// one pass).
func BestOf(iters, batch int, pass func()) int64 {
	best := int64(-1)
	for i := 0; i < iters; i++ {
		t0 := time.Now()
		for j := 0; j < batch; j++ {
			pass()
		}
		dt := time.Since(t0).Nanoseconds()
		samples = append(samples, dt)
		if (best < 0) || (dt < best) {
			best = dt
		}
	}
	return best
}

// ReportCompute prints the markers the runner parses from stdout: the batch
// size actually used, every recorded sample, and the best sample per record
// (sample / (batch x records)).
func ReportCompute(bestNs int64, records, batch int) {
	fmt.Printf("COMPUTE_BATCH=%d\n", batch)
	var sb strings.Builder
	for i, v := range samples {
		if i > 0 {
			sb.WriteByte(',')
		}
		sb.WriteString(strconv.FormatInt(v, 10))
	}
	fmt.Printf("COMPUTE_SAMPLES_NS=%s\n", sb.String())
	fmt.Printf("COMPUTE_NS_PER_RECORD=%v\n", float64(bestNs)/(float64(records)*float64(batch)))
}

// Sink is written by compute passes so their results are observably used and
// the compiler cannot drop the work (fairness rule 4). Assign it once, after
// BestOf returns, from a variable the pass wrote — never inside the timed pass
// (boxing into `any` allocates).
var Sink any
