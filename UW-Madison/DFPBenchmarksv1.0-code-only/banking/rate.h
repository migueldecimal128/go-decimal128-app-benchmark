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
// Id: $Id: rate.h,v 1.5 2009-04-12 05:56:19 mjanders Exp $
// Date: $Date: 2009-04-12 05:56:19 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// rate.h
// Currency Conversion Microbenchmark
// Decimal Floating-Point Benchmark
// header file for rate.c
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.5 $

#ifndef RATE_H
#define RATE_H
#include "operator.h"
#include "const.h"
#include "deposit.h"
#include "deposit_const.h"
#include "mort_const.h"
#include "decfcn.h"

void initCoin(dec_t **conversion_rate);
void openConversionRateFile(FILE **dp);
void updateConversionTable(dec_t **conversion_rate, FILE **dp);
void openDepositRateFile(FILE **dp);
void updateDepositRateTable(deposit_rate_t *cur_rate, FILE **dp);
void closeDepositRateFile(FILE **dp);
void closeConversionRateFile(FILE **dp);
int updateTCMRateFile(dec_t *tcm, FILE *dp);
int updatePrimeRateFile(dec_t *prime, FILE *dp);
void printConversionRateTable(dec_t **conversion_rate, FILE *file_ptr);
void openMortgageRateFile(FILE **dp);
void closeMortgageRateFile(FILE **dp);
int  updateMortgageRate(FILE **dp, mort_rate_t *mort_rate);
#endif //# !RATE_H
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-12 05:56:19 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: rate.h,v $
  Revision 1.5  2009-04-12 05:56:19  mjanders
  Added 128-bit mode

  Revision 1.4  2009-01-06 18:39:12  mjanders
  Added common header

  Revision 1.3  2008/11/25 06:16:58  mjanders
  Finished adding BID

  Revision 1.2  2008/11/23 08:14:17  mjanders
  Moved around the include files to make more sense. All include files for the libraries come in operator.h

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.6  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.4  2006/11/17 02:20:00  lwang
  Credit Card benchmark added

  Revision 1.3  2006/11/07 03:34:02  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.2  2006/11/03 02:20:54  lwang
  minor revision

  Revision 1.1  2006/10/31 11:44:09  lwang
  tables.*->rate.*

  Revision 1.1  2006/10/28 01:54:18  lwang
  Banking System Initial Check-in

  Revision 1.1  2006/10/19 02:33:25  lwang
  extended with currency conversion, and checking/CD acct

  Revision 1.1  2006/07/31 06:46:23  lwang
  initial check-in for the currency conversion benchmark.

*/
