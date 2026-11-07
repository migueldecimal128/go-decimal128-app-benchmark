// SPDX-License-Identifier: MIT
//go:build bid754

// Library bid754: github.com/sky1core/bid754/bid754-go, a pure-Go mechanical
// port of the Intel Decimal Floating-Point Math Library (libbid) v20U4.
// Built with -tags bid754.
//
// What it lacks, and so what this file cannot provide (see unsupported):
// powers, logarithms and compound interest (the C library's transcendental
// functions need a binary128 floating-point library the port leaves out),
// and quantize in any direction but ties-to-even (Mul, Div and Quantize
// always round ties-to-even). Those apps are skipped for this library.
//
// Two properties differ from the contract (see deviations), neither of which
// the supported apps can reach: Minimum/Maximum follow the IEEE 754-2008
// minNum/maxNum rule (a NaN operand gives the other operand), and the Go zero
// value of Decimal128 is 0E-6176, not +0E+0; euro and tax always start from
// an explicit zero.

package dec

import (
	"encoding/binary"
	"strconv"

	lib "github.com/sky1core/bid754/bid754-go"
)

// Library identity, recorded with the results.
const (
	Name     = "bid754"
	Module   = "github.com/sky1core/bid754/bid754-go"
	BuildTag = "bid754"
)

// unsupported lists the operations this library does not have. Calling one
// panics; the runner does not run the apps that need them with this library.
var unsupported = map[string]string{
	"Decimal128.Pow":      "no general power",
	"Decimal128.PowInt":   "no integer power",
	"Decimal128.Compound": "no compound (no power)",
	"Decimal128.Log10":    "no logarithm",
	"Rounding.Quantize":   "quantize rounds ties-to-even only",
}

// deviations lists contract properties this library does not meet.
var deviations = map[string]string{
	"MinMaxNaN": "MinNum/MaxNum (IEEE 754-2008): a NaN operand gives the other operand",
	"ZeroValue": "the Go zero value is 0E-6176, not +0E+0",
}

// notInlined lists wrappers over Go's inlining budget, with their cost (see
// inline_test.go). Measured when the library was added.
var notInlined = map[string]string{
	// Library method already at the budget (its own IsZero costs 80).
	// +0.8 ns per call (6.0 vs 5.2 ns, Apple M3 Pro).
	"Decimal128.IsZero": "library method at the budget: +0.8 ns",
	// Called through function values by tax, the only app using them, so no
	// library's Minimum/Maximum is inlined there.
	"Minimum": "cost 81; called through a function value anyway",
	"Maximum": "cost 81; called through a function value anyway",
	// The re-layout of Intel's string form, part of the library's time:
	// about 42 ns per call against 23 ns for Intel's form alone.
	"Decimal128.String": "re-layout from Intel's form: about +20 ns",
}

// Decimal128 is a decimal128 value: the library's 16-byte little-endian BID
// encoding, as a distinct type (conversions are free).
type Decimal128 lib.Decimal128BID

// Rounding is an IEEE 754 rounding direction.
type Rounding lib.RoundingMode

const (
	TiesToEven     = Rounding(lib.RoundNearestEven)
	TiesToAway     = Rounding(lib.RoundNearestAway)
	TowardZero     = Rounding(lib.RoundTowardZero)
	TowardPositive = Rounding(lib.RoundTowardPositive)
	TowardNegative = Rounding(lib.RoundTowardNegative)
)

func notSupported(op string) string { return "bid754 does not support " + op + ": " + unsupported[op] }

// --- conversions ---------------------------------------------------------------

// FromBID128 builds the value from its BID encoding: the library's in-memory
// form, so this is a byte copy.
func FromBID128(hi, lo uint64) Decimal128 {
	var x Decimal128
	binary.LittleEndian.PutUint64(x[:8], lo)
	binary.LittleEndian.PutUint64(x[8:], hi)
	return x
}

// FromInt64 returns n exactly.
func FromInt64(n int64) Decimal128 { return Decimal128(lib.NewDecimal128FromInt64(n)) }

// MustParse reads a decimal string; it panics on bad input.
func MustParse(s string) Decimal128 {
	x, err := lib.NewDecimal128(s)
	if err != nil {
		panic(err)
	}
	return Decimal128(x)
}

// Zero returns +0E+0.
func Zero() Decimal128 { return Decimal128(lib.Zero128BID) }

// String is the to-scientific-string form. The library prints Intel's form
// (e.g. "+150E-2"); this re-lays it out (to "1.50"), as the C libbid row of
// decimal128-app-benchmark does with bid_to_gda_string. The re-layout is part
// of this library's measured time.
func (x Decimal128) String() string { return gdaString(lib.Decimal128BID(x).String()) }

// gdaString converts Intel's "<sign><coefficient>E<exponent>" (or +Inf, +NaN,
// +SNaN, with an optional NaN payload) to the General Decimal Arithmetic
// to-scientific-string: plain notation when the exponent is <= 0 and the
// adjusted exponent is >= -6, scientific otherwise.
func gdaString(s string) string {
	neg := s[0] == '-'
	body := s[1:]
	sign := ""
	if neg {
		sign = "-"
	}
	switch {
	case body == "Inf":
		return sign + "Infinity"
	case len(body) >= 4 && body[:4] == "SNaN":
		return sign + "sNaN" + body[4:]
	case len(body) >= 3 && body[:3] == "NaN":
		return sign + "NaN" + body[3:]
	}
	e := 0
	for e < len(body) && body[e] != 'E' {
		e++
	}
	digits := body[:e]
	exp, _ := strconv.Atoi(body[e+1:])
	n := len(digits)
	if (exp == 0) && !neg {
		return digits // already the plain form, no allocation
	}
	adjusted := exp + n - 1
	// At most 34 digits, a sign, "0." and 6 zeros, or "E+6144": under 64
	// bytes, so the buffer stays on the stack and the result is the only
	// allocation besides the library's own string.
	var buf [64]byte
	b := buf[:0]
	if neg {
		b = append(b, '-')
	}
	switch {
	case exp == 0:
		b = append(b, digits...)
	case (exp < 0) && (adjusted >= -6):
		if point := n + exp; point > 0 {
			b = append(b, digits[:point]...)
			b = append(b, '.')
			b = append(b, digits[point:]...)
		} else {
			b = append(b, '0', '.')
			for i := 0; i < -point; i++ {
				b = append(b, '0')
			}
			b = append(b, digits...)
		}
	default:
		b = append(b, digits[0])
		if n > 1 {
			b = append(b, '.')
			b = append(b, digits[1:]...)
		}
		b = append(b, 'E')
		if adjusted >= 0 {
			b = append(b, '+')
		}
		b = strconv.AppendInt(b, int64(adjusted), 10)
	}
	return string(b)
}

// --- arithmetic (ties-to-even) -------------------------------------------------

func (x Decimal128) Add(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128BID(x).Add(lib.Decimal128BID(y)))
}

func (x Decimal128) Sub(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128BID(x).Sub(lib.Decimal128BID(y)))
}

func (x Decimal128) Mul(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128BID(x).Mul(lib.Decimal128BID(y)))
}

func (x Decimal128) Quo(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128BID(x).Div(lib.Decimal128BID(y)))
}

// Quantize rounds x (ties-to-even) to the exponent of q.
func (x Decimal128) Quantize(q Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128BID(x).Quantize(lib.Decimal128BID(q)))
}

// Neg flips the sign (also of zero).
func (x Decimal128) Neg() Decimal128 { return Decimal128(lib.Decimal128BID(x).Negate()) }

// --- comparisons and classification -----------------------------------------------

// The comparisons, Minimum, Maximum and ToInt64 discard the library's flags
// result. They use a named result: the same code, written `r, _ := ...;
// return r`, costs 81 against Go's inlining budget of 80; this form costs 78.

func (x Decimal128) Less(y Decimal128) (r bool) {
	r, _ = lib.Decimal128BID(x).QuietLess(lib.Decimal128BID(y))
	return
}

func (x Decimal128) Equal(y Decimal128) (r bool) {
	r, _ = lib.Decimal128BID(x).QuietEqual(lib.Decimal128BID(y))
	return
}

func (x Decimal128) Signbit() bool { return lib.Decimal128BID(x).IsSignMinus() }
func (x Decimal128) IsZero() bool  { return lib.Decimal128BID(x).IsZero() }
func (x Decimal128) IsNaN() bool   { return lib.Decimal128BID(x).IsNaN() }
func (x Decimal128) IsInf() bool   { return lib.Decimal128BID(x).IsInf() }

// Minimum is IEEE 754-2008 minNum (see deviations).
func Minimum(x, y Decimal128) Decimal128 {
	r, _ := lib.Decimal128BID(x).MinNum(lib.Decimal128BID(y))
	return Decimal128(r)
}

// Maximum is IEEE 754-2008 maxNum (see deviations).
func Maximum(x, y Decimal128) Decimal128 {
	r, _ := lib.Decimal128BID(x).MaxNum(lib.Decimal128BID(y))
	return Decimal128(r)
}

// --- rounding direction ------------------------------------------------------------

// roundTable maps a direction to the library's method for it: bid754 has one
// method per direction rather than a direction argument. A table keeps the
// wrapper inlinable (cost 69; a switch over the five methods costs 397), at
// the price of an indirect call: TowardZero.RoundToIntegral costs about 34 ns
// against 26 ns calling RoundIntegralZero directly (Apple M3 Pro).
var roundTable = [...]func(lib.Decimal128BID) (lib.Decimal128BID, lib.ExceptionFlags){
	lib.RoundNearestEven:    lib.Decimal128BID.RoundIntegralNearestEven,
	lib.RoundNearestAway:    lib.Decimal128BID.RoundIntegralNearestAway,
	lib.RoundTowardZero:     lib.Decimal128BID.RoundIntegralZero,
	lib.RoundTowardPositive: lib.Decimal128BID.RoundIntegralPositive,
	lib.RoundTowardNegative: lib.Decimal128BID.RoundIntegralNegative,
}

// RoundToIntegral rounds x to an integer in direction r.
func (r Rounding) RoundToIntegral(x Decimal128) Decimal128 {
	v, _ := roundTable[r](lib.Decimal128BID(x))
	return Decimal128(v)
}

// ToInt64 converts x to int64, discarding any fraction in direction r.
func (r Rounding) ToInt64(x Decimal128) (n int64) {
	n, _ = lib.Decimal128BID(x).ConvertToInt64(lib.RoundingMode(r))
	return
}

// Quantize rounds x in direction r to the exponent of q. The library
// quantizes ties-to-even only.
func (r Rounding) Quantize(x, q Decimal128) Decimal128 {
	if r != TiesToEven {
		panic(notSupported("Rounding.Quantize"))
	}
	return x.Quantize(q)
}

// --- powers and logarithms: not in this library -------------------------------------

func (x Decimal128) PowInt(n int) Decimal128     { panic(notSupported("Decimal128.PowInt")) }
func (x Decimal128) Pow(y Decimal128) Decimal128 { panic(notSupported("Decimal128.Pow")) }
func (x Decimal128) Compound(n int) Decimal128   { panic(notSupported("Decimal128.Compound")) }
func (x Decimal128) Log10() Decimal128           { panic(notSupported("Decimal128.Log10")) }
