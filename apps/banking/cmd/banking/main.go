// SPDX-License-Identifier: GPL-3.0-or-later
//
// banking application benchmark — Go harness on the d128 backend (decimal128-go).
//
// Faithful transliteration of the UW-Madison GPL C program (apps/banking/c/src:
// main.c, basic_fcn.c, rate.c, deposit.c, mortgage.c, credit_card.c + the Jenkins
// hashtab/lookupa), via the proven Swift harness (apps/banking/swift).
// Derivative work → GPL; this repo only. Emulates a bank back-end (CD/checking
// deposits, mortgages, credit cards) over a stream of daily commands,
// reproducing the DEBUG stderr transcript byte-for-byte (external-gate; run with
// cwd = the banking data dir, which holds datafile/bin/).
//
// Round-mode discipline: the `set` context is ties-to-even throughout; the only
// non-ties-to-even windows (basic_fcn currency, mortgage closing) each wrap a
// single to-integral, so every other op is plain ties-to-even and the harness
// tracks the current mode only for toIntegral.
//
// Compute clock (D128_COMPUTE_ITERS=N): the main loop already rebuilds ALL state
// each iteration, so it is reused — iteration 0 captures the parsed commands,
// iterations 1..N replay them from memory with the transcript write off
// (format-but-not-write), timing only processing(), best of N.
package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"os"
	"strconv"
	"time"

	d "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/dec"
	harness "github.com/migueldecimal128/go-decimal128-app-benchmark/internal/harness"
)

type roundMode int

const (
	rne roundMode = iota
	rni
	rpi
	rtz
	rna
)

// fileReader is a file opened for sequential binary reads (C FILE* + fread),
// loaded whole at open so later reads never touch the disk.
type fileReader struct {
	data []byte
	pos  int
	dec  []dec // rate files: every 16-byte record, decoded once at open
}

func openReader(path string) *fileReader {
	b, err := os.ReadFile(path)
	if err != nil {
		return nil
	}
	return &fileReader{data: b}
}

// readBytes returns up to n bytes, or nil at end-of-file (fread == 0).
func (fr *fileReader) readBytes(n int) []byte {
	if fr.pos >= len(fr.data) {
		return nil
	}
	end := min(fr.pos+n, len(fr.data))
	s := fr.data[fr.pos:end]
	fr.pos = end
	return s
}

func (fr *fileReader) readInt32() (int, bool) {
	b := fr.readBytes(4)
	if len(b) != 4 {
		return 0, false
	}
	return int(int32(binary.LittleEndian.Uint32(b))), true
}

// predecode decodes every 16-byte record of a rate file once, when it is opened
// (outside the timed region), so the timed pass's reads decode nothing.
func (fr *fileReader) predecode() {
	fr.dec = make([]dec, len(fr.data)/16)
	for i := range fr.dec {
		fr.dec[i] = decodeD128(fr.data[16*i : 16*i+16])
	}
}

// readDec is fread of n 16-byte decimal128 records: the decoded values of the
// complete records read (0..n), mirroring fread's element count.
func (fr *fileReader) readDec(n int) []dec {
	i := fr.pos / 16
	k := min(n, len(fr.dec)-i)
	if k <= 0 {
		return nil
	}
	fr.pos += 16 * k
	return fr.dec[i : i+k]
}

type capCmd struct {
	ret int
	c   cmd
}

type bank struct {
	decodedRateFiles map[string][]dec // rate files, decoded once per run
	// transcript buffer (all C output goes to stderr; one write at the end).
	// emitEnabled gates the WRITE, not the format: callers build the string
	// first, so the silent compute pass still formats; emitSink consumes it.
	out         bytes.Buffer
	emitEnabled bool
	emitSink    uint64

	// command capture(1)/replay(2) for the compute pass (mirrors bank_next_cmd).
	cmdMode int
	capCmds []capCmd
	capIdx  int

	// global state (C file-scope globals)
	currentRound roundMode
	counter      int
	lineNum      int

	conversionRateFiles [COUNTRIES * 2]*fileReader
	depositRateFiles    [COUNTRIES + 1]*fileReader
	mortRateFiles       [4]*fileReader
	tcmRateFile         *fileReader
	primeRateFile       *fileReader

	conversionRate [2 * COUNTRIES][2]dec
	depositRate    []*depositRate
	mortRate       *mortRate
	tcmRate        dec
	primeRate      dec
	ccConst        *ccConst
	ccRate         *ccRate
	mortConst      *mortConst

	cdTable, checkingTable, mortgageTable, ccTable *htab

	// decimal constants (constdef)
	ONE, ZEROCONST, TWO, TWELVE, MTWO, ETWO, MTHREE  dec
	MFOUR, MFIVE, MNINE, TEN, MONTH, HUNDRED, YEAR   dec
	THOUSAND, CHECKING_RATE, NON_CUSTOMER_FIXED_RATE dec
	CENT                                             dec
}

var zeroD = d.MustParse("0") // ZERO64 == +0E+0

// --- decimal helpers --------------------------------------------------------
// Arithmetic calls the adapter (internal/dec) directly: a helper layer of its
// own on top would no longer inline (see internal/dec, "Cost").

func gt(x, y dec) bool { return y.Less(x) }
func lt(x, y dec) bool { return x.Less(y) }
func eq(x, y dec) bool { return x.Equal(y) }

func minus(x dec) dec { return x.Neg() }

func fromInt(n int) dec { return d.FromInt64(int64(n)) }

func toStr(x dec) string { return x.String() }
func isNeg(x dec) bool   { return x.Signbit() }
func isZero(x dec) bool  { return x.IsZero() }
func isPos(x dec) bool   { return !x.IsZero() && !x.Signbit() }
func isInf(x dec) bool   { return x.IsInf() }
func isNan(x dec) bool   { return x.IsNaN() }

var one128 = d.FromInt64(1)

// power is POWER(1+r, -n): r = X - 1 (ties-to-even, exact for the rates here),
// n toward zero, then IEEE 754 compound (1+r)^n rounded ties-to-even.
func power(x, y dec) dec {
	r := x.Sub(one128)
	n := int(d.TowardZero.ToInt64(y))
	return r.Compound(n)
}

func (bk *bank) toIntegral(x dec) dec {
	switch bk.currentRound {
	case rni:
		return d.TowardNegative.RoundToIntegral(x)
	case rpi:
		return d.TowardPositive.RoundToIntegral(x)
	case rtz:
		return d.TowardZero.RoundToIntegral(x)
	case rna:
		return d.TiesToAway.RoundToIntegral(x)
	default:
		return d.TiesToEven.RoundToIntegral(x)
	}
}

var f = fmt.Sprintf

func (bk *bank) emit(s string) {
	if bk.emitEnabled {
		bk.out.WriteString(s)
	} else {
		bk.emitSink += uint64(len(s))
	}
}

func (bk *bank) flushAndExit(code int) {
	os.Stderr.Write(bk.out.Bytes())
	os.Exit(code)
}

var errorMsgs = map[int]string{
	FILE_NOT_EXIST:        "FILE/DIRECTORY NOT EXIST",
	ACCT_NOT_EXIST:        "ACCT NOT EXIST",
	ACCT_LENGTH_FAIL:      "ACCT LENGTH FAIL",
	ACCT_TYPE_WRONG:       "ACCT TYPE WRONG",
	ACCT_EXIST_AND_ACTIVE: "ACCT EXIST AND ACTIVE",
	NOT_ENOUGH_BALANCE:    "NOT ENOUGH BALANCE",
	CC_WRONG_TX_TYPE:      "CC WRONG TRANSACTION TYPE",
	CC_REJ_PAY:            "CC PAYMENT REJECTED",
	CC_REJ_TX:             "CC TRANSACTION REJECTED",
	MORT_ACCT_OPEN_REJ:    "MORTGAGE ACCT OPENING REJECTED",
	WRONG_ACTION:          "WRONG ACTION TYPE (UNIVERSAL)",
	CD_NO_WIRETX:          "NO WIRETRANSFER ON CD ACCT",
	CUR_NOT_SUPPORT:       "THIS CURRENCY IS NOT SUPPORTED",
	CD_CK_NO_MONEY:        "CHECK/CD NO ENOUH MONEY",
	CD_CK_BIG_ERROR:       "BIG ERROR OCCURS IN CD/CK CONVERSION",
	CD_BAD_PERIOD:         "WRONG CD PERIOD",
	WRONG_CURRENCY:        "WRONG CURRENCY UNIT",
	LENGTH_TOO_LONG:       "ACCOUNT NAME TOO LONG",
	MORT_TYPE_WRONG:       "WRONG MORTGAGE TYPE",
	BAD_INPUT:             "BAD INPUTS",
	NOT_ACTIVE:            "ACCOUNT NOT ACTIVE",
	MORT_ACCT_OVERDUE:     "ACCOUNT OVERDUE",
	MORT_DEBT_PAID:        "MORTGAGE DEBT PAID",
	MORT_REFINANCE_FAIL:   "MORTGAGE REFINANCE FAIL",
	ADD_ACCT_FAIL:         "ADD ACCOUNT FAIL",
}

// errorPrint (basic_fcn.c) — DEBUG build; fp is always the stderr buffer.
func (bk *bank) errorPrint(location string, level, errorCode int, msg string) {
	myMsg, ok := errorMsgs[errorCode]
	if !ok {
		myMsg = "UNKNOWN PROBLEM\n"
	}
	switch level {
	case WARNING_CODE:
		bk.emit("[WARNING] " + location + ":[" + myMsg + "]:" + msg + "\n")
	case ERROR_CODE:
		bk.emit("[ERROR] " + location + ":[" + myMsg + "]:" + msg + "\n")
		bk.flushAndExit(level)
	default:
		if errorCode == INFO_CODE_NO_LINE {
			bk.emit("[INFO] " + msg + "\n")
		} else {
			bk.emit("[INFO] " + location + ":" + msg + "\n")
		}
	}
}

// --- basic_fcn helpers ----------------------------------------------------------

var currencyNames = [...]string{"Australia dollar", "Chinese yuan", "European euro",
	"U.K. Sterling", "Hong Kong dollar", "Japan yen", "New Zealand dollar",
	"Singapore dollar", "New Taiwan dollar", "United State dollar"}

func (bk *bank) checkName(v int) string {
	if (v >= 0) && (v < len(currencyNames)) {
		return currencyNames[v]
	}
	bk.emit(f("currency unit is %d\n", v))
	bk.errorPrint("basic_fcn.c", ERROR_CODE, WRONG_CURRENCY, "WRONG CURRENCY UNIT")
	return ""
}

// roundToAvailableCoin rounds to the closest available coin in the target NCU.
func (bk *bank) roundToAvailableCoin(inputAmt, minUnit dec) dec {
	converted := zeroD
	if !isZero(inputAmt) {
		tempGT := gt(minUnit, bk.ONE)
		temp := inputAmt
		if tempGT {
			temp = inputAmt.Quo(bk.TEN)
		}
		bk.currentRound = rni
		newInput := bk.toIntegral(temp)
		bk.currentRound = rne
		temp = temp.Sub(newInput)
		if !isZero(temp) {
			if tempGT {
				temp = temp.Mul(bk.TEN)
				newInput = newInput.Mul(bk.TEN)
			}
			temp = temp.Quo(minUnit)
			temp = bk.toIntegral(temp)
			temp = temp.Mul(minUnit)
			converted = temp.Add(newInput)
		} else {
			converted = inputAmt
		}
	}
	return converted
}

func (bk *bank) checkGEZero(v dec) int {
	if isNan(v) || isInf(v) || isNeg(v) || isZero(v) {
		return 0
	}
	return 1
}

func (bk *bank) checkLEOne(v dec) int {
	if isNan(v) || isInf(v) {
		return 0
	}
	if lt(v, bk.TWO) {
		return 1
	}
	return 0
}

// constdef (basic_fcn.c): set the shared constants + mort_const + tables.
func (bk *bank) constdef() {
	p := d.MustParse
	bk.ONE, bk.TWO, bk.ZEROCONST = p("1.0"), p("2.0"), p("0.0")
	bk.TWELVE, bk.MTWO, bk.ETWO = p("12"), p("-2"), p("1E-2")
	bk.MTHREE, bk.MFOUR, bk.MFIVE = p("-3"), p("-4"), p("-5")
	bk.MNINE, bk.TEN, bk.MONTH = p("-9"), p("10"), p("30")
	bk.HUNDRED, bk.THOUSAND, bk.YEAR = p("100"), p("1000"), p("360")
	bk.CHECKING_RATE, bk.NON_CUSTOMER_FIXED_RATE = p("0.5"), p("0.02")
	bk.CENT = p("0.01")
	bk.mortConst = &mortConst{
		hundredThousand:   p("100000"),
		title_search:      p("85"),
		title_examination: p("65"),
		closing:           p("100"),
		recording:         p("50"),

		title_insurance_rate_1:               p("0.001"),
		title_insurance_rate_2:               p("5.00"),
		title_insurance_base:                 p("575.00"),
		documentary_stamp_tax_on_note_rate_1: p("0.01"),
		documentary_stamp_tax_on_note_rate_2: p("0.35"),
		intagible_tax_on_note:                p("0.002"),
		lender_title_insurance_policy:        p("25"),
		alta_title_endorsements:              p("50"),
		title_endorsement_extra:              p("25.00"),
		title_endorsement_rate:               p("0.1"),
		origination:                          p("0.01"),
		appraisal:                            p("300.00"),
		credit_report:                        p("65"),
		additional_closing_fees:              p("0.005"),
		closing_cost_other:                   p("1075.00"),
		premature_close_penalty_year_0:       p("0.01"),
		premature_close_penalty_year_1:       p("0.075"),
		premature_close_penalty_year_2:       p("0.005"),
		premature_close_penalty_year_3:       p("0.025"),
		overdue_surcharge_rate:               p("0.05"),
		min_overdue_yr_penalty_rate:          p("10"),
		min_overdue_yr_penalty_point:         p("1"),
		min_overdue_yr_penalty_margin:        p("5"),
	}
	bk.initCoin() // rate.c
	bk.ccConst = &ccConst{}
	bk.ccConstInit(bk.ccConst) // credit_card.c
}

// decodeD128 decodes 16 little-endian BID bytes. The command + rate data files
// read here are the BID-encoded .bid128 variants (value-equal to the DPD .b128
// the C d128 arm reads), so the golden is identical.
func decodeD128(b []byte) dec {
	hi, lo := harness.BID128(b, 0)
	return d.FromBID128(hi, lo)
}

// parseCmd is parse_cmd (basic_fcn.c, DEC_D128 branch); ok=false at EOF.
func (bk *bank) parseCmd(fr *fileReader, c *cmd) (int, bool) {
	nameBytes := fr.readBytes(MAX_ACCT_NAME_LENGTH)
	if len(nameBytes) < MAX_ACCT_NAME_LENGTH {
		return 0, false
	}
	// C string: bytes up to the first NUL (truncated to 15 if no NUL in 16).
	n := bytes.IndexByte(nameBytes, 0)
	if n < 0 {
		n = 15
	}
	c.acct_no = string(nameBytes[:n])
	fb := fr.readBytes(16)
	if len(fb) != 16 {
		return 0, false
	}
	c.fund = decodeD128(fb)
	sb := fr.readBytes(16)
	if len(sb) != 16 {
		return 0, false
	}
	c.str1 = decodeD128(sb)
	var ok bool
	if c.action, ok = fr.readInt32(); !ok {
		return 0, false
	}
	if c.length, ok = fr.readInt32(); !ok {
		return 0, false
	}
	if c.acct_type, ok = fr.readInt32(); !ok {
		return 0, false
	}
	if c.sel, ok = fr.readInt32(); !ok {
		return 0, false
	}
	return bk.checkCmd(c), true
}

// checkCmd is the per-command work after the read and decode (the DEBUG echo,
// input validation and its warnings), split out of parseCmd so the replayed
// commands of the timed pass do it too. Returns parseCmd's result.
func (bk *bank) checkCmd(c *cmd) int {
	// #ifdef DEBUG cmd echo
	bk.emit(f("cmd: %s %d %s %d %d %d %s\n", c.acct_no, c.action, toStr(c.fund),
		c.acct_type, c.length, c.sel, toStr(c.str1)))
	if isNeg(c.fund) || isNeg(c.str1) || (c.action < 0) || (c.length < 0) ||
		(c.acct_type < 0) || (c.sel < 0) {
		bk.errorPrint("basic_fcn.c", ERROR_CODE, BAD_INPUT,
			f("Negative input in the cmd file at line %d\n", bk.lineNum))
	}
	if (c.action == MORT_OPEN) && lt(c.fund, c.str1) {
		bk.errorPrint("basic_fcn.c", WARNING_CODE, MORT_ACCT_OPEN_REJ,
			"This action for "+c.acct_no+" is not valid because the "+
				"                          downpayment is greater than the loan\n")
		return MORT_ACCT_OPEN_REJ
	}
	return 0
}

// nextCmd is bank_next_cmd: capture/replay wrapper over parseCmd.
func (bk *bank) nextCmd(fr *fileReader, c *cmd) (int, bool) {
	if bk.cmdMode == 2 { // replay a parsed command from memory
		if bk.capIdx >= len(bk.capCmds) {
			return 0, false
		}
		s := bk.capCmds[bk.capIdx]
		bk.capIdx++
		*c = s.c
		return bk.checkCmd(c), true // echo + validation, as parseCmd does
	}
	ret, ok := bk.parseCmd(fr, c)
	if !ok {
		return 0, false
	}
	if bk.cmdMode == 1 {
		bk.capCmds = append(bk.capCmds, capCmd{ret, *c})
	}
	return ret, true
}

func (bk *bank) openOrDie(path, loc string) *fileReader {
	fr := openReader(path)
	if fr == nil {
		bk.errorPrint(loc, ERROR_CODE, FILE_NOT_EXIST, "Error: file '"+path+"' not found\n")
		bk.flushAndExit(255)
	}
	// Only rate files are opened here: decode once per run (cached by path).
	if dec, ok := bk.decodedRateFiles[path]; ok {
		fr.dec = dec
	} else {
		fr.predecode()
		bk.decodedRateFiles[path] = fr.dec
	}
	return fr
}

// myprint is myprintf → stdout (not part of the stderr golden gate).
func myprint(s string) { os.Stdout.WriteString(s) }

// initialization (main.c)
func (bk *bank) initialization() {
	bk.conversionRate = [2 * COUNTRIES][2]dec{}
	bk.depositRate = make([]*depositRate, COUNTRIES+1)
	for i := range bk.depositRate {
		bk.depositRate[i] = &depositRate{}
	}
	bk.mortRate = &mortRate{}
	bk.ccRate = &ccRate{}
	bk.constdef()
	bk.openConversionRateFile()
	bk.updateConversionTable()
	bk.openDepositRateFile()
	bk.updateDepositRateTable()
	bk.openMortgageRateFile()
	bk.updateMortgageRate()
	bk.updateTCMRateFile()
	bk.updatePrimeRateFile()
	bk.getCCRate(bk.ccRate, bk.ccConst, bk.primeRate)
}

// processing (main.c): the command dispatch loop.
func (bk *bank) processing(cmdFile *fileReader) {
	var c cmd
	bk.lineNum = 1
	for {
		ret, ok := bk.nextCmd(cmdFile, &c)
		if !ok {
			return
		}
		if ret < 0 {
			continue
		}
		bk.lineNum++
		switch {
		case c.action == NEXT_DAY:
			bk.info("Processing Deposit Accounts\n")
			bk.info("Updating Prime Rate Table\n")
			primeRateChange := bk.updatePrimeRateFile()
			if primeRateChange == 1 {
				bk.getCCRate(bk.ccRate, bk.ccConst, bk.primeRate)
			}
			bk.info("Updating Currency Conversion Rate Table\n")
			bk.updateConversionTable()
			bk.info("Updating CD Rate Table\n")
			bk.updateDepositRateTable()
			bk.info("Updating Mortgage Rate Table\n")
			bk.updateMortgageRate()
			bk.info("Updating TCM Rate Table\n")
			bk.updateTCMRateFile()
			bk.endOfTheDayDeposit(bk.cdTable, bk.checkingTable)
			bk.info("Processing Mortgage Accounts\n")
			bk.endOfTheDayMortgage(bk.mortgageTable, bk.checkingTable, bk.tcmRate)
			bk.info("Processing Credit Card Accounts\n")
			bk.endOfTheDayCreditCard(bk.ccTable, bk.checkingTable, primeRateChange)
			bk.counter++
			bk.emit(f("End of Day %d\n", bk.counter))
		case c.acct_type == CD:
			bk.dispatchCD(&c)
		case c.acct_type == CHECKING:
			bk.dispatchChecking(&c)
		case (c.acct_type >= MORTGAGE) && (c.acct_type != CREDIT_CARD):
			bk.dispatchMortgage(&c)
		case c.acct_type == CREDIT_CARD:
			bk.dispatchCreditCard(&c)
		default:
			bk.errorPrint("main.c", WARNING_CODE, WRONG_ACTION,
				"###### Wrong Action. This cmd is skipped ######")
		}
	}
}

// info is errorPrint("main.c", INFO_CODE, INFO_CODE, msg).
func (bk *bank) info(msg string) { bk.errorPrint("main.c", INFO_CODE, INFO_CODE, msg) }

func (bk *bank) wrongAction() {
	bk.emit("Wrong Action\n")
	bk.flushAndExit(255)
}

func (bk *bank) dispatchCD(c *cmd) {
	switch c.action {
	case CD_CK_OPEN:
		bk.info(c.acct_no + " opens a CD account\n")
		bk.depositAcctOpen(bk.cdTable, c.acct_no, c.fund, c.length, CD, c.sel, PAY_DATE, bk.depositRate[c.sel])
	case CD_CK_INQUIRY:
		bk.info(c.acct_no + " inquiries his/her CD account\n")
		bk.depositAcctInquiry(bk.cdTable, c.acct_no)
	case CD_SET_CLOSE:
		bk.info("Mark Close " + c.acct_no + "'s CD account\n")
		bk.depositMarkClose(bk.cdTable, c.acct_no)
	case CD_EXTEND:
		bk.info("Mark Extend " + c.acct_no + "'s CD account\n")
		bk.depositMarkExtend(bk.cdTable, c.acct_no)
	case CD_CK_CLOSE:
		bk.info("Close " + c.acct_no + "'s CD account\n")
		bk.depositAcctClose(bk.cdTable, c.acct_no, CD, bk.depositRate)
	case CD_CK_DEPOSIT:
		bk.info("Deposit to " + c.acct_no + "'s CD account\n")
		bk.moneyDeposit(bk.cdTable, c.acct_no, c.fund, c.sel)
	case CD_CK_WITHDRAWL:
		bk.info("Withdrawl to " + c.acct_no + "'s CD account\n")
		bk.moneyWithdrawl(bk.cdTable, c.acct_no, c.sel, c.fund, bk.depositRate)
	case WIRETRANSFER:
		bk.errorPrint("main.c", WARNING_CODE, CD_NO_WIRETX, "Wire transfer not allow in CD account\n")
	default:
		bk.wrongAction()
	}
}

func (bk *bank) dispatchChecking(c *cmd) {
	switch c.action {
	case CD_CK_OPEN:
		bk.info(c.acct_no + " opens a CHECKING account\n")
		bk.depositAcctOpen(bk.checkingTable, c.acct_no, c.fund, c.length, CHECKING, c.sel, PAY_DATE, bk.depositRate[c.sel])
	case CD_CK_INQUIRY:
		bk.info(c.acct_no + " inquiries his/her CHECKING account\n")
		bk.depositAcctInquiry(bk.checkingTable, c.acct_no)
	case CD_SET_CLOSE, CD_EXTEND:
		bk.errorPrint("main.c", WARNING_CODE, ACCT_TYPE_WRONG,
			"SET Close or Extend is not supported in Checking account")
	case CD_CK_CLOSE:
		bk.info("Close " + c.acct_no + "'s CD account\n")
		bk.depositAcctClose(bk.checkingTable, c.acct_no, CHECKING, nil)
	case CD_CK_DEPOSIT:
		bk.info("Deposit to " + c.acct_no + "'s CHECKING account\n")
		bk.moneyDeposit(bk.checkingTable, c.acct_no, c.fund, c.sel)
	case CD_CK_WITHDRAWL:
		bk.info("Withdrawl to " + c.acct_no + "'s CHECKING account\n")
		bk.moneyWithdrawl(bk.checkingTable, c.acct_no, c.sel, c.fund, bk.depositRate)
	case WIRETRANSFER:
		bk.info(c.acct_no + " performs Wire-transfer\n")
		bk.wireTransfer(bk.checkingTable, c.sel/10, c.acct_no, c.sel%10, c.fund)
	default:
		bk.wrongAction()
	}
}

func (bk *bank) dispatchMortgage(c *cmd) {
	switch c.action {
	case MORT_OPEN:
		bk.info(c.acct_no + " opens a mortgage account\n")
		bk.mortgageAcctOpen(bk.mortgageTable, bk.checkingTable, bk.mortRate, c.length,
			c.fund, c.str1, c.acct_no, c.acct_type, c.sel)
	case MORT_INQUIRY:
		bk.info(c.acct_no + " inquiries his/her mortgage account\n")
		bk.mortgageAcctInquiry(bk.mortgageTable, c.acct_no)
	case MORT_REFINANCE:
		bk.info(c.acct_no + " refinances his/her mortgage account\n")
		bk.mortgageAcctRefinance(bk.mortgageTable, bk.checkingTable, bk.mortRate, c.length,
			c.acct_no, c.acct_type, c.sel)
	case MORT_CLOSE:
		bk.info(c.acct_no + " closes his/her mortgage account\n")
		bk.mortgageAcctClose(bk.mortgageTable, c.acct_no)
	case MORT_PAY:
		bk.info(c.acct_no + " pays his/her mortgage account\n")
		bk.mortgageAcctPay(bk.mortgageTable, c.acct_no)
	default:
		bk.info(c.acct_no + " inquiries the schedule table\n")
		c.acct_type = c.acct_type - MORTGAGE
		bk.scheduleTable(c.fund, c.length, bk.mortRate, c.acct_type)
	}
}

func (bk *bank) dispatchCreditCard(c *cmd) {
	bk.info("Handling " + c.acct_no + "'s Credit Card account\n")
	switch c.action {
	case CC_OPEN:
		bk.creditCardAcctOpen(bk.ccTable, bk.checkingTable, c.acct_no, bk.ccRate, bk.ccConst, c.fund, c.sel)
	case CC_INQUIRY:
		bk.info(c.acct_no + " inquiries his/her Credit Card account\n")
		bk.creditCardAcctInquiry(bk.ccTable, c.acct_no)
	case CC_CLOSE:
		bk.info(c.acct_no + " closes his/her Credit Card account\n")
		bk.creditCardAcctClose(bk.ccTable, c.acct_no)
	case CC_PAY:
		bk.info(c.acct_no + " pays his/her Credit Card account\n")
		bk.creditCardAcctPay(bk.ccTable, c.acct_no, c.sel, c.fund)
	default:
		bk.info("A transaction is posted to " + c.acct_no + " Credit Card account\n")
		bk.creditCardAcctDebt(bk.ccTable, c.acct_no, c.sel/10, bk.ccConst, c.sel%10, c.fund)
	}
}

// drain lists and deletes every account in t (the end-of-run report).
func (bk *bank) drain(t *htab, name func(any) string) {
	for t.count != 0 {
		bk.emit(name(t.hstuff()) + ", ")
		t.hdel()
	}
}

func main() { os.Exit((&bank{emitEnabled: true, decodedRateFiles: map[string][]dec{}}).run()) }

// run is main.c's top level.
func (bk *bank) run() int {
	harness.AnnounceLibrary(d.Name)
	if len(os.Args) != 2 {
		bk.emit("[USAGE] cmd cmd_file\n")
		bk.flushAndExit(255)
	}
	bankingIters := 1
	if v, err := strconv.Atoi(os.Getenv("BANKING_ITERS")); (err == nil) && (v > 0) {
		bankingIters = v
	}
	computeIters := harness.ComputeIters()
	totalIters := bankingIters
	if computeIters > 0 {
		totalIters = computeIters + 1
		bk.cmdMode = 1 // capture parsed commands during iteration 0
	}
	best := int64(-1)

	depName := func(s any) string { return s.(*depositAcct).acct_no }
	for iter := 0; iter < totalIters; iter++ {
		cmdFile := openReader(os.Args[1])
		if cmdFile == nil {
			bk.errorPrint("main.c", ERROR_CODE, FILE_NOT_EXIST, "Error: file '"+os.Args[1]+"' not found\n")
			return 0
		}
		bk.tcmRateFile = bk.openOrDie("datafile/bin/1YearTCM.txt.bid128", "main.c")
		bk.primeRateFile = bk.openOrDie("datafile/bin/Prime_rate.txt.bid128", "main.c")

		bk.currentRound = rne // CONTEXTDEFAULT
		bk.counter = 0
		bk.emit("Start of the Banking benchmark\n")
		bk.primeRate = zeroD // ZERO64(prime_rate)
		bk.initialization()

		bk.cdTable, bk.checkingTable = newHtab(8), newHtab(8)
		bk.mortgageTable, bk.ccTable = newHtab(8), newHtab(8)

		if computeIters > 0 {
			bk.capIdx = 0 // rewind the captured command cursor
		}
		t0 := time.Now()
		bk.processing(cmdFile)
		dt := time.Since(t0).Nanoseconds()
		if computeIters > 0 {
			if iter == 0 {
				bk.cmdMode, bk.emitEnabled = 2, false // captured -> replay + silent
			} else {
				harness.AddSample(dt)
				if (best < 0) || (dt < best) {
					best = dt
				}
			}
		}

		bk.emit(f("The number of CD accounts: %d\n", bk.cdTable.count))
		bk.emit("The active CD accounts includes:\n")
		bk.drain(bk.cdTable, depName)
		bk.emit(f("\nThe number of Checking accounts: %d\n", bk.checkingTable.count))
		bk.emit("The active Checking accounts includes:")
		bk.drain(bk.checkingTable, depName)
		bk.emit(f("\nThe number of Mortgage accounts: %d\n", bk.mortgageTable.count))
		bk.emit("The active Mortgage accounts includes:")
		bk.drain(bk.mortgageTable, func(s any) string { return s.(*mortAcct).acct_no })
		bk.emit(f("\nThe number of Credit Card accounts: %d\n", bk.ccTable.count))
		bk.emit("The active Credit Card accounts includes:")
		bk.drain(bk.ccTable, func(s any) string { return s.(*ccAcct).acct_no })
		bk.emit("\n")
		bk.emit("End of the Banking benchmark\n")
	}

	if computeIters > 0 {
		// This run's stderr is not gated; the sink makes the formatted output
		// observable so the formatting can't be dropped.
		fmt.Fprintf(os.Stderr, "compute-sink %d\n", bk.emitSink)
		recs := max(len(bk.capCmds), 1)
		harness.ReportCompute(best, recs, 1) // records == 643; pass >= ~400 us: no batching
	} else {
		os.Stderr.Write(bk.out.Bytes())
	}
	return 0
}
