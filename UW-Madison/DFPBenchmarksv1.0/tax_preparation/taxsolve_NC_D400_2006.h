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

#ifndef TAXSOLVE_NC_D400_2006_H
#define TAXSOLVE_NC_D400_2006_H

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
dec_t P_P0007;   //      0.0007
dec_t P_P06;     //      0.06
dec_t P_P07;     //      0.07
dec_t P_P0775;   //      0.0775
dec_t P_P0825;   //      0.0825
dec_t P_P3333;   //      0.3333
dec_t P_P9;      //      0.9
dec_t P_1;       //      1
dec_t P_25;      //     25
dec_t P_50;      //     50
dec_t P_300;     //    300
dec_t P_600;     //    600
dec_t P_637P5;   //    637.5   //10625 x 0.06
dec_t P_765;     //    765     //12750 x 0.06
dec_t P_800;     //    800
dec_t P_1000;    //   1000
dec_t P_1020;    //   1020     //17000 x 0.06
dec_t P_1275;    //   1275     //21250 x 0.06
dec_t P_1300;    //   1300
dec_t P_3000;    //   3000
dec_t P_3393P75; //   3393.75
dec_t P_3600;    //   3600
dec_t P_3750;    //   3750
dec_t P_4072P5;  //   4072.5
dec_t P_4200;    //   4200
dec_t P_4400;    //   4400
dec_t P_4500;    //   4500
dec_t P_4800;    //   4800
dec_t P_5150;    //   5150
dec_t P_5400;    //   5400
dec_t P_5430;    //   5430
dec_t P_5900;    //   5900
dec_t P_6000;    //   6000
dec_t P_6600;    //   6600
dec_t P_6787P5;  //   6787.5
dec_t P_7200;    //   7200
dec_t P_7268P75; //   7268.75
dec_t P_7800;    //   7800
dec_t P_8400;    //   8400
dec_t P_8722P5;  //   8722.5
dec_t P_10625;   //  10625
dec_t P_11630;   //  11630
dec_t P_12750;   //  12750
dec_t P_14537P5; //  14537.5
dec_t P_17000;   //  17000
dec_t P_21250;   //  21250
dec_t P_50000;   //  50000
dec_t P_60000;   //  60000
dec_t P_68000;   //  68000
dec_t P_80000;   //  80000
dec_t P_100000;  // 100000
dec_t P_120000;  // 120000
dec_t P_160000;  // 160000
dec_t P_200000;  // 200000

dec_t Exemptions, Itemized, Limited, LimitedL6;
dec_t AB, AGI, ws2, ws10;
int status;
size_t size;

void macro_assign();
void initialization();
void action();
void ending();
void read_from_file();
void TaxRateFunction( dec_t * tax, dec_t income, int status );

#endif
