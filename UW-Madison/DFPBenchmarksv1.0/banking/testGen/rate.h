//----------------------------------------
// rate.h
// Currency Conversion Microbenchmark
// Decimal Floating-Point Benchmark
// header file for rate.c
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.1.1.1 $

#ifndef RATE_H
#define RATE_H
#include "const.h"
#include "deposit.h"
#include "deposit_const.h"
#include "mort_const.h"
#include "operator.h"

void initCoin(dec64_t **conversion_rate);
void openConversionRateFile(FILE **dp);
void updateConversionTable(dec64_t **conversion_rate, FILE **dp);
void openDepositRateFile(FILE **dp);
void updateDepositRateTable(deposit_rate_t *cur_rate, FILE **dp);
void closeDepositRateFile(FILE **dp);
void closeConversionRateFile(FILE **dp);
int updateTCMRateFile(dec64_t *tcm, FILE *dp);
int updatePrimeRateFile(dec64_t *prime, FILE *dp);
void printConversionRateTable(dec64_t **conversion_rate, FILE *file_ptr);
void openMortgageRateFile(FILE **dp);
void closeMortgageRateFile(FILE **dp);
int  updateMortgageRate(FILE **dp, mort_rate_t *mort_rate);
#endif //# !RATE_H
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2008-04-18 01:13:49 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: rate.h,v $
  Revision 1.1.1.1  2008-04-18 01:13:49  mjanders


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
