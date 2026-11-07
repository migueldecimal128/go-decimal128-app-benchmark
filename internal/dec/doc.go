// SPDX-License-Identifier: MIT

// Package dec is the only way the benchmark apps reach a decimal library: the
// Go counterpart of the C suite's operator.h. Every app imports it as
//
//	import d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/dec"
//
// and uses only the names below. The library behind them is chosen when the
// program is compiled, by a build tag; there is no interface, no generic type
// parameter and no function variable on the measured path, so a call costs
// exactly what the library's own call costs, provided the compiler inlines the
// wrapper. TestWrappersInline checks that it does.
//
// # Libraries
//
// Each library has one file, lib_<name>.go, selected by its build tag. There
// is no default: a build without a library tag compiles nolib.go, which fails
// with a message saying so. README.md lists the libraries, how each is
// identified and run, and the steps to add one: a new lib_<name>.go starting
// with //go:build <tag>, and !<tag> added to nolib.go's build line.
// go test -tags <tag> ./internal/dec then
// checks the contract (contract_test.go) and that every wrapper inlines
// (inline_test.go). A library that does not pass the contract tests will not
// pass the gates either; the tests point at the operation that differs.
//
// # Contract
//
// Decimal128 is an IEEE 754-2019 decimal128 value: 34 significant digits,
// exponent range [-6176, 6111], signed zeros, infinities and NaNs. Every
// operation below must give the IEEE 754 result, including the exponent (the
// "quantum": 1.50 and 1.5 are equal but print differently, and the goldens
// keep the difference). Unless a Rounding is named, results are rounded
// ties-to-even.
//
//   - Arithmetic: Add, Sub, Mul, Quo, Neg. Neg flips the sign, also of zero.
//   - Quantize(q): x rounded (ties-to-even) to the exponent of q;
//     r.Quantize(x, q) rounds in direction r instead.
//   - Comparisons: Less and Equal compare numerically; any NaN gives false;
//     -0 equals +0. Signbit reports the sign bit (true for -0). IsZero, IsNaN,
//     IsInf classify.
//   - Minimum, Maximum: IEEE 754-2019 minimum / maximum (a NaN operand gives NaN).
//   - Rounding r: r.RoundToIntegral(x) rounds x to an integer in direction r
//     (the exponent becomes max(exponent, 0)); r.ToInt64(x) converts to int64,
//     discarding any fraction in direction r.
//   - Powers: x.PowInt(n) is x^n for an integer n, x.Pow(y) is the general
//     power, x.Compound(n) is (1+x)^n; x.Log10() is the base-10 logarithm. All
//     correctly rounded (ties-to-even).
//   - Conversions: FromBID128(hi, lo) decodes the 128-bit BID interchange
//     encoding given as its high and low 64-bit halves; FromInt64 is exact;
//     MustParse reads a decimal string (panics on bad input); Zero is +0E+0.
//   - String is the to-scientific-string of the General Decimal Arithmetic
//     specification (as decNumber prints): plain notation when the exponent
//     is <= 0 and the adjusted exponent is >= -6, otherwise scientific
//     notation such as 1.2E-7 or 1E+3.
//
// A library that needs extra work to meet the contract (for example, rewriting
// its string form) does that work inside its wrapper; it is then part of that
// library's measured time and should be noted with its results.
//
// # Cost
//
// Every library is called through exactly one wrapper layer, so comparisons
// between libraries are like for like. Measured for miguel-decimal128-go against the
// same apps calling the library directly (alternating runs, same machine):
//
//   - The machine code makes the same calls, to the same library functions,
//     except for two wrappers over Go's inlining budget of 80, which are real
//     calls (see notInlined in inline_test.go): FromBID128 (81), used only to
//     decode input outside the timed loops, and Rounding.Quantize (82), which
//     telco calls 1-2 times per record: +0.9% on telco's fastest sample,
//     +1.2% on its median.
//   - Each inlined wrapper leaves one position marker at its call site (a NOP
//     the compiler keeps for stack traces): tax fed gains about 400.
//   - Compute time: within noise for euro, risk, banking and tax ca / nc; tax
//     fed, the most call-dense program, is about 2.4% slower on its fastest
//     sample and 0.3% on its median.
//
// Two wrapper layers do not fit the budget. Keep app code calling these names
// directly rather than through helpers of its own (banking's helpers were
// removed for this reason).
package dec
