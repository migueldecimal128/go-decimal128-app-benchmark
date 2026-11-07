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

#ifndef DECASM_H
#define DECASM_H

#if defined(HW)
#define SET_ROUNDMODE(C, ROUNDMODE) \
     asm volatile ("droundmode/a %0" \
                   : : "r"(ROUNDMODE)\
                  )
#define ABS32(ZM, XM, C) \
     asm volatile ("dabs32/a  %0, %1"        \
                   : "=f"((ZM).a)                    \
                   : "f"((XM).a)   \
                   )
#define ABS64(ZM, XM, C) \
     asm volatile ("dabs64/a  %0, %1"        \
                   : "=f"((ZM).a)   \
                   : "f"((XM).a) \
                   )
#define ADD32(ZM, XM, YM, C)   \
     asm volatile ("dadd32/a  %0, %1, %2"        \
                   : "=f"((ZM).a)                    \
                   : "f"((XM).a), "f"((YM).a)   \
                   )
#define ADD64(ZM, XM, YM, C)   \
     asm volatile ("dadd64/a  %0, %1, %2"        \
                   : "=f"((ZM).a)   \
                   : "f"((XM).a), "f"((YM).a) \
                   )
#define COMPARE32(ZM, XM, YM, C)\
     asm volatile ("dcmp32/a  %0, %1, %2"        \
                   : "=f"((ZM).a)                    \
                   : "f"((XM).a), "f"((YM).a)   \
                   )
#define COMPARE64(ZM, XM, YM, C)\
     asm volatile ("dcmp64/a  %0, %1, %2"        \
                   : "=f"(((ZM)).a)   \
                   : "f"(((XM)).a), "f"(((YM)).a) \
                   )
#define COMPARETOTAL32(ZM, XM, YM, C){ \
  decNumber a, b;\
  decimal32ToNumber(&(XM).b, &a);\
  decimal32ToNumber(&(YM).b, &b);\
  decNumberCompareTotal(&a, &a, &b, C);\
  decimal32FromNumber(&(ZM).b, &a, C); }
#define COMPARETOTAL64(ZM, XM, YM, C){\
  decNumber a, b;\
  decimal64ToNumber(&(XM).b, &a);\
  decimal64ToNumber(&(YM).b, &b);\
  decNumberCompareTotal(&a, &a, &b, C);\
  decimal64FromNumber(&(ZM).b, &a, C);\
}
#define DIVIDE32(ZM, XM, YM, C)   \
     asm volatile ("ddiv32/a  %0, %1, %2"        \
                   : "=f"((ZM).a)                    \
                   : "f"((XM).a), "f"((YM).a)   \
                   )
#define DIVIDE64(ZM, XM, YM, C)   \
     asm volatile ("ddiv64/a  %0, %1, %2"        \
                   : "=f"((ZM).a)   \
                   : "f"((XM).a), "f"((YM).a) \
                   )
#define DIVIDEINTEGER32(ZM, XM, YM, C){\
  decNumber a, b;\
  decimal32ToNumber(&(XM).b, &a);\
  decimal32ToNumber(&(YM).b, &b);\
  decNumberDivideInteger(&a, &a, &b, C);\
  decimal32FromNumber(&(ZM).b, &a, C);\
}
#define DIVIDEINTEGER64(ZM, XM, YM, C){\
  decNumber a, b;\
  decimal64ToNumber(&(XM).b, &a);\
  decimal64ToNumber(&(YM).b, &b);\
  decNumberDivideInteger(&a, &a, &b, C);\
  decimal64FromNumber(&(ZM).b, &a, C);\
}
#define EXP32(ZM, XM, C){\
  decNumber a;\
  decimal32ToNumber(&(XM).b, &a);\
  decNumberExp(&a, &a, C);\
  decimal32FromNumber(&(ZM).b, &a, C);\
}
#define EXP64(ZM, XM, C){\
  decNumber a;\
  decimal64ToNumber(&(XM).b, &a);\
  decNumberExp(&a, &a, C);\
  decimal64FromNumber(&(ZM).b, &a, C);\
}
#define LN32(ZM, XM, C){\
  decNumber a;\
  decimal32ToNumber(&(XM).b, &a);\
  decNumberLn(&a, &a, C);\
  decimal32FromNumber(&(ZM).b, &a, C);\
}
#define LN64(ZM, XM, C){\
  decNumber a;\
  decimal64ToNumber(&(XM).b, &a);\
  decNumberLn(&a, &a, C);\
  decimal64FromNumber(&(ZM).b, &a, C);\
}
#define LOG1032(ZM, XM, C){\
  decNumber a;\
  decimal32ToNumber(&(XM).b, &a);\
  decNumberLog10(&a, &a, C);\
  decimal32FromNumber(&(ZM).b, &a, C);\
}
#define LOG1064(ZM, XM, C){\
  decNumber a;\
  decimal64ToNumber(&(XM).b, &a);\
  decNumberLog10(&a, &a, C);\
  decimal64FromNumber(&(ZM).b, &a, C);\
}
#define NORMALIZE32(ZM, XM, C){\
  decNumber a;\
  decimal32ToNumber(&(XM).b, &a);\
  decNumberNormalize(&a, &a, C);\
  decimal32FromNumber(&(ZM).b, &a, C);\
}
#define NORMALIZE64(ZM, XM, C){\
  decNumber a;\
  decimal64ToNumber(&(XM).b, &a);\
  decNumberNormalize(&a, &a, C);\
  decimal64FromNumber(&(ZM).b, &a, C);\
}

#define POWER32(ZM, XM, YM, C){\
  decNumber a, b;\
  decimal32ToNumber(&(XM).b, &a);\
  decimal32ToNumber(&(YM).b, &b);\
  decNumberPower(&a, &a, &b, C);\
  decimal32FromNumber(&(ZM).b, &a, C);\
}

#define POWER64(ZM, XM, YM, C){\
  decNumber a, b;\
  decimal64ToNumber(&(XM).b, &a);\
  decimal64ToNumber(&(YM).b, &b);\
  decNumberPower(&a, &a, &b, C);\
  decimal64FromNumber(&(ZM).b, &a, C);\
}
#define REMAINDER32(ZM, XM, YM, C){\
  decNumber a, b;\
  decimal32ToNumber(&(XM).b, &a);\
  decimal32ToNumber(&(YM).b, &b);\
  decNumberRemainder(&a, &a, &b, C);\
  decimal32FromNumber(&(ZM).b, &a, C);\
}

#define REMAINDER64(ZM, XM, YM, C){\
  decNumber a, b;\
  decimal64ToNumber(&(XM).b, &a);\
  decimal64ToNumber(&(YM).b, &b);\
  decNumberRemainder(&a, &a, &b, C);\
  decimal64FromNumber(&(ZM).b, &a, C);\
}
#define MAX32(ZM, XM, YM, C)\
  asm volatile ("dmax32/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define MAX64(ZM, XM, YM, C)   \
  asm volatile ("dmax64/a  %0, %1, %2"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a), "f"((YM).a) \
               )
#define MIN32(ZM, XM, YM, C)\
  asm volatile ("dmin32/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define MIN64(ZM, XM, YM, C)   \
  asm volatile ("dmin64/a  %0, %1, %2"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a), "f"((YM).a) \
               )
#define MINUS32(ZM, XM, C)\
  asm volatile ("dminus32/a  %0, %1"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a) \
               )
#define MINUS64(ZM, XM, C)\
  asm volatile ("dminus64/a  %0, %1"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a) \
               )
#define MULTIPLY32(ZM, XM, YM, C)   \
  asm volatile ("dmul32/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define MULTIPLY64(ZM, XM, YM, C)   \
  asm volatile ("dmul64/a  %0, %1, %2"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a), "f"((YM).a) \
               )
#define PLUS32(ZM, XM, C)\
  asm volatile ("dplus32/a  %0, %1"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a) \
               )
#define PLUS64(ZM, XM, C)\
  asm volatile ("dplus64/a  %0, %1"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a) \
               )
#define QUANTIZE32(ZM, XM, YM, C) \
  asm volatile ("dqat32/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define QUANTIZE64(ZM, XM, YM, C) \
  asm volatile ("dqat64/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define RESCALE32(ZM, XM, YM, C) \
  asm volatile ("dres32/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define RESCALE64(ZM, XM, YM, C) \
  asm volatile ("dres64/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define SAMEQUANTUM32(ZM, XM, YM) \
  asm volatile ("dsameq32/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define SAMEQUANTUM64(ZM, XM, YM) \
  asm volatile ("dsameq64/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define SQUAREROOT32(ZM, XM, C) \
  asm volatile ("dsqrt32/a  %0, %1"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a)   \
               )
#define SQUAREROOT64(ZM, XM, C) \
  asm volatile ("dsqrt64/a  %0, %1"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a) \
               )
#define SUBTRACT32(ZM, XM, YM, C)   \
  asm volatile ("dsub32/a  %0, %1, %2"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a), "f"((YM).a)   \
               )
#define SUBTRACT64(ZM, XM, YM, C)   \
  asm volatile ("dsub64/a  %0, %1, %2"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a), "f"((YM).a) \
               )
#define TOINTEGRAL32(ZM, XM, C)   \
  asm volatile ("dtoint32/a  %0, %1"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a)   \
               )
#define TOINTEGRAL64(ZM, XM, C)   \
  asm volatile ("dtoint64/a  %0, %1"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a) \
               )
#define COPY32(ZM, XM)   \
  asm volatile ("dcpy32/a  %0, %1"        \
                : "=f"((ZM).a)                    \
                : "f"((XM).a)   \
               )
#define COPY64(ZM, XM)   \
  asm volatile ("dcpy64/a  %0, %1"        \
                : "=f"((ZM).a)   \
                : "f"((XM).a) \
               )
#define ZERO32(ZM) \
  asm volatile ("dzero32/a %0" \
                : "=f"((ZM).a)\
               )
#define ZERO64(ZM)\
  asm volatile ("dzero64/a %0" \
                : "=f"((ZM).a)\
              )
#define FROMSTRING32(ZM, R, C) \
  decimal32FromString(&(ZM).b, R, C)
#define FROMSTRING64(ZM, R, C) \
  decimal64FromString(&(ZM).b, R, C)
#define FROMINTEGER32(ZM, R, C)\
  asm volatile ("dfromint32/a %0, %1" \
                : "=f" ((ZM).a)\
                : "r" (R)\
               )
#define FROMINTEGER64(ZM, R, C)\
  asm volatile ("dfromint64/a %0, %1" \
                : "=f" ((ZM).a)\
                : "r" (R)\
               )
#define TRIM32(ZM) \
  asm volatile ("dtrim32/a %0" \
                : "=f" ((ZM).a)\
               )
#define TRIM64(ZM) \
  asm volatile ("dtrim64/a %0" \
                : "=f" ((ZM).a)\
               )
#define TOSTRING32(ZM, R) \
  decimal32ToString(&(ZM).b, R)
#define TOSTRING64(ZM, R) \
  decimal64ToString(&(ZM).b, R)
#define TOENGSTRING32(ZM, R) \
  decimal32ToEngString(&(ZM).b, R);
#define TOENGSTRING64(ZM, R) \
  decimal64ToEngString(&(ZM).b, R)
#define ISNEGATIVE32(ZM) \
  ((((ZM).b.bytes[DECIMAL32_Bytes-1])&(0x80))==0x80)
#define ISNEGATIVE64(ZM) \
  ((((ZM).b.bytes[DECIMAL64_Bytes-1])&(0x80))==0x80)
#define ISZERO32(ZM) \
 (((((ZM).b.bytes[DECIMAL32_Bytes-1])&(0x60))!=0x60)&& \
 ((((ZM).b.bytes[DECIMAL32_Bytes-1])&(0x1c))==0) && \
 ((((ZM).b.bytes[DECIMAL32_Bytes-2])&(0x0f))==0) && \
 ((((ZM).b.bytes[DECIMAL32_Bytes-3])&(0xff))==0) && \
 ((((ZM).b.bytes[DECIMAL32_Bytes-4])&(0xff))==0))
#define ISZERO64(ZM) \
 (((((ZM).b.bytes[DECIMAL64_Bytes-1])&(0x60))!=0x60)&& \
 ((((ZM).b.bytes[DECIMAL64_Bytes-1])&(0x1c))==0) && \
 ((((ZM).b.bytes[DECIMAL64_Bytes-2])&(0x03))==0) && \
 ((((ZM).b.bytes[DECIMAL64_Bytes-3])&(0xff))==0) && \
 ((((ZM).b.bytes[DECIMAL64_Bytes-4])&(0xff))==0) && \
 ((((ZM).b.bytes[DECIMAL64_Bytes-5])&(0xff))==0) && \
 ((((ZM).b.bytes[DECIMAL64_Bytes-6])&(0xff))==0) && \
 ((((ZM).b.bytes[DECIMAL64_Bytes-7])&(0xff))==0) && \
 ((((ZM).b.bytes[DECIMAL64_Bytes-8])&(0xff))==0))
#define ISPOSITIVE32(ZM)       (ISNEGATIVE32((ZM))==0)
#define ISPOSITIVE64(ZM)       (ISNEGATIVE64((ZM))==0)
#define ISINFINITE32(ZM) ((((ZM).b.bytes[DECIMAL32_Bytes-1])&(0x78))==0x78)
#define ISINFINITE64(ZM) ((((ZM).b.bytes[DECIMAL64_Bytes-1])&(0x78))==0x78)
#define ISNAN32(ZM)      ((((ZM).b.bytes[DECIMAL32_Bytes-1])&(0x7c))==0x7c)
#define ISNAN64(ZM)      ((((ZM).b.bytes[DECIMAL64_Bytes-1])&(0x7c))==0x7c)
#define ISNORMAL32(ZM)  (ISNAN32(ZM)==0)&&(ISINFINITE32(ZM)==0)
#define ISNORMAL64(ZM)  (ISNAN64(ZM)==0)&&(ISINFINITE64(ZM)==0)
#else  //Software Support
//NOTE: in order to make the runtime for software fcn call similar to the 
//hardware fcn call, we add several dummy IEEE->decNum and decNum->IEEE fcn 
//call for those op that are not supported in hardware.
#define ABS64(Z, X, C)       ABS(&Z, &X, C)
#define ADD64(Z, X, Y, C)    ADD(&Z, &X, &Y, C)
#define COMPARE64(Z, X, Y, C)   COMPARE(&Z, &X, &Y, C)
#define EQ64(Z, X, Y, C)        EQ(&Z, &X, &Y, C)
#define GT64(Z, X, Y, C)        GT(&Z, &X, &Y, C)
#define LT64(Z, X, Y, C)        LT(&Z, &X, &Y, C)
#define GE64(Z, X, Y, C)        GE(&Z, &X, &Y, C)
#define LE64(Z, X, Y, C)        LE(&Z, &X, &Y, C)
#define NE64(Z, X, Y, C)        NE(&Z, &X, &Y, C)
#define COMPARETOTAL64(Z, X, Y, C) COMPARETOTAL(&Z, &X, &Y, C)
#define DIVIDE64(Z, X, Y, C)  DIVIDE(&Z, &X, &Y, C)
#define DIVIDEINTEGER64(Z, X, Y, C) DIVIDEINTEGER(&Z, &X, &Y, C)
#define EXP64(Z, X, C) EXP(&Z, &X, C)
#define LN64(Z, X, C) LN(&Z, &X, C)
#define LOG1064(Z, X, C) LOG10(&Z, &X, C)
#define MAX64(Z, X, Y, C) MAX(&Z, &X, &Y, C)
#define MIN64(Z, X, Y, C) MIN(&Z, &X, &Y, C)
#define MINUS64(Z, X, C)  MINUS(&Z, &X, C)
#define MULTIPLY64(Z, X, Y, C) MULTIPLY(&Z, &X, &Y, C)
#define NORMALIZE64(Z, X, C) NORMALIZE(&Z, &X, C)
#define PLUS64(Z, X, C) PLUS(&Z, &X, C)
#define POWER64(Z, X, Y, C)   POWER(&Z, &X, &Y, C)
#define QUANTIZE64(Z, X, Y, C)  QUANTIZE(&Z, &X, &Y, C) 
#define RESCALE64(Z, X, Y, C) RESCALE(&Z, &X, &Y, C)
#define REMAINDER64(Z, X, Y, C) REMAINDER(&Z, &X, &Y, C)
#define SAMEQUANTUM64(Z, X, Y) SAMEQUANTUM(&Z, &X, &Y)
#define SQUAREROOT64(Z, X, C) SQUAREROOT(&Z, &X, C)
#define SUBTRACT64(Z, X, Y, C) SUBTRACT(&Z, &X, &Y, C)
#define TOINTEGRAL64(Z, X, C) TOINTEGRAL(&Z, &X, C)
#define COPY64(Z, X) COPY(&Z, &X)
#define ZERO64(Z) ZERO(&Z)
#define FROMSTRING64(Z, R, C)    FROMSTRING(&Z, R, C)
#define FROMINTEGER64(Z, R, C)     FROMINTEGER(&Z, R, C)
#define TOSTRING64(Z, R)      TOSTRING(&Z, R)
#define TOENGSTRING64(Z, R)   TOENGSTRING(&Z, R)
#define ISNEGATIVE64(Z)       ISNEGATIVE(&Z)
#define ISZERO64(Z)     ISZERO(&Z)
#define ISPOSITIVE64(Z) ((ISZERO64(Z)==0)&&(ISNEGATIVE64(Z)==0))
#define ISINFINITE64(Z) ISINFINITE(&Z)
#define ISNAN64(Z)  ISNAN(&Z)
#define ISNORMAL64(ZM)  (ISNAN64(ZM)==0)&&(ISINFINITE64(ZM)==0)
#define TRIM64(Z)  TRIM(&Z)
#endif //end of !HW

#endif //end of !DECASM_H
