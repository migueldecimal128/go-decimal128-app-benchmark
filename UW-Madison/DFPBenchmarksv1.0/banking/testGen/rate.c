//----------------------------------------
// rate.c
// Currency Conversion Microbenchmark
// Decimal Floating-Point Benchmark
// header file to generate/access/update/close all the rate tables
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.6 $

#include "dfp_includes.h"
#include "rate.h"
#include "operator.h"
/////////////////////////////////////////////////////
//GLOBAL EXTERNAL VARIABLES
//The constants used in the microbenchmark
// M* means "minus *". E.g. MTWO = -2
//////////////////////////////////////////////////////
extern dec64_t CHECKING_RATE, CENT;
extern int counter;
extern context_t *setp;

void initCoin(dec64_t **conversion_rate){
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
#if defined(BID)
  FILEOPEN(dp[ 0], "../datafile/bin/AUD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 1], "../datafile/bin/CNY_USD.txt.bid", "rb");
  FILEOPEN(dp[ 2], "../datafile/bin/EUR_USD.txt.bid", "rb");
  FILEOPEN(dp[ 3], "../datafile/bin/GBP_USD.txt.bid", "rb");
  FILEOPEN(dp[ 4], "../datafile/bin/HKD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 5], "../datafile/bin/JPY_USD.txt.bid", "rb");
  FILEOPEN(dp[ 6], "../datafile/bin/NZD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 7], "../datafile/bin/SGD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 8], "../datafile/bin/TWD_USD.txt.bid", "rb");
  FILEOPEN(dp[ 9], "../datafile/bin/USD_AUD.txt.bid", "rb");
  FILEOPEN(dp[10], "../datafile/bin/USD_CNY.txt.bid", "rb");
  FILEOPEN(dp[11], "../datafile/bin/USD_EUR.txt.bid", "rb");
  FILEOPEN(dp[12], "../datafile/bin/USD_GBP.txt.bid", "rb");
  FILEOPEN(dp[13], "../datafile/bin/USD_HKD.txt.bid", "rb");
  FILEOPEN(dp[14], "../datafile/bin/USD_JPY.txt.bid", "rb");
  FILEOPEN(dp[15], "../datafile/bin/USD_NZD.txt.bid", "rb");
  FILEOPEN(dp[16], "../datafile/bin/USD_SGD.txt.bid", "rb");
  FILEOPEN(dp[17], "../datafile/bin/USD_TWD.txt.bid", "rb");
#else
  FILEOPEN(dp[ 0], "../datafile/bin/AUD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 1], "../datafile/bin/CNY_USD.txt.b64", "rb");
  FILEOPEN(dp[ 2], "../datafile/bin/EUR_USD.txt.b64", "rb");
  FILEOPEN(dp[ 3], "../datafile/bin/GBP_USD.txt.b64", "rb");
  FILEOPEN(dp[ 4], "../datafile/bin/HKD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 5], "../datafile/bin/JPY_USD.txt.b64", "rb");
  FILEOPEN(dp[ 6], "../datafile/bin/NZD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 7], "../datafile/bin/SGD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 8], "../datafile/bin/TWD_USD.txt.b64", "rb");
  FILEOPEN(dp[ 9], "../datafile/bin/USD_AUD.txt.b64", "rb");
  FILEOPEN(dp[10], "../datafile/bin/USD_CNY.txt.b64", "rb");
  FILEOPEN(dp[11], "../datafile/bin/USD_EUR.txt.b64", "rb");
  FILEOPEN(dp[12], "../datafile/bin/USD_GBP.txt.b64", "rb");
  FILEOPEN(dp[13], "../datafile/bin/USD_HKD.txt.b64", "rb");
  FILEOPEN(dp[14], "../datafile/bin/USD_JPY.txt.b64", "rb");
  FILEOPEN(dp[15], "../datafile/bin/USD_NZD.txt.b64", "rb");
  FILEOPEN(dp[16], "../datafile/bin/USD_SGD.txt.b64", "rb");
  FILEOPEN(dp[17], "../datafile/bin/USD_TWD.txt.b64", "rb");
#endif
}


void openDepositRateFile(FILE **dp){
#if defined(BID)
  FILEOPEN(dp[ 0], "../datafile/bin/AUD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 1], "../datafile/bin/CNY_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 2], "../datafile/bin/EUR_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 3], "../datafile/bin/GBP_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 4], "../datafile/bin/HKD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 5], "../datafile/bin/JPY_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 6], "../datafile/bin/NZD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 7], "../datafile/bin/SGD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 8], "../datafile/bin/TWD_Interest_Rate.txt.bid", "rb");
  FILEOPEN(dp[ 9], "../datafile/bin/USD_Interest_Rate.txt.bid", "rb");
#else
  FILEOPEN(dp[ 0], "../datafile/bin/AUD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 1], "../datafile/bin/CNY_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 2], "../datafile/bin/EUR_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 3], "../datafile/bin/GBP_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 4], "../datafile/bin/HKD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 5], "../datafile/bin/JPY_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 6], "../datafile/bin/NZD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 7], "../datafile/bin/SGD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 8], "../datafile/bin/TWD_Interest_Rate.txt.b64", "rb");
  FILEOPEN(dp[ 9], "../datafile/bin/USD_Interest_Rate.txt.b64", "rb");
#endif
}

void openMortgageRateFile(FILE **dp){
#if defined(BID)
  FILEOPEN(dp[FRM15YEAR-MORTGAGE], "../datafile/bin/15YearFRM.txt.bid", "rb");
  FILEOPEN(dp[FRM30YEAR-MORTGAGE], "../datafile/bin/30YearFRM.txt.bid", "rb");
  FILEOPEN(dp[ARM1YEAR-MORTGAGE],  "../datafile/bin/1YearARM.txt.bid", "rb");
  FILEOPEN(dp[ARM51YEAR-MORTGAGE], "../datafile/bin/51ARM.txt.bid", "rb");
#else
  FILEOPEN(dp[FRM15YEAR-MORTGAGE], "../datafile/bin/15YearFRM.txt.b64", "rb");
  FILEOPEN(dp[FRM30YEAR-MORTGAGE], "../datafile/bin/30YearFRM.txt.b64", "rb");
  FILEOPEN(dp[ARM1YEAR-MORTGAGE],  "../datafile/bin/1YearARM.txt.b64", "rb");
  FILEOPEN(dp[ARM51YEAR-MORTGAGE], "../datafile/bin/51ARM.txt.b64", "rb");
#endif
}
void updateConversionTable(dec64_t **conversion_rate, FILE **dp){
  int size;
  int i;
  for (i=0; i<COUNTRIES*2; i++) {
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
    decimal64 temp;
    if ((size = fread(&temp, sizeof(decimal64), 1, dp[i]))!=0){
      if (size==1){
	decimal64ToNumber(&temp, &conversion_rate[i][0]);
      }
#else
    dec64_t num;
    if ((size = fread(&num, sizeof(dec64_t), 1, dp[i]))!=0){
      if (size==1)
        COPY64(conversion_rate[i][0], num);
#endif
      else {
        fprintf(stderr, "[CONV TABLE]size written in is %d\n", size);
        exit(-1);
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
#ifdef DECNUMBER_ARBITRARY
    decimal64 rate[3];
    if ((size = fread(&rate, sizeof(decimal64), 3, dp[i]))!=0){
#else
    dec64_t rate[3];      
    if ((size = fread(&rate, sizeof(dec64_t), 3, dp[i]))!=0){

#endif
      if (size!=3){
        printf("size read in is %d\n", size);
        exit(-1);
      }
      COPY64((cur_rate[i].checking), CHECKING_RATE);
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
      decimal64ToNumber(&rate[0], &(cur_rate[i].one));
      decimal64ToNumber(&rate[1], &(cur_rate[i].three));
      decimal64ToNumber(&rate[2], &(cur_rate[i].six));
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
#ifdef DECNUMBER_ARBITRARY 
    decimal64 num[3];
    if ((size = fread(num, sizeof(decimal64), 2, dp[FRM15YEAR-MORTGAGE]))!=0){
#else
    dec64_t num[3];
    if ((size = fread(num, sizeof(dec64_t), 2, dp[FRM15YEAR-MORTGAGE]))!=0){

#endif
      if (size==0){
        errorPrint(LOC, INFO_CODE, INFO_CODE,
                   stderr, "Use the old 15-year FRM rate\n");
      }
      else if (size==2){
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
	decimal64ToNumber(&num[0], &(mort_rate->frm15));
	decimal64ToNumber(&num[1], &(mort_rate->frm15Point));
#else
        COPY64(mort_rate->frm15, num[0]);
        COPY64(mort_rate->frm15Point, num[1]);
#endif
      }
      else {
        fprintf(stderr, "[MORT RATE1]size written in is %d\n", size);
        exit(-1);
      }
    }
    //30YearFRM.txt
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
    if ((size = fread(num, sizeof(decimal64), 2, dp[FRM30YEAR-MORTGAGE]))!=0){
#else
    if ((size = fread(num, sizeof(dec64_t), 2, dp[FRM30YEAR-MORTGAGE]))!=0){

#endif
      if (size==0){
        errorPrint(LOC, INFO_CODE, INFO_CODE,
                   stderr, "Use the old 30-year FRM rate\n");
      }
      else if (size==2){
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
	decimal64ToNumber(&num[0], &(mort_rate->frm30));
	decimal64ToNumber(&num[1], &(mort_rate->frm30Point));
#else
        COPY64(mort_rate->frm30, num[0]);
        COPY64(mort_rate->frm30Point, num[1]);
#endif
      }
      else {
        fprintf(stderr, "[MORT RATE2]size written in is %d\n", size);
        exit(-1);
      }
    }
    //1YearARM.txt
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
    if ((size = fread(num, sizeof(decimal64), 3, dp[ARM1YEAR-MORTGAGE]))!=0){
#else
    if ((size = fread(num, sizeof(dec64_t), 3, dp[ARM1YEAR-MORTGAGE]))!=0){
#endif
      if (size==0){
        errorPrint(LOC, INFO_CODE, INFO_CODE,
                   stderr, "Use the old 1-year ARM rate\n");
      }
      else if (size==3){
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
	decimal64ToNumber(&num[0], &(mort_rate->arm1));
	decimal64ToNumber(&num[1], &(mort_rate->arm1Point));
	decimal64ToNumber(&num[2], &(mort_rate->arm1Margin));
#else
        COPY64(mort_rate->arm1, num[0]);
        COPY64(mort_rate->arm1Point, num[1]);
        COPY64(mort_rate->arm1Margin, num[2]);
#endif
      }
      else {
        fprintf(stderr, "[MORT RATE3]size written in is %d\n", size);
        exit(-1);
      }
    }
    //51YearARM.txt
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
    if ((size = fread(num, sizeof(decimal64), 3, dp[ARM51YEAR-MORTGAGE]))!=0){
#else
    if ((size = fread(num, sizeof(dec64_t), 3, dp[ARM51YEAR-MORTGAGE]))!=0){
#endif
      if (size==0){
        errorPrint(LOC, INFO_CODE, INFO_CODE,
                   stderr, "Use the old 5/1-year ARM rate\n");
      }
      else if (size==3){
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
	decimal64ToNumber(&num[0], &(mort_rate->arm51));
	decimal64ToNumber(&num[1], &(mort_rate->arm51Point));
	decimal64ToNumber(&num[2], &(mort_rate->arm51Margin));
#else
        COPY64(mort_rate->arm51, num[0]);
        COPY64(mort_rate->arm51Point, num[1]);
        COPY64(mort_rate->arm51Margin, num[2]);
#endif
      }
      else {
        fprintf(stderr, "[MORT RATE4]size written in is %d\n", size);
        exit(-1);
      }
    }
  }
  return SUCCESS;
}
int updateTCMRateFile(dec64_t *tcm, FILE *dp){
  int size;
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
  decimal64 num;
  if ((size = fread(&num, sizeof(decimal64), 1, dp))!=0){
    if (size==1)
      decimal64ToNumber(&num, tcm);
#else
  dec64_t num;
  if ((size = fread(&num, sizeof(dec64_t), 1, dp))!=0){
    if (size==1)
      COPY64(*tcm, num);
#endif
    else {
      fprintf(stderr, "[TCM RATE]size written in is %d\n", size);
      exit(-1);
    }
  }
  else{
    errorPrint(LOC, INFO_CODE, INFO_CODE,
               stderr, "Use the old TCM rate\n");
  }
  return SUCCESS;
}

int updatePrimeRateFile(dec64_t *prime, FILE *dp){
  int size;
  dec64_t num; // comp;
  int mybool;
//#ifndef HW
#ifdef DECNUMBER_ARBITRARY
  decimal64 temp;
  if ((size = fread(&temp, sizeof(decimal64), 1, dp))!=0){
    if (size==1){
      decimal64ToNumber(&temp, &num);
#else
  if ((size = fread(&num, sizeof(dec64_t), 1, dp))!=0){
    if (size==1){
#endif
      //COMPARE64(comp, *prime, num, setp);
      EQ64(mybool, *prime, num, setp);
      if (mybool) //no change
        return SUCCESS;
      else {
        COPY64(*prime, num);
        return (!SUCCESS);
      }
    }
    else {
      fprintf(stderr, "[PRIME RATE]size written in is %d\n", size);
      exit(-1);
    }
  }
  else{
    errorPrint(LOC, INFO_CODE, INFO_CODE,
               stderr, "Use the old Prime rate\n");
  }
  return SUCCESS;
}

void closeDepositRateFile(FILE **dp){
  fclose(dp[0]);
  fclose(dp[1]);
  fclose(dp[2]);
  fclose(dp[3]);
  fclose(dp[4]);
  fclose(dp[5]);
  fclose(dp[6]);
  fclose(dp[7]);
  fclose(dp[8]);
  fclose(dp[9]);
}

void closeConversionRateFile(FILE **dp){
  fclose(dp[0]);
  fclose(dp[1]);
  fclose(dp[2]);
  fclose(dp[3]);
  fclose(dp[4]);
  fclose(dp[5]);
  fclose(dp[6]);
  fclose(dp[7]);
  fclose(dp[8]);
  fclose(dp[9]);
  fclose(dp[10]);
  fclose(dp[11]);
  fclose(dp[12]);
  fclose(dp[13]);
  fclose(dp[14]);
  fclose(dp[15]);
  fclose(dp[16]);
  fclose(dp[17]);
}

void closeMortgageRateFile(FILE **dp){
  fclose(dp[FRM15YEAR-MORTGAGE]);
  fclose(dp[FRM30YEAR-MORTGAGE]);
  fclose(dp[ARM1YEAR-MORTGAGE]);
  fclose(dp[ARM51YEAR-MORTGAGE]);
}

void printConversionRateTable(dec64_t **conversion_rate, FILE *file_ptr){
  int i;
  char str1[LENGTH64];
  char input_str[STR_MAX_LENGTH];
  for (i=0; i<COUNTRIES; i++){
    checkName(str1, i);
    sprintf(input_str, "Rate from %s to USD ", str1);
    errorPrint(LOC, INFO_CODE, INFO_CODE, file_ptr, input_str);
    TOSTRING64(conversion_rate[i][0], str1);
    sprintf(input_str, "%s\n", str1);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, file_ptr, input_str);
    checkName(str1, i);
    sprintf(input_str, "Rate from USD to %s: ", str1);
    errorPrint(LOC, INFO_CODE, INFO_CODE, file_ptr, input_str);
    TOSTRING64(conversion_rate[i+COUNTRIES][0], str1);
    sprintf(input_str, "%s\n", str1);
    errorPrint(LOC, INFO_CODE, INFO_CODE_NO_LINE, file_ptr, input_str);
  }
}

/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2009-04-12 05:53:19 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: rate.c,v $
  Revision 1.6  2009-04-12 05:53:19  mjanders
  Added 128-bit mode

  Revision 1.5  2008-11-25 06:12:23  mjanders
  Moved over to using BID binary input data

  Revision 1.4  2008/11/24 08:58:48  mjanders
  Added BID support

  Revision 1.3  2008/05/15 17:03:05  stsen
  compatibility for BID

  Revision 1.2  2008/05/14 05:44:28  stsen
  updates for BID (works for decNumber, but still not working for BID)

  Revision 1.1.1.1  2008/04/18 01:13:49  mjanders


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
