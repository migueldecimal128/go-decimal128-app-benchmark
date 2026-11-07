// =============================================================================
//  credit_card.c
//  Defines common functions for credit card actions (in banking benchmark)
// =============================================================================
//  Original Author : Liang-Kai Wang, 5/2007
//  Maintainer:       Chuck Tsen, 2/2009
//  Department of Electrical and Computer Engineering
//  University of Wisconsin-Madison
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
// Id: $Id: credit_card.c,v 1.6 2009-04-16 21:31:46 mjanders Exp $
// Date: $Date: 2009-04-16 21:31:46 $
// Author: $Author: mjanders $
// $Revision: 1.6 $
// =============================================================================

#include "dfp_includes.h"
#include "credit_card.h"

/////////////////////////////////////////////////////
//GLOBAL EXTERNAL VARIABLES
//The constants used in the microbenchmark
// M* means "minus *". E.g. MTWO = -2
//////////////////////////////////////////////////////
extern dec64_t ONE, MTWO, ETWO, HUNDRED, YEAR, CENT;
extern int counter;
//for all the countries (incl. US) with 1/3/6 CD interest rate
extern context_t *setp;
#if defined(NATIVE_GCC_BID)
extern _IDEC_flags flags;
#endif
int endOfTheDayCreditCard(htab *cc_table, htab *checking_table,
			  FILE *fout, dec64_t **conversion_rate,
			  deposit_rate_t *deposit_rate,
			  cc_rate_t *cc_rate, cc_const_t *cc_const,
			  int prime_rate_change
#ifdef TEST
			  , FILE *ascii_file, FILE *bin_file
#endif
)
{
  char input_str[STR_MAX_LENGTH];
  cc_acct_t *this_acct;
  if (hfirst(cc_table))
  {
    do {
      this_acct = (cc_acct_t *)hstuff(cc_table);
      if ((this_acct->intro_apr_bal_tx_end_date==-1) &&
	  (counter==this_acct->start_date+INIT_BAL_TX_PERIOD))
      {
	//this is the last date for low rate Bal Tx
	//if the user hasn't do any BAL_TX
	//Since no BAL TX so far, then we set the end date to
	//a "positive" day that only hit today.
	//so that when the prime rate changes, the Bal Tx rate changes 
	//accordingly
	this_acct->intro_apr_bal_tx_end_date = counter;
      }
      //If the end date arrive, then we use the latest BAL_TX rate
      //since it's a variable rate.
      //If there hasn't been a BAL_TX within a year, then
      //this_acct->intro_apr_bal_tx_end_date==-1
      //then the following action won't take effect!
      if(counter==this_acct->intro_apr_bal_tx_end_date)
	COPY64(this_acct->bal_tx_apr, cc_rate->bal_tx_apr);
      if (prime_rate_change==1) //prime rate changes
      { 
#ifdef DEBUG
	errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout,
		   "********** Prime Rate Changes ************\n");
#endif
	if (this_acct->overdueHistory==0)
	{
	  COPY64(this_acct->purchase_apr, cc_rate->purchase_apr);
	  COPY64(this_acct->cash_adv_apr, cc_rate->cash_adv_apr);
	  //we need the following line to cover if
	  //we doesn't hit the end date but prime rate changes
	  if ((counter>=this_acct->intro_apr_bal_tx_end_date)&&
	      (this_acct->intro_apr_bal_tx_end_date>=0)) 
	  {
	    COPY64(this_acct->bal_tx_apr, cc_rate->bal_tx_apr);
	  }
	  COPY64(this_acct->default_apr, cc_rate->default_apr);
	  if (this_acct->underpaid==1)
	  {
	    //to align those rates again
	    autoDefault(this_acct);
	  }
	}
#ifdef DEBUG
	else {
          errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout,
                     "Unfortunately, you default before. \
The rates remain at default rate\n");
        }
#endif
      }
      //Finance Charge for Cash ADV is accumulated every day
      if (ISPOSITIVE64(this_acct->cash_adv))
      {
	dec64_t temp;
	MULTIPLY64(temp, this_acct->cash_adv, this_acct->cash_adv_apr, setp);
	//RESCALE64(temp, temp, MTWO, setp);
	roundToAvailableCoin(&temp, temp, CENT);
	ADD64(this_acct->cash_adv, this_acct->cash_adv, temp, setp);
	ADD64(this_acct->posted_interest_cash_adv,
	      this_acct->posted_interest_cash_adv, temp, setp);
	
      }
      //Finance Charge for Balance transfer is generated every day
      if (ISPOSITIVE64(this_acct->bal_tx))
      {
	dec64_t temp;
	MULTIPLY64(temp, this_acct->bal_tx, this_acct->bal_tx_apr, setp);
	//RESCALE64(temp, temp, MTWO, setp);
	roundToAvailableCoin(&temp, temp, CENT);
	ADD64(this_acct->bal_tx, this_acct->bal_tx, temp, setp);
	ADD64(this_acct->posted_interest_bal_tx,
	      this_acct->posted_interest_bal_tx, temp, setp);
      }
      if (this_acct->fullpaid==0)
      {
	//full paid last period.
	//This also does underpaid because when underpaid,
	//all the rates go up to default rate
	dec64_t temp;
	MULTIPLY64(temp, this_acct->purchase, this_acct->purchase_apr, setp);
	//RESCALE64(temp, temp, MTWO, setp);
	roundToAvailableCoin(&temp, temp, CENT);
	ADD64(this_acct->purchase, this_acct->purchase, temp, setp);
	ADD64(this_acct->posted_interest_purchase,
	      this_acct->posted_interest_purchase, temp, setp);
      }
      //Handle Purchase expense
      //First, handle the close date
      if (counter == this_acct->close_date)
      {
	//The minimum finance charge if cur_bal!=0
	dec64_t converted_rate;
#ifdef DEBUG
	mysprintf(input_str, 
		" Today (Day %d) is the day to generate statement \n", 
		counter);
	errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	if (ISPOSITIVE64(this_acct->purchase)&&
	    (ISZERO64(this_acct->purchase)==0))
	  MAX64(this_acct->purchase, this_acct->purchase, 
		cc_const->min_fin_charge, setp);
	//MODIFY
	COPY64(this_acct->purchase_statement, this_acct->purchase);
	DIVIDE64(converted_rate, cc_const->min_payment_rate, HUNDRED, setp);
	MULTIPLY64(this_acct->min_payment, this_acct->purchase, 
		   converted_rate, setp);
	//RESCALE64(this_acct->min_payment, this_acct->min_payment, MTWO, setp);
	roundToAvailableCoin(&this_acct->min_payment, this_acct->min_payment, CENT);
	//Generate the purchase_statement
	ZERO64(this_acct->purchase_total_paid);
      }
      //Second, handle the last date
      if (counter == this_acct->last_date)
      {
#ifdef DEBUG
	mysprintf(input_str, 
		" Today (Day %d) is the last day to pay the monthly \n", 
		counter);
	errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
	//If the account is set to autopay from his Check account
	//When set autopay, it pays off all the expenses, not just purchase 
	//expense
	if (this_acct->autopay)
	{
	  int ret;
          deposit_acct_t *deposit_acct;
          if (hfind(checking_table, this_acct->acct_no,
                    mystrlen(this_acct->acct_no))==FALSE)
		    {
            char input_str[STR_MAX_LENGTH];
            mysprintf(input_str, "The auto-pay of %s Credit Card acct on Day %d is cancelled!\n Remember to deposit more money for the next withdrawl\n",
                    this_acct->acct_no, counter);
            errorPrint(LOC, WARNING_CODE, NOT_ENOUGH_BALANCE, stderr,
                       input_str);
	    this_acct->autopay = 0; //reset the autopay to zero
            this_acct->fullpaid = 0;
            this_acct->underpaid = 1;
          }
	  else {
	    dec64_t temp_autopay;
	    deposit_acct = (deposit_acct_t *)hstuff(checking_table);
	    //good
	    SUBTRACT64(temp_autopay, this_acct->purchase_statement, 
		       this_acct->purchase_total_paid, setp);
	    //the above may be zero or negative
	    ADD64(temp_autopay, temp_autopay, this_acct->bal_tx, setp);
	    ADD64(temp_autopay, temp_autopay, this_acct->cash_adv, setp);
	    if (ISPOSITIVE64(temp_autopay))
	    {
	      //If it is positive, then we withdrawl money from the 
	      //checking account
	      ret = money_withdrawl(checking_table, this_acct->acct_no, 
				    COUNTRIES, temp_autopay,
				    conversion_rate, fout, deposit_rate);
	      if (ret!=0)
	      {
		//Not good!
		mysprintf(input_str, "The auto-pay of %s Credit Card acct on Day\
 %d is cancelled!\n Remember to deposit more money for the next withdrawl\n", 
			this_acct->acct_no, counter);
		errorPrint(LOC, WARNING_CODE, NOT_ENOUGH_BALANCE, stderr, 
			   input_str);
		this_acct->fullpaid = 0;
		this_acct->underpaid = 1;
	      }
	      else {
		//deposit account is recorded during the withdrawl!
		//so don't do another deposit_record!!
		this_acct->fullpaid  = 1;
		this_acct->underpaid = 0;
		ZERO64(this_acct->purchase);
		ZERO64(this_acct->cash_adv);
		ZERO64(this_acct->bal_tx);
		COPY64(this_acct->credit_allowance, this_acct->credit_limit);
		COPY64(this_acct->last_paid, temp_autopay);
	      }
	    }
	  }
	}
	else {//not autopay, user pay by cash
	  //dec64_t temp_no_autopay;
	  //The purchase_statement is decreasing when user pay money.
	  //the min_payment was "purchase_statement-min_payment"
	  //So when the user pay more than the REAL min payment, then
	  //the purchase_statement will be less than the min payment

	  int lt_res = 0;
	  LT64(lt_res, this_acct->purchase_total_paid, 
	  		this_acct->min_payment, setp);
	  if(lt_res) 
	    this_acct->underpaid = 1;
	  else
	    this_acct->underpaid = 0;

	  LT64(lt_res, this_acct->purchase_total_paid,
	                      this_acct->purchase_statement, setp);
	  if(lt_res)  
	    this_acct->fullpaid = 0;
	  else
	    this_acct->fullpaid = 1;
	}
	if (this_acct->underpaid)//if underpaid
	{
	  //first, align the rate
	  autoDefault(this_acct);
	  this_acct->overdueHistory++;
	  //second, apply the late fee
	  lateFee(this_acct, cc_const, fout);
	}
	//new close date and last date are updated at last_date
	this_acct->close_date = counter-
	  (this_acct->last_date-this_acct->close_date) + this_acct->pay_date;
	this_acct->last_date = this_acct->close_date + CC_GRACE_PERIOD;
	//reset the purchase_statement since we may have negative 
	//purchase_statement (optional)
	ZERO64(this_acct->purchase_statement);
	ZERO64(this_acct->purchase_total_paid);
	ZERO64(this_acct->min_payment);
      }
      //recalculate the allowance because we now have interest
      SUBTRACT64(this_acct->credit_allowance, 
		 this_acct->credit_limit, this_acct->purchase, setp);
      SUBTRACT64(this_acct->credit_allowance, 
		 this_acct->credit_allowance, this_acct->bal_tx, setp);
      SUBTRACT64(this_acct->credit_allowance, 
		 this_acct->credit_allowance, this_acct->cash_adv, setp);
    }while(hnext(cc_table));
  }
  return SUCCESS;
}

int lateFee(cc_acct_t *this_acct, cc_const_t *cc_const, FILE *fout)
{
  dec64_t comp1, comp2;
#ifdef DEBUG
  char str[LENGTH64];
#endif
  //The late fee is only for purchase. All the others have finance charge 
  //every day.
  SUBTRACT64(comp1, (this_acct->purchase), (cc_const->level1_thr), setp);
  SUBTRACT64(comp2, (this_acct->purchase), (cc_const->level2_thr), setp);
  if (ISNEGATIVE64(comp1))
  {
#ifdef DEBUG
    TOSTRING64(cc_const->level1_fee, str);
#endif
    ADD64(this_acct->purchase, this_acct->purchase, cc_const->level1_fee, 
	  setp);
  }
  else if ((ISNEGATIVE64(comp1)==0)&&(ISNEGATIVE64(comp2)))
  {
#ifdef DEBUG
    TOSTRING64(cc_const->level2_fee, str);
#endif
    ADD64(this_acct->purchase, this_acct->purchase, cc_const->level2_fee, 
	  setp);
  }
  else {
#ifdef DEBUG
    TOSTRING64(cc_const->level3_fee, str);
#endif
    ADD64(this_acct->purchase, this_acct->purchase, cc_const->level3_fee, 
	  setp);
  }
#ifdef DEBUG
  if (1)
  {
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Late fee is %s and is accumulated to the purchase\n", 
	    str);
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
  }
#endif
  return SUCCESS;
  }

int credit_card_acct_close(htab *cc_table, char *acct_no, FILE *fout)
{
  cc_acct_t *this_acct;
  char p_debt[LENGTH64], c_debt[LENGTH64], b_debt[LENGTH64];
  char input_str[STR_MAX_LENGTH];
  if (hfind(cc_table, acct_no, mystrlen(acct_no))==FALSE)
  {
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (cc_acct_t *)hstuff(cc_table);
  TOSTRING64(this_acct->purchase, p_debt);
  TOSTRING64(this_acct->cash_adv, c_debt);
  TOSTRING64(this_acct->bal_tx, b_debt);
  mysprintf(input_str, "###### %s's credit card account is going to close.\n \
Please pay the debt shown below ######", 
	  this_acct->acct_no);
  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
  mysprintf(input_str, "Normal Purchase: %s\nCash Advance: %s\nBalance Transfer:\
 %s\n", p_debt, c_debt, b_debt);
  errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
  myfree(hkey(cc_table));
  myfree(hstuff(cc_table));
  hdel(cc_table);
  return SUCCESS;
}

int zero_it(int value, cc_acct_t *this_acct)
{
  if (value==PURCHASE)
    ZERO64(this_acct->purchase);
  else if (value==CASH_ADVANCE)
    ZERO64(this_acct->cash_adv);
  else if (value==BALANCE_TRANSFER)
    ZERO64(this_acct->bal_tx);
  else {
    errorPrint(LOC, ERROR_CODE, CC_WRONG_TX_TYPE, stderr, 
	       "Wrong Transaction Type");
  }
  return SUCCESS;
}

int doPayment(cc_acct_t *this_acct, dec64_t payment, FILE *fout)
{
  dec64_t comp;
  dec64_t *lowest, *middle, *largest;
  dec64_t *lowest_v, *middle_v, *largest_v;
  int array[3];
  dec64_t temp;

  //The payment always goes to high-rate interest first,
  //then go to the next higher interest, then go to the lowest interest
  ADD64(temp, this_acct->purchase, this_acct->cash_adv, setp);
  ADD64(temp, temp, this_acct->bal_tx, setp);
  SUBTRACT64(comp, payment, temp, setp);
  if (ISNEGATIVE64(comp)==0)
  {
    //payment>=total balance
    zero_it(PURCHASE, this_acct);
    zero_it(CASH_ADVANCE, this_acct);
    zero_it(BALANCE_TRANSFER, this_acct);
    if ((counter>this_acct->close_date)&&(counter <=this_acct->last_date))
      COPY64(this_acct->purchase_total_paid, this_acct->purchase_statement);
#ifdef DEBUG
    if (1)
    {
      char str[LENGTH64];
      char input_str[STR_MAX_LENGTH];
      TOSTRING64(comp, str);
      mysprintf(input_str, "Payment is paid in full, all the extra $%s is returned to the customer\n", str);
      errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
    }
#endif
    return SUCCESS;
  }

  //if payment < total balance, then we proceed the following
  //SORT THE RATES
  //ARRAY[0] => LOWEST RATE
  //ARRAY[1] => MID RATE
  //ARRAY[2] => HIGHEST RATE
  array[0] = PURCHASE;
  array[1] = CASH_ADVANCE;
  array[2] = BALANCE_TRANSFER;

  lowest  = &this_acct->purchase_apr;
  middle  = &this_acct->cash_adv_apr;
  largest = &this_acct->bal_tx_apr;

  lowest_v  = &this_acct->purchase;
  middle_v  = &this_acct->cash_adv;
  largest_v = &this_acct->bal_tx;

  //Now, we sort the rates, along with the values and identifiers

  int gt_res = 0;
  GT64(gt_res, *lowest, *middle, setp);
  if(gt_res)
  {
    //check if L>M
    dec64_t *temp0_ptr, *temp0_v;
    int temp0_ary;
    temp0_ptr = middle;
    temp0_v = middle_v;
    temp0_ary = array[1];    
    middle = lowest;
    middle_v = lowest_v;
    array[1] = array[0];
    lowest = temp0_ptr;
    lowest_v = temp0_v;
    array[0] = temp0_ary;
  }

  GT64(gt_res, *middle, *largest, setp);
  if(gt_res)
  {
    //if M>Largest
    dec64_t *temp0_ptr, *temp0_v;
    int temp0_ary;
    temp0_ptr = middle;
    temp0_v = middle_v;
    temp0_ary = array[1];
    middle = largest;
    middle_v = largest_v;
    array[1] = array[2];
    largest = temp0_ptr;
    largest_v = temp0_v;
    array[2] = temp0_ary;
    //then check the lowest and mid again

    GT64(gt_res, *lowest, *middle, setp);
    if(gt_res)
    {
      //if Lowest>M
      dec64_t *temp1_ptr, *temp1_v;
      int temp1_ary;
      temp1_ptr = middle;
      temp1_v = middle_v;
      temp1_ary = array[1];
      middle = lowest;
      middle_v = lowest_v;
      array[1] = array[0];
      lowest = temp1_ptr;
      lowest_v = temp1_v;
      array[0] = temp1_ary;
    }
  }
  //Payment first pays the lowest rate payment
  SUBTRACT64(temp, *lowest_v, payment, setp);
  //Check if we should deduct the purchase_statement
  if ((array[0]==PURCHASE)&&(counter>this_acct->close_date)&&
      (counter <=this_acct->last_date))
    ADD64(this_acct->purchase_total_paid, this_acct->purchase_total_paid, 
	  payment, setp);
  if (ISPOSITIVE64(temp)==0)
  {
    //if largest<=payment, then we need to go to the mid-rate
    zero_it(array[0], this_acct);
    //Check if we should deduct the purchase_statement
    if ((array[1]==PURCHASE)&&(counter>this_acct->close_date)&&
	(counter <=this_acct->last_date))
      SUBTRACT64(this_acct->purchase_total_paid, 
		 this_acct->purchase_total_paid, temp, setp);
    ADD64(temp, temp, *middle_v, setp);
    if (ISPOSITIVE64(temp)==0)
    {
      //if mid-rate value <=remaining
      zero_it(array[1], this_acct);
      ADD64(*largest_v, *largest_v, temp, setp);
      //Check if we should deduct the purchase_statement
      if ((array[2]==PURCHASE)&&(counter>this_acct->close_date)&&
	  (counter <=this_acct->last_date))
	SUBTRACT64(this_acct->purchase_total_paid, 
		   this_acct->purchase_total_paid, temp, setp);
    }
    else 
      COPY64(*middle_v, temp);
  }
  else 
    COPY64(*lowest_v, temp);
  return SUCCESS;
}

void autoDefault(cc_acct_t *this_acct)
{
  //All the rates go up to default rate if the user doesn't pay his account in due date
  COPY64(this_acct->purchase_apr, this_acct->default_apr);
  COPY64(this_acct->cash_adv_apr, this_acct->default_apr);
  COPY64(this_acct->bal_tx_apr, this_acct->default_apr);
}
int credit_card_acct_pay(htab *cc_table, char *acct_no, int payment_type,
			 dec64_t payment, FILE *fout)
{
  //(1) we update the close date and last date only when counter
  //    hit the last date
  //(2) counter >= close date only when it's in the pay period
  //    Otherwise, counter < close
  dec64_t real_pay;
  cc_acct_t *this_acct;
  if (hfind(cc_table, acct_no, mystrlen(acct_no))==FALSE)
  {
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (cc_acct_t *)hstuff(cc_table);

  if (payment_type == FULL_PAY)
  {
    ADD64(real_pay, this_acct->purchase, this_acct->cash_adv, setp);
    ADD64(real_pay, real_pay, this_acct->bal_tx, setp);
  }
  else if ((payment_type == STATEMENT_PAY)||(payment_type == MIN_PAY))
  {
    if (counter < this_acct->close_date)
    {
      char input_str[STR_MAX_LENGTH];
      mysprintf(input_str, "###### No statement or min payment available at \
this time. The payment on %s's Credit Card acct is rejected ######\n", 
	      this_acct->acct_no);
      errorPrint(LOC, WARNING_CODE, CC_REJ_PAY, stderr, input_str);
      return CC_REJ_PAY;
    }      
    //Just need to pay the difference between the statement and the 
    //previous total paid
    SUBTRACT64(real_pay, this_acct->purchase_statement,
	       this_acct->purchase_total_paid, setp);
    if (ISPOSITIVE64(real_pay)==0)
    {
      char input_str[STR_MAX_LENGTH];
      mysprintf(input_str, "###### No need to pay anymore. \
The payment on %s's Credit Card is rejected ######\n", this_acct->acct_no);
      errorPrint(LOC, WARNING_CODE, CC_REJ_PAY, stderr, input_str);
      return CC_REJ_PAY;
    }
    if (payment_type == MIN_PAY)
    {
      //only need to pay the minimum (Note: he may already pay but with 
      //an amount less than the minimum payment. So this time, we only need to
      //pay the difference
      SUBTRACT64(real_pay, this_acct->min_payment, 
		 this_acct->purchase_total_paid, setp);
      if (ISPOSITIVE64(real_pay)==0)
      {
	char input_str[STR_MAX_LENGTH];
	char str[LENGTH64];
	SUBTRACT64(real_pay, this_acct->purchase_statement, 
		   this_acct->purchase_total_paid, setp);
	if (ISPOSITIVE64(real_pay))
	{
	  TOSTRING64(real_pay, str);
	  mysprintf(input_str, "Min-payment for %s's Credit Card has been paid. This action is rejected. However, you still have %s needed to pay to pay the\
 statement in full", this_acct->acct_no, str);
	  errorPrint(LOC, WARNING_CODE, CC_REJ_PAY, stderr, input_str);
	}
	return CC_REJ_PAY;
      }
    }
  }
  else 
    COPY64(real_pay, payment);
  doPayment(this_acct, real_pay, fout);
  ADD64(this_acct->credit_allowance, this_acct->credit_allowance, real_pay,
	setp);
  MIN64(this_acct->credit_allowance, this_acct->credit_limit, 
	this_acct->credit_allowance, setp);
  if (ISPOSITIVE64(this_acct->credit_allowance))
    this_acct->overCreditLimit = 0;
  COPY64(this_acct->last_paid, real_pay);
#ifdef DEBUG
  {
    char input_str[STR_MAX_LENGTH];
    char str[LENGTH64];
    TOSTRING64(real_pay, str);
    mysprintf(input_str, "###### Receive %s's payment ($%s) to his CREDIT CARD \
account on Day %d\n", this_acct->acct_no, str, counter);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
  }
#endif
  return SUCCESS;
}

int credit_card_acct_debt(htab *cc_table, char *acct_no, int currency, 
			  dec64_t **conversion_rate, FILE *fout,
			  cc_const_t *cc_const,
			  int type, dec64_t fund)
{
  dec64_t converted_fund, r_plus, result;
  dec64_t converted_rate;
  dec64_t transaction;
  dec64_t temp;
  cc_acct_t *this_acct;
#ifdef DEBUG
  char unit[STR_MAX_LENGTH];
#endif
  char type_str[STR_MAX_LENGTH];
#ifdef DEBUG
  checkName(unit, currency);
#endif
  if (hfind(cc_table, acct_no, mystrlen(acct_no))==FALSE)
  {
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (cc_acct_t *)hstuff(cc_table);

  //First convert the expense if it's not USD
  if (currency != COUNTRIES)
  {
    if (type==BALANCE_TRANSFER)
    {
      char input_str[STR_MAX_LENGTH];
      errorPrint(LOC, WARNING_CODE, CC_REJ_TX, stderr, 
		 "Balance Transfer can only work with USD\n");
      mysprintf(input_str, "###### Transaction on %s's Credit Card acct is \
cancelled ######\n", this_acct->acct_no);
      errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
      return CC_REJ_TX;
    }
    conversionUS(currency, COUNTRIES, fund, &converted_fund,
                 &transaction, conversion_rate);
  }
  else
    COPY64(converted_fund, fund);
  //Then check the allowance
  SUBTRACT64(temp, this_acct->credit_allowance, converted_fund, setp);
  
  int lt_res = 0;
  LT64(lt_res, temp, this_acct->credit_limit_upper, setp);
  int negOneTimesLtRes = -1 * lt_res;
  FROMINTEGER64(result, negOneTimesLtRes, setp);
  if(lt_res)
  {
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "%s's debt is too much (2x credit limit).\n", 
	    this_acct->acct_no);
    errorPrint(LOC, WARNING_CODE, CC_REJ_TX, stderr, input_str);
    mysprintf(input_str, "###### Transaction on %s's Credit Card acct is \
cancelled ######\n", this_acct->acct_no);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
    return CC_REJ_TX;
  }
  COPY64(this_acct->credit_allowance, temp);
  if (ISNEGATIVE64(this_acct->credit_allowance))
  {
#ifdef DEBUG
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout,
	       "Your debt is over the credit limit, but this transaction is \
allowed with fee\n");
#endif
    this_acct->overCreditLimit = 1;
  }
  //Handle the transaction now
  if (type==PURCHASE)
  {
    if (currency != COUNTRIES)
    {
      dec64_t temp;
      DIVIDE64(converted_rate, cc_const->for_cur_tx_rate, HUNDRED, setp);
      ADD64(r_plus, converted_rate, ONE, setp);
      MULTIPLY64(temp, converted_fund, converted_rate, setp);
      SUBTRACT64(this_acct->credit_allowance, 
		 this_acct->credit_allowance, temp, setp);
      MULTIPLY64(converted_fund, converted_fund, r_plus, setp);
    }
    //RESCALE64(converted_fund, converted_fund, MTWO, setp);
    roundToAvailableCoin(&converted_fund, converted_fund, CENT);

    ADD64(this_acct->purchase, this_acct->purchase, converted_fund, setp);
    mystrcpy(type_str, "PURCHASE");
  }
  else if (type==CASH_ADVANCE)
  {
    DIVIDE64(converted_rate, cc_const->cash_adv_tx_rate, HUNDRED, setp);
    MULTIPLY64(result, converted_fund, converted_rate, setp);
    MAX64(result, result, cc_const->min_cash_adv_tx_fee, setp);
    if (currency != COUNTRIES)
    {
      dec64_t result2;
      DIVIDE64(converted_rate, cc_const->for_cur_tx_rate, HUNDRED, setp);
      MULTIPLY64(result2, converted_fund, converted_rate, setp);
      ADD64(result, result, result2, setp);
    }
    //RESCALE64(result, result, MTWO, setp);
    roundToAvailableCoin(&result, result, CENT);
    SUBTRACT64(this_acct->credit_allowance,
	       this_acct->credit_allowance, result, setp);
    ADD64(converted_fund, result, converted_fund, setp);
    ADD64(this_acct->cash_adv, this_acct->cash_adv, converted_fund, setp);
    mystrcpy(type_str, "CASH ADVANCE");
  }
  else if (type==BALANCE_TRANSFER)
  {
    DIVIDE64(converted_rate, cc_const->bal_tx_rate, HUNDRED, setp);
    MULTIPLY64(result, converted_fund, converted_rate, setp);
    MAX64(result, result, cc_const->min_bal_tx_fee, setp);
    MIN64(result, result, cc_const->max_bal_tx_fee, setp);
    SUBTRACT64(this_acct->credit_allowance,
               this_acct->credit_allowance, result, setp);
    ADD64(converted_fund, converted_fund, result, setp);
    ADD64(this_acct->bal_tx, this_acct->bal_tx, converted_fund, setp);
    if (this_acct->intro_apr_bal_tx_end_date==-1)
    {
      //this is the first balance transfer
      if (counter < this_acct->start_date + INIT_BAL_TX_PERIOD)
	this_acct->intro_apr_bal_tx_end_date = counter + INIT_BAL_TX_PERIOD;
    }
    mystrcpy(type_str, "BALANCE TRANSFER");
  }
  else { //no this action
    errorPrint(LOC, ERROR_CODE, CC_WRONG_TX_TYPE, stderr, 
	       "Wrong Transaction Type");
  }
  //applied the over credit limit fee if needed. this is added in the 
  //purchase expense
  if (this_acct->overCreditLimit ==1) 
    ADD64(this_acct->purchase, this_acct->purchase, 
	  cc_const->over_credit_limit_fee, setp);
  this_acct->last_tx_unit = currency;
  this_acct->last_tx_type = type;
  COPY64(this_acct->last_tx, fund);
#ifdef DEBUG
  {
    char input_str[STR_MAX_LENGTH];
    char str[LENGTH64];
    TOSTRING64(fund, str);
    mysprintf(input_str, "###### New Transaction is posted: %s %s with %s type \
has commited into %s's CREDIT CARD account ######", str, unit, type_str, 
	    this_acct->acct_no);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
  }
#endif
  return SUCCESS;
}

int getCCRate(cc_rate_t *cur_rate, cc_const_t *cc_const, dec64_t prime_rate)
{
  ADD64(cur_rate->purchase_apr, cc_const->purchase_bal_tx_extra, prime_rate,
	setp);
  DIVIDE64(cur_rate->purchase_apr, cur_rate->purchase_apr, YEAR, setp);
  DIVIDE64(cur_rate->purchase_apr, cur_rate->purchase_apr, HUNDRED, setp);
  ADD64(cur_rate->bal_tx_apr, cc_const->purchase_bal_tx_extra, prime_rate,
	setp);
  DIVIDE64(cur_rate->bal_tx_apr, cur_rate->bal_tx_apr, YEAR, setp);
  DIVIDE64(cur_rate->bal_tx_apr, cur_rate->bal_tx_apr, HUNDRED, setp);
  ADD64(cur_rate->cash_adv_apr, cc_const->cash_adv_extra, prime_rate, setp);
  MAX64(cur_rate->cash_adv_apr, cur_rate->cash_adv_apr, 
	cc_const->min_cash_adv, setp);
  DIVIDE64(cur_rate->cash_adv_apr, cur_rate->cash_adv_apr, YEAR, setp);
  DIVIDE64(cur_rate->cash_adv_apr, cur_rate->cash_adv_apr, HUNDRED, setp);
  ADD64(cur_rate->default_apr, cc_const->default_extra, prime_rate, setp);
  MAX64(cur_rate->default_apr, cur_rate->default_apr, cc_const->min_default,
	setp);
  DIVIDE64(cur_rate->default_apr, cur_rate->default_apr, YEAR, setp);
  DIVIDE64(cur_rate->default_apr, cur_rate->default_apr, HUNDRED, setp);
  FROMSTRING64(cur_rate->init_bal_tx_apr, INIT_BAL_TX_RATE, setp);
  DIVIDE64(cur_rate->init_bal_tx_apr, cur_rate->init_bal_tx_apr, YEAR, setp);
  DIVIDE64(cur_rate->init_bal_tx_apr, cur_rate->init_bal_tx_apr, HUNDRED,
	   setp);
  return SUCCESS;
}

int credit_card_acct_init(cc_acct_t *this_acct, char *acct_no,
			  int autopay, dec64_t credit_limit, 
			  cc_rate_t *cc_rate)
{
  mystrcpy(this_acct->acct_no, acct_no);
  this_acct->start_date = counter;
  this_acct->pay_date = PAY_DATE;
  this_acct->close_date = counter + PAY_DATE;
  this_acct->last_date = counter + PAY_DATE + CC_GRACE_PERIOD;
  this_acct->status = 1;
  this_acct->underpaid = 0;
  this_acct->fullpaid = 1; //Set this to "1" initially
  this_acct->overdueHistory = 0;
  this_acct->autopay = autopay;
  this_acct->intro_apr_bal_tx_end_date = INIT_BAL_TX_PERIOD;
  ZERO64(this_acct->purchase_statement);
  ZERO64(this_acct->last_paid);
  this_acct->last_tx_unit = 0;
  this_acct->last_tx_type = 0;
  ZERO64(this_acct->last_tx);
  COPY64(this_acct->credit_limit, credit_limit);
  MINUS64(this_acct->credit_limit_upper, credit_limit, setp);
  COPY64(this_acct->credit_allowance, credit_limit);
  this_acct->overCreditLimit = 0;
  COPY64(this_acct->purchase_apr, cc_rate->purchase_apr);
  COPY64(this_acct->cash_adv_apr, cc_rate->cash_adv_apr);
  //initially give the user better BAL_TX rate
  COPY64(this_acct->bal_tx_apr, cc_rate->init_bal_tx_apr);
  COPY64(this_acct->default_apr, cc_rate->default_apr);
  this_acct->all_rate_default = 0;
  ZERO64(this_acct->purchase);
  ZERO64(this_acct->posted_interest_purchase);
  ZERO64(this_acct->cash_adv);
  ZERO64(this_acct->posted_interest_cash_adv);
  ZERO64(this_acct->bal_tx);
  ZERO64(this_acct->posted_interest_bal_tx);
  ZERO64(this_acct->min_payment);
  ZERO64(this_acct->purchase_total_paid);
  return SUCCESS;
}


int credit_card_acct_open(htab *cc_table, htab *checking_table,
			  char *acct_no, 
			  cc_rate_t *cc_rate, cc_const_t *cc_const,
			  dec64_t credit_limit, int autopay,
			  FILE *fout)
{
  deposit_acct_t *deposit_acct;
  int keyl;
  ub1 *key;
  cc_acct_t *this_acct;
  char input_str[STR_MAX_LENGTH];

  keyl = mystrlen(acct_no);
  if (keyl>MAX_ACCT_NAME_LENGTH)
  {
    errorPrint(LOC, ERROR_CODE, LENGTH_TOO_LONG,
               stderr, "account name is too long\n");
  }
  if (hadd(cc_table, acct_no, keyl, (void *)0)==FALSE)
  {
    mysprintf(input_str, "[credit_card_acct_open] cannot add %s's account due to hash table conflict (likely already an account with the same name)\n",
            acct_no);
    errorPrint(LOC, WARNING_CODE, ADD_ACCT_FAIL, fout, input_str);
    myfprintf(stderr, input_str);
    return ADD_ACCT_FAIL;
  }
//else {
//  mysprintf(input_str, "[credit_card_acct_open] added %s's account\n", acct_no);
//  myfprintf(stderr, input_str);
//}
  key = (ub1 *)mymalloc(keyl);       /* dumb use of mymalloc */
  mymemcpy(key, acct_no, keyl);          /* copy buf into key */
  hkey(cc_table)=key;                     /* replace buf with key */
  //other stuffs
  this_acct = mycalloc(1, sizeof(cc_acct_t));
  credit_card_acct_init(this_acct, acct_no, autopay, credit_limit, cc_rate);
  hstuff(cc_table)=(void*)this_acct;

  if (autopay!=0)//check the existence of the checking account
  {
    if (hfind(checking_table, acct_no, mystrlen(acct_no))==FALSE)
    {
      char input_str[STR_MAX_LENGTH];
      mysprintf(input_str, "###### Checking Acct for %s doesn't exist. Credit Card account open fails ######\n", acct_no);
      errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, stderr, input_str);
      return ACCT_NOT_EXIST;
    }
    deposit_acct = (deposit_acct_t *)hstuff(checking_table);
  }
#ifdef DEBUG
  {
    char fund_str[LENGTH64];
    char input_str[STR_MAX_LENGTH];
    TOSTRING64(credit_limit, fund_str);
    mysprintf(input_str, "###### %s opens a CREDIT CARD account with credit \
limit as $%s######\n", this_acct->acct_no, fund_str);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
  }
#endif
  return SUCCESS;
}


int cc_const_init(cc_const_t *cc_const)
{
  FROMSTRING64(cc_const->purchase_bal_tx_extra, PURCHASE_BAL_TX_EXTRA, setp);
  FROMSTRING64(cc_const->cash_adv_extra, CASH_ADV_EXTRA, setp);
  FROMSTRING64(cc_const->min_cash_adv, MIN_CASH_ADV, setp);
  FROMSTRING64(cc_const->min_default, MIN_DEFAULT, setp);
  FROMSTRING64(cc_const->default_extra, DEFAULT_EXTRA, setp);
  FROMSTRING64(cc_const->min_fin_charge, MIN_FIN_CHARGE, setp);
  FROMSTRING64(cc_const->for_cur_tx_rate, FOR_CUR_TX_RATE, setp);
  FROMSTRING64(cc_const->cash_adv_tx_rate, CASH_ADV_TX_RATE, setp);
  FROMSTRING64(cc_const->min_cash_adv_tx_fee, MIN_CASH_ADV_TX_FEE, setp);
  FROMSTRING64(cc_const->min_payment_rate, MIN_PAYMENT_RATE, setp);
  FROMSTRING64(cc_const->bal_tx_rate, BAL_TX_RATE, setp);
  FROMSTRING64(cc_const->min_bal_tx_fee, MIN_BAL_TX_FEE, setp);
  FROMSTRING64(cc_const->max_bal_tx_fee, MAX_BAL_TX_FEE, setp);
  FROMSTRING64(cc_const->over_credit_limit_fee, OVER_CREDIT_LIMIT_FEE, setp);
  FROMSTRING64(cc_const->level1_fee, LATE_FEE_LEVEL_1, setp);
  FROMSTRING64(cc_const->level2_fee, LATE_FEE_LEVEL_2, setp);
  FROMSTRING64(cc_const->level3_fee, LATE_FEE_LEVEL_3, setp);
  FROMSTRING64(cc_const->level1_thr, LATE_FEE_LEVEL_1_THR, setp);
  FROMSTRING64(cc_const->level2_thr, LATE_FEE_LEVEL_2_THR, setp);
  return SUCCESS;
}

void check_cc_tx_type(char *str, int type)
{
  if (type==PURCHASE)
    mystrcpy(str, "Purchase");
  else if (type==CASH_ADVANCE)
    mystrcpy(str, "Cash Advance");
  else if (type==BALANCE_TRANSFER)
    mystrcpy(str, "Balance Transfer");
  else
    errorPrint(LOC, ERROR_CODE, CC_WRONG_TX_TYPE, stderr, 
	       "Wrong Transaction Type");
}


int credit_card_acct_inquiry(htab *cc_table, char *acct_no, FILE *fout)
{
  char str1[LENGTH64], str2[LENGTH64], unit[LENGTH64];
  dec64_t temp;
  cc_acct_t *this_acct;
  if (hfind(cc_table, acct_no, mystrlen(acct_no))==FALSE)
  {
    char input_str[STR_MAX_LENGTH];
    mysprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (cc_acct_t *)hstuff(cc_table);
  myfprintf(fout, "=============== CREDIT CARD ACCOUNT REPORT ==============\n");
  myfprintf(fout, "Account Type: Credit Card\n");
  myfprintf(fout, "Account Status: %s\n", 
	  (this_acct->status==1)?"ACTIVE":"NON-ACTIVE");
  myfprintf(fout, "Account Name: %s\n", this_acct->acct_no);
  myfprintf(fout, "Account Starts: %d\n", this_acct->start_date);
  myfprintf(fout, "Account Paid: Every %d days\n", this_acct->pay_date);
  myfprintf(fout, "Current Payment Due Date: Day %d\n", this_acct->last_date);
  myfprintf(fout, "Underpaid Flag: %s\n", (this_acct->underpaid==1)?"ON":"OFF");
  myfprintf(fout, "Fullpaid Flag: %s\n", (this_acct->fullpaid==1)?"ON":"OFF");
  myfprintf(fout, "Overdue History: %d times\n", this_acct->overdueHistory);
  if (this_acct->autopay==1)
    myfprintf(fout, "Debt is Paid Through %s's Checking Account\n", 
	    this_acct->acct_no);
  else
    myfprintf(fout, "Debt is Paid by Cash by Due Date\n");
  myfprintf(fout, "Introduction APR for balance transfer ends: Day %d\n", 
	  this_acct->intro_apr_bal_tx_end_date);
  if(ISZERO64(this_acct->purchase_statement)==0)
  {
    TOSTRING64(this_acct->purchase_statement, str1);
    myfprintf(fout, "Monthly Statement for Purchase: $%s \n", str1);
  }
  if (ISZERO64(this_acct->min_payment)==0)
  {
    TOSTRING64(this_acct->min_payment, str1);
    myfprintf(fout, "Minimum Payment [Purchase] in This Statement: $%s\n", str1);
  }
  if(ISZERO64(this_acct->purchase_total_paid)==0)
  {
    TOSTRING64(this_acct->purchase_total_paid, str1);
    myfprintf(fout, "Statement Has Been Paid: $%s \n", str1);
  }
  if(ISZERO64(this_acct->last_paid)==0)
  {
    TOSTRING64(this_acct->last_paid, str1);
    myfprintf(fout, "The Last Payment: $%s\n", str1);
  }
  if (ISZERO64(this_acct->last_tx)==0)
  {
    TOSTRING64(this_acct->last_tx, str1);
    check_cc_tx_type(str2, this_acct->last_tx_type);
    checkName(unit, this_acct->last_tx_unit);
    myfprintf(fout, "The Last Transaction: [%s] $%s in %s\n", str2, str1, unit);
  }
  TOSTRING64(this_acct->credit_limit, str1);
  myfprintf(fout, "Credit Limit: $%s\n", str1);
  TOSTRING64(this_acct->credit_allowance, str1);
  myfprintf(fout, "Current Credit Allowance: $%s\n", str1);
  myfprintf(fout, "Over Credit Limit Flag: %s\n", 
	  (this_acct->overCreditLimit==1)?"ON":"OFF");
  MULTIPLY64(temp, this_acct->purchase_apr, YEAR, setp);
  MULTIPLY64(temp, temp, HUNDRED, setp);
  QUANTIZE64(temp, temp, ETWO, setp);
  TOSTRING64(temp, str1);
  myfprintf(fout, "Purchase APR: %s (percent)\n", str1);
  MULTIPLY64(temp, this_acct->cash_adv_apr, YEAR, setp);
  MULTIPLY64(temp, temp, HUNDRED, setp);
  QUANTIZE64(temp, temp, ETWO, setp);
  TOSTRING64(temp, str1);
  myfprintf(fout, "Cash Advance APR: %s (percent)\n", str1);
  MULTIPLY64(temp, this_acct->bal_tx_apr, YEAR, setp);
  MULTIPLY64(temp, temp, HUNDRED, setp);
  QUANTIZE64(temp, temp, ETWO, setp);
  TOSTRING64(temp, str1);  
  myfprintf(fout, "Balance Transfer APR: %s (percent)\n", str1);
  MULTIPLY64(temp, this_acct->default_apr, YEAR, setp);
  MULTIPLY64(temp, temp, HUNDRED, setp);
  QUANTIZE64(temp, temp, ETWO, setp);
  TOSTRING64(temp, str1);
  myfprintf(fout, "Default APR: %s (percent)\n", str1);
  myfprintf(fout, "Align to Default Rate Flag: %s\n", 
	  (this_acct->all_rate_default==1)?"ON":"OFF");
  TOSTRING64(this_acct->purchase, str1);
  myfprintf(fout, "Purchase Balance: $%s\n", str1);
  TOSTRING64(this_acct->posted_interest_purchase, str1);
  myfprintf(fout, 
	  "Accumulated Interest [Purchase] since the Account Starts: $%s\n", 
	  str1);
  TOSTRING64(this_acct->cash_adv, str1);
  myfprintf(fout, "Cash Advance Balance: $%s\n", str1);
  TOSTRING64(this_acct->posted_interest_cash_adv, str1);
  myfprintf(fout, "Accumulated Interest [Cash Advance] since the Account \
Starts): $%s\n", str1);
  TOSTRING64(this_acct->bal_tx, str1);
  myfprintf(fout, "Balance Transfer Balance: $%s\n", str1);
  TOSTRING64(this_acct->posted_interest_bal_tx, str1);
  myfprintf(fout, "Accumulated Interest [Balance Transfer] since the Account \
Starts): $%s\n", str1);
  myfprintf(fout, "=======================================================\n");
  return SUCCESS;
}
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-16 21:31:46 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: credit_card.c,v $
  Revision 1.6  2009-04-16 21:31:46  mjanders
  added more informative error message

  Revision 1.5  2009-04-12 05:53:19  mjanders
  Added 128-bit mode

  Revision 1.4  2009-02-02 23:26:22  stsen
  updated comments and style

  Revision 1.8  2009/01/08 21:34:17  mjanders
  Cleanup

  Revision 1.7  2009/01/06 18:39:12  mjanders
  Added common header

  Revision 1.6  2009/01/02 23:53:27  mjanders
  Added native_gcc_bid mode and tested

  Revision 1.5  2008/11/25 06:15:07  mjanders
  Finished adding BID

  Revision 1.4  2008/11/23 08:14:17  mjanders
  Moved around the include files to make more sense. All include files for the libraries come in operator.h

  Revision 1.3  2008/11/21 04:34:39  mjanders
  Finished adding BID

  Revision 1.2  2008/05/15 04:56:31  mjanders
  Switched all COMPAREs to new way of comparing numbers

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.12  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.11  2007/03/14 18:09:55  lwang
  temp check-in

  Revision 1.10  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.7  2006/11/30 03:25:01  lwang
  fix problem on the size of the acct name and linked list

  Revision 1.6  2006/11/22 01:40:31  lwang
  comment added

*/
