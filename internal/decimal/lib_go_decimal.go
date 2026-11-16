// SPDX-License-Identifier: MIT
//go:build go1.28 && go_decimal

// Library go-decimal: the built-in decimal128 type proposed in
// https://go.dev/issue/19787, as implemented by the fork at
// https://github.com/marcelocantos/go/tree/decimal. Built with -tags
// go_decimal, the fork's go command and GOEXPERIMENT=decimal.
//
// Arithmetic and comparison are the type's operators, with no second decimal
// engine and no dynamic dispatch. Quantize, rounding to an integer, the
// powers and the logarithm are math/decimal's functions, and parsing and
// formatting are strconv's. String re-lays strconv's text out as the
// to-scientific-string; that work is part of this library's measured time.
//
// The file's go1.28 constraint is the language version that has the decimal
// types; the module's own version is unchanged.

package decimal

import (
	"bytes"
	"math/decimal"
	"strconv"
)

const (
	Name     = "go-decimal"
	Module   = "builtin"
	BuildTag = "go_decimal"

	decimal128Digits = 34
)

var unsupported = map[string]string{}

var deviations = map[string]string{}

var notInlined = map[string]string{
	"MustParse":         "parsing and its error check exceed the budget; used for constants and input",
	"Decimal128.String": "strconv's formatting and the re-layout to the to-scientific-string",
}

// A defined type adds the benchmark's methods; arithmetic uses the built-in
// type's operators directly.
type Decimal128 decimal128

type Rounding decimal.RoundingMode

const (
	TiesToEven     = Rounding(decimal.ToNearestEven)
	TiesToAway     = Rounding(decimal.ToNearestAway)
	TowardZero     = Rounding(decimal.ToZero)
	TowardPositive = Rounding(decimal.ToPositiveInf)
	TowardNegative = Rounding(decimal.ToNegativeInf)
)

// --- conversions ---

func FromBID128(hi, lo uint64) Decimal128 { return Decimal128(decimal.BID128frombits(hi, lo)) }

func FromInt64(n int64) Decimal128 { return Decimal128(n) }

func Zero() Decimal128 { return 0 }

func MustParse(s string) Decimal128 {
	x, err := strconv.ParseDecimal(s, 128)
	if err != nil {
		panic(err)
	}
	return Decimal128(x)
}

// String is the to-scientific-string. strconv's shortest 'G' form keeps
// every coefficient digit and differs from it in three ways, which are
// rewritten here: it spells the infinities "+Inf" and "-Inf", it pads the
// exponent to two digits, and it uses an exponent for an adjusted exponent
// below -4 or at least 6, where the to-scientific-string uses plain
// notation for every value whose quantum is at most units and whose
// adjusted exponent is at least -6.
func (x Decimal128) String() string {
	// The canonical form fits a sign, "0.", five leading zeros and every
	// coefficient digit. Adapt it in that buffer, then allocate one string.
	var buf [len("-0.00000") + decimal128Digits]byte
	s := strconv.AppendDecimal(buf[:0], decimal128(x), 'G', -1, 128)
	if len(s) == len("+Inf") && s[1] == 'I' {
		if s[0] == '-' {
			return "-Infinity"
		}
		return "Infinity"
	}
	i := bytes.IndexByte(s, 'E')
	if i < 0 {
		return string(s)
	}
	j := i + 2 // the exponent's digits, after its sign
	for j < len(s)-1 && s[j] == '0' {
		j++
	}
	exp := 0
	for _, c := range s[j:] {
		exp = exp*10 + int(c-'0')
	}
	start := 0
	if s[0] == '-' {
		start = 1
	}
	if s[i+1] == '+' {
		digits := i - start
		if s[start+1] == '.' {
			digits--
		}
		// The quantum's exponent is exp-(digits-1). When it is not
		// positive, the to-scientific-string is plain: move the point.
		if exp < digits {
			copy(s[start+1:], s[start+2:i]) // remove the old point
			end := i - 1
			point := start + exp + 1
			if point < end {
				copy(s[point+1:], s[point:end])
				s[point] = '.'
				end++
			}
			return string(s[:end])
		}
	} else if exp <= 6 {
		// An adjusted exponent of -5 or -6: "0.", zeros, and the digits.
		var o [len("-0.00000") + decimal128Digits]byte
		n := 0
		if start == 1 {
			o[0] = '-'
			n = 1
		}
		o[n], o[n+1] = '0', '.'
		n += 2
		for k := 1; k < exp; k++ {
			o[n] = '0'
			n++
		}
		for _, c := range s[start:i] {
			if c != '.' {
				o[n] = c
				n++
			}
		}
		return string(o[:n])
	}
	n := copy(s[i+2:], s[j:]) // drop the exponent's padding
	return string(s[:i+2+n])
}

// --- arithmetic (ties-to-even) ---

func (x Decimal128) Add(y Decimal128) Decimal128      { return x + y }
func (x Decimal128) Sub(y Decimal128) Decimal128      { return x - y }
func (x Decimal128) Mul(y Decimal128) Decimal128      { return x * y }
func (x Decimal128) Quo(y Decimal128) Decimal128      { return x / y }
func (x Decimal128) Neg() Decimal128                  { return -x }
func (x Decimal128) Quantize(q Decimal128) Decimal128 { return decimal.Quantize(x, q) }

// --- comparisons and classification ---

func (x Decimal128) Less(y Decimal128) bool  { return x < y }
func (x Decimal128) Equal(y Decimal128) bool { return x == y }
func (x Decimal128) Signbit() bool           { return decimal.Signbit(x) }
func (x Decimal128) IsZero() bool            { return x == 0 }
func (x Decimal128) IsNaN() bool             { return decimal.IsNaN(x) }
func (x Decimal128) IsInf() bool             { return decimal.IsInf(x, 0) }

func Minimum(x, y Decimal128) Decimal128 { return min(x, y) }
func Maximum(x, y Decimal128) Decimal128 { return max(x, y) }

// --- rounding direction ---

func (r Rounding) RoundToIntegral(x Decimal128) Decimal128 {
	switch r {
	case TiesToEven:
		return decimal.RoundToEven(x)
	case TiesToAway:
		return decimal.Round(x)
	case TowardZero:
		return decimal.Trunc(x)
	case TowardPositive:
		return decimal.Ceil(x)
	case TowardNegative:
		return decimal.Floor(x)
	default:
		panic("invalid rounding direction")
	}
}

func (r Rounding) ToInt64(x Decimal128) int64 { return int64(r.RoundToIntegral(x)) }

func (r Rounding) Quantize(x, q Decimal128) Decimal128 {
	return decimal.QuantizeMode(x, q, decimal.RoundingMode(r))
}

// --- powers and logarithms: math/decimal's, correctly rounded ---

func (x Decimal128) PowInt(n int) Decimal128 { return Decimal128(decimal.Pown(decimal128(x), n)) }
func (x Decimal128) Pow(y Decimal128) Decimal128 {
	return Decimal128(decimal.Pow(decimal128(x), decimal128(y)))
}
func (x Decimal128) Compound(n int) Decimal128 { return Decimal128(decimal.Compound(decimal128(x), n)) }
func (x Decimal128) Log10() Decimal128         { return Decimal128(decimal.Log10(decimal128(x))) }
