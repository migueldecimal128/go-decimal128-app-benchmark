/************************************************************************/
/* TaxSolve_NC_D400_2006.c - North Carolina 2006 NC-DC400 State Taxes.	*/
/* Copyright (C) 2004, 2005 - S.Jenkins					*/
/* 									*/
/* Compile:   gcc taxsolve_NC_D400_2006.c -o taxsolve_NC_D400_2005	*/
/* Run:  ./taxsolve_NC_D400_2006		  			*/
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
/* 1-30-2004	http://opentaxsolver.sourceforge.com/			*/
/* 2-8-2005     Updated for 2004 tax year.				*/
/* 3-15-2006    Updated for 2005 tax year.				*/
/* 1-7-2007     Updated for 2006 tax year.                              */
/*                  by Lincoln Baxter (lab@lincolnbaxter.com)           */
/*                                                                      */
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
#include "taxsolve_NC_D400_2006.h"
#include "decfcn.h"

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
    myfprintf(stderr, "=== NC Tax Preparation Program starts ===\n");

    //-------------------------------------------------------------------------------------------
    //Initialize the decContext
    CONTEXTDEFAULT(set, RNE);

    //-------------------------------------------------------------------------------------------
    //Perform tax calculations
    macro_assign();
    
    //Open input and output files
    assert(argc==4);
    //argv[0]: NCStateTax executable name
    //argv[1]: input file name
    //argv[2]: output file number (to append to the output file name)
    //argv[3]: loop count


    infile = myfopen(argv[1], "rb");
    if (infile==0) {myprintf("ERROR: Parameter file '%s' could not be opened.\n", argv[1]); exit(1);}
    /* Base name of output file on input file. */
    mystrcpy(outfname, argv[2]);
    i = mystrlen(outfname)-1;
    while ((i>=0) && (outfname[i]!='.')) i--;
    if (i<0) mystrcat(outfname,".out"); else mystrcpy(&(outfname[i]),".out");
    outfile = myfopen(outfname,"w");
    if (outfile==0) {myprintf("ERROR: Output file '%s' could not be opened.\n", outfname); exit(1);}
#ifdef DEBUG
    myprintf("Writing results to file:  %s\n", outfname);
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
    myprintf("=== NC Tax Preparation Program is done === \n");
    myprintf("=== Total Processing %d tax returns === \n", index);
    myprintf("=== Output is stored in %s\n", outfname);

#ifdef DEBUG
    myprintf("\nListing results from file: %s\n\n", outfname);
    Display_File( outfname );
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
#ifdef COUNTING
    decimal_printout(stderr);
#endif
    return 0;
}

void macro_assign()
{
    dec_t temp;
    
    ZERO64(ZERO);                              //      0
    FROMSTRING64(P_P0007, "0.0007", &set);     //      0.0007
    FROMSTRING64(P_P06, "0.06", &set);         //      0.06
    FROMSTRING64(P_P07, "0.07", &set);         //      0.07
    FROMSTRING64(P_P0775, "0.0775", &set);     //      0.0775
    FROMSTRING64(P_P0825, "0.0825", &set);     //      0.0825
    FROMSTRING64(P_P3333, "0.3333", &set);     //      0.3333
    FROMSTRING64(P_P9, "0.9", &set);           //      0.9
    FROMSTRING64(P_1, "1.0", &set);            //      1
    FROMSTRING64(P_25, "25.0", &set);          //     25
    ADD64(P_50, P_25, P_25, &set);             //     50
    FROMSTRING64(P_300, "300.0", &set);        //    300
    ADD64(P_600, P_300, P_300, &set);          //    600
    FROMSTRING64(P_637P5, "637.5", &set);      //    637.5
    FROMSTRING64(P_765, "765.0", &set);        //    765
    FROMSTRING64(P_800, "800.0", &set);        //    800
    FROMSTRING64(P_1000, "1000.0", &set);      //   1000
    FROMSTRING64(P_1020, "1020.0", &set);      //   1020
    FROMSTRING64(P_1275, "1275.0", &set);      //   1275
    ADD64(P_1300, P_1000, P_300, &set);        //   1300
    FROMSTRING64(P_3000, "3000.0", &set);      //   3000
    FROMSTRING64(P_3393P75, "3393.75", &set);  //   3393.75
    ADD64(P_3600, P_3000, P_600, &set);        //   3600
    FROMSTRING64(P_3750, "3750.0", &set);      //   3750
    FROMSTRING64(P_4072P5, "4072.5", &set);    //   4072.5
    FROMSTRING64(P_4200, "4200.0", &set);      //   4200
    FROMSTRING64(P_4400, "4400.0", &set);      //   4400
    ADD64(P_4500, P_4200, P_300, &set);        //   4500
    ADD64(P_4800, P_4500, P_300, &set);        //   4800
    ADD64(temp, P_4800, P_300, &set);          //   5100
    ADD64(P_5150, temp, P_50, &set);           //   5150
    ADD64(P_5400, temp, P_300, &set);          //   5400
    FROMSTRING64(P_5430, "5430.0", &set);      //   5430
    ADD64(P_5900, temp, P_800, &set);          //   5900
    ADD64(P_6000, P_5400, P_600, &set);        //   6000
    ADD64(P_6600, P_6000, P_600, &set);        //   6600
    FROMSTRING64(P_6787P5, "6787.5", &set);    //   6787.5
    ADD64(P_7200, P_6600, P_600, &set);        //   7200
    FROMSTRING64(P_7268P75, "7268.75", &set);  //   7268.75
    ADD64(P_7800, P_7200, P_600, &set);        //   7800
    ADD64(P_8400, P_7800, P_600, &set);        //   8400
    FROMSTRING64(P_8722P5, "8722.5", &set);    //   8722.5
    DIVIDE64(P_10625, P_637P5, P_P06, &set);   //  10625
    FROMSTRING64(P_11630, "11630.0", &set);    //  11630
    DIVIDE64(P_12750, P_765, P_P06, &set);     //  12750
    FROMSTRING64(P_14537P5, "14537.5", &set);  //  14537.5
    DIVIDE64(P_17000, P_1020, P_P06, &set);    //  17000
    ADD64(temp, P_17000, P_3000, &set);        //  20000
    DIVIDE64(P_21250, P_1275, P_P06, &set);    //  21250
    FROMSTRING64(P_50000, "50000.0", &set);    //  50000
    FROMSTRING64(P_60000, "60000.0", &set);    //  60000
    FROMSTRING64(P_68000, "68000.0", &set);    //  68000
    ADD64(P_80000, temp, P_60000, &set);     //  80000
    ADD64(P_100000, P_50000, P_50000, &set);   // 100000
    ADD64(P_120000, P_60000, P_60000, &set);   // 120000
    ADD64(P_160000, P_80000, P_80000, &set);   // 160000
    ADD64(P_200000, P_100000, P_100000, &set); // 200000
}

void initialization()
{
    
    int i;
    dec_t Exception;
    char word[1000];
    time_t now;
    
    /* Pre-initialize all lines to zeros. */
    for (i=0; i<MAX_LINES; i++) ZERO64(L[i]);
    ZERO64(AB);
    ZERO64(AGI);
    ZERO64(ws2);
    ZERO64(ws10);
    
    //-------------------------------------------------------------------------------------------
    /* Accept Form's "Title" line, and put out with date-stamp for your records. */
    //read_line( infile, word );
    mystrcpy(word, "(c) Title:  NC State 540 Tax Form - 2006\n");
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
    
    
    //-------------------------------------------------------------------------------------------
    //Reading -- Exception
    read_from_file();
    COPY64(Exception, line.amt);
    //Note that the value of Exception has to be "no" or "0". The "yes" or "1" case
    // is not handled in this code.

    //Reading -- Exemptions
    read_from_file();
    COPY64(Exemptions, line.amt);

    //Reading -- Itemized
    read_from_file();
    COPY64(Itemized, line.amt);

    //Reading -- Limited
    read_from_file();
    COPY64(Limited, line.amt);

    //Reading -- LimitedL6
    read_from_file();
    COPY64(LimitedL6, line.amt);

    
    //get_parameter( infile, 's', word, "Exemptions" );
    //get_parameter( infile, 'i', &Exemptions, "Exemptions ?");

    //get_parameter( infile, 's', word, "Itemized" );
    //get_parameter( infile, 'b', &Itemized, "Itemized ?"); 

    //get_parameter( infile, 's', word, "Limited" );
    //get_parameter( infile, 'b', &Limited, "Limited ?"); 

    //get_parameter( infile, 's', word, "LimitedL6" );
    //get_parameter( infile, 'b', &LimitedL6, "LimitedL6 ?"); 

    
    //-------------------------------------------------------------------------------------------
    //Reading -- WS2, WS10, AGI, L6 - L46
    read_from_file();
    while (1) {
        if (mystrcmp(line.line_name, "WS2") == 0) {
            ADD64(ws2, ws2, line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "WS10") == 0) {
            ADD64(ws10, ws10, line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "AGI") == 0) {
            /* Adjusted Gross Income */
            ADD64(AGI, AGI, line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L6") == 0) {
            /* Taxable income from Fed1040 Line 43. */
            ADD64(L[6], L[6], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L12") == 0) {
            /* Part-year residents, amnt from line 50, else 1.0 */
            ADD64(L[12], L[12], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L15") == 0) {
            /* Tax credits. (D-400TC part 4 line 32) */
            ADD64(L[15], L[15], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L17") == 0) {
            /* Consumer Use Tax. (pg 6) */
            ADD64(L[17], L[17], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L19") == 0) {
            /* North Carolina Income Tax Withheld */
            ADD64(L[19], L[19], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L20") == 0) {
            /* Other tax payments. */
            ADD64(L[20], L[20], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L29") == 0) {
            /* Itemized deductions or std. deduction from Fed1040 line 40. */
            ADD64(L[29], L[29], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L32") == 0) {
            /* State and local income taxes on Sched A line 5, 8 taken from Sched A if not defined */
            ADD64(L[32], L[32], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L35") == 0) {
            /* Interest income from non-NC states. */
            ADD64(L[35], L[35], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L36") == 0) {
            /* Additional 1st yr deprec. adjustment */
            ADD64(L[36], L[36], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L37") == 0) {
            /* Other additions to fed income */
            ADD64(L[37], L[37], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L39") == 0) {
            /* Taxable refunds. (pg 24, 1099-G, etc.) from US 1040 line 10 */
            ADD64(L[39], L[39], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L40") == 0) {
            /* Interest from US or NC obligations. */
            ADD64(L[40], L[40], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L41") == 0) {
            /* Taxable soc sec and RR benfits. */
            ADD64(L[41], L[41], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L42") == 0) {
            /* Retirement benefits (Bailey)*/
            ADD64(L[42], L[42], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L43") == 0) {
            /* Other retirement benefits */
            ADD64(L[43], L[43], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L44") == 0) {
            /* Severance wages (See Line instructions on Page 13) */
            ADD64(L[44], L[44], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L45") == 0) {
            /* Adjust additional 1st yr deprec. 2002, 2003, and 2004 */
            ADD64(L[45], L[45], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L46") == 0) {
            /* Other deductions from fed income */
            ADD64(L[46], L[46], line.amt, &set);
            break;
        }
        else { break; }
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
        size = myfread(&line64, sizeof(line64_t), 1, infile);
        if (myfeof(infile) != 0) break;
        if (size!=1){ myfprintf(stderr, "size written in is %d\n", size); exit(-1);}
        mystrcpy(line.line_name, line64.line_name);
        decimal64ToNumber(&line64.amt, &line.amt);
 #endif
 #if defined(WIDTH128)
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
    int i;
    dec_t temp1, temp2, temp3, temp4;
    int lt1, lt2, eq1;
    
    /* Exemptions calculation of Deductions */
    
    if (ISZERO64(Exemptions) || ISNEGATIVE64(Exemptions)) { /* calculation of std Deductions */
        switch (status) {
        case 1: COPY64(L[30], P_3000); break;  /* NC std single deduction. */
        case 2: COPY64(L[30], P_6000); break;  /* NC std Married/joint deduction. */
        case 3: COPY64(L[30], P_3000); break;  /* NC std Married/sep deduction. */
        case 4: COPY64(L[30], P_4400); break;  /* NC std Head of house deduction. */
        case 5: COPY64(L[30], P_6000); break;  /* NC std widow(er) deduction. */
        }
    }
    else {
        char word[2000];
        int exemp;
        //exemp = myatoi(TOSTRING64(Exemptions,word));
        TOSTRING64(Exemptions, word);
	exemp = myatoi(word);
        
        switch (status) {
        case 1: /* NC single deduction. */
            switch ( exemp ) {
            case 1: COPY64(L[30], P_3750); break;
            case 2: COPY64(L[30], P_4500); break;
            }
            break;
        case 2: /* NC Married/joint deduction. */
        case 5: /* NC widow(er) deduction. */
            switch ( exemp ) {
            case 1: COPY64(L[30], P_6600); break;
            case 2: COPY64(L[30], P_7200); break;
            case 3: COPY64(L[30], P_7800); break;
            case 4: COPY64(L[30], P_8400); break;
            }
            break;
        case 3: /* NC Married/sep deduction. */
            switch ( exemp ) {
            case 1: COPY64(L[30], P_3600); break;
            case 2: COPY64(L[30], P_4200); break;
            case 3: COPY64(L[30], P_4800); break;
            case 4: COPY64(L[30], P_5400); break;
            }
            break;
        case 4: /* NC Head of house deduction. */
            switch ( exemp ) {
            case 1: COPY64(L[30], P_5150); break;
            case 2: COPY64(L[30], P_5900); break;
            }
            break;
        }
    }
    
    SUBTRACT64(L[31], L[29], L[30], &set);
    //L[31] = L[29] - L[30];
    
    if (ISNEGATIVE64(L[31])) ZERO64(L[31]);
    //if (L[31]<0.0) L[31] = 0.0;
    
    if (ISZERO64(Itemized)) ZERO64(L[32]);
    //if (Itemized == 0)
    //    L[32] = 0.0;
    
    LT64(lt1, L[31], L[32], &set);
    if (ISZERO64(L[32]) || lt1)
        COPY64(L[33], L[31]);
    else 
        COPY64(L[33], L[32]);
    
    
    //if (L[32]==0.0)          L[33] = L[31];
    //else if (L[31] < L[32])  L[33] = L[31];  
    //else                     L[33] = L[32];
    
    switch (status) {
    case 1: COPY64(AB,  P_60000); break;  /* NC std single */
    case 2: COPY64(AB, P_100000); break;  /* NC std Married/joint */
    case 3: COPY64(AB,  P_50000); break;  /* NC std Married/sep */
    case 4: COPY64(AB,  P_80000); break;  /* NC std Head of house */
    case 5: COPY64(AB, P_100000); break;  /* NC std widow(er) */
    }
    
    LT64(lt1, AGI, AB, &set);
    if (lt1) { // Compare Fed AGI
        /*A*/
        MULTIPLY64(L[34], Exemptions, P_800, &set);	/* Check instructions pg 11 */
        //L[34] = Exemptions * 800.0;	/* Check instructions pg 11 */
    } /*A*/
    else  { /*B*/
        MULTIPLY64(L[34], Exemptions, P_1300, &set);	/* Check instructions pg 11 */
        //L[34] = Exemptions * 1300.0;	/* Check instructions pg 11 */
        if (ISZERO64(Limited) == 0) {
            //if ( Limited ) {
            dec_t ws[MAX_LINES];
            for (i=0; i<MAX_LINES; i++) ZERO64(ws[i]);
            COPY64(ws[2], L[34]);
            
            if (ISZERO64(LimitedL6)) {
                COPY64(ws[3], ws10);
                COPY64(ws[4], ws2);
                DIVIDE64(ws[5], ws[3], ws[4], &set);
                MULTIPLY64(ws[6], ws[2], ws[5], &set);
                SET_ROUNDMODE(set, RNA);    // 0.5 rounds up
                TOINTEGRAL64(ws[6], ws[6], &set);
                SET_ROUNDMODE(set, RNE);  //defualt
                COPY64(L[34], ws[6]);
            }
            else {
                MULTIPLY64(ws[7], ws[2], P_P3333, &set);
                SET_ROUNDMODE(set, RNA);    // 0.5 rounds up
                TOINTEGRAL64(ws[7], ws[7], &set);
                SET_ROUNDMODE(set, RNE);  //defualt
                //ws[7] = (int) ws[2] * .3333 + 0.5;
                COPY64(L[34], ws[7]);
            }
        }
    } /*B*/
    

    ADD64(temp1, L[33], L[34], &set);
    ADD64(temp2, L[35], L[36], &set);
    ADD64(temp1, temp1, L[37], &set);
    ADD64(L[38], temp1, temp2, &set);
    //L[38] = L[33] + L[34] + L[35] + L[36] + L[37];	 /* Total additions. */
    
    COPY64(L[7], L[38]);			/* Additions to Federal Taxable Income. */
    
    ADD64(L[8], L[6], L[7], &set);
    
    ADD64(temp1, L[39], L[40], &set);
    ADD64(temp2, L[41], L[42], &set);
    ADD64(temp3, L[43], L[44], &set);
    ADD64(temp4, L[45], L[46], &set);
    ADD64(temp1, temp1, temp2, &set);
    ADD64(temp2, temp3, temp4, &set);
    ADD64(L[47], temp1, temp2, &set);
    //L[47] = L[39] + L[40] + L[41] + L[42] + L[43] + L[44] + L[45] + L[46];
    
    COPY64(L[9], L[45]);			/* Deductions from federal taxable income. */	
    
    SUBTRACT64(L[10], L[8], L[9], &set);
    
    COPY64(L[11], L[10]);
    
    MULTIPLY64(L[13], L[12], L[11], &set);
    
    LT64(lt1, L[13], P_68000, &set);
    EQ64(eq1, L[13], P_68000, &set);
    if (lt1 || eq1) {
        //if (L[13] <= 68000.0) {
        dec_t m;
        //dec_t x;

 
        
        DIVIDE64(m, L[13], P_50, &set); /* Round income to nearest $50. */
        SET_ROUNDMODE(set, RTZ);    // 0.5 rounds up
        TOINTEGRAL64(m, m, &set);
        SET_ROUNDMODE(set, RNE);  //defualt
        MULTIPLY64(m, P_50, m, &set);
        ADD64(m, m, P_25, &set);
       
        TaxRateFunction(&L[14], m, status);     /* Lookup. */
    

        SET_ROUNDMODE(set, RNA);    // 0.5 rounds up
        TOINTEGRAL64(L[14], L[14], &set);
        SET_ROUNDMODE(set, RNE);  //defualt
        //m = L[13] / 50.0;    /* Round income to nearest $50. */
        //x = TaxRateFunction( 50.0 * (float)m + 25.0, status );       /* Lookup. */
        //m = (int)(x + 0.5);  /* Round result to whole dollar. */
        //L[14] = (float)m;
    }
    else TaxRateFunction( &L[14], L[13], status );
    
    SUBTRACT64(L[16], L[14], L[15], &set);
    
    /* calculate USE tax if not entered on L17
     * based on Use Tax Worksheet 2 on page 8
     * only handle default case of purchases < 1000
     * and no taxes paid to another state
     *
     * if you have purchases greater that 1000 that need
     * to be reported, or taxes paid to another state, then
     * fill out the Work Sheet in the
     * instructions and enter the amount on L17 in 
     * data file
     */
    
    if (ISZERO64(L[17])) {
        //if ( L[17] == 0.0 ) {
        dec_t ws[MAX_LINES];
        for (i=0; i<MAX_LINES; i++) ZERO64(ws[i]);
        
        //printf( "Calculating USE tax based on income (no records, and no out of state purchases) (page 8)\n" );
        
        MULTIPLY64(ws[1], L[13], P_P0007, &set);
        SET_ROUNDMODE(set, RNA);    // 0.5 rounds up
        TOINTEGRAL64(ws[1], ws[1], &set);
        SET_ROUNDMODE(set, RNE);  //defualt
        //ws[1] = (int) (L[13] * 0.0007) + 0.5;
        
        COPY64(ws[5], ws[1]);
        COPY64(L[17], ws[5]); 
    }
    //else { 
    //printf( "Assuming you have calculated your USE tax (%2.2f)\n" ,L[17] );
    //}
    
    
    ADD64(L[18], L[16], L[17], &set);
    
    ADD64(L[21], L[19], L[20], &set);

    
    LT64(lt1, L[21], L[18], &set);
    if (lt1) {
        //if (L[18] > L[21]) {

        dec_t min_payment;
        ZERO64(min_payment);
        
        //float min_payment = 0.0; 
        SUBTRACT64(L[22], L[18], L[21], &set);	/* You OWE */

        /* check for under payment see form D422 Part I */
        MULTIPLY64(min_payment, L[18], P_P9, &set); /* min est tax payments see form D422 Part I */

	LT64(lt1, L[21], min_payment, &set);
        LT64(lt2, P_1000, L[22], &set);
        if ((lt1) && (lt2)) {
	    //if ( L[21] < min_payment && L[22] > 1000.00 ) {
            /* we would calc penalty here... */
            //printf("ERROR: underpayment of est. taxes underpayment panalty calculation not performed\n"); exit(1);
        }
        COPY64(L[23], L[22]);  /* Assumes no penalties. */
    }
    else {
        SUBTRACT64(L[24], L[21], L[18], &set);
        ZERO64(L[25]);  /* Apply estimated 2006 tax. */
        ZERO64(L[26]);  /* Wildlife fund. */
        ADD64(L[27], L[25], L[26], &set);
        SUBTRACT64(L[28], L[24], L[27], &set);	/* REFUND */
    } 
}



void ending()
{
    //dec_t temp1;
    int lt1;
    int k;
    
    showline(6);	/* Taxable fed income */
    showline(7);	/* Additions to fed income */
    showline(8);	
    showline(9);	/* Deductions */
    showline(10);
    showline(11);
    LT64(lt1, L[12], P_1, &set);
    if (lt1) showline(12);	// Part-yr
    //if (L[12]<1.0) showline(12);	/* Part-yr */
    showline_wmsg(13,"North Carolina Taxable Income");
    showline_wmsg(14,"North Carolina Income Tax");
    showline(15);
    showline(16);
    showline(17);
    showline(18);
    showline_wmsg(19,"North Carolina Tax Withheld");
    showline_wmsg(20,"Other Tax Payments");
    showline(21);
    LT64(lt1, L[21], L[18], &set);
    if (lt1) {
        //if (L[18] > L[21]) {
        showline(22);
        showline_wmsg(23,"PAY THIS AMOUNT");
    }
    else {
        showline(24);
        showline(25);
        showline(26);
        showline(27);
        showline_wmsg(28,"AMOUNT TO BE REFUNDED");
    }

    for (k=29; k<=47; k++)
        showline(k);
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


/* modified by lincoln baxter for 2006 NC-Rate Schedule on page 25
 * of the D-400 instruction book
 * */
void TaxRateFunction( dec_t * tax, dec_t x, int status )
{
    //dec_t temp1, temp2, temp3;
    int lt1, lt2, lt3;
   
// DEBUG 
    
    char word[2000];
 
    switch (status)
        {
        case 1:	/*Single*/
	    LT64(lt1, x, P_12750, &set);
            LT64(lt2, x, P_60000, &set);
            LT64(lt3, x, P_120000, &set);
            //if (ISNEGATIVE64(temp1)) {
            if (lt1) {
	        //if (x<12750.0) {
                MULTIPLY64((*tax), P_P06, x, &set);
                //return 0.06 * x;
            }
            //else if (ISNEGATIVE64(temp2)) {
            else if (lt2) {
                // else if (x<60000.0) {
                SUBTRACT64((*tax), x, P_12750, &set);
                MULTIPLY64((*tax), P_P07, (*tax), &set);
                ADD64((*tax), P_765, (*tax), &set);
                //return 765.0 + 0.07 * (x-12750.0);
            }
            //else if (ISNEGATIVE64(temp3)) {
            else if (lt3) {
                // else if (x<120000.0) {
                SUBTRACT64((*tax), x, P_60000, &set);
                MULTIPLY64((*tax), P_P0775, (*tax), &set);
                ADD64((*tax), P_4072P5, (*tax), &set);
                //return 4072.5 + 0.0775 * (x-60000.0);
            }
            else {
                SUBTRACT64((*tax), x, P_120000, &set);
                MULTIPLY64((*tax), P_P0825, (*tax), &set);
                ADD64((*tax), P_8722P5, (*tax), &set);
                //return 8722.5 + 0.0825 * (x-120000.0);
            }

            break;
        case 2:	/*Married/joint*/
        case 5:	/*Widow(er)*/
            LT64(lt1, x, P_21250, &set);
            LT64(lt2, x, P_100000, &set);
            LT64(lt3, x, P_200000, &set);
            if (lt1) {
                //if (x<21250.0) {
                MULTIPLY64((*tax), P_P06, x, &set);
                //return 0.06 * x;
            }
            //else if (ISNEGATIVE64(temp2)) {
            else if (lt2) {
                // else if (x<100000.0) {
                SUBTRACT64((*tax), x, P_21250, &set);
                MULTIPLY64((*tax), P_P07, (*tax), &set);
                ADD64((*tax), P_1275, (*tax), &set);
                //return 1275.0 + 0.07 * (x-21250.0);
            }
            //else if (ISNEGATIVE64(temp3)) {
            else if (lt3) {
                // else if (x<200000.0) {
                SUBTRACT64((*tax), x, P_100000, &set);
                MULTIPLY64((*tax), P_P0775, (*tax), &set);
                ADD64((*tax), P_6787P5, (*tax), &set);
                //return 6787.5 + 0.0775 * (x-100000.0);
            }
            else {
                SUBTRACT64((*tax), x, P_200000, &set);
                MULTIPLY64((*tax), P_P0825, (*tax), &set);
                ADD64((*tax), P_14537P5, (*tax), &set);
                //return 14537.5 + 0.0825 * (x-200000.0);
            }
            break;
        case 3:	/*Married/Separate*/
            LT64(lt1, x, P_10625, &set);
            LT64(lt2, x, P_50000, &set);
            LT64(lt3, x, P_100000, &set);
            //if (ISNEGATIVE64(temp1)) {
            if (lt1) {
                //if (x<10625.0) {
                MULTIPLY64((*tax), P_P06, x, &set);
                //return 0.06 * x;
            }
            //else if (ISNEGATIVE64(temp2)) {
            else if (lt2) {
                // else if (x<50000.0) {
                SUBTRACT64((*tax), x, P_10625, &set);
                MULTIPLY64((*tax), P_P07, (*tax), &set);
                ADD64((*tax), P_637P5, (*tax), &set);
                //return 637.5 + 0.07 * (x-10625.0);
            }
            //else if (ISNEGATIVE64(temp3)) {
            else if (lt3) {
            //else if (x<100000.0) {
                SUBTRACT64((*tax), x, P_50000, &set);
                MULTIPLY64((*tax), P_P0775, (*tax), &set);
                ADD64((*tax), P_3393P75, (*tax), &set);
                //return 3393.75 + 0.0775 * (x-50000.0);
            }
            else {
                SUBTRACT64((*tax), x, P_100000, &set);
                MULTIPLY64((*tax), P_P0825, (*tax), &set);
                ADD64((*tax), P_7268P75, (*tax), &set);
                //return 7268.75 + 0.0825 * (x-100000.0);
            }
            break;
        case 4:	/*Head of Household*/
            LT64(lt1, x, P_17000, &set);
            LT64(lt2, x, P_80000, &set);
            LT64(lt3, x, P_160000, &set);
            //if (ISNEGATIVE64(temp1)) {
            if (lt1) {
                //if (x<17000.0) {
                MULTIPLY64((*tax), P_P06, x, &set);
                //return 0.06 * x;
            }
            //else if (ISNEGATIVE64(temp2)) {
            else if (lt2) {
                //else if (x<80000.0) {
                SUBTRACT64((*tax), x, P_17000, &set);
                MULTIPLY64((*tax), P_P07, (*tax), &set);
                ADD64((*tax), P_1020, (*tax), &set);
                //return 1020.0 + 0.07 * (x-17000.0);
            }
            //else if (ISNEGATIVE64(temp3)) {
            else if (lt3) {
                // else if (x<160000.0) {
                SUBTRACT64((*tax), x, P_80000, &set);
                MULTIPLY64((*tax), P_P0775, (*tax), &set);
                ADD64((*tax), P_5430, (*tax), &set);
                //return 5430.0 + 0.0775 * (x-80000.0);
            }
            else {
                SUBTRACT64((*tax), x, P_160000, &set);
                MULTIPLY64((*tax), P_P0825, (*tax), &set);
                ADD64((*tax), P_11630, (*tax), &set);
                //return 11630.0 + 0.0825 * (x-160000.0);
            }
            break;
        }
}
