//----------------------------------------
// mortgage.c
// Decimal Floating-Point Benchmark
// This file is to define the common functions for mortgage actions
// Developed by Liang-Kai Wang & Chuck Tsen
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.6 $

#include "dfp_includes.h"
#include "mortgage.h"
/////////////////////////////////////////////////////
//GLOBAL EXTERNAL VARIABLES
//The constants used in the microbenchmark
// M* means "minus *". E.g. MTWO = -2
//////////////////////////////////////////////////////
extern dec64_t ONE, TWO, TWELVE, MTWO, ETWO, MFOUR, EFOUR, TEN, MONTH, HUNDRED, YEAR, CENT;
extern mort_const_t mort_const;
extern int counter;
extern context_t *setp;

//This is to computethe title insurance fee
int closing_cost_fcn(dec64_t *title_insurance_fee, dec64_t principal, 
		     dec64_t downPayment){
  //from http://www.michaelbelle.com/calculator.htm
  //Buyer Part
  //Sales Price: this_acct->loan + this_acct->downPayment
  //Financed Amount: this_acct->loan
  //Lender's Title Insurance Policy = (Financed Amount==0)?0:25
  //Intangible Tax on Note = financed.value  * .002
  //Alta Title Endorsements = (Financed Amount==0)?0:50
  //title Insurance Policy = (Sales Price - 100000) * .001 * 5.00 +  575.00
  //Florida Form 9 Title Endorsement = (Financed Amount==0)?0:(title Insurance Policy+25.00)*0.1
  //Title Search Fee = 85
  //Title Examination Fee = 65
  //Closing Fee = 100
  //Recording Fees = 50
  //Documentary Stamp Tax on Note = Financed Amount * 0.01 * 0.35
  //Buyer's Closing costs Total = (title Insurance Policy) + (Lender's Title Insurance Policy) + 
  //                              (Florida Form 9 Title Endorsement) + (Alta Title Endorsements) +
  //                              (Title Search Fee) + (Title Examination Fee) + (Closing Fee) + 
  //                              (Documentary Stamp Tax on Note) + (Intangible Tax on Note) + 
  //                              (Recording Fees)
  //--------------------------------------------------------------------------------------------
  //Seller part (not computed, for reference only)
  //Documentary Stamp Tax on Deed = Sales Price * 0.01 * 0.7 (not computed)
  //Recording Fees Seller = 10.5 (not computed)
  //Seller's Closing Costs Total = Documentary Stamp Tax on Deed + Recording Fees Seller (not computed)
  dec64_t sales, temp, temp_1, title_insurance_policy, 
    documentary_stamp_tax_on_note, intagible_tax_on_note, 
    lender_title_insurance_policy, alta_title_endorsements, title_endorsement;

  ADD64(sales, downPayment, principal, setp);
  //title Insurance Policy = (Sales Price - 100000) * .001 * 5.00 +  575.00
  SUBTRACT64(temp, sales, mort_const.hundredThousand, setp);
  MULTIPLY64(temp, temp, mort_const.title_insurance_rate_1, setp);
  MULTIPLY64(temp, temp, mort_const.title_insurance_rate_2, setp);
  ADD64(title_insurance_policy, temp, mort_const.title_insurance_base, setp);
  //Documentary Stamp Tax on Note = Financed Amount * 0.01 * 0.35
  MULTIPLY64(temp, principal , mort_const.documentary_stamp_tax_on_note_rate_1, 
	     setp);
  //  FROMSTRING64(temp_1, DOCUMENTARY_STAMP_TAX_ON_NOTE_RATE_2, setp);
  MULTIPLY64(documentary_stamp_tax_on_note, temp, 
	     mort_const.documentary_stamp_tax_on_note_rate_2, setp);
  //Intangible Tax on Note = financed.value  * .002
  MULTIPLY64(intagible_tax_on_note, 
	     mort_const.intagible_tax_on_note, principal, setp);

  if (ISZERO64(principal)){
    ZERO64(lender_title_insurance_policy);
    ZERO64(alta_title_endorsements);
    ZERO64(title_endorsement);
  }
  else {
    COPY64(lender_title_insurance_policy, 
	   mort_const.lender_title_insurance_policy);
    COPY64(alta_title_endorsements, mort_const.alta_title_endorsements);
    COPY64(temp, mort_const.title_endorsement_extra);
    ADD64(temp, title_insurance_policy, temp, setp);
    COPY64(temp_1, mort_const.title_endorsement_rate);
    MULTIPLY64(title_endorsement, temp, temp_1, setp);
  }
  //TOTAL
  COPY64(temp, title_insurance_policy);
  ADD64(temp, temp, lender_title_insurance_policy, setp);
  ADD64(temp, temp, title_endorsement, setp);
  ADD64(temp, temp, alta_title_endorsements, setp);
  ADD64(temp, temp, mort_const.title_search, setp);
  ADD64(temp, temp, mort_const.title_examination, setp);
  ADD64(temp, temp, mort_const.closing, setp);
  ADD64(temp, temp, documentary_stamp_tax_on_note, setp);
  ADD64(temp, temp, intagible_tax_on_note, setp);
  ADD64(*title_insurance_fee, temp, mort_const.recording, setp);
  return SUCCESS;
}

int closingCost(mort_acct_t *this_acct){
  //This is to compute the closing cost. This is done when the mortgage starts
  //From: http://www.moneytoys.com/closing-costs-calculator.php
  dec64_t closing_cost;
  closing_cost_fcn(&closing_cost, this_acct->loan, 
			  this_acct->downPayment);
  /*
  ADD64(temp, mort_const.origination, this_acct->point, setp);
  ADD64(temp, temp, mort_const.additional_closing_fees, setp);
  MULTIPLY64(temp, temp, this_acct->loan, setp);
  ADD64(temp, temp, mort_const.appraisal, setp);
  ADD64(temp, temp, mort_const.credit_report, setp);
  ADD64(temp, temp, title_insurance, setp);
  ADD64(temp, temp, this_acct->downPayment, setp);
  ADD64(this_acct->closingCost, temp, mort_const.closing_cost_other, setp);
  */
  ADD64(this_acct->closingCost, closing_cost, this_acct->downPayment, setp);
  roundToAvailableCoin(&this_acct->closingCost, this_acct->closingCost, CENT);
  return SUCCESS;
}

//This is to compute the penalty due to premature close
//Note that mortgage account close automatically when it matures.
int closePenalty(dec64_t* penalty, mort_acct_t *this_acct){
  int duration = counter - this_acct->open_date;
  int years = (duration/360);
  dec64_t rate;
  if (years==0){
    COPY64(rate, mort_const.premature_close_penalty_year_0);
  }
  else if (years==1){
    COPY64(rate, mort_const.premature_close_penalty_year_1);
  }
  else if (years==2){
    COPY64(rate, mort_const.premature_close_penalty_year_2);
  }
  else if (years==3){
    COPY64(rate, mort_const.premature_close_penalty_year_3);
  }
  else { //years>4
    ZERO64(rate);
  }
  MULTIPLY64(*penalty, this_acct->loan, rate, setp);
  roundToAvailableCoin(penalty, *penalty, CENT);
  return SUCCESS;
}

//This is for premature close
int mortgage_acct_close(htab *mortgage_table, char *acct_no, 
			FILE *fout){
  dec64_t closingCost;
  //Prematurely close
  dec64_t penalty;
  mort_acct_t *this_acct;
  char input_str[STR_MAX_LENGTH];
#ifdef DEBUG
  char str[LENGTH64];
#endif
  if (hfind(mortgage_table, acct_no, strlen(acct_no))==FALSE){
    sprintf(input_str, "Cannot find %s's account\n", acct_no);
    fprintf(stderr, input_str);
    return (!SUCCESS);
  }
  this_acct = (mort_acct_t *)hstuff(mortgage_table);
  closePenalty(&penalty, this_acct);
#ifdef DEBUG
  sprintf(input_str, "###### %s's mortgage account is closed prematurely! ##### \n", this_acct->acct_no);
  errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
  TOSTRING64(this_acct->remaining_principal, str);
  sprintf(input_str, "Remaining Principal: %s\n", str);
  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
  if (this_acct->overdue==0){
#ifdef DEBUG
    TOSTRING64(this_acct->closingCost, str);
    sprintf(input_str, "The Closing Cost: %s\n", str);
    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
    ADD64(closingCost, penalty, this_acct->closingCost, setp);
    ADD64(this_acct->remaining_principal, closingCost, this_acct->remaining_principal, setp);
  }
  else {
    ADD64(this_acct->remaining_principal, this_acct->remaining_principal, penalty, setp);
  }
#ifdef DEBUG
  TOSTRING64(this_acct->remaining_principal, str);
  sprintf(input_str, "The total payment is %s\n", str);
  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
  free(hkey(mortgage_table));
  free(hstuff(mortgage_table));
  hdel(mortgage_table);  
  return SUCCESS;
}

//This is to compute the monthly payment
int termPayment(dec64_t *monthly, int totalTerm, dec64_t rate, dec64_t loan){
  dec64_t temp1, temp2, temp3, r_plus;
  //monthly=principal*r/(1-Math.pow(1+r,-number))
  totalTerm = -totalTerm;
  FROMINTEGER64(temp1, totalTerm, setp); //value = -number
  ADD64(r_plus, rate, ONE, setp);  //r = r+1
  POWER64(temp2, r_plus, temp1, setp); //POWER(1+r, -number);
  SUBTRACT64(temp2, ONE, temp2, setp); //1-POWER(1+r, -number);
  MULTIPLY64(temp3, loan, rate, setp); //principal*r
  DIVIDE64(*monthly, temp3, temp2, setp); //monthly
  roundToAvailableCoin(monthly, *monthly, CENT); //rescale
  return SUCCESS;
}

//This is to compute the number of terms (months)
int termComputation(dec64_t rate, dec64_t loan, dec64_t monthly){
  dec64_t temp1, temp2, r_plus;
  char term_str[LENGTH64];
  ADD64(r_plus, rate, ONE, setp); //r_plus = 1+r  
  LOG1064(temp2, r_plus, setp); //temp2 = log10(r_plus)
  MULTIPLY64(temp1, rate, loan, setp); //temp1 = r*principal
  SUBTRACT64(temp1, monthly, temp1, setp); //temp1 = monthly-temp1
  if(ISNEGATIVE64(temp1)){
    errorPrint(LOC, WARNING_CODE, NOT_ENOUGH_BALANCE, stderr, 
	       "monthly payment is not enough to pay the interest. Illegal operation in non-negative mortgage\n");
    return NOT_ENOUGH_BALANCE;
  }
  DIVIDE64(temp1, monthly, temp1, setp);//temp1 = monthly/temp1
  LOG1064(temp1, temp1, setp); //temp1 = log10(temp1)

  DIVIDE64(temp1, temp1, temp2, setp);//temp1 = temp1/temp2
#ifdef BID
  *setp = BID_ROUNDING_UP;
#else
  setp->round = DEC_ROUND_CEILING;
#endif
  TOINTEGRAL64(temp1, temp1, setp);
#ifdef BID
  *setp = BID_ROUNDING_TO_NEAREST;
#else
  setp->round = DEC_ROUND_HALF_EVEN;
#endif
  TOSTRING64(temp1, term_str);
  return atoi(term_str);
}

//This is to show the schedule table based on the available information
int scheduleTable(FILE *fout, dec64_t loan, int year, 
		  mort_rate_t *mort_rate, int acct_type){
  //The rate here is yearly rate because in schedule table, we don't have
  //an account usually. This action is usually for showing results to customer
  dec64_t r, r_plus;
  dec64_t temp1, temp2, temp3;
  int number = 12*year; //total number of payments
  int neg_number = -number;
  int months;
  dec64_t new_r, monthly;
  dec64_t *principalPay, *interestPay, *remainingPrincipal, totalInterest, 
    totalPayment;
  char principal_string[LENGTH64], interest_string[LENGTH64],
    remainingPrincipal_string[LENGTH64];
  int index;
  dec64_t rate; 
  char input_str[STR_MAX_LENGTH];

  principalPay = (dec64_t *)calloc(number, sizeof(dec64_t));
  interestPay = (dec64_t *)calloc(number, sizeof(dec64_t));
  remainingPrincipal = (dec64_t *)calloc(number, sizeof(dec64_t));

  if (acct_type == FRM15YEAR){
    COPY64(rate, mort_rate->frm15);
  }
  else if (acct_type == FRM30YEAR) {
    COPY64(rate, mort_rate->frm30);
  }
  else if (acct_type == ARM1YEAR) {
    COPY64(rate, mort_rate->arm1);
  }
  else if (acct_type == ARM51YEAR) {
    COPY64(rate, mort_rate->arm51);
  }
  else {
    errorPrint(LOC, ERROR_CODE, MORT_TYPE_WRONG, 
	       stderr, "No this mortgage type!\n");
  }

  DIVIDE64(new_r, rate, HUNDRED, setp);
  if ((checkGEZero(loan)==0)||(checkGEZero(new_r)==0)||
      (checkLEOne(new_r)==0)||(year<=0)){
    errorPrint(LOC, ERROR_CODE, BAD_INPUT, stderr, 
	       "Bad Inputs in Schduling Table");
  }
  ZERO64(totalInterest);
  ZERO64(totalPayment);

  // we want to rewrite this mortgage monthly payment equation  
  // to use fewer divides and a positive exponent for pow
  // it was originally written as:
  //    mnthly_pymt = (loan * rate) / (1-(1+rate)^(-N))
  // we want to rewrite as:
  //    mnthly_pymt = ((loan * rate) * (1+rate)^N) / ((1 + rate)^N)-1
  FROMINTEGER64(temp1, neg_number, setp); //value = -number
  DIVIDE64(r, new_r, TWELVE, setp); //r = rate/12
  ADD64(r_plus, r, ONE, setp);  //r = r+1
  POWER64(temp2, r_plus, temp1, setp); //POWER(1+r, -number);
  SUBTRACT64(temp2, ONE, temp2, setp); //1-POWER(1+r, -number);
  MULTIPLY64(temp3, loan, r, setp); //loan*r
  DIVIDE64(monthly, temp3, temp2, setp); //monthly
  roundToAvailableCoin(&monthly, monthly, CENT); //rescale

  for (months=1; months<=number; months++){
    //remainingPrincipal=loan*(1-Math.pow(1+r,-number+months))/
    //                   (1-Math.pow(1+r,-number));
    FROMINTEGER64(temp3, neg_number+months, setp);
    POWER64(temp3, r_plus, temp3, setp);
    SUBTRACT64(temp3, ONE, temp3, setp);
    MULTIPLY64(remainingPrincipal[months-1], loan, temp3, setp);
    DIVIDE64(remainingPrincipal[months-1], remainingPrincipal[months-1], temp2, setp);
    roundToAvailableCoin(&remainingPrincipal[months-1], 
			 remainingPrincipal[months-1], 
			 temp1);
    //interest[months]=remaining * r
    if (months>1){
      MULTIPLY64(interestPay[months-1], remainingPrincipal[months-2], r, setp);
    }
    else{
      MULTIPLY64(interestPay[0], loan, r, setp);
    }
    roundToAvailableCoin(&interestPay[months-1], 
			 interestPay[months-1], temp1);
    //principalPay=monthly-interest[months];
    SUBTRACT64(principalPay[months-1], monthly, interestPay[months-1], setp);
    roundToAvailableCoin(&principalPay[months-1], 
			 principalPay[months-1], temp1);
    ADD64(totalInterest, totalInterest, interestPay[months-1], setp);
    ADD64(totalPayment, totalPayment, monthly, setp);
  }

  sprintf(input_str, "\t Month \t Principal payment \t Interest payment \t Remaining principal\n");
  errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
  for (index = 0; index < number; index++){
    TOSTRING64(principalPay[index], principal_string);
    TOSTRING64(interestPay[index], interest_string);
    TOSTRING64(remainingPrincipal[index], remainingPrincipal_string);
    sprintf(input_str, "\t %d \t %s \t %s \t %s\n", index,
	    principal_string, interest_string, remainingPrincipal_string);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, fout, input_str);
  }
  for (index=0; index<number; index++){
    free(&principalPay[index]);
    free(&interestPay[index]);
    free(&remainingPrincipal[index]);
  }  
  return SUCCESS;
}

//This is to compute the interest paid in this period.
int interestPaid(dec64_t *current_interest, mort_acct_t *this_acct){
  MULTIPLY64(*current_interest, this_acct->remaining_principal, 
	     this_acct->rate, setp);
  //RESCALE64(*current_interest, *current_interest, MTWO, setp);
  QUANTIZE64(*current_interest, *current_interest, ETWO, setp);
  return SUCCESS;
}
//This is to initialize the mortgage account.
int mortgage_acct_init(mort_acct_t *this_acct, int period, //in months
		       dec64_t loan, dec64_t downPayment, int auto_pay,
		       char *acct_no){
  int length = strlen(acct_no);
  if (length>MAX_ACCT_NAME_LENGTH) {
    errorPrint(LOC, ERROR_CODE, LENGTH_TOO_LONG,
               stderr, "account name is too long\n");
  }
  strcpy(this_acct->acct_no, acct_no);
  this_acct->open_date = counter;
  if (this_acct->acct_type==FRM15YEAR)
    this_acct->period = 15*12; //now the # terms = # periods (month), but may get changed later
  else if (this_acct->acct_type==FRM30YEAR)
    this_acct->period = 30*12; //now the # terms = # periods (month), but may get changed later
  else if (this_acct->acct_type==ARM1YEAR){ //ARM 1 Year or ARM 5/1 YEAR
    if (period<1) { 
      errorPrint(LOC, WARNING_CODE, MORT_LENGTH_FAIL, stderr,
		 "ARM 1Year's length should be greater than 1 year");
      return MORT_LENGTH_FAIL;
    }
    this_acct->period = period*12; //now the # terms = # periods (month), but may get changed later
  }
  else {
    if (period<5) {
      errorPrint(LOC, WARNING_CODE, MORT_LENGTH_FAIL, stderr,
		 "ARM 5/1Year's length should be greater than 5 year");
      return MORT_LENGTH_FAIL;
    }
    this_acct->period = period*12; //now the # terms = # periods (month), but may get changed later
  }
  this_acct->maturity_date = counter+this_acct->period*30;
  this_acct->pay_date = PAY_DATE;
  this_acct->totalTerm = this_acct->period; //now the # terms = # periods (month), but may get changed later
  this_acct->currentTerm = 0;
  this_acct->status = 1;
  this_acct->overdue = 0;
  this_acct->debt_paid = 0;
  COPY64(this_acct->loan, loan);
  ZERO64(this_acct->accu_interest);
  COPY64(this_acct->remaining_principal, loan);
  COPY64(this_acct->downPayment, downPayment);
  closingCost(this_acct);
  this_acct->auto_pay = auto_pay;
  return SUCCESS;
}

//This to inquiry the mortgage account
int mortgage_acct_inquiry(htab *mortgage_table, char *acct_no, FILE *fout){
#ifdef DEBUG
  mort_acct_t *this_acct;
  char temp_str[LENGTH64];
  dec64_t rate;
#endif
  if (hfind(mortgage_table, acct_no, strlen(acct_no))==FALSE){
    char input_str[STR_MAX_LENGTH];
    sprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
#ifdef DEBUG
  this_acct = (mort_acct_t *)hstuff(mortgage_table);
  fprintf(fout, "============= MORTGAGE ACCOUNT REPORT ===============\n");
  fprintf(fout, "Current Date is %d\n", counter);
  fprintf(fout, "Account Number: %s\n", this_acct->acct_no);
  fprintf(fout, "Account Period: %d month(s)\n", this_acct->period);
  fprintf(fout, "Account Opening Date: %d\n", this_acct->open_date);
  fprintf(fout, "Account Mature Date: %d\n", this_acct->maturity_date);
  fprintf(fout, "Account Pay Date: Every %d days\n", this_acct->pay_date);
  fprintf(fout, "Account Total Terms: %d months(s)\n", this_acct->totalTerm);
  fprintf(fout, "Account Current Terms: %d^{th} month\n", this_acct->currentTerm);
  fprintf(fout, "Account Status: %s\n", (this_acct->status==1)?"Active":"Non-Active");
  fprintf(fout, "Account Type: ");
  if(this_acct->acct_type==FRM15YEAR)
    fprintf(fout, "15-Year Fixed Rate Mortgage\n");
  else if(this_acct->acct_type==FRM30YEAR)
    fprintf(fout, "30-Year Fixed Rate Mortgage\n");
  else if(this_acct->acct_type==ARM1YEAR)
    fprintf(fout, "1-Year ARM\n");
  else 
    fprintf(fout, "5/1 ARM\n");
  fprintf(fout, "Account Overdue: %s\n", (this_acct->overdue==1)?"Yes":"No");
  fprintf(fout, "Account Debt Paid? %s\n", (this_acct->debt_paid==1)?"Yes":"Not Yet");
  fprintf(fout, "Account auto-pay from %s Checking account? %s\n", this_acct->acct_no, (this_acct->auto_pay==1)?"Yes":"No");
  MULTIPLY64(rate, this_acct->rate, HUNDRED, setp);
  TOSTRING64(rate, temp_str);
  if (this_acct->overdue==0){
    fprintf(fout, "Account Mortgage Rate (monthly rate): %s (in percentage)\n", temp_str);
    MULTIPLY64(rate, this_acct->start_rate, HUNDRED, setp);
    TOSTRING64(rate, temp_str);
    fprintf(fout, "Account Starting Mortgage Rate (monthly rate): %s (in percentage)\n", temp_str);
  }
  else
    fprintf(fout, "Account Penalty Rate (daily rate): %s (in percentage)\n", temp_str);

  TOSTRING64(this_acct->loan, temp_str);
  fprintf(fout, "Account finance part (The part that needs mortgage): $%s\n", temp_str);
  TOSTRING64(this_acct->monthly, temp_str);
  fprintf(fout, "Account Monthly Payment: $%s\n", temp_str);
  TOSTRING64(this_acct->accu_interest, temp_str);
  fprintf(fout, "Account Accumulated Interest paid so far: $%s\n", temp_str);
  TOSTRING64(this_acct->remaining_principal, temp_str);
  fprintf(fout, "Account Remaining Principal: $%s\n", temp_str);
  TOSTRING64(this_acct->point, temp_str);
  fprintf(fout, "Account Discount Point: %s (in percentage)\n", temp_str);
  TOSTRING64(this_acct->margin, temp_str);
  fprintf(fout, "Account Margin : %s (in percentage)\n", temp_str);
  TOSTRING64(this_acct->downPayment, temp_str);
  fprintf(fout, "Account Down Payment: $%s\n", temp_str);
  TOSTRING64(this_acct->closingCost, temp_str);
  fprintf(fout, "Account Closing Cost: $%s\n", temp_str);
#endif
  return SUCCESS;
}


//This is to pay the monthly payment
int mortgage_acct_pay(htab *mortgage_table, char *acct_no, FILE *fout){
  char input_str[STR_MAX_LENGTH];
  mort_acct_t *this_acct;
  if (hfind(mortgage_table, acct_no, strlen(acct_no))==FALSE){
    char input_str[STR_MAX_LENGTH];
    sprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (mort_acct_t *)hstuff(mortgage_table);
  if (this_acct->status==0){
    sprintf(input_str, "%s's account is not active\n", this_acct->acct_no);
    errorPrint(LOC, WARNING_CODE, NOT_ACTIVE, stderr, input_str);
    return NOT_ACTIVE;
  }
  if (this_acct->overdue){
    sprintf(input_str, "Account overdue. Mortgage Pay for %s is rejected\n", 
	    this_acct->acct_no);
    errorPrint(LOC, WARNING_CODE, MORT_ACCT_OVERDUE, stderr, input_str);
    return MORT_ACCT_OVERDUE;
  }
  if (this_acct->debt_paid){
    sprintf(input_str, "%s have paid the monthly payment. Mortgage Pay is rejected\n", this_acct->acct_no);
    errorPrint(LOC, WARNING_CODE, MORT_DEBT_PAID, stderr, input_str);
    return MORT_DEBT_PAID;
  }
  this_acct->debt_paid = 1;
#ifdef DEBUG
  sprintf(input_str, "###### Monthly payment for %s's Mortgage is received on Day %d ######\n",
	  this_acct->acct_no, counter);
  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
#endif
  return SUCCESS;
}

//This is to parse the current mortgage rate based on the acct type.
int getMortgageRate(mort_acct_t *this_acct, mort_rate_t *mort_rate){
  if (this_acct->acct_type == FRM15YEAR){
    COPY64(this_acct->start_rate, mort_rate->frm15);
    COPY64(this_acct->rate, mort_rate->frm15);
    COPY64(this_acct->point, mort_rate->frm15Point);
    ZERO64(this_acct->margin);
  }
  else if (this_acct->acct_type == FRM30YEAR) {
    COPY64(this_acct->start_rate, mort_rate->frm30);
    COPY64(this_acct->rate, mort_rate->frm30);
    COPY64(this_acct->point, mort_rate->frm30Point);
    ZERO64(this_acct->margin);
  }
  else if (this_acct->acct_type == ARM1YEAR) {
    COPY64(this_acct->start_rate, mort_rate->arm1);
    COPY64(this_acct->rate, mort_rate->arm1);
    COPY64(this_acct->point, mort_rate->arm1Point);
    COPY64(this_acct->margin, mort_rate->arm1Margin);
  }
  else if (this_acct->acct_type == ARM51YEAR) {
    COPY64(this_acct->start_rate, mort_rate->arm51);
    COPY64(this_acct->rate, mort_rate->arm51);
    COPY64(this_acct->point, mort_rate->arm51Point);
    COPY64(this_acct->margin, mort_rate->arm51Margin);
  }
  else {
    errorPrint(LOC, ERROR_CODE, MORT_TYPE_WRONG, 
	       stderr, "No this mortgage type!\n");
  }
  DIVIDE64(this_acct->rate, this_acct->rate, TWELVE, setp);
  //RESCALE64(this_acct->rate, this_acct->rate, RATE_PRECISION, setp);
  QUANTIZE64(this_acct->rate, this_acct->rate, RATE_PRECISION, setp);
  DIVIDE64(this_acct->rate, this_acct->rate, HUNDRED, setp);
  DIVIDE64(this_acct->start_rate, this_acct->start_rate, TWELVE, setp);
  //RESCALE64(this_acct->start_rate, this_acct->start_rate, RATE_PRECISION, setp);
  QUANTIZE64(this_acct->start_rate, this_acct->start_rate, RATE_PRECISION, setp);
  DIVIDE64(this_acct->start_rate, this_acct->start_rate, HUNDRED, setp);
  DIVIDE64(this_acct->point, this_acct->point, HUNDRED, setp);
  DIVIDE64(this_acct->margin, this_acct->margin, HUNDRED, setp);
  return SUCCESS;
}

int mortgage_acct_open(htab *mortgage_table, htab *checking_table,
		       mort_rate_t *mort_rate, int period,
                       dec64_t loan, dec64_t downPayment,
                       char *acct_no, int acct_type, int auto_pay,
		       FILE *fout) {
  //we handle two mortgage-acct methods
  //1. Given the period (years), the interest rate, the principal to calculate the monthly payment
  //2. Given the monthly payment, the principal, and the interest rate, calculate the period
  deposit_acct_t *deposit_acct;
  mort_acct_t *this_acct;
  int keyl;
  ub1 *key;
  int ret;
  char input_str[STR_MAX_LENGTH];
  keyl = strlen((char *)acct_no);
  if (hadd(mortgage_table, acct_no, keyl, (void *)0)==FALSE) {
    sprintf(input_str, "[mortgage_acct_open] cannot add %s's account\n",
            acct_no);
    errorPrint(LOC, WARNING_CODE, ADD_ACCT_FAIL, fout, input_str);
    return ADD_ACCT_FAIL;
  } 
  /* if not a duplicate */
  key = (ub1 *)malloc(keyl);       /* dumb use of malloc */
  memcpy(key, acct_no, keyl);          /* copy buf into key */
  hkey(mortgage_table)=key;                     /* replace buf with key */
  this_acct = calloc(1, sizeof(mort_acct_t));
  this_acct->acct_type = acct_type;
  TOSTRING64(loan, input_str);
  ret = mortgage_acct_init(this_acct, period, loan, downPayment, auto_pay, 
			   acct_no);
  if (ret==0)    //other stuffs
    hstuff(mortgage_table)=(void*)this_acct;
  else {
    sprintf(input_str, "Initialization fail at %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_INIT_FAIL, fout, input_str);
    free(hkey(mortgage_table));
    free(this_acct);
    hdel(mortgage_table);
    return ACCT_INIT_FAIL;
  }
  if (auto_pay!=0){//check the existence of the checking account
    if (hfind(checking_table, acct_no, strlen(acct_no))==FALSE){
      char input_str[STR_MAX_LENGTH];
      sprintf(input_str, "###### Checking Acct for %s doesn't exist. Mortgage account open fails ######\n", acct_no);
      errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, stderr, input_str);
      free(hkey(mortgage_table));
      free(this_acct);
      hdel(mortgage_table);
      return ACCT_NOT_EXIST;
    }
    deposit_acct = (deposit_acct_t *)hstuff(checking_table);
  }
  getMortgageRate(this_acct, mort_rate);
  termPayment(&this_acct->monthly, this_acct->period, this_acct->rate, loan);
#ifdef DEBUG
  {
    char input_str[STR_MAX_LENGTH];
    char fund_str[LENGTH64];
    char downpay_str[LENGTH64];
    TOSTRING64(loan, fund_str);
    TOSTRING64(downPayment, downpay_str);
    sprintf(input_str, "###### %s's MORTGAGE acct with financed part: $%s and downpayment: $%s is opened ######\n", this_acct->acct_no, fund_str, downpay_str);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, stderr, input_str);
  }
#endif
  return SUCCESS;
}

//Mortgage refinancing. 
int mortgage_acct_refinance(htab *mortgage_table, htab *checking_table,
			    mort_rate_t *mort_rate, int period,
			    char *acct_no, int acct_type, int auto_pay,
			    FILE *fout){
  mort_acct_t *this_acct;
  deposit_acct_t *deposit_acct;
  char input_str[STR_MAX_LENGTH];
  //We need to make sure no duplication of the acct in the acct_list
  if (hfind(mortgage_table, acct_no, strlen(acct_no))==FALSE){
    sprintf(input_str, "Cannot find %s's account\n", acct_no);
    errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, fout, input_str);
    return ACCT_NOT_EXIST;
  }
  this_acct = (mort_acct_t *)hstuff(mortgage_table);
  this_acct->acct_type = acct_type;
  if (auto_pay!=0){//check the existence of the checking account
    if (hfind(checking_table, acct_no, strlen(acct_no))==FALSE){
      char input_str[STR_MAX_LENGTH];
      sprintf(input_str, "###### Checking Acct for %s doesn't exist. Mortgage account open fails ######\n", acct_no);
      errorPrint(LOC, WARNING_CODE, ACCT_NOT_EXIST, stderr, input_str);
      return ACCT_NOT_EXIST;
    }
    deposit_acct = (deposit_acct_t *)hstuff(checking_table);
  }
  getMortgageRate(this_acct, mort_rate);
  mortgage_acct_init(this_acct, period, this_acct->remaining_principal, 
		     this_acct->downPayment, auto_pay, acct_no);
  termPayment(&this_acct->monthly, this_acct->period, this_acct->rate, 
	      this_acct->remaining_principal);
#ifdef DEBUG
 {
   char fund_str[LENGTH64];
   char downpay_str[LENGTH64];
   TOSTRING64(this_acct->remaining_principal, fund_str);
   TOSTRING64(this_acct->downPayment, downpay_str);
   sprintf(input_str, "###### %s's MORTGAGE acct with re-financed part: $%s and downpayment: $%s is opened ######\n", 
	   this_acct->acct_no, fund_str, downpay_str);
   errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, stderr, input_str);
 }
#endif
 return SUCCESS;
}

int penaltyOverdue(mort_acct_t *this_acct, mort_rate_t *mort_rate){
  dec64_t daily_rate;
  dec64_t rate, point, margin;
  dec64_t surcharge, 
    min_overdue_yr_penalty_rate,
    min_overdue_yr_penalty_point,
    min_overdue_yr_penalty_margin;

  //penalty surcharge = loan amount * OVERDUE_SURCHARGE_RATE
  MULTIPLY64(surcharge, this_acct->loan, 
	     mort_const.overdue_surcharge_rate, setp);
  roundToAvailableCoin(&surcharge, surcharge, CENT);
  ADD64(this_acct->remaining_principal, this_acct->remaining_principal, 
	surcharge, setp);
  //daily rate!
  COPY64(min_overdue_yr_penalty_rate, mort_const.min_overdue_yr_penalty_rate);
  DIVIDE64(min_overdue_yr_penalty_rate, min_overdue_yr_penalty_rate, YEAR, 
	   setp);
  DIVIDE64(min_overdue_yr_penalty_rate, min_overdue_yr_penalty_rate, HUNDRED, 
	   setp);
  COPY64(min_overdue_yr_penalty_point, 
	 mort_const.min_overdue_yr_penalty_point);
  DIVIDE64(min_overdue_yr_penalty_point, min_overdue_yr_penalty_point, 
	   HUNDRED, setp);
  COPY64(min_overdue_yr_penalty_margin, 
	 mort_const.min_overdue_yr_penalty_margin);
  DIVIDE64(min_overdue_yr_penalty_margin, min_overdue_yr_penalty_margin, 
	   HUNDRED, setp);
  if (this_acct->acct_type == FRM15YEAR){
    COPY64(rate, mort_rate->frm15);
    COPY64(point, mort_rate->frm15Point);
    ZERO64(margin);
  }
  else if (this_acct->acct_type == FRM30YEAR) {
    COPY64(rate, mort_rate->frm30);
    COPY64(point, mort_rate->frm30Point);
    ZERO64(margin);
  }
  else if (this_acct->acct_type == ARM1YEAR) {
    COPY64(rate, mort_rate->arm1);
    COPY64(point, mort_rate->arm1Point);
    COPY64(margin, mort_rate->arm1Margin);
  }
  else if (this_acct->acct_type == ARM51YEAR) {
    COPY64(rate, mort_rate->arm51);
    COPY64(point, mort_rate->arm51Point);
    COPY64(margin, mort_rate->arm51Margin);
  }
  DIVIDE64(rate, rate, YEAR, setp);
  DIVIDE64(rate, rate, HUNDRED, setp);
  DIVIDE64(point, point, YEAR, setp);
  DIVIDE64(point, point, HUNDRED, setp);
  DIVIDE64(margin, margin, YEAR, setp);
  DIVIDE64(margin, margin, HUNDRED, setp);
  
  DIVIDE64(daily_rate, this_acct->rate, MONTH, setp);
  MAX64(rate, rate, daily_rate, setp);
  MAX64(rate, rate, min_overdue_yr_penalty_rate, setp);
  //RESCALE64(rate, rate, RATE_PRECISION, setp);
  QUANTIZE64(rate, rate, RATE_PRECISION, setp);
  COPY64(this_acct->rate, rate);
  if ((this_acct->acct_type == ARM1YEAR)||(this_acct->acct_type == ARM51YEAR)){
    MAX64(point, point, this_acct->point, setp);
    MAX64(point, point, min_overdue_yr_penalty_point, setp);
    COPY64(this_acct->point, point);
    MAX64(margin, margin, this_acct->margin, setp);
    MAX64(margin, margin, min_overdue_yr_penalty_margin, setp);
    COPY64(this_acct->margin, margin);
  }
  return SUCCESS;
}

int endOfTheDayMortgage(htab *mortgage_table, htab *checking_table,
			FILE *fout, dec64_t **conversion_rate,
                        deposit_rate_t *deposit_rate,
                        mort_rate_t *mort_rate,
                        dec64_t tcm_rate
#ifdef TEST
			, FILE *ascii_file, FILE *bin_file
#endif
			){
  mort_acct_t *this_acct;
  if (hfirst(mortgage_table)){
    do {
      this_acct = (mort_acct_t *)hstuff(mortgage_table);
      if (this_acct->overdue){
	dec64_t temp;
#ifdef DEBUG
        errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE,
                   fout, "Account is in the overdue status\n");
#endif
        //if this overdues, then we increase the remaining principal daily.
        //now the rate becomes the penalty rate (daily)
        ADD64(temp, ONE, this_acct->rate, setp);
        MULTIPLY64(this_acct->remaining_principal, 
		   this_acct->remaining_principal, temp, setp);
        roundToAvailableCoin(&this_acct->remaining_principal,
			     this_acct->remaining_principal, CENT);
      }
      else if (((counter-this_acct->open_date)%this_acct->pay_date==0)&&
               (counter!=this_acct->open_date)){
        //First, check if the pay date arrives
	//for test generator only. Normally, people pay ontime
        if (this_acct->auto_pay) {
          int ret;
          deposit_acct_t *deposit_acct;
	  if (hfind(checking_table, this_acct->acct_no, 
		    strlen(this_acct->acct_no))==FALSE){
	    char input_str[STR_MAX_LENGTH];
	    sprintf(input_str, "The auto-pay of %s Mortgage acct on Day %d is cancelled!\n Remember to deposit more money for the next withdrawl\n", 
		    this_acct->acct_no, counter);
	    errorPrint(LOC, WARNING_CODE, NOT_ENOUGH_BALANCE, stderr, 
		       input_str);
	    this_acct->overdue=1;
	    this_acct->auto_pay=0; //reset the autopay to zero
	    this_acct->debt_paid=0;
	  }
	  else {
	    deposit_acct = (deposit_acct_t *)hstuff(checking_table);
	    if (this_acct->currentTerm == this_acct->totalTerm-1){
	      dec64_t temp;
	      ADD64(temp, ONE, (this_acct->rate), setp);
	      MULTIPLY64(temp, this_acct->remaining_principal, temp, setp);
	      ret = money_withdrawl(checking_table, this_acct->acct_no, 
				    COUNTRIES, temp, conversion_rate, fout, 
				    deposit_rate);
	    }
	    else {
	      ret = money_withdrawl(checking_table, this_acct->acct_no, 
				    COUNTRIES, this_acct->monthly, 
				    conversion_rate, fout,
				    deposit_rate);
	    }
	    if (ret!=0){
	      //Not good!
	      char input_str[STR_MAX_LENGTH];
	      sprintf(input_str, "The auto-pay of %s Mortgage acct on Day\
 %d is cancelled!\n Remember to deposit more money for the next withdrawl\n",
		      this_acct->acct_no, counter);
	      errorPrint(LOC, WARNING_CODE, NOT_ENOUGH_BALANCE, stderr,
			 input_str);
	      this_acct->debt_paid = 0;
	    }
	    //deposit account is recorded during the withdrawl!
	    this_acct->debt_paid=1;
	  }
	}
#ifdef TEST
	else {
	  cmd64_t this_cmd64;
	  int ret;
	  ret = mortgage_acct_pay(mortgage_table, this_acct->acct_no, fout);
	  if (ret==0){
	    strcpy(this_cmd64.acct_no, this_acct->acct_no);
         #ifdef DECNUMBER_ARBITRARY
            decimal64FromString(&this_cmd64.fund, "0", setp);
	    decimal64FromString(&this_cmd64.str1, "0", setp);
         #else
            ZERO64(this_cmd64.fund);
            ZERO64(this_cmd64.str1);
         #endif

	    this_cmd64.action = MORT_PAY;
	    this_cmd64.length = 0;
	    this_cmd64.acct_type = this_acct->acct_type;
	    this_cmd64.select = 0;
	    fprintf(ascii_file, "%-16s %-3d %-19s %-6d %-8d %-8d %-19s\n",
		    this_acct->acct_no, MORT_PAY, "0",
		    this_acct->acct_type, 0,
		    0, "0");
#ifdef DEBUG
	    fprintf(stderr, "%-16s %-3d %-19s %-6d %-8d %-8d %-19s\n",
		    this_acct->acct_no, MORT_PAY, "0",
		    this_acct->acct_type, 0,
		    0, "0");
#endif
	    fwrite(&this_cmd64, sizeof(cmd64_t), 1, bin_file);
	  }
	}
#endif
        if (this_acct->debt_paid==0){
          errorPrint(LOC, INFO_CODE, INFO_CODE, fout, "Account overdue! \n");
          this_acct->overdue=1;
	  //apply penalty, rate, and increase the remaining principal.
          penaltyOverdue(this_acct, mort_rate); 
	  continue;
        }
        else {//have paid
          dec64_t current_interest;
          this_acct->currentTerm++;
          interestPaid(&current_interest, this_acct);
          remainingPrincipal_fcn(this_acct);
          ADD64(this_acct->accu_interest, this_acct->accu_interest, 
		current_interest, setp);
          this_acct->debt_paid = 0;
        }
        if (counter==this_acct->maturity_date){
          //maturity date, show the details and close the acct.
#ifdef DEBUG
	  {
	    char input_str[STR_MAX_LENGTH];
	    sprintf(input_str, "on Day %d, %s mortgage account is matured!\n",
		    counter, this_acct->acct_no);
	    errorPrint(LOC, INFO_CODE, INFO_CODE, fout, input_str);
	  }
#endif
	  free(hkey(mortgage_table));
	  free(hstuff(mortgage_table));
	  hdel(mortgage_table);
	  continue;
        }
	/*
	//The next term is the last term
	if ((this_acct->currentTerm == this_acct->totalTerm-1)&&
	    ((this_acct->acct_type==FRM15YEAR)||
	     (this_acct->acct_type==FRM30YEAR))){
#ifdef DEBUG
	  errorPrint(LOC, INFO_CODE, INFO_CODE, fout, 
		     "Change the monthly payment for FRM for the last term\n");
#endif
	  termPayment(&this_acct->monthly,
		      this_acct->totalTerm-this_acct->currentTerm,
		      this_acct->rate, this_acct->remaining_principal);
	}
	*/
      }
      if ((counter%360==0)&&
	  (counter!=this_acct->open_date)&&
	  (this_acct->currentTerm != this_acct->totalTerm)&&
	   ((this_acct->acct_type==ARM51YEAR)||
	    (this_acct->acct_type==ARM1YEAR))){
        //Then, update monthly payment and interest rate for ARM
        determineNewRate(this_acct, tcm_rate);
        //currentTerm is incremented above
        termPayment(&this_acct->monthly, 
		    this_acct->totalTerm-this_acct->currentTerm, 
		    this_acct->rate, this_acct->remaining_principal);
      }
    }while(hnext(mortgage_table));
  }
  return SUCCESS;
}

int remainingPrincipal_fcn(mort_acct_t *this_acct){
  dec64_t monthly_interest;
  // first determine monthly interest payment
  dec64_t temp;
  MULTIPLY64(monthly_interest, this_acct->remaining_principal, 
	     this_acct->rate, setp);
  // the amount paid down is the monthly payment minus the portion that went to interest
  SUBTRACT64(temp, this_acct->monthly, monthly_interest, setp);
  SUBTRACT64(this_acct->remaining_principal, this_acct->remaining_principal, 
	     temp, setp);
  //specialRescale(&this_acct->remaining_principal, 
//		 this_acct->remaining_principal, MTWO);
  specialRescale(&this_acct->remaining_principal, 
		 this_acct->remaining_principal, ETWO);	// Switched to use QUANTIZE
  return SUCCESS;
}

int determineNewRate(mort_acct_t *this_acct, dec64_t tcm_rate){
  dec64_t temp1, temp2;
  dec64_t six;
  if (((this_acct->acct_type == ARM51YEAR)&&(counter<5*360))||
      (this_acct->acct_type == FRM15YEAR)||
      (this_acct->acct_type == FRM30YEAR)
      ){
    return SUCCESS;
  }
  ADD64(six, TEN, MFOUR, setp);
  // after first 1 or 5 years, rate is the less of (last_years_rate + 2%)
  // or (t-bill rate + margin)
  // max adjustment is last year's rate + 2%
  //Need to convert it back to yearly rate first
  
  MULTIPLY64(temp1, this_acct->margin, HUNDRED, setp);
  ADD64(temp1, tcm_rate, temp1, setp);
  MULTIPLY64(temp2, HUNDRED, this_acct->rate, setp);
  MULTIPLY64(temp2, TWELVE, temp2, setp);
  ADD64(temp2, temp2, TWO, setp);
  MIN64(temp1, temp1, temp2, setp);
  MULTIPLY64(temp2, HUNDRED, this_acct->start_rate, setp);
  MULTIPLY64(temp2, temp2, TWELVE, setp);
  ADD64(temp2, temp2, six, setp);
  MIN64(this_acct->rate, temp1, temp2, setp);
  DIVIDE64(this_acct->rate, this_acct->rate, TWELVE, setp);
  //RESCALE64(this_acct->rate, this_acct->rate,  RATE_PRECISION, setp);
  QUANTIZE64(this_acct->rate, this_acct->rate,  RATE_PRECISION, setp);
  DIVIDE64(this_acct->rate, this_acct->rate, HUNDRED, setp);  
  return SUCCESS;
}
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-19 21:40:13 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: mortgage.c,v $
  Revision 1.6  2009-04-19 21:40:13  mjanders
  Updated decnumber and BID libraries

  Revision 1.5  2008-11-24 08:58:48  mjanders
  Added BID support

  Revision 1.4  2008/05/15 21:55:25  stsen
  compiles correctly for each SW library (except we're missing LOG and POW from IDFPL)

  Revision 1.3  2008/05/15 17:04:12  stsen
  added variable neg_number for -number

  Revision 1.2  2008/05/14 22:24:51  stsen
  updated for BID

  Revision 1.1.1.1  2008/04/18 01:13:49  mjanders


  Revision 1.20  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.19  2007/03/14 18:09:55  lwang
  temp check-in

  Revision 1.18  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.14  2006/11/30 15:01:20  lwang
  Clecn up several bug in mortgage and rate file

  Revision 1.13  2006/11/30 03:25:01  lwang
  fix problem on the size of the acct name and linked list

  Revision 1.12  2006/11/22 01:40:31  lwang
  comment added

  Revision 1.11  2006/11/21 03:02:44  lwang
  Major Revision with new technique to access database

  Revision 1.10  2006/11/19 23:23:09  lwang
  First draft of Banking System is done

  Revision 1.9  2006/11/17 02:20:00  lwang
  Credit Card benchmark added

  Revision 1.8  2006/11/11 02:12:50  lwang
  included Chuck's ARM

  Revision 1.7  2006/11/11 02:12:07  lwang
  new checkin

  Revision 1.6  2006/11/07 08:26:02  lwang
  After spliting CD and Checking database

  Revision 1.5  2006/11/07 05:20:10  lwang
  Name of the functions changed. Before spliting CD and CK

  Revision 1.4  2006/11/07 03:34:01  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.3  2006/11/03 02:20:24  lwang
  initial check-in

  Revision 1.2  2006/10/31 11:45:22  lwang
  minor update

  Revision 1.1  2006/10/28 01:54:18  lwang
  Banking System Initial Check-in

*/
