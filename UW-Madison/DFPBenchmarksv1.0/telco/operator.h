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
// operator.h
// Decimal Floating-Point Benchmark
// This file is to define the names for all the operations in decNumber
// and other libraries.
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.15 $

#ifndef _OPERATOR_H
#define _OPERATOR_H
#ifdef COUNTING
extern int mul_total, div_total, add_total, add_minmax_total, add_other_total;
extern int mul_rounded, div_rounded, add_rounded, add_other_rounded;
//the below one can run on fast pass adder
extern int add_no_exp_diff; 
extern int add_minmax_no_exp_diff; 
extern int add_other_no_exp_diff; 
//this one may run on fast pass
extern int add_no_right_shifted_diff_exp;
extern int add_minmax_no_right_shifted_diff_exp;
extern int add_other_no_right_shifted_diff_exp;
//the below can run on fast pass mul
extern int mul_lt_eq_16_coeff_on_result;
//length of multiplier does matter because mul can be sequential base
extern int sig_length_multiplier[16];
#include "decfcn.h"
#endif

#if defined (DECNUMBER_ARBITRARY)

#if defined (WIDTH64)
//macros for operations of decNumber library
//First define Rounding API modes
#define RPI DEC_ROUND_CEILING
#define RNI DEC_ROUND_FLOOR
#define RTZ DEC_ROUND_DOWN
#define RNE DEC_ROUND_HALF_EVEN
#define RNA DEC_ROUND_HALF_UP

#define SET_ROUNDMODE(C, ROUNDMODE) C.round = ROUNDMODE     
#define CONTEXTDEFAULT(X, Y) decContextDefault(&X, DEC_INIT_DECIMAL64)
#define ABS(Z, X, C)     decNumberAbs(Z, X, C)
#ifdef COUNTING
#define ADD(Z, X, Y, C) {\
 decimal64 x64, y64;\
 decimal64FromNumber(&x64, X, C);\
 decimal64FromNumber(&y64, Y, C);\
 add_check(&x64, &y64, X, Y);\
 add_total++;\
 (C)->status = (C)->status & (~DEC_Rounded);\
 decNumberAdd(Z, X, Y, C);\
 if ((C)->status & DEC_Rounded)\
   add_rounded++;\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define ADD(Z, X, Y, C)      decNumberAdd(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#define COMPARE(Z, X, Y, C)     decNumberCompare(Z, X, Y, C)
#define EQ(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = ISZERO(&tmp);\
}
#define GT(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = ISPOSITIVE(&tmp);\
}
#define LT(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = ISNEGATIVE(&tmp);\
}
#define GE(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = (ISPOSITIVE(&tmp) || ISZERO(&tmp));\
}
#define LE(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = (ISNEGATIVE(&tmp) || ISZERO(&tmp));\
}
#define NE(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = (!ISZERO(&tmp));\
}
#define COMPARETOTAL(Z, X, Y, C) {\
  char decVerString[LENGTH64];\
  float decVersion = 0.00;\
  strcpy(decVerString, decNumberVersion());\
  sscanf(decVerString, "%*s %f", &decVersion);\
  if (decVersion>3.32){\
    decNumberCompareTotal(Z, X, Y, C);\
  }\
  else \
    fprintf(stderr, "decNumberCompareTotal is not supported in ver 3.32 of decNumber library or before\n");\
}
#ifdef COUNTING
#define DIVIDE(Z, X, Y, C) {\
 (C)->status = (C)->status & (~DEC_Rounded);\
 decNumberDivide(Z, X, Y, C);\
 if ((C)->status & DEC_Rounded)\
   div_rounded++;\
 div_total++;\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define DIVIDE(Z, X, Y, C)      decNumberDivide(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#define DIVIDEINTEGER(Z, X, Y, C)     decNumberDivideInteger(Z, X, Y, C)
#define EXP(Z, X, C)        decNumberExp(Z, X, C)
#define LN(Z, X, C)         decNumberLn(Z, X, C)
#define LOG10(Z, X, C)      decNumberLog10(Z, X, C)
#ifdef COUNTING
#define MAX(Z, X, Y, C){\
 decimal64 x64, y64;\
 decimal64FromNumber(&x64, X, C);\
 decimal64FromNumber(&y64, Y, C);\
 add_minmax_check(&x64, &y64, X, Y);\
 add_minmax_total++;\
 decNumberMax(Z, X, Y, C);\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define MAX(Z, X, Y, C)     decNumberMax(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#ifdef COUNTING
#define MIN(Z, X, Y, C){\
 decimal64 x64, y64;\
 decimal64FromNumber(&x64, X, C);\
 decimal64FromNumber(&y64, Y, C);\
 add_minmax_check(&x64, &y64, X, Y);\
 add_minmax_total++;\
 decNumberMin(Z, X, Y, C);\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define MIN(Z, X, Y, C)     decNumberMin(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#define MINUS(Z, X, C)     decNumberMinus(Z, X, C)
#ifdef COUNTING
#define MULTIPLY(Z, X, Y, C){\
  mul_check(X, Y);\
  (C)->status = (C)->status & (~DEC_Rounded);\
  decNumberMultiply(Z, X, Y, C);\
  if ((C)->status & DEC_Rounded)\
    mul_rounded++;\
  mul_total++;\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define MULTIPLY(Z, X, Y, C)     decNumberMultiply(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#define NORMALIZE(Z, X, C)     decNumberNormalize(Z, X, C)
#define PLUS(Z, X, C)     decNumberPlus(Z, X, C)
#define POWER(Z, X, Y, C) {\
  int inA_save = decNumberIsNaN(X);\
  int inB_save = decNumberIsNaN(Y);\
  char decVerString[LENGTH64];\
  float decVersion = 0.00;\
  decNumberPower(Z, X, Y, C);\
  strcpy(decVerString, decNumberVersion());\
  sscanf(decVerString, "%*s %f", &decVersion);\
  if ((decVersion<3.32)&&(inA_save==0)&&(inB_save==0)&&\
      (decNumberIsNaN(Z)==1)){\
   fprintf(stderr, "[ERROR] This power function cannot handle fraction power! At LINE %d of FILE %s\n", __LINE__, __FILE__);\
   fprintf(stderr, "decNumber Version is %s\n", decVerString);\
   exit(1);\
 }\
}
#define QUANTIZE(Z, X, Y, C)     decNumberQuantize(Z, X, Y, C)
#define REMAINDER(Z, X, Y, C)     decNumberRemainder(Z, X, Y, C)
#define REMAINDERNEAR(Z, X, Y, C)     decNumberRemainderNear(Z, X, Y, C)
#ifdef COUNTING
#define RESCALE(Z, X, Y, C){\
 decimal64 x64, y64;\
 decimal64FromNumber(&x64, X, C);\
 decimal64FromNumber(&y64, Y, C);\
 add_rescale_check(&x64, &y64, X, Y);\
 add_other_total++;\
 (C)->status = (C)->status & (~DEC_Rounded);\
 decNumberRescale(Z, X, Y, C);\
 if ((C)->status & DEC_Rounded)\
   add_other_rounded++;\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define RESCALE(Z, X, Y, C)     decNumberRescale(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
     //SAME QUANTUM DOESN'T NEED DECCONTEXT
#define SAMEQUANTUM(Z, X, Y)     decNumberSameQuantum(Z, X, Y)
#ifdef COUNTING
#define SQUAREROOT(Z, X, C) {\
 (C)->status = (C)->status & (~DEC_Rounded);\
 decNumberSquareRoot(Z, X, C);\
 if ((C)->status & DEC_Rounded)\
   div_rounded++;\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define SQUAREROOT(Z, X, C)     decNumberSquareRoot(Z, X, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#ifdef COUNTING
#define SUBTRACT(Z, X, Y, C) {\
 decimal64 x64, y64;\
 decimal64FromNumber(&x64, X, C);\
 decimal64FromNumber(&y64, Y, C);\
 add_check(&x64, &y64, X, Y);\
 add_total++;\
 (C)->status = (C)->status & (~DEC_Rounded);\
 decNumberSubtract(Z, X, Y, C);\
 if ((C)->status & DEC_Rounded)\
   add_rounded++;\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define SUBTRACT(Z, X, Y, C)     decNumberSubtract(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#ifdef COUNTING
#define TOINTEGRAL(Z, X, C){\
 decimal64 x64, y64;\
 decNumber zero_local;\
 decNumberFromString(&zero_local, "0E0", C);\
 decimal64FromNumber(&x64, X, C);\
 decimal64FromNumber(&y64, &zero_local, C);\
   add_toint_check(&x64, &y64, X, &zero_local);\
   add_other_total++;\
   decNumberToIntegralValue(Z, X, C);\
}
#else // COUNTING (DECNUMBER_ARBITRARY)
#define TOINTEGRAL(Z, X, C)     decNumberToIntegralValue(Z, X, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
     //COPY DOESN'T NEED DECCONTEXT
#define COPY(Z, X)     decNumberCopy(Z, X)
     //TRIM DOESN'T NEED DECCONTEXT
#define TRIM(Z)     decNumberTrim(Z)
     //ZERO DOESN'T NEED DECCONTEXT
#define ZERO(Z)     decNumberZero(Z)
#define FROMSTRING(Z, R, C)    decNumberFromString(Z, R, C)
#define FROMINTEGER(Z, R, C) {\
 char times_str[10];\
 int length;\
 length = sprintf(times_str, "%d", R);\
 if (length<0)\
   fprintf(stderr, "Fatal Error at line %d of file %s\n", __LINE__, __FILE__);\
 else if (length>10)\
   fprintf(stderr, "Fatal Error at line %d of file %s\n", __LINE__, __FILE__);\
 else\
   FROMSTRING64(*Z, times_str, C);\
}
#define TOSTRING(Z, R)      decNumberToString (Z, R)
#define TOENGSTRING(Z, R)     decNumberToEngString (Z, R)
#define ISNEGATIVE(Z)     decNumberIsNegative(Z)
#define ISZERO(Z)     decNumberIsZero(Z)
#define ISPOSITIVE(Z) ((ISZERO(Z)==0)&&(ISNEGATIVE(Z)==0))
#define ISINFINITE(Z) decNumberIsInfinite(Z)
#define ISNAN(Z)  decNumberIsNaN(Z)
#else // WIDTH128

//macros for operations of decNumber library
//First define Rounding API modes
#define RPI DEC_ROUND_CEILING
#define RNI DEC_ROUND_FLOOR
#define RTZ DEC_ROUND_DOWN
#define RNE DEC_ROUND_HALF_EVEN
#define RNA DEC_ROUND_HALF_UP

#define SET_ROUNDMODE(C, ROUNDMODE) C.round = ROUNDMODE     
#define CONTEXTDEFAULT(X, Y) decContextDefault(&X, DEC_INIT_DECIMAL128)
#define ABS(Z, X, C)     decNumberAbs(Z, X, C)
#ifdef COUNTING
#define ADD(Z, X, Y, C) {\

#else // COUNTING (DECNUMBER_ARBITRARY)
#define ADD(Z, X, Y, C)      decNumberAdd(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#define COMPARE(Z, X, Y, C)     decNumberCompare(Z, X, Y, C)
#define EQ(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = ISZERO(&tmp);\
}
#define GT(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = ISPOSITIVE(&tmp);\
}
#define LT(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = ISNEGATIVE(&tmp);\
}
#define GE(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = (ISPOSITIVE(&tmp) || ISZERO(&tmp));\
}
#define LE(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = (ISNEGATIVE(&tmp) || ISZERO(&tmp));\
}
#define NE(Z, X, Y, C)  {\
                        decNumber tmp;\
                        decNumberCompare(&tmp, X, Y, C);\
                        *Z = (!ISZERO(&tmp));\
}
#define COMPARETOTAL(Z, X, Y, C)
#ifdef COUNTING
#define DIVIDE(Z, X, Y, C)
#else // COUNTING (DECNUMBER_ARBITRARY)
#define DIVIDE(Z, X, Y, C)      decNumberDivide(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#define DIVIDEINTEGER(Z, X, Y, C)     decNumberDivideInteger(Z, X, Y, C)
#define EXP(Z, X, C)        decNumberExp(Z, X, C)
#define LN(Z, X, C)         decNumberLn(Z, X, C)
#define LOG10(Z, X, C)      decNumberLog10(Z, X, C)
#ifdef COUNTING
#define MAX(Z, X, Y, C)
#else // COUNTING (DECNUMBER_ARBITRARY)
#define MAX(Z, X, Y, C)     decNumberMax(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#ifdef COUNTING
#define MIN(Z, X, Y, C)
#else // COUNTING (DECNUMBER_ARBITRARY)
#define MIN(Z, X, Y, C)     decNumberMin(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#define MINUS(Z, X, C)     decNumberMinus(Z, X, C)
#ifdef COUNTING
#define MULTIPLY(Z, X, Y, C)
#else // COUNTING (DECNUMBER_ARBITRARY)
#define MULTIPLY(Z, X, Y, C)     decNumberMultiply(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#define NORMALIZE(Z, X, C)     decNumberNormalize(Z, X, C)
#define PLUS(Z, X, C)     decNumberPlus(Z, X, C)
#define POWER(Z, X, Y, C) {\
  int inA_save = decNumberIsNaN(X);\
  int inB_save = decNumberIsNaN(Y);\
  char decVerString[LENGTH64];\
  float decVersion = 0.00;\
  decNumberPower(Z, X, Y, C);\
  strcpy(decVerString, decNumberVersion());\
  sscanf(decVerString, "%*s %f", &decVersion);\
  if ((decVersion<3.32)&&(inA_save==0)&&(inB_save==0)&&\
      (decNumberIsNaN(Z)==1)){\
   fprintf(stderr, "[ERROR] This power function cannot handle fraction power! At LINE %d of FILE %s\n", __LINE__, __FILE__);\
   fprintf(stderr, "decNumber Version is %s\n", decVerString);\
   exit(1);\
 }\
}
#define QUANTIZE(Z, X, Y, C)     decNumberQuantize(Z, X, Y, C)
#define REMAINDER(Z, X, Y, C)     decNumberRemainder(Z, X, Y, C)
#define REMAINDERNEAR(Z, X, Y, C)     decNumberRemainderNear(Z, X, Y, C)
#ifdef COUNTING
#define RESCALE(Z, X, Y, C)
#else // COUNTING (DECNUMBER_ARBITRARY)
#define RESCALE(Z, X, Y, C)     decNumberRescale(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
     //SAME QUANTUM DOESN'T NEED DECCONTEXT
#define SAMEQUANTUM(Z, X, Y)     decNumberSameQuantum(Z, X, Y)
#ifdef COUNTING
#define SQUAREROOT(Z, X, C) 
#else // COUNTING (DECNUMBER_ARBITRARY)
#define SQUAREROOT(Z, X, C)     decNumberSquareRoot(Z, X, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#ifdef COUNTING

#else // COUNTING (DECNUMBER_ARBITRARY)
#define SUBTRACT(Z, X, Y, C)     decNumberSubtract(Z, X, Y, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
#ifdef COUNTING

#else // COUNTING (DECNUMBER_ARBITRARY)
#define TOINTEGRAL(Z, X, C)     decNumberToIntegralValue(Z, X, C)
#endif // COUNTING (DECNUMBER_ARBITRARY)
     //COPY DOESN'T NEED DECCONTEXT
#define COPY(Z, X)     decNumberCopy(Z, X)
     //TRIM DOESN'T NEED DECCONTEXT
#define TRIM(Z)     decNumberTrim(Z)
     //ZERO DOESN'T NEED DECCONTEXT
#define ZERO(Z)     decNumberZero(Z)
#define FROMSTRING(Z, R, C)    decNumberFromString(Z, R, C)
#define FROMINTEGER(Z, R, C) {\
 char times_str[10];\
 int length;\
 length = sprintf(times_str, "%d", R);\
 if (length<0)\
   fprintf(stderr, "Fatal Error at line %d of file %s\n", __LINE__, __FILE__);\
 else if (length>10)\
   fprintf(stderr, "Fatal Error at line %d of file %s\n", __LINE__, __FILE__);\
 else\
   FROMSTRING64(*Z, times_str, C);\
}
#define TOSTRING(Z, R)      decNumberToString (Z, R)
#define TOENGSTRING(Z, R)     decNumberToEngString (Z, R)
#define ISNEGATIVE(Z)     decNumberIsNegative(Z)
#define ISZERO(Z)     decNumberIsZero(Z)
#define ISPOSITIVE(Z) ((ISZERO(Z)==0)&&(ISNEGATIVE(Z)==0))
#define ISINFINITE(Z) decNumberIsInfinite(Z)
#define ISNAN(Z)  decNumberIsNaN(Z)
#endif // WIDTH64

#if defined (DEC32) // DEC32
#define IEEEFROMSTR(Z, R, C) decimal32FromString(Z, R, C)
#define IEEETOSTR(Z, R) decimal32ToString(Z, R)
#define IEEETOENGSTR(Z, R) decimal32ToEngString(Z, R)
#define IEEEFROMNUMBER(Z, X, C) decimal32ToNumber(Z, X, C)
#define IEEETONUMBER(Z, X) decimal32ToNumber(Z, X)
#elif defined (DEC64) // DEC32
#define IEEEFROMSTR(Z, R, C) decimal64FromString(Z, R, C)
#define IEEETOSTR(Z, R) decimal64ToString(Z, R)
#define IEEETOENGSTR(Z, R) decimal64ToEngString(Z, R)
#define IEEEFROMNUMBER(Z, X, C) decimal64ToNumber(Z, X, C)
#define IEEETONUMBER(Z, X) decimal64ToNumber(Z, X)
#elif defined (DEC128) // DEC32
#define IEEEFROMSTR(Z, R, C) decimal128FromString(Z, R, C)
#define IEEETOSTR(Z, R) decimal128ToString(Z, R)
#define IEEETOENGSTR(Z, R) decimal128ToEngString(Z, R)
#define IEEEFROMNUMBER(Z, X, C) decimal128ToNumber(Z, X, C)
#define IEEETONUMBER(Z, X) decimal128ToNumber(Z, X)
#endif //ENDOF DEC32
//HW or LIBRARY SUPPORT
#include "decAsm.h"

#elif defined(DECNUMBER_FIXED) // end of DECNUMBER_ARBITRARY

#if defined(WIDTH64)
// First define round mode API
#define RPI DEC_ROUND_CEILING
#define RNI DEC_ROUND_FLOOR
#define RTZ DEC_ROUND_DOWN
#define RNE DEC_ROUND_HALF_EVEN
#define RNA DEC_ROUND_HALF_UP

#define CONTEXTDEFAULT(X, Y) decContextDefault(&X, DEC_INIT_DECIMAL64)
#define SET_ROUNDMODE(C, ROUNDMODE) C.round = ROUNDMODE     
#define ABS(Z, X, C) decDoubleAbs(Z, X, C)
#define ADD(Z, X, Y, C) decDoubleAdd(Z, X, Y, C)
#define COMPARE(Z, X, Y, C) decDoubleCompare(Z, X, Y, C)
#define EQ(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = ISZERO(&tmp);\
}
#define GT(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = ISPOSITIVE(&tmp);\
}
#define LT(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = ISNEGATIVE(&tmp);\
}
#define GE(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = (ISPOSITIVE(&tmp) || ISZERO(&tmp));\
}
#define LE(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = (ISNEGATIVE(&tmp) || ISZERO(&tmp));\
}
#define NE(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = (!ISZERO(&tmp));\
}
#define DIVIDE(Z, X, Y, C) decDoubleDivide(Z, X, Y, C)
#define MAX(Z, X, Y, C) decDoubleMax(Z, X, Y, C)
#define MIN(Z, X, Y, C) decDoubleMin(Z, X, Y, C)
#define MINUS(Z, X, C) decDoubleMinus(Z, X, C)
#define MULTIPLY(Z, X, Y, C) decDoubleMultiply(Z, X, Y, C)
#define POWER(Z, X, Y, C) {decNumber numX, numY, numZ; decDoubleToNumber(X, &numX); decDoubleToNumber(Y, &numY); decNumberPower(&numZ, &numX, &numY, C); decDoubleFromNumber(Z, &numZ, C);}
#define QUANTIZE(Z, X, Y, C) decDoubleQuantize(Z, X, Y, C)
#define RESCALE(Z, X, Y, C) {decNumber numX, numY, numZ; decDoubleToNumber(X, &numX); decDoubleToNumber(Y, &numY); decNumberRescale(&numZ, &numX, &numY, C); decDoubleFromNumber(Z, &numZ, C);}
#define SAMEQUANTUM(Z, X, Y) decDoubleSameQuantum(Z, X, Y)
#define SQUAREROOT(Z, X, C) {decNumber numX, numZ; decDoubleToNumber(X, &numX); decNumberSquareRoot(&numZ, &numX, C); decDoubleFromNumber(Z, &numZ, C);}
#define SUBTRACT(Z, X, Y, C) decDoubleSubtract(Z, X, Y, C)
#define TOINTEGRAL(Z, X, C) decDoubleToIntegralExact(Z, X, C)
#define COPY(Z, X) decDoubleCopy(Z, X)
#define ZERO(Z) decDoubleZero(Z)
#define FROMSTRING(Z, R, C) decDoubleFromString(Z, R, C)
#define FROMINTEGER(Z, R, C) decDoubleFromInt32(Z, R)
#define TOSTRING(Z, R) decDoubleToString(Z, R)
#define ISNEGATIVE(Z) decDoubleIsSigned(Z)
#define ISZERO(Z)  decDoubleIsZero(Z)
#define ISPOSITIVE(Z) ((ISZERO(Z)==0)&&(ISNEGATIVE(Z)==0))
#define ISINFINITE(Z) decDoubleIsInfinite(Z)
#define ISNAN(Z) decDoubleIsNaN(Z)
#define ISNORMAL(Z) decDoubleIsNormal(Z)
#include "decAsm.h"

#else // WIDTH128
// First define round mode API
#define RPI DEC_ROUND_CEILING
#define RNI DEC_ROUND_FLOOR
#define RTZ DEC_ROUND_DOWN
#define RNE DEC_ROUND_HALF_EVEN
#define RNA DEC_ROUND_HALF_UP

#define CONTEXTDEFAULT(X, Y) decContextDefault(&X, DEC_INIT_DECIMAL128)
#define SET_ROUNDMODE(C, ROUNDMODE) C.round = ROUNDMODE     
#define ABS(Z, X, C) decQuadAbs(Z, X, C)
#define ADD(Z, X, Y, C) decQuadAdd(Z, X, Y, C)
#define COMPARE(Z, X, Y, C) decQuadCompare(Z, X, Y, C)
#define EQ(Z, X, Y, C)  {\
                        decQuad tmp;\
                        decQuadCompare(&tmp, X, Y, C);\
                        *Z = ISZERO(&tmp);\
}
#define GT(Z, X, Y, C)  {\
                        decQuad tmp;\
                        decQuadCompare(&tmp, X, Y, C);\
                        *Z = ISPOSITIVE(&tmp);\
}
#define LT(Z, X, Y, C)  {\
                        decQuad tmp;\
                        decQuadCompare(&tmp, X, Y, C);\
                        *Z = ISNEGATIVE(&tmp);\
}
#define GE(Z, X, Y, C)  {\
                        decQuad tmp;\
                        decQuadCompare(&tmp, X, Y, C);\
                        *Z = (ISPOSITIVE(&tmp) || ISZERO(&tmp));\
}
#define LE(Z, X, Y, C)  {\
                        decQuad tmp;\
                        decQuadCompare(&tmp, X, Y, C);\
                        *Z = (ISNEGATIVE(&tmp) || ISZERO(&tmp));\
}
#define NE(Z, X, Y, C)  {\
                        decQuad tmp;\
                        decQuadCompare(&tmp, X, Y, C);\
                        *Z = (!ISZERO(&tmp));\
}
#define DIVIDE(Z, X, Y, C) decQuadDivide(Z, X, Y, C)
#define MAX(Z, X, Y, C) decQuadMax(Z, X, Y, C)
#define MIN(Z, X, Y, C) decQuadMin(Z, X, Y, C)
#define MINUS(Z, X, C) decQuadMinus(Z, X, C)
#define MULTIPLY(Z, X, Y, C) decQuadMultiply(Z, X, Y, C)
#define POWER(Z, X, Y, C) {decNumber numX, numY, numZ; decQuadToNumber(X, &numX); decQuadToNumber(Y, &numY); decNumberPower(&numZ, &numX, &numY, C); decQuadFromNumber(Z, &numZ, C);}
#define QUANTIZE(Z, X, Y, C) decQuadQuantize(Z, X, Y, C)
#define RESCALE(Z, X, Y, C) {decNumber numX, numY, numZ; decQuadToNumber(X, &numX); decQuadToNumber(Y, &numY); decNumberRescale(&numZ, &numX, &numY, C); decQuadFromNumber(Z, &numZ, C);}
#define SAMEQUANTUM(Z, X, Y) decQuadSameQuantum(Z, X, Y)
#define SQUAREROOT(Z, X, C) {decNumber numX, numZ; decQuadToNumber(X, &numX); decNumberSquareRoot(&numZ, &numX, C); decQuadFromNumber(Z, &numZ, C);}
#define SUBTRACT(Z, X, Y, C) decQuadSubtract(Z, X, Y, C)
#define TOINTEGRAL(Z, X, C) decQuadToIntegralExact(Z, X, C)
#define COPY(Z, X) decQuadCopy(Z, X)
#define ZERO(Z) decQuadZero(Z)
#define FROMSTRING(Z, R, C) decQuadFromString(Z, R, C)
#define FROMINTEGER(Z, R, C) decQuadFromInt32(Z, R)
#define TOSTRING(Z, R) decQuadToString(Z, R)
#define ISNEGATIVE(Z) decQuadIsSigned(Z)
#define ISZERO(Z)  decQuadIsZero(Z)
#define ISPOSITIVE(Z) ((ISZERO(Z)==0)&&(ISNEGATIVE(Z)==0))
#define ISINFINITE(Z) decQuadIsInfinite(Z)
#define ISNAN(Z) decQuadIsNaN(Z)
#define ISNORMAL(Z) decQuadIsNormal(Z)
#include "decAsm.h"
#endif // WIDTH64

#elif defined(BID)  // end of DECNUMBER_FIXED 

#if defined(WIDTH64)
//BID library support

// Define round mode API
#define RPI BID_ROUNDING_UP
#define RNI BID_ROUNDING_DOWN
#define RTZ BID_ROUNDING_TO_ZERO
#define RNE BID_ROUNDING_TO_NEAREST
#define RNA BID_ROUNDING_TIES_AWAY

// Define masks
#define MASK_STEERING_BITS              0x6000000000000000ull
#define MASK_BINARY_SIG2                0x0007ffffffffffffull
#define MASK_BINARY_OR2                 0x0020000000000000ull
#define MASK_BINARY_SIG1                0x001fffffffffffffull
#define MASK_NAN                        0x7c00000000000000ull
#define MASK_INF                        0x7800000000000000ull

#define CONTEXTDEFAULT(X, Y) set = RNE
#define SET_ROUNDMODE(C, ROUNDMODE) C = ROUNDMODE
#define ABS(Z, X, C) __bid64_abs(Z, X) 
#define ADD(Z, X, Y, C) __bid64_add(Z, X, Y, C) 
#define EQ(Z, X, Y, C)  __bid64_quiet_equal(Z, X, Y)
#define GT(Z, X, Y, C)  __bid64_quiet_greater(Z, X, Y)
#define LT(Z, X, Y, C)  __bid64_quiet_less(Z, X, Y)
#define GE(Z, X, Y, C)  __bid64_quiet_greater_equal(Z, X, Y)
#define LE(Z, X, Y, C)  __bid64_quiet_less_equal(Z, X, Y)
#define NE(Z, X, Y, C)  __bid64_quiet_not_equal(Z, X, Y)

#define DIVIDE(Z, X, Y, C) __bid64_div(Z, X, Y, C)
#define LOG10(Z, X, C) __bid64_log(Z, X, C)	// This may be wrong. No Documentation
#define MAX(Z, X, Y, C) __bid64_maxnum(Z, X, Y)
#define MIN(Z, X, Y, C) __bid64_minnum(Z, X, Y) 
#define MINUS(Z, X, C) __bid64_negate(Z, X)
#define MULTIPLY(Z, X, Y, C) __bid64_mul(Z, X, Y, C)
#define POWER(Z, X, Y, C) __bid64_pow(Z, X, Y, C)
#define QUANTIZE(Z, X, Y, C) __bid64_quantize(Z, X, Y, C) 
#define RESCALE(Z, X, Y, C) {fprintf(stderr, "Not supported. Use Quantize\n"); exit(-1);}
#define SAMEQUANTUM(Z, X, Y) __bid64_sameQuantum(Z, X, Y) 
#define SQUAREROOT(Z, X, C) __bid64_sqrt(Z, X, C) 
#define SUBTRACT(Z, X, Y, C) __bid64_sub(Z, X, Y, C) 
#define TOINTEGRAL(Z, X, C) __bid64_round_integral_exact(Z, X, C) 
#define COPY(Z, X) __bid64_copy(Z, X) 
#define ZERO(Z) *Z = 0x31c0000000000000ull
#define FROMSTRING(Z, R, C) __bid64_from_string(Z, R, C)
#define FROMINTEGER(Z, R, C) __bid64_from_int32(Z, &R)  // doesnt use C
#define TOSTRING(Z, R) __bid64_to_string(R, Z) 
#define ISNEGATIVE(Z) ((*Z & 0x8000000000000000ull) == 0x8000000000000000ull)
#define ISZERO(Z) (!((*Z & MASK_INF) == MASK_INF) && ((((*Z & MASK_STEERING_BITS) == MASK_STEERING_BITS) && (((*Z & MASK_BINARY_SIG2) | MASK_BINARY_OR2) > 9999999999999999ull)) || ((*Z & MASK_BINARY_SIG1) == 0)))
#define ISINFINITE(Z)  (((*Z & MASK_INF) == MASK_INF) && ((*Z & MASK_NAN) != MASK_NAN))
#define ISNAN(Z) ((*Z & MASK_NAN) == MASK_NAN)
#define ISNORMAL(Z) 
#include "decAsm.h"

#else // WIDTH128
//BID library support

// Define round mode API
#define RPI BID_ROUNDING_UP
#define RNI BID_ROUNDING_DOWN
#define RTZ BID_ROUNDING_TO_ZERO
#define RNE BID_ROUNDING_TO_NEAREST
#define RNA BID_ROUNDING_TIES_AWAY

// Define masks
#define MASK_STEERING_BITS              0x6000000000000000ull // not_needed in 128
#define MASK_BINARY_SIG2                0x0007ffffffffffffull // not_needed in 128
#define MASK_BINARY_OR2                 0x0020000000000000ull // not needed in 128
#define MASK_BINARY_UP                0x0001ffffffffffffull
#define MASK_BINARY_LOW                0xfffffffffffffffffull  // not really needed, since all the lower bits are significand
#define MASK_NAN                        0x7c00000000000000ull // same as decimal64, but will apply to upper 64 bits
#define MASK_INF                        0x7800000000000000ull // same as decimal64, but will apply to upper 64 bits

#define CONTEXTDEFAULT(X, Y) set = RNE
#define SET_ROUNDMODE(C, ROUNDMODE) C = ROUNDMODE
#define ABS(Z, X, C) __bid128_abs(Z, X) 
#define ADD(Z, X, Y, C) __bid128_add(Z, X, Y, C) 
#define EQ(Z, X, Y, C)  __bid128_quiet_equal(Z, X, Y)
#define GT(Z, X, Y, C)  __bid128_quiet_greater(Z, X, Y)
#define LT(Z, X, Y, C)  __bid128_quiet_less(Z, X, Y)
#define GE(Z, X, Y, C)  __bid128_quiet_greater_equal(Z, X, Y)
#define LE(Z, X, Y, C)  __bid128_quiet_less_equal(Z, X, Y)
#define NE(Z, X, Y, C)  __bid128_quiet_not_equal(Z, X, Y)

#define DIVIDE(Z, X, Y, C) __bid128_div(Z, X, Y, C)
#define LOG10(Z, X, C) __bid128_log(Z, X, C)	// This may be wrong. No Documentation
#define MAX(Z, X, Y, C) __bid128_maxnum(Z, X, Y)
#define MIN(Z, X, Y, C) __bid128_minnum(Z, X, Y) 
#define MINUS(Z, X, C) __bid128_negate(Z, X)
#define MULTIPLY(Z, X, Y, C) __bid128_mul(Z, X, Y, C)
#define POWER(Z, X, Y, C) __bid128_pow(Z, X, Y, C)
#define QUANTIZE(Z, X, Y, C) __bid128_quantize(Z, X, Y, C) 
#define RESCALE(Z, X, Y, C) {fprintf(stderr, "Not supported. Use Quantize\n"); exit(-1);}
#define SAMEQUANTUM(Z, X, Y) __bid128_sameQuantum(Z, X, Y) 
#define SQUAREROOT(Z, X, C) __bid128_sqrt(Z, X, C) 
#define SUBTRACT(Z, X, Y, C) __bid128_sub(Z, X, Y, C) 
#define TOINTEGRAL(Z, X, C) __bid128_round_integral_exact(Z, X, C) 
#define COPY(Z, X) __bid128_copy(Z, X) 
#define ZERO(Z) {(*Z).w[1] = 0x3040000000000000ull; (*Z).w[0] = 0ull;}
#define FROMSTRING(Z, R, C) __bid128_from_string(Z, R, C)
#define FROMINTEGER(Z, R, C) __bid128_from_int32(Z, &R)  // doesnt use C
#define TOSTRING(Z, R) __bid128_to_string(R, Z)
#define ISNEGATIVE(Z) ((*Z.w[1] & 0x8000000000000000ull) == 0x8000000000000000ull)
#define ISZERO(Z) ( !((*Z.w[1] & MASK_INF) == MASK_INF) && (((*Z.w[1] & MASK_BINARY_UP) == 0) && *Z.w[0] == 0)) // coefficient==0
#define ISINFINITE(Z)  (((*Z.w[1] & MASK_INF) == MASK_INF) && ((*Z.w[1] & MASK_NAN) != MASK_NAN))
#define ISNAN(Z) ((*Z.w[1] & MASK_NAN) == MASK_NAN)
#define ISNORMAL(Z) 
#include "decAsm.h"
#endif // WIDTH64

#elif defined(NATIVE_GCC_DPD) // end of BID 

// First define round mode API
#define RPI DEC_ROUND_CEILING
#define RNI DEC_ROUND_FLOOR
#define RTZ DEC_ROUND_DOWN
#define RNE DEC_ROUND_HALF_EVEN
#define RNA DEC_ROUND_HALF_UP

#define SET_ROUNDMODE(C, ROUNDMODE) C.round = ROUNDMODE     
//#define SET_ROUNDMODE(C, ROUNDMODE) __dfp_set_round(ROUNDMODE) 
#define CONTEXTDEFAULT(X, Y) decContextDefault(&X, DEC_INIT_DECIMAL64)
#define ABS(Z, X, C) decDoubleAbs(Z, X, C)
#define ADD(Z, X, Y, C) decDoubleAdd(Z, X, Y, C)
//#define ADD(Z, X, Y, C) *Z = *X + *Y 
#define COMPARE(Z, X, Y, C) decDoubleCompare(Z, X, Y, C)
#define EQ(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = ISZERO(&tmp);\
}
#define GT(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = ISPOSITIVE(&tmp);\
}
#define LT(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = ISNEGATIVE(&tmp);\
}
#define GE(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = (ISPOSITIVE(&tmp) || ISZERO(&tmp));\
}
#define LE(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = (ISNEGATIVE(&tmp) || ISZERO(&tmp));\
}
#define NE(Z, X, Y, C)  {\
                        decDouble tmp;\
                        decDoubleCompare(&tmp, X, Y, C);\
                        *Z = (!ISZERO(&tmp));\
}
#define DIVIDE(Z, X, Y, C) decDoubleDivide(Z, X, Y, C)
#define MAX(Z, X, Y, C) decDoubleMax(Z, X, Y, C)
#define MIN(Z, X, Y, C) decDoubleMin(Z, X, Y, C)
#define MINUS(Z, X, C) decDoubleMinus(Z, X, C)
#define MULTIPLY(Z, X, Y, C) decDoubleMultiply(Z, X, Y, C)
#define POWER(Z, X, Y, C) {decNumber numX, numY, numZ; decDoubleToNumber(X, &numX); decDoubleToNumber(Y, &numY); decNumberPower(&numZ, &numX, &numY, C); decDoubleFromNumber(Z, &numZ, C);}
#define QUANTIZE(Z, X, Y, C) decDoubleQuantize(Z, X, Y, C)
#define RESCALE(Z, X, Y, C) {decNumber numX, numY, numZ; decDoubleToNumber(X, &numX); decDoubleToNumber(Y, &numY); decNumberRescale(&numZ, &numX, &numY, C); decDoubleFromNumber(Z, &numZ, C);}
#define SAMEQUANTUM(Z, X, Y) decDoubleSameQuantum(Z, X, Y)
#define SQUAREROOT(Z, X, C) {decNumber numX, numZ; decDoubleToNumber(X, &numX); decNumberSquareRoot(&numZ, &numX, C); decDoubleFromNumber(Z, &numZ, C);}
#define SUBTRACT(Z, X, Y, C) decDoubleSubtract(Z, X, Y, C)
#define TOINTEGRAL(Z, X, C) decDoubleToIntegralExact(Z, X, C)
#define COPY(Z, X) decDoubleCopy(Z, X)
#define ZERO(Z) decDoubleZero(Z)
#define FROMSTRING(Z, R, C) decDoubleFromString(Z, R, C)
#define FROMINTEGER(Z, R, C) decDoubleFromInt32(Z, R)
#define TOSTRING(Z, R) decDoubleToString(Z, R)
#define ISNEGATIVE(Z) decDoubleIsSigned(Z)
#define ISZERO(Z)  decDoubleIsZero(Z)
#define ISPOSITIVE(Z) ((ISZERO(Z)==0)&&(ISNEGATIVE(Z)==0))
#define ISINFINITE(Z) decDoubleIsInfinite(Z)
#define ISNAN(Z) decDoubleIsNaN(Z)
#define ISNORMAL(Z) decDoubleIsNormal(Z)
#include "decAsm.h"

#elif defined(NATIVE_GCC_BID)  // end of NATIVE_GCC_DPD 
//BID library support

// Define round mode API
#define RPI FE_DEC_UPWARD
#define RNI FE_DEC_DOWNWARD 
#define RTZ FE_DEC_TOWARDZERO
#define RNE FE_DEC_TONEAREST
#define RNA FE_DEC_TONEARESTFROMZERO

// Define masks
#define MASK_STEERING_BITS              0x6000000000000000ull
#define MASK_BINARY_SIG2                0x0007ffffffffffffull
#define MASK_BINARY_OR2                 0x0020000000000000ull
#define MASK_BINARY_SIG1                0x001fffffffffffffull
#define MASK_NAN                        0x7c00000000000000ull
#define MASK_INF                        0x7800000000000000ull


#define CONTEXTDEFAULT(X, Y) {\
                             __dfp_set_round(RNE);\
                             set = RNE;\
}
#define SET_ROUNDMODE(C, ROUNDMODE) {\
                             __dfp_set_round(ROUNDMODE);\
                             C = ROUNDMODE;\
}
#define ABS(Z, X, C) { if(*X < 0) { *Z = -(*X); } 
#define ADD(Z, X, Y, C) *Z = *X + *Y 
#define EQ(Z, X, Y, C) { *Z = (*X == *Y); }
#define GT(Z, X, Y, C) { *Z = (*X > *Y); } 
#define LT(Z, X, Y, C) { *Z = (*X < *Y); } 
#define GE(Z, X, Y, C) { *Z = (*X >= *Y); } 
#define LE(Z, X, Y, C) { *Z = (*X <= *Y); } 
#define NE(Z, X, Y, C) { *Z = (*X != *Y); } 
#define DIVIDE(Z, X, Y, C) *Z = (*X) / (*Y) 
#define LOG10(Z, X, C) __bid64_log((BID_UINT64*)Z, (BID_UINT64*)X, C)	// This may be wrong. No Documentation
#define MAX(Z, X, Y, C) { if(*X > *Y) { *Z = *X; } else { *Z = *Y; } }
#define MIN(Z, X, Y, C) { if(*X < *Y) { *Z = *X; } else { *Z = *Y; } }
#define MINUS(Z, X, C) *Z = -(*X)
#define MULTIPLY(Z, X, Y, C) *Z = (*X) * (*Y)
#define POWER(Z, X, Y, C) __bid64_pow((BID_UINT64*)Z, (BID_UINT64*)X, (BID_UINT64*)Y, C)
#define QUANTIZE(Z, X, Y, C) __bid64_quantize((BID_UINT64*)Z, (BID_UINT64*)X, (BID_UINT64*)Y, C, &flags) 
#define RESCALE(Z, X, Y, C) {fprintf(stderr, "Not supported. Use Quantize\n"); exit(-1);}
#define SAMEQUANTUM(Z, X, Y) __bid64_sameQuantum((BID_UINT64*)Z, (BID_UINT64*)X, (BID_UINT64*)Y) 
#define SQUAREROOT(Z, X, C) __bid64_sqrt((BID_UINT64*)Z, (BID_UINT64*)X, C, &flags) 
#define SUBTRACT(Z, X, Y, C) *Z = *X - *Y; 
#define TOINTEGRAL(Z, X, C) __bid64_round_integral_exact((BID_UINT64*)Z, (BID_UINT64*)X, C, &flags) 
#define COPY(Z, X) *Z = *X 
#define ZERO(Z) *Z = 0
#define FROMSTRING(Z, R, C) __bid64_from_string((BID_UINT64*)Z, R, C, &flags)
#define FROMINTEGER(Z, R, C) __bid64_from_int32((BID_UINT64*)Z, &R)  // doesnt use C
#define TOSTRING(Z, R) __bid64_to_string(R, (BID_UINT64*)Z, &flags) 

#define ISNEGATIVE(Z) ((*((BID_UINT64 *)Z) & 0x8000000000000000ull) == 0x8000000000000000ull)
#define ISZERO(Z) (!((*((BID_UINT64 *)Z) & MASK_INF) == MASK_INF) && ((((*((BID_UINT64 *)Z) & MASK_STEERING_BITS) == MASK_STEERING_BITS) && (((*((BID_UINT64 *)Z) & MASK_BINARY_SIG2) | MASK_BINARY_OR2) > 9999999999999999ull)) || ((*((BID_UINT64 *)Z) & MASK_BINARY_SIG1) == 0)))
#define ISINFINITE(Z)  (((*((BID_UINT64 *)Z) & MASK_INF) == MASK_INF) && ((*((BID_UINT64 *)Z) & MASK_NAN) != MASK_NAN))
#define ISNAN(Z) ((*((BID_UINT64 *)Z) & MASK_NAN) == MASK_NAN)
#define ISNORMAL(Z)
#include "decAsm.h"

//macros for operation of binary double
#else //double
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define CONTEXTDEFAULT(X, Y) 
#define ABS(Z, X, C) Z; *Z = fabs(*X);
#define ADD(Z, X, Y, C) Z; *Z=(*X)+(*Y);
#define COMPARE(Z, X, Y, C) Z; *Z = (*X>*Y)?1:((*X==*Y)?0:-1);
#define DIVIDE(Z, X, Y, C) Z; *Z = (*X)/(*Y);
#define DIVIDEINTEGER(Z, X, Y, C) Z; int temp = (int)(*X)/(*Y); *Z = temp;
#define MAX(Z, X, Y, C) Z; *Z=((*X)>=(*Y))?*X:*Y;
#define MIN(Z, X, Y, C) Z; *Z=((*X)<(*Y))?*X:*Y;
#define MINUS(Z, X, C) Z; double tempMinus; tempMinus=0-(*X); *Z = tempMinus;
#define MULTIPLY(Z, X, Y, C) Z; *Z=(*X)*(*Y);
#define NORMALIZE(Z, X, C) Z; printf("NORMALIZE:No mirror fcn in binary\n");
#define PLUS(Z, X, C) *Z = 0.0 + (*X);
#define POWER(Z, X, Y, C) Z; *Z = pow(*X, *Y);
#define QUANTIZE(Z, X, Y, C) Z; \
{ \
  int exp1, exp2; \
  double mantissa, trash; \
  char str1[65];\
  char str2[65];\
  int length;\
  mantissa = frexp(*X, &exp1); \
  trash = frexp(*Y, &exp2);  \
  if (exp2>exp1){ \
    length = sprintf(str1, "%f", *X);\
    strncpy(str2, str1, length-(exp2-exp1));\
    *Z = atof(str2);\
  } \
  else \
    *Z = *X; \
}

#define REMAINDER(Z, X, Y, C) Z; \
{\
  int quotient = (int)((*X)/(*Y));\
  *Z = (*X)-(*Y)*(double)quotient;\
}

#define REMAINDERNEAR(Z, X, Y, C) Z;\
{\
  int quotient = (int)((*X)/(*Y));\
  double quotient_double = (*X)/(*Y); \
  double diff = quotient_double-quotient; \
  if (diff>0.5) \
    quotient++; \
  else if (diff==0.5) \
    quotient = quotient + (quotient%2); \
  *Z = (*X)-(*Y)*(double)quotient; \
}
#define RESCALE(Z, X, Y, C) Z; \
{ \
  int exp1, exp2; \
  double mantissa; \
  char str1[65];\
  char str2[65];\
  int length;\
  exp2 = (int)(*Y);\
  mantissa = frexp(*X, &exp1); \
  if (exp2>exp1){ \
    length = sprintf(str1, "%f", *X);\
    printf ("length is %d\n", length);\
    printf ("exp2-exp1 is %d\n", exp2-exp1);\
    if (length >= exp2-exp1){\
      strncpy(str2, str1, length-(exp2-exp1));\
      *Z = atof(str2);\
    }\
    else {*Z = *X;}\
  } \
  else \
    *Z = *X; \
}

#define SAMEQUANTUM(Z, X, Y) Z; \
{\
  int exp1, exp2; \
  long long mantissa1, mantissa2;\
  mantissa1 = frexp(*X, &exp1);\
  mantissa2 = frexp(*Y, &exp2);\
  if ((exp1==exp2)||(isnan(*X)==1)&&(isnan(*Y)==1)|| \
      (isinf(*X)==1)&&(isinf(*Y)==1)) \
    *Z = 1.0; \
  else \
    *Z = 0.0;\
}
#define SQUAREROOT(Z, X, C)  Z; *Z = sqrt(*X);
#define SUBTRACT(Z, X, Y, C)  Z; *Z = (*X)-(*Y);
#define TOINT(Z, X, C) Z; *Z=floor(*X);
#define COPY(Z, X) Z; *Z = *X;
#define TRIM(Z) Z; printf("TRIM: No mirror fcn in binary\n");
#define ZERO(Z) Z; *Z = 0;
#define FROMSTRING(Z, R, C) Z; *Z = atof(X);
#define TOSTRING(Z, R) &R[0]; sprintf(R, "%f", *Z);
#define TOENGSTRING(Z, R) &R[0]; printf("TOENGSTR: No mirror fcn in binary\n");
#define ISNEGATIVE(Z)  (Z<0)
#define ISZERO(Z)  (Z==0)     

#endif
//End of macros for operations

#endif


/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-19 22:09:04 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: operator.h,v $
  Revision 1.15  2009-04-19 22:09:04  mjanders
  Updated decnumber and Intel libraries

  Revision 1.14  2009-04-02 20:41:04  mjanders
  Fixed bug

  Revision 1.13  2009-03-20 20:15:56  mjanders
  small changes

  Revision 1.12  2009-03-15 06:24:34  mjanders
  Added 128 bit support

  Revision 1.11  2009-01-02 17:20:03  mjanders
  Added native_gcc_bid rounding

  Revision 1.10  2008/11/19 06:02:43  mjanders
  Fixed mistake with flags and FROMINTEGER

  Revision 1.9  2008/09/24 07:39:47  mjanders
  Added flags variable

  Revision 1.8  2008/09/23 07:32:19  mjanders
  Changed over to native gcc functions.

  Revision 1.7  2008/09/21 23:18:23  mjanders
  Added support for native gcc types

  Revision 1.6  2008/07/23 05:04:12  mjanders
  Added round mode API.

  Revision 1.5  2008/06/11 07:40:46  mjanders
  Fixed ISZERO in the BID section, though it is not used in this benchmark

  Revision 1.4  2008/05/19 00:32:37  mjanders
  Added BID functionality

  Revision 1.3  2008/04/25 00:36:32  mjanders
  There was a problem with using DECNUMBER_ARBITRARY. The 'RESCALE' macro
  does not have a corresponding decDouble function. It just said
  'Not supported. Use Quantize'. In order to use QUANTIZE I would have to
  edit the benchmark C code to call the function differently:

  QUANTIZE: Set first parameter to value of second parameter with the exponent
  explicitly written in the 3rd parameter
  RESCALE: Set first parameter to the value of the second parameter with the
  same exponent as the number in the 3rd parameter.

  I decided to just make it convert to decNumber in operator.h and call
  decNumberRescale for simplicity.

  Revision 1.2  2008/04/24 23:13:16  mjanders
  Converted telco benchmark to use decFloats. Changed the Makefile to
  define DECNUMBER_FIXED in this case. Added COUNTING switch to the Makefile.
  Updated const.h to make dec_t be a decDouble if DECNUMBER_FIXED is defined.
  copied over the update operator.h file and fixed the TOINTEGRAL bug.

  Updated cTelco754r.c on lines 170 and 166 to fix the mymemcpy issue for
  the new datatypes. I also added in a declaration of add_op_zero to the
  top of this file because it caused a compilation error when COUNTING=ON.

  Revision 1.3  2008/04/24 03:07:15  stsen
  removed check for ISZERO, which was causing a bug if the entry was actually zero

  Revision 1.2  2008/04/23 17:05:13  stsen
  context annotations for ifdef

  Revision 1.1.1.1  2008/04/15 23:33:39  stsen
  initial benchmark structure


  Revision 1.3  2007/07/29 17:06:16  lwang
  update version with decDouble support

  Revision 1.2  2007/07/29 01:01:26  lwang
  updated

  Revision 1.1.1.1  2007/03/13 03:49:13  lwang
  Euro currency conversion, simplified version

  Revision 1.5  2006/12/24 02:18:22  lwang
  support for HW, initial check-in

  Revision 1.4  2006/11/19 23:23:09  lwang
  First draft of Banking System is done

  Revision 1.3  2006/11/17 02:20:00  lwang
  Credit Card benchmark added

  Revision 1.2  2006/11/07 03:34:01  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.1  2006/10/28 01:54:18  lwang
  Banking System Initial Check-in

  Revision 1.2  2006/10/12 05:51:46  lwang
  new version, bug free

  Revision 1.1  2006/09/15 12:52:30  lwang
  initial check-in for the CD microbenchmark

*/
