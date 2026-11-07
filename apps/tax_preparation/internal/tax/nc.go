// SPDX-License-Identifier: GPL-3.0-or-later
//
// NC D-400 (2006) — faithful transliteration of apps/tax_preparation/c/src/
// taxsolve_NC_D400_2006.c, via the Swift NC.swift. GPL, this repo only.
// Ties-to-even arithmetic; to-integral uses toward-zero (round income to the
// nearest $50) and ties-away (tax + USE tax + limited worksheet).
package tax

import (
	"fmt"

	d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/decimal"
)

func RunNC() int {
	path, loop, ok := parseArgs("tax_nc")
	if !ok {
		return 1
	}
	o := newOut()
	in := openInput(path)
	add := func(a, b d.Decimal128) d.Decimal128 { return a.Add(b) }
	p := d.MustParse

	// --- macro_assign() ----------------------------------------------------------
	P_P0007 := p("0.0007")
	P_P06 := p("0.06")
	P_P07 := p("0.07")
	P_P0775 := p("0.0775")
	P_P0825 := p("0.0825")
	P_P3333 := p("0.3333")
	P_P9 := p("0.9")
	P_1 := p("1.0")
	P_25 := p("25.0")
	P_50 := add(P_25, P_25)
	P_300 := p("300.0")
	P_600 := add(P_300, P_300)
	P_637P5 := p("637.5")
	P_765 := p("765.0")
	P_800 := p("800.0")
	P_1000 := p("1000.0")
	P_1020 := p("1020.0")
	P_1275 := p("1275.0")
	P_1300 := add(P_1000, P_300)
	P_3000 := p("3000.0")
	P_3393P75 := p("3393.75")
	P_3600 := add(P_3000, P_600)
	P_3750 := p("3750.0")
	P_4072P5 := p("4072.5")
	P_4200 := p("4200.0")
	P_4400 := p("4400.0")
	P_4500 := add(P_4200, P_300)
	P_4800 := add(P_4500, P_300)
	t5100 := add(P_4800, P_300)
	P_5150 := add(t5100, P_50)
	P_5400 := add(t5100, P_300)
	P_5430 := p("5430.0")
	P_5900 := add(t5100, P_800)
	P_6000 := add(P_5400, P_600)
	P_6600 := add(P_6000, P_600)
	P_6787P5 := p("6787.5")
	P_7200 := add(P_6600, P_600)
	P_7268P75 := p("7268.75")
	P_7800 := add(P_7200, P_600)
	P_8400 := add(P_7800, P_600)
	P_8722P5 := p("8722.5")
	P_10625 := P_637P5.Quo(P_P06)
	P_11630 := p("11630.0")
	P_12750 := P_765.Quo(P_P06)
	P_14537P5 := p("14537.5")
	P_17000 := P_1020.Quo(P_P06)
	t20000 := add(P_17000, P_3000)
	P_21250 := P_1275.Quo(P_P06)
	P_50000 := p("50000.0")
	P_60000 := p("60000.0")
	P_68000 := p("68000.0")
	P_80000 := add(t20000, P_60000)
	P_100000 := add(P_50000, P_50000)
	P_120000 := add(P_60000, P_60000)
	P_160000 := add(P_80000, P_80000)
	P_200000 := add(P_100000, P_100000)

	// NC bracket tax (TaxRateFunction), no post-rounding here.
	taxRate := func(x d.Decimal128, status int) d.Decimal128 {
		bracket := func(lo1, r0, b1, r1, add1, b2, r2, add2, b3, r3, add3 d.Decimal128) d.Decimal128 {
			_ = b3
			switch {
			case x.Less(lo1):
				return r0.Mul(x)
			case x.Less(b1):
				return add1.Add(r1.Mul(x.Sub(lo1)))
			case x.Less(b2):
				return add2.Add(r2.Mul(x.Sub(b1)))
			default:
				return add3.Add(r3.Mul(x.Sub(b2)))
			}
		}
		switch status {
		case 1:
			return bracket(P_12750, P_P06, P_60000, P_P07, P_765,
				P_120000, P_P0775, P_4072P5, P_120000, P_P0825, P_8722P5)
		case 2, 5:
			return bracket(P_21250, P_P06, P_100000, P_P07, P_1275,
				P_200000, P_P0775, P_6787P5, P_200000, P_P0825, P_14537P5)
		case 3:
			return bracket(P_10625, P_P06, P_50000, P_P07, P_637P5,
				P_100000, P_P0775, P_3393P75, P_100000, P_P0825, P_7268P75)
		case 4:
			return bracket(P_17000, P_P06, P_80000, P_P07, P_1020,
				P_160000, P_P0775, P_5430, P_160000, P_P0825, P_11630)
		default:
			return zero
		}
	}

	o.line("=== NC Tax Preparation Program starts ===")

	// The per-return body, re-run unchanged by the timed pass with the write off.
	processReturn := func(index int) {
		o.line(fmt.Sprintf("Processing %d tax return", index+1))
		L := zeros(60)
		AB, AGI, ws2, ws10 := zero, zero, zero, zero

		// --- initialization() -----------------------------------------------------
		r1, ok := in.read()
		if !ok {
			return
		}
		status := int(d.TowardZero.ToInt64(r1.amt))
		in.read() // Exception (assumed "no"/0)
		Exemptions := in.amt()
		Itemized := in.amt()
		Limited := in.amt()
		LimitedL6 := in.amt()
	loopL:
		for rr, ok := in.read(); ok; rr, ok = in.read() {
			switch rr.name {
			case "WS2":
				ws2 = ws2.Add(rr.amt)
			case "WS10":
				ws10 = ws10.Add(rr.amt)
			case "AGI":
				AGI = AGI.Add(rr.amt)
			case "L6", "L12", "L15", "L17", "L19", "L20", "L29", "L32", "L35", "L36",
				"L37", "L39", "L40", "L41", "L42", "L43", "L44", "L45":
				j := lineNo(rr.name)
				L[j] = L[j].Add(rr.amt)
			case "L46":
				L[46] = L[46].Add(rr.amt)
				break loopL
			default:
				break loopL
			}
		}

		// --- action() -------------------------------------------------------------
		if Exemptions.IsZero() || isNegative(Exemptions) {
			switch status {
			case 1, 3:
				L[30] = P_3000
			case 2, 5:
				L[30] = P_6000
			case 4:
				L[30] = P_4400
			}
		} else {
			exemp := int(d.TowardZero.ToInt64(Exemptions))
			switch status {
			case 1:
				switch exemp {
				case 1:
					L[30] = P_3750
				case 2:
					L[30] = P_4500
				}
			case 2, 5:
				switch exemp {
				case 1:
					L[30] = P_6600
				case 2:
					L[30] = P_7200
				case 3:
					L[30] = P_7800
				case 4:
					L[30] = P_8400
				}
			case 3:
				switch exemp {
				case 1:
					L[30] = P_3600
				case 2:
					L[30] = P_4200
				case 3:
					L[30] = P_4800
				case 4:
					L[30] = P_5400
				}
			case 4:
				switch exemp {
				case 1:
					L[30] = P_5150
				case 2:
					L[30] = P_5900
				}
			}
		}
		L[31] = L[29].Sub(L[30])
		if isNegative(L[31]) {
			L[31] = zero
		}
		if Itemized.IsZero() {
			L[32] = zero
		}
		if L[32].IsZero() || L[31].Less(L[32]) {
			L[33] = L[31]
		} else {
			L[33] = L[32]
		}

		switch status {
		case 1:
			AB = P_60000
		case 2, 5:
			AB = P_100000
		case 3:
			AB = P_50000
		case 4:
			AB = P_80000
		}
		if AGI.Less(AB) {
			L[34] = Exemptions.Mul(P_800)
		} else {
			L[34] = Exemptions.Mul(P_1300)
			if !Limited.IsZero() {
				ws2v := L[34]
				if LimitedL6.IsZero() {
					ws5 := ws10.Quo(ws2)
					L[34] = d.TiesToAway.RoundToIntegral(ws2v.Mul(ws5))
				} else {
					L[34] = d.TiesToAway.RoundToIntegral(ws2v.Mul(P_P3333))
				}
			}
		}
		temp1 := L[33].Add(L[34])
		temp2 := L[35].Add(L[36])
		temp1 = temp1.Add(L[37])
		L[38] = temp1.Add(temp2)
		L[7] = L[38]
		L[8] = L[6].Add(L[7])
		temp1 = L[39].Add(L[40])
		temp2 = L[41].Add(L[42])
		temp3 := L[43].Add(L[44])
		temp4 := L[45].Add(L[46])
		temp1 = temp1.Add(temp2)
		temp2 = temp3.Add(temp4)
		L[47] = temp1.Add(temp2)
		L[9] = L[45]
		L[10] = L[8].Sub(L[9])
		L[11] = L[10]
		L[13] = L[12].Mul(L[11])

		if !P_68000.Less(L[13]) { // L13 <= 68000
			m := L[13].Quo(P_50)
			m = d.TowardZero.RoundToIntegral(m)
			m = P_50.Mul(m)
			m = m.Add(P_25)
			L[14] = taxRate(m, status)
			L[14] = d.TiesToAway.RoundToIntegral(L[14])
		} else {
			L[14] = taxRate(L[13], status)
		}
		L[16] = L[14].Sub(L[15])
		if L[17].IsZero() {
			L[17] = d.TiesToAway.RoundToIntegral(L[13].Mul(P_P0007))
		}
		L[18] = L[16].Add(L[17])
		L[21] = L[19].Add(L[20])
		owe := L[21].Less(L[18])
		if owe {
			L[22] = L[18].Sub(L[21])
			_ = L[18].Mul(P_P9) // min_payment (penalty check, no output)
			L[23] = L[22]
		} else {
			L[24] = L[21].Sub(L[18])
			L[25], L[26] = zero, zero
			L[27] = L[25].Add(L[26])
			L[28] = L[24].Sub(L[27])
		}

		// --- ending() -------------------------------------------------------------
		for j := 6; j <= 11; j++ {
			o.binary(L[j].String())
		}
		if L[12].Less(P_1) {
			o.binary(L[12].String())
		}
		o.binary(L[13].String()) // wmsg 13
		o.binary(L[14].String()) // wmsg 14
		for j := 15; j <= 18; j++ {
			o.binary(L[j].String())
		}
		o.binary(L[19].String()) // wmsg 19
		o.binary(L[20].String()) // wmsg 20
		o.binary(L[21].String())
		if owe {
			o.binary(L[22].String())
			o.binary(L[23].String()) // wmsg 23
		} else {
			for j := 24; j <= 28; j++ {
				o.binary(L[j].String())
			}
		}
		for k := 29; k <= 47; k++ {
			o.binary(L[k].String())
		}
	}

	drive(in, o, loop, processReturn, nil) // all state is per-return local: no reset
	return 0
}
