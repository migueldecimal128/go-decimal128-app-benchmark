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
// Id: $Id: deposit_const.h,v 1.2 2009-01-06 18:39:12 mjanders Exp $
// Date: $Date: 2009-01-06 18:39:12 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// deposit_const.h
// Decimal Floating-Point Benchmark
// This file is to define the common macros and constants for the deposit accounts
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.2 $

#ifndef DEPOSIT_CONST_H
#define DEPOSIT_CONST_H
//#define EURO_INT_DIGITS "-3.00"
#define NON_CUSTOMER_FIXED "0.02"
//Max length in the filename
#define MAXFILE 256
//Max length in the country code
#define MAX_COUNTRY_CODE 10
//this is to check if there is a conversion error
#define MAX_ERROR "-10.0"
#define PENALTY_RATE "0.02" //2% of penalty if withdrawl premature
#define PENALTY_SURCHARGE "30" //$30 of penalty if withdrawl premature
#define GRACE_PERIOD 10
// The "action" identifier
#define NEXT_DAY 0
#define CD_CK_OPEN 1
#define CD_CK_INQUIRY 2
#define CD_SET_CLOSE 3
#define CD_CK_CLOSE 4
#define CD_CK_DEPOSIT 5
#define CD_CK_WITHDRAWL 6
#define CD_EXTEND 7
#define WIRETRANSFER 8
//Define the minor coin available in the bank
//Note: only show the frequently used coin. Rarely coin is removed
//Australia dollar
#define AUD "0.05"
//Chinese yuan
#define CNY "0.1"
//European euro
#define EUR "0.01"
//U.K. Sterling
#define GBP "0.01"
//Hong Kong dollar
#define HKD "0.1"
//Japan yen
#define JPY "1"
//New Zealand dollar
#define NZD "0.1"
//Singapore dollar
#define SGD "0.05"
//New Taiwan dollar
#define TWD "1"
//US dollar
#define USD "0.01"


#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-01-06 18:39:12 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: deposit_const.h,v $
  Revision 1.2  2009-01-06 18:39:12  mjanders
  Added common header

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.8  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.7  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.5  2006/11/21 03:02:44  lwang
  Major Revision with new technique to access database

  Revision 1.4  2006/11/07 08:26:02  lwang
  After spliting CD and Checking database

  Revision 1.3  2006/11/07 05:20:10  lwang
  Name of the functions changed. Before spliting CD and CK

  Revision 1.2  2006/11/03 02:19:49  lwang
  initial check-in

*/
