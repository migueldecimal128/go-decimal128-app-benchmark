// SPDX-License-Identifier: GPL-3.0-or-later
//
// Package tax: tax_preparation application benchmark — shared Go scaffolding.
//
// Faithful transliteration of the UW-Madison GPL C tax solvers, via the proven
// Swift harness (apps/tax_preparation/swift). Derivative work → GPL; this repo
// only. Each tax form is one program (main → macro_assign → per-return
// initialization/action/ending). The gated output is the DEBUG
// "BINARY OUTPUT: <value>" line each showline/myfwrite emits to stderr (value =
// Decimal128.String = FORMAT_AUTO). Input is the native BID stream of 48-byte
// records {char line_name[...]; BID_UINT128 amt (16 LE bytes at offset 32, after
// alignment padding)} — value-identical to the DPD variant (40-byte records).
//
// external-gate suite: the runner diffs stderr against
// apps/tax_preparation/golden/Tax.<SUB>.stderr.golden. With D128_COMPUTE_ITERS=N
// a silent pass replays the decoded rows N times (arithmetic + formatting, no
// transcript write) and prints COMPUTE_NS_PER_RECORD.
package tax

import (
	"bytes"
	"fmt"
	"os"
	"strconv"

	d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/decimal"
	harness "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/harness"
)

// Tax filing-status codes (taxsolve_*.h).
const (
	single                = 1
	marriedFilingJointly  = 2
	marriedFilingSeparate = 3
	headOfHousehold       = 4
	widow                 = 5
)

var zero = d.MustParse("0")

func zeros(n int) []d.Decimal128 {
	s := make([]d.Decimal128, n)
	for i := range s {
		s[i] = zero
	}
	return s
}

func isNegative(x d.Decimal128) bool { return x.Less(zero) }

// record is one decoded input row (line_name, amt).
type record struct {
	name string
	amt  d.Decimal128
}

// input is a sequential reader over the binary named-line input
// (read_from_file()), with capture (pass 1) / replay (pass 2) of DECODED rows so
// the timed pass serves input from memory — no re-decode. Mirrors the C
// g_tax_read_mode.
type input struct {
	data      []byte
	pos       int
	capturing bool
	replaying bool
	cap       []record
	capIdx    int
}

func openInput(path string) *input {
	b, err := os.ReadFile(path)
	if err != nil {
		fmt.Fprintf(os.Stderr, "tax: cannot read '%s'\n", path)
		os.Exit(1)
	}
	return &input{data: b}
}

func (in *input) beginReplay() { in.replaying, in.capturing, in.capIdx = true, false, 0 }
func (in *input) rewind()      { in.capIdx = 0 }

// read returns the next record; ok=false at EOF.
func (in *input) read() (r record, ok bool) {
	if in.replaying {
		if in.capIdx >= len(in.cap) {
			return record{}, false
		}
		r = in.cap[in.capIdx]
		in.capIdx++
		return r, true
	}
	if in.pos+48 > len(in.data) {
		return record{}, false
	}
	n := 0
	for (n < 32) && (in.data[in.pos+n] != 0) {
		n++
	}
	hi, lo := harness.BID128(in.data, in.pos+32)
	r = record{string(in.data[in.pos : in.pos+n]), d.FromBID128(hi, lo)}
	in.pos += 48
	if in.capturing {
		in.cap = append(in.cap, r)
	}
	return r, true
}

// amt reads one record and returns its amount, or zero at EOF
// (the Swift `input.read()?.amt ?? zero`).
func (in *input) amt() d.Decimal128 {
	if r, ok := in.read(); ok {
		return r.amt
	}
	return zero
}

// out accumulates the stderr transcript and flushes once. `enabled` gates the
// WRITE, not the format: callers format the value (x.String()) at the call site,
// so in the silent compute pass the value is still formatted and only the append
// is skipped (the C's "format but not write", myfwrite g_tax_write). `sink`
// consumes the formatted length so the format cannot be dropped.
type out struct {
	buf     bytes.Buffer
	enabled bool
	sink    uint64
}

func newOut() *out { return &out{enabled: true} }

func (o *out) line(s string) {
	if o.enabled {
		o.buf.WriteString(s)
		o.buf.WriteByte('\n')
	} else {
		o.sink += uint64(len(s))
	}
}

// binary is showline / myfwrite: one "BINARY OUTPUT: <value-string>".
func (o *out) binary(s string) {
	if o.enabled {
		o.buf.WriteString("BINARY OUTPUT: ")
		o.buf.WriteString(s)
		o.buf.WriteByte('\n')
	} else {
		o.sink += uint64(len(s))
	}
}

func (o *out) flush() { os.Stderr.Write(o.buf.Bytes()) }

// parseArgs checks `<input> <outname> <loop>` and returns (input path, loop).
func parseArgs(prog string) (string, int, bool) {
	harness.AnnounceLibrary(d.Name)
	if len(os.Args) == 4 {
		if loop, err := strconv.Atoi(os.Args[3]); err == nil {
			return os.Args[1], loop, true
		}
	}
	fmt.Fprintf(os.Stderr, "%s: usage: <input> <outname> <loop>\n", prog)
	return "", 0, false
}

// drive runs the two passes shared by every form. Pass 1 is the gated
// transcript (correctness), doubling as warm-up and capturing decoded rows when
// a compute pass is requested. Pass 2 (D128_COMPUTE_ITERS=N) replays the rows
// and re-runs the identical per-return work with the transcript write off;
// `reset` restores any state the form carries across returns before each pass.
func drive(in *input, o *out, loop int, processReturn func(int), reset func()) {
	citers := harness.ComputeIters()
	in.capturing = citers > 0
	for i := 0; i < loop; i++ {
		processReturn(i)
	}
	in.capturing = false
	o.flush()

	if (citers > 0) && (len(in.cap) > 0) {
		o.enabled = false
		in.beginReplay()
		// Each timed sample is `batch` passes (D128_COMPUTE_BATCH) so it lasts long
		// enough to time; each pass rewinds the replay and resets the form's state
		// (cheap next to a return, so it sits inside the clock).
		batch := harness.ComputeBatch()
		best := int64(-1)
		for k := 0; k < citers; k++ {
			dt := harness.BestOf(1, batch, func() {
				in.rewind()
				if reset != nil {
					reset()
				}
				for i := 0; i < loop; i++ {
					processReturn(i)
				}
			})
			if (best < 0) || (dt < best) {
				best = dt
			}
		}
		o.enabled = true
		fmt.Fprintf(os.Stderr, "compute-sink %d\n", o.sink) // observable -> format can't be dropped
		harness.ReportCompute(best, loop, batch)            // records == loop (== 1)
	}
}
