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
// Id: $Id: credit_card_const.h,v 1.2 2009-01-06 18:39:12 mjanders Exp $
// Date: $Date: 2009-01-06 18:39:12 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// credit_card_const.h
// Currency Conversion Microbenchmark
// Decimal Floating-Point Benchmark
// This file is to provide the common constants used in the credit card operations
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.2 $


#ifndef CREDIT_CARD_CONST_H
#define CREDIT_CARD_CONST_H

//constants
#define PURCHASE 0
#define CASH_ADVANCE 1
#define BALANCE_TRANSFER 2
//ACTION
#define CC_OPEN 1
#define CC_INQUIRY 2
#define CC_CLOSE 3
#define CC_PAY 4
#define CC_DEBT 5
//PAYMENT TYPE
#define FULL_PAY 1
#define STATEMENT_PAY 2
#define MIN_PAY 3
//Purchase and Balance Transfer Rate Extra
#define PURCHASE_BAL_TX_EXTRA "5.49"
//Cash Advance Rate Extra
#define CASH_ADV_EXTRA "14.99"
//Minimum Cash Advance Rate
#define MIN_CASH_ADV "19.99"
//Default Rate Extra
#define DEFAULT_EXTRA "23.99"
//Max Default Rate
#define MIN_DEFAULT "28.99"
//Initial Balance Transfer Rate
#define INIT_BAL_TX_RATE "0.00"
//Initial Balance Transfer Rate Period : 360 days
#define INIT_BAL_TX_PERIOD 360
//Grace Period
#define CC_GRACE_PERIOD 20
//Minimum finance charge
#define MIN_FIN_CHARGE "0.5"
//Transaction rate for foreign currency: 3%
#define FOR_CUR_TX_RATE "3.00"
//Cash advance: 3% OR $5 MINIMUM
#define CASH_ADV_TX_RATE "3.00"
#define MIN_CASH_ADV_TX_FEE "5.00"
//Minimum Payment Rate
#define MIN_PAYMENT_RATE "10.00"
//Balance Transfer: 3%, $5 minimum, $250 maximum
#define BAL_TX_RATE "3.00"
#define MIN_BAL_TX_FEE "5.00"
#define MAX_BAL_TX_FEE "250.00"
//Late Fee
#define LATE_FEE_LEVEL_1 "15.00"
#define LATE_FEE_LEVEL_1_THR "100.00"
#define LATE_FEE_LEVEL_2 "29.00"
#define LATE_FEE_LEVEL_2_THR "250.00"
#define LATE_FEE_LEVEL_3 "39.00"
//Over the credit limit fee
#define OVER_CREDIT_LIMIT_FEE "39.00"
#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-01-06 18:39:12 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: credit_card_const.h,v $
  Revision 1.2  2009-01-06 18:39:12  mjanders
  Added common header

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.4  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.3  2006/11/22 01:40:31  lwang
  comment added

*/
