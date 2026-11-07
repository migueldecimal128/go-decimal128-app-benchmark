// SPDX-License-Identifier: MIT

package decimal

import "testing"

// The contract (doc.go) as tests. They use only this package's API, so they
// apply unchanged to every library's adapter: go test -tags <lib> ./internal/decimal.
// Each case is a property the benchmark goldens depend on; a library that
// fails one would fail a gate.

func p(s string) Decimal128 { return MustParse(s) }

// skipUnsupported skips a check of an operation the library declares it lacks.
func skipUnsupported(t *testing.T, op string) bool {
	t.Helper()
	if why, ok := unsupported[op]; ok {
		t.Logf("%s: not supported by %s (%s)", op, Name, why)
		return true
	}
	return false
}

// deviates reports a contract property the library declares it does not meet.
func deviates(t *testing.T, prop string) bool {
	t.Helper()
	if why, ok := deviations[prop]; ok {
		t.Logf("%s: %s deviates: %s", prop, Name, why)
		return true
	}
	return false
}

func TestString(t *testing.T) {
	// The to-scientific-string form, round-tripped through MustParse.
	for _, s := range []string{
		"0", "-0", "1", "-5", "1.50", "0.000", "1000", "1E+3", "1.2E+3",
		"0.0000012", // adjusted exponent -6: still plain
		"1.2E-7",    // adjusted exponent -7: scientific
		"1.23E-10", "9.999999999999999999999999999999999E+6144", "1E-6176",
		"Infinity", "-Infinity", "NaN",
	} {
		if got := p(s).String(); got != s {
			t.Errorf("MustParse(%q).String() = %q", s, got)
		}
	}
	for _, c := range [][2]string{
		{"0.00000012", "1.2E-7"}, {"123E-10", "1.23E-8"}, {"1e3", "1E+3"},
		{"+7", "7"}, {".5", "0.5"},
	} {
		if got := p(c[0]).String(); got != c[1] {
			t.Errorf("MustParse(%q).String() = %q, want %q", c[0], got, c[1])
		}
	}
}

func TestConversions(t *testing.T) {
	if got := Zero().String(); got != "0" {
		t.Errorf("Zero() = %s", got)
	}
	for _, c := range []struct {
		n    int64
		want string
	}{{0, "0"}, {-5, "-5"}, {643, "643"}, {-9223372036854775808, "-9223372036854775808"}} {
		if got := FromInt64(c.n).String(); got != c.want {
			t.Errorf("FromInt64(%d) = %s", c.n, got)
		}
	}
	// BID128: 1 is coefficient 1 with exponent 0 (biased 6176 = 0x3040 << 49 ...).
	for _, c := range []struct {
		hi, lo uint64
		want   string
	}{
		{0x3040000000000000, 1, "1"},
		{0xB040000000000000, 150, "-150"},
		{0x303C000000000000, 150, "1.50"},
		{0x3040000000000000, 0, "0"},
		{0x7800000000000000, 0, "Infinity"},
		{0x7C00000000000000, 0, "NaN"},
	} {
		if got := FromBID128(c.hi, c.lo).String(); got != c.want {
			t.Errorf("FromBID128(%#x, %d) = %s, want %s", c.hi, c.lo, got, c.want)
		}
	}
}

func TestArithmetic(t *testing.T) {
	for _, c := range []struct {
		op         string
		x, y, want string
	}{
		// Exponents are kept: the ideal exponent of a sum is the smaller one,
		// of a product the sum of the two.
		{"add", "1.50", "1", "2.50"},
		{"add", "1", "-1", "0"},
		{"add", "1.0", "-1.0", "0.0"},
		{"sub", "2.50", "0.5", "2.00"},
		{"mul", "1.50", "2", "3.00"},
		{"mul", "0.05", "0.05", "0.0025"},
		{"quo", "1", "8", "0.125"},
		{"quo", "1.00", "2", "0.50"},
		{"quo", "100", "4", "25"},
		// 34 digits, ties-to-even.
		{"quo", "2", "3", "0.6666666666666666666666666666666667"},
		{"quo", "1", "3", "0.3333333333333333333333333333333333"},
		{"add", "1E+34", "0.5", "1.000000000000000000000000000000000E+34"},
		{"add", "1E+34", "1.5", "1.000000000000000000000000000000000E+34"},
		{"add", "1E+34", "2.5", "1.000000000000000000000000000000000E+34"},
		{"add", "1E+34", "3.5", "1.000000000000000000000000000000000E+34"},
		{"quantize", "1.235", "0.01", "1.24"},
		{"quantize", "1.245", "0.01", "1.24"},
		{"quantize", "-1.235", "0.01", "-1.24"},
		{"quantize", "2", "0.01", "2.00"},
		{"quantize", "123.4567", "0.0001", "123.4567"},
	} {
		x, y := p(c.x), p(c.y)
		var got Decimal128
		switch c.op {
		case "add":
			got = x.Add(y)
		case "sub":
			got = x.Sub(y)
		case "mul":
			got = x.Mul(y)
		case "quo":
			got = x.Quo(y)
		case "quantize":
			got = x.Quantize(y)
		}
		if got.String() != c.want {
			t.Errorf("%s(%s, %s) = %s, want %s", c.op, c.x, c.y, got, c.want)
		}
	}
	for _, c := range []struct {
		r          Rounding
		x, q, want string
	}{
		{TowardZero, "1.239", "0.01", "1.23"},
		{TowardZero, "-1.239", "0.01", "-1.23"},
		{TowardZero, "0.0675", "0.01", "0.06"},
		{TiesToEven, "1.235", "0.01", "1.24"},
		{TowardPositive, "1.231", "0.01", "1.24"},
		{TowardNegative, "1.239", "0.01", "1.23"},
		{TiesToAway, "1.245", "0.01", "1.25"},
	} {
		if (c.r != TiesToEven) && skipUnsupported(t, "Rounding.Quantize") {
			continue
		}
		if got := c.r.Quantize(p(c.x), p(c.q)).String(); got != c.want {
			t.Errorf("Quantize(%s, %s) in direction %v = %s, want %s", c.x, c.q, c.r, got, c.want)
		}
	}
	if got := p("0").Neg().String(); got != "-0" {
		t.Errorf("Neg(0) = %s, want -0", got)
	}
	if got := p("-1.50").Neg().String(); got != "1.50" {
		t.Errorf("Neg(-1.50) = %s", got)
	}
}

func TestComparisons(t *testing.T) {
	nan, negz, z := p("NaN"), p("-0"), p("0")
	if !p("1.5").Less(p("1.50001")) || p("1.50").Less(p("1.5")) || !p("-2").Less(p("1")) {
		t.Error("Less: wrong order")
	}
	if negz.Less(z) || z.Less(negz) || !negz.Equal(z) {
		t.Error("-0 and +0 must be equal")
	}
	if !p("1.50").Equal(p("1.5")) {
		t.Error("Equal compares numerically: 1.50 == 1.5")
	}
	if nan.Less(z) || z.Less(nan) || nan.Equal(nan) {
		t.Error("NaN must compare false")
	}
	if !negz.Signbit() || z.Signbit() || !p("-3").Signbit() {
		t.Error("Signbit is the sign bit (true for -0)")
	}
	if !negz.IsZero() || !p("0.000").IsZero() || p("1E-6176").IsZero() {
		t.Error("IsZero")
	}
	if !nan.IsNaN() || p("Infinity").IsNaN() || !p("-Infinity").IsInf() || p("1").IsInf() {
		t.Error("IsNaN / IsInf")
	}
	if got := Minimum(p("2"), p("-3")).String(); got != "-3" {
		t.Errorf("Minimum(2, -3) = %s", got)
	}
	if got := Maximum(p("2"), p("-3")).String(); got != "2" {
		t.Errorf("Maximum(2, -3) = %s", got)
	}
	if (!Minimum(nan, z).IsNaN() || !Maximum(z, nan).IsNaN()) && !deviates(t, "MinMaxNaN") {
		t.Error("Minimum / Maximum with a NaN operand must be NaN")
	}
}

// TestZeroValue: the Go zero value of Decimal128 should be +0E+0, so code that
// relies on it (banking's model structs) behaves as with an explicit zero.
func TestZeroValue(t *testing.T) {
	var z Decimal128
	if got := z.String(); got != "0" && !deviates(t, "ZeroValue") {
		t.Errorf("zero value = %s, want 0", got)
	}
}

func TestRounding(t *testing.T) {
	type row struct {
		x                                string
		even, away, zero, toward, downto string
	}
	for _, c := range []row{
		{"2.5", "2", "3", "2", "3", "2"},
		{"3.5", "4", "4", "3", "4", "3"},
		{"-2.5", "-2", "-3", "-2", "-2", "-3"},
		{"2.4", "2", "2", "2", "3", "2"},
		{"-0.4", "-0", "-0", "-0", "-0", "-1"},
		{"7", "7", "7", "7", "7", "7"},
		{"1.2E+3", "1.2E+3", "1.2E+3", "1.2E+3", "1.2E+3", "1.2E+3"},
	} {
		x := p(c.x)
		for _, m := range []struct {
			r    Rounding
			name string
			want string
		}{
			{TiesToEven, "TiesToEven", c.even}, {TiesToAway, "TiesToAway", c.away},
			{TowardZero, "TowardZero", c.zero}, {TowardPositive, "TowardPositive", c.toward},
			{TowardNegative, "TowardNegative", c.downto},
		} {
			if got := m.r.RoundToIntegral(x).String(); got != m.want {
				t.Errorf("%s.RoundToIntegral(%s) = %s, want %s", m.name, c.x, got, m.want)
			}
		}
	}
	for _, c := range []struct {
		x    string
		want int64
	}{{"2.7", 2}, {"-2.7", -2}, {"643", 643}, {"1.2E+3", 1200}, {"-0.5", 0}} {
		if got := TowardZero.ToInt64(p(c.x)); got != c.want {
			t.Errorf("TowardZero.ToInt64(%s) = %d, want %d", c.x, got, c.want)
		}
	}
}

func TestPowers(t *testing.T) {
	for _, c := range []struct {
		op, name string
		f        func() Decimal128
		want     string
	}{
		{"Decimal128.PowInt", "PowInt(2, 10)", func() Decimal128 { return p("2").PowInt(10) }, "1024"},
		{"Decimal128.PowInt", "PowInt(1.05, 2)", func() Decimal128 { return p("1.05").PowInt(2) }, "1.1025"},
		{"Decimal128.PowInt", "PowInt(2, -2)", func() Decimal128 { return p("2").PowInt(-2) }, "0.25"},
		{"Decimal128.Pow", "Pow(2, 0.5)", func() Decimal128 { return p("2").Pow(p("0.5")) }, "1.414213562373095048801688724209698"},
		{"Decimal128.Compound", "Compound(0.05, 2)", func() Decimal128 { return p("0.05").Compound(2) }, "1.1025"},
		{"Decimal128.Compound", "Compound(0.5, -1)", func() Decimal128 { return p("0.5").Compound(-1) }, "0.6666666666666666666666666666666667"},
		{"Decimal128.Log10", "Log10(1000)", func() Decimal128 { return p("1000").Log10() }, "3"},
		{"Decimal128.Log10", "Log10(2)", func() Decimal128 { return p("2").Log10() }, "0.3010299956639811952137388947244930"},
	} {
		if skipUnsupported(t, c.op) {
			continue
		}
		if got := c.f().String(); got != c.want {
			t.Errorf("%s = %s, want %s", c.name, got, c.want)
		}
	}
}
