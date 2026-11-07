//----------------------------------------
// credit_card.h
// Decimal Floating-Point Benchmark
// This header file is to define the common functions for credit card actions
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.2 $

#ifndef CREDIT_CARD_H
#define CREDIT_CARD_H

#include "const.h"
#include "credit_card_const.h"
#include "operator.h"
#include "basic_fcn.h"
#include "decfcn.h"

///////////////////////////////////////////////////////////////////////////////
// Credit Card Public Function
///////////////////////////////////////////////////////////////////////////////
int credit_card_acct_open(htab *cc_table, htab *checking_table,
                          char *acct_no,
                          cc_rate_t *cc_rate, cc_const_t *cc_const,
                          dec64_t credit_limit, int autopay,
                          FILE *file_ptr);
int credit_card_acct_pay(htab *cc_table, char *acct_no, int payment_type,
                         dec64_t payment, FILE *fp);
int credit_card_acct_close(htab *cc_table, char *acct_no, FILE *fp);
int credit_card_acct_inquiry(htab *cc_table, char *acct_no, FILE *fout);
int credit_card_acct_debt(htab *cc_table, char *acct_no, int currency,
                          dec64_t **conversion_rate, FILE *fout,
                          cc_const_t *cc_const,
                          int type, dec64_t fund);
int endOfTheDayCreditCard(htab *cc_table, htab *checking_table,
                          FILE *fout, dec64_t **conversion_rate,
                          deposit_rate_t *deposit_rate,
                          cc_rate_t *cc_rate, cc_const_t *cc_const,
                          int prime_rate_change
#ifdef TEST
			  , FILE *ascii_file, FILE *bin_file
#endif
);
int cc_const_init(cc_const_t *cc_const);
////////////////////////////////////////////////////////////////////////////////
// Other helper functions
///////////////////////////////////////////////////////////////////////////////
int credit_card_acct_init(cc_acct_t *this_acct, char *acct_no,
                          int autopay, dec64_t credit_limit,
                          cc_rate_t *cc_rate);
int credit_card_acct_record(cc_acct_t *this_acct);
int lateFee(cc_acct_t *this_acct, cc_const_t *cc_const, FILE *fout);
int zero_it(int value, cc_acct_t *this_acct);
int doPayment(cc_acct_t *this_acct, dec64_t payment, FILE *fp);
void autoDefault(cc_acct_t *this_acct);
int getCCRate(cc_rate_t *cur_rate, cc_const_t *cc_const, dec64_t prime_rate);
void check_cc_tx_type(char *str, int type);
#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-12 05:53:19 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: credit_card.h,v $
  Revision 1.2  2009-04-12 05:53:19  mjanders
  Added 128-bit mode

  Revision 1.1.1.1  2008-04-18 01:13:49  mjanders


  Revision 1.6  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.5  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.3  2006/11/22 01:40:31  lwang
  comment added

*/
