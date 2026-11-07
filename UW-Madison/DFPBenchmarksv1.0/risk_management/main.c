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

#include "const.h"
#include "dfp_includes.h"
#include "dfp_datatypes.h"
#include "operator.h"
#include "risk_const.h"
#include "decfcn.h"
#include "fcns.h"
#include "rdtsc.h"

context_t set;

#if defined(NATIVE_GCC_BID) // Special case because global flags conflict
_IDEC_flags flags = 0x00;      // working flags 
#endif

// COUNTING MICHAEL!!!
unsigned long long add_cycles = 0;
unsigned long long mul_cycles = 0;
unsigned long long quant_cycles = 0;
unsigned long long zero_cycles = 0;
unsigned long long div_cycles = 0;
unsigned long long sqrt_cycles = 0;
unsigned long long sub_cycles = 0;
unsigned long long copy_cycles = 0;

unsigned long long add_calls = 0;
unsigned long long mul_calls = 0;
unsigned long long quant_calls = 0;
unsigned long long zero_calls = 0;
unsigned long long div_calls = 0;
unsigned long long sqrt_calls = 0;
unsigned long long sub_calls = 0;
unsigned long long copy_calls = 0;

unsigned long long add_rounded = 0;
unsigned long long mul_rounded = 0;
unsigned long long div_rounded = 0;

unsigned long long count_begin_func;
unsigned long long count_end_func;
unsigned long long count_overhead;
// END COUNTING MICHAEL!!!

dec_t ONE, TWELVE, TWELVE_SQRT, MTWO, MFOUR, HUNDRED;
//Location of the rate files
#if defined(BID) || defined(NATIVE_GCC_BID)
 #if defined(WIDTH64)
  #define TBILL_LIB "testGen/bin/3month_tbill.csv.bid64"
  static char company_name[TOTAL_COMPANY][30] =
    {"testGen/bin/aapl.csv.bid64",  "testGen/bin/ace.csv.bid64", 
     "testGen/bin/adbe.csv.bid64",  "testGen/bin/bac.csv.bid64", 
     "testGen/bin/ba.csv.bid64",    "testGen/bin/borl.csv.bid64",
     "testGen/bin/dell.csv.bid64",  "testGen/bin/etn.csv.bid64", 
     "testGen/bin/fdx.csv.bid64",   "testGen/bin/ge.csv.bid64", 
     "testGen/bin/ibm.csv.bid64",   "testGen/bin/jnj.csv.bid64",
     "testGen/bin/jpm.csv.bid64",   "testGen/bin/ko.csv.bid64", 
     "testGen/bin/kss.csv.bid64",   "testGen/bin/mmm.csv.bid64", 
     "testGen/bin/mrk.csv.bid64",   "testGen/bin/msft.csv.bid64",
     "testGen/bin/nipny.csv.bid64", "testGen/bin/odp.csv.bid64"};
  static char sp500p[30] = "testGen/bin/sp500.csv.bid64";
 #endif
 #if defined(WIDTH128)
 #define TBILL_LIB "testGen/bin/3month_tbill.csv.bid128"
   static char company_name[TOTAL_COMPANY][30] =
     {"testGen/bin/aapl.csv.bid128",  "testGen/bin/ace.csv.bid128", 
      "testGen/bin/adbe.csv.bid128",  "testGen/bin/bac.csv.bid128", 
      "testGen/bin/ba.csv.bid128",    "testGen/bin/borl.csv.bid128",
      "testGen/bin/dell.csv.bid128",  "testGen/bin/etn.csv.bid128", 
      "testGen/bin/fdx.csv.bid128",   "testGen/bin/ge.csv.bid128", 
      "testGen/bin/ibm.csv.bid128",   "testGen/bin/jnj.csv.bid128",
      "testGen/bin/jpm.csv.bid128",   "testGen/bin/ko.csv.bid128", 
      "testGen/bin/kss.csv.bid128",   "testGen/bin/mmm.csv.bid128", 
      "testGen/bin/mrk.csv.bid128",   "testGen/bin/msft.csv.bid128",
      "testGen/bin/nipny.csv.bid128", "testGen/bin/odp.csv.bid128"};
   static char sp500p[30] = "testGen/bin/sp500.csv.bid128";
  #endif
#else
 #if defined(WIDTH64)
  #define TBILL_LIB "testGen/bin/3month_tbill.csv.d64"
  static char company_name[TOTAL_COMPANY][30] =
    {"testGen/bin/aapl.csv.d64",  "testGen/bin/ace.csv.d64", 
     "testGen/bin/adbe.csv.d64",  "testGen/bin/bac.csv.d64", 
     "testGen/bin/ba.csv.d64",    "testGen/bin/borl.csv.d64",
     "testGen/bin/dell.csv.d64",  "testGen/bin/etn.csv.d64", 
     "testGen/bin/fdx.csv.d64",   "testGen/bin/ge.csv.d64", 
     "testGen/bin/ibm.csv.d64",   "testGen/bin/jnj.csv.d64",
     "testGen/bin/jpm.csv.d64",   "testGen/bin/ko.csv.d64", 
     "testGen/bin/kss.csv.d64",   "testGen/bin/mmm.csv.d64", 
     "testGen/bin/mrk.csv.d64",   "testGen/bin/msft.csv.d64",
     "testGen/bin/nipny.csv.d64", "testGen/bin/odp.csv.d64"};
  static char sp500p[30] = "testGen/bin/sp500.csv.d64";
 #endif
 #if defined(WIDTH128)
  #define TBILL_LIB "testGen/bin/3month_tbill.csv.d128"
  static char company_name[TOTAL_COMPANY][30] =
    {"testGen/bin/aapl.csv.d128",  "testGen/bin/ace.csv.d128", 
     "testGen/bin/adbe.csv.d128",  "testGen/bin/bac.csv.d128", 
     "testGen/bin/ba.csv.d128",    "testGen/bin/borl.csv.d128",
     "testGen/bin/dell.csv.d128",  "testGen/bin/etn.csv.d128", 
     "testGen/bin/fdx.csv.d128",   "testGen/bin/ge.csv.d128", 
     "testGen/bin/ibm.csv.d128",   "testGen/bin/jnj.csv.d128",
     "testGen/bin/jpm.csv.d128",   "testGen/bin/ko.csv.d128", 
     "testGen/bin/kss.csv.d128",   "testGen/bin/mmm.csv.d128", 
     "testGen/bin/mrk.csv.d128",   "testGen/bin/msft.csv.d128",
     "testGen/bin/nipny.csv.d128", "testGen/bin/odp.csv.d128"};
  static char sp500p[30] = "testGen/bin/sp500.csv.d128";
 #endif
#endif

static char company_output[TOTAL_COMPANY][30] =
  {"testGen/result/aapl.log",  "testGen/result/ace.log",
   "testGen/result/adbe.log",  "testGen/result/bac.log",
   "testGen/result/ba.log",    "testGen/result/borl.log",
   "testGen/result/dell.log",  "testGen/result/etn.log",
   "testGen/result/fdx.log",   "testGen/result/ge.log",
   "testGen/result/ibm.log",   "testGen/result/jnj.log",
   "testGen/result/jpm.log",   "testGen/result/ko.log",
   "testGen/result/kss.log",   "testGen/result/mmm.log",
   "testGen/result/mrk.log",   "testGen/result/msft.log",
   "testGen/result/nipny.log", "testGen/result/odp.log"};
#ifdef COUNTING
//the global variable to check the operand characteristic
//Total add/sub, minmax, rescale/toint, multiply, and divide function calls
int mul_total, div_total, add_total, minmax_total, rescale_toint_total;
//Total number of the above fcn where their results are rounded
int mul_rounded, div_rounded, add_rounded, rescale_toint_rounded;
//number of add/sub where there operands have the same exponent
int add_no_exp_diff;
//number of minmax where there operands have the same exponent
int minmax_no_exp_diff;
//number of rescale and toIntegral where there operands have the same exponent
int rescale_toint_no_exp_diff;
//number of add/sub where no right shifting is needed in either operand
int add_no_right_shifted_diff_exp;
//number of minmax where no right shifting is needed in either operand
int minmax_no_right_shifted_diff_exp;
//number of rescale and toIntegral calls where no right shifting is needed in either operand
int rescale_toint_no_right_shifted_diff_exp;
//number of multiply where its number of digits in the product is less than 16 digits
int mul_lt_eq_16_coeff_on_result;
//The array that shows how many function calls with a certain amount of multiplier operands
//are shown in this benchmark run. E.g., sig_length_multiplier[13] = # of calls where their
//multiplier operand is 14-digit (not 13-digit) long.
int sig_length_multiplier[16];
//number of add/sub where at least one of its operands is zero
int add_op_zero;
#endif

int main(int argc, char *argv[]){
  unsigned long long begin_time, end_time, total_time; // for read time stamp counter
  int term = TERM;
  dec_t term_dec;
  cmd_t sp500_data[TERM], ticker_data[TERM];
  int i;
  dec_t current_riskfree_rate, riskfree_rate_average;
  dec_t avg_return_M, returns_M[TERM], monthly_deviation_sq_M[TERM],
    monthly_variance_M;
  dec_t avg_return, returns[TERM], monthly_deviation_sq[TERM],
    monthly_variance;
  dec_t monthly_deviation_prdt[TERM], covariance,
    monthly_std, annual_variance, annual_std;
  dec_t current_index, current_index_sp500;
  dec_t expected_ret, annual_dps_M, annual_dps;
  int year = 5;
  //output portion
  dec_t beta, alpha, Rf_m, intercept, system_var, unsystem_var, r_squared, 
    prediction, risk_premium;
  FILE *oup;
  char new_filename[100];
  int loop, user_loop;
  char str[30];
  if (argc!=3) {
    myfprintf(stderr, "[USAGE] cmd loop strname\n");
    exit(-1);
  }
  user_loop = myatoi(argv[1]);
  mystrcpy(str, argv[2]);

  myfprintf(stderr, "The program starts\n");

  initialized();
  FROMSTRING64(risk_premium, RISK_PREMIUM, &set);

  FROMINTEGER64(term_dec, term, &set);    
  read_TBill(TBILL_LIB, &current_riskfree_rate,
	     &riskfree_rate_average, term, &term_dec);
  process_company(sp500p, term, &term_dec, sp500_data, &avg_return_M,
                  returns_M, monthly_deviation_sq_M, &monthly_variance_M,
                  &current_index_sp500, &annual_dps_M);

  total_time = 0;
  //Since computing 20 companies is fast, users can provide the number of 
  //iteration to repetitively compute these results to make GPROF collect 
  //enough samples.
  for (loop = 0; loop<user_loop; loop++){
    myfprintf(stderr, "in Loop %d\n", loop);
    for (i=0; i<TOTAL_COMPANY; i++){
      mysprintf(new_filename, "%s_%d_%s", company_output[i], loop, str);
      oup = myfopen(new_filename, "w");
      assert(oup!=NULL);
      
      begin_time = rdtsc();
      company_vs_sp500((char*)company_name[i], term, &term_dec, ticker_data,
		       &avg_return, returns,
		       &avg_return_M, returns_M,
		       monthly_deviation_sq, &monthly_variance,
		       monthly_deviation_prdt, &covariance,
		       &monthly_std, &annual_variance,
		       &annual_std, &current_index, &annual_dps);
      generate_output(&covariance, term, &term_dec, &avg_return, 
		      &avg_return_M, &current_riskfree_rate, 
		      &riskfree_rate_average, &monthly_variance_M, 
		      &monthly_variance, &risk_premium, &current_index, 
		      &expected_ret, &annual_dps, year, &beta, &alpha, &Rf_m,
		      &intercept, &system_var, &unsystem_var, &r_squared,
		      &prediction);
      export_output(covariance, avg_return, avg_return_M, 
		    current_riskfree_rate, riskfree_rate_average, 
		    monthly_variance_M, monthly_variance,
		    risk_premium, current_index, expected_ret,
		    annual_dps, beta, alpha, Rf_m,
		    intercept, system_var, unsystem_var, r_squared,
		    prediction, oup);
      end_time = rdtsc();
      total_time += (end_time - begin_time);
      myfclose(oup);
    }
  }

  myfprintf(stderr, "The program ends sucessfully\n");
  myprintf("Total benchmark run_time: %lld\n", total_time);
 /* 
  myprintf("%ld,", add_calls);
  myprintf("%ld\n", add_cycles);
  myprintf("%ld,", copy_calls);
  myprintf("%ld\n", copy_cycles);
  myprintf("%ld,", div_calls);
  myprintf("%ld\n", div_cycles);
  myprintf("%ld,", mul_calls);
  myprintf("%ld\n", mul_cycles);
  myprintf("%ld,", quant_calls);
  myprintf("%ld\n", quant_cycles);
  myprintf("%ld,", sqrt_calls);
  myprintf("%ld\n", sqrt_cycles);
  myprintf("%ld,", sub_calls);
  myprintf("%ld\n", sub_cycles);
  myprintf("%ld,", zero_calls);
  myprintf("%ld\n", zero_cycles);
*/

  /*
  myprintf("Add rounded: %ld\n", add_rounded);
  myprintf("%ld\n", add_calls);
  myprintf("Mul rounded: %ld\n", mul_rounded);
  myprintf("%ld\n", mul_calls);
  myprintf("Div rounded: %ld\n", div_rounded);
  myprintf("%ld\n", div_calls);

  myprintf("Average cycles for add: %lld\n", add_cycles/add_calls);
  myprintf("Average cycles for copy: %lld\n", copy_cycles/copy_calls);
  myprintf("Average cycles for div: %lld\n", div_cycles/div_calls);
  myprintf("Average cycles for mul: %lld\n", mul_cycles/mul_calls);
  myprintf("Average cycles for quant: %lld\n", quant_cycles/quant_calls);
  myprintf("Average cycles for sqrt: %lld\n", sqrt_cycles/sqrt_calls);
  myprintf("Average cycles for sub: %lld\n", sub_cycles/sub_calls);
  myprintf("Average cycles for zero: %lld\n", zero_cycles/zero_calls);
*/

#ifdef COUNTING
  decimal_printout(stderr);
#endif
  return 0;
}
