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

#ifndef TAXSOLVE_CA_540_2006_H
#define TAXSOLVE_CA_540_2006_H

#define SINGLE 		            1
#define MARRIED_FILLING_JOINTLY 2
#define MARRIED_FILLING_SEPARAT 3
#define HEAD_OF_HOUSEHOLD       4
#define WIDOW		            5

#define MAX_NAME_LENGTH 24

#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
 #ifdef WIDTH64
 typedef struct line64_struct{
   char line_name[MAX_NAME_LENGTH];
   decimal64 amt;
 }line64_t;
 #endif
 #ifdef WIDTH128
 typedef struct line128_struct{
   char line_name[MAX_NAME_LENGTH];
   decimal128 amt;
 }line128_t;
 #endif
#endif

typedef struct line_struct{
    char line_name[MAX_NAME_LENGTH];
    dec_t amt;
}line_t;

line_t line;

#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
 #ifdef WIDTH64
  line64_t line64;
 #endif
 #ifdef WIDTH128
  line128_t line128;
 #endif
#endif

FILE *infile=0;	 /* Main input file to be used for reading tax input data. */
FILE *outfile=0; /* Main output file. */
context_t set;

dec_t ZERO;      //      0
dec_t P_P01;     //      0.01
dec_t P_P02;     //     0.02
dec_t P_P04;     //     0.04
dec_t P_P06;     //     0.06
dec_t P_P08;     //     0.08
dec_t P_P093;    //    0.093
dec_t P_1;       //      1
dec_t P_2;       //      2
dec_t P_6;       //      6
dec_t P_91;      //     91
dec_t P_285;     //    285
dec_t P_1250;    //   1250
dec_t P_2500;    //   2500
dec_t P_3410;    //   3410
dec_t P_6622;    //   6622
dec_t P_6820;    //   6820
dec_t P_13244;   //  13244
dec_t P_13251;   //  13251
dec_t P_15698;   //  15698
dec_t P_24776;   //  24776
dec_t P_31396;   //  31396
dec_t P_31397;   //  31397
dec_t P_34394;   //  34394
dec_t P_40473;   //  40473
dec_t P_43467;   //  43467
dec_t P_49552;   //  49552
dec_t P_50090;   //  50090
dec_t P_59166;   //  59166
dec_t P_68788;   //  68788
dec_t P_86934;   //  86934
dec_t P_150743;  // 150743
dec_t P_226119;  // 226119
dec_t P_301491;  // 301491


dec_t ws_a, ws_b, ws_c, ws_d, ws_e, ws_f, ws_g, ws_h, ws_i, ws_j, ws_k, ws_l, ws_m, ws_n;
dec_t iline7, iline8, iline9, iline10;
int status;
size_t size;

void macro_assign();
void initialization();
void action();
void ending();
void read_from_file();
void TaxRateFormula( dec_t * tax, dec_t x, int status );
void TaxRateFunction( dec_t * tax, dec_t income, int status );

#endif
