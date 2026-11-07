//----------------------------------------
// Basic_fcn.c
// Decimal Floating-Point Benchmark
// This file is to define the common functions used throughout the benchmark.
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.6 $

#include "dfp_includes.h"
#include "basic_fcn.h"
/////////////////////////////////////////////////////
//GLOBAL EXTERNAL VARIABLES
//The constants used in the microbenchmark
// M* means "minus *". E.g. MTWO = -2
//////////////////////////////////////////////////////
dec64_t ONE, ZEROCONST, TWO, TWELVE, MTWO, ETWO, MTHREE, MFOUR, EFOUR, MFIVE, MNINE,
  TEN, MONTH, HUNDRED, YEAR, THOUSAND, CHECKING_RATE, NON_CUSTOMER_FIXED_RATE,
  CENT;
mort_const_t mort_const;
extern int counter;
extern context_t *setp;
extern int line_num;

// generate a random value (for account transactions)
void value_gen(char *name, int minimum, int range){
  char c;
  int length;
  int index=0;
  int my_rand;
#ifdef HAS_EXP
  int exp;
  char *ptr;
#endif
  c = 43;
  name[index++] = c;
  length = rand()%range + minimum;
  for (; index <= length; index++){
    c = (char) (rand() % 10 + 48);
    name[index] = c;
  }
  name[index++] = 'E';
#ifdef HAS_EXP
  ptr = &name[index];
  exp = rand()%767-398;
  sprintf(ptr, "%d", exp);
#else
  my_rand = rand()%2;
  if ((length>4)&&(my_rand==1)){
    c = (char)'-';
    name[index++] = c;
    c = (char)'2';
    name[index++] = c;
  }
  else {
    c = (char)'0';
    name[index++] = c;
  }
  name[index] = '\0';
#endif
}


// name the account
int naming(char *name, int max_acct_num, int acct_in_use, int acct_type){
  int length;
  int byte;
  if (acct_in_use==max_acct_num) return REACH_MAX_ACCT;
  do {
    length = 4 + rand()%(MAX_ACCT_NAME_LENGTH-14); // determine the length of the account name (from 4 to 6 characters)
  }while (length==0);
  for (byte = 0; byte < length; byte++){
    name[byte] = (char) (rand() % 26 + 65); // generate a random character
  }
  name[byte] = '\0';
  if (acct_type==CHECKING){
    strcat(name, "_CK");
  }
  else if (acct_type==CD){
    strcat(name, "_CD");
  }
  else if (acct_type==FRM15YEAR){
    strcat(name, "_FRM15");
  }
  else if (acct_type==FRM30YEAR){
    strcat(name, "_FRM30");
  }
  else if (acct_type==ARM1YEAR){
    strcat(name, "_ARM1");
  }
  else if (acct_type==ARM51YEAR){
    strcat(name, "_ARM51");
  }
  else {
    strcat(name, "_CC");
  }
  return 0;
}



void errorPrint(const char *location, int level, 
		int errorCode, FILE *fp, const char *msg){
  char myMsg[MSG_LENGTH];
  switch(errorCode){
  case FILE_NOT_EXIST:
    strcpy(myMsg, "FILE/DIRECTORY NOT EXIST");
    break;
  case FILE_WRITE_DENY:
    strcpy(myMsg, "FILE WRITE DENY");
    break;
  case FILE_READ_DENY:
    strcpy(myMsg, "FILE READ DENY");
    break;
  case FILE_CLOSE_FAIL:
    strcpy(myMsg, "FILE CLOSE FAIL");
    break;
  case ACCT_NOT_EXIST:
    strcpy(myMsg, "ACCT NOT EXIST");
    break;
  case ACCT_LENGTH_FAIL:
    strcpy(myMsg, "ACCT LENGTH FAIL");
    break;
  case ACCT_TYPE_WRONG:
    strcpy(myMsg, "ACCT TYPE WRONG");
    break;
  case ACCT_EXIST_AND_ACTIVE:
    strcpy(myMsg, "ACCT EXIST AND ACTIVE");
    break;
  case NOT_ENOUGH_BALANCE:
    strcpy(myMsg, "NOT ENOUGH BALANCE");
    break;
  case FILE_REMOVE_FAIL:
    strcpy(myMsg, "FILE REMOVE FAILS");
    break;
  case CC_WRONG_TX_TYPE:
    strcpy(myMsg, "CC WRONG TRANSACTION TYPE");
    break;
  case CC_REJ_PAY:
    strcpy(myMsg, "CC PAYMENT REJECTED");
    break;
  case CC_REJ_TX:
    strcpy(myMsg, "CC TRANSACTION REJECTED");
    break;
  case MORT_ACCT_OPEN_REJ:
    strcpy(myMsg, "MORTGAGE ACCT OPENING REJECTED");
    break;
  case WRONG_ACTION:
    strcpy(myMsg, "WRONG ACTION TYPE (UNIVERSAL)");
    break;
  case CD_NO_WIRETX:
    strcpy(myMsg, "NO WIRETRANSFER ON CD ACCT");
    break;
  case CUR_NOT_SUPPORT:
    strcpy(myMsg, "THIS CURRENCY IS NOT SUPPORTED");
    break;
  case CD_CK_NO_MONEY:
    strcpy(myMsg, "CHECK/CD NO ENOUH MONEY");
    break;
  case CD_CK_BIG_ERROR:
    strcpy(myMsg, "BIG ERROR OCCURS IN CD/CK CONVERSION");
    break;
  case CD_BAD_PERIOD:
    strcpy(myMsg, "WRONG CD PERIOD");
    break;
  case WRONG_CURRENCY:
    strcpy(myMsg, "WRONG CURRENCY UNIT");
    break;
  case LENGTH_TOO_LONG:
    strcpy(myMsg, "ACCOUNT NAME TOO LONG");
    break;
  case MORT_TYPE_WRONG:
    strcpy(myMsg, "WRONG MORTGAGE TYPE");
    break;
  case BAD_INPUT:
    strcpy(myMsg, "BAD INPUTS");
    break;
  case NOT_ACTIVE:
    strcpy(myMsg, "ACCOUNT NOT ACTIVE");
    break;
  case MORT_ACCT_OVERDUE:
    strcpy(myMsg, "ACCOUNT OVERDUE");
    break;
  case MORT_DEBT_PAID:
    strcpy(myMsg, "MORTGAGE DEBT PAID");
    break;
  case MORT_REFINANCE_FAIL:
    strcpy(myMsg, "MORTGAGE REFINANCE FAIL");
    break;
  case MORT_LENGTH_FAIL:
    strcpy(myMsg, "MORTGAGE LENGTH FAIL");
    break;
  case SPARC_SETUP_FAIL:
    strcpy(myMsg, "SPARC SETUP FAIL");
    break;
  case SPARC_SAMPLE_FAIL:
    strcpy(myMsg, "SPARC SAMPLE FAIL");
    break;
  case READ_WRITE_SIZE_FAIL:
    strcpy(myMsg, "READ/WRITE SIZE FAIL");
    break;
  case ADD_ACCT_FAIL:
    strcpy(myMsg, "ADD ACCOUNT FAIL");
    break;
  case ACCT_INIT_FAIL:
    strcpy(myMsg, "ACCT INIT FAIL");
    break;
  default :
    strcpy(myMsg, "UNKNOWN PROBLEM\n");
  }
#ifdef DEBUG
  if(level==WARNING_CODE){
    //warning message
    fprintf(stderr, "[WARNING] %s:[%s]:%s\n", location, myMsg, msg);
  }
  else if (level==ERROR_CODE){
    //error message
    fprintf(stderr, "[ERROR] %s:[%s]:%s\n", location, myMsg, msg);
    exit(level);
  }
  else {
    if (errorCode == INFO_CODE_NO_LINE)
      fprintf(fp, "[INFO] %s\n", msg);
    else
      fprintf(fp, "[INFO] %s:%s\n", location, msg);
  }
#endif
  if (level==ERROR_CODE){
    //error message
    fprintf(stderr, "[ERROR] %s:[%s]:%s\n", location, myMsg, msg);
    exit(level);
  }
}

/******* Check Country Code *******/
void checkName(char* name, int intValue){
  if (intValue == 0)  sprintf(name, "Australia dollar");
  else if(intValue == 1) sprintf(name, "Chinese yuan");
  else if(intValue == 2) sprintf(name, "European euro");
  else if(intValue == 3) sprintf(name, "U.K. Sterling");
  else if(intValue == 4) sprintf(name, "Hong Kong dollar");
  else if(intValue == 5) sprintf(name, "Japan yen");
  else if(intValue == 6) sprintf(name, "New Zealand dollar");
  else if(intValue == 7) sprintf(name, "Singapore dollar");
  else if(intValue == 8) sprintf(name, "New Taiwan dollar");
  else if(intValue == 9) sprintf(name, "United State dollar");
  else {
    fprintf(stderr, "currency unit is %d\n", intValue);
    errorPrint(LOC, ERROR_CODE, WRONG_CURRENCY, stderr, "WRONG CURRENCY UNIT");
  }
}

// Round to the closest available coin in target NCU
void roundToAvailableCoin(dec64_t *c, dec64_t a, dec64_t b){
  dec64_t temp, new_a; //large_ncu;
  int b_gt_one = 0;

  // is b greater than one?
  GT64(b_gt_one, b, ONE, setp);
  if (b_gt_one){
    DIVIDE64(temp, a, TEN, setp); //a = a/10;
  } else {
    COPY64(temp, a); //a = a
  }

/*
  COMPARE64(large_ncu, b, ONE, setp);
  if (ISPOSITIVE64(large_ncu)) {//b is greater than one
    DIVIDE64(temp, a, TEN, setp); //a = a/10;
  }
  else
    COPY64(temp, a); //a = a
*/

  SET_ROUNDMODE((*setp), RTZ);
  TOINTEGRAL64(new_a, temp, setp); //int(a)
  SET_ROUNDMODE((*setp), RNE);
  
  SUBTRACT64(temp, temp, new_a, setp); //a-int(a)

  if (ISZERO64(temp)==0){//means there are some fraction we need to round
    if (b_gt_one){ //b is greater than one
      MULTIPLY64(temp, temp, TEN, setp); //a = a*10;
      MULTIPLY64(new_a, new_a, TEN, setp);//int_a = int(a) * 10
    }
    DIVIDE64(temp, temp, b, setp);
    TOINTEGRAL64(temp, temp, setp);
    MULTIPLY64(temp, temp, b, setp);
    ADD64((*c), temp, new_a, setp);
  }
  else {
    //no fraction part that needs to be rounded, pass the input to the output
    COPY64((*c), a);
  }
}

void specialRescale(dec64_t *c, dec64_t a, dec64_t b){
  dec64_t temp;
  #ifdef BID
  _IDEC_flags backup_status;
  if ((_IDEC_glbflags & BID_FLAG_MASK) != 0){
    char input_str[STR_MAX_LENGTH];
    sprintf(input_str, "decContext in Bad Status, Status: %x\n", 
	    _IDEC_glbflags);
    errorPrint(LOC, ERROR_CODE, ERROR_CODE, stderr, input_str);
  }
  backup_status = _IDEC_glbflags;
  //RESCALE64(temp, a, b, setp);
  QUANTIZE64(temp, a, b, setp);
  if ((_IDEC_glbflags & BID_FLAG_MASK) != 0){
    COPY64(*c, a);
  }
  else{
    COPY64(*c, temp);
  }
  _IDEC_glbflags = backup_status;
  #else
  uint32_t backup_status;
  if (setp->status & DEC_Errors){
    char input_str[STR_MAX_LENGTH];
    sprintf(input_str, "decContext in Bad Status, Status: %s\n", 
	    decContextStatusToString(setp));
    errorPrint(LOC, ERROR_CODE, ERROR_CODE, stderr, input_str);
  }
  backup_status = setp->status;
  //RESCALE64(temp, a, b, setp);
  QUANTIZE64(temp, a, b, setp);
  if (setp->status & DEC_Errors){
    COPY64(*c, a);
  }
  else{
    COPY64(*c, temp);
  }
  setp->status = backup_status;
  #endif
}

void constdef(dec64_t **conversion_rate, cc_const_t *cc_const){
  FROMSTRING64(ONE, "1.0", setp);
  FROMSTRING64(TWO, "2.0", setp);
  FROMSTRING64(ZEROCONST, "0.0", setp);
  FROMSTRING64(TWELVE, "12", setp);
  FROMSTRING64(MTWO, "-2", setp);
  FROMSTRING64(ETWO, "1E-2", setp); // Scale setting for QUANTIZE
  FROMSTRING64(MTHREE, "-3", setp);
  FROMSTRING64(MFOUR, "-4", setp);
  FROMSTRING64(EFOUR, "1E-4", setp); // Scale setting for QUANTIZE
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

int checkGEZero(dec64_t in){
  //first, check the sign of the number
  //second, check if it is zero
  if (ISNAN64(in)|ISINFINITE64(in))
    return 0;
  else if (ISNEGATIVE64(in)|ISZERO64(in))
    return 0;
  else
    return 1;
}

int checkLEOne(dec64_t in){
  //dec64_t temp;
  if (ISNAN64(in)|ISINFINITE64(in))
    return 0;
  else {
    int mybool;
    LT64(mybool, in, TWO, setp);
    if (mybool) return 1;
    else return 0;
   /*
    COMPARE64(temp, in, TWO, setp);
    if (ISNEGATIVE64(temp))
      return 1;
    else
      return 0;
      */
  }
}
int parse_cmd(FILE* file_ptr, cmd_t *this_cmd){
  /*This fcn is to parse user's input cmd*/
  //[Account number] [ACTION] [$] [acct_length+acct_type] [SELECT]
  int ret;
  //dec64_t temp;
  char input_str[STR_MAX_LENGTH];
//#ifndef HW // edited on May15 2008
#ifdef DECNUMBER_ARBITRARY
  decimal64 fund, str1;
  if ((ret = fread(this_cmd->acct_no, sizeof(char), MAX_ACCT_NAME_LENGTH,
                   file_ptr))==0){
    return EOF;
  }
  if (strlen(this_cmd->acct_no)>=MAX_ACCT_NAME_LENGTH) {
    sprintf(input_str, "char str is too long, truncated to %d bytes\n",
	    MAX_ACCT_NAME_LENGTH-1);
    errorPrint(LOC, WARNING_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
    this_cmd->acct_no[15]='\0';
  }
  if ((ret = fread(&fund, sizeof(decimal64), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    sprintf(input_str, "fund size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  decimal64ToNumber(&fund, &this_cmd->fund);
  if ((ret = fread(&str1, sizeof(decimal64), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    sprintf(input_str, "str1 size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  decimal64ToNumber(&str1, &this_cmd->str1);
  if ((ret = fread(&this_cmd->action, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    sprintf(input_str, "action size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = fread(&this_cmd->length, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    sprintf(input_str, "length size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = fread(&this_cmd->acct_type, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    sprintf(input_str, "acct type size error, total read in %d element\n",
            ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
  if ((ret = fread(&this_cmd->select, sizeof(int), 1, file_ptr))==0)
    return EOF;
  if (ret!=1) {
    sprintf(input_str, "select1 size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
#else
  if ((ret = fread(this_cmd, sizeof(cmd_t), 1, file_ptr))==0){
    return EOF;
  }
  if(ret!=1){
    sprintf(input_str, "size error, total read in %d element\n", ret);
    errorPrint(LOC, ERROR_CODE, ACCT_LENGTH_FAIL, stderr, input_str);
  }
#endif
#ifdef DEBUG
 {
   char str1[30], str2[30];
   TOSTRING64(this_cmd->fund, str1);
   TOSTRING64(this_cmd->str1, str2);
   fprintf(stderr, "cmd: %s %d %s %d %d %d %s\n", this_cmd->acct_no,
	   this_cmd->action, str1,
	   this_cmd->acct_type, this_cmd->length, this_cmd->select,
	   str2);

 }
#endif
  if ((ISNEGATIVE64(this_cmd->fund))||(ISNEGATIVE64(this_cmd->str1))||
      (this_cmd->action<0)||(this_cmd->length<0)||(this_cmd->acct_type<0)||
      (this_cmd->select<0)){
    sprintf(input_str, "Negative input in the cmd file at line %d\n",
            line_num);
    errorPrint(LOC, ERROR_CODE, BAD_INPUT, stderr, input_str);
  }

//  COMPARE64(temp, (this_cmd->fund), (this_cmd->str1), setp);
  int mybool;
  LT64(mybool, this_cmd->fund, this_cmd->str1, setp);
  if ((this_cmd->action==MORT_OPEN) && mybool ){
    sprintf(input_str, "This action for %s is not valid because the \
                          downpayment is greater than the loan\n",
            this_cmd->acct_no);
    errorPrint(LOC, WARNING_CODE, MORT_ACCT_OPEN_REJ, stderr, input_str);
    return MORT_ACCT_OPEN_REJ;
  }
  return 0;
}


/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-19 21:40:12 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: basic_fcn.c,v $
  Revision 1.6  2009-04-19 21:40:12  mjanders
  Updated decnumber and BID libraries

  Revision 1.5  2009-04-16 21:28:23  mjanders
  added comments and changed account name length to be 4 to 10 characters

  Revision 1.4  2008-11-24 08:58:48  mjanders
  Added BID support

  Revision 1.3  2008/05/15 16:58:56  stsen
  changes for BID compatibiltiy

  Revision 1.2  2008/05/14 22:24:01  stsen
  updated API for COMPARE to GT

  Revision 1.1.1.1  2008/04/18 01:13:49  mjanders


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
