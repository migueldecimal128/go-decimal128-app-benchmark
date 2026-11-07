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
// Id: $Id: mort_const.h,v 1.2 2009-01-06 18:39:12 mjanders Exp $
// Date: $Date: 2009-01-06 18:39:12 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// mort_const.h
// Decimal Floating-Point Benchmark
// This file is to define the common macros and constants for the mortgage accounts
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.2 $

#ifndef MORT_CONST_H
#define MORT_CONST_H
//to remove the first command line
#define LINE_MAX 200 
////////////////////////////////////////////////////
// Mortgage Related
#define ORIGINATION "0.01"
#define APPRAISAL "300.00"
#define CREDIT_REPORT "65"
//Title Insurance Fee is from http://www.michaelbelle.com/calculator.htm
#define LENDER_TITLE_INSURANCE "25"
#define INTANGIBLE_TAX_ON_NOTE_RATE "0.002"
#define ALTA_TITLE_ENDORSEMENTS "50"
#define TITLE_INSURANCE_THRESHOLD "100000"
#define TITLE_INSURANCE_RATE_1 "0.001"
#define TITLE_INSURANCE_RATE_2 "5.00"
#define TITLE_INSURANCE_BASE "575.00"
#define TITLE_ENDORSEMENT_EXTRA "25.00"
#define TITLE_ENDORSEMENT_RATE "0.1"
#define TITLE_SEARCH_FEE "85"
#define TITLE_EXAMINATION_FEE "65"
#define CLOSING_FEE "100"
#define RECORDING_FEE "50"
#define DOCUMENTARY_STAMP_TAX_ON_NOTE_RATE_1 "0.01"
#define DOCUMENTARY_STAMP_TAX_ON_NOTE_RATE_2 "0.35"
//CLOSEING fee is estimated from http://1stmt.com/closingfees/
#define ADDITIONAL_CLOSING_FEES "0.005"
#define CLOSING_COSTS_OTHER "1075.00"
//premature close penalty
#define PREMATURE_CLOSE_PENALTY_YEAR_0 "0.01"
#define PREMATURE_CLOSE_PENALTY_YEAR_1 "0.075"
#define PREMATURE_CLOSE_PENALTY_YEAR_2 "0.005"
#define PREMATURE_CLOSE_PENALTY_YEAR_3 "0.025"
//Overdue penalty (minimum 10%)
#define MIN_OVERDUE_YR_PENALTY_RATE "10"
#define MIN_OVERDUE_YR_PENALTY_POINT "1"
#define MIN_OVERDUE_YR_PENALTY_MARGIN "5"
#define OVERDUE_SURCHARGE_RATE "0.05"
//Mortgage Type
#define FRM15YEAR 3
#define FRM30YEAR 4
#define ARM1YEAR 5
#define ARM51YEAR 6
//Mortgage Action (need to align with deposit)
#define MORT_OPEN 1
#define MORT_INQUIRY 2
#define MORT_REFINANCE 3
#define MORT_CLOSE 4
#define MORT_PAY 5
#define MORT_SCHEDULE_TABLE 6
//Mortgage parameter
#define MAX_CAP "6.0"
#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-01-06 18:39:12 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: mort_const.h,v $
  Revision 1.2  2009-01-06 18:39:12  mjanders
  Added common header

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.7  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.6  2007/03/14 18:09:55  lwang
  temp check-in

  Revision 1.5  2006/11/30 15:01:20  lwang
  Clecn up several bug in mortgage and rate file

  Revision 1.4  2006/11/21 03:02:44  lwang
  Major Revision with new technique to access database

  Revision 1.3  2006/11/07 03:34:01  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.2  2006/11/03 02:20:24  lwang
  initial check-in

*/
