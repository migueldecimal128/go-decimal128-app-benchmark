// SPDX-License-Identifier: GPL-3.0-or-later
//
// Data model — the C structs from const.h (via the Swift Model.swift). Pointers
// to these structs are what the hash tables hold, so the C's pass-by-pointer
// mutation maps directly. Every decimal field starts as Go's zero value, which
// for decimal128-go is bit-identical to parse("0") (= the C ZERO64 clears).
package main

import d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/dec"

type dec = d.Decimal128

// account-type / action constants (const.h + *_const.h)
const (
	// account type
	CHECKING    = 0
	CD          = 1
	MORTGAGE    = 3
	CREDIT_CARD = 7
	COUNTRIES   = 9
	PAY_DATE    = 30

	MAX_ACCT_NAME_LENGTH = 16
	GRACE_PERIOD         = 10

	// deposit actions
	NEXT_DAY        = 0
	CD_CK_OPEN      = 1
	CD_CK_INQUIRY   = 2
	CD_SET_CLOSE    = 3
	CD_CK_CLOSE     = 4
	CD_CK_DEPOSIT   = 5
	CD_CK_WITHDRAWL = 6
	CD_EXTEND       = 7
	WIRETRANSFER    = 8

	// mortgage types
	FRM15YEAR = 3
	FRM30YEAR = 4
	ARM1YEAR  = 5
	ARM51YEAR = 6

	// mortgage actions
	MORT_OPEN      = 1
	MORT_INQUIRY   = 2
	MORT_REFINANCE = 3
	MORT_CLOSE     = 4
	MORT_PAY       = 5

	// credit card
	PURCHASE           = 0
	CASH_ADVANCE       = 1
	BALANCE_TRANSFER   = 2
	CC_OPEN            = 1
	CC_INQUIRY         = 2
	CC_CLOSE           = 3
	CC_PAY             = 4
	FULL_PAY           = 1
	STATEMENT_PAY      = 2
	MIN_PAY            = 3
	INIT_BAL_TX_PERIOD = 360
	CC_GRACE_PERIOD    = 20

	// error / message codes (const.h)
	INFO_CODE             = 0
	WARNING_CODE          = 1
	ERROR_CODE            = 2
	INFO_CODE_NO_LINE     = 1
	FILE_NOT_EXIST        = -2
	ACCT_NOT_EXIST        = -7
	ACCT_LENGTH_FAIL      = -8
	ACCT_TYPE_WRONG       = -9
	ACCT_EXIST_AND_ACTIVE = -10
	NOT_ENOUGH_BALANCE    = -11
	CC_WRONG_TX_TYPE      = -12
	CC_REJ_PAY            = -13
	CC_REJ_TX             = -14
	MORT_ACCT_OPEN_REJ    = -15
	WRONG_ACTION          = -16
	CD_NO_WIRETX          = -17
	CUR_NOT_SUPPORT       = -18
	CD_CK_NO_MONEY        = -19
	CD_CK_BIG_ERROR       = -20
	CD_BAD_PERIOD         = -21
	WRONG_CURRENCY        = -22
	LENGTH_TOO_LONG       = -23
	MORT_TYPE_WRONG       = -24
	BAD_INPUT             = -25
	NOT_ACTIVE            = -26
	MORT_ACCT_OVERDUE     = -27
	MORT_DEBT_PAID        = -28
	MORT_REFINANCE_FAIL   = -29
	ADD_ACCT_FAIL         = -34
)

type cmd struct {
	acct_no                        string
	fund, str1                     dec
	action, length, acct_type, sel int
}

type depositAcct struct {
	acct_no                                    string
	period, open_date, maturity_date, pay_date int
	set_close, extend, acct_type, currency     int
	last_withdrawl_unit, last_deposit_unit     int
	rate, interest, unposted_interest          dec
	last_statement, last_deposit               dec
	last_withdrawl, current_balance            dec
}

type mortAcct struct {
	acct_no                                    string
	period, open_date, maturity_date, pay_date int
	totalTerm, currentTerm, status, acct_type  int
	overdue, debt_paid, auto_pay               int
	start_rate, rate, loan, monthly            dec
	accu_interest, remaining_principal, point  dec
	downPayment, closingCost, margin           dec
}

type ccAcct struct {
	acct_no                                          string
	start_date, pay_date, close_date, last_date      int
	status, underpaid, fullpaid, overdueHistory      int
	autopay, intro_apr_bal_tx_end_date, last_tx_unit int
	last_tx_type, overCreditLimit, all_rate_default  int
	purchase_statement, purchase_total_paid          dec
	last_paid, last_tx, credit_limit                 dec
	credit_limit_upper, credit_allowance             dec
	purchase_apr, cash_adv_apr, bal_tx_apr           dec
	default_apr, purchase, posted_interest_purchase  dec
	cash_adv, posted_interest_cash_adv               dec
	bal_tx, posted_interest_bal_tx, min_payment      dec
}

type depositRate struct{ checking, one, three, six dec }

type mortRate struct {
	frm15, frm15Point, frm30, frm30Point dec
	arm1, arm1Point, arm1Margin          dec
	arm51, arm51Point, arm51Margin       dec
}

type ccRate struct {
	purchase_apr, cash_adv_apr, bal_tx_apr, default_apr, init_bal_tx_apr dec
}

type mortConst struct {
	hundredThousand, title_search, title_examination, closing, recording dec
	title_insurance_rate_1, title_insurance_rate_2, title_insurance_base dec
	documentary_stamp_tax_on_note_rate_1                                 dec
	documentary_stamp_tax_on_note_rate_2                                 dec
	intagible_tax_on_note, lender_title_insurance_policy                 dec
	alta_title_endorsements, title_endorsement_extra                     dec
	title_endorsement_rate, origination, appraisal, credit_report        dec
	additional_closing_fees, closing_cost_other                          dec
	premature_close_penalty_year_0, premature_close_penalty_year_1       dec
	premature_close_penalty_year_2, premature_close_penalty_year_3       dec
	overdue_surcharge_rate, min_overdue_yr_penalty_rate                  dec
	min_overdue_yr_penalty_point, min_overdue_yr_penalty_margin          dec
}

type ccConst struct {
	purchase_bal_tx_extra, cash_adv_extra, min_cash_adv, min_default   dec
	default_extra, min_fin_charge, for_cur_tx_rate, cash_adv_tx_rate   dec
	min_cash_adv_tx_fee, bal_tx_rate, min_payment_rate, min_bal_tx_fee dec
	max_bal_tx_fee, over_credit_limit_fee                              dec
	level1_fee, level2_fee, level3_fee, level1_thr, level2_thr         dec
}
