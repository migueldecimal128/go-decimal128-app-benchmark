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

/*
#include <time.h>
// ----- Non-ANSI timer things; may need altering -------------------
#ifndef TOPTIMEB
  #include <sys/timeb.h>           // timeb.h is in \include\sys
#else
  #include <timeb.h>               // timeb.h is in \include
#endif
*/
   
 #define  INNUMLEN      8           // input number length in bytes
 #define  MAXFILE     255           // maximum filename length
#ifndef DECNUMDIGITS
 #ifdef WIDTH64
  #define  DECNUMDIGITS 16           // default precision
 #else
  #define  DECNUMDIGITS 34           // default precision 
 #endif
#endif
  
   #ifdef DECNUMBER_ARBITRARY
    #ifdef WIDTH64
     #define dec_t decNumber 
     #define DEC_LENGTH64  DECIMAL64_Pmax
    #endif
    #ifdef WIDTH128
     #define dec_t decNumber 
     #define DEC_LENGTH128  DECIMAL128_Pmax
    #endif
   #endif // DECNUMBER_ARBITRARY
 
   #ifdef DECNUMBER_FIXED 
    #ifdef WIDTH64
     #define dec_t decDouble
     #define DEC_LENGTH64  DECIMAL64_Pmax
    #endif
    #ifdef WIDTH128
     #define dec_t decQuad 
     #define DEC_LENGTH128  DECIMAL128_Pmax
    #endif
   #endif // DECNUMBER_FIXED
  
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
#define LENGTH64  (DEC_LENGTH64 + EXTRA_STR_LENGTH)

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
  $Date: 2009-04-19 21:28:44 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: const.h,v $
  Revision 1.7  2009-04-19 21:28:44  mjanders
  Updated decnumber and Intel libraries

  Revision 1.6  2009-03-16 01:13:09  mjanders
  Added 128-bit modes

  Revision 1.5  2009-01-03 00:32:45  mjanders
  Moved around includes

  Revision 1.4  2009/01/03 00:21:34  mjanders
  Added native_gcc_bid round modes

  Revision 1.3  2008/09/24 07:32:51  mjanders
  Added native gcc types

  Revision 1.2  2008/05/19 20:00:43  mjanders
  Added BID functionality

  Revision 1.1.1.1  2008/04/15 23:33:39  stsen
  initial benchmark structure


  Revision 1.2  2007/07/29 17:05:29  lwang
  update version with decDouble support

  Revision 1.1.1.1  2007/03/13 03:49:13  lwang
  Euro currency conversion, simplified version

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
