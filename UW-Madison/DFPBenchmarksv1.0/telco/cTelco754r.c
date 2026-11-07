/* ------------------------------------------------------------------ */
/* The 'telco' 0.53 benchmark in C (using decNumber)                  */
/* This is the version with 'full optimization' (internal format      */
/*   everywhere).  See also cTelco754r1 and cTelco754r0.              */
/* ------------------------------------------------------------------ */
/* Copyright (c) IBM Corporation, 2001, 2005.  All rights reserved.   */
/* ------------------------------------------------------------------ */
/* Call as:                                                           */
/*                                                                    */
/*   cTelco754r [infile [outfile]] [flags]                            */
/*                                                                    */
/* where the parameters are:                                          */
/*   infile  -- input file name [default telco.testr]                 */
/*   outfile -- output file name [default telco.outc]                 */
/*   flags   -- may be before or after file names, and may be         */
/*                                                                    */
/*              -nocalc  -- omit all calculations for each number     */
/*              -notax   -- omit tax calculations (and summations)    */
/*                                                                    */
/* See 'http://www2.hursley.ibm.com/decimal/telco.html' for           */
/* background and details.                                            */
/*                                                                    */
/* ------------------------------------------------------------------ */
/* Note that a non-ANSI timer function is used; be prepared to modify */
/* 2003.04.28 -- changed initialization constant for decNumber 3.x    */
/* 2005.04.01 -- version for data in IEEE 754r decimal64 format       */

#include "const.h"
#include "dfp_includes.h"
#include "operator.h"
#include "dfp_datatypes.h"
#include "rdtsc.h"

#if defined(NATIVE_GCC_BID)
_IDEC_flags flags = 0x00;      // working flags 
#endif

#ifdef COUNTING
int mul_total, div_total, add_total, add_minmax_total, add_other_total;
int mul_rounded, div_rounded, add_rounded, add_other_rounded;
//the below one can run on fast pass adder
int add_no_exp_diff;
int add_minmax_no_exp_diff;
int add_other_no_exp_diff;
//this one may run on fast pass
int add_no_right_shifted_diff_exp;
int add_minmax_no_right_shifted_diff_exp;
int add_other_no_right_shifted_diff_exp;
// 
int add_op_zero;

//the below can run on fast pass mul
int mul_lt_eq_16_coeff_on_result;
//length of multiplier does matter because mul can be sequential base
int sig_length_multiplier[16];
#endif

// COUNTING MICHAEL!!!
long long add_cycles = 0;
long long mul_cycles = 0;
long long quant_cycles = 0;
long long zero_cycles = 0;

long long add_calls = 0;
long long mul_calls = 0;
long long quant_calls = 0;
long long zero_calls = 0;

long add_rounded = 0;
long int_rounded = 0;
long mul_rounded = 0;

unsigned long long count_begin_func;
unsigned long long count_end_func;
unsigned long long count_overhead;
// END COUNTING MICHAEL!!!


// ------------------------------------------------------------------
//static void showus(char *, struct timeb, struct timeb, int);

/* ------------------------------------------------------------------ */
/* main entry point                                                   */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[]) {

  dec_t sumT, sumB, sumD;      // sums
  dec_t n;                     // number from file
  dec_t p;                     // price
  dec_t b;                     // base tax
  dec_t d;                     // distance tax
  dec_t t;                     // total price
  dec_t baserate, distrate;    // call rates
  dec_t basetax, disttax;      // tax rates
  dec_t mtwo;                  // constant -2
  dec_t etwo;                  // constant with exponent=-2
  context_t set;                 // working context
 
#ifdef DECNUMBER_ARBITRARY
 #ifdef WIDTH64
  decimal64 in64;                // we read into this
 #else
  decimal128 in64;                // we read into this
 #endif
#else
  dec_t in64;			 // read into this
#endif

  char string[DECNUMDIGITS+14];    // conversion buffer
  int  i, j, numbers;              // counters
  FILE *inp=NULL;                  // input stream structure
  FILE *oup=NULL;                  // output stream structure
  char filein[MAXFILE+1];          // full input filename
  char fileou[MAXFILE+1];          // full output filename
  int  readlen;                    // read data length
  int  frc;                        // ferror return code
  int  calltype;                   // call type (0 or 1)
  int  calc=1;                     // 1 for calculations, 0 to skip
  int  tax=1;                      // 1 for tax calculations, 0 to skip
  //  struct timeb tbStart, tbFinis;   // time buffers
  unsigned int urc, len;           // work
  char **parm;                     // ..

#ifdef TIMING
    unsigned long long runtime = 0;
#endif

// loop 5 times
  int counter;
  for(counter = 0 ; counter < 10 ; counter++)
  {

  //char strTime[30], strPrice[30], strBtax[30], strDtax[30];
  // set up arithmetic context
  CONTEXTDEFAULT(set, RNE);
  //set.traps=0;                                    // no traps
  //set.digits=31;                                  // working precision

  // set up the call rates, tax rates and other constants
  FROMSTRING64(baserate, "0.0013", &set); // low call rate
  FROMSTRING64(distrate, "0.00894", &set);// high call rate
  FROMSTRING64(basetax,  "0.0675", &set); // base tax rate
  FROMSTRING64(disttax,  "0.0341", &set); // distance tax rate
  FROMSTRING64(mtwo,     "-2", &set);     // scale setting (for RESCALE)
  FROMSTRING64(etwo,     "1E-2", &set);     // scale setting (for QUANTIZE)

  // Get any parameters
  mystrcpy(filein, "telco.testr");             // default filenames
  mystrcpy(fileou, "telco.outc");              // ..

  j=0;                                       // parameter number
  for (i=0, parm=argv; i<argc ; parm++, i++) {
    if (mystrlen(*parm)>MAXFILE) {             // no buffer overruns, please
      myprintf("Parameter word too long (>%d characters): %s\n", MAXFILE, *parm);
      myexit(1); // abandon
    }
    if (i==0) ;                              // 0. is program name
    else if (*parm[0]=='-') {                // flag expected
      if (mystrcmp(*parm, "-nocalc")==0) calc=0;
      else if (mystrcmp(*parm, "-notax")==0) tax=0;
      else myprintf("Flag '%s' ignored\n", *parm);
    } 
    else {
      j++;                                   // have a parameter
      if (j==1) mystrcpy(filein, *parm);       // is input file name
      else if (j==2) mystrcpy(fileou, *parm);  // is output file name
      else myprintf("Extra parameter '%s' ignored\n", *parm);
    }
  } /* getting arguments */
  
  myprintf("telco C benchmark; processing '%s'\n", filein);
  
  myremove(fileou);                  // delete output file if it exists
  
  /* ---------------------------------------------------------------- */
  /* Benchmark timing starts here                                     */
  /* ---------------------------------------------------------------- */

  //ftime(&tbStart);                 // timestamp

  #ifdef TIMING
    unsigned long long start = 0;
    unsigned long long end = 0;
    start = rdtsc();
  #endif // TIMING
 
  ZERO64(sumT);            // zero accumulators
  ZERO64(sumB);
  ZERO64(sumD);

  inp=myfopen(filein,"rb");          // open file for reading, binary
  if (inp==NULL) {
    myprintf("Error: file '%s' not found\n", filein);
    myexit(0);
  }
  oup=myfopen(fileou,"wb");          // open file for writing, binary
  if (oup==NULL) {
    myfclose(inp);
    myprintf("Error: file '%s' could not be opened\n", fileou);
    myexit(0);
  }
  // From now on, files must be closed explicitly
  /* Start of the by-number loop */
  for (numbers=0; ; numbers++) {
    // get next 8-byte decimal64 number
    // or 16 byte decimal128 number
    #if defined(DECNUMBER_ARBITRARY) 
     #if defined(WIDTH64)
      readlen=myfread(in64.bytes, 1, DECIMAL64_Bytes, inp); // is this a bug?
     #else 
      readlen=myfread(in64.bytes, 1, DECIMAL128_Bytes, inp); // is this a bug?
     #endif
    #else // if BID or FIXED
     readlen=myfread(&in64, 1, sizeof(dec_t), inp); 
    #endif
    #if defined(WIDTH64)
      if (readlen<DECIMAL64_Bytes) {      // error or EOF
        frc=myferror(inp);                  // check for error
        // [note: ferror is single thread, but only this thread knows INP]
        if (frc!=0) myprintf("Error: cannot read file '%s'", filein);
        break;                            // [no message if EOF]
      }
    #else
      if (readlen<DECIMAL128_Bytes) {      // error or EOF
        frc=myferror(inp);                  // check for error
        // [note: ferror is single thread, but only this thread knows INP]
        if (frc!=0) myprintf("Error: cannot read file '%s'", filein);
        break;                            // [no message if EOF]
      }
    #endif
    if (calc!=0) {
      #if defined(BID)
        #if defined(WIDTH64)
          calltype = ((unsigned)in64) & 0x01;
        #else
          calltype = ((unsigned)in64.w[0]) & 0x01;
        #endif
      #elif defined(NATIVE_GCC_DPD) || defined(NATIVE_GCC_BID)
        {
          unsigned long long temp;
          memcpy(&temp, &in64, 8);
          calltype = temp & 0x01;
        }
      #else
        calltype=((unsigned)in64.bytes[0]) & 0x01; // last bit
      #endif
      // have packed decimal number; convert to working format
//#if !defined(HW) && !defined(DECNUMBER_FIXED) 
#if defined(DECNUMBER_ARBITRARY)
#if defined(WIDTH64)
      decimal64ToNumber(&in64, &n);
#else
      decimal128ToNumber(&in64, &n);
#endif
#else // DECNUMBER_ARBITRARY	 
   #if defined(HW)
      mymemcpy(&n.b, &in64, sizeof(decimal64));
   #else //defined(HW) (decdouble or bid)
      COPY64(n, in64);
   #endif //defined(HW)
#endif // !defined(HW) && !defined(DECNUMBER_FIXED) 
      if (calltype==0){                                 // p=r[c]*n
        MULTIPLY64(p, baserate, n, &set);
      }
      else {
        MULTIPLY64(p, distrate, n, &set);
      }
      SET_ROUNDMODE(set, RNE);
      QUANTIZE64(p, p, etwo, &set);           // to x.xx (converted RESCALE64 to QUANTIZE64 for P754 compliance - Apr2008)
      if (tax!=0) {
	SET_ROUNDMODE(set, RTZ);
        MULTIPLY64(b, p, basetax, &set);     // b=p*0.0675
        QUANTIZE64(b, b, etwo, &set);         // to x.xx (converted RESCALE64 to QUANTIZE64 for P754 compliance - Apr2008)
        ADD64(sumB, sumB, b, &set);          // sumB=sumB+b
        ADD64(t, p, b, &set);                // t=p+b
	if (calltype!=0) {
	  MULTIPLY64(d, p, disttax, &set);   // d=p*0.0341
	  QUANTIZE64(d, d, etwo, &set);       // to x.xx (converted RESCALE64 to QUANTIZE64 for P754 compliance - Apr2008)
	  ADD64(sumD, sumD, d, &set);        // sumD=sumD+d
	  ADD64(t, t, d, &set);              // t=t+d
        }
	else {
	  ZERO64(d);
	}
      } 
      else {
	t=p;
      }
      ADD64(sumT, sumT, t, &set);            // sumT=sumT+t
      TOSTRING64(t, string);
    } 
    else {
      mystrcpy(string, "0.77");
    }
    // [Adding CRLF is part of I/O, as other languages have WriteLine]
    mystrcat(string, "\r\n");                            // Add CRLF
    len=mystrlen(string);
    urc=myfwrite(string, 1, len, oup);
    if (urc!=len) {
      myprintf("Error: file '%s' could not be written", fileou);
      break;
    }
  } // numbers
  
  #ifdef TIMING
    end = rdtsc();
    runtime += end - start;
    printf(" === Benchmark runtime: [%llu cycles] ===\n", runtime);
  #endif // TIMING

  //printf("Number of Adds: %ld\n", add_calls);
  //printf("Total cycles spent in add: %ld\n", add_cycles);

  //printf("Number of Muls: %ld\n", mul_calls);
  //printf("Total cycles spent in mul: %ld\n", mul_cycles);

  //printf("Number of Quantizes: %ld\n", quant_calls);
  //printf("Total cycles spent in quant: %ld\n", quant_cycles);

  //printf("Number of Zeros: %ld\n", zero_calls);
  //printf("Total cycles spent in zero: %ld\n", zero_cycles);

  //printf("Add rounded: %ld\n", add_rounded);
  //printf("Int rounded: %ld\n", int_rounded);
  //printf("Mul rounded: %ld\n", mul_rounded);

/*
  printf("Average cycles per ADD: %lld\n", add_cycles/add_calls);
  printf("Average cycles per MUL: %lld\n", mul_cycles/mul_calls);
  printf("Average cycles per QNT: %lld\n", quant_cycles/quant_calls);
  printf("Average cycles per ZRO: %lld\n", zero_cycles/zero_calls);
  printf("Total cycles for ZRO: %lld\n", zero_cycles);
  printf("Total calls for ZRO: %lld\n", zero_calls);
  printf("Total cycles for ADD: %lld\n", add_cycles);
  printf("Total calls for ADD: %lld\n", add_calls);
  */

  if (oup!=NULL) myfclose(oup);
  if (inp!=NULL) myfclose(inp);
  myfflush(NULL);
  /* ---------------------------------------------------------------- */
  /* Benchmark timing ends here                                       */
  /* ---------------------------------------------------------------- */
  //ftime(&tbFinis);

  myprintf("-- telco C benchmark result --\n");
  //printf("   %d numbers read from '%s'\n", numbers, filein);
  //showus("Time per number", tbStart, tbFinis, numbers);
  myprintf("--\n");
  TOSTRING64(sumT, string);
  myprintf("   sumT = %s\n", string);
  TOSTRING64(sumB, string);
  myprintf("   sumB = %s\n", string);
  TOSTRING64(sumD, string);
  myprintf("   sumD = %s\n", string);
#ifdef COUNTING
  decimal_printout(stderr);
#endif

  } //counter

  return 0;
} // main

/* ------------------------------------------------------------------ */
/* Calculate and display time per iteration in microseconds           */
/*                                                                    */
/*   label is the benchmark identifier (up to 15 characters)          */
/*   time1 is the start time                                          */
/*   time2 is the finish time                                         */
/*   count is the iteration count for the benchmark                   */
/* ------------------------------------------------------------------ */
/*
static void showus(char *label, struct timeb time1, struct timeb time2,
                   int count) {
  int totalms;                     // milliseconds total
  double dtotus;                   // microseconds total (floating)
  double dcount;                   // count (floating)

  totalms=(time2.time-time1.time)*1000
    +time2.millitm-time1.millitm;
  dtotus=((double)totalms)*1000;
  dcount=(double)count;
  printf("   %-15s %7.3fus [%d]\n", label, dtotus/dcount, count);
} // showus

*/
