// SPDX-License-Identifier: MIT
//go:build go_builtin_decimal64 && go1.28

// Library go-builtin-decimal64: Go's predeclared decimal64 type, from the
// ubd128 toolchain (GO_DECIMAL_GOROOT), where decimal64 is stored as
// XOR-BID64 and decimal128 as UBD. It is lib_go_builtin_decimal128.go with
// decimal64 underneath, so that the apps, which name the type Decimal128,
// run unchanged at the other width: 16 significant digits and exponents
// [-398, 369]. It does not meet the decimal128 contract (doc.go); the apps
// it runs are checked against the UW-Madison suite's 64-bit goldens.
//
// The apps read 64-bit inputs through FromBID64 when Width is 64.

package decimal

import (
	"math/bits"
	"math/decimal"
	"strconv"
)

// Library identity, recorded with the results.
const (
	Name     = "go-builtin-decimal64"
	Module   = "std (GOEXPERIMENT=decimal, decimal64 XOR-BID64)"
	BuildTag = "go_builtin_decimal64"
)

// Width is the type's width in bits: the apps choose their 64-bit inputs by it.
const Width = 64

// unsupported lists the contract operations this library lacks (none).
var unsupported = map[string]string{}

// deviations lists contract properties this library does not meet.
var deviations = map[string]string{
	"precision": "decimal64: 16 digits and exponents [-398, 369], not decimal128's 34 and [-6176, 6111]",
}

// notInlined lists the wrappers over Go's inlining budget, each with where it
// runs and what that costs (see inline_test.go). Any other wrapper must
// inline.
var notInlined = map[string]string{
	// The error check around strconv.ParseDecimal puts it over the budget:
	// one extra call per parse. Apps parse their constants at setup; banking
	// also parses two constants in its penalty computation.
	"MustParse": "one extra call per parse",
	// strconv's 'f' (exponent -6 to 0) or a layout built here from its 'b'
	// form: the wrapper is the function, one call, as for a library's own
	// String, and the layout is part of this library's measured time (tax
	// prints every line with it).
	"Decimal128.String": "one call: strconv 'f', or a layout built here",
	// Input decoding only. On the UBD toolchains the BID conversion
	// inlines into math/decimal.BID128frombits, which then exceeds the
	// budget here: one call, as many as before. On XOR-BID it inlines.
	"FromBID128": "input decoding only; UBD's BID conversion inlines into it",
}

// Decimal128 is the predeclared decimal64 as a distinct type, under the name
// the apps use: the operators apply to it as to decimal64, and conversions
// cost nothing.
type Decimal128 decimal64

// Rounding is an IEEE 754 rounding direction.
type Rounding decimal.RoundingMode

const (
	TiesToEven     = Rounding(decimal.ToNearestEven)
	TiesToAway     = Rounding(decimal.ToNearestAway)
	TowardZero     = Rounding(decimal.ToZero)
	TowardPositive = Rounding(decimal.ToPositiveInf)
	TowardNegative = Rounding(decimal.ToNegativeInf)
)

// --- conversions ---------------------------------------------------------------

// FromBID128 decodes the BID decimal128 interchange encoding (high, low
// 64-bit halves) and rounds it to decimal64.
func FromBID128(hi, lo uint64) Decimal128 {
	return Decimal128(decimal64(decimal.BID128frombits(hi, lo)))
}

// FromBID64 decodes the BID decimal64 interchange encoding.
func FromBID64(b uint64) Decimal128 { return Decimal128(decimal.BID64frombits(b)) }

// FromInt64 returns n exactly.
func FromInt64(n int64) Decimal128 { return Decimal128(n) }

// MustParse reads a decimal string; it panics on bad input.
func MustParse(s string) Decimal128 {
	d, err := strconv.ParseDecimal(s, 64)
	if err != nil {
		panic(err)
	}
	return Decimal128(decimal64(d))
}

// Zero returns +0E+0.
func Zero() Decimal128 { return 0 }

// String is the to-scientific-string of the General Decimal Arithmetic
// specification. strconv has no such format, but where the
// to-scientific-string is plain notation (the exponent at most 0 and the
// adjusted exponent, exp + digits - 1, at least -6) it is exactly strconv's
// 'f' with precision -1, every digit of the coefficient: one call and one
// allocation. That covers every money value and the apps' full-precision
// intermediates. Any other value is laid out here from the coefficient and
// exponent of strconv's 'b' format ("-150e-2" for -1.50). Either way the
// work is part of this library's measured time.
func (x Decimal128) String() string {
	d := decimal64(x)
	if _, hi, lo, exp, ok := decimal.Decompose(d); ok && exp <= 0 && plain(hi, lo, -exp-6) {
		return strconv.FormatDecimal(decimal128(d), 'f', -1, 64)
	}
	return stringGeneral(d)
}

// pow10 holds 10^0 to 10^19.
var pow10 = [20]uint64{1, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10,
	1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19}

// plain reports whether the coefficient hi*2^64 + lo is at least 10^k, which
// for k = -exp - 6 is the adjusted exponent being at least -6. A coefficient
// is below 10^16.
func plain(hi, lo uint64, k int) bool {
	switch {
	case k <= 0:
		return true
	case k <= 19:
		return hi != 0 || lo >= pow10[k]
	case k <= 33:
		ph, pl := bits.Mul64(pow10[19], pow10[k-19])
		return hi > ph || hi == ph && lo >= pl
	}
	return false
}

// stringGeneral is String for a special value or an exponent outside
// [-6, 0].
func stringGeneral(d decimal64) string {
	if decimal.IsNaN(d) {
		if decimal.Signbit(d) {
			return "-NaN"
		}
		return "NaN"
	}
	if decimal.IsInf(d, 0) {
		if decimal.Signbit(d) {
			return "-Infinity"
		}
		return "Infinity"
	}
	var buf [48]byte
	b := strconv.AppendDecimal(buf[:0], decimal128(d), 'b', -1, 64)
	neg := b[0] == '-'
	if neg {
		b = b[1:]
	}
	e := 0
	for e < len(b) && b[e] != 'e' {
		e++
	}
	digits := b[:e]
	exp, err := strconv.Atoi(string(b[e+1:]))
	if err != nil {
		panic("decimal: unexpected 'b' format " + string(b))
	}
	n := len(digits)
	adjusted := exp + n - 1
	out := make([]byte, 0, n+16)
	if neg {
		out = append(out, '-')
	}
	switch {
	case exp == 0:
		out = append(out, digits...)
	case exp < 0 && adjusted >= -6:
		if point := n + exp; point > 0 {
			out = append(out, digits[:point]...)
			out = append(out, '.')
			out = append(out, digits[point:]...)
		} else {
			out = append(out, '0', '.')
			for i := point; i < 0; i++ {
				out = append(out, '0')
			}
			out = append(out, digits...)
		}
	default:
		out = append(out, digits[0])
		if n > 1 {
			out = append(out, '.')
			out = append(out, digits[1:]...)
		}
		out = append(out, 'E')
		if adjusted >= 0 {
			out = append(out, '+')
		}
		out = strconv.AppendInt(out, int64(adjusted), 10)
	}
	return string(out)
}

// --- arithmetic (ties-to-even) -------------------------------------------------

func (x Decimal128) Add(y Decimal128) Decimal128 { return x + y }
func (x Decimal128) Sub(y Decimal128) Decimal128 { return x - y }
func (x Decimal128) Mul(y Decimal128) Decimal128 { return x * y }
func (x Decimal128) Quo(y Decimal128) Decimal128 { return x / y }

// Quantize rounds x (ties-to-even) to the exponent of q.
func (x Decimal128) Quantize(q Decimal128) Decimal128 { return decimal.Quantize(x, q) }

// Neg flips the sign (also of zero).
func (x Decimal128) Neg() Decimal128 { return -x }

// --- comparisons and classification -----------------------------------------------

func (x Decimal128) Less(y Decimal128) bool  { return x < y }
func (x Decimal128) Equal(y Decimal128) bool { return x == y }
func (x Decimal128) Signbit() bool           { return decimal.Signbit(x) }
func (x Decimal128) IsZero() bool            { return x == 0 }
func (x Decimal128) IsNaN() bool             { return decimal.IsNaN(x) }
func (x Decimal128) IsInf() bool             { return decimal.IsInf(x, 0) }

// Minimum is the IEEE 754-2019 minimum (NaN if either operand is NaN).
func Minimum(x, y Decimal128) Decimal128 { return min(x, y) }

// Maximum is the IEEE 754-2019 maximum (NaN if either operand is NaN).
func Maximum(x, y Decimal128) Decimal128 { return max(x, y) }

// --- rounding direction ------------------------------------------------------------

// RoundToIntegral rounds x to an integer in direction r.
func (r Rounding) RoundToIntegral(x Decimal128) Decimal128 {
	switch r {
	case TowardZero:
		return decimal.Trunc(x)
	case TowardNegative:
		return decimal.Floor(x)
	case TowardPositive:
		return decimal.Ceil(x)
	case TiesToAway:
		return decimal.Round(x)
	}
	return decimal.RoundToEven(x)
}

// ToInt64 converts x to int64, discarding any fraction in direction r.
func (r Rounding) ToInt64(x Decimal128) int64 { return int64(r.RoundToIntegral(x)) }

// --- powers and logarithms (correctly rounded, ties-to-even) ----------------------

// PowInt is x^n for an integer n.
func (x Decimal128) PowInt(n int) Decimal128 { return decimal.Pown(x, n) }

// Pow is the general power x^y.
func (x Decimal128) Pow(y Decimal128) Decimal128 { return decimal.Pow(x, y) }

// Compound is (1+x)^n.
func (x Decimal128) Compound(n int) Decimal128 { return decimal.Compound(x, n) }

// Log10 is the base-10 logarithm.
func (x Decimal128) Log10() Decimal128 { return decimal.Log10(x) }

// Quantize rounds x in direction r to the exponent of q.
func (r Rounding) Quantize(x, q Decimal128) Decimal128 {
	return decimal.QuantizeMode(x, q, decimal.RoundingMode(r))
}
