// SPDX-License-Identifier: GPL-3.0-or-later
//
// CA 540 (2006) — faithful transliteration of apps/tax_preparation/c/src/
// taxsolve_CA_540_2006.c (macro_assign / initialization / action / ending +
// TaxRateFormula/Function), via the Swift CA.swift. GPL, this repo only. All
// arithmetic ties-to-even; the two to-integral calls (exemption worksheet, tax
// rounding) use ties-away (0.5 rounds up).
package tax

import (
	"fmt"

	d "github.com/migueldecimal128/decimal128-go"
)

func RunCA() int {
	path, loop, ok := parseArgs("tax_ca")
	if !ok {
		return 1
	}
	o := newOut()
	in := openInput(path)
	add := func(a, b d.Decimal128) d.Decimal128 { return a.Add(b) }
	p := d.MustParse

	// --- macro_assign(): constant table (built exactly as the C does) ----------
	P_P01 := p("0.01")
	P_P02 := add(P_P01, P_P01)
	P_P04 := add(P_P02, P_P02)
	P_P06 := add(P_P04, P_P02)
	P_P08 := add(P_P04, P_P04)
	P_P093 := p("0.093")
	P_1 := p("1.0")
	P_2 := add(P_1, P_1)
	P_6 := p("6.0")
	P_91 := p("91.0")
	P_285 := p("285.0")
	P_1250 := p("1250.0")
	P_2500 := add(P_1250, P_1250)
	P_3410 := p("3410.0")
	P_6622 := p("6622.0")
	P_6820 := add(P_3410, P_3410)
	P_13244 := p("13244.0")
	P_13251 := p("13251.0")
	P_15698 := p("15698.0")
	P_24776 := p("24776.0")
	P_31396 := p("31396.0")
	P_31397 := add(P_31396, P_1)
	P_34394 := p("34394.0")
	P_40473 := p("40473.0")
	P_43467 := p("43467.0")
	P_49552 := p("49552.0")
	P_50090 := p("50090.0")
	P_59166 := p("59166.0")
	P_68788 := p("68788.0")
	P_86934 := p("86934.0")
	P_150743 := p("150743.0")
	P_226119 := p("226119.0")
	P_301491 := p("301491.0")

	brkpt := [3][6]d.Decimal128{
		{zero, P_6622, P_15698, P_24776, P_34394, P_43467},  // single / MFS
		{zero, P_13244, P_31396, P_49552, P_68788, P_86934}, // MFJ / widow
		{zero, P_13251, P_31397, P_40473, P_50090, P_59166}, // head of household
	}
	txrt := [6]d.Decimal128{P_P01, P_P02, P_P04, P_P06, P_P08, P_P093}

	// CA progressive tax (TaxRateFormula) then ties-away round-to-integer
	// (TaxRateFunction).
	taxRateFunction := func(x d.Decimal128, status int) d.Decimal128 {
		srow := 0
		switch status {
		case marriedFilingJointly, widow:
			srow = 1
		case headOfHousehold:
			srow = 2
		}
		sum := zero
		bracket := 0
		for brkpt[srow][bracket+1].Less(x) {
			t := brkpt[srow][bracket+1].Sub(brkpt[srow][bracket])
			sum = sum.Add(t.Mul(txrt[bracket]))
			bracket++
			if bracket == 5 {
				break
			}
		}
		t := x.Sub(brkpt[srow][bracket]).Mul(txrt[bracket])
		return d.TiesToAway.RoundToIntegral(t.Add(sum))
	}

	o.line("=== CA Tax Preparation Program starts ===")

	// The per-return body, re-run unchanged by the timed pass with the write off.
	processReturn := func(index int) {
		o.line(fmt.Sprintf("Processing %d tax return", index+1))
		L := zeros(50)

		// --- initialization(): read status, dependents, L12..L43 ---------------
		r1, ok := in.read()
		if !ok {
			return
		}
		status := int(d.TowardZero.ToInt64(r1.amt))
		L[6] = in.amt()     // dependent?
		iline8 := in.amt()  // blind
		iline9 := in.amt()  // senior
		iline10 := in.amt() // # dependents
	loopL:
		for rr, ok := in.read(); ok; rr, ok = in.read() {
			var j int
			switch rr.name {
			case "L12", "L13", "L14", "L16", "L18", "L23", "L25", "L26", "L27",
				"L28", "L31", "L32", "L33", "L36", "L37", "L38", "L39":
				j = lineNo(rr.name)
			case "L43":
				L[43] = L[43].Add(rr.amt)
				break loopL
			default:
				break loopL
			}
			L[j] = L[j].Add(rr.amt)
		}

		// --- action(): compute the return -------------------------------------
		iline7 := P_2
		if (status == single) || (status == marriedFilingSeparate) || (status == headOfHousehold) {
			iline7 = P_1
		}
		if !L[6].IsZero() {
			iline7 = zero
		}
		L[7] = P_91.Mul(iline7)
		L[8] = iline8.Mul(P_91)
		L[9] = iline9.Mul(P_91)
		L[10] = iline10.Mul(P_285)
		temp1 := L[7].Add(L[8])
		L[11] = L[9].Add(L[10])
		L[11] = temp1.Add(L[11])
		L[15] = L[13].Sub(L[14])
		L[17] = L[15].Add(L[16])
		if (status == single) || (status == marriedFilingSeparate) {
			if L[18].Less(P_3410) {
				L[18] = P_3410
			}
		} else if L[18].Less(P_6820) {
			L[18] = P_6820
		}
		L[19] = L[17].Sub(L[18])
		if isNegative(L[19]) {
			L[19] = zero
		}
		L[20] = taxRateFunction(L[19], status)

		lt1 := P_150743.Less(L[13])
		lt2 := P_301491.Less(L[13])
		lt3 := P_226119.Less(L[13])
		// Exemption-credits worksheet variables (also shown in ending() under lt1).
		ws := zeros(14) // ws_a .. ws_n
		const (
			wa = iota
			wb
			wc
			wd
			we
			wf
			wg
			wh
			wi
			wj
			wk
			wl
			wm
			wn
		)
		if (((status == single) || (status == marriedFilingSeparate)) && lt1) ||
			(((status == marriedFilingJointly) || (status == widow)) && lt2) ||
			((status == headOfHousehold) && lt3) {
			ws[wa] = L[13]
			switch status {
			case single, marriedFilingSeparate:
				ws[wb] = P_150743
			case marriedFilingJointly, widow:
				ws[wb] = P_301491
			case headOfHousehold:
				ws[wb] = P_226119
			}
			ws[wc] = ws[wa].Sub(ws[wb])
			if status != marriedFilingSeparate {
				ws[wd] = ws[wc].Quo(P_2500)
			} else {
				ws[wd] = ws[wc].Quo(P_1250)
			}
			ws[wd] = d.TiesToAway.RoundToIntegral(ws[wd])
			ws[we] = P_6.Mul(ws[wd])
			temp1 = iline7.Add(iline8)
			ws[wf] = temp1.Add(iline9)
			ws[wg] = ws[we].Mul(ws[wf])
			temp1 = L[7].Add(L[8])
			ws[wh] = temp1.Add(L[9])
			ws[wi] = ws[wh].Sub(ws[wg])
			if isNegative(ws[wi]) {
				ws[wi] = zero
			}
			ws[wj] = iline10
			ws[wk] = ws[we].Mul(ws[wj])
			ws[wl] = L[10]
			ws[wm] = ws[wl].Sub(ws[wk])
			if isNegative(ws[wm]) {
				ws[wm] = zero
			}
			ws[wn] = ws[wi].Add(ws[wm])
			L[21] = ws[wn]
		} else {
			L[21] = L[11]
		}
		L[22] = L[20].Sub(L[21])
		if isNegative(L[22]) {
			L[22] = zero
		}
		L[24] = L[22].Add(L[23])
		temp1 = L[25].Add(L[26])
		temp2 := L[27].Add(L[28])
		L[29] = temp1.Add(temp2)
		temp1 = L[24].Sub(L[29])
		if isNegative(temp1) {
			L[30] = zero
		} else {
			L[30] = temp1
		}
		temp1 = L[30].Add(L[31])
		temp2 = L[32].Add(L[33])
		L[34] = temp1.Add(temp2)
		L[35] = L[34]
		temp1 = L[36].Add(L[37])
		temp2 = L[38].Add(L[39])
		temp1 = temp1.Add(L[43])
		L[44] = temp1.Add(temp2)
		if L[35].Less(L[44]) {
			L[45] = L[44].Sub(L[35])
		} else {
			L[48] = L[35].Sub(L[44])
		}

		// --- ending(): the gated showline sequence (each -> one BINARY OUTPUT) --
		for _, j := range []int{7, 8, 9, 10, 11} {
			o.binary(L[j].String())
		}
		if isNegative(L[15]) {
			o.binary(zero.Sub(L[15]).String()) // MINUS64(temp1, L15)
		} else {
			o.binary(L[15].String())
		}
		o.binary(L[18].String())
		o.binary(L[19].String())
		o.binary(L[20].String())
		// AGI-limitations worksheet dump (ending uses lt1 for all five statuses).
		if lt1 && (status >= single) && (status <= widow) {
			for _, v := range ws {
				o.binary(v.String())
			}
			o.binary(ws[wn].String())
			o.binary(L[11].String())
		}
		for _, j := range []int{21, 22, 23, 24, 29, 30, 34, 35, 44} {
			o.binary(L[j].String())
		}
		if L[35].Less(L[44]) {
			o.binary(L[45].String()) // (m) L45 = ...  REBATE!!!
		} else {
			o.binary(L[47].String()) // (m) L47 = ...  YOU OWE!!! (L47 unset == 0)
		}
	}

	drive(in, o, loop, processReturn, nil) // all state is per-return local: no reset
	return 0
}

// lineNo parses the number out of an "L<n>" line name.
func lineNo(name string) int {
	n := 0
	for i := 1; i < len(name); i++ {
		c := name[i]
		if (c < '0') || (c > '9') {
			break
		}
		n = n*10 + int(c-'0')
	}
	return n
}
