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

/*
 * Filename: dfp_includes.h
 * Purpose: This file decides which header files to
 * include for the decimal floating point libraries
 */

#ifndef DFP_INC_H
#define DFP_INC_H

#if defined(DECNUMBER_ARBITRARY)
#if defined(WIDTH64)
#include "decNumber.h"
#include "decimal64.h"
#include "decfcn.h"
#endif
#if defined(WIDTH128)
#include "decNumber.h"
#include "decimal128.h"
#include "decfcn.h"
#endif
#endif

#if defined(DECNUMBER_FIXED) 
#include "decNumber.h"
#if defined(WIDTH64)
#include "decDouble.h"
#include "decimal64.h"
#include "decfcn.h"
#endif
#if defined(WIDTH128)
#include "decQuad.h"
#include "decimal128.h"
#include "decfcn.h"
#endif
#endif

#if defined(BID)  
#include "bid_conf.h"
#include "bid_functions.h"
#include "decfcn.h"
#endif

#if defined(NATIVE_GCC_DPD)
#include "decNumber.h"
#include "decDouble.h"
#include "decimal64.h"
#include "dfp-round.h"
#include "decfcn.h"
#endif

#if defined(NATIVE_GCC_BID)  
#include "bid_conf.h"
#include "bid_functions.h"
#include "dfp-round.h"
#include "decfcn.h"
#endif

#endif // defined DFP_INC_H
