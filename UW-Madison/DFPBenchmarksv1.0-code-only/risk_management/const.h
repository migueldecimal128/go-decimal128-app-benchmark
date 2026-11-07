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

//----------------------------------------
// const.h
// Currency Conversion Microbenchmark
// Decimal Floating-Point Benchmark
// This file is to provide the common constant used in the benchmark,
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.7 $

#ifndef _CONST_H
#define _CONST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <errno.h>

#ifdef WIDTH64
 #define  DECNUMDIGITS 16           // default precision
#else
 #define  DECNUMDIGITS 34           // default precision 
#endif

   #ifdef DECNUMBER_FIXED 
    #ifdef WIDTH64
    #define dec_t decDouble
    #define DEC_LENGTH64  DECIMAL64_Pmax
    #endif //WIDTH64
    #ifdef WIDTH128
     #define dec_t decQuad 
     #define DEC_LENGTH128  DECIMAL128_Pmax
    #endif //WIDTH128
   #endif // DECNUMBER_FIXED

   #ifdef DECNUMBER_ARBITRARY
    #ifdef WIDTH64
     #define dec_t decNumber 
     #define dec_t decNumber
     #define DEC_LENGTH64  DECIMAL64_Pmax
    #endif
    #ifdef WIDTH128
     #define dec_t decNumber 
     #define DEC_LENGTH128  DECIMAL128_Pmax
    #endif //WIDTH128
   #endif // DECNUMBER_ARBITRARY

   #ifdef BID
    #ifdef WIDTH64
     #define dec_t BID_UINT64
     #define DEC_LENGTH64 16 
     #define DECIMAL64_Bytes 8
    #endif
    #ifdef WIDTH128
      #define dec_t BID_UINT128
      #define DEC_LENGTH128 34 
      #define DECIMAL128_Bytes 16
    #endif
   #endif // BID

   #ifdef NATIVE_GCC_DPD 
    #define dec_t _Decimal64 
    #define DEC_LENGTH64  DECIMAL64_Pmax
   #endif // NATIVE_GCC_DPD 
   
   #ifdef NATIVE_GCC_BID 
    #define dec_t _Decimal64 
    #define DEC_LENGTH64 16 
    #define DECIMAL64_Bytes 8
   #endif // NATIVE_GCC_BID 

//The length of the coefficient
//#define DEC_LENGTH32  DECIMAL32_Pmax
//#define DEC_LENGTH64  DECIMAL64_Pmax
//#define DEC_LENGTH128 DECIMAL128_Pmax
//in decNumber lib, if we want to perform decNumberToString, the output
//string needs to have at least 14 characters longer than the number of
//digits in the number (number->digits) Page 24 in the decNum lib doc
#define EXTRA_STR_LENGTH 14
#define LENGTH32  (DEC_LENGTH32 + EXTRA_STR_LENGTH)
#define LENGTH64  (DEC_LENGTH64 + EXTRA_STR_LENGTH)
#define LENGTH128 (DEC_LENGTH128+ EXTRA_STR_LENGTH)

#define STR_MAX_LENGTH 300
#define PATHNAME_MAX_LENGTH 300
//Define the length of max buffer
#define MAX_BUF_SIZE 100
//RETURN ERROR CODE
#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)
#define LOC __FILE__ ":" TOSTR(__LINE__)

#define MSG_LENGTH 100
#define INFO_CODE 0
#define WARNING_CODE 1
#define ERROR_CODE 2
#define INFO_CODE_NO_LINE 1

#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-19 21:51:57 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: const.h,v $
  Revision 1.7  2009-04-19 21:51:57  mjanders
  Updated decnumber and Intel libraries

  Revision 1.6  2009-03-20 20:15:27  mjanders
  Added 128-bit mode

  Revision 1.5  2008-11-25 06:35:21  mjanders
  Not needed

  Revision 1.4  2008/11/23 07:55:15  mjanders
  Got rid of header file includes, they are taken care of in operator.h

  Revision 1.3  2008/09/24 07:35:49  mjanders
  Added native gcc tpes

  Revision 1.2  2008/08/08 07:44:20  mjanders
  Added support for BID

  Revision 1.1.1.1  2008/04/22 17:48:02  mjanders
  Initial import of risk_management from tarball.

  Revision 1.1.1.1  2007/03/13 03:55:38  lwang
  initial check-in for Risk management

  Revision 1.9  2006/11/30 15:01:20  lwang
  Clecn up several bug in mortgage and rate file

  Revision 1.8  2006/11/30 03:25:01  lwang
  fix problem on the size of the acct name and linked list

  Revision 1.7  2006/11/21 03:02:44  lwang
  Major Revision with new technique to access database

  Revision 1.6  2006/11/19 23:23:09  lwang
  First draft of Banking System is done

  Revision 1.5  2006/11/17 02:20:00  lwang
  Credit Card benchmark added

  Revision 1.4  2006/11/07 03:34:01  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.3  2006/11/03 02:18:09  lwang
  major revision

  Revision 1.2  2006/10/31 11:45:22  lwang
  minor update

  Revision 1.1  2006/10/28 01:54:18  lwang
  Banking System Initial Check-in

  Revision 1.4  2006/10/19 02:33:25  lwang
  extended with currency conversion, and checking/CD acct

  Revision 1.3  2006/10/12 12:03:49  lwang
  initial check-in

  Revision 1.2  2006/10/12 05:51:46  lwang
  new version, bug free

  Revision 1.1  2006/09/15 12:52:30  lwang
  initial check-in for the CD microbenchmark

  Revision 1.1  2006/07/31 06:46:23  lwang
  initial check-in for the currency conversion benchmark.

*/
