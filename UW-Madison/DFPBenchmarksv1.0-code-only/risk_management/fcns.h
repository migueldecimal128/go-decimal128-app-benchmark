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

#ifndef FCNS_H
#define FCNS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//The risk premium is set to 5% now. If we want to compute this
//we need to run some more models.
//Refer to http://www.investopedia.com/articles/04/012104.asp
void initialized();
//process riskfree_rate
int read_TBill(char *filename, dec_t *current_riskfree,
               dec_t *avg_riskfree, int term, dec_t *term_dec);
int read_stock(char *filename, cmd_t *data);
//process fields
int process_company(char *filename, int term, dec_t *term_dec, cmd_t *data,
                    dec_t *avg_return, dec_t *returns,
                    dec_t *monthly_deviation_sq, dec_t *monthly_variance,
                    dec_t *current_index, dec_t *annual_dps);

int company_vs_sp500(char *filename, int term, dec_t *term_dec, cmd_t *data,
		     dec_t *avg_return, dec_t *returns,
		     dec_t *avg_return_M, dec_t *returnsM,
		     dec_t *monthly_deviation_sq, dec_t *monthly_variance,
		     dec_t *monthly_deviation_prdt, dec_t *covariance,
		     dec_t *monthly_std, dec_t *annual_variance,
		     dec_t *annual_std, dec_t *current_index,
                     dec_t *annual_dps);

int generate_output(dec_t *covar, int term, dec_t *term_dec,
                    dec_t *var_market,
                    dec_t *avg_return, dec_t *avg_return_market,
                    dec_t *risk_free, dec_t *monthly_variance_market,
                    dec_t *monthly_variance_ticker,
                    dec_t *historical_return,
                    dec_t *current_price, dec_t *expected_ret,
                    dec_t *annual_dps, int year,
                    dec_t *beta, dec_t *alpha, dec_t *Rf_m,
                    dec_t *intercept, dec_t *system_var,
                    dec_t *unsystem_var, dec_t *r_squared,
                    dec_t* prediction
                    );
//return(i) = (Price(i)*Split-Price(i-1)+DSP*Split)/Price(i-1), 
//Split is always 0 for s&p 500
//int returns_fcn(dec_t *cur_price, dec_t *last_price, dec_t *split,
//		dec_t *dividend, dec_t *ret);
int returns_fcn(cmd_t *data, int i, dec_t *returns);

//monthly_deviation(i) = (return(i)-avg(return))^2
#define monthly_deviation_sq_fcn(Z, RETURN, AVG_RETURN) SUBTRACT64(*Z, *RETURN, *AVG_RETURN, &set); MULTIPLY64(*Z, *Z, *Z, &set)
//var = {sigma[(x(i)-avg(x))^2]/(n-1)} . Note it's (n-1) not n
int variance(int term, dec_t *term_dec, dec_t *avg_return,
             dec_t *returns, dec_t *monthly_deviation_sq,
             dec_t *monthly_variance);
//monthly_covariance(i) = (return1(i)-avg(return1)) * (returnM(i)-avg(returnM))
int monthly_deviation_prdt_ret(dec_t *ret1, dec_t *retM, dec_t *avg_ret1,
                               dec_t *avg_retM, dec_t *ret_cov);
int covariance_fcn(dec_t *ret1, dec_t *retM, dec_t *avg_ret1,
                   dec_t *avg_retM, int term,
                   dec_t *monthly_deviation_prdt,
                   dec_t *covariance);
//beta = sum(covariance)/sum(market_variance)
#define beta_fcn(Z, COVAR, VAR_M) DIVIDE64(*Z, *COVAR, *VAR_M, &set)
//Rf(1-beta) = [(1+Risk_Free_Rate)^(1/12)-1]*(1-beta)
int Rf_m_fcn(dec_t *risk_free, dec_t *beta, dec_t *Rf_m);
//monthly variance = variance()
#define monthly_var(A, B, C, D, E, F, G) variance(A, B, C, D, E, F, G)
//monthly standard deviation = sqrt(monthly variance)
#define monthly_std(Z, MONTHLY_VAR) SQUAREROOT64(*Z, *MONTHLY_VAR, &set)
//annual variance = monthly variance * 12
#define annual_var(Z, MONTHLY_VAR) MULTIPLY64(*Z, *MONTHLY_VAR, TWELVE, &set)
//annual standard deviation = (monthly std dev)*(sqrt(12))
#define annual_std(Z, MONTHLY_STD) MULTIPLY64(*Z, *MONTHLY_STD, TWELVE_SQRT, &set)
//alpha = avg_return-avg_returnM*beta
#define alpha_fcn(Z, AVG_RETURN, AVG_RETURN_M, BETA) MULTIPLY64(*Z, *AVG_RETURN_M, *BETA, &set); SUBTRACT64(*Z, *AVG_RETURN, *Z, &set);
#define intercept_fcn(Z, ALPHA, RF_M) SUBTRACT64(*Z, *ALPHA, *RF_M, &set)
//Systematic variance = beta^2*variance_market
#define system_var_fcn(Z, BETA, VAR_M) MULTIPLY64(*Z, *BETA, *BETA, &set); MULTIPLY64(*Z, *Z, *VAR_M, &set)
//Unsystematic variance = stock variance - systematic variance
#define unsystem_var_fcn(Z, STOCK_VAR, SYSTEM_VAR) SUBTRACT64(*Z, *STOCK_VAR, *SYSTEM_VAR, &set)
//R squared = (systematic variance)/(stock variance)
#define r_squared_fcn(Z, SYSTEM_VAR, STOCK_VAR) DIVIDE64(*Z, *SYSTEM_VAR, *STOCK_VAR, &set)
//expected return = (riskfree rate + beta*historical_return_premium)
#define expected_ret_fcn(Z, RISKFREE, BETA, HIST_RET) MULTIPLY64(*Z, *BETA, *HIST_RET, &set); ADD64(*Z, *Z, *RISKFREE, &set)
//prediction = (current price) * (1 + expected_return-(Annualized_DPS/Current_Price))^#year
int prediction_fcn(dec_t *current_price, dec_t *expected_ret, 
		   dec_t *annual_dps, int year, dec_t *prediction);
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
		  );
#endif
