// SPDX-License-Identifier: GPL-3.0-or-later
//
// risk_management application benchmark — Go harness on the d128 backend
// (decimal128-go).
//
// Faithful transliteration of the UW-Madison GPL C program (apps/risk_management/
// c/src {main.c, fcns.c, fcns.h}), via the proven Swift harness
// (apps/risk_management/swift). Derivative work → GPL; this repo only.
//
// Computes per-stock CAPM statistics (covariance, beta, alpha, expected return,
// prediction, …) against the S&P 500, writing one labelled log per stock. Values
// format via Decimal128.String (= C TOSTRING64). All arithmetic is ties-to-even
// (the risk context is never re-rounded). RESCALE(x, e) is quantize(x, 10^e)
// (e = -2 or -4), reproduced as quantize to a "0.01"/"0.0001" quantum.
//
// file-gate suite: run with cwd = the risk data dir; the runner diffs each
// testGen/result/<stock>.log_<loop>_<suffix> against apps/risk_management/golden.
// With D128_COMPUTE_ITERS=N a silent pass replays preloaded raw data N times
// (arithmetic + formatting, no file reads or writes) and prints
// COMPUTE_NS_PER_RECORD.
package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"

	d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/decimal"
	harness "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/harness"
)

const (
	term = 60
	year = 5
)

// Stocks in company_output[] order (== the runner's RISK_STOCKS + goldens).
var stocks = []string{"aapl", "ace", "adbe", "bac", "ba", "borl", "dell", "etn",
	"fdx", "ge", "ibm", "jnj", "jpm", "ko", "kss", "mmm", "mrk",
	"msft", "nipny", "odp"}

var (
	one         = d.MustParse("1")
	twelve      = d.MustParse("12")
	hundred     = d.MustParse("100")
	q2          = d.MustParse("0.01")   // RESCALE MTWO  (exponent -2)
	q4          = d.MustParse("0.0001") // RESCALE MFOUR (exponent -4)
	zero        = d.MustParse("0")
	riskPremium = d.MustParse("0.55")
	termDec     = d.FromInt64(term)
	yearDec     = d.FromInt64(year)
)

// power replicates the risk operator.h POWER macro: an integral exponent goes
// to pown (matches decNumberPower's integer path), otherwise the general pow.
func power(x, y d.Decimal128) d.Decimal128 {
	if y.Equal(d.TowardZero.RoundToIntegral(y)) {
		return x.PowInt(int(d.TowardZero.ToInt64(y)))
	}
	return x.Pow(y)
}

func readFile(path string) []byte {
	b, err := os.ReadFile(path)
	if err != nil {
		fmt.Fprintf(os.Stderr, "risk: cannot read '%s'\n", path)
		os.Exit(1)
	}
	return b
}

func decode(b []byte, off int) d.Decimal128 {
	hi, lo := harness.BID128(b, off)
	return d.FromBID128(hi, lo)
}

// readTBill: 60 rates -> (current = last/100, avg = rescale(-2)(mean)/100).
func readTBill(path string) (current, avg d.Decimal128) {
	b := readFile(path)
	accu := zero
	var last d.Decimal128
	for i := 0; i < term; i++ {
		r := decode(b, i*16)
		accu = accu.Add(r)
		last = r
	}
	current = last.Quo(hundred)
	avg = accu.Quo(termDec)
	avg = avg.Quantize(q2)
	avg = avg.Quo(hundred)
	return current, avg
}

type rawStock struct{ index, split, dividend [term]d.Decimal128 }

// readStock: 60 records of (index, volume, split, dividend); volume unused.
func readStock(path string) *rawStock {
	b := readFile(path)
	r := new(rawStock)
	p := 0
	for i := 0; i < term; i++ {
		r.index[i] = decode(b, p)
		p += 32 // index, then skip volume
		r.split[i] = decode(b, p)
		p += 16
		r.dividend[i] = decode(b, p)
		p += 16
	}
	return r
}

type company struct {
	avgReturn       d.Decimal128
	returns         [term]d.Decimal128
	monthlyVariance d.Decimal128
	currentIndex    d.Decimal128
	annualDps       d.Decimal128
}

// computeCompany is the compute half of process_company (+ variance): monthly
// returns, avg return, unnormalised variance (the C computes n-1 but never
// divides — reproduced), annual DPS. Split from the file read so the timed pass
// runs it on preloaded raw data.
func computeCompany(r *rawStock, c *company) {
	var dps [12]d.Decimal128
	for i := range dps {
		dps[i] = zero
	}
	c.avgReturn, c.annualDps = zero, zero
	c.returns[0] = zero
	for i := 1; i < term; i++ {
		t1 := r.index[i].Mul(r.split[i])
		t2 := r.dividend[i].Mul(r.split[i])
		t1 = t1.Sub(r.index[i-1])
		t1 = t1.Add(t2)
		c.returns[i] = t1.Quo(r.index[i-1]).Quantize(q4)
		c.avgReturn = c.avgReturn.Add(c.returns[i])
		dps[i%12] = r.dividend[i]
	}
	c.currentIndex = r.index[term-1]
	c.avgReturn = c.avgReturn.Quo(termDec.Sub(one)) // / (n-1)
	c.avgReturn = c.avgReturn.Quantize(q4)
	c.monthlyVariance = zero
	for i := 1; i < term; i++ {
		dev := c.returns[i].Sub(c.avgReturn)
		dev = dev.Mul(dev).Quantize(q4)
		c.monthlyVariance = c.monthlyVariance.Add(dev)
	}
	for i := 0; i < 12; i++ {
		c.annualDps = c.annualDps.Add(dps[i])
	}
	c.annualDps = c.annualDps.Quantize(q4)
}

func covariance(t, m *company) d.Decimal128 {
	cov := zero
	for i := 1; i < term; i++ {
		t1 := t.returns[i].Sub(t.avgReturn)
		t2 := m.returns[i].Sub(m.avgReturn)
		cov = cov.Add(t1.Mul(t2).Quantize(q4))
	}
	return cov
}

type setup struct {
	currentRiskfree, riskfreeAvg d.Decimal128
	market                       company
}

// generateOutput is generate_output + export_output's formatting: the per-stock
// CAPM figures as the labelled log text (percent fields ×100 for display only).
func generateOutput(s *setup, ticker *company, sb *strings.Builder) {
	m := &s.market
	covar := covariance(ticker, m)
	beta := covar.Quo(m.monthlyVariance)
	alpha := ticker.avgReturn.Sub(m.avgReturn.Mul(beta))
	// Rf_m = [(1+Rf_avg)^(1/12) - 1] * (1 - beta)
	rfm := one.Add(s.riskfreeAvg)
	rfm = power(rfm, one.Quo(twelve)) // non-integer exponent -> pow
	rfm = rfm.Sub(one)
	rfm = rfm.Mul(one.Sub(beta))
	intercept := alpha.Sub(rfm)
	systemVar := beta.Mul(beta).Mul(m.monthlyVariance)
	unsystemVar := ticker.monthlyVariance.Sub(m.monthlyVariance)
	rSquared := m.monthlyVariance.Quo(ticker.monthlyVariance)
	expectedRet := beta.Mul(riskPremium).Add(s.currentRiskfree)
	// prediction = price * (1 + (expected_ret - dps/price))^year
	pt := ticker.annualDps.Quo(ticker.currentIndex)
	pt = expectedRet.Sub(pt)
	pt = one.Add(pt)
	pt = power(pt, yearDec) // integer exponent -> pown
	prediction := ticker.currentIndex.Mul(pt)

	line := func(label string, v d.Decimal128, tail string) {
		sb.WriteString(label)
		sb.WriteString(v.String())
		sb.WriteString(tail)
	}
	line("covariance is ", covar, "\n")
	line("avg_return is ", ticker.avgReturn, "\n")
	line("avg_ret_M  is ", m.avgReturn, "\n")
	line("current_riskfree_rate  is ", s.currentRiskfree.Mul(hundred), " (percent) \n")
	line("riskfree rate avg  is ", s.riskfreeAvg.Mul(hundred), " (percent) \n")
	line("monthly_var market  is ", m.monthlyVariance, "\n")
	line("monthly_var_ticker  is ", ticker.monthlyVariance, "\n")
	line("risk premium  is ", riskPremium, "\n")
	line("current index  is ", ticker.currentIndex, "\n")
	line("expected return is ", expectedRet, "\n")
	line("annual DPS  is ", ticker.annualDps, "\n")
	line("beta  is ", beta, "\n")
	line("alpha  is ", alpha, "\n")
	line("Rf_m  is ", rfm, "\n")
	line("intercept  is ", intercept, "\n")
	line("system var is ", systemVar, "\n")
	line("unsystem var  is ", unsystemVar, "\n")
	line("R square  is ", rSquared, "\n")
	line("prediction  is ", prediction, "\n")
}

func main() { os.Exit(run()) }

func run() int {
	harness.AnnounceLibrary(d.Name)
	if len(os.Args) != 3 {
		os.Stderr.WriteString("[USAGE] cmd loop strname\n")
		return 1
	}
	userLoop, err := strconv.Atoi(os.Args[1])
	if err != nil {
		os.Stderr.WriteString("[USAGE] cmd loop strname\n")
		return 1
	}
	str := os.Args[2]

	os.Stderr.WriteString("The program starts\n")
	var s setup
	s.currentRiskfree, s.riskfreeAvg = readTBill("testGen/bin/3month_tbill.csv.bid128")
	computeCompany(readStock("testGen/bin/sp500.csv.bid128"), &s.market)

	var ticker company
	var sb strings.Builder
	for loop := 0; loop < userLoop; loop++ {
		fmt.Fprintf(os.Stderr, "in Loop %d\n", loop)
		for _, stock := range stocks {
			computeCompany(readStock("testGen/bin/"+stock+".csv.bid128"), &ticker)
			sb.Reset()
			generateOutput(&s, &ticker, &sb)
			outPath := fmt.Sprintf("testGen/result/%s.log_%d_%s", stock, loop, str)
			if err := os.WriteFile(outPath, []byte(sb.String()), 0o644); err != nil {
				fmt.Fprintf(os.Stderr, "risk: %v\n", err)
			}
		}
	}

	// Pass 2 (opt-in): silent, timed compute over preloaded raw data. Same
	// per-stock work as the loop above (computeCompany + generate_output +
	// format) but no file read and no write. Correctness is owned by pass 1's
	// gated output files.
	if citers := harness.ComputeIters(); citers > 0 {
		raws := make([]*rawStock, len(stocks))
		for i, stock := range stocks {
			raws[i] = readStock("testGen/bin/" + stock + ".csv.bid128") // preload once
		}
		sink := 0
		best := harness.BestOf(citers, 1, func() { // pass >= ~250 us: no batching
			for _, raw := range raws {
				computeCompany(raw, &ticker)
				sb.Reset()
				generateOutput(&s, &ticker, &sb)
				sink += sb.Len()
			}
		})
		fmt.Fprintf(os.Stderr, "compute-sink %d\n", sink) // observable -> work can't be dropped
		harness.ReportCompute(best, len(stocks), 1)
	}

	os.Stderr.WriteString("The program ends sucessfully\n")
	return 0
}
