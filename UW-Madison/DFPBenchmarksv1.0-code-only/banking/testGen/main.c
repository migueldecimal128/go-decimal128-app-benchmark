#include "dfp_includes.h"
#include <time.h>
#include "deposit.h"
#include "mortgage.h"
#include "credit_card.h"

context_t *setp;
int counter;
int line_num;

int initialization(FILE **conversion_rate_file_ptr,
                   FILE **deposit_rate_file_ptr,
		   FILE **mort_rate_file_ptr, FILE *tcm_rate_file_ptr,
                   FILE *prime_rate_file_ptr,
                   deposit_rate_t* deposit_rate, dec64_t **conversion_rate,
		   mort_rate_t *mort_rate, dec64_t *tcm_rate,
                   dec64_t *prime_rate,
		   cc_const_t *cc_const, cc_rate_t *cc_rate){
  int i;
  for (i=0; i<2*COUNTRIES; i++) {
    conversion_rate[i] = calloc(2, sizeof(dec64_t));
  }
  constdef(conversion_rate, cc_const);
  //Currency Conversion Rate Table
  openConversionRateFile(conversion_rate_file_ptr);
  updateConversionTable(conversion_rate, conversion_rate_file_ptr);
  //Deposit Rate Table
  openDepositRateFile(deposit_rate_file_ptr);
  updateDepositRateTable(deposit_rate, deposit_rate_file_ptr);
  //Mortgage Rate Table
  openMortgageRateFile(mort_rate_file_ptr);
  updateMortgageRate(mort_rate_file_ptr, mort_rate);
  //Treasury Constant Maturity Rate Table
  updateTCMRateFile(tcm_rate, tcm_rate_file_ptr);
  //Update the Prime Rate Table
  updatePrimeRateFile(prime_rate, prime_rate_file_ptr);
  getCCRate(cc_rate, cc_const, *prime_rate);
  return 0;
}

int processing(htab *cd_table, htab *checking_table, htab *mortgage_table, 
	       htab *cc_table, 
	       deposit_rate_t *deposit_rate, //FILE *cmd_file_ptr, 
	       cmd_t *this_cmd, mort_rate_t *mort_rate, 
	       dec64_t **conversion_rate, dec64_t *tcm_rate,
	       cc_const_t *cc_const, cc_rate_t *cc_rate, dec64_t *prime_rate,
	       FILE *prime_rate_file_ptr,
	       FILE **conversion_rate_file_ptr, FILE **deposit_rate_file_ptr,
	       FILE **mort_rate_file_ptr, FILE *tcm_rate_file_ptr,
	       FILE *fout
	       /*For test generator*/ ,
	       FILE *ascii_file, FILE *bin_file,
	       int max_acct, int min_tx, int max_tx, int days
	       ){
  int ret;
  int prime_rate_change;
#ifdef DEBUG
  char input_str[STR_MAX_LENGTH];
#endif
  line_num = 1;
  //while ((ret = parse_cmd(cmd_file_ptr, this_cmd))!=EOF){
  /*For test generator*/
  int cur_day;
  char value[30];
  cmd64_t this_cmd64;

  int tx;
  int cur_tx;
  for (cur_day = 0; cur_day<days; cur_day++){
    //If not a good return from parse_cmd, continue!
    //if (ret<0) continue;
    tx = rand()%(max_tx-min_tx)+min_tx;
    //fprintf(stderr, "cur day is %d, total days are %d\n", cur_day, days);
    for (cur_tx=0; cur_tx<tx; cur_tx++) {
      line_num++;
#ifdef DEBUG
      /* For test generator */
      //fprintf(stderr, "Current tx count is %d, current day is %d\n", cur_tx, cur_day);
#endif
#ifdef DEPOSIT_ONLY
      this_cmd->acct_type = rand()%2;
      printf("acct_type rand: %d\n", this_cmd->acct_type);
#elif defined(MORTGAGE_ONLY)
      this_cmd->acct_type = rand()%4+FRM15YEAR;
#elif defined(CREDITCARD_ONLY)
      this_cmd->acct_type = CREDIT_CARD;
#else
      ZERO64(this_cmd->fund);
      ZERO64(this_cmd->str1);
      this_cmd->length = 0;
      this_cmd->select = 0;
      do {
	this_cmd->acct_type = (rand()%TX_TYPE);
      }while(this_cmd->acct_type==CASH); //cash tx is disable now
      //The following is to make all acct types have the same chances
      if (this_cmd->acct_type==4) 
	this_cmd->acct_type = this_cmd->acct_type + 3;
      if (this_cmd->acct_type==3) 
	this_cmd->acct_type = this_cmd->acct_type + (rand()%4);
#endif
      if (this_cmd->acct_type==CD){
	if (((int)hcount(cd_table))<STARTUP_MIN_ACCT)
	  this_cmd->action = CD_CK_OPEN;
	else {
	  if (cur_day < THRESHOLD_DAY) {
	    do {
	      this_cmd->action = rand()%DEPOSIT_ACTION+1;
	    }while ((this_cmd->action==CD_SET_CLOSE) ||
		    (this_cmd->action==CD_CK_CLOSE) ||
#ifndef INQUIRY
		    (this_cmd->action==CD_CK_INQUIRY)||
#endif		
		    (this_cmd->action==CD_EXTEND));
	  }
	  else {
#ifdef DEPOSIT_OPEN_ONLY
	    this_cmd->action = CD_CK_OPEN;
#else
	    do {
	      this_cmd->action = rand()%DEPOSIT_ACTION+1;
	    }while(
#ifndef INQUIRY
		   (this_cmd->action==CD_CK_INQUIRY)||
#endif
		   ((this_cmd->action==CD_CK_CLOSE)&&
		   (cur_day<THRESHOLD_CLOSE)));
#endif
	  }
	}
	if (this_cmd->action==CD_CK_OPEN){
	  ret = naming(this_cmd->acct_no, max_acct,
		       hcount(cd_table), this_cmd->acct_type);
	  if (ret!=0) {cur_tx--; continue;}
	  this_cmd->select = rand()%COUNTRIES;
	  do {
	    value_gen(value, CD_CK_MIN, CD_CK_RANGE);
	    FROMSTRING64(this_cmd->fund, value, setp);
	  }while(ISZERO64(this_cmd->fund)==1);
	  if (rand()%3==0)
	    this_cmd->length = 3*(rand()%2+1);
	  else
	    this_cmd->length = 1;
	  ret = deposit_acct_open(cd_table, this_cmd->acct_no, this_cmd->fund,
				  this_cmd->length, CD, this_cmd->select, 
				  PAY_DATE, fout, 
				  &deposit_rate[this_cmd->select]);
	  if (ret!=0) {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s opens a CD account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_CK_INQUIRY){
	  if (hnext(cd_table)){
	    deposit_acct_t *this_stuff = hstuff(cd_table);
	    strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    ret = deposit_acct_inquiry(cd_table, this_cmd->acct_no, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s inquiries his/her CD account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_SET_CLOSE) {
          if (hnext(cd_table)){
            deposit_acct_t *this_stuff = hstuff(cd_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    ret = deposit_mark_close(cd_table, this_cmd->acct_no, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
          else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "Mark Close %s's CD account\n", 
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_EXTEND) {
	  if (hnext(cd_table)){
            deposit_acct_t *this_stuff = hstuff(cd_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    ret = deposit_mark_extend(cd_table, this_cmd->acct_no, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
          else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "Mark Extend %s's CD account\n", 
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_CK_CLOSE) {
          if (hnext(cd_table)){
            deposit_acct_t *this_stuff = hstuff(cd_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    ret = deposit_acct_close(cd_table, this_cmd->acct_no, 
				     conversion_rate, CD, fout, deposit_rate);
	    if (ret!=0) {cur_tx--; continue;}
	  }
          else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "Close %s's CD account\n", this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_CK_DEPOSIT) {
          if (hnext(cd_table)){
            deposit_acct_t *this_stuff = hstuff(cd_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    this_cmd->select = rand()%COUNTRIES;
	    do {
	      value_gen(value, CD_CK_MIN, CD_CK_RANGE);
	      FROMSTRING64(this_cmd->fund, value, setp);
	    }while(ISZERO64(this_cmd->fund)==1);
	    ret = money_deposit(cd_table, this_cmd->acct_no, this_cmd->fund, 
				conversion_rate, this_cmd->select, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "Deposit to %s's CD account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_CK_WITHDRAWL) {
	  if (hnext(cd_table)){
            deposit_acct_t *this_stuff = hstuff(cd_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    this_cmd->select = rand()%COUNTRIES;
	    do {
	      value_gen(value, CD_CK_MIN, CD_CK_RANGE);
	      FROMSTRING64(this_cmd->fund, value, setp);
	    }while(ISZERO64(this_cmd->fund)==1);
	    ret = money_withdrawl(cd_table, this_cmd->acct_no, 
				  this_cmd->select, this_cmd->fund, 
				  conversion_rate, fout,
				  deposit_rate);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "Withdrawl to %s's CD account\n", 
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==WIRETRANSFER) {
	  //[Account number] [ACTION] [$] [USELESS+TO_CURRENCY] [FROM_CURRENCY]
#ifdef DEBUG
	  sprintf(input_str, "Wire transfer not allow in CD account\n");
	  errorPrint(LOC, WARNING_CODE, CD_NO_WIRETX, fout, input_str);
#endif
	  cur_tx--;
	  continue;
	}
      }
      else if (this_cmd->acct_type==CHECKING){
        if (((int)hcount(checking_table))<STARTUP_MIN_ACCT)
          this_cmd->action = CD_CK_OPEN;
	else {
	  if (cur_day < THRESHOLD_DAY) {
            do {
              this_cmd->action = rand()%DEPOSIT_ACTION+1;
            }while ((this_cmd->action==CD_SET_CLOSE) ||
                    (this_cmd->action==CD_CK_CLOSE) ||
#ifndef INQUIRY
                    (this_cmd->action==CD_CK_INQUIRY)||
#endif
                    (this_cmd->action==CD_EXTEND));
          }
          else {
#ifdef DEPOSIT_OPEN_ONLY
	    this_cmd->action = CD_CK_OPEN;
#else
	    do {
	      this_cmd->action = rand()%DEPOSIT_ACTION+1;
	    }while((this_cmd->action==CD_SET_CLOSE)||
#ifndef INQUIRY
		   (this_cmd->action==CD_CK_INQUIRY)||
#endif
		   (this_cmd->action==CD_EXTEND)||
		   ((this_cmd->action==CD_CK_CLOSE)&&
		    (cur_day<THRESHOLD_CLOSE)));
#endif
	  }
	}
	if (this_cmd->action==CD_CK_OPEN){
	  ret = naming(this_cmd->acct_no, max_acct,
		       hcount(checking_table), this_cmd->acct_type);
	  if (ret!=0) {cur_tx--; continue;}
	  this_cmd->select = rand()%COUNTRIES;
	  do {
	    value_gen(value, CD_CK_MIN, CD_CK_RANGE);
	    FROMSTRING64(this_cmd->fund, value, setp);
	  }while(ISZERO64(this_cmd->fund)==1);
	  ret = deposit_acct_open(checking_table, this_cmd->acct_no, 
				  this_cmd->fund,
				  this_cmd->length, CHECKING, this_cmd->select,
				  PAY_DATE, fout,
				  &deposit_rate[this_cmd->select]);
	  if (ret!=0) {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s opens a CHECKING account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_CK_INQUIRY){
          if (hnext(checking_table)){
            deposit_acct_t *this_stuff = hstuff(checking_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    ret = deposit_acct_inquiry(checking_table, this_cmd->acct_no, 
				       fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
#ifdef DEBUG
	    sprintf(input_str, "%s inquiries his/her CHECKING account\n",
		    this_cmd->acct_no);
	    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if ((this_cmd->action==CD_SET_CLOSE)||
		 (this_cmd->action==CD_EXTEND)){
#ifdef DEBUG
	  errorPrint(LOC, WARNING_CODE, ACCT_TYPE_WRONG, fout, 
		     "SET Close or Extend is not supported in Checking account");
#endif
	  cur_tx--;
	  continue;
	}
	else if (this_cmd->action==CD_CK_CLOSE) {
          if (hnext(checking_table)){
            deposit_acct_t *this_stuff = hstuff(checking_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    ret = deposit_acct_close(checking_table, this_cmd->acct_no, 
				     conversion_rate, CHECKING, fout, NULL);
	    if (ret!=0) {cur_tx--; continue;}
          }
          else {cur_tx--; continue;}
#ifdef DEBUG
	    sprintf(input_str, "Close %s's Checking account\n", 
		    this_cmd->acct_no);
	    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_CK_DEPOSIT) {
          if (hnext(checking_table)){
            deposit_acct_t *this_stuff = hstuff(checking_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    this_cmd->select = rand()%COUNTRIES;
	    do {
	      value_gen(value, CD_CK_MIN, CD_CK_RANGE);
	      FROMSTRING64(this_cmd->fund, value, setp);
	    }while(ISZERO64(this_cmd->fund)==1);
	    ret = money_deposit(checking_table, this_cmd->acct_no, 
				this_cmd->fund, conversion_rate, 
				this_cmd->select, fout);
	    if (ret!=0) {cur_tx--; continue;}
          }
          else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "Deposit to %s's CHECKING account\n", 
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CD_CK_WITHDRAWL) {
          if (hnext(checking_table)){
            deposit_acct_t *this_stuff = hstuff(checking_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    this_cmd->select = rand()%COUNTRIES;
	    do {
	      value_gen(value, CD_CK_MIN, CD_CK_RANGE);
	      FROMSTRING64(this_cmd->fund, value, setp);
	    }while(ISZERO64(this_cmd->fund)==1);
	    ret = money_withdrawl(checking_table, this_cmd->acct_no, 
				  this_cmd->select, this_cmd->fund, 
				  conversion_rate, fout, deposit_rate);
	    if (ret!=0) {cur_tx--; continue;}
          }
          else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "Withdrawl to %s's CHECKING account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==WIRETRANSFER) {
	  //[Account number] [ACTION] [$] [USELESS+TO_CURRENCY] [FROM_CURRENCY]
	  dec64_t transaction, processFee, out;
	  if (rand()%10==0)
	    strcpy(this_cmd->acct_no, "NON_CUSTOMER");
	  else {
	    if (hnext(checking_table)){
	      deposit_acct_t *this_stuff = hstuff(checking_table);
	      strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    }
	    else {cur_tx--; continue;}
	  }
	  this_cmd->select = rand()%COUNTRIES*10 + rand()%COUNTRIES;
	  do {
	    value_gen(value, CD_CK_MIN, CD_CK_RANGE);
	    FROMSTRING64(this_cmd->fund, value, setp);
	  }while(ISZERO64(this_cmd->fund)==1);
	  ret = wireTransfer(checking_table, this_cmd->select/10, 
			     this_cmd->acct_no, this_cmd->select%10, 
			     this_cmd->fund, &out, &transaction,
			     &processFee, conversion_rate, fout);
	  if (ret!=0) {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s performs Wire-transfer\n", this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
      }
      else if ((this_cmd->acct_type>=MORTGAGE)&&
	       (this_cmd->acct_type<CREDIT_CARD)){
	if (((int)hcount(mortgage_table))<STARTUP_MIN_ACCT)
          this_cmd->action = MORT_OPEN;
	else {
          if (cur_day < THRESHOLD_DAY) {
	    do {
	      this_cmd->action = rand()%MORT_SCHEDULE_TABLE+1;
	    }while((this_cmd->action==MORT_REFINANCE)||
		   (this_cmd->action==MORT_CLOSE)||
#ifndef INQUIRY
		   (this_cmd->action==MORT_INQUIRY)||
#endif
		   (this_cmd->action==MORT_PAY));
	  }
	  else{
	    do {
	      this_cmd->action = rand()%MORT_SCHEDULE_TABLE+1;
	    }while((this_cmd->action==MORT_PAY)||
#ifndef INQUIRY
		   (this_cmd->action==MORT_INQUIRY)||
#endif
		   ((this_cmd->action==MORT_CLOSE)&&
		    (cur_day<THRESHOLD_CLOSE)));
	  }
	}
	if (this_cmd->action==MORT_OPEN) {
	  ret = naming(this_cmd->acct_no, max_acct,
		       hcount(mortgage_table), this_cmd->acct_type);
	  if (ret!=0) {cur_tx--; continue;}
	  {
	    dec64_t TEN;
	    FROMSTRING64(TEN, "10", setp);
	    do {
	      value_gen(value, MORT_MIN, MORT_RANGE);
	      FROMSTRING64(this_cmd->fund, value, setp);
	    }while(ISZERO64(this_cmd->fund)==1);
	    DIVIDE64(this_cmd->str1, this_cmd->fund, TEN, setp);
	    this_cmd->select = rand()%2;
	    if (this_cmd->select){ //autopayment, do the checking first!
	      if (hfind(checking_table, this_cmd->acct_no, 
			strlen(this_cmd->acct_no))==FALSE)
		this_cmd->select = 0;
	    }
	    if (this_cmd->acct_type==FRM15YEAR)
	      this_cmd->length = 15*12;
	    else if (this_cmd->acct_type==FRM30YEAR)
	      this_cmd->length = 30*12;
	    else{
	      int year;
	      while ((year =rand()%40)<20);
	      this_cmd->length = year*12;
	    }
	  }
	  ret = mortgage_acct_open(mortgage_table, checking_table, 
				   mort_rate,  this_cmd->length, 
				   this_cmd->fund, this_cmd->str1, 
				   this_cmd->acct_no, this_cmd->acct_type, 
				   this_cmd->select,
				   fout);
	  if (ret!=0) {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s opens a mortgage account\n", 
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==MORT_INQUIRY){
	  if (hnext(mortgage_table)){
	    mort_acct_t *this_stuff = hstuff(mortgage_table);
	    strcpy(this_cmd->acct_no, this_stuff->acct_no);
#ifdef DEBUG
	  sprintf(input_str, "%s inquiries his/her mortgage account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	    ret = mortgage_acct_inquiry(mortgage_table, this_cmd->acct_no, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
	}
	else if (this_cmd->action==MORT_REFINANCE){
	  if (hnext(mortgage_table)){
	    mort_acct_t *this_stuff = hstuff(mortgage_table);
	    strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    {
	      this_cmd->select = rand()%2;
	      if (this_cmd->acct_type==FRM15YEAR)
		this_cmd->length = 15*12;
	      else if (this_cmd->acct_type==FRM30YEAR)
		this_cmd->length = 30*12;
	      else{
		int year;
		while ((year =rand()%40)<20);
		this_cmd->length = year*12;
	      }
	    }
	  }
	  else {cur_tx--; continue;}
	  ret = mortgage_acct_refinance(mortgage_table, checking_table,
					mort_rate, this_cmd->length,
					this_cmd->acct_no, this_cmd->acct_type,
					this_cmd->select, fout);
	  if (ret!=0) {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s refinances his/her mortgage account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==MORT_CLOSE) {
          if (hnext(mortgage_table)){
            mort_acct_t *this_stuff = hstuff(mortgage_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
#ifdef DEBUG
	    sprintf(input_str, "%s closes his/her mortgage account\n",
		    this_cmd->acct_no);
	    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	    ret = mortgage_acct_close(mortgage_table, this_cmd->acct_no, fout);
	    if (ret !=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
	}
	else if (this_cmd->action==MORT_PAY) {
          if (hnext(mortgage_table)){
            mort_acct_t *this_stuff = hstuff(mortgage_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
#ifdef DEBUG
	    sprintf(input_str, "%s pays his/her mortgage account\n",
		    this_cmd->acct_no);
	    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	    ret = mortgage_acct_pay(mortgage_table, this_cmd->acct_no, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
	}
	else{ //schedule table (this should be moved to personal finance later!
	  {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "Customer inquiries the schedule table\n");
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	  this_cmd->acct_type = this_cmd->acct_type;
	  do {
	    value_gen(value, MORT_MIN, MORT_RANGE);
	    FROMSTRING64(this_cmd->fund, value, setp);
	  }while(ISZERO64(this_cmd->fund)==1);
	  {
	    if (this_cmd->acct_type==FRM15YEAR)
	      this_cmd->length = 15*12;
	    else if (this_cmd->acct_type==FRM30YEAR)
	      this_cmd->length = 30*12;
	    else{
	      int year;
	      while ((year =rand()%40)<20);
	      this_cmd->length = year*12;
	    }
	  }
	  scheduleTable(fout, (this_cmd->fund), this_cmd->length, mort_rate,
			this_cmd->acct_type);
	}
      }
      else if (this_cmd->acct_type == CREDIT_CARD){
	if (((int)hcount(cc_table))<STARTUP_MIN_ACCT)
          this_cmd->action = CC_OPEN;
	else {
	  if (cur_day < THRESHOLD_DAY){
	    do {
	      this_cmd->action = rand()%CC_DEBT+1;
	    }while((this_cmd->action==CC_CLOSE)||
#ifndef INQUIRY
		   (this_cmd->action==CC_INQUIRY)||
#endif
		   (this_cmd->action==CC_PAY));
	  }
	  else {
	    do {
	      this_cmd->action = rand()%CC_DEBT+1;
	    }while((this_cmd->action==CC_PAY)||
#ifndef INQUIRY
		   (this_cmd->action==CC_INQUIRY)||
#endif
		   ((this_cmd->action==CC_CLOSE)&&(cur_day<THRESHOLD_CLOSE)));
	  }
	}
	if (this_cmd->action==CC_OPEN) {
	  ret = naming(this_cmd->acct_no, max_acct,
		       hcount(cc_table), this_cmd->acct_type);
	  if (ret!=0) {cur_tx--; continue;}
	  {
	    char *credit_limit[9] = {"500", "1000", "2000", "3000", "5000", 
				     "10000", "20000", "30000", "50000"};
	    int choices = rand()%9;
	    FROMSTRING64(this_cmd->fund, credit_limit[choices], setp);
	    this_cmd->select = rand()%2;
	    if (this_cmd->select){ //autopay
	      if (hfind(checking_table, this_cmd->acct_no, 
			strlen(this_cmd->acct_no))==FALSE)
		this_cmd->select = 0;
	    }
	  }
	  ret = credit_card_acct_open(cc_table, checking_table, 
				      this_cmd->acct_no,
				      cc_rate, cc_const,
				      (this_cmd->fund),
				      this_cmd->select,
				      fout);
	  if (ret!=0) {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s opens a Credit Card account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CC_INQUIRY) {
	  if (hnext(cc_table)){
            cc_acct_t *this_stuff = hstuff(cc_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    ret = credit_card_acct_inquiry(cc_table, this_cmd->acct_no, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s inquiries his/her Credit Card account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CC_CLOSE) {
          if (hnext(cc_table)){
            cc_acct_t *this_stuff = hstuff(cc_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    ret = credit_card_acct_close(cc_table, this_cmd->acct_no, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s closes his/her Credit Card account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CC_PAY) {
	  if (hnext(cc_table)){
            cc_acct_t *this_stuff = hstuff(cc_table);
	    if (ISZERO64(this_stuff->purchase_statement)&&
		ISZERO64(this_stuff->cash_adv)&&
		ISZERO64(this_stuff->bal_tx))
	      {cur_tx--; continue;}
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    {
	      this_cmd->select = rand()%4;
	      ZERO64(this_cmd->fund);
	      if (this_cmd->select==0){
		if (ISPOSITIVE64(this_stuff->purchase_statement)||
		    ISPOSITIVE64(this_stuff->posted_interest_purchase)){
		  ADD64(this_cmd->fund, this_cmd->fund, 
			this_stuff->purchase_statement, setp);
		  ADD64(this_cmd->fund, this_cmd->fund,
			this_stuff->posted_interest_purchase, setp);
		}
		if (ISPOSITIVE64(this_stuff->cash_adv)||
		    ISPOSITIVE64(this_stuff->posted_interest_cash_adv)){
		  ADD64(this_cmd->fund, this_cmd->fund, this_stuff->cash_adv,
			setp);
		  ADD64(this_cmd->fund, this_cmd->fund, 
			this_stuff->posted_interest_cash_adv, setp);
		}
		if (ISPOSITIVE64(this_stuff->bal_tx)||
		    ISPOSITIVE64(this_stuff->posted_interest_bal_tx)) {
		  ADD64(this_cmd->fund, this_cmd->fund, this_stuff->bal_tx,
			setp);
		  ADD64(this_cmd->fund, this_cmd->fund,
			this_stuff->posted_interest_bal_tx, setp);
		}
	      }
	    }
	    ret = credit_card_acct_pay(cc_table, this_cmd->acct_no, 
				       this_cmd->select, this_cmd->fund, fout);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str, "%s pays his/her Credit Card account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else if (this_cmd->action==CC_DEBT){
          if (hnext(cc_table)){
            cc_acct_t *this_stuff = hstuff(cc_table);
            strcpy(this_cmd->acct_no, this_stuff->acct_no);
	    {
	      this_cmd->select = (rand()%COUNTRIES)*10;
	      this_cmd->select = this_cmd->select + (rand()%3); //cc type
	      do {
		value_gen(value, CC_MIN, CC_RANGE);
		FROMSTRING64(this_cmd->fund, value, setp);
	      }while(ISZERO64(this_cmd->fund)==1);
	    }
	    ret = credit_card_acct_debt(cc_table, this_cmd->acct_no, 
					this_cmd->select/10,
					conversion_rate, fout,
					cc_const, this_cmd->select%10,
					this_cmd->fund);
	    if (ret!=0) {cur_tx--; continue;}
	  }
	  else {cur_tx--; continue;}
#ifdef DEBUG
	  sprintf(input_str,
		  "A transaction is posted to %s Credit Card account\n",
		  this_cmd->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	}
	else {
	  char input_str[STR_MAX_LENGTH];
	  fprintf(stderr, "%s has a wrong action\n", this_cmd->acct_no);
	  errorPrint(LOC, WARNING_CODE, WRONG_ACTION, fout, input_str);
	  continue;
	}
      }
      else {
	errorPrint(LOC, WARNING_CODE, WRONG_ACTION, stderr,
		   "###### Wrong Acct Type. This cmd is skipped ######");
	continue;
      }
#ifdef BID
      if ((__bid_IDEC_glbflags & BID_FLAG_MASK) == 0){
	char input_str[STR_MAX_LENGTH];
	sprintf(input_str, "[END CHECK] decContext in Bad Status: 0x%08x\n",
		__bid_IDEC_glbflags);
      }
#else
      if (setp->status & DEC_Errors){
	char input_str[STR_MAX_LENGTH];
	sprintf(input_str, "[END CHECK] decContext in Bad Status: 0x%08x, Status: %s\n",
		setp->status, decContextStatusToString(setp));
	errorPrint(LOC, ERROR_CODE, ERROR_CODE, stderr, input_str);
      }
#endif
      /* Now start writing stuffs into files */
      //FILE *ascii_file, FILE *bin_file
      //First, copy over everything from cmd_t to cmd64_t
      {
	char value[30], str1[30];
	strcpy(this_cmd64.acct_no, this_cmd->acct_no);
#ifdef DECNUMBER_ARBITRARY
	decimal64FromNumber(&this_cmd64.fund, &this_cmd->fund, setp);
#else
        memcpy(&(this_cmd64.fund), &(this_cmd->fund), sizeof(this_cmd->fund));
        //COPY64(this_cmd64.fund, this_cmd->fund);
#endif
	TOSTRING64(this_cmd->fund, value);
#ifdef DECNUMBER_ARBITRARY
	decimal64FromNumber(&this_cmd64.str1, &this_cmd->str1, setp);
#else
	memcpy(&(this_cmd64.str1), &(this_cmd->str1), sizeof(this_cmd->str1));
#endif
	TOSTRING64(this_cmd->str1, str1);
	this_cmd64.action = this_cmd->action;
	this_cmd64.length = this_cmd->length;
	this_cmd64.acct_type = this_cmd->acct_type;
	this_cmd64.select = this_cmd->select;
	if ((this_cmd->action<0)||(this_cmd->select<0)||(this_cmd->length<0)){
	  char input_str[STR_MAX_LENGTH];
	  sprintf(input_str, "Bad inputs for %s's account", this_cmd->acct_no);
	  errorPrint(LOC, WARNING_CODE, WRONG_ACTION, fout, input_str);
	  continue;
	}
	fprintf(ascii_file, "%-16s %-3d %-19s %-6d %-8d %-8d %-19s\n",
		this_cmd->acct_no, this_cmd->action, value, 
		this_cmd->acct_type, this_cmd->length,
		this_cmd->select, str1);
#ifdef DEBUG
	fprintf(stderr, "%-16s %-3d %-19s %-6d %-8d %-8d %-19s\n",
		this_cmd->acct_no, this_cmd->action, value, 
		this_cmd->acct_type, this_cmd->length,
		this_cmd->select, str1);
#endif
#ifdef WIDTH64
	fwrite(&this_cmd64, sizeof(cmd64_t), 1, bin_file);
#else //128
        cmd128_t temp_cmd;
        char temp_fund[255];
        char temp_str1[255];
        strcpy(temp_cmd.acct_no, this_cmd64.acct_no);
#if defined(DECNUMBER_ARBITRARY) || defined(DECNUMBER_FIXED)
        decContext c;
        decContextDefault(&c, DEC_INIT_DECIMAL128);
        decimal64ToString(&(this_cmd64.fund), temp_fund);
        decimal64ToString(&(this_cmd64.str1), temp_str1);
        decimal128FromString(&(temp_cmd.fund), temp_fund, &c);
        decimal128FromString(&(temp_cmd.str1), temp_str1, &c);
#endif
#if defined(BID)
        _IDEC_round r = BID_ROUNDING_TO_NEAREST;
        __bid64_to_string(temp_fund, &(this_cmd64.fund));
        __bid64_to_string(temp_str1, &(this_cmd64.str1));
        __bid128_from_string(&(temp_cmd.fund), temp_fund, &r);
        __bid128_from_string(&(temp_cmd.str1), temp_str1, &r);
#endif //BID
       temp_cmd.action = this_cmd64.action; 
       temp_cmd.length = this_cmd64.length;
       temp_cmd.acct_type = this_cmd64.acct_type;
       temp_cmd.select = this_cmd64.select;
       fwrite(&temp_cmd, sizeof(cmd128_t), 1, bin_file);
#endif // WIDTH64

      }
    }
#ifdef DEBUG
    sprintf(input_str, " The End of Day %d \n", counter);
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout,
	       "Processing Deposit Accounts\n");
#endif
    //NOTE: Prime rate table needs to be updated before processing
    //      credit card accounts
#ifdef DEBUG
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, "Updating Prime Rate Table\n");
#endif
    prime_rate_change = updatePrimeRateFile(prime_rate, prime_rate_file_ptr);
    //Parse the new rate to the CC Rate if needed
    if (prime_rate_change==1)
      getCCRate(cc_rate, cc_const, *prime_rate);
#ifdef DEBUG
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout,
	       "Updating Currency Conversion Rate Table\n");
#endif
    updateConversionTable(conversion_rate, conversion_rate_file_ptr);
#ifdef DEBUG
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, "Updating CD Rate Table\n");
#endif
    updateDepositRateTable(deposit_rate, deposit_rate_file_ptr);
#ifdef DEBUG
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout,
	       "Updating Mortgage Rate Table\n");
#endif
    updateMortgageRate(mort_rate_file_ptr, mort_rate);
#ifdef DEBUG
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, "Updating TCM Rate Table\n");
#endif
    updateTCMRateFile(tcm_rate, tcm_rate_file_ptr);
    //Handle deposit account (CD first, then Checking)
    endOfTheDayDeposit(cd_table, checking_table, fout, deposit_rate);
#ifdef DEBUG
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout,
	       "Processing Mortgage Accounts\n");
#endif
    //Handle mortgage account
    endOfTheDayMortgage(mortgage_table, checking_table, 
			fout, conversion_rate, deposit_rate, mort_rate,
			*tcm_rate
#ifdef TEST
			, ascii_file, bin_file
#endif
			);
#ifdef DEBUG
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout,
	       "Processing Credit Card Accounts\n");
#endif
    endOfTheDayCreditCard(cc_table, checking_table, fout, conversion_rate, 
			  deposit_rate, cc_rate, cc_const, 
			  prime_rate_change
#ifdef TEST
			  , ascii_file, bin_file
#endif
			  );
    //Lastly, increment the day count
    counter++;
    fprintf(stderr, "End of Day %d\n", counter);
    fprintf(ascii_file, "%-16s %-3d %-19s %-6d %-8d %-8d %-19s\n",
	    "0", 0, "0", 0, 0, 0, "0");
#ifdef DEBUG
    fprintf(stderr, "%-16s %-3d %-19s %-6d %-8d %-8d %-19s\n",
	    "0", 0, "0", 0, 0, 0, "0");
#endif
    
    #ifdef WIDTH64
     strcpy(this_cmd64.acct_no, "0");
     this_cmd64.action = 0;
     this_cmd64.length = 0;
     this_cmd64.acct_type = 0;
     this_cmd64.select = 0;
    #else
     cmd128_t temp_cmd;
     strcpy(temp_cmd.acct_no, "0");
     temp_cmd.action = 0;
     temp_cmd.length = 0;
     temp_cmd.acct_type = 0;
     temp_cmd.select = 0;
    #endif
    #ifdef DECNUMBER_ARBITRARY
     #ifdef WIDTH64
       decimal64FromString(&this_cmd64.fund, "0", setp);
       decimal64FromString(&this_cmd64.str1, "0", setp);
     #else
       decimal128FromString(&temp_cmd.fund, "0", setp);
       decimal128FromString(&temp_cmd.str1, "0", setp);
     #endif // WIDTH
    #else
     #ifdef WIDTH64
       ZERO64((this_cmd64.fund));
       ZERO64((this_cmd64.str1));
     #else // 128
     // This is a mess. Could be cleaner
      #ifdef DECNUMBER_FIXED
       decQuadZero(&(temp_cmd.fund));
       decQuadZero(&(temp_cmd.str1));
      #else // BID
       (temp_cmd.fund).w[1] = 0x3040000000000000ull; (temp_cmd.fund).w[0] = 0ull;
       (temp_cmd.str1).w[1] = 0x3040000000000000ull; (temp_cmd.str1).w[0] = 0ull;
      #endif
     #endif
    #endif
    #ifdef WIDTH64
     fwrite(&this_cmd64, sizeof(cmd64_t), 1, bin_file);
    #else
     fwrite(&temp_cmd, sizeof(cmd128_t), 1, bin_file);
    #endif
  }
  return 0;
}

int main(int argc, char *argv[]){
  htab *cd_table, *checking_table, *mortgage_table, *cc_table;
  context_t set;
  FILE *conversion_rate_file_ptr[COUNTRIES*2];
  FILE *deposit_rate_file_ptr[COUNTRIES+1];
  deposit_rate_t deposit_rate[COUNTRIES+1];
  dec64_t *conversion_rate[2*COUNTRIES];
  FILE *mort_rate_file_ptr[4];
  FILE *tcm_rate_file_ptr;
  FILE *prime_rate_file_ptr;
  mort_rate_t mort_rate;
  //Credit Card
  cc_const_t cc_const;
  cc_rate_t cc_rate;
  dec64_t tcm_rate;
  dec64_t prime_rate;
  cmd_t this_cmd;
  /*
    FOR TEST GENERATOR
  */
  char ascii_file_str[PATHNAME_MAX_LENGTH], bin_file_str[PATHNAME_MAX_LENGTH];
  FILE *ascii_file, *bin_file;
  int max_acct, min_tx, max_tx, days;


  //random number seed
 // srand(time(NULL));
  srand(14);
  if (argc!=6){
    fprintf(stderr, "[USAGE] cmd cmd_file max_acct min_tx max_tx #days\n");
    exit(-1);
  }
#if defined(BID)
  FILEOPEN(tcm_rate_file_ptr, "../datafile/bin/1YearTCM.txt.bid", "rb");
  FILEOPEN(prime_rate_file_ptr, "../datafile/bin/Prime_rate.txt.bid", "rb");
#else
  FILEOPEN(tcm_rate_file_ptr, "../datafile/bin/1YearTCM.txt.b64", "rb");
  FILEOPEN(prime_rate_file_ptr, "../datafile/bin/Prime_rate.txt.b64", "rb");
#endif
  /*
    FOR TEST GENERATOR
  */
  sprintf(ascii_file_str, "ascii/%s.txt", argv[1]);
  sprintf(bin_file_str, "bin/%s.bin", argv[1]);
  FILEOPEN(ascii_file, ascii_file_str, "wb");
  FILEOPEN(bin_file, bin_file_str, "wb");
  max_acct = atoi(argv[2]);
  min_tx   = atoi(argv[3]);
  max_tx   = atoi(argv[4]);
  days     = atoi(argv[5]);

#ifdef BID
  set = BID_ROUNDING_TO_NEAREST;
#else
 #if defined(WIDTH64)
  decContextDefault(&set, DEC_INIT_DECIMAL64);
 #else
  decContextDefault(&set, DEC_INIT_DECIMAL128);
 #endif
#endif //BID
  setp = &set;
  counter = 0;
  fprintf(stderr, "Start of the Banking benchmark Generator\n");
  ZERO64(prime_rate);
  initialization(conversion_rate_file_ptr, deposit_rate_file_ptr,
		 mort_rate_file_ptr, tcm_rate_file_ptr,
                 prime_rate_file_ptr, 
		 deposit_rate, conversion_rate, 
		 &mort_rate, &tcm_rate, &prime_rate,
		 &cc_const, &cc_rate);

  cd_table = hcreate(8);          /* create hash table */
  checking_table = hcreate(8);
  mortgage_table = hcreate(8);
  cc_table = hcreate(8);

  processing(cd_table, checking_table, mortgage_table, cc_table,
	     deposit_rate, //cmd_file_ptr, 
	     &this_cmd, &mort_rate,
	     conversion_rate, &tcm_rate, &cc_const, &cc_rate, &prime_rate,
	     prime_rate_file_ptr,
	     conversion_rate_file_ptr, deposit_rate_file_ptr,
	     mort_rate_file_ptr, tcm_rate_file_ptr,
	     stderr
	     /* FOR Test Generator */ ,
	     ascii_file, bin_file, max_acct, min_tx, max_tx, days
	     );
  fprintf(stderr, "The number of CD accounts: %d\n", (int)hcount(cd_table));
#ifdef DEBUG
  fprintf(stderr, "The active CD accounts includes:\n");
#endif
  while (hcount(cd_table)){  /* while the table is not empty */
#ifdef DEBUG
    deposit_acct_t *this_acct;
    this_acct = (deposit_acct_t *)hstuff(cd_table);
    fprintf(stderr, "%s, ", this_acct->acct_no);
#endif
    free(hstuff(cd_table));
    free(hkey(cd_table));            /* free memory for the line */
    hdel(cd_table);                /* delete from hash table */
  }
  fprintf(stderr, "\nThe number of Checking accounts: %d\n", (int)hcount(checking_table));
#ifdef DEBUG
  fprintf(stderr, "The active Checking accounts includes:");
#endif
  while (hcount(checking_table)){  /* while the table is not empty */
#ifdef DEBUG
    deposit_acct_t *this_acct;
    this_acct = (deposit_acct_t *)hstuff(checking_table);
    fprintf(stderr, "%s, ", this_acct->acct_no);
#endif
    free(hstuff(checking_table));
    free(hkey(checking_table));            /* free memory for the line */
    hdel(checking_table);                /* delete from hash table */
  }
  fprintf(stderr, "\nThe number of Mortgage accounts: %d\n", (int)hcount(mortgage_table));
#ifdef DEBUG
  fprintf(stderr, "The active Mortgage accounts includes:");
#endif
  while (hcount(mortgage_table)){  /* while the table is not empty */
#ifdef DEBUG
    mort_acct_t *this_acct;
    this_acct = (mort_acct_t *)hstuff(mortgage_table);
    fprintf(stderr, "%s, ", this_acct->acct_no);
#endif
    free(hstuff(mortgage_table));
    free(hkey(mortgage_table));            /* free memory for the line */
    hdel(mortgage_table);                /* delete from hash table */
  }
  fprintf(stderr, "\nThe number of Credit Card accounts: %d\n", (int)hcount(cc_table));
#ifdef DEBUG
  fprintf(stderr, "The active Credit Card accounts includes:");
#endif
  while (hcount(cc_table)){  /* while the table is not empty */
#ifdef DEBUG
    cc_acct_t *this_acct;
    this_acct = (cc_acct_t *)hstuff(cc_table);
    fprintf(stderr, "%s, ", this_acct->acct_no);
#endif
    free(hstuff(cc_table));
    free(hkey(cc_table));            /* free memory for the line */
    hdel(cc_table);                /* delete from hash table */
  }
  fprintf(stderr, "\n");

  hdestroy(cd_table);                         /* destroy hash table */
  hdestroy(checking_table);                         /* destroy hash table */
  hdestroy(mortgage_table);
  hdestroy(cc_table);
  fprintf(stderr, "End of the Banking benchmark\n");
  return SUCCESS;
}
