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
// Id: $Id: deposit.c,v 1.9 2009-05-18 03:07:53 mjanders Exp $
// Date: $Date: 2009-05-18 03:07:53 $
// Author: $Author: mjanders $
// =============================================================================

#include "const.h" 
#include "deposit.h"
#include "decfcn.h"
#include "rdtsc.h"

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
extern unsigned long long rsc_rounded;
extern unsigned long long div_rounded;

extern unsigned long long count_begin_func;
extern unsigned long long count_end_func;
extern unsigned long long count_overhead;
// END COUNTING MICHAEL!!!

extern int counter;
extern dec_t MTWO, MFOUR, HUNDRED, YEAR, NON_CUSTOMER_FIXED_RATE, 
  CENT;
#ifdef NEW_FEATURES
//for deposit
extern dec_t daily_deposit_withdrawl[360], daily_deposit_deposit[360],
  daily_deposit_cash_flow[360], daily_deposit_interest[360],
  daily_deposit_other[360],
  monthly_deposit_withdrawl, monthly_deposit_deposit,
  monthly_deposit_cash_flow, monthly_deposit_interest, monthly_deposit_other,
  yearly_deposit_withdrawl, yearly_deposit_deposit,
  yearly_deposit_cash_flow, yearly_deposit_interest, yearly_deposit_other,
  ytd_deposit_deposit, ytd_deposit_withdrawl,
  ytd_deposit_cash_flow, ytd_deposit_interest, ytd_deposit_other;
#endif

// Deposit Account Open
// Include both CD and Checking
int deposit_acct_open(htab *t, char *acct_no, dec_t fund, int period,
		      int acct_type, int currency, int pay_date,
		      FILE *fout, deposit_rate_t *deposit_rate
#ifdef NEW_FEATURES
		      , dec_t **conversion_rate
#endif
){
  int keyl;
  ub1 *key;
  deposit_acct_t *this_acct;
  char input_str[STR_MAX_LENGTH];
  keyl = mystrlen((char *)acct_no);
  if (hadd(t, acct_no, keyl, (void *)0)==FALSE) { /* if not a duplicate */
    mysprintf(input_str, "[deposit_acct_open] cannot add %s's account\n", 
	    acct_no);
    errorPrint(LOC, WARNING_CODE, ADD_ACCT_FAIL, fout, input_str);
    myfprintf(stderr, input_str);
    return ADD_ACCT_FAIL;
  }
  key = (ub1 *)mymalloc(keyl);       /* dumb use of malloc */
  mymemcpy(key, acct_no, keyl);          /* copy buf into key */
  hkey(t)=key;                     /* replace buf with key */
  this_acct = mycalloc(1, sizeof(deposit_acct_t));
  deposit_acct_init(this_acct, acct_no, fund, period, acct_type, currency, 
		    pay_date);
  get_deposit_rate(deposit_rate, this_acct);
  hstuff(t)=(void*)this_acct;
#ifdef NEW_FEATURES
  {
    dec_t converted_amt;
    if (currency!=COUNTRIES){//not the USD
      MULTIPLY64(converted_amt, fund, conversion_rate[currency][0], setp);
      roundToAvailableCoin(&converted_amt, converted_amt, 
			   conversion_rate[currency+COUNTRIES][1]);
    }
    else 
      COPY64(converted_amt, fund);
    ADD64(daily_deposit_deposit[counter%360], 
	  daily_deposit_deposit[counter%360], converted_amt, setp);
  }
#endif
  return SUCCESS;
}

int deposit_acct_close(htab *t, char *acct_no, dec_t **conversion_rate,
                       int acct_type, FILE *fout,
		       deposit_rate_t *deposit_rate){
  dec_t penalty;
  deposit_acct_t *this_acct;
  char input_str[STR_MAX_LENGTH];
  if (hfind(t, acct_no, strlen(acct_no))==FALSE){
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (deposit_acct_t *)hstuff(t);
  mysprintf(input_str, "##### Account %s is going to close", this_acct->acct_no);
  ZERO64(penalty);
  //this covers also the GRACE PERIOD
  if ((this_acct->acct_type==CD) & (this_acct->maturity_date>counter)){
    char penalty_str[STR_MAX_LENGTH];
    penaltyComputation(&penalty, conversion_rate, this_acct,
		       &deposit_rate[this_acct->currency]);
    TOSTRING64(penalty, penalty_str);
    mystrcat(input_str, " PREMATURELY #####\n");
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
    mysprintf(input_str,
            "User needs to pay total penalty (fixed surcharge is $%s):$%s\n",
            PENALTY_SURCHARGE, penalty_str);
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
  }
  else {
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, " ######\n");
  }
#ifdef NEW_FEATURES
  {
    dec_t converted_penalty, converted_balance;
    ADD64(this_acct->current_balance, this_acct->current_balance,
	  this_acct->unposted_interest, setp);
    if (this_acct->currency!=COUNTRIES) {//not the USD
      MULTIPLY64(converted_penalty, penalty, 
		 conversion_rate[this_acct->currency][0], setp);
      roundToAvailableCoin(&converted_penalty, converted_penalty,
                           conversion_rate[this_acct->currency+COUNTRIES][1]);
      MULTIPLY64(converted_balance, this_acct->current_balance,
                 conversion_rate[this_acct->currency][0], setp);
      roundToAvailableCoin(&converted_balance, converted_balance,
                           conversion_rate[this_acct->currency+COUNTRIES][1]);
    }
    else {
      COPY64(converted_penalty, penalty);
      COPY64(converted_balance, this_acct->current_balance);
    }
    ADD64(daily_deposit_other[counter%360], daily_deposit_other[counter%360],
	  converted_penalty, setp);
    ADD64(daily_deposit_withdrawl[counter%360], 
	  daily_deposit_other[counter%360], converted_balance, setp);
  }
#endif
  myfree(hkey(t));
  myfree(hstuff(t));
  hdel(t);
  return SUCCESS;
}

int deposit_mark_extend(htab *t, char *acct_no, FILE *fout){
  deposit_acct_t *this_acct;
  if (hfind(t, acct_no, strlen(acct_no))==FALSE){
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (deposit_acct_t *)hstuff(t);
  this_acct->extend = 1;
  return SUCCESS;
}

int deposit_mark_close(htab *t, char *acct_no, FILE *fout){
  deposit_acct_t *this_acct;
  if (hfind(t, acct_no, strlen(acct_no))==FALSE){
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (deposit_acct_t *)hstuff(t);
  this_acct->set_close = 1;
  return SUCCESS;
}

int deposit_acct_inquiry(htab *t, char *acct_no, FILE *fout){
  deposit_acct_t *this_acct;
  char str[STR_MAX_LENGTH];
  if (hfind(t, acct_no, strlen(acct_no))==FALSE){
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (deposit_acct_t *)hstuff(t);
  myfprintf(fout, "============== DEPOSIT ACCOUNT REPORT ===============\n");
  myfprintf(fout, "Account Number: %s\n", this_acct->acct_no);
  myfprintf(fout, "Account Type: %s\n",
          (this_acct->acct_type==CD)?"CD":"CHECKING");
  if (this_acct->acct_type==CD)
    myfprintf(fout, "Account Period: %d month(s)\n", this_acct->period);
  myfprintf(fout, "Account Opening Date: %d\n", this_acct->open_date);
  if (this_acct->acct_type==CD)
    myfprintf(fout, "Account Mature Date: %d\n", this_acct->maturity_date);
  myfprintf(fout, "Account Pay Date: Every %d days\n", this_acct->pay_date);
  if (this_acct->acct_type==CD){
    myfprintf(fout, "Account Close Flag: %s\n",
            (this_acct->set_close==1)?"SET":"DISABLE");
    myfprintf(fout, "Account Extend Flag: %s\n",
            (this_acct->extend==1)?"SET":"DISABLE");
  }
  if ((ISZERO64(this_acct->last_withdrawl))||
      (ISNEGATIVE64(this_acct->last_withdrawl)))
    myfprintf(fout, "Account Last Withdrawl: Not Available\n");
  else {
    TOSTRING64(this_acct->last_withdrawl, str);
    myfprintf(fout, "Account Last Withdrawl: %s", str);
    checkName(str, this_acct->last_withdrawl_unit);
    myfprintf(fout, " in %s\n", str);
  }
  if ((ISNEGATIVE64(this_acct->last_deposit))||
      (ISZERO64(this_acct->last_deposit)))
    myfprintf(fout, "Account Last Deposit: Not Available\n");
  else {
    TOSTRING64(this_acct->last_deposit, str);
    myfprintf(fout, "Account Last Deposit: %s", str);
    checkName(str, this_acct->last_deposit_unit);
    myfprintf(fout, " in %s\n", str);
  }
  TOSTRING64(this_acct->rate, str);
  myfprintf(fout, "Account Current Daily Rate: %s percentage \n", str);
  TOSTRING64(this_acct->unposted_interest, str);
  myfprintf(fout, "Account Accumulate Interest during this pay period: %s\n",
          str);
  TOSTRING64(this_acct->interest, str);
  myfprintf(fout, "Account Total Interest: %s\n", str);
  if ((ISNEGATIVE64(this_acct->last_statement))||
      (ISZERO64(this_acct->last_statement)))
    mystrcpy(str, "Not Available");
  else
    TOSTRING64(this_acct->last_statement, str);
  myfprintf(fout, "Acount Last Statement: %s\n", str);
  TOSTRING64(this_acct->current_balance, str);
  myfprintf(fout, "Account Current Balance: %s", str);
  checkName(str, this_acct->currency);
  myfprintf(fout, " in %s\n", str);
  myfprintf(fout, "========================================================\n");
  return SUCCESS;
}

int money_deposit(htab *t, char *acct_no, dec_t fund, 
		  dec_t **conversion_rate, 
		  int deposit_unit, FILE *fout){
  int bias = 0;
  dec_t temp_value;
  deposit_acct_t *this_acct;
  if (hfind(t, acct_no, strlen(acct_no))==FALSE){
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (deposit_acct_t *)hstuff(t);
  this_acct->last_deposit_unit = deposit_unit;
  COPY64(this_acct->last_deposit, fund);
  if (deposit_unit != this_acct->currency){
    //first convert the withdrawl amount to the acct's currency
    dec_t transaction;
    conversionUS(deposit_unit, this_acct->currency,
                 fund, &temp_value,
                 &transaction, conversion_rate);
  }
  else {
    COPY64(temp_value, fund);
  }
  ADD64(this_acct->current_balance, this_acct->current_balance, temp_value,
        setp);
  if (this_acct->currency!=COUNTRIES)
    bias = COUNTRIES;
  roundToAvailableCoin(&this_acct->current_balance,
                       this_acct->current_balance,
                       conversion_rate[this_acct->currency+bias][1]);

#ifdef NEW_FEATURES
  {
    dec_t converted_amt;
    if (deposit_unit!=COUNTRIES){
      MULTIPLY64(converted_amt, fund, conversion_rate[deposit_unit][0],
		 setp);
      roundToAvailableCoin(&converted_amt, converted_amt, 
			   conversion_rate[deposit_unit+bias][1]);
    }
    else 
      COPY64(converted_amt, fund);
    ADD64(daily_deposit_deposit[counter%360],
	  daily_deposit_deposit[counter%360], converted_amt, setp);
  }
#endif
  return SUCCESS;
}

int money_withdrawl(htab *t, char *acct_no, int withdrawl_unit,
                    dec_t money, dec_t **conversion_rate, FILE* fout,
                    deposit_rate_t *deposit_rate){
  deposit_acct_t *this_acct;
  dec_t penalty, temp_value, temp_current_balance;
#ifdef DEBUG
  char fromStr[LENGTH_T], toStr[LENGTH_T];
#endif
  if (hfind(t, acct_no, strlen(acct_no))==FALSE){
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (deposit_acct_t *)hstuff(t);
#ifdef CD_WITHDRAWL_SAME_CURRENCY
  if ((acct_type==CD)&(withdrawl_unit!=this_acct->currency)){
    char input_str[STR_MAX_LENGTH];
    errorPrint(LOC, WARNING_CODE, WRONG_CURRENCY,
               stderr,
               "Not allow to withdrawl CD account with different currency\n"\
	       );
    mysprintf(input_str, 
	      "###### %s FAILs to withdrawl his %s account ######\n",
	      this_acct->acct_no);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
    return WRONG_CURRENCY;
  }
#endif
  //Before the User or the System extends the account, 
  //user can withdrawl money w/o penalty
  if ((this_acct->acct_type==CD)&(this_acct->maturity_date > counter))
    penaltyComputation(&penalty, conversion_rate, this_acct,
		       &deposit_rate[this_acct->currency]);
  else  //accomodate CD with grace period and Checking account
    ZERO64(penalty);
  //FIRST, add the unposted balance to the current balance
#ifdef DEBUG
  {
    TOSTRING64(this_acct->current_balance, fromStr);
    myfprintf(fout, "The original balance is %s\n", fromStr);
    TOSTRING64(this_acct->unposted_interest, fromStr);
    myfprintf(fout, "The unposted interest is %s\n", fromStr);
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, 
	       "Unposted interest goes into current balance immediately\n");
  }
#endif
   ADD64(temp_current_balance, this_acct->current_balance,
	 this_acct->unposted_interest, setp);
   //Now there is no fee for withdrawl with different currency
   //Since penalty is in the original currency unit, 
   //we need to subtract it from the balance first
   SUBTRACT64(temp_current_balance, temp_current_balance, penalty, setp);
#ifdef DEBUG
   checkName(fromStr, withdrawl_unit);
   checkName(toStr, this_acct->currency);
#endif
 if (withdrawl_unit != this_acct->currency){
   //first convert the withdrawl amount to the acct's currency
   dec_t transaction;
   conversionUS(withdrawl_unit, this_acct->currency, money,
		&temp_value, &transaction, conversion_rate);
 }
 else {
   COPY64(temp_value, money);
 }
#ifdef DEBUG
 {
   char str[LENGTH_T];
   TOSTRING64(temp_value, str);
   myfprintf(fout, "The converted amount is %s\n", str);
 }
#endif
 SUBTRACT64(temp_current_balance, temp_current_balance, temp_value, setp);
 if (ISNEGATIVE64(temp_current_balance)) {
   errorPrint(LOC, WARNING_CODE, NOT_ENOUGH_BALANCE,
	      stderr,
	      "Deposit account balance is not enough for withdrawl.\n");
#ifdef DEBUG
   {
     char input_str[STR_MAX_LENGTH];
     mysprintf(input_str,
	     "###### %s FAILs to withdrawl his %s account ######\n",
	     this_acct->acct_no,
	     (this_acct->acct_type==CD)?"CD":"Checking");
     errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
   }
#endif
   return NOT_ENOUGH_BALANCE;
 }
 else {
   //successfully withdrawl
   int bias = 0;
   COPY64(this_acct->last_withdrawl, money);
   this_acct->last_withdrawl_unit = withdrawl_unit;
   ZERO64(this_acct->unposted_interest);
   COPY64(this_acct->current_balance, temp_current_balance);
   if (this_acct->currency!=COUNTRIES)
     bias = this_acct->currency+COUNTRIES;
   else
     bias = 0;
   roundToAvailableCoin(&this_acct->current_balance,
			this_acct->current_balance,
			conversion_rate[bias][1]);
#ifdef DEBUG
   {
     char str[LENGTH_T];
     char input_str[STR_MAX_LENGTH];
     TOSTRING64(money, str);
     mysprintf(input_str,
	     "##### %s WITHDRAWL %s %s from its %s account #####\n",
	     this_acct->acct_no, fromStr, str, (this_acct->acct_type==CD)?
	     "CD":"Checking");
     errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
     TOSTRING64(penalty, str);
     mysprintf(input_str,
              "Total withdrawl penalty (fixed surcharge: $%s) is $%s %s\n",
	     PENALTY_SURCHARGE, str, toStr);
     errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
   }
#endif
 }
#ifdef NEW_FEATURES
 {
   dec_t converted_amt, converted_penalty;
   if (withdrawl_unit!=COUNTRIES){
     MULTIPLY64(converted_amt, money, conversion_rate[withdrawl_unit][0],
		setp);
     roundToAvailableCoin(&converted_amt, converted_amt,
			  conversion_rate[withdrawl_unit+COUNTRIES][1]);
   }
   else
     COPY64(converted_amt, money);
   if (this_acct->currency!=COUNTRIES){
     MULTIPLY64(converted_penalty, penalty, 
		conversion_rate[this_acct->currency][0],
                setp);
     roundToAvailableCoin(&converted_penalty, converted_penalty,
                          conversion_rate[this_acct->currency+COUNTRIES][1]);
   }
   else 
     COPY64(converted_penalty, penalty);
   ADD64(daily_deposit_withdrawl[counter%360],
	 daily_deposit_withdrawl[counter%360], converted_amt, setp);
   ADD64(daily_deposit_other[counter%360],
	 daily_deposit_other[counter%360], converted_penalty, setp);
 }
#endif
 return SUCCESS;
}

void endOfTheDayDeposit(htab *cd_table, htab *checking_table, 
			FILE *fout, deposit_rate_t *deposit_rate
#ifdef NEW_FEATURES
			, dec_t **conversion_rate
#endif
){
  deposit_acct_t *this_acct;
  //First handle the checking account
  if (hfirst(checking_table)){
    do {
      //first, accumulate the interest
      //this also handles if it meets the pay date
      //it also updates the current balance if needed
      //This fcn records the acct as well.
      this_acct = (deposit_acct_t *)hstuff(checking_table);
      deposit_payInterest(this_acct, fout
#ifdef NEW_FEATURES
                          ,conversion_rate
#endif
);
    }while (hnext(checking_table));                    /* go to next element */
  }
  if (hfirst(cd_table)){
    do {
      this_acct = (deposit_acct_t *)hstuff(cd_table);
      deposit_payInterest(this_acct, fout
#ifdef NEW_FEATURES
			  ,conversion_rate
#endif
);
      if ((this_acct->maturity_date <= counter) &&
          (this_acct->maturity_date+GRACE_PERIOD > counter)) {
        if (this_acct->set_close==1) {
	  char input_str[STR_MAX_LENGTH];
	  mysprintf(input_str, "##### Account %s is going to close #####", 
		  this_acct->acct_no);
	  errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
#ifdef NEW_FEATURES
	  {
	    dec_t converted_balance;
	    ADD64(this_acct->current_balance, this_acct->current_balance,
		  this_acct->unposted_interest, setp);
	    if (this_acct->currency!=COUNTRIES) {//not the USD
	      MULTIPLY64(converted_balance, this_acct->current_balance,
			 conversion_rate[this_acct->currency][0], setp);
	      roundToAvailableCoin(&converted_balance, converted_balance,
				   conversion_rate[this_acct->currency+COUNTRIES][1]);
	    }
	    else {
	      COPY64(converted_balance, this_acct->current_balance);
	    }
	    ADD64(daily_deposit_withdrawl[counter%360],
		  daily_deposit_other[counter%360], converted_balance, setp);
	  }
#endif
	  myfree(hkey(cd_table));
	  myfree(hstuff(cd_table));
	  hdel(cd_table);
	}
	else if (this_acct->extend==1) {//User exteneds
          this_acct->extend = 0;
          get_deposit_rate(&deposit_rate[this_acct->currency], this_acct);
          ADD64(this_acct->current_balance, this_acct->current_balance,
                this_acct->unposted_interest, setp);
          ZERO64(this_acct->unposted_interest);
          this_acct->open_date = counter;
          this_acct->maturity_date = counter + this_acct->period * 30;
        }
      }
      else if (counter == this_acct->maturity_date + GRACE_PERIOD){
        //default extend on Grace_day
        //get the current rate
        this_acct->extend = 0;
        get_deposit_rate(&deposit_rate[this_acct->currency], this_acct);
        ADD64(this_acct->current_balance, this_acct->current_balance,
              this_acct->unposted_interest, setp);
        ZERO64(this_acct->unposted_interest);
        this_acct->open_date = counter - GRACE_PERIOD;
        this_acct->maturity_date = counter - GRACE_PERIOD +
          this_acct->period * 30;
      }
    }while (hnext(cd_table));
  }
  //reset the next daily counter
#ifdef NEW_FEATURES
  SUBTRACT64(daily_deposit_cash_flow[counter], daily_deposit_deposit[counter],
	     daily_deposit_withdrawl[counter], setp);
  ADD64(daily_deposit_cash_flow[counter], daily_deposit_cash_flow[counter],
	daily_deposit_interest[counter], setp);
  ADD64(daily_deposit_cash_flow[counter], daily_deposit_cash_flow[counter],
        daily_deposit_other[counter], setp);
  {
    char strA[30];
    TOSTRING64(daily_deposit_cash_flow[counter], strA);
    myfprintf(stderr, "daily cash flow is %s\n", strA);    
  }
  if ((counter%30==0)||(counter!=0)){
    int index;
    ZERO64(monthly_deposit_deposit);
    for (index=counter; index>counter-30; index--)
      ADD64(monthly_deposit_deposit, monthly_deposit_deposit, 
	    daily_deposit_deposit[(index)%360], setp);
    ZERO64(monthly_deposit_withdrawl);
    for (index=counter; index>counter-30; index--)
      ADD64(monthly_deposit_withdrawl, monthly_deposit_withdrawl,
            daily_deposit_withdrawl[(index)%360], setp);
    ZERO64(monthly_deposit_cash_flow);
    for (index=counter; index>counter-30; index--)
      ADD64(monthly_deposit_cash_flow, monthly_deposit_cash_flow,
            daily_deposit_cash_flow[(index)%360], setp);
    ZERO64(monthly_deposit_interest);
    for (index=counter; index>counter-30; index--)
      ADD64(monthly_deposit_interest, monthly_deposit_interest,
            daily_deposit_interest[(index)%360], setp);
    ZERO64(monthly_deposit_other);
    for (index=counter; index>counter-30; index--)
      ADD64(monthly_deposit_other, monthly_deposit_other,
            daily_deposit_other[(index)%360], setp);
  }
  if ((counter%360==0)||(counter!=0)){
    int index;
    ZERO64(yearly_deposit_deposit);
    for (index=counter; index>counter-360; index--)
      ADD64(yearly_deposit_deposit, yearly_deposit_deposit,
            daily_deposit_deposit[(index)%360], setp);
    ZERO64(yearly_deposit_withdrawl);
    for (index=counter; index>counter-360; index--)
      ADD64(yearly_deposit_withdrawl, yearly_deposit_withdrawl,
            daily_deposit_withdrawl[(index)%360], setp);
    ZERO64(yearly_deposit_cash_flow);
    for (index=counter; index>counter-360; index--)
      ADD64(yearly_deposit_cash_flow, yearly_deposit_cash_flow,
            daily_deposit_cash_flow[(index)%360], setp);
    ZERO64(yearly_deposit_interest);
    for (index=counter; index>counter-360; index--)
      ADD64(yearly_deposit_interest, yearly_deposit_interest,
            daily_deposit_interest[(index)%360], setp);
    ZERO64(yearly_deposit_other);
    for (index=counter; index>counter-360; index--)
      ADD64(yearly_deposit_other, yearly_deposit_other,
            daily_deposit_other[(index)%360], setp);
  }
  if (counter<360){
    ADD64(ytd_deposit_deposit, ytd_deposit_deposit, 
	  daily_deposit_deposit[counter], setp);
    ADD64(ytd_deposit_withdrawl, ytd_deposit_withdrawl,
          daily_deposit_withdrawl[counter], setp);
    ADD64(ytd_deposit_interest, ytd_deposit_interest,
          daily_deposit_interest[counter], setp);
    ADD64(ytd_deposit_cash_flow, ytd_deposit_cash_flow,
          daily_deposit_cash_flow[counter], setp);
    ADD64(ytd_deposit_other, ytd_deposit_other,
          daily_deposit_other[counter], setp);
  }
  else {
    ADD64(ytd_deposit_deposit, ytd_deposit_deposit, 
	  daily_deposit_deposit[counter%360], setp);
    SUBTRACT64(ytd_deposit_deposit, ytd_deposit_deposit,
	       daily_deposit_deposit[(counter+1)%360], setp);
    ADD64(ytd_deposit_withdrawl, ytd_deposit_withdrawl,
	  daily_deposit_withdrawl[counter%360], setp);
    SUBTRACT64(ytd_deposit_withdrawl, ytd_deposit_withdrawl,
               daily_deposit_withdrawl[(counter+1)%360], setp);
    ADD64(ytd_deposit_interest, ytd_deposit_interest,
	  daily_deposit_interest[counter%360], setp);
    SUBTRACT64(ytd_deposit_interest, ytd_deposit_interest,
               daily_deposit_interest[(counter+1)%360], setp);
    ADD64(ytd_deposit_cash_flow, ytd_deposit_cash_flow,
	  daily_deposit_cash_flow[counter%360], setp);
    SUBTRACT64(ytd_deposit_cash_flow, ytd_deposit_cash_flow,
               daily_deposit_cash_flow[(counter+1)%360], setp);
    ADD64(ytd_deposit_other, ytd_deposit_other,
	  daily_deposit_other[counter%360], setp);
    SUBTRACT64(ytd_deposit_other, ytd_deposit_other,
               daily_deposit_other[(counter+1)%360], setp);
  }

  ZERO64(daily_deposit_deposit[(counter+1)%360]);
  ZERO64(daily_deposit_withdrawl[(counter+1)%360]);
  ZERO64(daily_deposit_other[(counter+1)%360]);
  ZERO64(daily_deposit_interest[(counter+1)%360]);
#endif
}

int wireTransfer(htab *t, int fromUnit, char *acct_no,
		 int to, dec_t in, dec_t *out, dec_t *transaction,
                 dec_t *processFee, dec_t **conversion_rate, FILE *fout){
#ifdef DEBUG
  char inUnit[STR_MAX_LENGTH], outUnit[STR_MAX_LENGTH];
#endif
  dec_t temp2;
  char input_str[STR_MAX_LENGTH];
  int from;
  if (mystrcmp(acct_no, "NON_CUSTOMER")!=0){//Is our customer
    deposit_acct_t *this_acct;
    if (hfind(t, acct_no, strlen(acct_no))==FALSE){
      char input_str[STR_MAX_LENGTH];
      mysprintf(input_str, "Cannot find %s's account\n", acct_no);
      errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
      return ACCT_NOT_EXIST;
    }
    this_acct = (deposit_acct_t *)hstuff(t);
    from = this_acct->currency;
    ADD64(this_acct->current_balance, this_acct->current_balance,
          this_acct->unposted_interest, setp);
    ZERO64(this_acct->unposted_interest);
    if (this_acct->currency!=to)//not same currency unit
      conversionUS(from, to, in, out, transaction,
                   conversion_rate);
    else {
      COPY64((*out), in);
    }
    SUBTRACT64(temp2, this_acct->current_balance, in, setp);
    //The remaining balance needs to be checked!
    if(ISNEGATIVE64(temp2)){
      mysprintf(input_str, "###### This wire transfer cannot be made due to lack of money in %s's account ######\n",
              this_acct->acct_no);
      errorPrint(LOC, WARNING_CODE, CD_CK_NO_MONEY, stderr, input_str);
      return CD_CK_NO_MONEY;
    }
    else if (ISZERO64(temp2)){
#ifdef DEBUG
      mysprintf(input_str, "%s's account will have zero balance after this wire transfer\n",
              this_acct->acct_no);
      errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
    }
    else {
      COPY64(this_acct->current_balance, temp2);
      COPY64(this_acct->last_withdrawl, in);
      this_acct->last_withdrawl_unit = this_acct->currency;
    }
    ZERO64((*processFee));
#ifdef DEBUG
    {
      char fromStr[LENGTH_T], toStr[LENGTH_T], processStr[LENGTH_T];
      checkName(inUnit, from);
      checkName(outUnit, to);
      TOSTRING64(in, fromStr);
      TOSTRING64((*out), toStr);
      TOSTRING64((*processFee), processStr);
      sprintf(input_str,
	      "###### %s wires %s in %s to %s in %s with process fee as %s in %s ######\n",
              this_acct->acct_no, fromStr, inUnit, toStr, outUnit, processStr,
              inUnit);
      errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
    }
#endif
  }
  else { //not our customer
    from = fromUnit;
    fixedExtraFee(processFee, in, conversion_rate, from);
    if (from==to){
      COPY64((*out), in);
      COPY64((*transaction), in);
    }
    else {
      conversionUS(from, to, in, out, transaction, conversion_rate);
    }
#ifdef DEBUG
    {
      char fromStr[LENGTH_T], toStr[LENGTH_T], processStr[LENGTH_T];
      checkName(inUnit, from);
      checkName(outUnit, to);
      TOSTRING64(in, fromStr);
      TOSTRING64((*out), toStr);
      TOSTRING64((*processFee), processStr);
      mysprintf(input_str, "###### Non-customer wires %s in %s to %s in %s with process fee as %s in %s ######\n",
              fromStr, inUnit, toStr, outUnit, processStr, inUnit);
      errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
    }
#endif
  }
  return SUCCESS;
}

int deposit_acct_init(deposit_acct_t *new_acct, char* acct_no, dec_t fund,
                      int period, int acct_type, int currency, int pay_date){
  int length = mystrlen(acct_no);
  if (length>MAX_ACCT_NAME_LENGTH) {
    return ACCT_LENGTH_FAIL;
  }
  mystrcpy(&new_acct->acct_no[0],acct_no);
  new_acct->period = period;
  new_acct->open_date = counter;
  new_acct->acct_type = acct_type;
  if (acct_type == CD)
    new_acct->maturity_date = counter + period *30;
  else  //normal checking account
    new_acct->maturity_date = -1;
  new_acct->pay_date = pay_date;
  new_acct->set_close = 0;
  new_acct->extend = 0;
  new_acct->acct_type = acct_type;
  new_acct->currency = currency;
  new_acct->last_withdrawl_unit = 0;
  new_acct->last_deposit_unit = 0;
  ZERO64(new_acct->interest);
  ZERO64(new_acct->unposted_interest);
  ZERO64(new_acct->last_statement);
  ZERO64(new_acct->last_deposit);
  ZERO64(new_acct->last_withdrawl);
  COPY64(new_acct->current_balance, fund);
  return SUCCESS;
}

int conversionUS(int from, int to,
                 dec_t moneyFrom, dec_t *moneyTo,
                 dec_t *transaction, dec_t **conversion_rate){
  if (ISZERO64(moneyFrom)==1){
    ZERO64(*moneyTo);
    ZERO64(*transaction);
    return SUCCESS;
  }
  if (from==to){
    COPY64((*moneyTo), moneyFrom);
    COPY64((*transaction), moneyFrom);
    return SUCCESS;
  }
  else if (from==COUNTRIES){
    // Step 1: transaction = moneyFrom * conversion_rate[to+COUNTRIES][0]
    MULTIPLY64((*transaction), moneyFrom,
               conversion_rate[to+COUNTRIES][0], setp);
    // Step 2:
    // Round to the closest available coin in target NCU
    roundToAvailableCoin(moneyTo, *transaction,
                         conversion_rate[to+COUNTRIES][1]);
  }
  else if (to==COUNTRIES){
    // Step 1: transaction = moneyFrom * conversion_rate[from][0]
    MULTIPLY64((*transaction), moneyFrom, conversion_rate[from][0], setp);
    // Step 2:
    // Round to the closest available coin in target NCU
    roundToAvailableCoin(moneyTo, *transaction, conversion_rate[from][1]);

  }
  else{ //from Non-US to Non-US
    //First, convert to US, then to the target
    MULTIPLY64((*transaction), moneyFrom, conversion_rate[from][0], setp);
    MULTIPLY64((*transaction), (*transaction), 
	       conversion_rate[to+COUNTRIES][0], setp);
    // Step 2:
    // Round to the closest available coin in target NCU
    roundToAvailableCoin(moneyTo, *transaction,
                         conversion_rate[to+COUNTRIES][1]);
  }
  return SUCCESS;
}

void fixedExtraFee(dec_t *out, dec_t in, dec_t **conversion_rate,
                   int from){
  dec_t rate;
  COPY64(rate, NON_CUSTOMER_FIXED_RATE);
  MULTIPLY64(*out, in, rate, setp);
  roundToAvailableCoin(out, *out, conversion_rate[from][1]);
}


void penaltyComputation(dec_t *penalty, dec_t **conversion_rate,
                        deposit_acct_t *this_acct,
			deposit_rate_t *deposit_rate) {
  //The penalty is computed based on the max of the default rate and 
  //the current interest rate, and the current balance
  dec_t penalty_surcharge;
  dec_t minimum_penalty_rate, penalty_rate;
  dec_t daily_rate;
  dec_t temp;
  int index;
  if (this_acct->period==1){
    DIVIDE64(daily_rate, (deposit_rate->one), YEAR, setp);
  }
  else if (this_acct->period==3){
    DIVIDE64(daily_rate, (deposit_rate->three), YEAR, setp);
  }
  else{
    DIVIDE64(daily_rate, (deposit_rate->six), YEAR, setp);
  }
  //RESCALE64(daily_rate, daily_rate, RATE_PRECISION, setp);  

  FROMSTRING64(minimum_penalty_rate, PENALTY_RATE, setp);
  MAX64(penalty_rate, minimum_penalty_rate, daily_rate, setp);
  ADD64(temp, this_acct->unposted_interest, this_acct->current_balance, setp);
  DIVIDE64(penalty_rate, penalty_rate, HUNDRED, setp);
  MULTIPLY64((*penalty), penalty_rate, temp, setp);
  FROMSTRING64(penalty_surcharge, PENALTY_SURCHARGE, setp);
  ADD64((*penalty), (*penalty), penalty_surcharge, setp);
  if (this_acct->currency==COUNTRIES)
    index = 0;
  else
    index = this_acct->currency+COUNTRIES;
  roundToAvailableCoin(penalty, *penalty, conversion_rate[index][1]);
}

void deposit_payInterest(deposit_acct_t *this_acct, FILE *fout
#ifdef NEW_FEATURES
			 ,dec_t **conversion_rate
#endif
) {
  dec_t temp;
  double comp;
  ADD64(temp, this_acct->current_balance, this_acct->unposted_interest,
        setp);
  MULTIPLY64(temp, temp, this_acct->rate, setp);
  DIVIDE64(temp, temp, HUNDRED, setp);
#ifdef NEW_FEATURES
  {
    dec_t converted_amt;
    if (this_acct->currency!=COUNTRIES){
      MULTIPLY64(converted_amt, temp, conversion_rate[this_acct->currency][0],
		 setp);
      roundToAvailableCoin(&converted_amt, converted_amt,
			   conversion_rate[this_acct->currency+COUNTRIES][1]);
    }
    else
      COPY64(converted_amt, temp);
    ADD64(daily_deposit_interest[counter%360],
	  daily_deposit_interest[counter%360], converted_amt, setp);
  }
#endif
  //RESCALE64(temp, temp, MTWO, setp);
  ADD64(this_acct->unposted_interest, this_acct->unposted_interest, temp,
        setp);
  roundToAvailableCoin(&this_acct->unposted_interest, 
		       this_acct->unposted_interest, CENT);
  comp = (counter-this_acct->open_date)%this_acct->pay_date;
  if ((comp==0)&&(counter != this_acct->open_date)){ //the interest pay day
#ifdef DEBUG
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str,
	      "####### Today is the interest pay day for %s\n",
	      this_acct->acct_no);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
#endif
    ADD64(this_acct->interest, this_acct->interest,
          this_acct->unposted_interest, setp);
    ADD64(this_acct->current_balance, this_acct->current_balance,
          this_acct->unposted_interest, setp);
    ZERO64(this_acct->unposted_interest);
    COPY64(this_acct->last_statement, this_acct->current_balance);
  }
}

void get_deposit_rate(deposit_rate_t *deposit_rate, deposit_acct_t *this_acct){
  dec_t daily_rate;
  if (this_acct->acct_type==CHECKING){
    DIVIDE64(daily_rate, (deposit_rate->checking), YEAR, setp);
  }
  else if (this_acct->period==1) {//for CD
    DIVIDE64(daily_rate, (deposit_rate->one), YEAR, setp);
  }
  else if (this_acct->period==3){
    DIVIDE64(daily_rate, (deposit_rate->three), YEAR, setp);
  }
  else if (this_acct->period==6){
    DIVIDE64(daily_rate, (deposit_rate->six), YEAR, setp);
  }
  else {
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Wrong CD period, CD period cannot be %d\n",
	      this_acct->period);
    errorPrint(LOC, ERROR_CODE, CD_BAD_PERIOD, stderr, input_str);
  }
  //RESCALE64(daily_rate, daily_rate, RATE_PRECISION, setp);
  COPY64(this_acct->rate, daily_rate);
}
 
