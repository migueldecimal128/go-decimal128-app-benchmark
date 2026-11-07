// SPDX-License-Identifier: GPL-3.0-or-later
//
// deposit.c — CD + checking accounts (open/close/inquiry/deposit/withdrawl,
// per-day interest posting, currency conversion, wire transfer), via the Swift
// Deposit.swift. NEW_FEATURES is OFF (not in the d128 build's defines), so all
// daily_deposit_* aggregation is excluded; DEBUG is ON (the verbose transcript).
package main

import d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/decimal"

const (
	depLOC           = "deposit.c"
	penaltyRate      = "0.02"
	penaltySurcharge = "30"
)

func cdOrChecking(t int, cd, other string) string {
	if t == CD {
		return cd
	}
	return other
}

func flag(v int, on, off string) string {
	if v == 1 {
		return on
	}
	return off
}

func (bk *bank) notFound(loc, acctNo string) int {
	bk.errorPrint(loc, WARNING_CODE, ACCT_NOT_EXIST, "Cannot find "+acctNo+"'s account\n")
	return ACCT_NOT_EXIST
}

func (bk *bank) depositAcctOpen(t *htab, acctNo string, fund dec, period, acctType, currency, payDate int,
	rate *depositRate) int {
	a := &depositAcct{}
	if !t.hadd(acctNo, a) { // duplicate
		s := "[deposit_acct_open] cannot add " + acctNo + "'s account\n"
		bk.errorPrint(depLOC, WARNING_CODE, ADD_ACCT_FAIL, s)
		bk.emit(s) // myfprintf(stderr, input_str)
		return ADD_ACCT_FAIL
	}
	bk.depositAcctInit(a, acctNo, fund, period, acctType, currency, payDate)
	bk.getDepositRate(rate, a)
	return 0
}

func (bk *bank) depositAcctClose(t *htab, acctNo string, acctType int, rates []*depositRate) int {
	if !t.hfind(acctNo) {
		return bk.notFound(depLOC, acctNo)
	}
	a := t.hstuff().(*depositAcct)
	inputStr := "##### Account " + a.acct_no + " is going to close"
	if (a.acct_type == CD) && (a.maturity_date > bk.counter) {
		penalty := bk.penaltyComputation(a, rates[a.currency])
		penaltyStr := toStr(penalty)
		inputStr += " PREMATURELY #####\n"
		bk.errorPrint(depLOC, INFO_CODE, INFO_CODE_NO_LINE, inputStr)
		bk.errorPrint(depLOC, INFO_CODE, INFO_CODE,
			"User needs to pay total penalty (fixed surcharge is $"+penaltySurcharge+"):$"+penaltyStr+"\n")
	} else {
		bk.errorPrint(depLOC, INFO_CODE, INFO_CODE_NO_LINE, inputStr)
		bk.errorPrint(depLOC, INFO_CODE, INFO_CODE_NO_LINE, " ######\n")
	}
	t.hdel()
	return 0
}

func (bk *bank) depositMarkExtend(t *htab, acctNo string) int {
	if !t.hfind(acctNo) {
		return bk.notFound(depLOC, acctNo)
	}
	t.hstuff().(*depositAcct).extend = 1
	return 0
}

func (bk *bank) depositMarkClose(t *htab, acctNo string) int {
	if !t.hfind(acctNo) {
		return bk.notFound(depLOC, acctNo)
	}
	t.hstuff().(*depositAcct).set_close = 1
	return 0
}

func (bk *bank) depositAcctInquiry(t *htab, acctNo string) int {
	if !t.hfind(acctNo) {
		return bk.notFound(depLOC, acctNo)
	}
	a := t.hstuff().(*depositAcct)
	bk.emit("============== DEPOSIT ACCOUNT REPORT ===============\n")
	bk.emit("Account Number: " + a.acct_no + "\n")
	bk.emit("Account Type: " + cdOrChecking(a.acct_type, "CD", "CHECKING") + "\n")
	if a.acct_type == CD {
		bk.emit(f("Account Period: %d month(s)\n", a.period))
	}
	bk.emit(f("Account Opening Date: %d\n", a.open_date))
	if a.acct_type == CD {
		bk.emit(f("Account Mature Date: %d\n", a.maturity_date))
	}
	bk.emit(f("Account Pay Date: Every %d days\n", a.pay_date))
	if a.acct_type == CD {
		bk.emit("Account Close Flag: " + flag(a.set_close, "SET", "DISABLE") + "\n")
		bk.emit("Account Extend Flag: " + flag(a.extend, "SET", "DISABLE") + "\n")
	}
	if isZero(a.last_withdrawl) || isNeg(a.last_withdrawl) {
		bk.emit("Account Last Withdrawl: Not Available\n")
	} else {
		bk.emit("Account Last Withdrawl: " + toStr(a.last_withdrawl))
		bk.emit(" in " + bk.checkName(a.last_withdrawl_unit) + "\n")
	}
	if isNeg(a.last_deposit) || isZero(a.last_deposit) {
		bk.emit("Account Last Deposit: Not Available\n")
	} else {
		bk.emit("Account Last Deposit: " + toStr(a.last_deposit))
		bk.emit(" in " + bk.checkName(a.last_deposit_unit) + "\n")
	}
	bk.emit("Account Current Daily Rate: " + toStr(a.rate) + " percentage \n")
	bk.emit("Account Accumulate Interest during this pay period: " + toStr(a.unposted_interest) + "\n")
	bk.emit("Account Total Interest: " + toStr(a.interest) + "\n")
	str := "Not Available"
	if !(isNeg(a.last_statement) || isZero(a.last_statement)) {
		str = toStr(a.last_statement)
	}
	bk.emit("Acount Last Statement: " + str + "\n")
	bk.emit("Account Current Balance: " + toStr(a.current_balance))
	bk.emit(" in " + bk.checkName(a.currency) + "\n")
	bk.emit("========================================================\n")
	return 0
}

func (bk *bank) moneyDeposit(t *htab, acctNo string, fund dec, depositUnit int) int {
	if !t.hfind(acctNo) {
		return bk.notFound(depLOC, acctNo)
	}
	a := t.hstuff().(*depositAcct)
	a.last_deposit_unit = depositUnit
	a.last_deposit = fund
	temp := fund
	if depositUnit != a.currency {
		temp, _ = bk.conversionUS(depositUnit, a.currency, fund)
	}
	a.current_balance = a.current_balance.Add(temp)
	bias := 0
	if a.currency != COUNTRIES {
		bias = COUNTRIES
	}
	a.current_balance = bk.roundToAvailableCoin(a.current_balance, bk.conversionRate[a.currency+bias][1])
	return 0
}

func (bk *bank) moneyWithdrawl(t *htab, acctNo string, withdrawlUnit int, money dec, rates []*depositRate) int {
	if !t.hfind(acctNo) {
		return bk.notFound(depLOC, acctNo)
	}
	a := t.hstuff().(*depositAcct)
	penalty := zeroD
	if (a.acct_type == CD) && (a.maturity_date > bk.counter) {
		penalty = bk.penaltyComputation(a, rates[a.currency])
	}
	// DEBUG
	bk.emit("The original balance is " + toStr(a.current_balance) + "\n")
	bk.emit("The unposted interest is " + toStr(a.unposted_interest) + "\n")
	bk.errorPrint(depLOC, INFO_CODE, INFO_CODE, "Unposted interest goes into current balance immediately\n")
	tempBal := a.current_balance.Add(a.unposted_interest)
	tempBal = tempBal.Sub(penalty)
	fromStr := bk.checkName(withdrawlUnit)
	toStrCur := bk.checkName(a.currency)
	temp := money
	if withdrawlUnit != a.currency {
		temp, _ = bk.conversionUS(withdrawlUnit, a.currency, money)
	}
	bk.emit("The converted amount is " + toStr(temp) + "\n")
	tempBal = tempBal.Sub(temp)
	kind := cdOrChecking(a.acct_type, "CD", "Checking")
	if isNeg(tempBal) {
		bk.errorPrint(depLOC, WARNING_CODE, NOT_ENOUGH_BALANCE, "Deposit account balance is not enough for withdrawl.\n")
		bk.errorPrint(depLOC, INFO_CODE, INFO_CODE_NO_LINE,
			"###### "+a.acct_no+" FAILs to withdrawl his "+kind+" account ######\n")
		return NOT_ENOUGH_BALANCE
	}
	a.last_withdrawl = money
	a.last_withdrawl_unit = withdrawlUnit
	a.unposted_interest = zeroD
	a.current_balance = tempBal
	bias := 0
	if a.currency != COUNTRIES {
		bias = a.currency + COUNTRIES
	}
	a.current_balance = bk.roundToAvailableCoin(a.current_balance, bk.conversionRate[bias][1])
	bk.errorPrint(depLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"##### "+a.acct_no+" WITHDRAWL "+fromStr+" "+toStr(money)+" from its "+kind+" account #####\n")
	bk.errorPrint(depLOC, INFO_CODE, INFO_CODE,
		"Total withdrawl penalty (fixed surcharge: $"+penaltySurcharge+") is $"+toStr(penalty)+" "+toStrCur+"\n")
	return 0
}

func (bk *bank) endOfTheDayDeposit(cdTable, checkingTable *htab) {
	// checking accounts first
	for ok := checkingTable.hfirst(); ok; ok = checkingTable.hnext() {
		bk.depositPayInterest(checkingTable.hstuff().(*depositAcct))
	}
	for ok := cdTable.hfirst(); ok; ok = cdTable.hnext() {
		a := cdTable.hstuff().(*depositAcct)
		bk.depositPayInterest(a)
		if (a.maturity_date <= bk.counter) && (a.maturity_date+GRACE_PERIOD > bk.counter) {
			if a.set_close == 1 {
				bk.errorPrint("deposit.c", INFO_CODE, INFO_CODE_NO_LINE,
					"##### Account "+a.acct_no+" is going to close #####")
				cdTable.hdel()
			} else if a.extend == 1 {
				a.extend = 0
				bk.getDepositRate(bk.depositRate[a.currency], a)
				a.current_balance = a.current_balance.Add(a.unposted_interest)
				a.unposted_interest = zeroD
				a.open_date = bk.counter
				a.maturity_date = bk.counter + a.period*30
			}
		} else if bk.counter == a.maturity_date+GRACE_PERIOD {
			a.extend = 0
			bk.getDepositRate(bk.depositRate[a.currency], a)
			a.current_balance = a.current_balance.Add(a.unposted_interest)
			a.unposted_interest = zeroD
			a.open_date = bk.counter - GRACE_PERIOD
			a.maturity_date = bk.counter - GRACE_PERIOD + a.period*30
		}
	}
}

func (bk *bank) wireTransfer(t *htab, fromUnit int, acctNo string, to int, inv dec) int {
	out := zeroD
	if acctNo != "NON_CUSTOMER" { // our customer
		if !t.hfind(acctNo) {
			return bk.notFound(depLOC, acctNo)
		}
		a := t.hstuff().(*depositAcct)
		from := a.currency
		a.current_balance = a.current_balance.Add(a.unposted_interest)
		a.unposted_interest = zeroD
		if a.currency != to {
			out, _ = bk.conversionUS(from, to, inv)
		} else {
			out = inv
		}
		temp2 := a.current_balance.Sub(inv)
		if isNeg(temp2) {
			bk.errorPrint(depLOC, WARNING_CODE, CD_CK_NO_MONEY,
				"###### This wire transfer cannot be made due to lack of money in "+a.acct_no+"'s account ######\n")
			return CD_CK_NO_MONEY
		} else if isZero(temp2) {
			bk.errorPrint(depLOC, INFO_CODE, INFO_CODE,
				a.acct_no+"'s account will have zero balance after this wire transfer\n")
		} else {
			a.current_balance = temp2
			a.last_withdrawl = inv
			a.last_withdrawl_unit = a.currency
		}
		processFee := zeroD
		inUnit := bk.checkName(from)
		outUnit := bk.checkName(to)
		bk.errorPrint(depLOC, INFO_CODE, INFO_CODE_NO_LINE,
			"###### "+a.acct_no+" wires "+toStr(inv)+" in "+inUnit+" to "+toStr(out)+" in "+outUnit+
				" with process fee as "+toStr(processFee)+" in "+inUnit+" ######\n")
	} else { // not our customer
		from := fromUnit
		processFee := bk.fixedExtraFee(inv, from)
		if from == to {
			out = inv
		} else {
			out, _ = bk.conversionUS(from, to, inv)
		}
		inUnit := bk.checkName(from)
		outUnit := bk.checkName(to)
		bk.errorPrint(depLOC, INFO_CODE, INFO_CODE_NO_LINE,
			"###### Non-customer wires "+toStr(inv)+" in "+inUnit+" to "+toStr(out)+" in "+outUnit+
				" with process fee as "+toStr(processFee)+" in "+inUnit+" ######\n")
	}
	return 0
}

// --- private helpers ------------------------------------------------------------

func (bk *bank) depositAcctInit(a *depositAcct, acctNo string, fund dec, period, acctType, currency, payDate int) int {
	if len(acctNo) > MAX_ACCT_NAME_LENGTH {
		return ACCT_LENGTH_FAIL
	}
	a.acct_no = acctNo
	a.period = period
	a.open_date = bk.counter
	if acctType == CD {
		a.maturity_date = bk.counter + period*30
	} else {
		a.maturity_date = -1
	}
	a.pay_date = payDate
	a.set_close, a.extend = 0, 0
	a.acct_type = acctType
	a.currency = currency
	a.last_withdrawl_unit, a.last_deposit_unit = 0, 0
	a.interest, a.unposted_interest, a.last_statement = zeroD, zeroD, zeroD
	a.last_deposit, a.last_withdrawl = zeroD, zeroD
	a.current_balance = fund
	return 0
}

// conversionUS returns (moneyTo, transaction).
func (bk *bank) conversionUS(from, to int, moneyFrom dec) (dec, dec) {
	if isZero(moneyFrom) {
		return zeroD, zeroD
	}
	if from == to {
		return moneyFrom, moneyFrom
	}
	cr := &bk.conversionRate
	var transaction, moneyTo dec
	switch {
	case from == COUNTRIES:
		transaction = moneyFrom.Mul(cr[to+COUNTRIES][0])
		moneyTo = bk.roundToAvailableCoin(transaction, cr[to+COUNTRIES][1])
	case to == COUNTRIES:
		transaction = moneyFrom.Mul(cr[from][0])
		moneyTo = bk.roundToAvailableCoin(transaction, cr[from][1])
	default:
		transaction = moneyFrom.Mul(cr[from][0])
		transaction = transaction.Mul(cr[to+COUNTRIES][0])
		moneyTo = bk.roundToAvailableCoin(transaction, cr[to+COUNTRIES][1])
	}
	return moneyTo, transaction
}

func (bk *bank) fixedExtraFee(inv dec, from int) dec {
	out := inv.Mul(bk.NON_CUSTOMER_FIXED_RATE)
	return bk.roundToAvailableCoin(out, bk.conversionRate[from][1])
}

func (bk *bank) penaltyComputation(a *depositAcct, r *depositRate) dec {
	var dailyRate dec
	switch a.period {
	case 1:
		dailyRate = r.one.Quo(bk.YEAR)
	case 3:
		dailyRate = r.three.Quo(bk.YEAR)
	default:
		dailyRate = r.six.Quo(bk.YEAR)
	}
	penRate := d.Maximum(d.MustParse(penaltyRate), dailyRate)
	temp := a.unposted_interest.Add(a.current_balance)
	penRate = penRate.Quo(bk.HUNDRED)
	penalty := penRate.Mul(temp)
	penalty = penalty.Add(d.MustParse(penaltySurcharge))
	index := 0
	if a.currency != COUNTRIES {
		index = a.currency + COUNTRIES
	}
	return bk.roundToAvailableCoin(penalty, bk.conversionRate[index][1])
}

func (bk *bank) depositPayInterest(a *depositAcct) {
	temp := a.current_balance.Add(a.unposted_interest)
	temp = temp.Mul(a.rate)
	temp = temp.Quo(bk.HUNDRED)
	a.unposted_interest = a.unposted_interest.Add(temp)
	a.unposted_interest = bk.roundToAvailableCoin(a.unposted_interest, bk.CENT)
	comp := (bk.counter - a.open_date) % a.pay_date
	if (comp == 0) && (bk.counter != a.open_date) {
		bk.errorPrint(depLOC, INFO_CODE, INFO_CODE_NO_LINE,
			"####### Today is the interest pay day for "+a.acct_no+"\n")
		a.interest = a.interest.Add(a.unposted_interest)
		a.current_balance = a.current_balance.Add(a.unposted_interest)
		a.unposted_interest = zeroD
		a.last_statement = a.current_balance
	}
}

func (bk *bank) getDepositRate(r *depositRate, a *depositAcct) {
	var dailyRate dec
	switch {
	case a.acct_type == CHECKING:
		dailyRate = r.checking.Quo(bk.YEAR)
	case a.period == 1:
		dailyRate = r.one.Quo(bk.YEAR)
	case a.period == 3:
		dailyRate = r.three.Quo(bk.YEAR)
	case a.period == 6:
		dailyRate = r.six.Quo(bk.YEAR)
	default:
		bk.errorPrint(depLOC, ERROR_CODE, CD_BAD_PERIOD, f("Wrong CD period, CD period cannot be %d\n", a.period))
		dailyRate = zeroD
	}
	a.rate = dailyRate
}
