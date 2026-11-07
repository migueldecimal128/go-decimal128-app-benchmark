// SPDX-License-Identifier: MIT

// Command telco is Mike Cowlishaw's telco benchmark
// (https://speleotrove.com/decimal/telcoSpec.html): a telephone-company
// billing run. For each call duration n, in seconds:
//
//	rate  = 0.0013 if n is even (local call), 0.00894 if odd (distance call)
//	price = rate * n, quantized to 0.01 ties-to-even
//	btax  = price * 0.0675, truncated to 0.01           (every call)
//	dtax  = price * 0.0341, truncated to 0.01           (distance calls)
//	total = price + btax (+ dtax)
//
// It writes one line per call ("<total>\r\n") to stderr and the three running
// totals to stdout. The input is the binary form of the published data: one
// big-endian uint64 per call; a trailing partial record is ignored.
//
// Unlike the UW-Madison apps, telco's timed loop includes decoding each record
// and formatting each output line, because that is how Cowlishaw defines the
// workload. With D128_COMPUTE_ITERS=N, a timed pass repeats the whole run over
// the preloaded input, formatting into memory, N times after the gated pass.
//
// Based on the telco program in decimal128-go (benchmark/telco, MIT), changed
// to call the library through internal/decimal. Cowlishaw's benchmark is freely
// distributable; this program is not derived from the UW-Madison suite.
package main

import (
	"encoding/binary"
	"fmt"
	"os"

	d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/decimal"
	harness "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/harness"
)

var (
	baseRate = d.MustParse("0.0013")
	distRate = d.MustParse("0.00894")
	baseTax  = d.MustParse("0.0675")
	distTax  = d.MustParse("0.0341")
	cent     = d.MustParse("0.01")
)

type totals struct{ t, b, d d.Decimal128 }

// bill processes every record of in, appending one line per call to out.
func bill(in, out []byte) ([]byte, totals) {
	sumT, sumB, sumD := d.Zero(), d.Zero(), d.Zero()
	for off := 0; off+8 <= len(in); off += 8 {
		n := binary.BigEndian.Uint64(in[off:])
		distance := (n & 1) != 0
		rate := baseRate
		if distance {
			rate = distRate
		}
		price := rate.Mul(d.FromInt64(int64(n))).Quantize(cent)
		btax := d.TowardZero.Quantize(price.Mul(baseTax), cent)
		sumB = sumB.Add(btax)
		total := price.Add(btax)
		if distance {
			dtax := d.TowardZero.Quantize(price.Mul(distTax), cent)
			sumD = sumD.Add(dtax)
			total = total.Add(dtax)
		}
		sumT = sumT.Add(total)
		out = append(out, total.String()...)
		out = append(out, '\r', '\n')
	}
	return out, totals{sumT, sumB, sumD}
}

func main() {
	harness.AnnounceLibrary(d.Name)
	if len(os.Args) < 2 {
		fmt.Fprintln(os.Stderr, "usage: telco <input>")
		os.Exit(2)
	}
	in, err := os.ReadFile(os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stdout, "ERROR: cannot read %s: %v\n", os.Args[1], err)
		os.Exit(1)
	}
	records := len(in) / 8

	// The gated pass: the transcript goes to stderr, the totals to stdout.
	out, tot := bill(in, make([]byte, 0, 8*records))
	os.Stderr.Write(out)
	fmt.Printf("sumT=%s\nsumB=%s\nsumD=%s\n", tot.t, tot.b, tot.d)

	if iters := harness.ComputeIters(); iters > 0 {
		batch := harness.ComputeBatch()
		best := harness.BestOf(iters, batch, func() { out, tot = bill(in, out[:0]) })
		harness.Sink = tot
		harness.ReportCompute(best, records, batch)
	}
}
