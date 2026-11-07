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

#ifndef DFP_H
#define DFP_H


#ifdef DECNUMBER_FIXED 
 #ifdef WIDTH64
  #define dec_t decDouble
 #endif
 #ifdef WIDTH128
  #define dec_t decQuad
 #endif
 #define context_t decContext
#endif

#ifdef DECNUMBER_ARBITRARY
 #define dec_t decNumber
 #define context_t decContext
 #ifdef WIDTH64
  #define DECNUMDIGITS 16
 #endif
 #ifdef WIDTH128
  #define DECNUMDIGITS 34
 #endif
#endif

#ifdef BID
 #ifdef WIDTH64
  #define dec_t BID_UINT64 
 #endif
 #ifdef WIDTH128
  #define dec_t BID_UINT128 
 #endif
 #define context_t _IDEC_round
#endif

#ifdef NATIVE_GCC_BID 
 #define dec_t _Decimal64
 #define context_t _IDEC_round
#endif

#ifdef NATIVE_GCC_DPD 
 #include "decimal64.h"
 #define dec_t _Decimal64
 #define context_t decContext
#endif

#ifdef HW
typedef union {
  double a;
  decimal64 b;
} dec64_t;
//end of HW
#else
#define dec64_t dec_t
#endif //!HW

#ifdef BID
#else
#endif // BID
#endif //DFP_H
