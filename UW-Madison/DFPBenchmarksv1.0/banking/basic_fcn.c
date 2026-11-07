// =============================================================================
//  filename
//  Description of file
// =============================================================================
//  Original Author : Liang-Kai Wang, 5/2007
//  Maintainer:       
// =============================================================================
//  LONG DESCRIPTION
//  For details, please refer to the following paper:
//
//      Wang, Tsen, Jhalani, Schulte  "Benchmarks and Performance Analysis
//      of Decimal Floating-Point Applications" from IEEE International
//      Conference on Computer Design, Oct 2007.
//
//  Please see the README for more info.
// =============================================================================
//  Copyright (C) 2008 MESA LAB - University of Wisconsin
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
// =============================================================================
// VERSION AND CVS INFO
// Id: $Id: basic_fcn.c,v 1.14 2009-05-18 03:07:53 mjanders Exp $
// Date: $Date: 2009-05-18 03:07:53 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// Basic_fcn.c
// Decimal Floating-Point Benchmark
// This file is to define the common functions used throughout the benchmark.
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.14 $
#include "const.h" 
#include "basic_fcn.h"
#include "decfcn.h"
#include "rdtsc.h"

/////////////////////////////////////////////////////
//GLOBAL EXTERNAL VARIABLES
//The constants used in the microbenchmark
// M* means "minus *". E.g. MTWO = -2
//////////////////////////////////////////////////////
dec_t ONE, ZEROCONST, TWO, TWELVE, MTWO, ETWO, MTHREE, MFOUR, MFIVE, MNINE,
  TEN, MONTH, HUNDRED, YEAR, THOUSAND, CHECKING_RATE, NON_CUSTOMER_FIXED_RATE,
  CENT;
mort_const_t mort_const;
extern int counter;
extern context_t *setp;

#if defined(NATIVE_GCC_BID)
extern _IDEC_flags flags;
#endif

// COUNTING MICHAEL!!!
extern unsigned long long add_cycles;
extern unsigned long long mul_cycles;
extern unsigned long long quant_cycles;
extern unsigned long long zero_cycles;
extern unsigned long long div_cycles;
extern unsigned long long sqrt_cycles;
extern unsigned long long sub_cycles;
extern unsigned long long copy_cycles;
extern unsigned long long int_cycles;
extern unsigned long long max_cycles;
extern unsigned long long minus_cycles;

extern unsigned long long add_calls;
extern unsigned long long mul_calls;
extern unsigned long long quant_calls;
extern unsigned long long zero_calls;
extern unsigned long long div_calls;
extern unsigned long long sqrt_calls;
extern unsigned long long sub_calls;
extern unsigned long long copy_calls;
extern unsigned long long int_calls;
extern unsigned long long max_calls;
extern unsigned long long minus_calls;

extern unsigned long long add_rounded;
extern unsigned long long int_rounded;
extern unsigned long long mul_rounded;
extern unsigned long long div_rounded;
extern unsigned long long rsc_rounded;

extern unsigned long long count_begin_func;
extern unsigned long long count_end_func;
extern unsigned long long count_overhead;
// END COUNTING MICHAEL!!!

extern int line_num;

void errorPrint(const char *location, int level, 
		int errorCode, FILE *fp, const char *msg){
  char myMsg[MSG_LENGTH];
  switch(errorCode){
  case FILE_NOT_EXIST:
    mystrcpy(myMsg, "FILE/DIRECTORY NOT EXIST");
    break;
  case FILE_WRITE_DENY:
    mystrcpy(myMsg, "FILE WRITE DENY");
    break;
  case FILE_READ_DENY:
    mystrcpy(myMsg, "FILE READ DENY");
    break;
  case FILE_CLOSE_FAIL:
    mystrcpy(myMsg, "FILE CLOSE FAIL");
    break;
  case ACCT_NOT_EXIST:
    mystrcpy(myMsg, "ACCT NOT EXIST");
    break;
  case ACCT_LENGTH_FAIL:
    mystrcpy(myMsg, "ACCT LENGTH FAIL");
    break;
  case ACCT_TYPE_WRONG:
    mystrcpy(myMsg, "ACCT TYPE WRONG");
    break;
  case ACCT_EXIST_AND_ACTIVE:
    mystrcpy(myMsg, "ACCT EXIST AND ACTIVE");
    break;
  case NOT_ENOUGH_BALANCE:
    mystrcpy(myMsg, "NOT ENOUGH BALANCE");
    break;
  case FILE_REMOVE_FAIL:
    mystrcpy(myMsg, "FILE REMOVE FAILS");
    break;
  case CC_WRONG_TX_TYPE:
    mystrcpy(myMsg, "CC WRONG TRANSACTION TYPE");
    break;
  case CC_REJ_PAY:
    mystrcpy(myMsg, "CC PAYMENT REJECTED");
    break;
  case CC_REJ_TX:
    mystrcpy(myMsg, "CC TRANSACTION REJECTED");
    break;
  case MORT_ACCT_OPEN_REJ:
    mystrcpy(myMsg, "MORTGAGE ACCT OPENING REJECTED");
    break;
  case WRONG_ACTION:
    mystrcpy(myMsg, "WRONG ACTION TYPE (UNIVERSAL)");
    break;
  case CD_NO_WIRETX:
    mystrcpy(myMsg, "NO WIRETRANSFER ON CD ACCT");
    break;
  case CUR_NOT_SUPPORT:
    mystrcpy(myMsg, "THIS CURRENCY IS NOT SUPPORTED");
    break;
  case CD_CK_NO_MONEY:
    mystrcpy(myMsg, "CHECK/CD NO ENOUH MONEY");
    break;
  case CD_CK_BIG_ERROR:
    mystrcpy(myMsg, "BIG ERROR OCCURS IN CD/CK CONVERSION");
    break;
  case CD_BAD_PERIOD:
    mystrcpy(myMsg, "WRONG CD PERIOD");
    break;
  case WRONG_CURRENCY:
    mystrcpy(myMsg, "WRONG CURRENCY UNIT");
    break;
  case LENGTH_TOO_LONG:
    mystrcpy(myMsg, "ACCOUNT NAME TOO LONG");
    break;
  case MORT_TYPE_WRONG:
    mystrcpy(myMsg, "WRONG MORTGAGE TYPE");
    break;
  case BAD_INPUT:
    mystrcpy(myMsg, "BAD INPUTS");
    break;
  case NOT_ACTIVE:
    mystrcpy(myMsg, "ACCOUNT NOT ACTIVE");
    break;
  case MORT_ACCT_OVERDUE:
    mystrcpy(myMsg, "ACCOUNT OVERDUE");
    break;
  case MORT_DEBT_PAID:
    mystrcpy(myMsg, "MORTGAGE DEBT PAID");
    break;
  case MORT_REFINANCE_FAIL:
    mystrcpy(myMsg, "MORTGAGE REFINANCE FAIL");
    break;
  case SPARC_SETUP_FAIL:
    mystrcpy(myMsg, "SPARC SETUP FAIL");
    break;
  case SPARC_SAMPLE_FAIL:
    mystrcpy(myMsg, "SPARC SAMPLE FAIL");
    break;
  case READ_WRITE_SIZE_FAIL:
    mystrcpy(myMsg, "READ/WRITE SIZE FAIL");
    break;
  case ADD_ACCT_FAIL:
    mystrcpy(myMsg, "ADD ACCOUNT FAIL");
    break;
  default :
    mystrcpy(myMsg, "UNKNOWN PROBLEM\n");
  }
#ifdef DEBUG
  if(level==WARNING_CODE){
    //warning message
    myfprintf(stderr, "[WARNING] %s:[%s]:%s\n", location, myMsg, msg);
  }
  else if (level==ERROR_CODE){
    //error message
    myfprintf(stderr, "[ERROR] %s:[%s]:%s\n", location, myMsg, msg);
    myexit(level);
  }
  else {
    if (errorCode == INFO_CODE_NO_LINE)
      myfprintf(fp, "[INFO] %s\n", msg);
    else
      myfprintf(fp, "[INFO] %s:%s\n", location, msg);
  }
#endif
  if (level==ERROR_CODE){
    //error message
    myfprintf(stderr, "[ERROR] %s:[%s]:%s\n", location, myMsg, msg);
    myexit(level);
  }
}

/******* Check Country Code *******/
void checkName(char* name, int intValue){
  if (intValue == 0)  mysprintf(name, "Australia dollar");
  else if(intValue == 1) mysprintf(name, "Chinese yuan");
  else if(intValue == 2) mysprintf(name, "European euro");
  else if(intValue == 3) mysprintf(name, "U.K. Sterling");
  else if(intValue == 4) mysprintf(name, "Hong Kong dollar");
  else if(intValue == 5) mysprintf(name, "Japan yen");
  else if(intValue == 6) mysprintf(name, "New Zealand dollar");
  else if(intValue == 7) mysprintf(name, "Singapore dollar");
  else if(intValue == 8) mysprintf(name, "New Taiwan dollar");
  else if(intValue == 9) mysprintf(name, "United State dollar");
  else {
    myfprintf(stderr, "currency unit is %d\n", intValue);
    errorPrint(LOC, ERROR_CODE, WRONG_CURRENCY, stderr, "WRONG CURRENCY UNIT");
  }
}

// Round to the closest available coin in target NCU
void roundToAvailableCoin(dec_t *converted_amt, dec_t input_amt, dec_t minUnit){
  //dec_t temp, new_input_amt, large_ncu;
  dec_t temp, new_input_amt;
  if (ISZERO64(input_amt)!=1) {
   int temp_gt = 0;
   GT64(temp_gt, minUnit, ONE, setp);
   if(temp_gt) {
      DIVIDE64(temp, input_amt, TEN, setp); //input_input_amtmt = input_amt/10;
    }
    else
      COPY64(temp, input_amt); //input_amt = input_amt
    SET_ROUNDMODE((*setp), RNI);
    TOINTEGRAL64(new_input_amt, temp, setp); //int(input_amt)
    SET_ROUNDMODE((*setp), RNE);
    SUBTRACT64(temp, temp, new_input_amt, setp); //input_amt-int(input_amt)
    if (ISZERO64(temp)==0){//means there are some fraction we need to round
	if(temp_gt) {
	MULTIPLY64(temp, temp, TEN, setp); //a = a*10;
	MULTIPLY64(new_input_amt, new_input_amt, TEN, setp);//int_input_amt = int(input_amt) * 10
      }
      DIVIDE64(temp, temp, minUnit, setp); //temp = temp / minUnit;
      TOINTEGRAL64(temp, temp, setp); //round(temp)
      MULTIPLY64(temp, temp, minUnit, setp); //temp = temp * minUnit;
      ADD64((*converted_amt), temp, new_input_amt, setp); //converted_amt = int(input_amt) + temp
    }
    else {
      //no fraction part that needs to be rounded, pass the input to the output
      COPY64((*converted_amt), input_amt);
    }
  }
}
void constdef(dec_t **conversion_rate, cc_const_t *cc_const){
  FROMSTRING64(ONE, "1.0", setp);
  FROMSTRING64(TWO, "2.0", setp);
  FROMSTRING64(ZEROCONST, "0.0", setp);
  FROMSTRING64(TWELVE, "12", setp);
  FROMSTRING64(MTWO, "-2", setp);
  FROMSTRING64(ETWO, "1E-2", setp);
  FROMSTRING64(MTHREE, "-3", setp);
  FROMSTRING64(MFOUR, "-4", setp);
  FROMSTRING64(MFIVE, "-5", setp);
  FROMSTRING64(MNINE, "-9", setp);
  FROMSTRING64(TEN, "10", setp);
  FROMSTRING64(MONTH, "30", setp);
  FROMSTRING64(HUNDRED, "100", setp);
  FROMSTRING64(THOUSAND, "1000", setp);
  FROMSTRING64(YEAR, "360", setp);
  FROMSTRING64(CHECKING_RATE, "0.5", setp);
  FROMSTRING64(NON_CUSTOMER_FIXED_RATE, NON_CUSTOMER_FIXED, setp);
  FROMSTRING64(CENT, "0.01", setp);
  FROMSTRING64(mort_const.hundredThousand, "100000", setp);
  FROMSTRING64(mort_const.title_search, TITLE_SEARCH_FEE, setp);
  FROMSTRING64(mort_const.title_examination, TITLE_EXAMINATION_FEE, setp);
  FROMSTRING64(mort_const.closing, CLOSING_FEE, setp);
  FROMSTRING64(mort_const.recording, RECORDING_FEE, setp);
  FROMSTRING64(mort_const.title_insurance_rate_1, 
	       TITLE_INSURANCE_RATE_1, setp);
  FROMSTRING64(mort_const.title_insurance_rate_2, 
	       TITLE_INSURANCE_RATE_2, setp);
  FROMSTRING64(mort_const.title_insurance_base,   TITLE_INSURANCE_BASE, setp);
  FROMSTRING64(mort_const.documentary_stamp_tax_on_note_rate_1, 
	       DOCUMENTARY_STAMP_TAX_ON_NOTE_RATE_1, setp);
  FROMSTRING64(mort_const.documentary_stamp_tax_on_note_rate_2,
               DOCUMENTARY_STAMP_TAX_ON_NOTE_RATE_2, setp);
  FROMSTRING64(mort_const.intagible_tax_on_note, 
	       INTANGIBLE_TAX_ON_NOTE_RATE, setp);
  FROMSTRING64(mort_const.lender_title_insurance_policy, 
	       LENDER_TITLE_INSURANCE, setp);
  FROMSTRING64(mort_const.alta_title_endorsements, 
	       ALTA_TITLE_ENDORSEMENTS, setp);
  FROMSTRING64(mort_const.title_endorsement_extra, 
	       TITLE_ENDORSEMENT_EXTRA, setp);
  FROMSTRING64(mort_const.title_endorsement_rate, 
	       TITLE_ENDORSEMENT_RATE, setp);
  FROMSTRING64(mort_const.origination, ORIGINATION, setp);
  FROMSTRING64(mort_const.appraisal, APPRAISAL, setp);
  FROMSTRING64(mort_const.credit_report, CREDIT_REPORT, setp);
  FROMSTRING64(mort_const.additional_closing_fees, 
	       ADDITIONAL_CLOSING_FEES, setp);
  FROMSTRING64(mort_const.closing_cost_other, CLOSING_COSTS_OTHER, setp);
  FROMSTRING64(mort_const.premature_close_penalty_year_0, 
	       PREMATURE_CLOSE_PENALTY_YEAR_0, setp);
  FROMSTRING64(mort_const.premature_close_penalty_year_1,
               PREMATURE_CLOSE_PENALTY_YEAR_1, setp);
  FROMSTRING64(mort_const.premature_close_penalty_year_2,
               PREMATURE_CLOSE_PENALTY_YEAR_2, setp);
  FROMSTRING64(mort_const.premature_close_penalty_year_3,
               PREMATURE_CLOSE_PENALTY_YEAR_3, setp);
  FROMSTRING64(mort_const.overdue_surcharge_rate, OVERDUE_SURCHARGE_RATE, 
	       setp);
  FROMSTRING64(mort_const.min_overdue_yr_penalty_rate, 
	       MIN_OVERDUE_YR_PENALTY_RATE, setp);
  FROMSTRING64(mort_const.min_overdue_yr_penalty_point, 
	       MIN_OVERDUE_YR_PENALTY_POINT, setp);
  FROMSTRING64(mort_const.min_overdue_yr_penalty_margin, 
	       MIN_OVERDUE_YR_PENALTY_MARGIN, setp);
  //USD and Foreign Coin Initialization
  initCoin(conversion_rate);
  //Credit Card Const Initialization
  cc_const_init(cc_const);
}

int checkGEZero(dec_t in){
  //first, check the sign of the number
  //second, check if it is zero
  if (ISNAN64(in)|ISINFINITE64(in))
    return 0;
  else if (ISNEGATIVE64(in)|ISZERO64(in))
    return 0;
  else
    return 1;
}

int checkLEOne(dec_t in){
  if (ISNAN64(in)|ISINFINITE64(in))
    return 0;
  else {
    int lt_res = 0;
    LT64(lt_res, in, TWO, setp);
    if(lt_res) 
      return 1;
    else
      return 0;
  }
}
int parse_cmd(FILE* file_ptr, cmd_t *this_cmd){
  /*This fcn is to parse user's input cmd*/
  //[Account number] [ACTION] [$] [acct_length+acct_type] [SELECT]
  int ret;
  char input_str[STR_MAX_LENGTH];
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
  decimal64 fund, str1;
  if ((ret = myfread(this_cmd->acct_no, sizeof(char), MAX_ACCT_NAME_LENGTH,
                   file_ptr))==0){
    return EOF;
  }
  if (mystrlen(this_cmd->acct_no)>=MAX_ACCT_NAME_LENGTH) {
    mysprintf(input_str, "char str is too long, truncated to %d bytes\n",
	    MAX_ACCT_NAME_LENGTH-1);
    errorPrint(LOC, WARNING_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
    this_cmd->acct_no[15]='\0';
  }
  if ((ret = myfread(&fund, sizeof(decimal64), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "fund size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  decimal64ToNumber(&fund, &this_cmd->fund);
  if ((ret = myfread(&str1, sizeof(decimal64), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "str1 size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  decimal64ToNumber(&str1, &this_cmd->str1);
  if ((ret = myfread(&this_cmd->action, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "action size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = myfread(&this_cmd->length, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "length size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = myfread(&this_cmd->acct_type, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "acct type size error, total read in %d element\n",
            ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = myfread(&this_cmd->select, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "select1 size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
 #else //WIDTH128
  decimal128 fund, str1;
  if ((ret = myfread(this_cmd->acct_no, sizeof(char), MAX_ACCT_NAME_LENGTH,
                   file_ptr))==0){
    return EOF;
  }
  if (mystrlen(this_cmd->acct_no)>=MAX_ACCT_NAME_LENGTH) {
    mysprintf(input_str, "char str is too long, truncated to %d bytes\n",
	    MAX_ACCT_NAME_LENGTH-1);
    errorPrint(LOC, WARNING_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
    this_cmd->acct_no[15]='\0';
  }
  if ((ret = myfread(&fund, sizeof(decimal128), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "fund size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  decimal128ToNumber(&fund, &this_cmd->fund);
  if ((ret = myfread(&str1, sizeof(decimal128), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "str1 size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  decimal128ToNumber(&str1, &this_cmd->str1);
  if ((ret = myfread(&this_cmd->action, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "action size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = myfread(&this_cmd->length, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "length size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = myfread(&this_cmd->acct_type, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "acct type size error, total read in %d element\n",
            ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = myfread(&this_cmd->select, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    mysprintf(input_str, "select1 size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
 #endif // 128
#else
  if ((ret = myfread(this_cmd, sizeof(cmd_t), 1, file_ptr))==0){
    return EOF;
  }
  if(ret!=1){
    mysprintf(input_str, "size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
#endif
#ifdef DEBUG
 {
   char str1[30], str2[30];
   TOSTRING64(this_cmd->fund, str1);
   TOSTRING64(this_cmd->str1, str2);
   myfprintf(stderr, "cmd: %s %d %s %d %d %d %s\n", this_cmd->acct_no,
	   this_cmd->action, str1,
	   this_cmd->acct_type, this_cmd->length, this_cmd->select,
	   str2);
 }
#endif
  if ((ISNEGATIVE64(this_cmd->fund))||(ISNEGATIVE64(this_cmd->str1))||
      (this_cmd->action<0)||(this_cmd->length<0)||(this_cmd->acct_type<0)||
      (this_cmd->select<0)){
    mysprintf(input_str, "Negative input in the cmd file at line %d\n",
            line_num);
    errorPrint(LOC, ERROR_CODE, BAD_INPUT, stderr, input_str);
  }
  int lt_res = 0;
  LT64(lt_res, (this_cmd->fund), (this_cmd->str1), setp);

  if ((this_cmd->action==MORT_OPEN)&&(lt_res)){
    mysprintf(input_str, "This action for %s is not valid because the \
                          downpayment is greater than the loan\n",
            this_cmd->acct_no);
    errorPrint(LOC, WARNING_CODE, MORT_ACCT_OPEN_REJ, stderr, input_str);
    return MORT_ACCT_OPEN_REJ;
  }
  return 0;
}


/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-05-18 03:07:53 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: basic_fcn.c,v $
  Revision 1.14  2009-05-18 03:07:53  mjanders
  Makefile

  Revision 1.13  2009-04-19 21:40:07  mjanders
  Updated decnumber and BID libraries

  Revision 1.12  2009-04-13 00:41:53  mjanders
  Removed debug output and fixed bug with 128-bit

  Revision 1.11  2009-04-12 05:57:11  mjanders
  Added 128-bit mode

  Revision 1.10  2009-02-18 00:42:15  mjanders
  Fixed bug where _IDEC_glb_flags was used even with DECNUMBER* was defined

  Revision 1.9  2009-01-08 21:30:48  mjanders
  Cleanup

  Revision 1.8  2009/01/06 18:39:12  mjanders
  Added common header

  Revision 1.7  2009/01/02 23:53:27  mjanders
  Added native_gcc_bid mode and tested

  Revision 1.6  2008/11/25 06:14:59  mjanders
  Finished adding BID

  Revision 1.5  2008/11/21 04:34:19  mjanders
  Finished adding BID

  Revision 1.4  2008/05/15 04:56:06  mjanders
  Switched all COMPAREs to new compare functions

  Revision 1.3  2008/05/14 22:19:46  mjanders
  Adding support for BID

  Revision 1.2  2008/04/24 21:39:08  mjanders
  Copied operator.h from other benchmarks so it will be the same format.
  Fixed MINUS() to only have 3 parameters rather than 4. We should fix
  this for euro and the others.

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.14  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.13  2007/03/14 18:09:55  lwang
  temp check-in

  Revision 1.12  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.10  2006/11/30 15:01:20  lwang
  Clecn up several bug in mortgage and rate file

  Revision 1.9  2006/11/22 01:40:31  lwang
  comment added

  Revision 1.8  2006/11/21 03:02:44  lwang
  Major Revision with new technique to access database

  Revision 1.7  2006/11/19 23:23:09  lwang
  First draft of Banking System is done

  Revision 1.6  2006/11/17 02:20:00  lwang
  Credit Card benchmark added

  Revision 1.5  2006/11/07 08:26:02  lwang
  After spliting CD and Checking database

  Revision 1.4  2006/11/07 05:20:10  lwang
  Name of the functions changed. Before spliting CD and CK

  Revision 1.3  2006/11/07 03:34:01  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.2  2006/11/03 02:17:46  lwang
  Initial check-in

*/
