//----------------------------------------
// basic_fcn.h
// Decimal Floating-Point Benchmark
// This file is to define the common functions used throughout the benchmark.
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.1.1.1 $

#ifndef BASIC_FCN_H
#define BASIC_FCN_H

#include <stdio.h>
#include <stdlib.h>
#include "operator.h"
#include "const.h"
#include "rate.h"
#include "deposit.h"
#include "deposit_const.h"
#include "credit_card.h"
void value_gen(char *name, int minimum, int range);
int naming(char *name, int max_acct_num, int acct_in_use, int acct_type);
void errorPrint(const char *location, int level, int errorCode, FILE *fp, 
		const char *msg);
void checkName(char* name, int intValue);
void roundToAvailableCoin(dec64_t *c, dec64_t a, dec64_t b);
void specialRescale(dec64_t *c, dec64_t a, dec64_t b);
void constdef(dec64_t **conversion_rate, cc_const_t *cc_const);
int checkGEZero(dec64_t in);
int checkLEOne(dec64_t in);
int parse_cmd(FILE* file_ptr, cmd_t *this_cmd);

#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2008-04-18 01:13:49 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: basic_fcn.h,v $
  Revision 1.1.1.1  2008-04-18 01:13:49  mjanders


  Revision 1.9  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.8  2007/03/14 18:09:55  lwang
  temp check-in

  Revision 1.7  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.5  2006/11/21 03:02:44  lwang
  Major Revision with new technique to access database

  Revision 1.4  2006/11/17 02:20:00  lwang
  Credit Card benchmark added

  Revision 1.3  2006/11/07 03:34:01  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.2  2006/11/03 02:17:46  lwang
  Initial check-in

*/
