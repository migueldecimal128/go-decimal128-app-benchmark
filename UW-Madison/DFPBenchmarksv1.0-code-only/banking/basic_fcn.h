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
// Id: $Id: basic_fcn.h,v 1.4 2009-04-12 05:57:11 mjanders Exp $
// Date: $Date: 2009-04-12 05:57:11 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// basic_fcn.h
// Decimal Floating-Point Benchmark
// This file is to define the common functions used throughout the benchmark.
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.4 $

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

void errorPrint(const char *location, int level, int errorCode, FILE *fp, 
		const char *msg);
void checkName(char* name, int intValue);
void roundToAvailableCoin(dec_t *c, dec_t a, dec_t b);
//void specialRescale(dec_t *c, dec_t a, dec_t b);
void constdef(dec_t **conversion_rate, cc_const_t *cc_const);
int checkGEZero(dec_t in);
int checkLEOne(dec_t in);
int parse_cmd(FILE* file_ptr, cmd_t *this_cmd);

#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-12 05:57:11 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: basic_fcn.h,v $
  Revision 1.4  2009-04-12 05:57:11  mjanders
  Added 128-bit mode

  Revision 1.3  2009-01-06 18:39:12  mjanders
  Added common header

  Revision 1.2  2008/11/23 08:14:17  mjanders
  Moved around the include files to make more sense. All include files for the libraries come in operator.h

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


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
