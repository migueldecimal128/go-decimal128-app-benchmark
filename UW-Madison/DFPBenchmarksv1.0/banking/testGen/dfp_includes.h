/*
 * Filename: dfp_includes.h
 * Purpose: This file decides which header files to
 * include for the decimal floating point libraries
 */

#ifndef DFP_INC_H
#define DFP_INC_H

#if defined(WIDTH64) 
#define DECNUMDIGITS 16
#else
#define DECNUMDIGITS 34
#endif


#if defined(DECNUMBER_ARBITRARY)
#if defined(WIDTH64)
#include "decimal64.h"
#else
#include "decimal64.h"
#include "decimal128.h"
#endif
#endif

#if defined(DECNUMBER_FIXED) 
#if defined(WIDTH64)
#include "decDouble.h"
#include "decimal64.h"
#else
#include "decimal64.h"
#include "decDouble.h"
#include "decimal128.h"
#endif
#endif

#if defined(BID)  
#include <math.h>
#include "bid_conf.h"
#include "bid_functions.h"
#if defined(WIDTH64)
#include "decimal64.h"
#else
#include "decimal128.h"
#endif
#endif

#if defined(NATIVE_GCC_DPD)
#include "decDouble.h"
#include "decimal64.h"
#endif

#if defined(NATIVE_GCC_BID)  
#include <math.h>
#include "bid_conf.h"
#include "bid_functions.h"
#endif

#endif // defined DFP_INC_H
