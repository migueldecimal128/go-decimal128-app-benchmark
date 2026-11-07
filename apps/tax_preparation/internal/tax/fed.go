// SPDX-License-Identifier: GPL-3.0-or-later
//
// US Federal 1040 (2006) — faithful transliteration of apps/tax_preparation/c/src/
// taxsolve_US_1040_2006.c (macro_assign / initialization / action / ending +
// TaxRateFormula/Function, the Qualified-Dividends & Capital-Gain worksheet, the
// Schedule-D tax worksheet, the Alternative Minimum Tax worksheet, Schedule-A,
// and the capital-gains reader), via the Swift FED.swift. GPL, this repo only.
//
// The gated output is the DEBUG "BINARY OUTPUT: <value>" line each myfwrite to
// the output file emits to stderr. `shownum` writes only to the output file, so
// it emits NOTHING to stderr.
//
// CHECK_ORIG == 0 (the header), so: the exemptions worksheet rounds the
// excess/den ratio toward +infinity (ceiling); TaxRateFunction truncates the step
// index toward zero and does NOT re-round the resulting tax. All other arithmetic
// is ties-to-even.
package tax

import (
	"fmt"
	"strconv"
	"strings"

	d "github.com/migueldecimal128/decimal128-go"
)

func RunFED() int {
	path, loop, ok := parseArgs("tax_fed")
	if !ok {
		return 1
	}
	o := newOut()
	in := openInput(path)
	add := func(a, b d.Decimal128) d.Decimal128 { return a.Add(b) }
	mul := func(a, b d.Decimal128) d.Decimal128 { return a.Mul(b) }
	p := d.MustParse
	minimum, maximum := d.Minimum, d.Maximum

	// --- macro_assign(): constant table, built with the SAME ops the C uses so the
	//     decimal exponents — and thus trailing zeros in the transcript — match.
	P_P5 := p("0.5")
	P_P01 := p("0.01")
	P_P02 := add(P_P01, P_P01)
	P_P03 := add(P_P01, P_P02)
	P_P05 := add(P_P02, P_P03)
	P_P025 := mul(P_P05, P_P5)
	P_P075 := add(P_P05, P_P025)
	P_P1 := add(P_P05, P_P05)
	P_P15 := p("0.15")
	P_P25 := mul(P_P5, P_P5)
	P_P26 := add(P_P25, P_P01)
	P_P28 := add(P_P25, P_P03)
	P_P33 := add(P_P28, P_P05)
	P_P35 := add(P_P33, P_P02)
	P_P8 := p("0.8")
	P_1 := add(P_P5, P_P5)
	P_1P5 := add(P_1, P_P5)
	P_2 := add(P_1, P_1)
	P_3 := add(P_2, P_1)
	P_5 := add(P_3, P_2)
	P_10 := add(P_5, P_5)
	P_25 := mul(P_5, P_5)
	P_50 := add(P_25, P_25)
	P_100 := add(P_50, P_50)
	P_150 := add(P_100, P_50)
	P_500 := mul(P_100, P_5)
	P_1000 := add(P_500, P_500)
	P_1100 := add(P_1000, P_100)
	P_1250 := add(P_1100, P_150)
	P_2500 := add(P_1250, P_1250)
	P_3000 := add(P_2500, P_500)
	P_3300 := mul(P_1100, P_3)
	P_5150 := p("5150.0")
	P_7550 := p("7550.0")
	P_10300 := p("10300.0")
	P_10750 := p("10750.0")
	P_15000 := mul(P_150, P_100)
	P_15100 := add(P_15000, P_100)
	P_30000 := add(P_15000, P_15000)
	P_30650 := add(add(P_500, P_150), P_30000) // temp = 650
	P_31275 := p("31275.0")
	P_41050 := p("41050.0")
	P_42500 := p("42500.0")
	P_50000 := mul(P_500, P_100)
	P_61250 := p("61250.0")
	P_61300 := add(P_61250, P_50)
	P_61850 := p("61850.0")
	P_62550 := p("62550.0")
	P_74200 := p("74200.0")
	P_75000 := p("75000.0")
	P_75250 := add(add(P_100, P_150), P_75000) // temp = 250
	P_87500 := p("87500.0")
	P_94225 := p("94225.0")
	P_100000 := add(P_50000, P_50000)
	P_105000 := add(add(P_2500, P_2500), P_100000) // temp = 5000
	P_106000 := add(P_105000, P_1000)
	P_112500 := p("112500.0")
	P_112875 := p("112875.0")
	P_122500 := p("122500.0")
	P_123700 := p("123700.0")
	P_150000 := mul(P_150, P_1000)
	P_150500 := add(P_150000, P_500)
	P_154800 := p("154800.0")
	P_168275 := p("168275.0")
	P_171650 := p("171650.0")
	P_175000 := p("175000.0")
	P_188150 := p("188150.0")
	P_188450 := p("188450.0")
	P_225750 := p("225750.0")
	P_336550 := p("336550.0")
	P_9e9 := p("9e9")
	N_3000 := P_3000.Neg()
	N_1500 := add(P_1000, P_500).Neg() // -(1500)

	// Filing-status persistent state (C file-scope; initialization() does NOT
	// reset Do_* — faithful for the single graded return, loop == 1).
	var doSchedD, doQDCGTW, doSDTW, doline22, itemize bool
	status := 0

	// --- small helpers (the ISx / NotLessThanZero / EQ primitives) -------------
	isPos := func(x d.Decimal128) bool { return !x.IsZero() && !isNegative(x) }
	nlz := func(x d.Decimal128) d.Decimal128 { // NotLessThanZero
		if isNegative(x) {
			return zero
		}
		return x
	}
	eq := func(x, y d.Decimal128) bool { return !x.Less(y) && !y.Less(x) }

	// get_date(): days from 1-1-1980 (short/long capital-gain classification).
	daysTab := [13]int{0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}
	getDate := func(w string) int {
		parts := strings.FieldsFunc(w, func(r rune) bool { return strings.ContainsRune("-_/ \t", r) })
		num := func(i int) int {
			if i < len(parts) {
				if n, err := strconv.Atoi(parts[i]); err == nil {
					return n
				}
			}
			return 0
		}
		month, day, year := num(0), num(1), num(2)
		if year > 99 {
			year -= 1900
		}
		if year < 80 {
			year += 100
		}
		days := 0
		if (month >= 1) && (month <= 12) {
			days = daysTab[month]
		}
		return (days + day + 365*(year-80)) - 1
	}

	// TaxRateFormula(): bracketed tax (values from the 1040 instructions, pg 84).
	brkpt := [4][7]d.Decimal128{
		{zero, P_7550, P_30650, P_74200, P_154800, P_336550, P_9e9},   // Single
		{zero, P_15100, P_61300, P_123700, P_188450, P_336550, P_9e9}, // MFJ
		{zero, P_7550, P_30650, P_61850, P_94225, P_168275, P_9e9},    // MFS
		{zero, P_10750, P_41050, P_106000, P_171650, P_336550, P_9e9}, // HoH
	}
	txrt := [7]d.Decimal128{P_P1, P_P15, P_P25, P_P28, P_P33, P_P35, zero} // same every status
	taxRateFormula := func(x d.Decimal128, st int) d.Decimal128 {
		if st == widow {
			st = marriedFilingJointly
		}
		st--
		sum := zero
		bracket := 0
		for brkpt[st][bracket+1].Less(x) {
			t := brkpt[st][bracket+1].Sub(brkpt[st][bracket])
			sum = sum.Add(t.Mul(txrt[bracket]))
			bracket++
		}
		t := x.Sub(brkpt[st][bracket]).Mul(txrt[bracket])
		return t.Add(sum)
	}

	// TaxRateFunction(): tax-table emulation (quantize to the table step) < 100k.
	taxRateFunction := func(income d.Decimal128, st int) d.Decimal128 {
		if !income.Less(P_100000) {
			return taxRateFormula(income, st)
		}
		var x0 d.Decimal128
		switch {
		case income.Less(P_5):
			x0 = P_5
		case income.Less(P_25):
			x0 = P_10
		case income.Less(P_3000):
			x0 = P_25
		default:
			x0 = P_50
		}
		// CHECK_ORIG == 0 branch: k truncated (toward zero), tax NOT re-rounded.
		dx := P_P5.Mul(x0)
		k := d.TowardZero.RoundToIntegral(income.Quo(x0))
		x := x0.Mul(k).Add(dx)
		return taxRateFormula(x, st)
	}

	o.line("=== Tax Preparation Program starts ===")

	// The per-return body, re-run unchanged by the timed pass with the write off.
	processReturn := func(index int) {
		o.line(fmt.Sprintf("Processing %d tax return", index+1))

		// Per-return line arrays (MAX_LINES-sized in the C; the used indices fit 100).
		L := zeros(100)
		SchedA := zeros(100)
		SchedD := zeros(100)
		L9b, stcg, ltcg := zero, zero, zero
		var capgain, outarr [10]string
		capgainAmt := zeros(10)
		ws_1, ws_12 := zero, zero
		schedD_ws_13, schedD_ws_14 := zero, zero

		// showline / showschedA family — only the myfwrite forms reach stderr.
		emit := func(x d.Decimal128) { o.binary(x.String()) }
		showline := func(j int) { emit(L[j]) }
		showlineNZ := func(j int) {
			if !L[j].IsZero() {
				emit(L[j])
			}
		}
		showschedA := func(j int) {
			if isPos(SchedA[j]) {
				emit(SchedA[j])
			}
		}

		// --- Qualified Dividends and Capital Gain Tax Worksheet (pg 38) --------
		capgainsQualDividends := func(st int) {
			ws := zeros(50)
			skipto16 := false
			ws[1] = L[43]
			ws[2] = L9b
			if doSchedD {
				ws[3] = nlz(minimum(SchedD[15], SchedD[16]))
			} else {
				ws[3] = L[13]
			}
			ws[4] = ws[2].Add(ws[3])
			ws[5] = zero // form 4952, usually 0
			ws[6] = nlz(ws[4].Sub(ws[5]))
			ws[7] = nlz(ws[1].Sub(ws[6]))
			switch st {
			case single, marriedFilingSeparate:
				ws[8] = minimum(ws[1], P_30650)
			case marriedFilingJointly, widow:
				ws[8] = minimum(ws[1], P_61300)
			case headOfHousehold:
				ws[8] = minimum(ws[1], P_41050)
			}
			if ws[7].Less(ws[8]) {
				ws[9] = ws[7]
				ws[10] = ws[8].Sub(ws[9])
				ws[11] = P_P05.Mul(ws[10])
				if eq(ws[6], ws[10]) {
					skipto16 = true
				} else {
					ws[12] = minimum(ws[1], ws[6])
				}
			} else {
				ws[12] = minimum(ws[1], ws[6])
			}
			if !skipto16 {
				ws[13] = ws[10]
				ws[14] = ws[12].Sub(ws[13])
				ws[15] = P_P15.Mul(ws[14])
			}
			ws[16] = taxRateFunction(ws[7], st)
			ws[17] = ws[11].Add(ws[15]).Add(ws[16])
			ws[18] = taxRateFunction(ws[1], st)
			ws[19] = minimum(ws[17], ws[18])
			L[44] = ws[19]
		}

		// --- Schedule D Tax Worksheet (pg D9) ------------------------------------
		schedDTaxWorksheet := func(st int) {
			ws := zeros(100)
			ws[1] = L[43]
			ws[2] = L9b
			ws[3] = zero // form 4952 line 4g
			ws[4] = zero // form 4952 line 4e
			ws[5] = nlz(ws[3].Sub(ws[4]))
			ws[6] = nlz(ws[2].Sub(ws[5]))
			ws[7] = minimum(SchedD[15], SchedD[16])
			ws[8] = minimum(ws[3], ws[4])
			ws[9] = nlz(ws[7].Sub(ws[8]))
			ws[10] = ws[6].Add(ws[9])
			ws[11] = SchedD[18].Add(SchedD[19])
			ws[12] = minimum(ws[9], ws[11])
			ws[13] = ws[10].Sub(ws[12])
			ws[14] = nlz(ws[1].Sub(ws[13]))
			schedD_ws_13 = ws[13]
			schedD_ws_14 = ws[14]
			switch st {
			case single, marriedFilingSeparate:
				ws[15] = minimum(ws[1], P_30650)
			case marriedFilingJointly, widow:
				ws[15] = minimum(ws[1], P_61300)
			case headOfHousehold:
				ws[15] = minimum(ws[1], P_41050)
			}
			ws[16] = minimum(ws[14], ws[15])
			ws[17] = nlz(ws[1].Sub(ws[10]))
			ws[18] = maximum(ws[16], ws[17])
			ws[19] = ws[15].Sub(ws[16])
			ws[20] = P_P05.Mul(ws[19])
			if !eq(ws[1], ws[15]) {
				ws[21] = minimum(ws[1], ws[13])
				ws[22] = ws[19]
				ws[23] = nlz(ws[21].Sub(ws[22]))
				ws[24] = P_P15.Mul(ws[23])
				t1 := zero.Less(SchedD[19])
				t2 := zero.Less(SchedD[18])
				if t1 {
					ws[25] = minimum(ws[9], SchedD[19])
					ws[26] = ws[10].Add(ws[18])
					ws[27] = ws[1]
					ws[28] = nlz(ws[26].Sub(ws[27]))
					ws[29] = nlz(ws[25].Sub(ws[28]))
					ws[30] = P_P25.Mul(ws[29])
				}
				if t2 {
					temp1 := ws[18].Add(ws[19])
					temp2 := ws[23].Add(ws[29])
					ws[31] = temp1.Add(temp2)
					ws[32] = ws[1].Sub(ws[31])
					ws[33] = P_P28.Mul(ws[32])
				}
			}
			ws[34] = taxRateFunction(ws[18], st)
			temp1 := ws[20].Add(ws[24])
			temp2 := ws[30].Add(ws[33])
			temp1 = temp1.Add(ws[34])
			ws[35] = temp1.Add(temp2)
			ws[36] = taxRateFunction(ws[1], st)
			ws[37] = minimum(ws[35], ws[36])
			L[44] = ws[37]
		}

		// --- Alternative Minimum Tax Worksheet (pg 39) ---------------------------
		alternativeMinimumTax := func(st int, itemized bool) d.Decimal128 {
			WS := zeros(100)
			if itemized {
				WS[1] = L[41]
				temp1 := P_P025.Mul(L[38])
				WS[2] = minimum(SchedA[4], temp1)
				WS[3] = SchedA[9].Add(SchedA[26])
				WS[4] = WS[1].Add(WS[2]).Add(WS[3])
			} else {
				WS[4] = L[38]
			}
			WS[5] = L[10].Add(L[21])
			WS[6] = WS[4].Sub(WS[5])
			WS[7] = zero // form 8914, usually 0
			WS[8] = WS[6].Sub(WS[7])
			switch st {
			case single, headOfHousehold:
				WS[9] = P_42500
			case marriedFilingJointly, widow:
				WS[9] = P_62550
			case marriedFilingSeparate:
				WS[9] = P_31275
			}
			if WS[8].Less(WS[9]) || eq(WS[8], WS[9]) {
				return zero
			}
			WS[10] = WS[8].Sub(WS[9])
			switch st {
			case single, headOfHousehold:
				WS[11] = P_112500
			case marriedFilingJointly, widow:
				WS[11] = P_150000
			case marriedFilingSeparate:
				WS[11] = P_75000
			}
			if WS[11].Less(WS[8]) {
				WS[12] = WS[8].Sub(WS[11])
				WS[13] = P_P25.Mul(WS[12])
				if WS[9].Less(WS[13]) {
					WS[13] = WS[9]
				}
				WS[14] = WS[10].Add(WS[13])
			} else {
				WS[14] = WS[10]
			}
			t1 := P_175000.Less(WS[14])
			t2 := P_87500.Less(WS[14])
			if ((st != marriedFilingSeparate) && t1) || ((st == marriedFilingSeparate) && t2) {
				return P_100
			}
			WS[15] = P_P26.Mul(WS[14])
			WS[16] = L[44].Sub(L[47])
			if WS[16].Less(WS[15]) {
				return P_100
			}
			return zero
		}

		// ====================== initialization(capgain) =========================
		r1, ok := in.read()
		if !ok {
			return
		}
		status = int(d.TowardZero.ToInt64(r1.amt)) // Status
		L[6] = in.amt()                            // Dependents

		// L7 - L12, D4 - D14.
		rec, recOK := in.read()
	loop1:
		for ; recOK; rec, recOK = in.read() {
			switch rec.name {
			case "L7":
				L[7] = L[7].Add(rec.amt)
			case "L8a":
				L[8] = L[8].Add(rec.amt)
			case "L9":
				L[9] = L[9].Add(rec.amt)
			case "L9b":
				L9b = L9b.Add(rec.amt)
			case "L10", "L11", "L12":
				j := lineNo(rec.name)
				L[j] = L[j].Add(rec.amt)
			case "D4", "D5", "D11", "D12", "D13", "D14":
				j := lineNo(rec.name)
				SchedD[j] = SchedD[j].Add(rec.amt)
			case "D6":
				SchedD[6] = rec.amt // COPY64 (carryover)
			default:
				break loop1
			}
		}
		// `rec` now holds the "Cap-Gains/Losses <n>" record that broke the loop.
		iteri := 0
		if recOK {
			iteri = int(d.TowardZero.ToInt64(rec.amt))
		}
		ci := 0
		for iteri > 0 {
			iteri--
			if SchedD[13].IsZero() && SchedD[6].IsZero() && SchedD[14].IsZero() {
				if !doSchedD {
					capgain[ci] = "(c)  Cap Gains Schedule-D\n"
					ci++
					doSchedD = true
				}
			}
			// Buy (cost -> negative).
			buy, ok := in.read()
			if !ok {
				return
			}
			amnt1 := buy.amt
			if isPos(amnt1) {
				amnt1 = amnt1.Neg()
			}
			date1 := getDate(buy.name)
			// Sell.
			sell, ok := in.read()
			if !ok {
				return
			}
			amnt2 := sell.amt
			date2 := getDate(sell.name)
			longTerm := (date2 - date1) > 365
			if longTerm {
				SchedD[10] = SchedD[10].Add(amnt2)
				ltcg = ltcg.Add(amnt1).Add(amnt2)
			} else {
				SchedD[3] = SchedD[3].Add(amnt2)
				stcg = stcg.Add(amnt1).Add(amnt2)
			}
			capgainAmt[ci] = amnt1.Add(amnt2)
			if longTerm {
				capgain[ci] = "(d) \t Long = "
			} else {
				capgain[ci] = "(d) \t Short = "
			}
			ci++
		}

		// L14 - L39, A1 - A27, L47 - L71.
	loop2:
		for rr, ok := in.read(); ok; rr, ok = in.read() {
			switch rr.name {
			case "L14", "L17", "L18", "L19", "L21", "L23", "L24", "L25", "L26", "L27",
				"L28", "L29", "L30", "L32", "L33", "L34", "L35", "L39",
				"L47", "L48", "L49", "L50", "L51", "L52", "L53", "L54", "L55",
				"L58", "L59", "L60", "L61", "L62", "L64", "L65", "L67", "L68", "L69", "L70",
				"L15b", "L16b", "L20b", "L31a", "L66a":
				j := lineNo(rr.name) // trailing a/b suffix ignored, as the C's line map
				L[j] = L[j].Add(rr.amt)
			case "A1", "A5", "A6", "A7", "A8", "A10", "A11", "A12", "A13", "A15", "A16",
				"A17", "A19", "A20", "A21", "A22", "A27":
				j := lineNo(rr.name)
				SchedA[j] = SchedA[j].Add(rr.amt)
			case "L71":
				L[71] = L[71].Add(rr.amt)
				break loop2 // last line of a return
			default:
				break loop2
			}
		}

		// ============================== action() ================================
		// printL6_L12(): shownum(6) is output-file only; the rest emit.
		showline(7)
		emit(L[8]) // (d) L8a
		showline(9)
		emit(L9b) // (d) L9b
		showline(10)
		showline(11)
		showline(12)

		// get_cap_gains("L13", capgain): warnings, capgain dump, then Sched-D.
		if isPos(SchedD[6]) {
			SchedD[6] = SchedD[6].Neg() // D6 must be negative
		}
		if isPos(SchedD[14]) {
			SchedD[14] = SchedD[14].Neg()
		}
		if !SchedD[13].IsZero() || !SchedD[6].IsZero() || !SchedD[14].IsZero() {
			doSchedD = true
		}
		for k := 0; k < 10; k++ {
			if strings.Contains(capgain[k], "(d)") {
				emit(capgainAmt[k])
			}
		}
		if doSchedD {
			stcg = stcg.Add(SchedD[6])
			SchedD[7] = stcg
			SchedD[15] = ltcg.Add(SchedD[13])
			SchedD[15] = SchedD[15].Add(SchedD[14])
			SchedD[16] = SchedD[7].Add(SchedD[15])

			emit(stcg) // Total Short-term Gains
			emit(ltcg) // Total Long-term Gains
			for i := 3; i < 17; i++ {
				if (i == 8) || (i == 9) || (i == 11) || (i == 12) {
					continue // do nothing
				}
				emit(SchedD[i]) // D7 & D15 carry an (m) label
			}

			if isPos(SchedD[16]) || SchedD[16].IsZero() { // gain
				L[13] = SchedD[16]
				if isPos(SchedD[15]) && isPos(SchedD[16]) { // lines 18-21
					// 28% Rate Gain Worksheet (collectibles etc., usually zero).
					wsd := zeros(50)
					wsd[5] = SchedD[14]
					if isNegative(SchedD[7]) {
						wsd[6] = SchedD[7]
					}
					temp1 := wsd[1].Add(wsd[2])
					temp2 := wsd[3].Add(wsd[4])
					temp3 := wsd[5].Add(wsd[6])
					wsd[7] = temp1.Add(temp2)
					wsd[7] = wsd[7].Add(temp3)
					wsd[7] = nlz(wsd[7])
					SchedD[18] = wsd[7]
					emit(SchedD[18]) // D18
					emit(SchedD[19]) // D19
					if SchedD[18].IsZero() && SchedD[19].IsZero() {
						doQDCGTW = true
					} else {
						doSDTW, doQDCGTW = true, false
					}
					doline22 = false
				} else {
					doline22 = true
				}
			} else { // loss (Schedule-D line 21)
				maxloss := N_3000
				if status == marriedFilingSeparate {
					maxloss = N_1500
				}
				if SchedD[16].Less(maxloss) {
					SchedD[21] = maxloss
				} else {
					SchedD[21] = SchedD[16]
				}
				emit(SchedD[21]) // D21
				L[13] = SchedD[21]
				doline22 = true
			}

			if doline22 && isPos(L9b) {
				doQDCGTW = true
			}
		}

		for i := 7; i <= 21; i++ {
			L[22] = L[22].Add(L[i])
		}

		if !L[33].IsZero() {
			// Student loan interest calculation (pg 33).
			ws := zeros(20)
			ws[1] = minimum(L[33], P_2500)
			ws[2] = L[22]
			temp1 := L[23].Add(L[24])
			temp2 := L[25].Add(L[28])
			temp3 := L[29].Add(L[30])
			temp4 := L[31].Add(L[32])
			ws[3] = L[34].Add(temp1)
			temp1 = temp2.Add(temp3)
			temp1 = temp1.Add(temp4)
			ws[3] = ws[3].Add(temp1)
			ws[4] = ws[2].Sub(ws[3])
			if status == marriedFilingJointly {
				ws[5] = P_105000
			} else {
				ws[5] = P_50000
			}
			if ws[5].Less(ws[4]) {
				ws[6] = ws[4].Sub(ws[5])
				if status == marriedFilingJointly {
					ws[7] = ws[6].Quo(P_30000)
				} else {
					ws[7] = ws[6].Quo(P_15000)
				}
				if P_1.Less(ws[7]) {
					ws[7] = P_1
				}
				ws[8] = ws[1].Mul(ws[7])
			} else {
				ws[8] = zero
			}
			ws[9] = ws[1].Sub(ws[8])
			L[33] = ws[9]
		}

		for i := 23; i <= 35; i++ {
			L[36] = L[36].Add(L[i])
		}
		L[37] = L[22].Sub(L[36])
		L[38] = L[37]

		// Schedule-A.
		SchedA[2] = L[38]
		SchedA[3] = P_P075.Mul(SchedA[2])
		SchedA[4] = nlz(SchedA[1].Sub(SchedA[3]))
		SchedA[9] = add(add(SchedA[5], SchedA[6]), add(SchedA[7], SchedA[8]))
		SchedA[14] = add(add(SchedA[10], SchedA[11]), add(SchedA[12], SchedA[13]))
		SchedA[18] = add(add(SchedA[15], SchedA[16]), SchedA[17])
		SchedA[23] = add(add(SchedA[20], SchedA[21]), SchedA[22])
		SchedA[24] = L[38]
		SchedA[25] = P_P02.Mul(SchedA[24])
		SchedA[26] = nlz(SchedA[23].Sub(SchedA[25]))

		// Itemized deductions worksheet (limitation), Sched-A line 28.
		sumA := func() d.Decimal128 {
			return add(add(add(SchedA[4], SchedA[9]), add(SchedA[14], SchedA[18])),
				add(add(SchedA[19], SchedA[26]), SchedA[27]))
		}
		arrIdx := 0
		outarr[arrIdx] = "" // outarr[0]
		arrIdx++
		over150500 := P_150500.Less(L[38])
		over75250 := P_75250.Less(L[38])
		const notLimited = "(c)  Deductions not limited.\n"
		if (over150500 && (status != marriedFilingSeparate)) ||
			(over75250 && (status == marriedFilingSeparate)) {
			ws := zeros(90)
			ws[1] = sumA()
			t1 := SchedA[4].Add(SchedA[13])
			t2 := SchedA[15].Add(SchedA[19])
			t1 = t1.Add(SchedA[27])
			ws[2] = t1.Add(t2)
			if ws[2].Less(ws[1]) {
				ws[3] = ws[1].Sub(ws[2])
				ws[4] = P_P8.Mul(ws[3])
				ws[5] = L[38]
				if status != marriedFilingSeparate {
					ws[6] = P_150500
				} else {
					ws[6] = P_75250
				}
				if ws[6].Less(ws[5]) {
					ws[7] = ws[5].Sub(ws[6])
					ws[8] = P_P03.Mul(ws[7])
					ws[9] = minimum(ws[4], ws[8])
					ws[10] = ws[9].Quo(P_3)
					ws[11] = ws[9].Sub(ws[10])
					ws[12] = ws[1].Sub(ws[11])
					SchedA[28] = ws[12]
					ws_1 = ws[1]
					ws_12 = ws[12]
					outarr[arrIdx-1] = "(t) Sched-A Itemized Deductions Work Sheet (pg A-7) limits deduction between = "
				} else {
					SchedA[28] = ws[1]
					outarr[arrIdx-1] = notLimited
				}
			} else {
				SchedA[28] = ws[1]
				outarr[arrIdx-1] = notLimited
			}
		} else {
			outarr[arrIdx-1] = notLimited
			SchedA[28] = sumA()
		}

		L[40] = SchedA[28]
		itemize = zero.Less(L[40])

		outarr[arrIdx] = "" // outarr[1]
		arrIdx++
		const useStd = "(c) Use standard deduction.\n"
		switch status {
		case single, marriedFilingSeparate:
			if L[40].Less(P_5150) {
				L[40], outarr[arrIdx-1], itemize = P_5150, useStd, false
			}
		case marriedFilingJointly, widow:
			if L[40].Less(P_10300) {
				L[40], outarr[arrIdx-1], itemize = P_10300, useStd, false
			}
		case headOfHousehold:
			if L[40].Less(P_7550) {
				L[40], outarr[arrIdx-1], itemize = P_7550, useStd, false
			}
		}

		L[41] = L[38].Sub(L[40])

		outarr[arrIdx] = "" // outarr[2]
		arrIdx++
		if L[38].Less(P_112875) || eq(L[38], P_112875) {
			L[42] = P_3300.Mul(L[6])
		} else {
			// Exemptions worksheet (pg 36).
			denom := P_2500
			ws := zeros(100)
			ws4 := zero
			switch status {
			case single:
				ws4 = P_150500
			case marriedFilingJointly, widow:
				ws4 = P_225750
			case marriedFilingSeparate:
				ws4, denom = P_112875, P_1250
			case headOfHousehold:
				ws4 = P_188150
			}
			ws[4] = ws4
			if L[38].Less(ws[4]) || eq(L[38], ws[4]) {
				L[42] = P_3300.Mul(L[6])
			} else {
				ws[2] = P_3300.Mul(L[6])
				ws[3] = L[38]
				ws[5] = ws[3].Sub(ws[4])
				over122500 := P_122500.Less(ws[5])
				over61250 := P_61250.Less(ws[5])
				if over122500 || (over61250 && (status == marriedFilingSeparate)) {
					outarr[arrIdx-1] = "(c) Exemptions limited by excessive income ... (Exemptions Worksheet, pg 36)\n"
					L[42] = P_1100.Mul(L[6])
				} else {
					// CHECK_ORIG == 0: divide then round toward +infinity (ceiling).
					ws[6] = d.TowardPositive.RoundToIntegral(ws[5].Quo(denom))
					ws[7] = P_P02.Mul(ws[6])
					ws[8] = ws[2].Mul(ws[7])
					ws[9] = ws[8].Quo(P_1P5)
					ws[10] = ws[2].Sub(ws[9])
					L[42] = ws[10]
				}
			}
		}

		L[43] = nlz(L[41].Sub(L[42]))

		L[44] = taxRateFunction(L[43], status)

		qt1 := zero.Less(L9b)
		qt2 := zero.Less(L[13])
		qt3 := zero.Less(SchedD[15])
		qt4 := zero.Less(SchedD[16])
		if !doSDTW && !doQDCGTW && (qt1 || qt2 || (qt3 && qt4)) {
			doQDCGTW = true
		}
		if doQDCGTW {
			capgainsQualDividends(status)
		} else if doSDTW {
			schedDTaxWorksheet(status)
		}

		L[45] = alternativeMinimumTax(status, itemize)
		L[46] = L[44].Add(L[45])

		for i := 47; i <= 55; i++ {
			L[56] = L[56].Add(L[i])
		}
		L[57] = nlz(L[46].Sub(L[56]))
		for i := 57; i <= 62; i++ {
			L[63] = L[63].Add(L[i])
		}
		for i := 64; i <= 71; i++ {
			L[72] = L[72].Add(L[i])
		}

		// ============================== ending() ================================
		endIdx := 0
		showline(13)
		for j := 14; j <= 21; j++ {
			showlineNZ(j)
		}
		showline(22) // showline_wmsg
		for j := 23; j <= 35; j++ {
			showlineNZ(j)
		}
		showline(36)
		showline(37) // showline_wmsg
		showline(38)
		// shownum(39): output-file only — no stderr.
		for j := 1; j <= 27; j++ {
			showschedA(j)
		}
		if strings.Contains(outarr[endIdx], "(t)") {
			emit(ws_1)
			emit(ws_12)
		}
		endIdx++
		showschedA(28)
		if strings.Contains(outarr[endIdx], "(t)") {
			emit(ws_1)
			emit(ws_12)
		}
		endIdx++
		// if (itemize) -> output-file only.
		showline(40)
		showline(41)
		if strings.Contains(outarr[endIdx], "(t)") { // NB: no increment in the C
			emit(ws_1)
			emit(ws_12)
		}
		showline(42)
		showline(43) // showline_wmsg
		if !doQDCGTW && doSDTW {
			emit(schedD_ws_13)
			emit(schedD_ws_14)
		}
		showline(44) // showline_wmsg
		// if (ISZERO(L45)) -> output-file only.
		showlineNZ(45)
		showline(46)
		for j := 47; j <= 55; j++ {
			showlineNZ(j)
		}
		showline(56)
		showline(57)
		for j := 58; j <= 62; j++ {
			showlineNZ(j)
		}
		showline(63) // showline_wmsg
		showline(64)
		for j := 65; j <= 71; j++ {
			showlineNZ(j)
		}
		showline(72) // showline_wmsg
		if L[63].Less(L[72]) {
			L[73] = L[72].Sub(L[63])
			emit(L[73]) // REBATE
			emit(L[74])
		} else {
			L[76] = L[63].Sub(L[72])
			emit(L[76]) // YOU OWE
		}
	}

	// Persistent filing-status state is reset before each timed replay so every
	// pass repeats pass 1's single-return computation.
	reset := func() {
		doSchedD, doQDCGTW, doSDTW, doline22, itemize = false, false, false, false, false
		status = 0
	}
	drive(in, o, loop, processReturn, reset)
	return 0
}
