// =============================================================================
//  dfp_datatypes.h 
//  Defines the types context_t and dec64_t. 
// =============================================================================
//  Original Author : Liang-Kai Wang, 5/2007
//  Maintainer:       Mike Anderson 1/2009
// =============================================================================
// Filename: dfp_datatypes.h
// The purpose of this file is to define the two types
// context_t and dec64_t. 
//
// context_t is whatever local information (flags, round modes) 
// that the dfp library requires to be sent in the operator function calls.
//
// dec64_t is a 64-bit representation of the dfp type. This is
// only a special case for decNumber, since all other types are already 
// 64 bits. 
//
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
// Id: $Id: dfp_datatypes.h,v 1.8 2009-04-12 05:56:19 mjanders Exp $
// Date: $Date: 2009-04-12 05:56:19 $
// Author: $Author: mjanders $
// =============================================================================

#ifndef DFP_H
#define DFP_H

#ifdef HW
typedef union {
  double a;
  decimal64 b;
} dec64_t;
//end of HW
#endif //!HW

#ifdef BID
#define context_t _IDEC_round
#endif

#ifdef DECNUMBER_FIXED 
#define context_t decContext
#endif

#ifdef DECNUMBER_ARBITRARY
#define context_t decContext
#endif

#ifdef NATIVE_GCC_BID 
#define context_t _IDEC_round
#endif

#ifdef NATIVE_GCC_DPD 
#define context_t decContext
#endif

#endif //DFP_H
