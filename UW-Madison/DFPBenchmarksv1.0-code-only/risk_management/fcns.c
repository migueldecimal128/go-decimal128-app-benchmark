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
extern context_t set;

#if defined(NATIVE_GCC_BID)
extern _IDEC_flags flags;      // working flags 
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

extern unsigned long long add_calls;
extern unsigned long long mul_calls;
extern unsigned long long quant_calls;
extern unsigned long long zero_calls;
extern unsigned long long div_calls;
extern unsigned long long sqrt_calls;
extern unsigned long long sub_calls;
extern unsigned long long copy_calls;

extern unsigned long long add_rounded;
extern unsigned long long mul_rounded;
extern unsigned long long div_rounded;

extern unsigned long long count_begin_func;
extern unsigned long long count_end_func;
extern unsigned long long count_overhead;
// END COUNTING MICHAEL!!!



extern dec_t ONE, TWELVE, TWELVE_SQRT, MTWO, MFOUR, HUNDRED;
extern const char company_name[TOTAL_COMPANY][20];
#ifdef COUNTING
extern int mul_total, div_total, add_total, minmax_total, rescale_toint_total;
extern int mul_rounded, div_rounded, add_rounded, rescale_toint_rounded;
//the below one can run on fast pass adder
extern int add_no_exp_diff;
extern int minmax_no_exp_diff;
extern int rescale_toint_no_exp_diff;
//this one may run on fast pass
extern int add_no_right_shifted_diff_exp;
extern int minmax_no_right_shifted_diff_exp;
extern int rescale_toint_no_right_shifted_diff_exp;
//the below can run on fast pass mul
extern int mul_lt_eq_16_coeff_on_result;
//length of multiplier does matter because mul can be sequential base
extern int sig_length_multiplier[16];
extern int add_op_zero;
#endif
//READ the Treasury Bill Rate
int read_TBill(char *filename, dec_t *current_riskfree, 
	       dec_t *avg_riskfree, int term, dec_t *term_dec){
  FILE *fp;
  dec_t *riskfree_rate;
  int i, ret;
  dec_t accu;
#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
  decimal64 *record;
 #endif
 #if defined(WIDTH128)
  decimal128 *record;
 #endif
#endif
  riskfree_rate = mycalloc(term, sizeof(dec_t));
  fp = myfopen(filename, "r");
  assert(fp!=NULL);
#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
  record = mycalloc(term, sizeof(decimal64));
  ret = myfread(record, sizeof(decimal64), term, fp);
  for (i=0; i<term; i++)
    decimal64ToNumber(&record[i], &riskfree_rate[i]);
 #endif
 #if defined(WIDTH128)
  record = mycalloc(term, sizeof(decimal128));
  ret = myfread(record, sizeof(decimal128), term, fp);
  for (i=0; i<term; i++)
    decimal128ToNumber(&record[i], &riskfree_rate[i]);
 #endif
#else
  ret = myfread(riskfree_rate, sizeof(dec_t), TERM, fp);
#endif
  if (ret!=TERM) {
    myfree(riskfree_rate);
    return -1;
  }
  COPY64(*current_riskfree, riskfree_rate[term-1]);
  DIVIDE64(*current_riskfree, *current_riskfree, HUNDRED, &set); //put into %
  ZERO64(accu);
  for (i=0; i<term; i++)
    ADD64(accu, accu, riskfree_rate[i], &set);
  DIVIDE64(*avg_riskfree, accu, *term_dec, &set);
#ifdef DECNUMBER_ARBITRARY
  RESCALE64(*avg_riskfree, *avg_riskfree, MTWO, &set);
#else
  QUANTIZE64(*avg_riskfree, *avg_riskfree, MTWO, &set);
#endif
  DIVIDE64(*avg_riskfree, *avg_riskfree, HUNDRED, &set); //put into %
  myfclose(fp);
  myfree(riskfree_rate);
#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
    myfree(record);
#endif
  return 0;
}
//initialization for the global variables
void initialized(){
#ifdef COUNTING
  int i;
  mul_total = 0, div_total = 0, add_total = 0;
  minmax_total = 0, rescale_toint_total = 0;
  mul_rounded = 0, div_rounded = 0, add_rounded = 0, rescale_toint_rounded = 0;
  add_no_exp_diff = 0, minmax_no_exp_diff = 0, rescale_toint_no_exp_diff = 0;
  add_no_right_shifted_diff_exp = 0, minmax_no_right_shifted_diff_exp = 0;
  rescale_toint_no_right_shifted_diff_exp = 0;
  mul_lt_eq_16_coeff_on_result = 0;
  add_op_zero = 0;
  for (i=0; i<16; i++)
    sig_length_multiplier[i] =0;
#endif  
#ifdef DEC32
  decContextDefault(&set, DEC_INIT_DECIMAL32);
#elif defined(DEC128)
  decContextDefault(&set, DEC_INIT_DECIMAL128);
#else
  CONTEXTDEFAULT(set, RNE);
#endif
  FROMSTRING64(ONE, "1", &set);
  FROMSTRING64(TWELVE, "12", &set);
  SQUAREROOT64(TWELVE_SQRT, TWELVE, &set);
#if defined(DECNUMBER_ARBITRARY)
  FROMSTRING64(MTWO, "-2", &set);
  FROMSTRING64(MFOUR, "-4", &set);
#else
  FROMSTRING64(MTWO, "0.01", &set);
  FROMSTRING64(MFOUR, "0.0001", &set);
#endif
  FROMSTRING64(HUNDRED, "100", &set);
}
//function to read the stock index from files
int read_stock(char *filename, cmd_t *data){
  FILE *fp;
  int ret;
  int i;
#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
  decimal64 record;
 #endif
 #if defined(WIDTH128)
  decimal128 record;
 #endif
#endif
  fp = myfopen(filename, "rb");
  assert(fp!=NULL);
  for (i=0; i<TERM; i++){
#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
    ret = myfread(&record, sizeof(decimal64), 1, fp);
    decimal64ToNumber(&record, &(data[i].index));
    ret = myfread(&record, sizeof(decimal64), 1, fp);
    decimal64ToNumber(&record, &(data[i].volume));
    ret = myfread(&record, sizeof(decimal64), 1, fp);
    decimal64ToNumber(&record, &(data[i].split));
    ret = myfread(&record, sizeof(decimal64), 1, fp);
    decimal64ToNumber(&record, &(data[i].dividend));    
 #endif
 #if defined(WIDTH128) 
    ret = myfread(&record, sizeof(decimal128), 1, fp);
    decimal128ToNumber(&record, &(data[i].index));
    ret = myfread(&record, sizeof(decimal128), 1, fp);
    decimal128ToNumber(&record, &(data[i].volume));
    ret = myfread(&record, sizeof(decimal128), 1, fp);
    decimal128ToNumber(&record, &(data[i].split));
    ret = myfread(&record, sizeof(decimal128), 1, fp);
    decimal128ToNumber(&record, &(data[i].dividend)); 
 #endif
#else
    ret = myfread(&data[i], sizeof(cmd_t), 1, fp);
#endif
    if (ret != 1) return -1;
  }
  myfclose(fp);
  return 0;
}
//this fcn processes one company at a time to compute all those statistics
int process_company(char *filename, int term, dec_t *term_dec, cmd_t *data, 
		    dec_t *avg_return, dec_t *returns, 
		    dec_t *monthly_deviation_sq, dec_t *monthly_variance,
		    dec_t *current_index, dec_t *annual_dps){
  //monthly deviation square is (X-avg(X))^2. 
  //monthly variance is sigma[(X-avg(X))^2]
  int i;
  dec_t temp;
  dec_t dps[12];
  for (i=0; i<12; i++)
    ZERO64(dps[i]);
  read_stock(filename, data);
  ZERO64(*avg_return);
  ZERO64(*annual_dps);
  ZERO64(returns[0]);
  //compute the monthly return
  for (i=1; i<term; i++) {
    returns_fcn(data, i, returns);
#if defined(DECNUMBER_ARBITRARY)
    RESCALE64(returns[i], returns[i], MFOUR, &set);
#else
    QUANTIZE64(returns[i], returns[i], MFOUR, &set);
#endif
    ADD64(*avg_return, *avg_return, returns[i], &set);
    COPY64(dps[i%12], (data[i].dividend));
  }
  COPY64(*current_index, (data[term-1].index));
  SUBTRACT64(temp, *term_dec, ONE, &set);
  DIVIDE64(*avg_return, *avg_return, temp, &set);
#if defined(DECNUMBER_ARBITRARY)
  RESCALE64(*avg_return, *avg_return, MFOUR, &set);
#else
  QUANTIZE64(*avg_return, *avg_return, MFOUR, &set);
#endif
  //compute the variance
  variance(term, term_dec, avg_return, returns, monthly_deviation_sq, 
	   monthly_variance);
  for (i=0; i<12; i++)
    ADD64(*annual_dps, *annual_dps, dps[i], &set);
#if defined(DECNUMBER_ARBITRARY)
  RESCALE64(*annual_dps, *annual_dps, MFOUR, &set);
#else
  QUANTIZE64(*annual_dps, *annual_dps, MFOUR, &set);
#endif
  return 0;
}
//This function computes the statistic for a company over a period of time
//and also its relationship to the S&P 500 index
int company_vs_sp500(char *filename, int term, dec_t *term_dec, cmd_t *data, 
		     dec_t *avg_return, dec_t *returns,
		     dec_t *avg_return_M, dec_t *returnsM,
		     dec_t *monthly_deviation_sq, dec_t *monthly_variance,
		     dec_t *monthly_deviation_prdt, dec_t *covariance,
		     dec_t *monthly_std, dec_t *annual_variance, 
		     dec_t *annual_std, dec_t *current_index, 
		     dec_t *annual_dps){
  int i;
  process_company(filename, term, term_dec, data, avg_return, returns, 
		  monthly_deviation_sq, monthly_variance, current_index,
		  annual_dps);
  for (i=1; i<term; i++){
    covariance_fcn(returns, returnsM, avg_return, avg_return_M,
		   term, monthly_deviation_prdt, covariance);
  }
  monthly_std(monthly_std, monthly_variance);
  annual_var(annual_variance, monthly_variance);
  annual_std(annual_std, monthly_std);  
  return 0;
}
//This function generates the output values
int generate_output(dec_t *covar, int term, dec_t *term_dec,
		    dec_t *avg_return, dec_t *avg_return_market, 
		    dec_t *current_riskfree_rate,
		    dec_t *riskfree_rate_average,
		    dec_t *monthly_variance_market,
		    dec_t *monthly_variance_ticker, 
		    dec_t *risk_premium,
		    dec_t *current_index, dec_t *expected_ret,
		    dec_t *annual_dps, int year, 
		    dec_t *beta, dec_t *alpha, dec_t *Rf_m,
		    dec_t *intercept, dec_t *system_var, 
		    dec_t *unsystem_var, dec_t *r_squared,
		    dec_t* prediction
		    ){
  beta_fcn(beta, covar, monthly_variance_market);
  alpha_fcn(alpha, avg_return, avg_return_market, beta);
  Rf_m_fcn(riskfree_rate_average, beta, Rf_m);
  intercept_fcn(intercept, alpha, Rf_m);
  system_var_fcn(system_var, beta, monthly_variance_market);
  unsystem_var_fcn(unsystem_var, monthly_variance_ticker,
		   monthly_variance_market);
  r_squared_fcn(r_squared, monthly_variance_market, monthly_variance_ticker);
  expected_ret_fcn(expected_ret, current_riskfree_rate, beta, 
		   risk_premium);
  prediction_fcn(current_index, expected_ret, annual_dps, year, prediction);
  return 0;
}
//This function generate the expected return
int returns_fcn(cmd_t *data, int i, dec_t *returns){
  //return(i) = (Price(i)*Split-Price(i-1)+DIVIDEND*Split)/Price(i-1),
  dec_t temp1, temp2;
  MULTIPLY64(temp1, (data[i].index), (data[i].split), &set);
  MULTIPLY64(temp2, (data[i].dividend), (data[i].split), &set);
  SUBTRACT64(temp1, temp1, (data[i-1].index), &set);
  ADD64(temp1, temp1, temp2, &set);
  DIVIDE64(returns[i], temp1, (data[i-1].index), &set);
  return 0;
}
//This function computes the variance 
int variance(int term, dec_t *term_dec, dec_t *avg_return,
	     dec_t *returns, dec_t *monthly_deviation_sq, 
	     dec_t *monthly_variance){
  //var = {sigma[(x(i)-avg_return(x))^2]/(n-1)} . Note it's (n-1) not n
  int i;
  dec_t temp;
  SUBTRACT64(temp, *term_dec, ONE, &set);
  ZERO64(*monthly_variance);
  ZERO64(monthly_deviation_sq[0]);
  for (i=1; i<term; i++){
    monthly_deviation_sq_fcn(&monthly_deviation_sq[i], &returns[i], avg_return);
#if defined(DECNUMBER_ARBITRARY)
    RESCALE64(monthly_deviation_sq[i], monthly_deviation_sq[i], MFOUR, &set);
#else
    QUANTIZE64(monthly_deviation_sq[i], monthly_deviation_sq[i], MFOUR, &set);
#endif
    ADD64(*monthly_variance, *monthly_variance, monthly_deviation_sq[i], &set);
  }
  return 0;
}
//This function compues the monthly deviation and prediction return
int monthly_deviation_prdt_ret(dec_t *ret1, dec_t *retM, dec_t *avg_ret1,
			       dec_t *avg_retM, dec_t *ret_cov){
  dec_t temp1, temp2;
  SUBTRACT64(temp1, *ret1, *avg_ret1, &set);
  SUBTRACT64(temp2, *retM, *avg_retM, &set);
  MULTIPLY64(*ret_cov, temp1, temp2, &set);
  return 0;
}
//This function computes the covariance
int covariance_fcn(dec_t *ret1, dec_t *retM, dec_t *avg_ret1,
		   dec_t *avg_retM, int term, 
		   dec_t *monthly_deviation_prdt, 
		   dec_t *covariance){
  int i;
  ZERO64(*covariance);
  ZERO64(monthly_deviation_prdt[0]);
  for (i=1; i<term; i++){
    monthly_deviation_prdt_ret(&ret1[i], &retM[i], avg_ret1, avg_retM, 
			       &monthly_deviation_prdt[i]);
#if defined(DECNUMBER_ARBITRARY)
    RESCALE64(monthly_deviation_prdt[i], monthly_deviation_prdt[i], MFOUR, 
	      &set);
#else
    QUANTIZE64(monthly_deviation_prdt[i], monthly_deviation_prdt[i], MFOUR,
	       &set);
#endif
    ADD64(*covariance, *covariance, monthly_deviation_prdt[i], &set);
  }
  return 0;
}
//This function computes the Rf function
int Rf_m_fcn(dec_t *risk_free, dec_t *beta, dec_t *Rf_m){
  //Rf(1-beta) = [(1+Risk_Free_Rate)^(1/12)-1]*(1-beta)
  dec_t temp;
  DIVIDE64(temp, ONE, TWELVE, &set);
  ADD64(*Rf_m, ONE, *risk_free, &set);
  POWER64((*Rf_m), (*Rf_m), temp, &set);
  SUBTRACT64(*Rf_m, *Rf_m, ONE, &set);
  SUBTRACT64(temp, ONE, *beta, &set);
  MULTIPLY64(*Rf_m, *Rf_m, temp, &set);
  return 0;
}
//This function compute the prediction value
int prediction_fcn(dec_t *current_price, dec_t *expected_ret,
		   dec_t *annual_dps, int year, dec_t *prediction){
  dec_t temp, year_dec;
  DIVIDE64(temp, *annual_dps, *current_price, &set);
  SUBTRACT64(temp, *expected_ret, temp, &set);
  ADD64(temp, ONE, temp, &set);
  FROMINTEGER64(year_dec, year, &set);
  POWER64(temp, temp, year_dec, &set);
  MULTIPLY64(*prediction, *current_price, temp, &set);
  return 0;
}
//This function prints out the result
int export_output(dec_t covar, 
		  dec_t avg_return, dec_t avg_return_market,
		  dec_t current_riskfree_rate,
		  dec_t riskfree_rate_average,
		  dec_t monthly_variance_market,
		  dec_t monthly_variance_ticker,
		  dec_t risk_premium,
		  dec_t current_index, dec_t expected_ret,
		  dec_t annual_dps, 
		  dec_t beta, dec_t alpha, dec_t Rf_m,
		  dec_t intercept, dec_t system_var,
		  dec_t unsystem_var, dec_t r_squared,
		  dec_t prediction, FILE *oup
		  )
{
  char str[100];
  TOSTRING64(covar, str);
  myfprintf(oup, "covariance is %s\n", str);
  TOSTRING64(avg_return, str);
  myfprintf(oup, "avg_return is %s\n", str);
  TOSTRING64(avg_return_market, str);
  myfprintf(oup, "avg_ret_M  is %s\n", str);
  MULTIPLY64(current_riskfree_rate, current_riskfree_rate, HUNDRED, &set);
  TOSTRING64(current_riskfree_rate, str);
  myfprintf(oup, "current_riskfree_rate  is %s (percent) \n", str);
  MULTIPLY64(riskfree_rate_average, riskfree_rate_average, HUNDRED, &set);
  TOSTRING64(riskfree_rate_average, str);
  myfprintf(oup, "riskfree rate avg  is %s (percent) \n", str);
  TOSTRING64(monthly_variance_market, str);
  myfprintf(oup, "monthly_var market  is %s\n", str);
  TOSTRING64(monthly_variance_ticker, str);
  myfprintf(oup, "monthly_var_ticker  is %s\n", str);
  TOSTRING64(risk_premium, str);
  myfprintf(oup, "risk premium  is %s\n", str);
  TOSTRING64(current_index, str);
  myfprintf(oup, "current index  is %s\n", str);
  TOSTRING64(expected_ret, str);
  myfprintf(oup, "expected return is %s\n", str);
  TOSTRING64(annual_dps, str);
  myfprintf(oup, "annual DPS  is %s\n", str);
  TOSTRING64(beta , str);
  myfprintf(oup, "beta  is %s\n", str);
  TOSTRING64(alpha, str);
  myfprintf(oup, "alpha  is %s\n", str);
  TOSTRING64(Rf_m, str);
  myfprintf(oup, "Rf_m  is %s\n", str);
  TOSTRING64(intercept, str);
  myfprintf(oup, "intercept  is %s\n", str);
  TOSTRING64(system_var, str);
  myfprintf(oup, "system var is %s\n", str);
  TOSTRING64(unsystem_var, str);
  myfprintf(oup, "unsystem var  is %s\n", str);
  TOSTRING64(r_squared, str);
  myfprintf(oup, "R square  is %s\n", str);
  TOSTRING64(prediction, str);
  myfprintf(oup, "prediction  is %s\n", str);
  return 0;
}
