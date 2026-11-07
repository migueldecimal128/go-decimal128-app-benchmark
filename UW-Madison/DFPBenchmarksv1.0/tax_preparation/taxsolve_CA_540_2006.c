/************************************************************************/
/* TaxSolve_CA_540_2006.c - California state 540 tax form.		*/
/* Copyright (C) 2006 - Aston Roberts					*/
/* 									*/
/* Compile:   gcc taxsolve_CA_540_2006.c -o taxsolve_CA_540_2006	*/
/* Run:	      ./taxsolve_CA_540_2006  CA_540_2006.dat 			*/
/* 									*/
/* GNU Public License - GPL:						*/
/* This program is free software; you can redistribute it and/or	*/
/* modify it under the terms of the GNU General Public License as	*/
/* published by the Free Software Foundation; either version 2 of the	*/
/* License, or (at your option) any later version.			*/
/* 									*/
/* This program is distributed in the hope that it will be useful,	*/
/* but WITHOUT ANY WARRANTY; without even the implied warranty of	*/
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU	*/
/* General Public License for more details.				*/
/* 									*/
/* You should have received a copy of the GNU General Public License	*/
/* along with this program; if not, write to the Free Software		*/
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA		*/
/* 02111-1307 USA							*/
/* 									*/
/* Aston Roberts 1-6-2007	aston_roberts@yahoo.com			*/
/************************************************************************/

#include "dfp_datatypes.h"
#include "dfp_includes.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include "taxsolve_routines.h"
#include "operator.h"
#include "decAsm.h"
#include "counter.h"
#include "taxsolve_CA_540_2006.h"

#if defined(NATIVE_GCC_BID)
_IDEC_flags flags = 0x00;      // working flags 
#endif

#ifdef COUNTING
#include "decfcn.h"
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

void reset(){
  int i;
  mul_total = 0, div_total = 0, add_total = 0;
  add_minmax_total = 0, add_other_total = 0;
  mul_rounded = 0, div_rounded = 0, add_rounded = 0, add_other_rounded = 0;
  add_no_exp_diff = 0, add_minmax_no_exp_diff = 0, add_other_no_exp_diff = 0;
  add_no_right_shifted_diff_exp = 0, add_minmax_no_right_shifted_diff_exp = 0;
  add_op_zero = 0;
  add_other_no_right_shifted_diff_exp = 0;
  mul_lt_eq_16_coeff_on_result = 0;
  for (i=0; i<16; i++)
    sig_length_multiplier[i] =0;
}
#endif

/*----------------------------------------------------------------------*/
/* Main                                                                 */
/*----------------------------------------------------------------------*/

int main( int argc, char *argv[] )
{

    int i, loop, index;
    char outfname[1000];
    
    //-------------------------------------------------------------------------------------------
    //adding linux counter if simplescalar is not defined
#ifndef SIMPLESCALAR
#ifndef GPROF
#ifdef LINUX
    unsigned startl, starth, endl, endh;
    ASM_MACRO(startl, starth);
#endif //!LINUX
#endif//!GPROF
#endif//!SIMPLESCALAR
#ifdef COUNTING
    reset();
#endif
  
    //-------------------------------------------------------------------------------------------
    /* Decode any command-line arguments. */
    fprintf(stderr, "=== CA Tax Preparation Program starts ===\n");

    //-------------------------------------------------------------------------------------------
    //Initialize the decContext
    CONTEXTDEFAULT(set, RNE);
 
    //-------------------------------------------------------------------------------------------
    //Perform tax calculations
    macro_assign();
 
    //We can call the following + three functions multiple times for
    // different inputs

    //Open input and output files
    assert(argc==4);
    //argv[0]: CAStateTax executable name
    //argv[1]: input file name
    //argv[2]: output file number (to append to the output file name)
    //argv[3]: loop count

    infile = myfopen(argv[1], "rb");
    if (infile==0) {myprintf("ERROR: Parameter file '%s' could not be opened.\n", argv[1]); exit(1);}
    /* Base name of output file on input file. */
    mystrcpy(outfname, argv[2]);
    i = mystrlen(outfname)-1;
    while ((i>=0) && (outfname[i]!='.')) i--;
    if (i<0) mystrcat(outfname,".out"); else strcpy(&(outfname[i]),".out");
    outfile = myfopen(outfname,"w");
    if (outfile==0) {myprintf("ERROR: Output file '%s' could not be opened.\n", outfname); exit(1);}
#ifdef DEBUG
    printf("Writing results to file:  %s\n", outfname);
#endif
    loop = myatoi(argv[3]);

    for (index = 0; index<loop; index++){
#ifdef DEBUG
      if (index%1000==0)
        myfprintf(stderr, "Processing %d tax return\n", index+1);
#endif
        CONTEXTDEFAULT(set, RNE);
        initialization();
        action();
        ending();
    }
    
    myfclose(infile);
    myfclose(outfile);

    myprintf("=== CA Tax Preparation Program is done === \n");
    myprintf("=== Total Processing %d tax returns === \n", index);
    myprintf("=== Output is stored in %s\n", outfname);
#ifdef DEBUG
    myprintf("\nListing results from file: %s\n\n", outfname);
    Display_File( outfname );
#endif
#ifdef COUNTING
    decimal_printout(stderr);
#endif


    //-------------------------------------------------------------------------------------------
    //output linux counter data
#ifndef SIMPLESCALAR
#ifndef GPROF
#ifdef LINUX
    ASM_MACRO(endl, endh);
    double different;
    //special way to compute cycle count because the lower 32-bit register
    //will saturate if execution time is more than 7 second
    different = showsDiff(starth, startl, endh, endl);
    myprintf("[From LINUX Counter]cycle lapse: %f \n", different);
#endif //# UNIX
#endif // # !GPROF
#endif // # !SIMPLESCALAR
    return 0;
}

void macro_assign()
{
    ZERO64(ZERO);                              //      0
    FROMSTRING64(P_P01, "0.01", &set);         //      0.01
    ADD64(P_P02, P_P01, P_P01, &set);          //      0.02
    ADD64(P_P04, P_P02, P_P02, &set);          //      0.04
    ADD64(P_P06, P_P04, P_P02, &set);          //      0.06
    ADD64(P_P08, P_P04, P_P04, &set);          //      0.08
    FROMSTRING64(P_P093, "0.093", &set);       //      0.093
    FROMSTRING64(P_1, "1.0", &set);            //      1
    ADD64(P_2, P_1, P_1, &set);                //      2
    FROMSTRING64(P_6, "6.0", &set);            //      6
    FROMSTRING64(P_91, "91.0", &set);          //     91
    FROMSTRING64(P_285, "285.0", &set);        //    285
    FROMSTRING64(P_1250, "1250.0", &set);      //   1250
    ADD64(P_2500, P_1250, P_1250, &set);       //   2500
    FROMSTRING64(P_3410, "3410.0", &set);      //   3410
    FROMSTRING64(P_6622, "6622.0", &set);      //   6622
    ADD64(P_6820, P_3410, P_3410, &set);       //   6820
    FROMSTRING64(P_13244, "13244.0", &set);    //  13244 
    FROMSTRING64(P_13251, "13251.0", &set);    //  13251
    FROMSTRING64(P_15698, "15698.0", &set);    //  15698
    FROMSTRING64(P_24776, "24776.0", &set);    //  24776
    FROMSTRING64(P_31396, "31396.0", &set);    //  31396
    ADD64(P_31397, P_31396, P_1, &set);        //  31397
    FROMSTRING64(P_34394, "34394.0", &set);    //  34394
    FROMSTRING64(P_40473, "40473.0", &set);    //  40473
    FROMSTRING64(P_43467, "43467.0", &set);    //  43467
    FROMSTRING64(P_49552, "49552.0", &set);    //  49552
    FROMSTRING64(P_50090, "50090.0", &set);    //  50090
    FROMSTRING64(P_59166, "59166.0", &set);    //  59166
    FROMSTRING64(P_68788, "68788.0", &set);    //  68788
    FROMSTRING64(P_86934, "86934.0", &set);    //  86934
    FROMSTRING64(P_150743, "150743.0", &set);  // 150743
    FROMSTRING64(P_226119, "226119.0", &set);  // 226119
    FROMSTRING64(P_301491, "301491.0", &set);  // 301491
}


void initialization()
{
    char word[2000];
    time_t now;
    int i;
    
    /* Pre-initialize all lines to zeros. */
    for (i=0; i<MAX_LINES; i++) {
      ZERO64(L[i]);
    }

    //-------------------------------------------------------------------------------------------
    /* Accept Form's "Title" line, and put out with date-stamp for your records. */
    //read_line( infile, word );
    mystrcpy(word, "(c) Title:  CA State 540 Tax Form - 2006\n");
    now = time(0);
#ifdef PRINT_DATE_TIME
    myfprintf(outfile,"\n%s(c) 	%s\n", word, ctime( &now ));
#else
    myfprintf(outfile,"\n%s\n", word);
#endif
    
    //-------------------------------------------------------------------------------------------
    //Reading -- Status
    read_from_file();
    //switch (myatoi(TOSTRING64(line.amt,word))) {
    TOSTRING64(line.amt, word);
    switch (myatoi(word)) {
    case 1:
        status = SINGLE;
        mystrcpy(word, "Single");
        break;
    case 2:
        status = MARRIED_FILLING_JOINTLY;
        mystrcpy(word, "Married/Joint");
        break;
    case 3:
        status = MARRIED_FILLING_SEPARAT;
        mystrcpy(word, "Married/Sep");
        break;
    case 4:
        status = HEAD_OF_HOUSEHOLD;
        mystrcpy(word, "Head_of_House");
        break;
    case 5:
        status = WIDOW;
        mystrcpy(word, "Widow(er)");
        break;
    default:
        myprintf("Error: unrecognized status '%s'. Exiting.\n", word);
        exit(0);
    }
    myfprintf(outfile,"(c) Status = %s (%d)\n", word, status);
    myfprintf(outfile,"\n(c) Step-2 fill-in box %d\n", status );

    //-------------------------------------------------------------------------------------------
    //Reading -- Dependents
    read_from_file();	/* Are you a dependent? (yes/No). */
    COPY64(L[6], line.amt);
    
    read_from_file(); /* Blind?, 1 if you or spouse, 2 if both. */
    COPY64(iline8, line.amt);
    
    read_from_file(); /* Senior?, 1 if you or spouse, 2 if both. */
    COPY64(iline9, line.amt);

    read_from_file(); /* Number of Dependents. */
    COPY64(iline10, line.amt);

    //-------------------------------------------------------------------------------------------
    //Reading -- L12 - L43
    read_from_file();
    while (1) {
        if (mystrcmp(line.line_name, "L12") == 0) {
            /* State Wages (W2 box 16). */
            ADD64(L[12], L[12], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L13") == 0) {
            /* Fed Wages (Fed 1040 line 37). */
            ADD64(L[13], L[13], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L14") == 0) {
            /* CA Adjustments, Schedule CA 540 line 37 column B. */
            ADD64(L[14], L[14], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L16") == 0) {
            /* CA Adjustments, Schedule CA 540 line 37 column C. */
            ADD64(L[16], L[16], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L18") == 0) {
            /* CA itemized deductions Sched CA540 line 44. */
            ADD64(L[18], L[18], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L23") == 0) {
            ADD64(L[23], L[23], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L25") == 0) {
            /* Special credit 1 */
            ADD64(L[25], L[25], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L26") == 0) {
            /* Special credit 2 */
            ADD64(L[26], L[26], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L27") == 0) {
            /* Special credit 3+ */
            ADD64(L[27], L[27], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L28") == 0) {
            /* Nonrefundable renter's credit */
            ADD64(L[28], L[28], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L31") == 0) {
            /* Alternative minimum tax Sched P. */
            ADD64(L[31], L[31], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L32") == 0) {
            /* Mental Health Services Tax. */
            ADD64(L[32], L[32], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L33") == 0) {
            /* Other taxes and credit recapture. */
            ADD64(L[33], L[33], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L36") == 0) {
            /* CA income tax withheld. */
            ADD64(L[36], L[36], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L37") == 0) {
            /* Estimated tax paid. */
            ADD64(L[37], L[37], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L38") == 0) {
            /* Realestate withholding. */
            ADD64(L[38], L[38], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L39") == 0) {
            /* Excess SDI. */
            ADD64(L[39], L[39], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L43") == 0) {
            /* Child/dependent care credit. */
            ADD64(L[43], L[43], line.amt, &set);
	    break;
        }
        else { break; }
#ifdef DECNUMBER_ARBITRARY
 #ifdef WIDTH64
        size = myfread(&line64, sizeof(line64_t), 1, infile);
        if (myfeof(infile) != 0) break;
        if (size!=1){ myfprintf(stderr, "size written in is %d\n", size); exit(-1);}
        mystrcpy(line.line_name, line64.line_name);
        decimal64ToNumber(&line64.amt, &line.amt);
 #endif
 #ifdef WIDTH128
        size = myfread(&line128, sizeof(line128_t), 1, infile);
        if (myfeof(infile) != 0) break;
        if (size!=1){ myfprintf(stderr, "size written in is %d\n", size); exit(-1);}
        mystrcpy(line.line_name, line128.line_name);
        decimal128ToNumber(&line128.amt, &line.amt);
 #endif
#else
        size = myfread(&line, sizeof(line_t), 1, infile);
        if (myfeof(infile) != 0) break;
        if (size!=1){ myfprintf(stderr, "size written in is %d\n", size); exit(-1);}
#endif
    }
}

void action()
{
    //dec_t temp1, temp2, temp3;
    dec_t temp1, temp2;
    int lt1, lt2, lt3;

    if ((status==SINGLE) || (status==MARRIED_FILLING_SEPARAT) || (status==HEAD_OF_HOUSEHOLD))
        COPY64(iline7, P_1);
    else COPY64(iline7, P_2);
    if (ISZERO64(L[6])==0) {
      ZERO64(iline7); /* <-- Possible exceptions here. */
    }
    MULTIPLY64(L[7], P_91, iline7, &set);

    MULTIPLY64(L[8], iline8, P_91, &set);
    MULTIPLY64(L[9], iline9, P_91, &set);
    MULTIPLY64(L[10], iline10, P_285, &set);

    ADD64(temp1, L[7], L[8], &set);
    ADD64(L[11], L[9], L[10], &set);
    ADD64(L[11], temp1, L[11], &set);

    SUBTRACT64(L[15], L[13], L[14], &set);
    ADD64(L[17], L[15], L[16], &set);
    
    if ((status==SINGLE) || (status==MARRIED_FILLING_SEPARAT)) {
	LT64(lt1, L[18], P_3410, &set);
        if (lt1) COPY64(L[18], P_3410);

    }
    else {
	LT64(lt1, L[18], P_6820, &set);
        if (lt1) COPY64(L[18], P_6820);
    }
    
    SUBTRACT64(L[19], L[17], L[18], &set);
    if (ISNEGATIVE64(L[19])) {
      ZERO64(L[19]);
    } 
    
    /* Step 6. */
    TaxRateFunction( &L[20], L[19], status );

    LT64(lt1, P_150743, L[13], &set);
    LT64(lt2, P_301491, L[13], &set);
    LT64(lt3, P_226119, L[13], &set);
    if ( ( ((status==SINGLE) || (status==MARRIED_FILLING_SEPARAT)) && (lt1) ) ||
         ( ((status==MARRIED_FILLING_JOINTLY) || (status==WIDOW)) && (lt2) ) ||
         ( (status==HEAD_OF_HOUSEHOLD) && (lt3) )
         ) { // Line21-Exemption-credits-worksheet

        ZERO64(ws_b);

        COPY64(ws_a, L[13]);

        switch (status) {
        case SINGLE:  case MARRIED_FILLING_SEPARAT:  COPY64(ws_b, P_150743);  break;
        case MARRIED_FILLING_JOINTLY:  case WIDOW:   COPY64(ws_b, P_301491);  break;
        case HEAD_OF_HOUSEHOLD:                      COPY64(ws_b, P_226119);  break;
        }
        SUBTRACT64(ws_c, ws_a, ws_b, &set);

        if (status!=MARRIED_FILLING_SEPARAT) {DIVIDE64(ws_d, ws_c, P_2500, &set);}
        else                                 {DIVIDE64(ws_d, ws_c, P_1250, &set);}
        SET_ROUNDMODE(set, RNA);    // 0.5 rounds up
        TOINTEGRAL64(ws_d, ws_d, &set);
        SET_ROUNDMODE(set, RNE);  //defualt

        MULTIPLY64(ws_e, P_6, ws_d, &set);
        ADD64(temp1, iline7, iline8, &set);
        ADD64(ws_f, temp1, iline9, &set);
        MULTIPLY64(ws_g, ws_e, ws_f, &set);
        ADD64(temp1, L[7], L[8], &set);
        ADD64(ws_h, temp1, L[9], &set);
        SUBTRACT64(ws_i, ws_h, ws_g, &set);
        if (ISNEGATIVE64(ws_i)) {
          ZERO64(ws_i);
        }
        COPY64(ws_j, iline10);
        MULTIPLY64(ws_k, ws_e, ws_j, &set);
        COPY64(ws_l, L[10]);
        SUBTRACT64(ws_m, ws_l, ws_k, &set);
        if (ISNEGATIVE64(ws_m)) {
          ZERO64(ws_m);
        }
        ADD64(ws_n, ws_i, ws_m, &set);
        COPY64(L[21], ws_n);
    }
    else  COPY64(L[21], L[11]);

    SUBTRACT64(L[22], L[20], L[21], &set);
    if (ISNEGATIVE64(L[22])) {
      ZERO64(L[22]);
    }
    ADD64(L[24], L[22], L[23], &set);
    ADD64(temp1, L[25], L[26], &set);
    ADD64(temp2, L[27], L[28], &set);
    ADD64(L[29], temp1, temp2, &set);

    SUBTRACT64(temp1, L[24], L[29], &set);
    NotLessThanZero(&L[30], temp1);
    
    ADD64(temp1, L[30], L[31], &set);
    ADD64(temp2, L[32], L[33], &set);
    ADD64(L[34], temp1, temp2, &set);
    COPY64(L[35], L[34]); 	/*(next-page)*/

    ADD64(temp1, L[36], L[37], &set);
    ADD64(temp2, L[38], L[39], &set);
    ADD64(temp1, temp1, L[43], &set);
    ADD64(L[44], temp1, temp2, &set);

    //-------------------------------------------------------------------------------------------
    //Overpaid Tax / Tax Due
    LT64(lt1, L[35], L[44], &set);
    if (lt1){
        SUBTRACT64(L[45], L[44], L[35], &set);
    }
    else {
        SUBTRACT64(L[48], L[35], L[44], &set);
    }
}

void ending()
{
    //dec_t temp1, temp2, temp3;
    dec_t temp1;
    int lt1, lt2, lt3; 

    if (ISZERO64(L[6])) {
      myfprintf(outfile,"(c) L6 = no\n");
    }
    else{
      myfprintf(outfile,"(c) L6 = yes, (check box on line 6).\n");
    } 
    
    showline(7);
    showline(8);
    showline(9);
    showline(10);
    showline_wmsg(11,"Exemption amount");
    
    if (ISNEGATIVE64(L[15])) {
        MINUS64(temp1, L[15], &set);

#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
        {
            decimal64 myout;
            myfprintf(outfile,"(m) L15 = (");
            decimal64FromNumber(&myout, &temp1, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, ")\n");
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            myfprintf(outfile,"(m) L15 = (");
            decimal128FromNumber(&myout, &temp1, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, ")\n");
        }
 #endif 
#else
        myfprintf(outfile,"(m) L15 = (");
        myfwrite(&temp1, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, ")\n");
#endif
    }
    else showline(15);
    
    showline(18);
    showline_wmsg(19,"Taxable Income");		/* Taxable income. */

    myfprintf(outfile,"\n(c) Fill-in the 'Tax Table Shedule' circle.\n");
    showline( 20 );

    // IS THERE A BUG HERE??? We compute lt1, lt2, and lt3 but 
    // only use lt1. Similar process above uses all 3. 
    LT64(lt1, P_150743, L[13], &set);
    LT64(lt2, P_301491, L[13], &set);
    LT64(lt3, P_226119, L[13], &set);
    if ( ( ((status==SINGLE) || (status==MARRIED_FILLING_SEPARAT)) && (lt1) ) ||
         ( ((status==MARRIED_FILLING_JOINTLY) || (status==WIDOW)) && (lt1) ) ||
         ( (status==HEAD_OF_HOUSEHOLD) && (lt1) )
         ) { //Line21-Exemption-credits-worksheet
        myfprintf(outfile,"(c)  Doing AGI Limitations worksheet.\n");
        myfprintf(outfile,"(c)  AGI Worksheet:\n");
        
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
        {
            decimal64 myout;
            myfprintf(outfile,"(d)   a: ");
            decimal64FromNumber(&myout, &ws_a, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   b: ");
            decimal64FromNumber(&myout, &ws_b, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   c: ");
            decimal64FromNumber(&myout, &ws_c, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   d: ");
            decimal64FromNumber(&myout, &ws_d, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   e: ");
            decimal64FromNumber(&myout, &ws_e, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   f: ");
            decimal64FromNumber(&myout, &ws_f, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   g: ");
            decimal64FromNumber(&myout, &ws_g, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   h: ");
            decimal64FromNumber(&myout, &ws_h, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   i: ");
            decimal64FromNumber(&myout, &ws_i, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   j: ");
            decimal64FromNumber(&myout, &ws_j, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   k: ");
            decimal64FromNumber(&myout, &ws_k, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   l: ");
            decimal64FromNumber(&myout, &ws_l, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   m: ");
            decimal64FromNumber(&myout, &ws_m, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);

            myfprintf(outfile, "\n(d)   n: ");
            decimal64FromNumber(&myout, &ws_n, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\n");

            myfprintf(outfile,"(c)  Your exemptions may be limited. Used Exemptions-Credits-Worksheet for Line 21.\n");
            
            myfprintf(outfile,"(m)   WorkSheet[n]=");
            decimal64FromNumber(&myout, &ws_n, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile," (vs. L11=");
            decimal64FromNumber(&myout, &L[11], &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile,")\n");
        }
 #endif
 #ifdef WIDTH128
       {
            decimal128 myout;
            myfprintf(outfile,"(d)   a: ");
            decimal128FromNumber(&myout, &ws_a, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   b: ");
            decimal128FromNumber(&myout, &ws_b, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   c: ");
            decimal128FromNumber(&myout, &ws_c, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   d: ");
            decimal128FromNumber(&myout, &ws_d, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   e: ");
            decimal128FromNumber(&myout, &ws_e, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   f: ");
            decimal128FromNumber(&myout, &ws_f, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   g: ");
            decimal128FromNumber(&myout, &ws_g, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   h: ");
            decimal128FromNumber(&myout, &ws_h, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   i: ");
            decimal128FromNumber(&myout, &ws_i, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   j: ");
            decimal128FromNumber(&myout, &ws_j, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   k: ");
            decimal128FromNumber(&myout, &ws_k, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   l: ");
            decimal128FromNumber(&myout, &ws_l, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   m: ");
            decimal128FromNumber(&myout, &ws_m, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);

            myfprintf(outfile, "\n(d)   n: ");
            decimal128FromNumber(&myout, &ws_n, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\n");

            myfprintf(outfile,"(c)  Your exemptions may be limited. Used Exemptions-Credits-Worksheet for Line 21.\n");
            
            myfprintf(outfile,"(m)   WorkSheet[n]=");
            decimal128FromNumber(&myout, &ws_n, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile," (vs. L11=");
            decimal128FromNumber(&myout, &L[11], &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile,")\n");
        }
 #endif
#else
        myfprintf(outfile,"(d)   a: ");
        myfwrite(&ws_a, sizeof(dec_t), 1, outfile);
        
        myfprintf(outfile, "\n(d)   b: ");
        myfwrite(&ws_b, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   c: ");
        myfwrite(&ws_c, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   d: ");
        myfwrite(&ws_d, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   e: ");
        myfwrite(&ws_e, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   f: ");
        myfwrite(&ws_f, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   g: ");
        myfwrite(&ws_g, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   h: ");
        myfwrite(&ws_h, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   i: ");
        myfwrite(&ws_i, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   j: ");
        myfwrite(&ws_j, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   k: ");
        myfwrite(&ws_k, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   l: ");
        myfwrite(&ws_l, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   m: ");
        myfwrite(&ws_m, sizeof(dec_t), 1, outfile);

        myfprintf(outfile, "\n(d)   n: ");
        myfwrite(&ws_n, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n");

        myfprintf(outfile,"(c)  Your exemptions may be limited. Used Exemptions-Credits-Worksheet for Line 21.\n");

        myfprintf(outfile,"(m)   WorkSheet[n]=");
        myfwrite(&ws_n, sizeof(dec_t), 1, outfile);
        myfprintf(outfile," (vs. L11=");
        myfwrite(&L[11], sizeof(dec_t), 1, outfile);
        myfprintf(outfile,")\n");
#endif
    }
    showline(21);
    showline(22);
    showline(23);		/* Taxes on distributions (sched G-1 or form FTB 5870A) */
    showline(24);
    showline(29);			/* Total credits. */
    showline(30);
    showline_wmsg(34,"Total Tax");	/* Total tax. */
    showline(35);
    showline_wmsg(44,"Total Payments");
    LT64(lt1, L[35], L[44], &set);
    if (lt1) {
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
        {
            decimal64 myout;
            myfprintf(outfile,"(m) L45 = ");
            decimal64FromNumber(&myout, &L[45], &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "  REBATE!!!\n");
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            myfprintf(outfile,"(m) L45 = ");
            decimal128FromNumber(&myout, &L[45], &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "  REBATE!!!\n");
        }
 #endif
#else
        myfprintf(outfile,"(m) L45 = ");
        myfwrite(&L[45], sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "  REBATE!!!\n");
#endif
    }
    else {
#if defined(DECNUMBER_ARBITRARY)        
 #if defined(WIDTH64)
        {
            decimal64 myout;
            myfprintf(outfile,"(m) L47 = ");
            decimal64FromNumber(&myout, &L[47], &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "  YOU OWE!!!\n");
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            myfprintf(outfile,"(m) L47 = ");
            decimal128FromNumber(&myout, &L[47], &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "  YOU OWE!!!\n");
        }
 #endif
#else
        myfprintf(outfile,"(m) L47 = ");
        myfwrite(&L[47], sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "  YOU OWE!!!\n");
#endif
    }
    
    myfprintf(outfile,"\n(c) Select any charity contributions (step 10) and complete form accordingly.\n");

}

void read_from_file()
{
#if defined(DECNUMBER_ARBITRARY)        
 #if defined(WIDTH64)
    size = myfread(&line64, sizeof(line64_t), 1, infile);
    if (size!=1){ myfprintf(stderr, "size written in is %d\n", size); exit(-1);}
    mystrcpy(line.line_name, line64.line_name);
    decimal64ToNumber(&line64.amt, &line.amt);
 #endif
 #if defined(WIDTH128)
    size = myfread(&line128, sizeof(line128_t), 1, infile);
    if (size!=1){ myfprintf(stderr, "size written in is %d\n", size); exit(-1);}
    mystrcpy(line.line_name, line128.line_name);
    decimal128ToNumber(&line128.amt, &line.amt);
 #endif
#else
	size = myfread(&line, sizeof(line_t), 1, infile);
	if (size!=1){ myfprintf(stderr, "size written in is %d\n", size); exit(-1);}
#endif
}

void TaxRateFormula( dec_t * tax, dec_t x, int status )
{     /* Values taken from page 26 of CA personal income tax booklet 2006 */
    dec_t brkpt[3][6], txrt[6], sum, temp1;
    int lt1;
    int srow = -1, bracket = 0;
    
    ZERO64(sum);

    /* Single OR Married, filing seperate. */
    ZERO64(brkpt[0][0]);
    COPY64(brkpt[0][1], P_6622);
    COPY64(brkpt[0][2], P_15698);
    COPY64(brkpt[0][3], P_24776);
    COPY64(brkpt[0][4], P_34394);
    COPY64(brkpt[0][5], P_43467);
    
    /* Married, filing jointly OR Widow(er). */
    ZERO64(brkpt[1][0]);
    COPY64(brkpt[1][1], P_13244);
    COPY64(brkpt[1][2], P_31396);
    COPY64(brkpt[1][3], P_49552);
    COPY64(brkpt[1][4], P_68788);
    COPY64(brkpt[1][5], P_86934);
    
    /* Head of Household. */
    ZERO64(brkpt[2][0]);
    COPY64(brkpt[2][1], P_13251);
    COPY64(brkpt[2][2], P_31397);
    COPY64(brkpt[2][3], P_40473);
    COPY64(brkpt[2][4], P_50090);
    COPY64(brkpt[2][5], P_59166);

    /* Tax Rate */
    COPY64(txrt[0], P_P01);
    COPY64(txrt[1], P_P02);
    COPY64(txrt[2], P_P04);
    COPY64(txrt[3], P_P06);
    COPY64(txrt[4], P_P08);
    COPY64(txrt[5], P_P093);
    
    switch (status) {
    case SINGLE:                  srow = 0; break;
    case MARRIED_FILLING_JOINTLY: srow = 1; break;
    case MARRIED_FILLING_SEPARAT: srow = 0; break;
    case HEAD_OF_HOUSEHOLD:       srow = 2; break;
    case WIDOW:                   srow = 1; break;
    };
    LT64(lt1, brkpt[srow][bracket+1], x, &set);
    while (lt1) {
        SUBTRACT64(temp1, brkpt[srow][bracket+1], brkpt[srow][bracket], &set);
        MULTIPLY64(temp1, temp1, txrt[bracket], &set);
        ADD64(sum, sum, temp1, &set);
        bracket = bracket + 1;
        if (bracket == 5) break;
        LT64(lt1, brkpt[srow][bracket+1], x, &set);
	
    }

    SUBTRACT64(temp1, x, brkpt[srow][bracket], &set);
    MULTIPLY64(temp1, temp1, txrt[bracket], &set);
    ADD64((*tax), temp1, sum, &set);
}


void TaxRateFunction( dec_t * tax, dec_t income, int status )
{
    TaxRateFormula( tax, income, status );

    //convert tax from float to int
    SET_ROUNDMODE(set, RNA);    // 0.5 rounds up
    TOINTEGRAL64((*tax), (*tax), &set);
    SET_ROUNDMODE(set, RNE);  //defualt
}


