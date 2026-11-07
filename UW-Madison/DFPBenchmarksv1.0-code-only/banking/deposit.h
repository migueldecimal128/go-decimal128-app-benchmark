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
// Id: $Id: deposit.h,v 1.4 2009-04-12 05:56:19 mjanders Exp $
// Date: $Date: 2009-04-12 05:56:19 $
// Author: $Author: mjanders $
// =============================================================================

#ifndef UNIQUE_H
#define UNIQUE_H

#include "standard.h"
#include "hashtab.h"
#include "operator.h"
#include "const.h"
#include "deposit_const.h"
#include "dfp_datatypes.h"
#include "basic_fcn.h"
#define LINELEN 4096
//Public function
int deposit_acct_open(htab *t, char *acct_no, dec_t fund, int period,
		      int acct_type, int currency, int pay_date, 
		      FILE *fout, deposit_rate_t *deposit_rate
#ifdef NEW_FEATURES
		      , dec_t **conversion_rate
#endif
);
int deposit_acct_close(htab *t, char *acct_name, dec_t **conversion_rate,
                       int acct_type, FILE *fout, 
		       deposit_rate_t *deposit_rate);
int deposit_mark_extend(htab *t, char *acct_name, FILE *fout);
int deposit_mark_close(htab *t, char *acct_no, FILE *fout);
int deposit_acct_inquiry(htab *t, char *acct_no, FILE *fout);
int money_deposit(htab *t, char *acct_no, dec_t fund,
                  dec_t **conversion_rate, int deposit_unit, FILE *fout);
int money_withdrawl(htab *t, char *acct_no, int withdrawl_unit,
                    dec_t money, dec_t **conversion_rate, FILE* fout,
                    deposit_rate_t *deposit_rate);
void endOfTheDayDeposit(htab *cd_table, htab *checking_table,
                        FILE *fout, deposit_rate_t *deposit_rate
#ifdef NEW_FEATURES
			, dec_t **conversion_rate
#endif
);
int wireTransfer(htab *t, int fromUnit, char *acct_no,
                 int to, dec_t in, dec_t *out, dec_t *transaction,
                 dec_t *processFee, dec_t **conversion_rate, FILE *fout);
//Private function
int deposit_acct_init(deposit_acct_t *new_acct, char* acct_no, dec_t fund,
                      int period, int acct_type, int currency, int pay_date);
int conversionUS(int from, int to,
                 dec_t moneyFrom, dec_t *moneyTo,
                 dec_t *transaction, dec_t **conversion_rate);
void fixedExtraFee(dec_t *out, dec_t in, dec_t **conversion_rate,
                   int from);
void penaltyComputation(dec_t *penalty, dec_t **conversion_rate,
                        deposit_acct_t *this_acct,
                        deposit_rate_t *deposit_rate);
void deposit_payInterest(deposit_acct_t *this_acct, FILE *fout
#ifdef NEW_FEATURES
			 , dec_t **conversion_rate
#endif
);
void get_deposit_rate(deposit_rate_t *deposit_rate, deposit_acct_t *this_acct);
#endif
