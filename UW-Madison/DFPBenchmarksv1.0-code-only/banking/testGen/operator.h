//----------------------------------------
// operator.h
// Decimal Floating-Point Benchmark
// This file is to define the names for all the operations in decNumber
// and other libraries.
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.9 $

#ifndef _OPERATOR_H
#define _OPERATOR_H

//#if defined (GCC) || defined(DEC32) || defined(DEC64) || defined(DEC128) || defined(SIMPLESCALAR)
#if defined (GCC) || defined(DECNUMBER_ARBITRARY) || defined(SIMPLESCALAR)
//macros for operations of decNumber library
//First define Rounding API modes
#define RPI DEC_ROUND_CEILING
#define RNI DEC_ROUND_FLOOR
#define RTZ DEC_ROUND_DOWN
#define RNE DEC_ROUND_HALF_EVEN
#define RNA DEC_ROUND_HALF_UP

#define SET_ROUNDMODE(C, ROUNDMODE) C.round = ROUNDMODE     
#define CONTEXTDEFAULT(X, Y) decContextDefault(&X, DEC_INIT_DECIMAL64)
#define ABS(Z, X, C)         decNumberAbs(Z, X, C)
#define ADD(Z, X, Y, C)      decNumberAdd(Z, X, Y, C)
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
#define DIVIDE(Z, X, Y, C)      decNumberDivide(Z, X, Y, C)
#define DIVIDEINTEGER(Z, X, Y, C)     decNumberDivideInteger(Z, X, Y, C)
#define EXP(Z, X, C)        decNumberExp(Z, X, C)
#define LN(Z, X, C)         decNumberLn(Z, X, C)
#define LOG10(Z, X, C)      decNumberLog10(Z, X, C)
#define MAX(Z, X, Y, C)     decNumberMax(Z, X, Y, C)
#define MIN(Z, X, Y, C)     decNumberMin(Z, X, Y, C)
#define MINUS(Z, X, C)     decNumberMinus(Z, X, C)
#define MULTIPLY(Z, X, Y, C)     decNumberMultiply(Z, X, Y, C)
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
 /*printf("Power function is working\n")*/

#define QUANTIZE(Z, X, Y, C)     decNumberQuantize(Z, X, Y, C)
#define REMAINDER(Z, X, Y, C)     decNumberRemainder(Z, X, Y, C)
#define REMAINDERNEAR(Z, X, Y, C)     decNumberRemainderNear(Z, X, Y, C)
#define RESCALE(Z, X, Y, C)     decNumberRescale(Z, X, Y, C)
     //SAME QUANTUM DOESN'T NEED DECCONTEXT
#define SAMEQUANTUM(Z, X, Y)     decNumberSameQuantum(Z, X, Y)
#define SQUAREROOT(Z, X, C)     decNumberSquareRoot(Z, X, C)
#define SUBTRACT(Z, X, Y, C)     decNumberSubtract(Z, X, Y, C)
#define TOINTEGRAL(Z, X, C)     decNumberToIntegralValue(Z, X, C)
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
   FROMSTRING(Z, times_str, C);\
}
#define TOSTRING(Z, R)      decNumberToString (Z, R)
#define TOENGSTRING(Z, R)     decNumberToEngString (Z, R)
#define ISNEGATIVE(Z)     decNumberIsNegative(Z)
#define ISZERO(Z)     decNumberIsZero(Z)
#define ISPOSITIVE(Z) ((ISZERO(Z)==0)&&(ISNEGATIVE(Z)==0))
#define ISINFINITE(Z) decNumberIsInfinite(Z)
#define ISNAN(Z)  decNumberIsNaN(Z)
#if defined (DEC32)
#define IEEEFROMSTR(Z, R, C) decimal32FromString(Z, R, C)
#define IEEETOSTR(Z, R) decimal32ToString(Z, R)
#define IEEETOENGSTR(Z, R) decimal32ToEngString(Z, R)
#define IEEEFROMNUMBER(Z, X, C) decimal32ToNumber(Z, X, C)
#define IEEETONUMBER(Z, X) decimal32ToNumber(Z, X)
#elif defined (DEC64)
#define IEEEFROMSTR(Z, R, C) decimal64FromString(Z, R, C)
#define IEEETOSTR(Z, R) decimal64ToString(Z, R)
#define IEEETOENGSTR(Z, R) decimal64ToEngString(Z, R)
#define IEEEFROMNUMBER(Z, X, C) decimal64ToNumber(Z, X, C)
#define IEEETONUMBER(Z, X) decimal64ToNumber(Z, X)
#elif defined (DEC128)
#define IEEEFROMSTR(Z, R, C) decimal128FromString(Z, R, C)
#define IEEETOSTR(Z, R) decimal128ToString(Z, R)
#define IEEETOENGSTR(Z, R) decimal128ToEngString(Z, R)
#define IEEEFROMNUMBER(Z, X, C) decimal128ToNumber(Z, X, C)
#define IEEETONUMBER(Z, X) decimal128ToNumber(Z, X)
#endif //ENDOF DEC32
//HW or LIBRARY SUPPORT
#include "decAsm.h"
#elif defined(DECNUMBER_FIXED)// (end of DECNUMBER_ARBITRARY)

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
#define LOG10(Z, X, C) {\
 decNumber dnX, dnZ;\
 decDoubleToNumber(X, &dnX);\
 decNumberLog10(&dnZ, &dnX, C);\
 decDoubleFromNumber(Z, &dnZ, C); \
 }
#define MAX(Z, X, Y, C) decDoubleMax(Z, X, Y, C)
#define MIN(Z, X, Y, C) decDoubleMin(Z, X, Y, C)
//#define MINUS(Z, X, Y, C) decDoubleMinus(Z, X, Y, C)
#define MINUS(Z, X, C) decDoubleMinus(Z, X, C)
#define MULTIPLY(Z, X, Y, C) decDoubleMultiply(Z, X, Y, C)
#define POWER(Z, X, Y, C) {decNumber numX, numY, numZ; decDoubleToNumber(X, &numX); decDoubleToNumber(Y, &numY); decNumberPower(&numZ, &numX, &numY, C); decDoubleFromNumber(Z, &numZ, C);}
#define QUANTIZE(Z, X, Y, C) decDoubleQuantize(Z, X, Y, C)
//#define RESCALE(Z, X, Y, C) {fprintf(stderr, "Not supported. Use Quantize\n"); exit(-1);}
#define RESCALE(Z, X, Y, C) {\
 decNumber dnX, dnY, dnZ;\
 decDoubleToNumber(X, &dnX);\
 decDoubleToNumber(Y, &dnY);\
 decNumberRescale(&dnZ, &dnX, &dnY, C);\
 decDoubleFromNumber(Z, &dnZ, C); \
 }
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
#elif defined(BID)  // end of DECNUMBER_FIXED
#include "decDouble.h"

//BID library support
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
#define COMPARE(Z, X, Y, C) decDoubleCompare(Z, X, Y, C)
#define EQ(Z, X, Y, C)  __bid64_quiet_equal(Z, X, Y)
#define GT(Z, X, Y, C)  __bid64_quiet_greater(Z, X, Y)
#define LT(Z, X, Y, C)  __bid64_quiet_less(Z, X, Y)
#define GE(Z, X, Y, C)  __bid64_quiet_greater_equal(Z, X, Y)
#define LE(Z, X, Y, C)  __bid64_quiet_less_equal(Z, X, Y)
#define NE(Z, X, Y, C)  __bid64_quiet_not_equal(Z, X, Y)

#define DIVIDE(Z, X, Y, C) __bid64_div(Z, X, Y, C)
//#define LOG10(Z, X, C) __bid64_log(Z, X, C)	// This may be wrong. No Documentation
#define LOG10(Z, X, C) { \
  double input, result; \
  __bid64_to_binary64(&input, X, C); \
  result = log(input); \
  __binary64_to_bid64(Z, &result, C); \
}  
#define MAX(Z, X, Y, C) __bid64_maxnum(Z, X, Y)
#define MIN(Z, X, Y, C) __bid64_minnum(Z, X, Y) 
#define MINUS(Z, X, C) __bid64_negate(Z, X)
#define MULTIPLY(Z, X, Y, C) __bid64_mul(Z, X, Y, C)
//#define POWER(Z, X, Y, C) __bid64_pow(Z, X, Y, C)
#define POWER(Z, X, Y, C) { \
  double base, exponent, result; \
  __bid64_to_binary64(&base, X, C); \
  __bid64_to_binary64(&exponent, Y, C); \
  result = pow(base, exponent); \
  __binary64_to_bid64(Z, &result, C); \
}  
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
//


//macros for operation of binary double
#else //double
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

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
#define TOINTEGRAL(Z, X, C) Z; *Z=floor(*X);
#define COPY(Z, X) Z; *Z = *X;
#define TRIM(Z) Z; printf("TRIM: No mirror fcn in binary\n");
#define ZERO(Z) Z; *Z = 0;
#define FROMSTRING(Z, R, C) Z; *Z = atof(X);
#define TOSTRING(Z, R) &X[0]; sprintf(X, "%f", *Z);
#define TOENGSTRING(Z, R) &X[0]; printf("TOENGSTR: No mirror fcn in binary\n");
#define ISNEGATIVE(Z)  (Z<0)
#define ISZERO(Z)  (Z==0)     

#endif
//End of macros for operations

#endif


/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-19 21:40:13 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: operator.h,v $
  Revision 1.9  2009-04-19 21:40:13  mjanders
  Updated decnumber and BID libraries

  Revision 1.8  2008-11-24 08:58:48  mjanders
  Added BID support

  Revision 1.7  2008/08/06 04:07:51  mjanders
  Changed RESCALE for decDouble so that it will use DecNumber's version

  Revision 1.6  2008/05/15 21:55:25  stsen
  compiles correctly for each SW library (except we're missing LOG and POW from IDFPL)

  Revision 1.5  2008/05/15 19:54:19  stsen
  added ISPOSITIVE for decDouble

  Revision 1.4  2008/05/15 17:02:22  stsen
  made update to API, replacing explicit comparisons (EQ, LT, GT, etc) for COMPARE64

  Revision 1.3  2008/05/14 22:25:17  stsen
  updated for BID, and new API for COMPARE->GT

  Revision 1.2  2008/05/14 05:44:28  stsen
  updates for BID (works for decNumber, but still not working for BID)

  Revision 1.1.1.1  2008/04/18 01:13:49  mjanders


  Revision 1.6  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

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
