// =============================================================================
//  dfp_includes.h 
//  Contains include directives to various dfp library header files.
// =============================================================================
//  Original Author : Mike Anderson 1/2009
//  Maintainer:       Mike Anderson
// =============================================================================
//  The benchmarks can be run using various DFP solutions. Each choice of
//  DFP_DATATYPE requires that a different set of library header files be
//  included. This file chooses which files are appropriate to be
//  included. 
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
// Id: $Id: dfp_includes.h,v 1.5 2009-04-12 05:56:19 mjanders Exp $
// Date: $Date: 2009-04-12 05:56:19 $
// Author: $Author: mjanders $
// =============================================================================

#ifndef DFP_INC_H
#define DFP_INC_H

#if defined(DECNUMBER_ARBITRARY)
#if defined(WIDTH64)
#include "decimal64.h"
#else
#include "decimal128.h"
#endif
#endif

#if defined(DECNUMBER_FIXED) 
#if defined(WIDTH64)
#include "decDouble.h"
#include "decimal64.h"
#else
#include "decQuad.h"
#include "decimal128.h"
#endif
#endif

#if defined(BID)  
#include <math.h>
#include "bid_conf.h"
#include "bid_functions.h"
#endif

#if defined(NATIVE_GCC_DPD)
#include "decDouble.h"
#include "decimal64.h"
#include "dfp-round.h"
#endif

#if defined(NATIVE_GCC_BID)  
#include <math.h>
#include "bid_conf.h"
#include "bid_functions.h"
#include "dfp-round.h"
#endif

#endif // defined DFP_INC_H
