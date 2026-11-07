// SPDX-License-Identifier: GPL-3.0-or-later
//
// mortgage.c — mortgage accounts (open/refinance/close/pay/inquiry, per-day
// interest + amortisation, ARM rate resets, closing-cost & penalty computation,
// the amortisation schedule table), via the Swift Mortgage.swift. The
// transcendentals module: termPayment / scheduleTable use POWER (= IEEE
// compound), termComputation uses LOG10 + the toward-positive to-integral window.
// NEW_FEATURES off, DEBUG on.
package main

import (
	"strconv"

	d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/decimal"
)

const mortLOC = "mortgage.c"

// closingCostFcn: title-insurance / closing-cost breakdown.
func (bk *bank) closingCostFcn(principal, downPayment dec) dec {
	mc := bk.mortConst
	sales := downPayment.Add(principal)
	temp := sales.Sub(mc.hundredThousand)
	temp = temp.Mul(mc.title_insurance_rate_1)
	temp = temp.Mul(mc.title_insurance_rate_2)
	titleInsurancePolicy := temp.Add(mc.title_insurance_base)
	temp = principal.Mul(mc.documentary_stamp_tax_on_note_rate_1)
	docStamp := temp.Mul(mc.documentary_stamp_tax_on_note_rate_2)
	intangible := mc.intagible_tax_on_note.Mul(principal)
	lenderPolicy, alta, endorsement := zeroD, zeroD, zeroD
	if !isZero(principal) {
		lenderPolicy = mc.lender_title_insurance_policy
		alta = mc.alta_title_endorsements
		temp = titleInsurancePolicy.Add(mc.title_endorsement_extra)
		endorsement = temp.Mul(mc.title_endorsement_rate)
	}
	temp = titleInsurancePolicy
	temp = temp.Add(lenderPolicy)
	temp = temp.Add(endorsement)
	temp = temp.Add(alta)
	temp = temp.Add(mc.title_search)
	temp = temp.Add(mc.title_examination)
	temp = temp.Add(mc.closing)
	temp = temp.Add(docStamp)
	temp = temp.Add(intangible)
	return temp.Add(mc.recording)
}

func (bk *bank) closingCost(a *mortAcct) {
	cc := bk.closingCostFcn(a.loan, a.downPayment)
	a.closingCost = cc.Add(a.downPayment)
	a.closingCost = bk.roundToAvailableCoin(a.closingCost, bk.CENT)
}

func (bk *bank) closePenalty(a *mortAcct) dec {
	years := (bk.counter - a.open_date) / 360
	mc := bk.mortConst
	rate := zeroD
	switch years {
	case 0:
		rate = mc.premature_close_penalty_year_0
	case 1:
		rate = mc.premature_close_penalty_year_1
	case 2:
		rate = mc.premature_close_penalty_year_2
	case 3:
		rate = mc.premature_close_penalty_year_3
	}
	return bk.roundToAvailableCoin(a.loan.Mul(rate), bk.CENT)
}

func (bk *bank) mortgageAcctClose(t *htab, acctNo string) int {
	if !t.hfind(acctNo) {
		return bk.notFound(mortLOC, acctNo)
	}
	a := t.hstuff().(*mortAcct)
	penalty := bk.closePenalty(a)
	bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"###### "+a.acct_no+"'s mortgage account is closed prematurely! ##### \n")
	bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE, "Remaining Principal: "+toStr(a.remaining_principal)+"\n")
	if a.overdue == 0 {
		bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE, "The Closing Cost: "+toStr(a.closingCost)+"\n")
		closingCost := penalty.Add(a.closingCost)
		a.remaining_principal = closingCost.Add(a.remaining_principal)
	} else {
		a.remaining_principal = a.remaining_principal.Add(penalty)
	}
	bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE, "The total payment is "+toStr(a.remaining_principal)+"\n")
	t.hdel()
	return 0
}

// termPayment: monthly = principal*r / (1 - (1+r)^-number).
func (bk *bank) termPayment(totalTerm int, rate, loan dec) dec {
	temp1 := fromInt(-totalTerm) // -number
	rPlus := rate.Add(bk.ONE)    // 1+r
	temp2 := power(rPlus, temp1) // (1+r)^-number
	temp2 = bk.ONE.Sub(temp2)    // 1 - (1+r)^-number
	temp3 := loan.Mul(rate)      // principal*r
	monthly := temp3.Quo(temp2)
	return bk.roundToAvailableCoin(monthly, bk.CENT)
}

// termComputation: number of months to pay off (given monthly); log10 + the
// toward-positive to-integral. Unused by the dispatch, ported for completeness.
func (bk *bank) termComputation(rate, loan, monthly dec) int {
	rPlus := rate.Add(bk.ONE)
	temp2 := rPlus.Log10()
	temp1 := rate.Mul(loan)
	temp1 = monthly.Sub(temp1)
	if isNeg(temp1) {
		bk.errorPrint(mortLOC, WARNING_CODE, NOT_ENOUGH_BALANCE,
			"monthly payment is not enough to pay the interest. Illegal operation in non-negative mortgage\n")
		return NOT_ENOUGH_BALANCE
	}
	temp1 = monthly.Quo(temp1)
	temp1 = temp1.Log10()
	temp1 = temp1.Quo(temp2)
	bk.currentRound = rpi
	temp1 = bk.toIntegral(temp1)
	bk.currentRound = rne
	n, _ := strconv.Atoi(toStr(temp1))
	return n
}

func (bk *bank) scheduleTable(loan dec, year int, mr *mortRate, acctType int) int {
	number := 12 * year
	n := max(number, 0)
	principalPay, interestPay, remaining := make([]dec, n), make([]dec, n), make([]dec, n)
	var rate dec
	switch acctType {
	case FRM15YEAR:
		rate = mr.frm15
	case FRM30YEAR:
		rate = mr.frm30
	case ARM1YEAR:
		rate = mr.arm1
	case ARM51YEAR:
		rate = mr.arm51
	default:
		bk.errorPrint(mortLOC, ERROR_CODE, MORT_TYPE_WRONG, "No this mortgage type!\n")
		rate = zeroD
	}
	newR := rate.Quo(bk.HUNDRED)
	if (bk.checkGEZero(loan) == 0) || (bk.checkGEZero(newR) == 0) || (bk.checkLEOne(newR) == 0) || (year <= 0) {
		bk.errorPrint(mortLOC, ERROR_CODE, BAD_INPUT, "Bad Inputs in Schduling Table")
	}
	temp1 := fromInt(-number) // -number
	r := newR.Quo(bk.TWELVE)  // rate/12
	rPlus := r.Add(bk.ONE)    // 1+r
	temp2 := power(rPlus, temp1)
	temp2 = bk.ONE.Sub(temp2)
	temp3 := loan.Mul(r)
	monthly := temp3.Quo(temp2)
	monthly = bk.roundToAvailableCoin(monthly, bk.CENT)
	totalInterest, totalPayment := zeroD, zeroD
	for months := 1; months <= number; months++ {
		t3 := fromInt(-number + months)
		t3 = power(rPlus, t3)
		t3 = bk.ONE.Sub(t3)
		i := months - 1
		remaining[i] = loan.Mul(t3)
		remaining[i] = remaining[i].Quo(temp2)
		remaining[i] = bk.roundToAvailableCoin(remaining[i], temp1)
		if months > 1 {
			interestPay[i] = remaining[i-1].Mul(r)
		} else {
			interestPay[0] = loan.Mul(r)
		}
		interestPay[i] = bk.roundToAvailableCoin(interestPay[i], temp1)
		principalPay[i] = monthly.Sub(interestPay[i])
		principalPay[i] = bk.roundToAvailableCoin(principalPay[i], temp1)
		totalInterest = totalInterest.Add(interestPay[i])
		totalPayment = totalPayment.Add(monthly)
	}
	_, _ = totalInterest, totalPayment
	bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"\t Month \t Principal payment \t Interest payment \t Remaining principal\n")
	for index := 0; index < number; index++ {
		bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE_NO_LINE,
			f("\t %d \t %s \t %s \t %s\n", index, toStr(principalPay[index]),
				toStr(interestPay[index]), toStr(remaining[index])))
	}
	return 0
}

func (bk *bank) interestPaid(a *mortAcct) dec {
	return bk.roundToAvailableCoin(a.remaining_principal.Mul(a.rate), bk.CENT)
}

func (bk *bank) mortgageAcctInit(a *mortAcct, period int, loan, downPayment dec, autoPay int, acctNo string) int {
	if len(acctNo) > MAX_ACCT_NAME_LENGTH {
		bk.errorPrint(mortLOC, ERROR_CODE, LENGTH_TOO_LONG, "account name is too long\n")
	}
	a.acct_no = acctNo
	a.open_date = bk.counter
	switch a.acct_type {
	case FRM15YEAR:
		a.period = 15 * 12
	case FRM30YEAR:
		a.period = 30 * 12
	case ARM1YEAR:
		if period < 1 {
			bk.errorPrint(mortLOC, WARNING_CODE, ACCT_LENGTH_FAIL,
				"ARM 1Year's length should be greater than 1 year for "+a.acct_no+"'s account\n")
			return ACCT_LENGTH_FAIL
		}
		a.period = period * 12
	default:
		if period < 5 {
			bk.errorPrint(mortLOC, WARNING_CODE, ACCT_LENGTH_FAIL,
				"ARM 5/1Year's length should be greater than 5 year for "+a.acct_no+"'s account\n")
			return ACCT_LENGTH_FAIL
		}
		a.period = period * 12
	}
	a.maturity_date = bk.counter + a.period*30
	a.pay_date = PAY_DATE
	a.totalTerm = a.period
	a.currentTerm = 0
	a.status = 1
	a.overdue = 0
	a.debt_paid = 0
	a.loan = loan
	a.accu_interest = zeroD
	a.remaining_principal = loan
	a.downPayment = downPayment
	bk.closingCost(a)
	a.auto_pay = autoPay
	return 0
}

func (bk *bank) mortgageAcctInquiry(t *htab, acctNo string) int {
	if !t.hfind(acctNo) {
		return bk.notFound(mortLOC, acctNo)
	}
	a := t.hstuff().(*mortAcct)
	bk.emit("============= MORTGAGE ACCOUNT REPORT ===============\n")
	bk.emit(f("Current Date is %d\n", bk.counter))
	bk.emit("Account Number: " + a.acct_no + "\n")
	bk.emit(f("Account Period: %d month(s)\n", a.period))
	bk.emit(f("Account Opening Date: %d\n", a.open_date))
	bk.emit(f("Account Mature Date: %d\n", a.maturity_date))
	bk.emit(f("Account Pay Date: Every %d days\n", a.pay_date))
	bk.emit(f("Account Total Terms: %d months(s)\n", a.totalTerm))
	bk.emit(f("Account Current Terms: %d^{th} month\n", a.currentTerm))
	bk.emit("Account Status: " + flag(a.status, "Active", "Non-Active") + "\n")
	bk.emit("Account Type: ")
	switch a.acct_type {
	case FRM15YEAR:
		bk.emit("15-Year Fixed Rate Mortgage\n")
	case FRM30YEAR:
		bk.emit("30-Year Fixed Rate Mortgage\n")
	case ARM1YEAR:
		bk.emit("1-Year ARM\n")
	default:
		bk.emit("5/1 ARM\n")
	}
	bk.emit("Account Overdue: " + flag(a.overdue, "Yes", "No") + "\n")
	bk.emit("Account Debt Paid? " + flag(a.debt_paid, "Yes", "Not Yet") + "\n")
	bk.emit("Account auto-pay from " + a.acct_no + " Checking account? " + flag(a.auto_pay, "Yes", "No") + "\n")
	rate := a.rate.Mul(bk.HUNDRED)
	if a.overdue == 0 {
		bk.emit("Account Mortgage Rate (monthly rate): " + toStr(rate) + " (in percentage)\n")
		rate = a.start_rate.Mul(bk.HUNDRED)
		bk.emit("Account Starting Mortgage Rate (monthly rate): " + toStr(rate) + " (in percentage)\n")
	} else {
		bk.emit("Account Penalty Rate (daily rate): " + toStr(rate) + " (in percentage)\n")
	}
	bk.emit("Account finance part (The part that needs mortgage): $" + toStr(a.loan) + "\n")
	bk.emit("Account Monthly Payment: $" + toStr(a.monthly) + "\n")
	bk.emit("Account Accumulated Interest paid so far: $" + toStr(a.accu_interest) + "\n")
	bk.emit("Account Remaining Principal: $" + toStr(a.remaining_principal) + "\n")
	bk.emit("Account Discount Point: " + toStr(a.point) + " (in percentage)\n")
	bk.emit("Account Margin : " + toStr(a.margin) + " (in percentage)\n")
	bk.emit("Account Down Payment: $" + toStr(a.downPayment) + "\n")
	bk.emit("Account Closing Cost: $" + toStr(a.closingCost) + "\n")
	bk.emit("============================================\n")
	return 0
}

func (bk *bank) mortgageAcctPay(t *htab, acctNo string) int {
	if !t.hfind(acctNo) {
		return bk.notFound(mortLOC, acctNo)
	}
	a := t.hstuff().(*mortAcct)
	if a.status == 0 {
		bk.errorPrint(mortLOC, WARNING_CODE, NOT_ACTIVE, a.acct_no+"'s account is not active\n")
		return NOT_ACTIVE
	}
	if a.overdue != 0 {
		bk.errorPrint(mortLOC, WARNING_CODE, MORT_ACCT_OVERDUE,
			"Account overdue. Mortgage Pay for "+a.acct_no+" is rejected\n")
		return MORT_ACCT_OVERDUE
	}
	if a.debt_paid != 0 {
		bk.errorPrint(mortLOC, WARNING_CODE, MORT_DEBT_PAID,
			a.acct_no+" have paid the monthly payment. Mortgage Pay is rejected\n")
		return MORT_DEBT_PAID
	}
	a.debt_paid = 1
	bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE,
		f("###### Monthly payment for %s's Mortgage is received on Day %d ######\n", a.acct_no, bk.counter))
	return 0
}

func (bk *bank) getMortgageRate(a *mortAcct, mr *mortRate) {
	switch a.acct_type {
	case FRM15YEAR:
		a.start_rate, a.rate, a.point, a.margin = mr.frm15, mr.frm15, mr.frm15Point, zeroD
	case FRM30YEAR:
		a.start_rate, a.rate, a.point, a.margin = mr.frm30, mr.frm30, mr.frm30Point, zeroD
	case ARM1YEAR:
		a.start_rate, a.rate, a.point, a.margin = mr.arm1, mr.arm1, mr.arm1Point, mr.arm1Margin
	case ARM51YEAR:
		a.start_rate, a.rate, a.point, a.margin = mr.arm51, mr.arm51, mr.arm51Point, mr.arm51Margin
	default:
		bk.errorPrint(mortLOC, ERROR_CODE, MORT_TYPE_WRONG, "No this mortgage type!\n")
	}
	a.rate = a.rate.Quo(bk.TWELVE).Quo(bk.HUNDRED)
	a.start_rate = a.start_rate.Quo(bk.TWELVE).Quo(bk.HUNDRED)
	a.point = a.point.Quo(bk.HUNDRED)
	a.margin = a.margin.Quo(bk.HUNDRED)
}

func (bk *bank) noChecking(acctNo string) int {
	bk.errorPrint(mortLOC, WARNING_CODE, ACCT_NOT_EXIST,
		"###### Checking Acct for "+acctNo+" doesn't exist. Mortgage account open fails ######\n")
	return ACCT_NOT_EXIST
}

func (bk *bank) mortgageAcctOpen(t, checking *htab, mr *mortRate, period int, loan, downPayment dec,
	acctNo string, acctType, autoPay int) int {
	a := &mortAcct{}
	if !t.hadd(acctNo, a) {
		bk.errorPrint(mortLOC, WARNING_CODE, ADD_ACCT_FAIL, "[mortgage_acct_open] cannot add "+acctNo+"'s account\n")
		return ACCT_NOT_EXIST
	}
	a.acct_type = acctType
	if bk.mortgageAcctInit(a, period, loan, downPayment, autoPay, acctNo) != 0 {
		t.hdel()
	}
	if (autoPay != 0) && !checking.hfind(acctNo) {
		return bk.noChecking(acctNo)
	}
	bk.getMortgageRate(a, mr)
	a.monthly = bk.termPayment(a.period, a.rate, loan)
	bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"###### "+a.acct_no+"'s MORTGAGE acct with financed part: $"+toStr(loan)+
			" and downpayment: $"+toStr(downPayment)+" is opened ######\n")
	return 0
}

func (bk *bank) mortgageAcctRefinance(t, checking *htab, mr *mortRate, period int, acctNo string,
	acctType, autoPay int) int {
	if !t.hfind(acctNo) {
		return bk.notFound(mortLOC, acctNo)
	}
	a := t.hstuff().(*mortAcct)
	a.acct_type = acctType
	if (autoPay != 0) && !checking.hfind(acctNo) {
		return bk.noChecking(acctNo)
	}
	bk.getMortgageRate(a, mr)
	bk.mortgageAcctInit(a, period, a.remaining_principal, a.downPayment, autoPay, acctNo)
	a.monthly = bk.termPayment(a.period, a.rate, a.remaining_principal)
	bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"###### "+a.acct_no+"'s MORTGAGE acct with re-financed part: $"+toStr(a.remaining_principal)+
			" and downpayment: $"+toStr(a.downPayment)+" is opened ######\n")
	return 0
}

func (bk *bank) penaltyOverdue(a *mortAcct, mr *mortRate) int {
	mc := bk.mortConst
	surcharge := bk.roundToAvailableCoin(a.loan.Mul(mc.overdue_surcharge_rate), bk.CENT)
	a.remaining_principal = a.remaining_principal.Add(surcharge)
	minRate := mc.min_overdue_yr_penalty_rate.Quo(bk.YEAR).Quo(bk.HUNDRED)
	minPoint := mc.min_overdue_yr_penalty_point.Quo(bk.HUNDRED)
	minMargin := mc.min_overdue_yr_penalty_margin.Quo(bk.HUNDRED)
	rate, point, margin := zeroD, zeroD, zeroD
	switch a.acct_type {
	case FRM15YEAR:
		rate, point = mr.frm15, mr.frm15Point
	case FRM30YEAR:
		rate, point = mr.frm30, mr.frm30Point
	case ARM1YEAR:
		rate, point, margin = mr.arm1, mr.arm1Point, mr.arm1Margin
	case ARM51YEAR:
		rate, point, margin = mr.arm51, mr.arm51Point, mr.arm51Margin
	}
	rate = rate.Quo(bk.YEAR).Quo(bk.HUNDRED)
	point = point.Quo(bk.YEAR).Quo(bk.HUNDRED)
	margin = margin.Quo(bk.YEAR).Quo(bk.HUNDRED)
	dailyRate := a.rate.Quo(bk.MONTH)
	rate = d.Maximum(rate, dailyRate)
	rate = d.Maximum(rate, minRate)
	a.rate = rate
	if (a.acct_type == ARM1YEAR) || (a.acct_type == ARM51YEAR) {
		point = d.Maximum(point, a.point)
		a.point = d.Maximum(point, minPoint)
		margin = d.Maximum(margin, a.margin)
		a.margin = d.Maximum(margin, minMargin)
	}
	return 0
}

func (bk *bank) autoPayCancelled(acctNo, kind string) {
	bk.errorPrint(mortLOC, WARNING_CODE, NOT_ENOUGH_BALANCE,
		f("The auto-pay of %s %s acct on Day %d is cancelled!\n Remember to deposit more money for the next withdrawl\n",
			acctNo, kind, bk.counter))
}

func (bk *bank) endOfTheDayMortgage(t, checking *htab, tcmRate dec) int {
	for ok := t.hfirst(); ok; ok = t.hnext() {
		a := t.hstuff().(*mortAcct)
		if a.overdue != 0 {
			bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE_NO_LINE, "Account is in the overdue status\n")
			temp := bk.ONE.Add(a.rate)
			a.remaining_principal = a.remaining_principal.Mul(temp)
			a.remaining_principal = bk.roundToAvailableCoin(a.remaining_principal, bk.CENT)
		} else if ((bk.counter-a.open_date)%a.pay_date == 0) && (bk.counter != a.open_date) {
			if a.auto_pay != 0 {
				if !checking.hfind(a.acct_no) {
					bk.autoPayCancelled(a.acct_no, "Mortgage")
					a.overdue, a.auto_pay, a.debt_paid = 1, 0, 0
				} else {
					var ret int
					if a.currentTerm == a.totalTerm-1 {
						temp := a.remaining_principal.Mul(bk.ONE.Add(a.rate))
						ret = bk.moneyWithdrawl(checking, a.acct_no, COUNTRIES, temp, bk.depositRate)
					} else {
						ret = bk.moneyWithdrawl(checking, a.acct_no, COUNTRIES, a.monthly, bk.depositRate)
					}
					if ret != 0 {
						bk.autoPayCancelled(a.acct_no, "Mortgage")
						a.debt_paid = 0
					}
					a.debt_paid = 1
				}
			}
			if a.debt_paid == 0 {
				bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE, "Account overdue! \n")
				a.overdue = 1
				bk.penaltyOverdue(a, bk.mortRate)
				continue
			}
			a.currentTerm++
			currentInterest := bk.interestPaid(a)
			bk.remainingPrincipalFcn(a)
			a.accu_interest = a.accu_interest.Add(currentInterest)
			a.debt_paid = 0
			if bk.counter == a.maturity_date {
				bk.errorPrint(mortLOC, INFO_CODE, INFO_CODE,
					f("on Day %d, %s mortgage account is matured!\n", bk.counter, a.acct_no))
				t.hdel()
				continue
			}
		}
		if (bk.counter%360 == 0) && (bk.counter != a.open_date) && (a.currentTerm != a.totalTerm) &&
			((a.acct_type == ARM51YEAR) || (a.acct_type == ARM1YEAR)) {
			bk.determineNewRate(a, tcmRate)
			a.monthly = bk.termPayment(a.totalTerm-a.currentTerm, a.rate, a.remaining_principal)
		}
	}
	return 0
}

func (bk *bank) remainingPrincipalFcn(a *mortAcct) int {
	monthlyInterest := a.remaining_principal.Mul(a.rate)
	temp := a.monthly.Sub(monthlyInterest)
	a.remaining_principal = a.remaining_principal.Sub(temp)
	a.remaining_principal = bk.roundToAvailableCoin(a.remaining_principal, bk.CENT)
	return 0
}

func (bk *bank) determineNewRate(a *mortAcct, tcmRate dec) int {
	if ((a.acct_type == ARM51YEAR) && (bk.counter < 5*360)) ||
		(a.acct_type == FRM15YEAR) || (a.acct_type == FRM30YEAR) {
		return 0
	}
	six := bk.TEN.Add(bk.MFOUR) // 10 + (-4) = 6
	temp1 := a.margin.Mul(bk.HUNDRED)
	temp1 = tcmRate.Add(temp1)
	temp2 := bk.HUNDRED.Mul(a.rate)
	temp2 = bk.TWELVE.Mul(temp2)
	temp2 = temp2.Add(bk.TWO)
	temp1 = d.Minimum(temp1, temp2)
	temp2 = bk.HUNDRED.Mul(a.start_rate)
	temp2 = temp2.Mul(bk.TWELVE)
	temp2 = temp2.Add(six)
	a.rate = d.Minimum(temp1, temp2)
	a.rate = a.rate.Quo(bk.TWELVE)
	a.rate = a.rate.Quo(bk.HUNDRED)
	return 0
}
