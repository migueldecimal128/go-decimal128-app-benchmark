// SPDX-License-Identifier: GPL-3.0-or-later
//
// rate.c — currency-conversion / deposit / mortgage / TCM / prime rate tables
// (via the Swift Rate.swift). The rate files are streams: each daily update
// consumes the next row(s); at EOF the table keeps its previous value (some
// print "Use the old … rate"). Reads the BID-encoded .bid128 variants.
package main

import d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/dec"

func (bk *bank) initCoin() {
	for i := 0; i <= 8; i++ {
		bk.conversionRate[i][1] = bk.CENT // COPY64 CENT
	}
	coins := [...]string{"0.05", "0.1", "0.01", "0.01", "0.1", "1", "0.1", "0.05", "1"} // AUD..TWD
	for i, c := range coins {
		bk.conversionRate[9+i][1] = d.MustParse(c)
	}
}

func (bk *bank) openConversionRateFile() {
	names := [...]string{"AUD_USD", "CNY_USD", "EUR_USD", "GBP_USD", "HKD_USD", "JPY_USD",
		"NZD_USD", "SGD_USD", "TWD_USD", "USD_AUD", "USD_CNY", "USD_EUR",
		"USD_GBP", "USD_HKD", "USD_JPY", "USD_NZD", "USD_SGD", "USD_TWD"}
	for i, n := range names {
		bk.conversionRateFiles[i] = bk.openOrDie("datafile/bin/"+n+".txt.bid128", "rate.c")
	}
}

func (bk *bank) openDepositRateFile() {
	cur := [...]string{"AUD", "CNY", "EUR", "GBP", "HKD", "JPY", "NZD", "SGD", "TWD", "USD"}
	for i, c := range cur {
		bk.depositRateFiles[i] = bk.openOrDie("datafile/bin/"+c+"_Interest_Rate.txt.bid128", "rate.c")
	}
}

func (bk *bank) openMortgageRateFile() {
	names := [...]string{"15YearFRM", "30YearFRM", "1YearARM", "51ARM"} // FRM15/30, ARM1, ARM51
	for i, n := range names {
		bk.mortRateFiles[i] = bk.openOrDie("datafile/bin/"+n+".txt.bid128", "rate.c")
	}
}

func (bk *bank) sizeFail(tag string, n int) {
	bk.emit(f("[%s]size written in is %d\n", tag, n))
	bk.flushAndExit(255)
}

func (bk *bank) updateConversionTable() {
	for i := 0; i < COUNTRIES*2; i++ {
		recs := bk.conversionRateFiles[i].readDec(1)
		if len(recs) != 0 {
			if len(recs) == 1 {
				bk.conversionRate[i][0] = recs[0]
			} else {
				bk.sizeFail("CONV TABLE", len(recs))
			}
		} else {
			bk.errorPrint("rate.c", INFO_CODE, INFO_CODE, "Use the old conversion rate\n")
		}
	}
}

func (bk *bank) updateDepositRateTable() {
	for i := 0; i < COUNTRIES+1; i++ {
		recs := bk.depositRateFiles[i].readDec(3)
		if len(recs) != 0 {
			if len(recs) != 3 {
				myprint(f("size read in is %d\n", len(recs)))
				bk.flushAndExit(255)
			}
			r := bk.depositRate[i]
			r.checking = bk.CHECKING_RATE
			r.one = recs[0]
			r.three = recs[1]
			r.six = recs[2]
		}
	}
}

func (bk *bank) updateMortgageRate() int {
	if bk.counter%7 == 0 {
		mr := bk.mortRate
		recs := bk.mortRateFiles[FRM15YEAR-MORTGAGE].readDec(2)
		if len(recs) != 0 {
			if len(recs) == 2 {
				mr.frm15, mr.frm15Point = recs[0], recs[1]
			} else {
				bk.sizeFail("MORT RATE1", len(recs))
			}
		}
		recs = bk.mortRateFiles[FRM30YEAR-MORTGAGE].readDec(2)
		if len(recs) != 0 {
			if len(recs) == 2 {
				mr.frm30, mr.frm30Point = recs[0], recs[1]
			} else {
				bk.sizeFail("MORT RATE2", len(recs))
			}
		}
		recs = bk.mortRateFiles[ARM1YEAR-MORTGAGE].readDec(3)
		if len(recs) != 0 {
			if len(recs) == 3 {
				mr.arm1, mr.arm1Point = recs[0], recs[1]
				mr.arm1Margin = recs[2]
			} else {
				bk.sizeFail("MORT RATE3", len(recs))
			}
		}
		recs = bk.mortRateFiles[ARM51YEAR-MORTGAGE].readDec(3)
		if len(recs) != 0 {
			if len(recs) == 3 {
				mr.arm51, mr.arm51Point = recs[0], recs[1]
				mr.arm51Margin = recs[2]
			} else {
				bk.sizeFail("MORT RATE4", len(recs))
			}
		}
	}
	return 0
}

func (bk *bank) updateTCMRateFile() int {
	recs := bk.tcmRateFile.readDec(1)
	if len(recs) != 0 {
		if len(recs) == 1 {
			bk.tcmRate = recs[0]
		} else {
			bk.sizeFail("TCM RATE", len(recs))
		}
	} else {
		bk.errorPrint("rate.c", INFO_CODE, INFO_CODE, "Use the old TCM rate\n")
	}
	return 0
}

func (bk *bank) updatePrimeRateFile() int {
	recs := bk.primeRateFile.readDec(1)
	if len(recs) != 0 {
		if len(recs) == 1 {
			num := recs[0]
			if eq(bk.primeRate, num) {
				return 0
			}
			bk.primeRate = num
			return 1
		}
		bk.sizeFail("PRIME RATE", len(recs))
	} else {
		bk.errorPrint("rate.c", INFO_CODE, INFO_CODE, "Use the old Prime rate\n")
	}
	return 0
}
