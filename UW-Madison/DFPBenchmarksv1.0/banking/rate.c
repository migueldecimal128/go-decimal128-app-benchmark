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
// =============================================================================
// VERSION AND CVS INFO
// Id: $Id: rate.c,v 1.13 2009-05-18 03:07:53 mjanders Exp $
// Date: $Date: 2009-05-18 03:07:53 $
// Author: $Author: mjanders $
// =============================================================================

//----------------------------------------
// rate.c
// Currency Conversion Microbenchmark
// Decimal Floating-Point Benchmark
// header file to generate/access/update/close all the rate tables
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.13 $

#include "const.h"
#include "rate.h"
#include "rdtsc.h"

/////////////////////////////////////////////////////
//GLOBAL EXTERNAL VARIABLES
//The constants used in the microbenchmark
// M* means "minus *". E.g. MTWO = -2
//////////////////////////////////////////////////////
extern dec_t CHECKING_RATE, CENT;
extern int counter;
extern context_t *setp;

#if defined(NATIVE_GCC_BID)
extern _IDEC_flags flags;
#endif

// COUNTING MICHAEL!!!
extern unsigned long long add_cycles;
extern unsigned long long mul_cycles;
extern unsigned long long quant_cycles;
extern unsigned long long zero_cycles;
extern unsigned long long div_cycles;
extern unsigned long long sqrt_cycles;
extern unsigned long long sub_cycles;
extern unsigned long long copy_cycles;
extern unsigned long long int_cycles;
extern unsigned long long max_cycles;
extern unsigned long long minus_cycles;

extern unsigned long long add_calls;
extern unsigned long long mul_calls;
extern unsigned long long quant_calls;
extern unsigned long long zero_calls;
extern unsigned long long div_calls;
extern unsigned long long sqrt_calls;
extern unsigned long long sub_calls;
extern unsigned long long copy_calls;
extern unsigned long long int_calls;
extern unsigned long long max_calls;
extern unsigned long long minus_calls;

extern unsigned long long add_rounded;
extern unsigned long long int_rounded;
extern unsigned long long mul_rounded;
extern unsigned long long rsc_rounded;
extern unsigned long long div_rounded;

extern unsigned long long count_begin_func;
extern unsigned long long count_end_func;
extern unsigned long long count_overhead;
// END COUNTING MICHAEL!!!



void initCoin(dec_t **conversion_rate){
  COPY64(conversion_rate[0][1], CENT);
  COPY64(conversion_rate[1][1], CENT);
  COPY64(conversion_rate[2][1], CENT);
  COPY64(conversion_rate[3][1], CENT);
  COPY64(conversion_rate[4][1], CENT);
  COPY64(conversion_rate[5][1], CENT);
  COPY64(conversion_rate[6][1], CENT);
  COPY64(conversion_rate[7][1], CENT);
  COPY64(conversion_rate[8][1], CENT);
  FROMSTRING64(conversion_rate[9][1], AUD, setp);
  FROMSTRING64(conversion_rate[10][1], CNY, setp);
  FROMSTRING64(conversion_rate[11][1], EUR, setp);
  FROMSTRING64(conversion_rate[12][1], GBP, setp);
  FROMSTRING64(conversion_rate[13][1], HKD, setp);
  FROMSTRING64(conversion_rate[14][1], JPY, setp);
  FROMSTRING64(conversion_rate[15][1], NZD, setp);
  FROMSTRING64(conversion_rate[16][1], SGD, setp);
  FROMSTRING64(conversion_rate[17][1], TWD, setp);
}


void openConversionRateFile(FILE **dp){
#if defined(BID) || defined(NATIVE_GCC_BID)
 #if defined(WIDTH64)
  FILEOPEN(dp[ 0], "datafile/bin/AUD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 1], "datafile/bin/CNY_USD.txt.bid", "rb");
  FILEOPEN(dp[ 2], "datafile/bin/EUR_USD.txt.bid", "rb");
  FILEOPEN(dp[ 3], "datafile/bin/GBP_USD.txt.bid", "rb");
  FILEOPEN(dp[ 4], "datafile/bin/HKD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 5], "datafile/bin/JPY_USD.txt.bid", "rb");
  FILEOPEN(dp[ 6], "datafile/bin/NZD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 7], "datafile/bin/SGD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 8], "datafile/bin/TWD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 9], "datafile/bin/USD_AUD.txt.bid", "rb");
  FILEOPEN(dp[10], "datafile/bin/USD_CNY.txt.bid", "rb");
  FILEOPEN(dp[11], "datafile/bin/USD_EUR.txt.bid", "rb");
  FILEOPEN(dp[12], "datafile/bin/USD_GBP.txt.bid", "rb");
  FILEOPEN(dp[13], "datafile/bin/USD_HKD.txt.bid", "rb");
  FILEOPEN(dp[14], "datafile/bin/USD_JPY.txt.bid", "rb");
  FILEOPEN(dp[15], "datafile/bin/USD_NZD.txt.bid", "rb");
  FILEOPEN(dp[16], "datafile/bin/USD_SGD.txt.bid", "rb");
  FILEOPEN(dp[17], "datafile/bin/USD_TWD.txt.bid", "rb");
 #else
  FILEOPEN(dp[ 0], "datafile/bin/AUD_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 1], "datafile/bin/CNY_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 2], "datafile/bin/EUR_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 3], "datafile/bin/GBP_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 4], "datafile/bin/HKD_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 5], "datafile/bin/JPY_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 6], "datafile/bin/NZD_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 7], "datafile/bin/SGD_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 8], "datafile/bin/TWD_USD.txt.bid128", "rb");
  FILEOPEN(dp[ 9], "datafile/bin/USD_AUD.txt.bid128", "rb");
  FILEOPEN(dp[10], "datafile/bin/USD_CNY.txt.bid128", "rb");
  FILEOPEN(dp[11], "datafile/bin/USD_EUR.txt.bid128", "rb");
  FILEOPEN(dp[12], "datafile/bin/USD_GBP.txt.bid128", "rb");
  FILEOPEN(dp[13], "datafile/bin/USD_HKD.txt.bid128", "rb");
  FILEOPEN(dp[14], "datafile/bin/USD_JPY.txt.bid128", "rb");
  FILEOPEN(dp[15], "datafile/bin/USD_NZD.txt.bid128", "rb");
  FILEOPEN(dp[16], "datafile/bin/USD_SGD.txt.bid128", "rb");
  FILEOPEN(dp[17], "datafile/bin/USD_TWD.txt.bid128", "rb");
 #endif // WIDTH
#else
 #if defined(WIDTH64)
  FILEOPEN(dp[ 0], "datafile/bin/AUD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 1], "datafile/bin/CNY_USD.txt.b64", "rb");
  FILEOPEN(dp[ 2], "datafile/bin/EUR_USD.txt.b64", "rb");
  FILEOPEN(dp[ 3], "datafile/bin/GBP_USD.txt.b64", "rb");
  FILEOPEN(dp[ 4], "datafile/bin/HKD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 5], "datafile/bin/JPY_USD.txt.b64", "rb");
  FILEOPEN(dp[ 6], "datafile/bin/NZD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 7], "datafile/bin/SGD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 8], "datafile/bin/TWD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 9], "datafile/bin/USD_AUD.txt.b64", "rb");
  FILEOPEN(dp[10], "datafile/bin/USD_CNY.txt.b64", "rb");
  FILEOPEN(dp[11], "datafile/bin/USD_EUR.txt.b64", "rb");
  FILEOPEN(dp[12], "datafile/bin/USD_GBP.txt.b64", "rb");
  FILEOPEN(dp[13], "datafile/bin/USD_HKD.txt.b64", "rb");
  FILEOPEN(dp[14], "datafile/bin/USD_JPY.txt.b64", "rb");
  FILEOPEN(dp[15], "datafile/bin/USD_NZD.txt.b64", "rb");
  FILEOPEN(dp[16], "datafile/bin/USD_SGD.txt.b64", "rb");
  FILEOPEN(dp[17], "datafile/bin/USD_TWD.txt.b64", "rb");
 #else
  FILEOPEN(dp[ 0], "datafile/bin/AUD_USD.txt.b128", "rb");
  FILEOPEN(dp[ 1], "datafile/bin/CNY_USD.txt.b128", "rb");
  FILEOPEN(dp[ 2], "datafile/bin/EUR_USD.txt.b128", "rb");
  FILEOPEN(dp[ 3], "datafile/bin/GBP_USD.txt.b128", "rb");
  FILEOPEN(dp[ 4], "datafile/bin/HKD_USD.txt.b128", "rb");
  FILEOPEN(dp[ 5], "datafile/bin/JPY_USD.txt.b128", "rb");
  FILEOPEN(dp[ 6], "datafile/bin/NZD_USD.txt.b128", "rb");
  FILEOPEN(dp[ 7], "datafile/bin/SGD_USD.txt.b128", "rb");
  FILEOPEN(dp[ 8], "datafile/bin/TWD_USD.txt.b128", "rb");
  FILEOPEN(dp[ 9], "datafile/bin/USD_AUD.txt.b128", "rb");
  FILEOPEN(dp[10], "datafile/bin/USD_CNY.txt.b128", "rb");
  FILEOPEN(dp[11], "datafile/bin/USD_EUR.txt.b128", "rb");
  FILEOPEN(dp[12], "datafile/bin/USD_GBP.txt.b128", "rb");
  FILEOPEN(dp[13], "datafile/bin/USD_HKD.txt.b128", "rb");
  FILEOPEN(dp[14], "datafile/bin/USD_JPY.txt.b128", "rb");
  FILEOPEN(dp[15], "datafile/bin/USD_NZD.txt.b128", "rb");
  FILEOPEN(dp[16], "datafile/bin/USD_SGD.txt.b128", "rb");
  FILEOPEN(dp[17], "datafile/bin/USD_TWD.txt.b128", "rb");
 #endif
#endif
}

void openDepositRateFile(FILE **dp){
#if defined(BID) || defined(NATIVE_GCC_BID)
 #if defined(WIDTH64)
  FILEOPEN(dp[ 0], "datafile/bin/AUD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 1], "datafile/bin/CNY_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 2], "datafile/bin/EUR_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 3], "datafile/bin/GBP_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 4], "datafile/bin/HKD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 5], "datafile/bin/JPY_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 6], "datafile/bin/NZD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 7], "datafile/bin/SGD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 8], "datafile/bin/TWD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 9], "datafile/bin/USD_Interest_Rate.txt.bid", "rb");
 #else
  FILEOPEN(dp[ 0], "datafile/bin/AUD_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 1], "datafile/bin/CNY_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 2], "datafile/bin/EUR_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 3], "datafile/bin/GBP_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 4], "datafile/bin/HKD_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 5], "datafile/bin/JPY_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 6], "datafile/bin/NZD_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 7], "datafile/bin/SGD_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 8], "datafile/bin/TWD_Interest_Rate.txt.bid128", "rb");
  FILEOPEN(dp[ 9], "datafile/bin/USD_Interest_Rate.txt.bid128", "rb");
 #endif //WIDTH
#else
 #if defined(WIDTH64)
  FILEOPEN(dp[ 0], "datafile/bin/AUD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 1], "datafile/bin/CNY_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 2], "datafile/bin/EUR_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 3], "datafile/bin/GBP_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 4], "datafile/bin/HKD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 5], "datafile/bin/JPY_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 6], "datafile/bin/NZD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 7], "datafile/bin/SGD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 8], "datafile/bin/TWD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 9], "datafile/bin/USD_Interest_Rate.txt.b64", "rb");
 #else 
  FILEOPEN(dp[ 0], "datafile/bin/AUD_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 1], "datafile/bin/CNY_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 2], "datafile/bin/EUR_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 3], "datafile/bin/GBP_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 4], "datafile/bin/HKD_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 5], "datafile/bin/JPY_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 6], "datafile/bin/NZD_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 7], "datafile/bin/SGD_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 8], "datafile/bin/TWD_Interest_Rate.txt.b128", "rb");
  FILEOPEN(dp[ 9], "datafile/bin/USD_Interest_Rate.txt.b128", "rb");
 #endif //WIDTH
#endif
}

void openMortgageRateFile(FILE **dp){
#if defined(BID) || defined(NATIVE_GCC_BID)
 #if defined(WIDTH64)
  FILEOPEN(dp[FRM15YEAR-MORTGAGE], "datafile/bin/15YearFRM.txt.bid", "rb");
  FILEOPEN(dp[FRM30YEAR-MORTGAGE], "datafile/bin/30YearFRM.txt.bid", "rb");
  FILEOPEN(dp[ARM1YEAR-MORTGAGE],  "datafile/bin/1YearARM.txt.bid", "rb");
  FILEOPEN(dp[ARM51YEAR-MORTGAGE], "datafile/bin/51ARM.txt.bid", "rb");
 #else
  FILEOPEN(dp[FRM15YEAR-MORTGAGE], "datafile/bin/15YearFRM.txt.bid128", "rb");
  FILEOPEN(dp[FRM30YEAR-MORTGAGE], "datafile/bin/30YearFRM.txt.bid128", "rb");
  FILEOPEN(dp[ARM1YEAR-MORTGAGE],  "datafile/bin/1YearARM.txt.bid128", "rb");
  FILEOPEN(dp[ARM51YEAR-MORTGAGE], "datafile/bin/51ARM.txt.bid128", "rb");
 #endif // WIDTH
#else
 #if defined(WIDTH64)
  FILEOPEN(dp[FRM15YEAR-MORTGAGE], "datafile/bin/15YearFRM.txt.b64", "rb");
  FILEOPEN(dp[FRM30YEAR-MORTGAGE], "datafile/bin/30YearFRM.txt.b64", "rb");
  FILEOPEN(dp[ARM1YEAR-MORTGAGE],  "datafile/bin/1YearARM.txt.b64", "rb");
  FILEOPEN(dp[ARM51YEAR-MORTGAGE], "datafile/bin/51ARM.txt.b64", "rb");
 #else
  FILEOPEN(dp[FRM15YEAR-MORTGAGE], "datafile/bin/15YearFRM.txt.b128", "rb");
  FILEOPEN(dp[FRM30YEAR-MORTGAGE], "datafile/bin/30YearFRM.txt.b128", "rb");
  FILEOPEN(dp[ARM1YEAR-MORTGAGE],  "datafile/bin/1YearARM.txt.b128", "rb");
  FILEOPEN(dp[ARM51YEAR-MORTGAGE], "datafile/bin/51ARM.txt.b128", "rb");
 #endif //WIDTH
#endif
}

void updateConversionTable(dec_t **conversion_rate, FILE **dp){
  int size;
  int i;
  for (i=0; i<COUNTRIES*2; i++) {
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
    decimal64 temp;
    if ((size = myfread(&temp, sizeof(decimal64), 1, dp[i]))!=0){
      if (size==1){
	decimal64ToNumber(&temp, &conversion_rate[i][0]);
	}
 #else
    decimal128 temp;
    if ((size = myfread(&temp, sizeof(decimal128), 1, dp[i]))!=0){
      if (size==1){
	decimal128ToNumber(&temp, &conversion_rate[i][0]);
	}
 #endif // WIDTH64
#else
    dec_t num;
    if ((size = myfread(&num, sizeof(dec_t), 1, dp[i]))!=0){
      if (size==1) {
        COPY64(conversion_rate[i][0], num);
      }
#endif
      else {
        myfprintf(stderr, "[CONV TABLE]size written in is %d\n", size);
        myexit(-1);
      }
    }
    else{
      errorPrint(LOC, INFO_CODE, INFO_CODE,
		 stderr, "Use the old conversion rate\n");
    }
  }
}
void updateDepositRateTable(deposit_rate_t *cur_rate, FILE **dp){
  int size;
  int i;
  for (i=0; i<COUNTRIES+1; i++) {
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
    decimal64 rate[3];
    if ((size = myfread(&rate, sizeof(decimal64), 3, dp[i]))!=0){
 #else
    decimal128 rate[3];
    if ((size = myfread(&rate, sizeof(decimal128), 3, dp[i]))!=0){
 #endif //WIDTH64
#else
    dec_t rate[3];      
    if ((size = myfread(&rate, sizeof(dec_t), 3, dp[i]))!=0){
#endif
      if (size!=3){
        myprintf("size read in is %d\n", size);
        myexit(-1);
      }
      COPY64((cur_rate[i].checking), CHECKING_RATE);
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
      decimal64ToNumber(&rate[0], &(cur_rate[i].one));
      decimal64ToNumber(&rate[1], &(cur_rate[i].three));
      decimal64ToNumber(&rate[2], &(cur_rate[i].six));
 #else 
      decimal128ToNumber(&rate[0], &(cur_rate[i].one));
      decimal128ToNumber(&rate[1], &(cur_rate[i].three));
      decimal128ToNumber(&rate[2], &(cur_rate[i].six));
 #endif
#else
      COPY64((cur_rate[i].one), rate[0]);
      COPY64((cur_rate[i].three), rate[1]);
      COPY64((cur_rate[i].six), rate[2]);
#endif

    }
  }
}
int updateMortgageRate(FILE **dp, mort_rate_t *mort_rate){
  int size;
  if (counter%7==0){
    //15YearFRM.txt
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
    decimal64 num[3];
    if ((size = myfread(num, sizeof(decimal64), 2, dp[FRM15YEAR-MORTGAGE]))!=0){
 #else
    decimal128 num[3];
    if ((size = myfread(num, sizeof(decimal128), 2, dp[FRM15YEAR-MORTGAGE]))!=0){
 #endif //WIDTH64
#else
    dec_t num[3];
    if ((size = myfread(num, sizeof(dec_t), 2, dp[FRM15YEAR-MORTGAGE]))!=0){
#endif
      if (size==0){
        errorPrint(LOC, INFO_CODE, INFO_CODE,
                   stderr, "Use the old 15-year FRM rate\n");
      }
      else if (size==2){
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
	decimal64ToNumber(&num[0], &(mort_rate->frm15));
	decimal64ToNumber(&num[1], &(mort_rate->frm15Point));
 #else
	decimal128ToNumber(&num[0], &(mort_rate->frm15));
	decimal128ToNumber(&num[1], &(mort_rate->frm15Point));
 #endif //WIDTH64
#else
        COPY64(mort_rate->frm15, num[0]);
        COPY64(mort_rate->frm15Point, num[1]);
#endif
      }
      else {
        myfprintf(stderr, "[MORT RATE1]size written in is %d\n", size);
        myexit(-1);
      }
    }
    //30YearFRM.txt
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
    if ((size = myfread(num, sizeof(decimal64), 2, dp[FRM30YEAR-MORTGAGE]))!=0){
 #else
    if ((size = myfread(num, sizeof(decimal128), 2, dp[FRM30YEAR-MORTGAGE]))!=0){
 #endif
#else
    if ((size = myfread(num, sizeof(dec_t), 2, dp[FRM30YEAR-MORTGAGE]))!=0){
#endif
      if (size==0){
        errorPrint(LOC, INFO_CODE, INFO_CODE,
                   stderr, "Use the old 30-year FRM rate\n");
      }
      else if (size==2){
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
	decimal64ToNumber(&num[0], &(mort_rate->frm30));
	decimal64ToNumber(&num[1], &(mort_rate->frm30Point));
 #else
	decimal128ToNumber(&num[0], &(mort_rate->frm30));
	decimal128ToNumber(&num[1], &(mort_rate->frm30Point));
 #endif // WIDTH64
#else
        COPY64(mort_rate->frm30, num[0]);
        COPY64(mort_rate->frm30Point, num[1]);
#endif
      }
      else {
        myfprintf(stderr, "[MORT RATE2]size written in is %d\n", size);
        myexit(-1);
      }
    }
    //1YearARM.txt
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
    if ((size = myfread(num, sizeof(decimal64), 3, dp[ARM1YEAR-MORTGAGE]))!=0){
 #else
    if ((size = myfread(num, sizeof(decimal128), 3, dp[ARM1YEAR-MORTGAGE]))!=0){
 #endif
#else
    if ((size = myfread(num, sizeof(dec_t), 3, dp[ARM1YEAR-MORTGAGE]))!=0){
#endif
      if (size==0){
        errorPrint(LOC, INFO_CODE, INFO_CODE,
                   stderr, "Use the old 1-year ARM rate\n");
      }
      else if (size==3){
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
	decimal64ToNumber(&num[0], &(mort_rate->arm1));
	decimal64ToNumber(&num[1], &(mort_rate->arm1Point));
	decimal64ToNumber(&num[2], &(mort_rate->arm1Margin));
 #else
	decimal128ToNumber(&num[0], &(mort_rate->arm1));
	decimal128ToNumber(&num[1], &(mort_rate->arm1Point));
	decimal128ToNumber(&num[2], &(mort_rate->arm1Margin));
 #endif //WIDTH64
#else
        COPY64(mort_rate->arm1, num[0]);
        COPY64(mort_rate->arm1Point, num[1]);
        COPY64(mort_rate->arm1Margin, num[2]);
#endif
      }
      else {
        myfprintf(stderr, "[MORT RATE3]size written in is %d\n", size);
        myexit(-1);
      }
    }
    //51YearARM.txt
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
    if ((size = myfread(num, sizeof(decimal64), 3, dp[ARM51YEAR-MORTGAGE]))!=0){
 #else
    if ((size = myfread(num, sizeof(decimal128), 3, dp[ARM51YEAR-MORTGAGE]))!=0){
 #endif
#else
    if ((size = myfread(num, sizeof(dec_t), 3, dp[ARM51YEAR-MORTGAGE]))!=0){
#endif
      if (size==0){
        errorPrint(LOC, INFO_CODE, INFO_CODE,
                   stderr, "Use the old 5/1-year ARM rate\n");
      }
      else if (size==3){
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
	decimal64ToNumber(&num[0], &(mort_rate->arm51));
	decimal64ToNumber(&num[1], &(mort_rate->arm51Point));
	decimal64ToNumber(&num[2], &(mort_rate->arm51Margin));
 #else
	decimal128ToNumber(&num[0], &(mort_rate->arm51));
	decimal128ToNumber(&num[1], &(mort_rate->arm51Point));
	decimal128ToNumber(&num[2], &(mort_rate->arm51Margin));
 #endif //WIDTH64
#else
        COPY64(mort_rate->arm51, num[0]);
        COPY64(mort_rate->arm51Point, num[1]);
        COPY64(mort_rate->arm51Margin, num[2]);
#endif
      }
      else {
        myfprintf(stderr, "[MORT RATE4]size written in is %d\n", size);
        myexit(-1);
      }
    }
  }
  return 0;
}
int updateTCMRateFile(dec_t *tcm, FILE *dp){
  int size;
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
  decimal64 num;
  if ((size = myfread(&num, sizeof(decimal64), 1, dp))!=0){
    if (size==1)
      decimal64ToNumber(&num, tcm);
 #else
  decimal128 num;
  if ((size = myfread(&num, sizeof(decimal128), 1, dp))!=0){
    if (size==1)
      decimal128ToNumber(&num, tcm);
 #endif // WIDTH64
#else
  dec_t num;
  if ((size = myfread(&num, sizeof(dec_t), 1, dp))!=0){
    if (size==1) {
      COPY64(*tcm, num);
    }
#endif
    else {
      myfprintf(stderr, "[TCM RATE]size written in is %d\n", size);
      myexit(-1);
    }
  }
  else{
    errorPrint(LOC, INFO_CODE, INFO_CODE,
               stderr, "Use the old TCM rate\n");
  }
  return 0;
}

int updatePrimeRateFile(dec_t *prime, FILE *dp){
  int size;
  //dec_t num, comp;
  dec_t num;
//#ifndef HW
//#if !defined(HW) && !defined(DECNUMBER_FIXED)
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
  decimal64 temp;
  if ((size = myfread(&temp, sizeof(decimal64), 1, dp))!=0){
    if (size==1){
      decimal64ToNumber(&temp, &num);
 #else
  decimal128 temp;
  if ((size = myfread(&temp, sizeof(decimal128), 1, dp))!=0){
    if (size==1){
      decimal128ToNumber(&temp, &num);
 #endif
#else
  if ((size = myfread(&num, sizeof(dec_t), 1, dp))!=0){
    if (size==1){
#endif
      int eq_res = 0;
      EQ64(eq_res, *prime, num, setp);
      if(eq_res) 
        return 0;
      else {
        COPY64(*prime, num);
        return 1;
      }
    }
    else {
      myfprintf(stderr, "[PRIME RATE]size written in is %d\n", size);
      myexit(-1);
    }
  }
  else{
    errorPrint(LOC, INFO_CODE, INFO_CODE,
               stderr, "Use the old Prime rate\n");
  }
  return 0;
}

void closeDepositRateFile(FILE **dp){
  myfclose(dp[0]);
  myfclose(dp[1]);
  myfclose(dp[2]);
  myfclose(dp[3]);
  myfclose(dp[4]);
  myfclose(dp[5]);
  myfclose(dp[6]);
  myfclose(dp[7]);
  myfclose(dp[8]);
  myfclose(dp[9]);
}

void closeConversionRateFile(FILE **dp){
  myfclose(dp[0]);
  myfclose(dp[1]);
  myfclose(dp[2]);
  myfclose(dp[3]);
  myfclose(dp[4]);
  myfclose(dp[5]);
  myfclose(dp[6]);
  myfclose(dp[7]);
  myfclose(dp[8]);
  myfclose(dp[9]);
  myfclose(dp[10]);
  myfclose(dp[11]);
  myfclose(dp[12]);
  myfclose(dp[13]);
  myfclose(dp[14]);
  myfclose(dp[15]);
  myfclose(dp[16]);
  myfclose(dp[17]);
}

void closeMortgageRateFile(FILE **dp){
  myfclose(dp[FRM15YEAR-MORTGAGE]);
  myfclose(dp[FRM30YEAR-MORTGAGE]);
  myfclose(dp[ARM1YEAR-MORTGAGE]);
  myfclose(dp[ARM51YEAR-MORTGAGE]);
}

void printConversionRateTable(dec_t **conversion_rate, FILE *file_ptr){
  int i;
  char str1[LENGTH_T];
  char input_str[STR_MAX_LENGTH];
  for (i=0; i<COUNTRIES; i++){
    checkName(str1, i);
    mysprintf(input_str, "Rate from %s to USD ", str1);
    errorPrint(LOC, INFO_CODE, INFO_CODE, file_ptr, input_str);
    TOSTRING64(conversion_rate[i][0], str1);
    mysprintf(input_str, "%s\n", str1);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, file_ptr, input_str);
    checkName(str1, i);
    mysprintf(input_str, "Rate from USD to %s: ", str1);
    errorPrint(LOC, INFO_CODE, INFO_CODE, file_ptr, input_str);
    TOSTRING64(conversion_rate[i+COUNTRIES][0], str1);
    mysprintf(input_str, "%s\n", str1);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, file_ptr, input_str);
  }
}

/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-05-18 03:07:53 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: rate.c,v $
  Revision 1.13  2009-05-18 03:07:53  mjanders
  Makefile

  Revision 1.12  2009-04-19 21:40:08  mjanders
  Updated decnumber and BID libraries

  Revision 1.11  2009-04-12 05:56:19  mjanders
  Added 128-bit mode

  Revision 1.10  2009-02-18 00:42:15  mjanders
  Fixed bug where _IDEC_glb_flags was used even with DECNUMBER* was defined

  Revision 1.9  2009-01-08 21:36:30  mjanders
  Cleanup

  Revision 1.8  2009/01/06 18:39:12  mjanders
  Added common header

  Revision 1.7  2009/01/02 23:53:27  mjanders
  Added native_gcc_bid mode and tested

  Revision 1.6  2008/11/25 06:16:58  mjanders
  Finished adding BID

  Revision 1.5  2008/11/23 08:14:17  mjanders
  Moved around the include files to make more sense. All include files for the libraries come in operator.h

  Revision 1.4  2008/11/21 04:34:39  mjanders
  Finished adding BID

  Revision 1.3  2008/05/15 04:53:45  mjanders
  Switched to new way of comparing numbers

  Revision 1.2  2008/04/24 21:39:08  mjanders
  Copied operator.h from other benchmarks so it will be the same format.
  Fixed MINUS() to only have 3 parameters rather than 4. We should fix
  this for euro and the others.

  Revision 1.1.1.1  2008/04/18 01:13:21  mjanders


  Revision 1.8  2007/03/18 03:18:19  lwang
  major change on the database structure and tests

  Revision 1.7  2007/03/06 18:21:04  lwang
  CHECKIN AGAIN

  Revision 1.5  2006/11/30 15:01:20  lwang
  Clecn up several bug in mortgage and rate file

  Revision 1.4  2006/11/17 02:20:00  lwang
  Credit Card benchmark added

  Revision 1.3  2006/11/07 03:34:01  lwang
  Minor revision before spliting CD and Checking and before adding Credit Card account

  Revision 1.2  2006/11/03 02:20:54  lwang
  minor revision

  Revision 1.1  2006/10/31 11:44:09  lwang
  tables.*->rate.*

  Revision 1.1  2006/10/28 01:54:18  lwang
  Banking System Initial Check-in

  Revision 1.1  2006/10/19 02:33:25  lwang
  extended with currency conversion, and checking/CD acct

  Revision 1.1  2006/07/31 06:46:23  lwang
  initial check-in for the currency conversion benchmark.

*/
