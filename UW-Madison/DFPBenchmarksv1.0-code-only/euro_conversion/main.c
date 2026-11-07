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
#include "decfcn.h"
#include "rdtsc.h"

#define COUNTRY 12
static context_t set;

#if defined(NATIVE_GCC_BID)
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
unsigned long long int_cycles = 0;

unsigned long long add_calls = 0;
unsigned long long mul_calls = 0;
unsigned long long quant_calls = 0;
unsigned long long zero_calls = 0;
unsigned long long div_calls = 0;
unsigned long long sqrt_calls = 0;
unsigned long long sub_calls = 0;
unsigned long long copy_calls = 0;
unsigned long long int_calls = 0;

unsigned long long add_rounded = 0;
unsigned long long mul_rounded = 0;
unsigned long long div_rounded = 0;

unsigned long long count_begin_func;
unsigned long long count_end_func;
unsigned long long count_overhead;
// END COUNTING MICHAEL!!!

dec_t intermediate_euro, ZEROCONST;
dec_t table[COUNTRY+1][2];
static dec_t ONE, TEN;

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
//number of add/sub where at least one of its operands is zero
int add_op_zero;
//number of multiply where its number of digits in the product is less than 16 digits
int mul_lt_eq_16_coeff_on_result;
//The array that shows how many function calls with a certain amount of multiplier operands
//are shown in this benchmark run. E.g., sig_length_multiplier[13] = # of calls where their
//multiplier operand is 14-digit (not 13-digit) long.
int sig_length_multiplier[16];
#endif


// Round to the closest available coin in target NCU
void roundToAvailableCoin(dec_t *c, dec_t a, dec_t b){
  //dec_t temp, new_a, large_ncu;
  dec_t temp, new_a;
  int gt_res;
  GT64(gt_res, b, ONE, &set);
  if(gt_res) { //b is greater than one
    DIVIDE64(temp, a, TEN, &set); //a = a/10;
  }
  else {
    COPY64(temp, a); //a = a
  }
  //Euro usually use ROUND_DOWN mode
   SET_ROUNDMODE(set, RTZ);
#ifdef DEBUG
  fprintf(stderr, "==== RoundToAvailableCoin Check Point #0  ==== \n");
  decfprintf(stderr, 1, "temp", &temp);
#endif
  TOINTEGRAL64(new_a, temp, &set); //int(a)
   SET_ROUNDMODE(set, RNE);
#ifdef DEBUG
  fprintf(stderr, "==== RoundToAvailableCoin Check Point #1  ==== \n");
  decfprintf(stderr, 2, "(temp, new_a)", &temp, &new_a);
#endif
  SUBTRACT64(temp, temp, new_a, &set); //a-int(a)
#ifdef DEBUG
  fprintf(stderr, "==== RoundToAvailableCoin Check Point #2  ==== \n");
  decfprintf(stderr, 1, "temp", &temp);
#endif
#if defined (HW)&&(DEBUG)
  fprintf(stderr, "%d: temp = 0x%02x%02x%02x%02x%02x%02x%02x%02x\n", 
	  global_counter,
	  temp.b.bytes[7], temp.b.bytes[6], temp.b.bytes[5], 
	  temp.b.bytes[4], temp.b.bytes[3], temp.b.bytes[2], 
	  temp.b.bytes[1], temp.b.bytes[0]);
#endif
  if (ISZERO64(temp)==0){//means there are some fraction we need to round
   if(gt_res) { //b is greater than one
      MULTIPLY64(temp, temp, TEN, &set); //a = a*10;
      MULTIPLY64(new_a, new_a, TEN, &set);//int_a = int(a) * 10
    }
    DIVIDE64(temp, temp, b, &set);

    TOINTEGRAL64(temp, temp, &set);
    MULTIPLY64(temp, temp, b, &set);
#ifdef DEBUG
    fprintf(stderr, "==== RoundToAvailableCoin Check Point #3  ==== \n");
    decfprintf(stderr, 2, "(temp, new_a)", &temp, &new_a);
#endif
    ADD64((*c), temp, new_a, &set);
#ifdef DEBUG
    fprintf(stderr, "==== RoundToAvailableCoin Check Point #4  ==== \n");
    decfprintf(stderr, 1, "c", c);
#endif
  }
  else {//no fraction part that needs to be rounded, pass the input to the output
    COPY64((*c), a);
  }
}

//Conversion fcn from euro to other national currency unit
void euro2ncu(int ncu_index, dec_t *euro, dec_t *ncu){
  dec_t temp;
  MULTIPLY64(temp, (*euro), table[ncu_index][0], &set);
  roundToAvailableCoin(ncu, temp, table[ncu_index][1]);
}
//Conversion fcn from other national currency unit to euro
void ncu2euro(int ncu_index, dec_t *euro, dec_t *ncu){
  dec_t temp;
  DIVIDE64(temp, (*euro), table[ncu_index][0], &set);
  roundToAvailableCoin(ncu, temp, table[COUNTRY][1]);
}
//Conversion fcn from one national currency unit to the other
void ncu2ncu(int src_ncu_index, int dest_ncu_index, 
	     dec_t *src_ncu, dec_t *dest_ncu){
  dec_t temp;
  DIVIDE64(temp, (*src_ncu), table[src_ncu_index][0], &set);
#ifdef DEBUG
  fprintf(stderr, "======== ncu2ncu Checkpoint ========\n");
  decfprintf(stderr, 3, "(src_ncu, table[src_ncu_index], temp)", 
	     src_ncu, &table[src_ncu_index][0], &temp);
#endif
  roundToAvailableCoin(&temp, temp, intermediate_euro);
  MULTIPLY64(temp, temp, table[dest_ncu_index][0], &set);
  roundToAvailableCoin(dest_ncu, temp, table[dest_ncu_index][1]);
}

//The conversion rate
void constructArray() {
  // EXCHANGE RATES:
  FROMSTRING64(table[0][0], "40.3399", &set); //BEF
  FROMSTRING64(table[1][0], "1.95583", &set); //DEM
  FROMSTRING64(table[2][0], "166.386", &set); //ESP
  FROMSTRING64(table[3][0], "6.55957", &set); //FRF
  FROMSTRING64(table[4][0], "0.787564", &set);//IEP
  FROMSTRING64(table[5][0], "1936.27", &set); //ITL
  FROMSTRING64(table[6][0], "40.3399", &set); //LUF
  FROMSTRING64(table[7][0], "2.20371", &set); //NLG
  FROMSTRING64(table[8][0], "13.7603", &set); //ATS
  FROMSTRING64(table[9][0], "200.482", &set); //PTE
  FROMSTRING64(table[10][0], "5.94573", &set); //FIM
  FROMSTRING64(table[11][0], "340.750", &set); //GRD
  FROMSTRING64(table[12][0], "1.00000", &set); //EURO
  // MINIMUM COIN UNITS:
  FROMSTRING64(table[0][1], "0.5", &set); //BEF
  FROMSTRING64(table[1][1], "0.01", &set); //DEM
  FROMSTRING64(table[2][1], "1", &set); //ESP
  FROMSTRING64(table[3][1], "0.05", &set); //FRF
  FROMSTRING64(table[4][1], "0.01", &set);//IEP
  FROMSTRING64(table[5][1], "1", &set); //ITL
  FROMSTRING64(table[6][1], "0.5", &set); //LUF
  FROMSTRING64(table[7][1], "0.05", &set); //NLG
  FROMSTRING64(table[8][1], "0.1", &set); //ATS
  FROMSTRING64(table[9][1], "5", &set); //PTE
  FROMSTRING64(table[10][1], "0.1", &set); //FIM
  FROMSTRING64(table[11][1], "0.5", &set); //GRD
  FROMSTRING64(table[12][1], "0.01", &set); //EURO
}
//Output structure
typedef struct output{
  int from;
  int to;
  dec_t inp;
  dec_t oup;
}output_t;
//Initialization function
#ifdef COUNTING
void initialization(){
  int i;
  mul_total = 0, div_total = 0, add_total = 0;
  minmax_total = 0, rescale_toint_total = 0;
  mul_rounded = 0, div_rounded = 0, add_rounded = 0, rescale_toint_rounded = 0;
  add_no_exp_diff = 0, minmax_no_exp_diff = 0, rescale_toint_no_exp_diff = 0;
  add_no_right_shifted_diff_exp = 0, minmax_no_right_shifted_diff_exp = 0;
  add_op_zero = 0;
  rescale_toint_no_right_shifted_diff_exp = 0;
  mul_lt_eq_16_coeff_on_result = 0;
  for (i=0; i<16; i++)
    sig_length_multiplier[i] =0;
}
#endif
int main(int argc, char *argv[]){
  unsigned long long begin_time, end_time, total_time; // for read time stamp counter
  char filein[MAXFILE+1];          // full input filename
  char fileou[MAXFILE+1];          // full output filename
  FILE *inp, *oup;
  int ret;
  output_t out;
  assert(argc==3);               //make sure user provide at least 2 args
  strcpy(filein, argv[1]);       // is input file name
  strcpy(fileou, "/tmp/");
  strcat(fileou, argv[2]);       // is output file name

// start loop here
total_time = 0;
int counter;
for(counter=0 ; counter < 15 ; counter++) {

  CONTEXTDEFAULT(set, RNE);
  ZERO64(ZEROCONST);
  FROMSTRING64(intermediate_euro, "0.001", &set);
  FROMSTRING64(ONE, "1", &set);
  FROMSTRING64(TEN, "10", &set);
  constructArray();
#ifdef COUNTING
  initialization();
#endif
  myfprintf(stderr, "Euro Conversion Starts\n");


  inp = myfopen(filein,"rb");
  if (inp == NULL) { // open file for reading
    myfclose(inp);
    myprintf("Error: file '%s' not found\n", filein);
    exit(0);
  }
  if ((oup=myfopen(fileou,"wb"))==NULL) { // open file for writing
    fclose(oup);
    printf("Error: file '%s' could not be opened\n", fileou);
    exit(0);
  }
  
  begin_time = rdtsc();

  while (!myfeof(inp)) {
#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
    #if defined(WIDTH64)
      decimal64 temp;
    #else
      decimal128 temp;
    #endif
#endif
    ret = myfread(&out.from, sizeof(int), 1, inp);
    ret = myfread(&out.to, sizeof(int), 1, inp);
#if !defined(HW) && defined(DECNUMBER_ARBITRARY)
    #if defined(WIDTH64)
      ret = myfread(&temp, sizeof(decimal64), 1, inp);
      decimal64ToNumber(&temp, &out.inp);
    #else
      ret = myfread(&temp, sizeof(decimal128), 1, inp);
      decimal128ToNumber(&temp, &out.inp);
    #endif
#else
    ret = myfread(&out.inp, sizeof(dec_t), 1, inp);
#endif
#ifdef DEBUG
    fprintf(stderr, "======== Data Read Checkpoint ========\n");
    decfprintf(stderr, 1, "Data read ", &out.inp);
#endif

    if (out.from==out.to) {
      COPY64(out.oup, out.inp);
    }
    else if (out.from==COUNTRY) {
      euro2ncu(out.to, &out.inp, &out.oup);
    }
    else if (out.to==COUNTRY) {
      ncu2euro(out.from, &out.oup, &out.inp);
    }
    else { 
      ncu2ncu(out.from, out.to, &out.inp, &out.oup);
    }
    fwrite(&out, sizeof(output_t), 1, oup);
#ifdef DEBUG
    fprintf(stderr, "======== Result Checkpoint ========\n");
    fprintf(stderr, "From %d to %d, ", out.from, out.to);
    decfprintf(stderr, 2, "(in, out)", &out.inp, &out.oup);
#endif
  }
  end_time = rdtsc();
  myfclose(inp);
  myfclose(oup);
  myfprintf(stderr, "Euro Conversion End Successfully\n");
  //myprintf("Total benchmark run_time: %lld\n", end_time - begin_time);
  total_time += (end_time - begin_time);
  myprintf("Total benchmark run_time: %lld\n", total_time); 
/*
  myprintf("%ld,", add_calls);
  myprintf("%ld\n", add_cycles);
  myprintf("%ld,", copy_calls);
  myprintf("%ld\n", copy_cycles);
  myprintf("%ld,", div_calls);
  myprintf("%ld\n", div_cycles);
  myprintf("%ld,", int_calls);
  myprintf("%ld\n", int_cycles);
  myprintf("%ld,", mul_calls);
  myprintf("%ld\n", mul_cycles);
  myprintf("%ld,", sub_calls);
  myprintf("%ld\n", sub_cycles);
  myprintf("%ld,", zero_calls);
  myprintf("%ld\n", zero_cycles);
*/
/*
  myprintf("Average cycles for ADD: %lld\n", add_cycles/add_calls);
  myprintf("Average cycles for COPY: %lld\n", copy_cycles/copy_calls);
  myprintf("Average cycles for DIV: %lld\n", div_cycles/div_calls);
  myprintf("Average cycles for INT: %lld\n", int_cycles/int_calls);
  myprintf("Average cycles for MUL: %lld\n", mul_cycles/mul_calls);
  myprintf("Average cycles for SUB: %lld\n", sub_cycles/sub_calls);
  myprintf("Average cycles for ZRO: %lld\n", zero_cycles/zero_calls);
*/

//  myprintf("Add rounded: %ld\n", add_rounded);
//  myprintf("%ld\n", add_calls);
//  myprintf("Mul rounded: %ld\n", mul_rounded);
//  myprintf("%ld\n", mul_calls);
//  myprintf("Div rounded: %ld\n", div_rounded);
//  myprintf("%ld\n", div_calls);

#if COUNTING
  decimal_printout(stdout);
#endif
} //counter
  return 0;
}
