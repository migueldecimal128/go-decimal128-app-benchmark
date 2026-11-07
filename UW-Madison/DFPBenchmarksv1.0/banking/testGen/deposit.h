#ifndef UNIQUE_H
#define UNIQUE_H

#include "standard.h"
#include "hashtab.h"
#include "const.h"
#include "deposit_const.h"
#include "operator.h"
#include "dfp.h"
#include "basic_fcn.h"
#define LINELEN 4096
//Public function
int deposit_acct_open(htab *t, char *acct_no, dec64_t fund, int period,
		      int acct_type, int currency, int pay_date, 
		      FILE *fout, deposit_rate_t *deposit_rate);
int deposit_acct_close(htab *t, char *acct_name, dec64_t **conversion_rate,
                       int acct_type, FILE *fout, 
		       deposit_rate_t *deposit_rate);
int deposit_mark_extend(htab *t, char *acct_name, FILE *fout);
int deposit_mark_close(htab *t, char *acct_no, FILE *fout);
int deposit_acct_inquiry(htab *t, char *acct_no, FILE *fout);
int money_deposit(htab *t, char *acct_no, dec64_t fund,
                  dec64_t **conversion_rate, int deposit_unit, FILE *fout);
int money_withdrawl(htab *t, char *acct_no, int withdrawl_unit,
                    dec64_t money, dec64_t **conversion_rate, FILE* fout,
                    deposit_rate_t *deposit_rate);
void endOfTheDayDeposit(htab *cd_table, htab *checking_table,
                        FILE *fout, deposit_rate_t *deposit_rate);
int wireTransfer(htab *t, int fromUnit, char *acct_no,
                 int to, dec64_t in, dec64_t *out, dec64_t *transaction,
                 dec64_t *processFee, dec64_t **conversion_rate, FILE *fout);
//Private function
int deposit_acct_init(deposit_acct_t *new_acct, char* acct_no, dec64_t fund,
                      int period, int acct_type, int currency, int pay_date);
int conversionUS(int from, int to,
                 dec64_t moneyFrom, dec64_t *moneyTo,
                 dec64_t *transaction, dec64_t **conversion_rate);
void fixedExtraFee(dec64_t *out, dec64_t in, dec64_t **conversion_rate,
                   int from);
void penaltyComputation(dec64_t *penalty, dec64_t **conversion_rate,
                        deposit_acct_t *this_acct,
                        deposit_rate_t *deposit_rate);
void deposit_payInterest(deposit_acct_t *this_acct, FILE *fout);
void get_deposit_rate(deposit_rate_t *deposit_rate, deposit_acct_t *this_acct);
#endif
