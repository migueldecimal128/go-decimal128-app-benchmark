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
// Id: $Id: credit_card.h,v 1.5 2009-04-12 05:57:12 mjanders Exp $
// Date: $Date: 2009-04-12 05:57:12 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// credit_card.h
// Decimal Floating-Point Benchmark
// This header file is to define the common functions for credit card actions
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.5 $

#ifndef CREDIT_CARD_H
#define CREDIT_CARD_H

#include "operator.h"
#include "const.h"
#include "credit_card_const.h"
#include "basic_fcn.h"
#include "deposit.h"
#include "decfcn.h"

///////////////////////////////////////////////////////////////////////////////
// Credit Card Public Function
///////////////////////////////////////////////////////////////////////////////
int credit_card_acct_open(htab *cc_table, htab *checking_table,
                          char *acct_no,
                          cc_rate_t *cc_rate, cc_const_t *cc_const,
                          dec_t credit_limit, int autopay,
                          FILE *fout);
int credit_card_acct_pay(htab *cc_table, char *acct_no, int payment_type,
                         dec_t payment, FILE *fout);
int credit_card_acct_close(htab *cc_table, char *acct_no, FILE *fout);
int credit_card_acct_inquiry(htab *cc_table, char *acct_no, FILE *fout);
int credit_card_acct_debt(htab *cc_table, char *acct_no, int currency,
                          dec_t **conversion_rate, FILE *fout,
                          cc_const_t *cc_const,
                          int type, dec_t fund);
int endOfTheDayCreditCard(htab *cc_table, htab *checking_table,
                          FILE *fout, dec_t **conversion_rate,
                          deposit_rate_t *deposit_rate,
                          cc_rate_t *cc_rate, cc_const_t *cc_const,
                          int prime_rate_change);
int cc_const_init(cc_const_t *cc_const);
////////////////////////////////////////////////////////////////////////////////
// Other helper functions
///////////////////////////////////////////////////////////////////////////////
int credit_card_acct_init(cc_acct_t *this_acct, char *acct_no,
                          int autopay, dec_t credit_limit,
                          cc_rate_t *cc_rate);
int credit_card_acct_record(cc_acct_t *this_acct);
int lateFee(cc_acct_t *this_acct, cc_const_t *cc_const, FILE *fout);
int zero_it(int value, cc_acct_t *this_acct);
int doPayment(cc_acct_t *this_acct, dec_t payment, FILE *fout);
void autoDefault(cc_acct_t *this_acct);
int getCCRate(cc_rate_t *cur_rate, cc_const_t *cc_const, dec_t prime_rate);
void check_cc_tx_type(char *str, int type);
#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-12 05:57:12 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: credit_card.h,v $
  Revision 1.5  2009-04-12 05:57:12  mjanders
  Added 128-bit mode

  Revision 1.4  2009-01-06 18:39:12  mjanders
  Added common header

  Revision 1.3  2008/11/25 06:19:40  mjanders
  Moved around includes

  Revision 1.2  2008/11/23 08:14:17  mjanders
  Moved around the include files to make more sense. All include files for the libraries come in operator.h

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.6  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.5  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.3  2006/11/22 01:40:31  lwang
  comment added

*/
