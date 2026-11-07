// SPDX-License-Identifier: GPL-3.0-or-later
//
// credit_card.c — rewards card: open/close/inquiry/pay/debt, daily finance
// charges (cash advance / balance transfer / purchase), statement generation,
// minimum-payment + late-fee logic, autopay, rate resets on prime-rate change;
// via the Swift CreditCard.swift. NEW_FEATURES off, DEBUG on. doPayment's
// pointer-sorted rate buckets are modelled as an identifier array
// (0=PURCHASE, 1=CASH_ADVANCE, 2=BALANCE_TRANSFER) with value/apr accessors.
package main

const ccLOC = "credit_card.c"

func ccVal(a *ccAcct, id int) dec {
	switch id {
	case PURCHASE:
		return a.purchase
	case CASH_ADVANCE:
		return a.cash_adv
	default:
		return a.bal_tx
	}
}

func ccSetVal(a *ccAcct, id int, v dec) {
	switch id {
	case PURCHASE:
		a.purchase = v
	case CASH_ADVANCE:
		a.cash_adv = v
	default:
		a.bal_tx = v
	}
}

func ccRateOf(a *ccAcct, id int) dec {
	switch id {
	case PURCHASE:
		return a.purchase_apr
	case CASH_ADVANCE:
		return a.cash_adv_apr
	default:
		return a.bal_tx_apr
	}
}

func (bk *bank) zeroIt(value int, a *ccAcct) int {
	switch value {
	case PURCHASE:
		a.purchase = zeroD
	case CASH_ADVANCE:
		a.cash_adv = zeroD
	case BALANCE_TRANSFER:
		a.bal_tx = zeroD
	default:
		bk.errorPrint(ccLOC, ERROR_CODE, CC_WRONG_TX_TYPE, "Wrong Transaction Type")
	}
	return 0
}

func autoDefault(a *ccAcct) {
	a.purchase_apr, a.cash_adv_apr, a.bal_tx_apr = a.default_apr, a.default_apr, a.default_apr
}

func (bk *bank) ccCancelled(acctNo string) {
	bk.errorPrint(ccLOC, WARNING_CODE, NOT_ENOUGH_BALANCE,
		f("The auto-pay of %s Credit Card acct on Day %d is cancelled!\n Remember to deposit more money for the next withdrawl\n",
			acctNo, bk.counter))
}

func (bk *bank) endOfTheDayCreditCard(t, checking *htab, primeRateChange int) int {
	for ok := t.hfirst(); ok; ok = t.hnext() {
		a := t.hstuff().(*ccAcct)
		if (a.intro_apr_bal_tx_end_date == -1) && (bk.counter == a.start_date+INIT_BAL_TX_PERIOD) {
			a.intro_apr_bal_tx_end_date = bk.counter
		}
		if bk.counter == a.intro_apr_bal_tx_end_date {
			a.bal_tx_apr = bk.ccRate.bal_tx_apr
		}
		if primeRateChange == 1 {
			bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE_NO_LINE, "********** Prime Rate Changes ************\n")
			if a.overdueHistory == 0 {
				a.purchase_apr = bk.ccRate.purchase_apr
				a.cash_adv_apr = bk.ccRate.cash_adv_apr
				if (bk.counter >= a.intro_apr_bal_tx_end_date) && (a.intro_apr_bal_tx_end_date >= 0) {
					a.bal_tx_apr = bk.ccRate.bal_tx_apr
				}
				a.default_apr = bk.ccRate.default_apr
				if a.underpaid == 1 {
					autoDefault(a)
				}
			} else {
				bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE_NO_LINE,
					"Unfortunately, you default before. The rates remain at default rate\n")
			}
		}
		if isPos(a.cash_adv) {
			temp := bk.roundToAvailableCoin(mul(a.cash_adv, a.cash_adv_apr), bk.CENT)
			a.cash_adv = add(a.cash_adv, temp)
			a.posted_interest_cash_adv = add(a.posted_interest_cash_adv, temp)
		}
		if isPos(a.bal_tx) {
			temp := bk.roundToAvailableCoin(mul(a.bal_tx, a.bal_tx_apr), bk.CENT)
			a.bal_tx = add(a.bal_tx, temp)
			a.posted_interest_bal_tx = add(a.posted_interest_bal_tx, temp)
		}
		if a.fullpaid == 0 {
			temp := bk.roundToAvailableCoin(mul(a.purchase, a.purchase_apr), bk.CENT)
			a.purchase = add(a.purchase, temp)
			a.posted_interest_purchase = add(a.posted_interest_purchase, temp)
		}
		if bk.counter == a.close_date {
			bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE,
				f(" Today (Day %d) is the day to generate statement \n", bk.counter))
			if isPos(a.purchase) && !isZero(a.purchase) {
				a.purchase = maxv(a.purchase, bk.ccConst.min_fin_charge)
			}
			a.purchase_statement = a.purchase
			convertedRate := divv(bk.ccConst.min_payment_rate, bk.HUNDRED)
			a.min_payment = mul(a.purchase, convertedRate)
			a.min_payment = bk.roundToAvailableCoin(a.min_payment, bk.CENT)
			a.purchase_total_paid = zeroD
		}
		if bk.counter == a.last_date {
			bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE,
				f(" Today (Day %d) is the last day to pay the monthly \n", bk.counter))
			if a.autopay != 0 {
				if !checking.hfind(a.acct_no) {
					bk.ccCancelled(a.acct_no)
					a.autopay, a.fullpaid, a.underpaid = 0, 0, 1
				} else {
					tempAutopay := sub(a.purchase_statement, a.purchase_total_paid)
					tempAutopay = add(tempAutopay, a.bal_tx)
					tempAutopay = add(tempAutopay, a.cash_adv)
					if isPos(tempAutopay) {
						if bk.moneyWithdrawl(checking, a.acct_no, COUNTRIES, tempAutopay, bk.depositRate) != 0 {
							bk.ccCancelled(a.acct_no)
							a.fullpaid, a.underpaid = 0, 1
						} else {
							a.fullpaid, a.underpaid = 1, 0
							a.purchase, a.cash_adv, a.bal_tx = zeroD, zeroD, zeroD
							a.credit_allowance = a.credit_limit
							a.last_paid = tempAutopay
						}
					}
				}
			} else {
				a.underpaid = 0
				if lt(a.purchase_total_paid, a.min_payment) {
					a.underpaid = 1
				}
				a.fullpaid = 1
				if lt(a.purchase_total_paid, a.purchase_statement) {
					a.fullpaid = 0
				}
			}
			if a.underpaid != 0 {
				autoDefault(a)
				a.overdueHistory++
				bk.lateFee(a, bk.ccConst)
			}
			a.close_date = bk.counter - (a.last_date - a.close_date) + a.pay_date
			a.last_date = a.close_date + CC_GRACE_PERIOD
			a.purchase_statement, a.purchase_total_paid, a.min_payment = zeroD, zeroD, zeroD
		}
		a.credit_allowance = sub(a.credit_limit, a.purchase)
		a.credit_allowance = sub(a.credit_allowance, a.bal_tx)
		a.credit_allowance = sub(a.credit_allowance, a.cash_adv)
	}
	return 0
}

func (bk *bank) lateFee(a *ccAcct, cc *ccConst) int {
	comp1 := sub(a.purchase, cc.level1_thr)
	comp2 := sub(a.purchase, cc.level2_thr)
	var fee dec
	switch {
	case isNeg(comp1):
		fee = cc.level1_fee
	case isNeg(comp2):
		fee = cc.level2_fee
	default:
		fee = cc.level3_fee
	}
	str := toStr(fee)
	a.purchase = add(a.purchase, fee)
	bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE, "Late fee is "+str+" and is accumulated to the purchase\n")
	return 0
}

func (bk *bank) creditCardAcctClose(t *htab, acctNo string) int {
	if !t.hfind(acctNo) {
		return bk.notFound(ccLOC, acctNo)
	}
	a := t.hstuff().(*ccAcct)
	pDebt, cDebt, bDebt := toStr(a.purchase), toStr(a.cash_adv), toStr(a.bal_tx)
	bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE,
		"###### "+a.acct_no+"'s credit card account is going to close.\n Please pay the debt shown below ######")
	bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"Normal Purchase: "+pDebt+"\nCash Advance: "+cDebt+"\nBalance Transfer: "+bDebt+"\n")
	t.hdel()
	return 0
}

func (bk *bank) doPayment(a *ccAcct, payment dec) int {
	temp := add(a.purchase, a.cash_adv)
	temp = add(temp, a.bal_tx)
	comp := sub(payment, temp)
	inPeriod := (bk.counter > a.close_date) && (bk.counter <= a.last_date)
	if !isNeg(comp) { // payment >= total balance
		bk.zeroIt(PURCHASE, a)
		bk.zeroIt(CASH_ADVANCE, a)
		bk.zeroIt(BALANCE_TRANSFER, a)
		if inPeriod {
			a.purchase_total_paid = a.purchase_statement
		}
		bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE,
			"Payment is paid in full, all the extra $"+toStr(comp)+" is returned to the customer\n")
		return 0
	}
	arr := [3]int{PURCHASE, CASH_ADVANCE, BALANCE_TRANSFER} // sort ascending by rate
	if gt(ccRateOf(a, arr[0]), ccRateOf(a, arr[1])) {
		arr[0], arr[1] = arr[1], arr[0]
	}
	if gt(ccRateOf(a, arr[1]), ccRateOf(a, arr[2])) {
		arr[1], arr[2] = arr[2], arr[1]
		if gt(ccRateOf(a, arr[0]), ccRateOf(a, arr[1])) {
			arr[0], arr[1] = arr[1], arr[0]
		}
	}
	temp = sub(ccVal(a, arr[0]), payment)
	if (arr[0] == PURCHASE) && inPeriod {
		a.purchase_total_paid = add(a.purchase_total_paid, payment)
	}
	if !isPos(temp) {
		bk.zeroIt(arr[0], a)
		if (arr[1] == PURCHASE) && inPeriod {
			a.purchase_total_paid = sub(a.purchase_total_paid, temp)
		}
		temp = add(temp, ccVal(a, arr[1]))
		if !isPos(temp) {
			bk.zeroIt(arr[1], a)
			ccSetVal(a, arr[2], add(ccVal(a, arr[2]), temp))
			if (arr[2] == PURCHASE) && inPeriod {
				a.purchase_total_paid = sub(a.purchase_total_paid, temp)
			}
		} else {
			ccSetVal(a, arr[1], temp)
		}
	} else {
		ccSetVal(a, arr[0], temp)
	}
	return 0
}

func (bk *bank) creditCardAcctPay(t *htab, acctNo string, paymentType int, payment dec) int {
	if !t.hfind(acctNo) {
		return bk.notFound(ccLOC, acctNo)
	}
	a := t.hstuff().(*ccAcct)
	var realPay dec
	switch {
	case paymentType == FULL_PAY:
		realPay = add(add(a.purchase, a.cash_adv), a.bal_tx)
	case (paymentType == STATEMENT_PAY) || (paymentType == MIN_PAY):
		if bk.counter < a.close_date {
			bk.errorPrint(ccLOC, WARNING_CODE, CC_REJ_PAY,
				"###### No statement or min payment available at this time. The payment on "+a.acct_no+
					"'s Credit Card acct is rejected ######\n")
			return CC_REJ_PAY
		}
		realPay = sub(a.purchase_statement, a.purchase_total_paid)
		if !isPos(realPay) {
			bk.errorPrint(ccLOC, WARNING_CODE, CC_REJ_PAY,
				"###### No need to pay anymore. The payment on "+a.acct_no+"'s Credit Card is rejected ######\n")
			return CC_REJ_PAY
		}
		if paymentType == MIN_PAY {
			realPay = sub(a.min_payment, a.purchase_total_paid)
			if !isPos(realPay) {
				realPay = sub(a.purchase_statement, a.purchase_total_paid)
				if isPos(realPay) {
					bk.errorPrint(ccLOC, WARNING_CODE, CC_REJ_PAY,
						"Min-payment for "+a.acct_no+"'s Credit Card has been paid. This action is rejected. However, you still have "+
							toStr(realPay)+" needed to pay to pay the statement in full")
				}
				return CC_REJ_PAY
			}
		}
	default:
		realPay = payment
	}
	bk.doPayment(a, realPay)
	a.credit_allowance = add(a.credit_allowance, realPay)
	a.credit_allowance = minv(a.credit_limit, a.credit_allowance)
	if isPos(a.credit_allowance) {
		a.overCreditLimit = 0
	}
	a.last_paid = realPay
	bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE_NO_LINE,
		f("###### Receive %s's payment ($%s) to his CREDIT CARD account on Day %d\n", a.acct_no, toStr(realPay), bk.counter))
	return 0
}

func (bk *bank) txCancelled(acctNo string) {
	bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"###### Transaction on "+acctNo+"'s Credit Card acct is cancelled ######\n")
}

func (bk *bank) creditCardAcctDebt(t *htab, acctNo string, currency int, cc *ccConst, typ int, fund dec) int {
	unit := bk.checkName(currency) // DEBUG
	typeStr := ""
	if !t.hfind(acctNo) {
		return bk.notFound(ccLOC, acctNo)
	}
	a := t.hstuff().(*ccAcct)
	convertedFund := fund
	if currency != COUNTRIES {
		if typ == BALANCE_TRANSFER {
			bk.errorPrint(ccLOC, WARNING_CODE, CC_REJ_TX, "Balance Transfer can only work with USD\n")
			bk.txCancelled(a.acct_no)
			return CC_REJ_TX
		}
		convertedFund, _ = bk.conversionUS(currency, COUNTRIES, fund)
	}
	temp0 := sub(a.credit_allowance, convertedFund)
	if lt(temp0, a.credit_limit_upper) {
		bk.errorPrint(ccLOC, WARNING_CODE, CC_REJ_TX, a.acct_no+"'s debt is too much (2x credit limit).\n")
		bk.txCancelled(a.acct_no)
		return CC_REJ_TX
	}
	a.credit_allowance = temp0
	if isNeg(a.credit_allowance) {
		bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE_NO_LINE,
			"Your debt is over the credit limit, but this transaction is allowed with fee\n")
		a.overCreditLimit = 1
	}
	switch typ {
	case PURCHASE:
		if currency != COUNTRIES {
			convertedRate := divv(cc.for_cur_tx_rate, bk.HUNDRED)
			rPlus := add(convertedRate, bk.ONE)
			temp := mul(convertedFund, convertedRate)
			a.credit_allowance = sub(a.credit_allowance, temp)
			convertedFund = mul(convertedFund, rPlus)
		}
		convertedFund = bk.roundToAvailableCoin(convertedFund, bk.CENT)
		a.purchase = add(a.purchase, convertedFund)
		typeStr = "PURCHASE"
	case CASH_ADVANCE:
		convertedRate := divv(cc.cash_adv_tx_rate, bk.HUNDRED)
		result := mul(convertedFund, convertedRate)
		result = maxv(result, cc.min_cash_adv_tx_fee)
		if currency != COUNTRIES {
			convertedRate = divv(cc.for_cur_tx_rate, bk.HUNDRED)
			result = add(result, mul(convertedFund, convertedRate))
		}
		result = bk.roundToAvailableCoin(result, bk.CENT)
		a.credit_allowance = sub(a.credit_allowance, result)
		convertedFund = add(result, convertedFund)
		a.cash_adv = add(a.cash_adv, convertedFund)
		typeStr = "CASH ADVANCE"
	case BALANCE_TRANSFER:
		convertedRate := divv(cc.bal_tx_rate, bk.HUNDRED)
		result := mul(convertedFund, convertedRate)
		result = maxv(result, cc.min_bal_tx_fee)
		result = minv(result, cc.max_bal_tx_fee)
		a.credit_allowance = sub(a.credit_allowance, result)
		convertedFund = add(convertedFund, result)
		a.bal_tx = add(a.bal_tx, convertedFund)
		if (a.intro_apr_bal_tx_end_date == -1) && (bk.counter < a.start_date+INIT_BAL_TX_PERIOD) {
			a.intro_apr_bal_tx_end_date = bk.counter + INIT_BAL_TX_PERIOD
		}
		typeStr = "BALANCE TRANSFER"
	default:
		bk.errorPrint(ccLOC, ERROR_CODE, CC_WRONG_TX_TYPE, "Wrong Transaction Type")
	}
	if a.overCreditLimit == 1 {
		a.purchase = add(a.purchase, cc.over_credit_limit_fee)
	}
	a.last_tx_unit = currency
	a.last_tx_type = typ
	a.last_tx = fund
	bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"###### New Transaction is posted: "+toStr(fund)+" "+unit+" with "+typeStr+
			" type has commited into "+a.acct_no+"'s CREDIT CARD account ######")
	return 0
}

func (bk *bank) getCCRate(r *ccRate, cc *ccConst, primeRate dec) int {
	r.purchase_apr = divv(divv(add(cc.purchase_bal_tx_extra, primeRate), bk.YEAR), bk.HUNDRED)
	r.bal_tx_apr = divv(divv(add(cc.purchase_bal_tx_extra, primeRate), bk.YEAR), bk.HUNDRED)
	r.cash_adv_apr = add(cc.cash_adv_extra, primeRate)
	r.cash_adv_apr = maxv(r.cash_adv_apr, cc.min_cash_adv)
	r.cash_adv_apr = divv(divv(r.cash_adv_apr, bk.YEAR), bk.HUNDRED)
	r.default_apr = add(cc.default_extra, primeRate)
	r.default_apr = maxv(r.default_apr, cc.min_default)
	r.default_apr = divv(divv(r.default_apr, bk.YEAR), bk.HUNDRED)
	r.init_bal_tx_apr = fromString("0.00") // INIT_BAL_TX_RATE
	r.init_bal_tx_apr = divv(divv(r.init_bal_tx_apr, bk.YEAR), bk.HUNDRED)
	return 0
}

func (bk *bank) creditCardAcctInit(a *ccAcct, acctNo string, autopay int, creditLimit dec, r *ccRate) int {
	a.acct_no = acctNo
	a.start_date = bk.counter
	a.pay_date = PAY_DATE
	a.close_date = bk.counter + PAY_DATE
	a.last_date = bk.counter + PAY_DATE + CC_GRACE_PERIOD
	a.status, a.underpaid, a.fullpaid, a.overdueHistory = 1, 0, 1, 0
	a.autopay = autopay
	a.intro_apr_bal_tx_end_date = INIT_BAL_TX_PERIOD
	a.purchase_statement, a.last_paid = zeroD, zeroD
	a.last_tx_unit, a.last_tx_type = 0, 0
	a.last_tx = zeroD
	a.credit_limit = creditLimit
	a.credit_limit_upper = minus(creditLimit)
	a.credit_allowance = creditLimit
	a.overCreditLimit = 0
	a.purchase_apr = r.purchase_apr
	a.cash_adv_apr = r.cash_adv_apr
	a.bal_tx_apr = r.init_bal_tx_apr
	a.default_apr = r.default_apr
	a.all_rate_default = 0
	a.purchase, a.posted_interest_purchase = zeroD, zeroD
	a.cash_adv, a.posted_interest_cash_adv = zeroD, zeroD
	a.bal_tx, a.posted_interest_bal_tx = zeroD, zeroD
	a.min_payment, a.purchase_total_paid = zeroD, zeroD
	return 0
}

func (bk *bank) creditCardAcctOpen(t, checking *htab, acctNo string, r *ccRate, cc *ccConst,
	creditLimit dec, autopay int) int {
	if len(acctNo) > MAX_ACCT_NAME_LENGTH {
		bk.errorPrint(ccLOC, ERROR_CODE, LENGTH_TOO_LONG, "account name is too long\n")
	}
	a := &ccAcct{}
	if !t.hadd(acctNo, a) {
		s := "[credit_card_acct_open] cannot add " + acctNo + "'s account\n"
		bk.errorPrint(ccLOC, WARNING_CODE, ADD_ACCT_FAIL, s)
		bk.emit(s)
		return ADD_ACCT_FAIL
	}
	bk.creditCardAcctInit(a, acctNo, autopay, creditLimit, r)
	if (autopay != 0) && !checking.hfind(acctNo) {
		bk.errorPrint(ccLOC, WARNING_CODE, ACCT_NOT_EXIST,
			"###### Checking Acct for "+acctNo+" doesn't exist. Credit Card account open fails ######\n")
		return ACCT_NOT_EXIST
	}
	bk.errorPrint(ccLOC, INFO_CODE, INFO_CODE_NO_LINE,
		"###### "+a.acct_no+" opens a CREDIT CARD account with credit limit as $"+toStr(creditLimit)+"######\n")
	return 0
}

func (bk *bank) ccConstInit(cc *ccConst) int {
	p := fromString
	cc.purchase_bal_tx_extra = p("5.49")
	cc.cash_adv_extra = p("14.99")
	cc.min_cash_adv = p("19.99")
	cc.min_default = p("28.99")
	cc.default_extra = p("23.99")
	cc.min_fin_charge = p("0.5")
	cc.for_cur_tx_rate = p("3.00")
	cc.cash_adv_tx_rate = p("3.00")
	cc.min_cash_adv_tx_fee = p("5.00")
	cc.min_payment_rate = p("10.00")
	cc.bal_tx_rate = p("3.00")
	cc.min_bal_tx_fee = p("5.00")
	cc.max_bal_tx_fee = p("250.00")
	cc.over_credit_limit_fee = p("39.00")
	cc.level1_fee = p("15.00")
	cc.level2_fee = p("29.00")
	cc.level3_fee = p("39.00")
	cc.level1_thr = p("100.00")
	cc.level2_thr = p("250.00")
	return 0
}

func (bk *bank) checkCCTxType(typ int) string {
	switch typ {
	case PURCHASE:
		return "Purchase"
	case CASH_ADVANCE:
		return "Cash Advance"
	case BALANCE_TRANSFER:
		return "Balance Transfer"
	}
	bk.errorPrint(ccLOC, ERROR_CODE, CC_WRONG_TX_TYPE, "Wrong Transaction Type")
	return ""
}

func (bk *bank) creditCardAcctInquiry(t *htab, acctNo string) int {
	if !t.hfind(acctNo) {
		return bk.notFound(ccLOC, acctNo)
	}
	a := t.hstuff().(*ccAcct)
	bk.emit("=============== CREDIT CARD ACCOUNT REPORT ==============\n")
	bk.emit("Account Type: Credit Card\n")
	bk.emit("Account Status: " + flag(a.status, "ACTIVE", "NON-ACTIVE") + "\n")
	bk.emit("Account Name: " + a.acct_no + "\n")
	bk.emit(f("Account Starts: %d\n", a.start_date))
	bk.emit(f("Account Paid: Every %d days\n", a.pay_date))
	bk.emit(f("Current Payment Due Date: Day %d\n", a.last_date))
	bk.emit("Underpaid Flag: " + flag(a.underpaid, "ON", "OFF") + "\n")
	bk.emit("Fullpaid Flag: " + flag(a.fullpaid, "ON", "OFF") + "\n")
	bk.emit(f("Overdue History: %d times\n", a.overdueHistory))
	if a.autopay == 1 {
		bk.emit("Debt is Paid Through " + a.acct_no + "'s Checking Account\n")
	} else {
		bk.emit("Debt is Paid by Cash by Due Date\n")
	}
	bk.emit(f("Introduction APR for balance transfer ends: Day %d\n", a.intro_apr_bal_tx_end_date))
	if !isZero(a.purchase_statement) {
		bk.emit("Monthly Statement for Purchase: $" + toStr(a.purchase_statement) + " \n")
	}
	if !isZero(a.min_payment) {
		bk.emit("Minimum Payment [Purchase] in This Statement: $" + toStr(a.min_payment) + "\n")
	}
	if !isZero(a.purchase_total_paid) {
		bk.emit("Statement Has Been Paid: $" + toStr(a.purchase_total_paid) + " \n")
	}
	if !isZero(a.last_paid) {
		bk.emit("The Last Payment: $" + toStr(a.last_paid) + "\n")
	}
	if !isZero(a.last_tx) {
		str1 := toStr(a.last_tx)
		str2 := bk.checkCCTxType(a.last_tx_type)
		unit := bk.checkName(a.last_tx_unit)
		bk.emit("The Last Transaction: [" + str2 + "] $" + str1 + " in " + unit + "\n")
	}
	bk.emit("Credit Limit: $" + toStr(a.credit_limit) + "\n")
	bk.emit("Current Credit Allowance: $" + toStr(a.credit_allowance) + "\n")
	bk.emit("Over Credit Limit Flag: " + flag(a.overCreditLimit, "ON", "OFF") + "\n")
	apr := func(r dec) string { return toStr(quantize(mul(mul(r, bk.YEAR), bk.HUNDRED), bk.ETWO)) }
	bk.emit("Purchase APR: " + apr(a.purchase_apr) + " (percent)\n")
	bk.emit("Cash Advance APR: " + apr(a.cash_adv_apr) + " (percent)\n")
	bk.emit("Balance Transfer APR: " + apr(a.bal_tx_apr) + " (percent)\n")
	bk.emit("Default APR: " + apr(a.default_apr) + " (percent)\n")
	bk.emit("Align to Default Rate Flag: " + flag(a.all_rate_default, "ON", "OFF") + "\n")
	bk.emit("Purchase Balance: $" + toStr(a.purchase) + "\n")
	bk.emit("Accumulated Interest [Purchase] since the Account Starts: $" + toStr(a.posted_interest_purchase) + "\n")
	bk.emit("Cash Advance Balance: $" + toStr(a.cash_adv) + "\n")
	bk.emit("Accumulated Interest [Cash Advance] since the Account Starts): $" + toStr(a.posted_interest_cash_adv) + "\n")
	bk.emit("Balance Transfer Balance: $" + toStr(a.bal_tx) + "\n")
	bk.emit("Accumulated Interest [Balance Transfer] since the Account Starts): $" + toStr(a.posted_interest_bal_tx) + "\n")
	bk.emit("=======================================================\n")
	return 0
}
