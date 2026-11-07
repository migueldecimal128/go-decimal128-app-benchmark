#ifndef CONST_H
#define CONST_H

#if defined(GCC) || defined(DEC32) || defined(DEC64) || defined(DEC128)
   #ifdef DECNUMBER_FIXED 
    #define DEC_LENGTH64  DECIMAL64_Pmax
    #define dec_t decDouble
   #endif // DECNUMBER_FIXED
   #ifdef DECNUMBER_ARBITRARY
    #define DEC_LENGTH64  DECIMAL64_Pmax
    #define dec_t decNumber
   #endif // DECNUMBER_ARBITRARY
   #ifdef BID
    #define dec_t BID_UINT64
    #define DEC_LENGTH64  16 
   #endif // BID
   //#define dec_t decNumber
   #include "dfp.h"
   #define EXTRA_STR_LENGTH 14
   #define LENGTH64  (DEC_LENGTH64 + EXTRA_STR_LENGTH)
#else //use binary or Intel's library
   #define dec_t double
#endif // defined(GCC) || defined(DEC32) || defined(DEC64) || defined(DEC128)

#define STR_MAX_LENGTH 300
#define PATHNAME_MAX_LENGTH 300
// How often (days) do we pay interest
#define PAY_DATE 30
//Define the precision
//#define RATE_PRECISION (MFOUR)
#define RATE_PRECISION (EFOUR)
//Define account type
#define CHECKING 0
#define CD 1
#define CASH 2
#define MORTGAGE 3
#define CREDIT_CARD 7
#define TX_TYPE 5
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
#define LOC __FILE__ ":" TOSTR(__LINE__)
#define ACCT_TYPE(X, Y){ \
  if (strlen(Y)>STR_MAX_LENGTH)\
    errorPrint(LOC, ERROR_CODE, ERROR_CODE, stderr, \
               "String length is too short\n");\
  if ((X)==CHECKING)\
    strcpy(Y, "CHECKING");\
  else if ((X)==CD)\
    strcpy(Y, "CD");\
  else if ((X)==FRM15YEAR)\
    strcpy(Y, "FRM 15 YEAR");\
  else if ((X)==FRM30YEAR)\
    strcpy(Y, "FRM 30 YEAR");\
  else if ((X)==ARM1YEAR)\
    strcpy(Y, "ARM 1 YEAR");\
  else if ((X)==ARM51YEAR)\
    strcpy(Y, "ARM 5/1 YEAR");\
  else if ((X)==CREDIT_CARD)\
    strcpy(Y, "CREDIT CARD");\
  else{\
    char _eStr[STR_MAX_LENGTH];\
    sprintf(_eStr, "Unknown Input Acct Type: %d\n", (X));\
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
#define MORT_LENGTH_FAIL -30
#define ACCESS_FILE_FAIL -31
#define SPARC_SETUP_FAIL -32
#define SPARC_SAMPLE_FAIL -33
#define READ_WRITE_SIZE_FAIL -34
#define ADD_ACCT_FAIL -35
#define REACH_MAX_ACCT -36
#define ACCT_INIT_FAIL -37
// For test generator
#define CD_CK_MIN 1
#define CD_CK_RANGE 9
#define MORT_MIN 1 
#define MORT_RANGE 9
#define CC_MIN 1
#define CC_RANGE 9
#define STARTUP_MIN_ACCT 10
#define THRESHOLD_DAY 15
#define THRESHOLD_CLOSE 365

#if defined(DECNUMBER_ARBITRARY) || defined(DECNUMBER_FIXED)
//Command Structure
typedef struct cmd64_struct{
  char acct_no[MAX_ACCT_NAME_LENGTH];
  decimal64 fund;
  decimal64 str1; //can be used for downPayment in mortgage
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
}cmd64_t;
#endif

#if defined(BID)
//Command Structure
typedef struct cmd64_struct{
  char acct_no[MAX_ACCT_NAME_LENGTH];
  BID_UINT64 fund;
  BID_UINT64 str1; //can be used for downPayment in mortgage
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
}cmd64_t;
#endif

#if defined(WIDTH128)
#if defined(DECNUMBER_ARBITRARY) || defined(DECNUMBER_FIXED) 
typedef struct cmd128_struct{
  char acct_no[MAX_ACCT_NAME_LENGTH];
  decimal128 fund;
  decimal128 str1; //can be used for downPayment in mortgage
  int action;
  int length;
  int acct_type;
  int select;
}cmd128_t;
#endif

#if defined(BID)
typedef struct cmd128_struct{
  char acct_no[MAX_ACCT_NAME_LENGTH];
  BID_UINT128 fund;
  BID_UINT128 str1; //can be used for downPayment in mortgage
  int action;
  int length;
  int acct_type;
  int select;
}cmd128_t;
#endif //BID
#endif //WIDTH128

typedef struct cmd_struct{
  char acct_no[MAX_ACCT_NAME_LENGTH];
  dec64_t fund;
  dec64_t str1; //can be used for downPayment in mortgage
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
  dec64_t rate;              //interest rate
  dec64_t interest;          //accumulated interest for the whole period
  dec64_t unposted_interest; //accumulated interest during this period
  dec64_t last_statement;    //the balance in the end of the last maturity
  dec64_t last_deposit;      //the amount the user last deposit
  dec64_t last_withdrawl;    //the amount the user last withdrawl
  dec64_t current_balance;   //the current balance
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
  dec64_t start_rate; //use for ARM
  dec64_t rate;
  dec64_t loan;
  dec64_t monthly;
  dec64_t accu_interest;
  dec64_t remaining_principal;
  dec64_t point;
  dec64_t downPayment;
  dec64_t closingCost;
  dec64_t margin;
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
  dec64_t purchase_statement;
  //The total amount that has been paid after we have the statement
  dec64_t purchase_total_paid;        //The amount that has been paid to "PURCHASE"
  dec64_t last_paid;         //the amount the user last deposit
  dec64_t last_tx;           //the amount of the last transaction
  dec64_t credit_limit;       //the credit limit
  //the upper bound where the system rejects the transaction. It's set to "-credit limit" now
  dec64_t credit_limit_upper;;
  dec64_t credit_allowance;
  //Reward doesn't get implemented now
  //dec64_t points
  //APR
  dec64_t purchase_apr;      //purchase interest rate
  dec64_t cash_adv_apr;  //cash advance interest rate
  dec64_t bal_tx_apr;     //transfer a balance from another card
  //this happens when you don't pay or underpay your monthly payment (purchase)
  dec64_t default_apr;
  //TRANSACTION
  //accumulated interest for the whole period for purchase transaction
  //the interest rate may be default rate or purchase rate
  //NOTE: when paid, always payoff the amount from the lowest interest rate
  dec64_t purchase;           //purchase transaction
  dec64_t posted_interest_purchase;
  dec64_t cash_adv;      //cash advance amount
  dec64_t posted_interest_cash_adv;   
//accumulated interest for the whole period for cash advance and balance transfer
  dec64_t bal_tx;  //expense from the balance transfer
  dec64_t posted_interest_bal_tx;  //balance transfer interest
  //extra fee due to annual fee, cash advance, balance transfer fee, late payment fee, over-the-credit-limit fee,
  //foreign transaction
  dec64_t min_payment;   //the minimum payment should be paid on due date
}cc_acct_t;


//Deposit Rate Structure
typedef struct deposit_rate_struct{
  dec64_t checking; //checking account rate
  dec64_t one; //one month rate
  dec64_t three; //three month rate
  dec64_t six; //six month rate
}deposit_rate_t;

//Mortgage Rate Structure
typedef struct mortgage_rate_struct{
  dec64_t frm15; //15-year FRM
  dec64_t frm15Point; //15-year FRM's fees and points
  dec64_t frm30; //30-year FRM
  dec64_t frm30Point; //30-year FRM's fees and points
  dec64_t arm1;  //1-year ARM
  dec64_t arm1Point;  //1-year ARM's fees and points
  dec64_t arm1Margin; //1-year ARM's margin
  dec64_t arm51; //5/1 ARM
  dec64_t arm51Point; //5/1 ARM's fees and points
  dec64_t arm51Margin; //5/1 ARM's margin
}mort_rate_t;

typedef struct cc_rate{
  dec64_t purchase_apr;
  dec64_t cash_adv_apr;
  dec64_t bal_tx_apr;
  dec64_t default_apr;
  dec64_t init_bal_tx_apr;
}cc_rate_t;

typedef struct mortgage_const{
  dec64_t hundredThousand;
  dec64_t title_search;
  dec64_t title_examination;
  dec64_t closing;
  dec64_t recording;
  dec64_t title_insurance_rate_1;
  dec64_t title_insurance_rate_2;
  dec64_t title_insurance_base;
  dec64_t documentary_stamp_tax_on_note_rate_1;
  dec64_t documentary_stamp_tax_on_note_rate_2;
  dec64_t intagible_tax_on_note;
  dec64_t lender_title_insurance_policy;
  dec64_t alta_title_endorsements;
  dec64_t title_endorsement_extra;
  dec64_t title_endorsement_rate;
  dec64_t origination;
  dec64_t appraisal;
  dec64_t credit_report;
  dec64_t additional_closing_fees;
  dec64_t closing_cost_other;
  dec64_t premature_close_penalty_year_0;
  dec64_t premature_close_penalty_year_1;
  dec64_t premature_close_penalty_year_2;
  dec64_t premature_close_penalty_year_3;
  dec64_t overdue_surcharge_rate;
  dec64_t min_overdue_yr_penalty_rate;
  dec64_t min_overdue_yr_penalty_point;
  dec64_t min_overdue_yr_penalty_margin;
}mort_const_t;

typedef struct cc_const{
  dec64_t purchase_bal_tx_extra;
  dec64_t cash_adv_extra;
  dec64_t min_cash_adv;
  dec64_t min_default;
  dec64_t default_extra;
  dec64_t min_fin_charge;
  dec64_t for_cur_tx_rate;
  dec64_t cash_adv_tx_rate;
  dec64_t min_cash_adv_tx_fee;
  dec64_t bal_tx_rate;
  dec64_t min_payment_rate;
  dec64_t min_bal_tx_fee;
  dec64_t max_bal_tx_fee;
  dec64_t over_credit_limit_fee;
  //APR is variable rate and tie to 1-year TCM
  dec64_t level1_fee;
  dec64_t level2_fee;
  dec64_t level3_fee;
  dec64_t level1_thr;
  dec64_t level2_thr;
}cc_const_t;

#ifndef FILEOPEN
#define FILEOPEN(C, A, B) \
  if ((C = fopen(A, B))==NULL) { \
  char input_str_macro[STR_MAX_LENGTH];\
  fclose(C); \
  sprintf(input_str_macro, "Error: file '%s' not found\n", A);\
  errorPrint(LOC, ERROR_CODE, FILE_NOT_EXIST, stderr, input_str_macro);\
  }
#endif

#endif
