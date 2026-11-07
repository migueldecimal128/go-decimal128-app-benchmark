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
// Id: $Id: const.h,v 1.14 2009-04-19 21:40:07 mjanders Exp $
// Date: $Date: 2009-04-19 21:40:07 $
// Author: $Author: mjanders $
// =============================================================================

/*
 * Filename: const.h
 * Purpose: Define constants and structures for Banking. Also
 * define the dec_t type.
 *
 * dec_t is the decimal floating point data type used throughout
 * Banking.  
 */


#ifndef CONST_H
#define CONST_H

#if defined(GCC) || defined(DEC32) || defined(DEC64) || defined(DEC128)

   #ifdef WIDTH64
    #define DECNUMDIGITS 16
   #else
    #define DECNUMDIGITS 34
   #endif

   #ifdef DECNUMBER_ARBITRARY
    #ifdef WIDTH64
     #define dec_t decNumber
     #define DEC_LENGTH64  DECIMAL64_Pmax
    #endif
    #ifdef WIDTH128
     #define dec_t decNumber 
     #define DEC_LENGTH128  DECIMAL128_Pmax
    #endif
   #endif // DECNUMBER_ARBITRARY

   #ifdef DECNUMBER_FIXED 
    #ifdef WIDTH64
     #define DEC_LENGTH64  DECIMAL64_Pmax
     #define dec_t decDouble
    #endif
    #ifdef WIDTH128
     #define dec_t decQuad
     #define DEC_LENGTH128  DECIMAL128_Pmax
    #endif
   #endif // DECNUMBER_FIXED

   #ifdef BID
    #ifdef WIDTH64
     #define dec_t BID_UINT64
     #define DEC_LENGTH64  16 
    #endif
    #ifdef WIDTH128
      #define dec_t BID_UINT128
      #define DEC_LENGTH128 34 
    #endif
   #endif // BID
   
   #ifdef NATIVE_GCC_DPD 
    #define DEC_LENGTH64  DECIMAL64_Pmax
    #define dec_t decDouble
   #endif // DECNUMBER_FIXED
   
   #ifdef NATIVE_GCC_BID
    #define dec_t _Decimal64 
    #define DEC_LENGTH64  16 
   #endif // BID
   
   #include "dfp_includes.h"
   #include "dfp_datatypes.h"
   #define EXTRA_STR_LENGTH 14
   #define LENGTH64  (DEC_LENGTH64 + EXTRA_STR_LENGTH)
   #define LENGTH128  (DEC_LENGTH128 + EXTRA_STR_LENGTH)
   #if defined(WIDTH64) 
    #define LENGTH_T LENGTH64
   #else
    #define LENGTH_T LENGTH128 
   #endif 
#else //use binary or Intel's library
   #define dec_t double
#endif // defined(GCC) || defined(DEC32) || defined(DEC64) || defined(DEC128)

#define STR_MAX_LENGTH 300
#define PATHNAME_MAX_LENGTH 300
// How often (days) do we pay interest
#define PAY_DATE 30
//Define the precision
#define RATE_PRECISION (MFOUR)
//Define account type
#define CHECKING 0
#define CD 1
#define CASH 2
#define MORTGAGE 3
#define CREDIT_CARD 7
//Define the special switches
#define DELETE_FILES_AFTER_CLOSE 1
//Define Customer and Non-customer
#define NON_CUSTOMER 0
#define CUSTOMER 1
//Define the length of the command file
#define CMD_LENGTH 6
//Define the length of the account name
#define MAX_ACCT_NAME_LENGTH 16
//Define the length of max buffer
#define MAX_BUF_SIZE 100
//Define the number of currency units that can be used in Checking/CD
#define COUNTRIES 9
//Define Checking Action (for check_duplicate_acct)
#define TESTING 1
#define NORMAL_ACCESS 0
#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)
#ifdef SHOW_LINE_NUMBERS
#define LOC __FILE__ ":" TOSTR(__LINE__)
#else
#define LOC __FILE__ 
#endif
#define ACCT_TYPE(X, Y){ \
  if (mystrlen(Y)>STR_MAX_LENGTH)\
    errorPrint(LOC, ERROR_CODE, ERROR_CODE, stderr, \
               "String length is too short\n");\
  if ((X)==CHECKING)\
    mystrcpy(Y, "CHECKING");\
  else if ((X)==CD)\
    mystrcpy(Y, "CD");\
  else if ((X)==FRM15YEAR)\
    mystrcpy(Y, "FRM 15 YEAR");\
  else if ((X)==FRM30YEAR)\
    mystrcpy(Y, "FRM 30 YEAR");\
  else if ((X)==ARM1YEAR)\
    mystrcpy(Y, "ARM 1 YEAR");\
  else if ((X)==ARM51YEAR)\
    mystrcpy(Y, "ARM 5/1 YEAR");\
  else if ((X)==CREDIT_CARD)\
    mystrcpy(Y, "CREDIT CARD");\
  else{\
    char _eStr[STR_MAX_LENGTH];\
    mysprintf(_eStr, "Unknown Input Acct Type: %d\n", (X));\
    errorPrint(LOC, ERROR_CODE, ERROR_CODE, stderr, _eStr);\
  }\
}
#define MSG_LENGTH 100
#define INFO_CODE 0
#define WARNING_CODE 1
#define ERROR_CODE 2
#define INFO_CODE_NO_LINE 1

#define FILE_NOT_EXIST -2
#define FILE_WRITE_DENY -3
#define FILE_READ_DENY -4
#define FILE_CLOSE_FAIL -5
#define FILE_REMOVE_FAIL -6
#define ACCT_NOT_EXIST -7
#define ACCT_LENGTH_FAIL -8
#define ACCT_TYPE_WRONG -9
#define ACCT_EXIST_AND_ACTIVE -10
#define NOT_ENOUGH_BALANCE -11
#define CC_WRONG_TX_TYPE -12
#define CC_REJ_PAY -13
#define CC_REJ_TX -14
#define MORT_ACCT_OPEN_REJ -15
#define WRONG_ACTION -16
#define CD_NO_WIRETX -17
#define CUR_NOT_SUPPORT -18
#define CD_CK_NO_MONEY -19
#define CD_CK_BIG_ERROR -20
#define CD_BAD_PERIOD -21
#define WRONG_CURRENCY -22
#define LENGTH_TOO_LONG -23
#define MORT_TYPE_WRONG -24
#define BAD_INPUT -25
#define NOT_ACTIVE -26
#define MORT_ACCT_OVERDUE -27
#define MORT_DEBT_PAID -28
#define MORT_REFINANCE_FAIL -29
#define ACCESS_FILE_FAIL -30
#define SPARC_SETUP_FAIL -31
#define SPARC_SAMPLE_FAIL -32
#define READ_WRITE_SIZE_FAIL -33
#define ADD_ACCT_FAIL -34
//Command Structure
typedef struct cmd_struct{
  char acct_no[MAX_ACCT_NAME_LENGTH];
  dec_t fund;
  dec_t str1; //can be used for downPayment in mortgage
  int action;
  //1, 3, or 6 for CD and length for ARM
  //Also the currency type for Credit Card Debt
  int length;
  // 0: Checking, 1: CD, 2: Cash, 3: 15-Year FRM, 4: 30-Year FRM
  // 5: 1-Year ARM, 6: 5/1-Year ARM, 7:Credit Card
  int acct_type;
  //can be "currency unit" for CD and Credit Card,
  //"auto_pay" and currency for mortgage
  //payment/expense type for Credit Card if it's account_pay
  int select;
}cmd_t;

typedef struct deposit_acct{
  //all the day computation assume 30 days in each month
  //30/360 rule used in the financial world
  char acct_no[MAX_ACCT_NAME_LENGTH];      //also the filename
  int period;             //period in days
  int open_date;          //opening date using the counter value
  int maturity_date;      //maturity date using the counter value
  int pay_date;           //how often do we pay the interest
  int set_close;          //close at maturity
  int extend;             //extend after maturity
  int acct_type;          //account type: CD or checking
  int currency;           //the currency unit
  int last_withdrawl_unit;//the currency unit for the last withdrawl
  int last_deposit_unit;  //the currency unit for the last deposit
  dec_t rate;              //interest rate
  dec_t interest;          //accumulated interest for the whole period
  dec_t unposted_interest; //accumulated interest during this period
  dec_t last_statement;    //the balance in the end of the last maturity
  dec_t last_deposit;      //the amount the user last deposit
  dec_t last_withdrawl;    //the amount the user last withdrawl
  dec_t current_balance;   //the current balance
}deposit_acct_t;

typedef struct mort_acct{
  //all the day computation assume 30 days in each month
  //30/360 rule used in the financial world
  char acct_no[MAX_ACCT_NAME_LENGTH];
  int period;
  int open_date;
  int maturity_date;
  int pay_date;
  int totalTerm;
  int currentTerm;
  int status;
  int acct_type;
  int overdue;
  int debt_paid;
  int auto_pay;           //automatically pay from his checking account
  dec_t start_rate; //use for ARM
  dec_t rate;
  dec_t loan;
  dec_t monthly;
  dec_t accu_interest;
  dec_t remaining_principal;
  dec_t point;
  dec_t downPayment;
  dec_t closingCost;
  dec_t margin;
}mort_acct_t;

typedef struct cc_acct{
  char acct_no[MAX_ACCT_NAME_LENGTH];    //also the filename
  int start_date;          //opening date using the counter value
  int pay_date;            //how often should the account be managed
  int close_date;          //the date to check the monthly balance
  int last_date;           //the last date to pay the purchase expense
  //There is no last date for other type since they accumulate interest immediately.
int status;              //active or non-action
  //int currency;            //the acct currency unit: always US dollar
  //the below two signals should be reset on close date
  int underpaid;    //the user doesn't pay the min required amount (purchase)
  int fullpaid;     //the user doesn't pay the full amount (purchase)
  int overdueHistory;      //the number of overdue
  int autopay;             //autopay from a checkin gaccount
  //there is no intro apr for purchase
  int intro_apr_bal_tx_end_date;  //intro apr for balance transfer
  int last_tx_unit;        //the currency unit for the last transaction
  int last_tx_type;        //the type of the last transaction
  int overCreditLimit;     //Over credit limit
  //if the user doesn't pay at all, set all rates to default rate
  //once it is set, no come back
  int all_rate_default;

  //HISTORY and RECORD
  //the payment users need to pay before the last date (purchase)
  dec_t purchase_statement;
  //The total amount that has been paid after we have the statement
  dec_t purchase_total_paid;        //The amount that has been paid to "PURCHASE"
  dec_t last_paid;         //the amount the user last deposit
  dec_t last_tx;           //the amount of the last transaction
  dec_t credit_limit;       //the credit limit
  //the upper bound where the system rejects the transaction. It's set to "-credit limit" now
  dec_t credit_limit_upper;;
  dec_t credit_allowance;
  //Reward doesn't get implemented now
  //dec_t points
  //APR
  dec_t purchase_apr;      //purchase interest rate
  dec_t cash_adv_apr;  //cash advance interest rate
  dec_t bal_tx_apr;     //transfer a balance from another card
  //this happens when you don't pay or underpay your monthly payment (purchase)
  dec_t default_apr;
  //TRANSACTION
  //accumulated interest for the whole period for purchase transaction
  //the interest rate may be default rate or purchase rate
  //NOTE: when paid, always payoff the amount from the lowest interest rate
  dec_t purchase;           //purchase transaction
  dec_t posted_interest_purchase;
  dec_t cash_adv;      //cash advance amount
  dec_t posted_interest_cash_adv;   
//accumulated interest for the whole period for cash advance and balance transfer
  dec_t bal_tx;  //expense from the balance transfer
  dec_t posted_interest_bal_tx;  //balance transfer interest
  //extra fee due to annual fee, cash advance, balance transfer fee, late payment fee, over-the-credit-limit fee,
  //foreign transaction
  dec_t min_payment;   //the minimum payment should be paid on due date
}cc_acct_t;


//Deposit Rate Structure
typedef struct deposit_rate_struct{
  dec_t checking; //checking account rate
  dec_t one; //one month rate
  dec_t three; //three month rate
  dec_t six; //six month rate
}deposit_rate_t;

//Mortgage Rate Structure
typedef struct mortgage_rate_struct{
  dec_t frm15; //15-year FRM
  dec_t frm15Point; //15-year FRM's fees and points
  dec_t frm30; //30-year FRM
  dec_t frm30Point; //30-year FRM's fees and points
  dec_t arm1;  //1-year ARM
  dec_t arm1Point;  //1-year ARM's fees and points
  dec_t arm1Margin; //1-year ARM's margin
  dec_t arm51; //5/1 ARM
  dec_t arm51Point; //5/1 ARM's fees and points
  dec_t arm51Margin; //5/1 ARM's margin
}mort_rate_t;

typedef struct cc_rate{
  dec_t purchase_apr;
  dec_t cash_adv_apr;
  dec_t bal_tx_apr;
  dec_t default_apr;
  dec_t init_bal_tx_apr;
}cc_rate_t;

typedef struct mortgage_const{
  dec_t hundredThousand;
  dec_t title_search;
  dec_t title_examination;
  dec_t closing;
  dec_t recording;
  dec_t title_insurance_rate_1;
  dec_t title_insurance_rate_2;
  dec_t title_insurance_base;
  dec_t documentary_stamp_tax_on_note_rate_1;
  dec_t documentary_stamp_tax_on_note_rate_2;
  dec_t intagible_tax_on_note;
  dec_t lender_title_insurance_policy;
  dec_t alta_title_endorsements;
  dec_t title_endorsement_extra;
  dec_t title_endorsement_rate;
  dec_t origination;
  dec_t appraisal;
  dec_t credit_report;
  dec_t additional_closing_fees;
  dec_t closing_cost_other;
  dec_t premature_close_penalty_year_0;
  dec_t premature_close_penalty_year_1;
  dec_t premature_close_penalty_year_2;
  dec_t premature_close_penalty_year_3;
  dec_t overdue_surcharge_rate;
  dec_t min_overdue_yr_penalty_rate;
  dec_t min_overdue_yr_penalty_point;
  dec_t min_overdue_yr_penalty_margin;
}mort_const_t;

typedef struct cc_const{
  dec_t purchase_bal_tx_extra;
  dec_t cash_adv_extra;
  dec_t min_cash_adv;
  dec_t min_default;
  dec_t default_extra;
  dec_t min_fin_charge;
  dec_t for_cur_tx_rate;
  dec_t cash_adv_tx_rate;
  dec_t min_cash_adv_tx_fee;
  dec_t bal_tx_rate;
  dec_t min_payment_rate;
  dec_t min_bal_tx_fee;
  dec_t max_bal_tx_fee;
  dec_t over_credit_limit_fee;
  //APR is variable rate and tie to 1-year TCM
  dec_t level1_fee;
  dec_t level2_fee;
  dec_t level3_fee;
  dec_t level1_thr;
  dec_t level2_thr;
}cc_const_t;

#ifndef FILEOPEN
#define FILEOPEN(C, A, B) \
  if ((C = myfopen(A, B))==NULL) { \
  char input_str_macro[STR_MAX_LENGTH];\
  myfclose(C); \
  mysprintf(input_str_macro, "Error: file '%s' not found\n", A);\
  errorPrint(LOC, ERROR_CODE, FILE_NOT_EXIST, stderr, input_str_macro);\
  }
#endif

#endif
