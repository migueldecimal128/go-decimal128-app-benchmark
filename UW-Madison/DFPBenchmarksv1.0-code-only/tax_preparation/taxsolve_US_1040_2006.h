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

#ifndef TAXSOLVE_US_1040_2006_H
#define TAXSOLVE_US_1040_2006_H

#define CHECK_ORIG      0

#define Yes             1
#define No              0
#define MAX_NAME_LENGTH 24
#define MAX_BUF_SIZE    16

#define SINGLE 		            1
#define MARRIED_FILLING_JOINTLY 2
#define MARRIED_FILLING_SEPARAT 3
#define HEAD_OF_HOUSEHOLD       4
#define WIDOW		            5

dec_t schedD_ws_13, schedD_ws_14;
dec_t SchedA[MAX_LINES], SchedD[MAX_LINES];
dec_t L9b;	/* Qualified dividends. */
dec_t stcg, ltcg;      /* Variables for short and long term gains. */
int Do_SchedD=No, Do_QDCGTW=No, Do_SDTW=No;
context_t set;
int status, doline22=0, itemize;
size_t size;
dec_t capgain_amt[10];

FILE *infile=0;	 /* Main input file to be used for reading tax input data. */
FILE *outfile=0; /* Main output file. */
int verbose=0;	 /* Declare and set the "verbosity" flag. */
char outarr[10][100];
int arr_index=0;
dec_t ws_1, ws_12;

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

struct FedReturnData
 {
  dec_t fedline[MAX_LINES], schedD[MAX_LINES];
  int Exception, Itemized;
 } LastYearsReturn;

dec_t N_3000;    // -3,000
dec_t N_1500;    // -1,500
dec_t ZERO;      //      0
dec_t P_P02;     //      0.02
dec_t P_P025;    //      0.025
dec_t P_P03;     //      0.03
dec_t P_P05;     //      0.05
dec_t P_P075;    //      0.075
dec_t P_P1;      //      0.1
dec_t P_P15;     //      0.15
dec_t P_P25;     //      0.25
dec_t P_P26;     //      0.26
dec_t P_P28;     //      0.28 (N_P25 + N_P03)
dec_t P_P33;     //      0.33
dec_t P_P35;     //      0.35 (N_P33 + N_P02)
dec_t P_P5;      //      0.5
dec_t P_P8;      //      0.8
dec_t P_P999999; //      0.999999
dec_t P_1;       //      1
dec_t P_1P5;     //      1.5
dec_t P_3;       //      3
dec_t P_5;       //      5
dec_t P_10;      //     10
dec_t P_25;      //     25
dec_t P_50;      //     50
dec_t P_100;     //    100
dec_t P_500;     //    500
dec_t P_1000;    //  1,000
dec_t P_1100;    //  1,100
dec_t P_1250;    //  1,250
dec_t P_2500;    //  2,500
dec_t P_3000;    //  3,000
dec_t P_3300;    //  3,300
dec_t P_5150;    //  5,150
dec_t P_7550;    //  7,550
dec_t P_10300;   // 10,300
dec_t P_10750;   // 10,750
dec_t P_15000;   // 15,000
dec_t P_15100;   // 15,100
dec_t P_30000;   // 30,000
dec_t P_30650;   // 30,650
dec_t P_31275;   // 31,275
dec_t P_41050;   // 41,050
dec_t P_42500;   // 42,500
dec_t P_50000;   // 50,000
dec_t P_61250;   // 61,250
dec_t P_61300;   // 61,300
dec_t P_61850;   // 61,850
dec_t P_62550;   // 62,550
dec_t P_74200;   // 74,200
dec_t P_75000;   // 75,000
dec_t P_75250;   // 75,250
dec_t P_87500;   // 87,500
dec_t P_94225;   // 94,225
dec_t P_100000;  //100,000
dec_t P_105000;  //105,000
dec_t P_106000;  //106,000
dec_t P_112500;  //112,500
dec_t P_112875;  //112,875
dec_t P_122500;  //122,500
dec_t P_123700;  //123,700
dec_t P_150000;  //150,000
dec_t P_150500;  //150,500
dec_t P_154800;  //154,800
dec_t P_168275;  //168,275
dec_t P_171650;  //171,650
dec_t P_175000;  //175,000
dec_t P_188150;  //188,150
dec_t P_188450;  //188,450
dec_t P_225750;  //225,750
dec_t P_336550;  //336,550
dec_t P_9e9;     // 9e9

void macro_assign();
void read_from_file();
void TaxRateFormula( dec_t * tax, dec_t x, int status );
void TaxRateFunction( dec_t * tax, dec_t income, int status );
void showschedA( int linenum );
void capgains_qualdividends_worksheets( int status, dec_t L9b );
void AlternativeMinimumTax( dec_t * amt, int status, int itemized );
void ImportFederalReturnData( char *fedlogfile, struct FedReturnData *fed_data );
void CapitalLossCarryOverWorksheet( char *fedlogfile, struct FedReturnData *LastYearsReturn );
void get_cap_gains( char *emssg, char * capgain[] );
void sched_D_tax_worksheet( int status, dec_t L9b );
void initialization(char * capgain[]);
void printL6_L12();
void action(char * capgain[]);
void ending(char * outfname);

/*
void printLine(line_t line){
  char str[30];
  TOSTRING64(line.amt, str);
  //fprintf(stderr, "name: 0x040x\n", line.line_name);
  //fprintf(stderr, "name: 0x040x\n", line.line_name);
  //fprintf(stderr, "name: 0x0200x\n", line.data);
  fprintf(stderr, "name: %s, amt: %s\n", line.line_name, str);
}
*/
#endif
