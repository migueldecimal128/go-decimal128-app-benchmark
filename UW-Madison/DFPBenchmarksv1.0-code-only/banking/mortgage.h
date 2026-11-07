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
// Id: $Id: mortgage.h,v 1.3 2009-04-12 05:56:19 mjanders Exp $
// Date: $Date: 2009-04-12 05:56:19 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// mortgage.h
// Decimal Floating-Point Benchmark
// This file is to privde the header files for mortgage.c
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.3 $

#ifndef MORTGAGE_H
#define MORTGAGE_H

#include "deposit.h"
#include "const.h"
#include "mort_const.h"
#include "operator.h"
#include "basic_fcn.h"
#include "rate.h"
#include "mort_const.h"
#include <string.h>

/////////////////////////////////////////////////////////////////////////////
// Mortgage Public Functions
/////////////////////////////////////////////////////////////////////////////
int mortgage_acct_open(htab *mortgage_table, htab *checking_table,
                       mort_rate_t *mort_rate, int period,
                       dec_t loan, dec_t downPayment,
                       char *acct_no, int acct_type, int auto_pay,
                       FILE *fout);
int mortgage_acct_close(htab *mortgage_table, char *acct_no, FILE *fout);
int mortgage_acct_inquiry(htab *mortgage_table, char *acct_no, FILE *fout);
int mortgage_acct_pay(htab *mortgage_table, char *acct_no, FILE *fout);
int endOfTheDayMortgage(htab *mortgage_table, htab *checking_table,
                        FILE *fout, dec_t **conversion_rate,
                        deposit_rate_t *deposit_rate,
                        mort_rate_t *mort_rate,
                        dec_t tcm_rate);
int mortgage_acct_refinance(htab *mortgage_table, htab *checking_table,
                            mort_rate_t *mort_rate, int period,
                            char *acct_no, int acct_type, int auto_pay,
                            FILE *fout);
int scheduleTable(FILE *fout, dec_t loan, int year, 
		  mort_rate_t *mort_rate, int acct_type);

/////////////////////////////////////////////////////////////////////////////
// Mortgage Related Actions
/////////////////////////////////////////////////////////////////////////////
int remove_mortgage_acct(mort_acct_t *this_acct, FILE *fout);
int mortgage_acct_init(mort_acct_t *this_acct, int period,
		       dec_t loan, dec_t downPayment, int auto_pay,
		       char *acct_no, FILE *fout);
/////////////////////////////////////////////////////////////////////////////
// Other Operations
/////////////////////////////////////////////////////////////////////////////
int getMortgageRate(mort_acct_t *this_acct, mort_rate_t *mort_rate);
int closing_cost_fcn(dec_t *title_insurance_fee, dec_t principal, 
		     dec_t downPayment);
int closingCost(mort_acct_t *this_acct);
int closePenalty(dec_t* penalty, mort_acct_t *this_acct);
int termPayment(dec_t *monthly, int totalTerm, dec_t rate, dec_t loan);
int termComputation(dec_t rate, dec_t loan, dec_t monthly);
int remainingPrincipal_fcn(mort_acct_t *this_acct);
int determineNewRate(mort_acct_t *this_acct, dec_t tcm_rate);
int interestPaid(dec_t *current_interest, mort_acct_t *this_acct);
int penaltyOverdue(mort_acct_t *this_acct, mort_rate_t *mort_rate);
#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-12 05:56:19 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: mortgage.h,v $
  Revision 1.3  2009-04-12 05:56:19  mjanders
  Added 128-bit mode

  Revision 1.2  2009-01-06 18:39:12  mjanders
  Added common header

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.9  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.8  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.6  2006/11/21 03:02:44  lwang
  Major Revision with new technique to access database

  Revision 1.5  2006/11/17 02:20:00  lwang
  Credit Card benchmark added

  Revision 1.4  2006/11/07 03:34:01  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.3  2006/11/03 02:20:25  lwang
  initial check-in

*/
