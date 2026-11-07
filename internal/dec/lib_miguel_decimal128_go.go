// SPDX-License-Identifier: MIT
//go:build miguel_decimal128_go

// Library miguel-decimal128-go: github.com/migueldecimal128/decimal128-go.
// Built with -tags miguel_decimal128_go.

package dec

import lib "github.com/migueldecimal128/decimal128-go"

// Library identity, recorded with the results.
const (
	Name     = "miguel-decimal128-go"
	Module   = "github.com/migueldecimal128/decimal128-go"
	BuildTag = "miguel_decimal128_go" // the go build tag that selects this file
)

// Decimal128 is a decimal128 value. It is a distinct type over the library's
// value type, so conversions between them cost nothing and only the methods
// declared here are available to the apps.
type Decimal128 lib.Decimal128

// Rounding is an IEEE 754 rounding direction.
type Rounding lib.Rounding

const (
	TiesToEven     = Rounding(lib.TiesToEven)
	TiesToAway     = Rounding(lib.TiesToAway)
	TowardZero     = Rounding(lib.TowardZero)
	TowardPositive = Rounding(lib.TowardPositive)
	TowardNegative = Rounding(lib.TowardNegative)
)

// --- conversions ---------------------------------------------------------------

// FromBID128 decodes the BID interchange encoding (high, low 64-bit halves).
func FromBID128(hi, lo uint64) Decimal128 { return Decimal128(lib.FromBID128(hi, lo)) }

// FromInt64 returns n exactly.
func FromInt64(n int64) Decimal128 { return Decimal128(lib.FromInt64(n)) }

// MustParse reads a decimal string; it panics on bad input.
func MustParse(s string) Decimal128 { return Decimal128(lib.MustParse(s)) }

// Zero returns +0E+0.
func Zero() Decimal128 { return Decimal128(lib.Zero()) }

// String is the to-scientific-string form.
func (x Decimal128) String() string { return lib.Decimal128(x).String() }

// --- arithmetic (ties-to-even) -------------------------------------------------

func (x Decimal128) Add(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128(x).Add(lib.Decimal128(y)))
}

func (x Decimal128) Sub(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128(x).Sub(lib.Decimal128(y)))
}

func (x Decimal128) Mul(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128(x).Mul(lib.Decimal128(y)))
}

func (x Decimal128) Quo(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128(x).Quo(lib.Decimal128(y)))
}

// Quantize rounds x (ties-to-even) to the exponent of q.
func (x Decimal128) Quantize(q Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128(x).Quantize(lib.Decimal128(q)))
}

// Neg flips the sign (also of zero).
func (x Decimal128) Neg() Decimal128 { return Decimal128(lib.Decimal128(x).Neg()) }

// --- comparisons and classification -----------------------------------------------

func (x Decimal128) Less(y Decimal128) bool  { return lib.Decimal128(x).Less(lib.Decimal128(y)) }
func (x Decimal128) Equal(y Decimal128) bool { return lib.Decimal128(x).Equal(lib.Decimal128(y)) }
func (x Decimal128) Signbit() bool           { return lib.Decimal128(x).Signbit() }
func (x Decimal128) IsZero() bool            { return lib.Decimal128(x).IsZero() }
func (x Decimal128) IsNaN() bool             { return lib.Decimal128(x).IsNaN() }
func (x Decimal128) IsInf() bool             { return lib.Decimal128(x).IsInf() }

// Minimum is the IEEE 754-2019 minimum (NaN if either operand is NaN).
func Minimum(x, y Decimal128) Decimal128 {
	return Decimal128(lib.Minimum(lib.Decimal128(x), lib.Decimal128(y)))
}

// Maximum is the IEEE 754-2019 maximum (NaN if either operand is NaN).
func Maximum(x, y Decimal128) Decimal128 {
	return Decimal128(lib.Maximum(lib.Decimal128(x), lib.Decimal128(y)))
}

// --- rounding direction ------------------------------------------------------------

// RoundToIntegral rounds x to an integer in direction r.
func (r Rounding) RoundToIntegral(x Decimal128) Decimal128 {
	return Decimal128(lib.Rounding(r).RoundToIntegral(lib.Decimal128(x)))
}

// ToInt64 converts x to int64, discarding any fraction in direction r.
func (r Rounding) ToInt64(x Decimal128) int64 { return lib.Rounding(r).ToInt64(lib.Decimal128(x)) }

// --- powers and logarithms (correctly rounded, ties-to-even) ----------------------

// PowInt is x^n for an integer n.
func (x Decimal128) PowInt(n int) Decimal128 { return Decimal128(lib.Decimal128(x).PowInt(n)) }

// Pow is the general power x^y.
func (x Decimal128) Pow(y Decimal128) Decimal128 {
	return Decimal128(lib.Decimal128(x).Pow(lib.Decimal128(y)))
}

// Compound is (1+x)^n.
func (x Decimal128) Compound(n int) Decimal128 { return Decimal128(lib.Decimal128(x).Compound(n)) }

// Log10 is the base-10 logarithm.
func (x Decimal128) Log10() Decimal128 { return Decimal128(lib.Decimal128(x).Log10()) }

// Quantize rounds x in direction r to the exponent of q.
func (r Rounding) Quantize(x, q Decimal128) Decimal128 {
	return Decimal128(lib.Rounding(r).Quantize(lib.Decimal128(x), lib.Decimal128(q)))
}
