/************************************************************************/
/* TaxSolve_usa_fed1040_2006.c - OpenTaxSolver for USFed1040 v3.00	*/
/* Copyright (C) 2004, 2005, 2006 - Aston Roberts			*/
/* 									*/
/* Tax Solver for US Fedral 1040 Income Tax return for 2006 Tax Year.	*/
/* 									*/
/* OTS Project Home Page and Updates:  					*/
/*		http://opentaxsolver.sourceforge.com/			*/
/* 									*/
/* Compile:   cc taxsolve_US_1040_2006 -o taxsolve_US_1040_2006         */
/* Run:       ./taxsolve_US_1040_2006  Fed1040_2006.dat                 */
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
/* Aston Roberts 12-15-2006	aston_roberts@yahoo.com			*/
/* Updates by:  Lincoln Baxter  lab@lincolnbaxter.com	2006		*/
/*    1-25-05   E. Ferguson						*/ 
/*    2-14-05   BWB							*/
/* 		et. al.							*/
/* Updated 2-10-06	Robert Heller					*/
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
#include "taxsolve_US_1040_2006.h"
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
    int i;
    char outfname[1000];
    char * capgain[10];
    int loop;
    int index;
    for (i=0; i<10; i++) capgain[i] = (char *)malloc((sizeof(char) * 50));
#ifdef COUNTING
    reset();
#endif
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
    
    //-------------------------------------------------------------------------------------------
    /* Decode any command-line arguments. */
    myfprintf(stderr, "=== Tax Preparation Program starts ===\n");

    //-------------------------------------------------------------------------------------------
    //Initialize the decContext
    CONTEXTDEFAULT(set, RNE);
    //set.round = RNA;         // 0.5 rounds up
    
    //-------------------------------------------------------------------------------------------
    //Perform tax calculations
    macro_assign();

    //We can call the following + three functions multiple times for
    // different inputs

    //Open input and output files
    assert(argc==4);
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
      initialization(capgain);
      action(capgain);
      ending(outfname);
    }

    
    myfclose(infile);
    myfclose(outfile);
    myprintf("=== Tax Preparation Program is done === \n");
    myprintf("=== Total Processing %d tax returns === \n", index);
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
    dec_t temp1, P_P01, P_2, P_150;

    FROMSTRING64(P_P5, "0.5", &set);           //      0.5
    ZERO64(ZERO);                              //      0
    FROMSTRING64(P_P01, "0.01", &set);         //      0.01
    ADD64(P_P02, P_P01, P_P01, &set);          //      0.02
    ADD64(P_P03, P_P01, P_P02, &set);          //      0.03
    ADD64(P_P05, P_P02, P_P03, &set);          //      0.05
    MULTIPLY64(P_P025, P_P05, P_P5, &set);     //      0.025
    ADD64(P_P075, P_P05, P_P025, &set);        //      0.075
    ADD64(P_P1, P_P05, P_P05, &set);           //      0.1
    FROMSTRING64(P_P15, "0.15", &set);         //      0.15
    MULTIPLY64(P_P25, P_P5, P_P5, &set);       //      0.25
    ADD64(P_P26, P_P25, P_P01, &set);          //      0.26
    ADD64(P_P28, P_P25, P_P03, &set);          //      0.28
    ADD64(P_P33, P_P28, P_P05, &set);          //      0.33
    ADD64(P_P35, P_P33, P_P02, &set);          //      0.35
    FROMSTRING64(P_P8, "0.8", &set);           //      0.8
    FROMSTRING64(P_P999999, "0.999999", &set); //      0.999999
    ADD64(P_1, P_P5, P_P5, &set);              //      1
    ADD64(P_1P5, P_1, P_P5, &set);             //      1.5
    ADD64(P_2, P_1, P_1, &set);                //      2
    ADD64(P_3, P_2, P_1, &set);                //      3
    ADD64(P_5, P_3, P_2, &set);                //      5
    ADD64(P_10, P_5, P_5, &set);               //     10
    MULTIPLY64(P_25, P_5, P_5, &set);          //     25
    ADD64(P_50, P_25, P_25, &set);             //     50
    ADD64(P_100, P_50, P_50, &set);            //    100
    ADD64(P_150, P_100, P_50, &set);           //    150
    MULTIPLY64(P_500, P_100, P_5, &set);       //    500
    ADD64(P_1000, P_500, P_500, &set);         //  1,000
    ADD64(P_1100, P_1000, P_100, &set);        //  1,100
    ADD64(P_1250, P_1100, P_150, &set);        //  1,250
    ADD64(P_2500, P_1250, P_1250, &set);       //  2,500
    ADD64(P_3000, P_2500, P_500, &set);        //  3,000
    MULTIPLY64(P_3300, P_1100, P_3, &set);     //  3,300
    FROMSTRING64(P_5150, "5150.0", &set);      //  5,150
    FROMSTRING64(P_7550, "7550.0", &set);      //  7,550
    FROMSTRING64(P_10300, "10300.0", &set);    // 10,300
    FROMSTRING64(P_10750, "10750.0", &set);    // 10,750
    MULTIPLY64(P_15000, P_150, P_100, &set);   // 15,000
    ADD64(P_15100, P_15000, P_100, &set);      // 15,100
    ADD64(P_30000, P_15000, P_15000, &set);    // 30,000
    ADD64(temp1, P_500, P_150, &set);          // temp = 650
    ADD64(P_30650, temp1, P_30000, &set);      // 30,650
    FROMSTRING64(P_31275, "31275.0", &set);    // 31,275
    FROMSTRING64(P_41050, "41050.0", &set);    // 41,050
    FROMSTRING64(P_42500, "42500.0", &set);    // 42,500
    MULTIPLY64(P_50000, P_500, P_100, &set);   // 50,000
    FROMSTRING64(P_61250, "61250.0", &set);    // 61,250
    ADD64(P_61300, P_61250, P_50, &set);       // 61,300
    FROMSTRING64(P_61850, "61850.0", &set);    // 61,850
    FROMSTRING64(P_62550, "62550.0", &set);    // 62,550
    FROMSTRING64(P_74200, "74200.0", &set);    // 74,200
    FROMSTRING64(P_75000, "75000.0", &set);    // 75,000
    ADD64(temp1, P_100, P_150, &set);          // temp = 250
    ADD64(P_75250, temp1, P_75000, &set);      // 75,250
    FROMSTRING64(P_87500, "87500.0", &set);    // 87,500
    FROMSTRING64(P_94225, "94225.0", &set);    // 94,225
    ADD64(P_100000, P_50000, P_50000, &set);   //100,000
    ADD64(temp1, P_2500, P_2500, &set);        // temp = 5000
    ADD64(P_105000, temp1, P_100000, &set);    //105,000
    ADD64(P_106000, P_105000, P_1000, &set);   //106,000
    FROMSTRING64(P_112500, "112500.0", &set);  //112,500
    FROMSTRING64(P_112875, "112875.0", &set);  //112,875
    FROMSTRING64(P_122500, "122500.0", &set);  //122,500
    FROMSTRING64(P_123700, "123700.0", &set);  //123,700
    MULTIPLY64(P_150000, P_150, P_1000, &set); //150,000
    ADD64(P_150500, P_150000, P_500, &set);    //150,500
    FROMSTRING64(P_154800, "154800.0", &set);  //154,800
    FROMSTRING64(P_168275, "168275.0", &set);  //168,275
    FROMSTRING64(P_171650, "171650.0", &set);  //171,650
    FROMSTRING64(P_175000, "175000.0", &set);  //175,000
    FROMSTRING64(P_188150, "188150.0", &set);  //188,150
    FROMSTRING64(P_188450, "188450.0", &set);  //188,450
    FROMSTRING64(P_225750, "225750.0", &set);  //225,750
    FROMSTRING64(P_336550, "336550.0", &set);  //336,550
    FROMSTRING64(P_9e9,    "9e9",    &set);    // 9e9
    MINUS64(N_3000, P_3000, &set);             // -3,000
    ADD64(temp1, P_1000, P_500, &set);         //temp = 1500
    MINUS64(N_1500, temp1, &set);              // -1,500
}

void initialization(char * capgain[])
{
    int i, date1=0, date2, iteri=0;
    dec_t amnt1, amnt2;
    char word[2000];
    time_t now;

    /* Pre-initialize all lines to zeros. */
    ZERO64(L9b);
    for (i=0; i<MAX_LINES; i++) {
        ZERO64(L[i]);
        ZERO64(SchedA[i]);
        ZERO64(SchedD[i]);
    }
    for (i=0; i<10; i++) {
        ZERO64(capgain_amt[i]);
    }
    ZERO64(stcg);
    ZERO64(ltcg);
    for (i=0; i<10; i++)
        mystrcpy(capgain[i], "");

    //-------------------------------------------------------------------------------------------
    /* Accept Form's "Title" line, and put out with date-stamp for your records. */
    //read_line( infile, word );
    mystrcpy(word, "(c) Title:  US Federal 1040 Tax Form - 2006 - EXAMPLE\n");
    now = mytime(0);
#ifdef PRINT_DATE_TIME
    myfprintf(outfile,"\n%s(c) 	%s\n", word, ctime( &now ));
#else
    myfprintf(outfile,"\n%s\n", word);
#endif

    //-------------------------------------------------------------------------------------------
    /* Accept parameters from input file. */
    /* Expect  US-Fed-1040 lines, something like:
       Title:  Federal 1040 1999 Return
       L7		{Wages}
       L8a		{Interest}
       L9		{Dividends}
       L13		{Capital Gains}
       L38		{Itemized deductions from Schedule A or Std Deduction}
       L58		{Witheld tax, from W-2}
    */
    
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
    //Reading -- Dependents
    read_from_file();
    COPY64(L[6], line.amt);

    //-------------------------------------------------------------------------------------------
    //Reading -- L7 - L12, D4 - D14
    read_from_file();
    while (1) {
        if (mystrcmp(line.line_name, "L7") == 0) {       /* Wages, salery, tips (W-2). */
            ADD64(L[7], L[7], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L8a") == 0) { /* Taxable interest. (Sched-B) */
            ADD64(L[8], L[8], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L9") == 0) {  /* Ordinary Dividends. (Sched-B) */
            ADD64(L[9], L[9], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L9b") == 0) { /* Qualified Dividends. (Sched-B) */
            ADD64(L9b, L9b, line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L10") == 0) { /* Taxable refunds. */
            ADD64(L[10], L[10], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L11") == 0) { /* Alimony received. */
            ADD64(L[11], L[11], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L12") == 0) { /* Business income/loss. */
            ADD64(L[12], L[12], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "D4") == 0) {
            /* Short term gain from 6252 and short-term gain or loss from Forms 4684, 6781, 8824. */
            ADD64(SchedD[4], SchedD[4], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "D5") == 0) {
            /* Net short-term gain or loss from partnerships, S corps, estates, trusts from K-1. */
            ADD64(SchedD[5], SchedD[5], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "D6") == 0) {
            /* Carryover short-term loss from 2005 */
            COPY64(SchedD[6], line.amt);
        }
        else if (mystrcmp(line.line_name, "D11") == 0) {
            /* Gain from Form 4797. */
            ADD64(SchedD[11], SchedD[11], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "D12") == 0) {
            /* Partnership net long-term gain or loss. */
            ADD64(SchedD[12], SchedD[12], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "D13") == 0) {
            /* Cap Gains Distributions - 1099-DIV col. 2a. */
            ADD64(SchedD[13], SchedD[13], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "D14") == 0) {
            /* Carryover long-term loss from 2003. Or, leave blank if last year's file entered in line 6. */
            ADD64(SchedD[14], SchedD[14], line.amt, &set);
        }
        else { break; }
        read_from_file();
    }
    
    //-------------------------------------------------------------------------------------------
    // Reading -- Cap-Gains/Losses

    TOSTRING64(line.amt, word);
    iteri = myatoi(word);
    i = 0;
    while (iteri) {
        iteri--;
        if (myfeof(infile)) {myprintf("ERROR: Unexpected EOF on '%s'\n","L13"); exit(1);}
        //        if (!Do_SchedD) {
        if ((ISZERO64(SchedD[13])!=0) && (ISZERO64(SchedD[6])!=0) && (ISZERO64(SchedD[14])!=0)) {
            if (!Do_SchedD) {
	      mysprintf(capgain[i++],"(c)  Cap Gains Schedule-D\n");
	      Do_SchedD = Yes;}}

        //start date
        read_from_file();
        COPY64(amnt1, line.amt);
        if (ISPOSITIVE64(amnt1) && (ISZERO64(amnt1)==0)) {
            /* Buy amounts must be negative. (It is a cost.) */
            MINUS64(amnt1, amnt1, &set);
        }
        mystrcpy(word, line.line_name);
        date1 = get_date( word, "L13" );
        
        //end date
        read_from_file();
        COPY64(amnt2, line.amt);
        mystrcpy(word, line.line_name);
        date2 = get_date( word, "L13" );

        if ((date2-date1)<0) {myprintf("ERROR: Buy-date after sell-date.\n");  exit(1);}
        if ((date2-date1)>365) {
            ADD64(SchedD[10], SchedD[10], amnt2, &set);
            ADD64(ltcg, ltcg, amnt1, &set);
            ADD64(ltcg, ltcg, amnt2, &set);
        }
        else {
            ADD64(SchedD[3], SchedD[3], amnt2, &set);
            ADD64(stcg, stcg, amnt1, &set);
            ADD64(stcg, stcg, amnt2, &set);
        }
        
        if ((date2-date1)>365) {
            dec_t temp1;
            ADD64(temp1, amnt1, amnt2, &set);
            COPY64(capgain_amt[i], temp1);
            mysprintf(capgain[i++], "(d) 	 Long = ");
            //mysprintf(capgain[i++], "	 Long %6.2f\n", atof(TOSTRING64(temp1, word)));
        }
        else {
            dec_t temp1;
            ADD64(temp1, amnt1, amnt2, &set);
            COPY64(capgain_amt[i], temp1);
            mysprintf(capgain[i++], "(d) 	 Short = ");
            //mysprintf(capgain[i++], "	 Short %6.2f\n", atof(TOSTRING64(temp1, word)));
        }
    }

    //-------------------------------------------------------------------------------------------
    // Reading -- L14 - L39, A1 - A27, L47 - L71
    read_from_file();
    while (1) {
        if (mystrcmp(line.line_name, "L14") == 0) {
            /* Other gains or losses. Form 4794. */
            ADD64(L[14], L[14], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L15b") == 0) {
            /* Taxable IRA distributions (pg 22). */
            ADD64(L[15], L[15], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L16b") == 0) {
            /* Taxable Pensions/annuities (pg 22) */
            ADD64(L[16], L[16], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L17") == 0) {
            /* Rent realestate, royalties, partnerships, S corp. (Sched E)*/
            ADD64(L[17], L[17], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L18") == 0) {
            /* Farm income/loss. (Sched F) */
            ADD64(L[18], L[18], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L19") == 0) {
            /* Unemployment compensation */
            ADD64(L[19], L[19], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L20b") == 0) {
            /* Taxable Social security benefits */
            ADD64(L[20], L[20], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L21") == 0) {
            /* Other income. (pg 27) */
            ADD64(L[21], L[21], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L23") == 0) {
            /* Educator expenses (see page 26) */
            ADD64(L[23], L[23], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L24") == 0) {
            /* Bus. exp.: reservists, artists, ... Attach Form 2106 */
            ADD64(L[24], L[24], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L25") == 0) {
            /* Health savings account deduction. Attach Form 8889 */
            ADD64(L[25], L[25], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L26") == 0) {
            /* Moving expenses. Attach Form 3903*/
            ADD64(L[26], L[26], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L27") == 0) {
            /* One-half of self-employment tax. Attach Schedule SE*/
            ADD64(L[27], L[27], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L28") == 0) {
            /* Self-employed health insurance deduction (see page 33)*/
            ADD64(L[28], L[28], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L29") == 0) {
            /* Self-employed SEP, SIMPLE, and qualified plans*/
            ADD64(L[29], L[29], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L30") == 0) {
            /* Penalty on early withdrawal of savings*/
            ADD64(L[30], L[30], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L31a") == 0) {
            /* Alimony paid*/
            ADD64(L[31], L[31], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L32") == 0) {
            /* IRA deduction (see page 26) */
            ADD64(L[32], L[32], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L33") == 0) {
            /* Student loan interest deduction (pg 28) */
            ADD64(L[33], L[33], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L34") == 0) {
            /* Jury Duty pay you gave to your employer */
            ADD64(L[34], L[34], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L35") == 0) {
            /* Domestic production activities deduction, Form 8903 */
            ADD64(L[35], L[35], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L39") == 0) {
            /* Number of boxes checked (0-4), line-39:  You, Spouse : >65, blind.  */
            ADD64(L[39], L[39], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A1") == 0) {
            /* Unreimbursed medical expenses. */
            ADD64(SchedA[1], SchedA[1], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A5") == 0) {
            /* State and local income taxes. */
            ADD64(SchedA[5], SchedA[5], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A6") == 0) {
            /* Real estate taxes. */
            ADD64(SchedA[6], SchedA[6], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A7") == 0) {
            /* Personal property taxes. */
            ADD64(SchedA[7], SchedA[7], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A8") == 0) {
            /* Other taxes. */
            ADD64(SchedA[8], SchedA[8], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A10") == 0) {
            /* Home mortgage interest and points */
            ADD64(SchedA[10], SchedA[10], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A11") == 0) {
            /* Home mortgage interest not reported to you on Form 1098.*/
            ADD64(SchedA[11], SchedA[11], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A12") == 0) {
            /* Points not reported to you on Form 1098.*/
            ADD64(SchedA[12], SchedA[12], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A13") == 0) {
            /* Investment interest. Attach Form 4952*/
            ADD64(SchedA[13], SchedA[13], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A15") == 0) {
            /* Charity contributions.*/
            ADD64(SchedA[15], SchedA[15], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A16") == 0) {
            /* Contributions other than cash or check.*/
            ADD64(SchedA[16], SchedA[16], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A17") == 0) {
            /* Carryover from prior year*/
            ADD64(SchedA[17], SchedA[17], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A19") == 0) {
            /* Casualty or theft loss(es).*/
            ADD64(SchedA[19], SchedA[19], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A20") == 0) {
            /* Unreimbursed employee expenses*/
            ADD64(SchedA[20], SchedA[20], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A21") == 0) {
            /* Tax preparation fees*/
            ADD64(SchedA[21], SchedA[21], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A22") == 0) {
            /* Other expenses*/
            ADD64(SchedA[22], SchedA[22], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "A27") == 0) {
            /* Other deductions, list on page A-6. */
            ADD64(SchedA[27], SchedA[27], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L47") == 0) {
            /* Foreign tax credit. */
            ADD64(L[47], L[47], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L48") == 0) {
            /* Child / dependent care expense credits. Form 2441. */
            ADD64(L[48], L[48], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L49") == 0) {
            /*  Credit for the elderly or the disabled. Attach Schedule R. */
            ADD64(L[49], L[49], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L50") == 0) {
            /*  Education credits. Form 8863. */
            ADD64(L[50], L[50], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L51") == 0) {
            /*  Retirement savings contributions credit. Form 8880. */
            ADD64(L[51], L[51], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L52") == 0) {
            /*  Residential energy credits. */
            ADD64(L[52], L[52], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L53") == 0) {
            /*  Child tax credit (see page 40). */
            ADD64(L[53], L[53], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L54") == 0) {
            /*  Credits from Forms 8396 8859 8839. */
            ADD64(L[54], L[54], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L55") == 0) {
            /*  Other credits. Forms 3800, 8801, ect. */
            ADD64(L[55], L[55], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L58") == 0) {
            /* Self-employment tax. Sched SE */
            ADD64(L[58], L[58], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L59") == 0) {
            /* Social Sec/Medicare tax on tips. Form 4137 */
            ADD64(L[59], L[59], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L60") == 0) {
            /* Tax on tax-favored accounts. Form 5329 */
            ADD64(L[60], L[60], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L61") == 0) {
            /* Advance earned income credit payments from W-2 */
            ADD64(L[61], L[61], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L62") == 0) {
            /* Household employment taxes. Sched H */
            ADD64(L[62], L[62], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L64") == 0) {
            /* Federal income tax withheld, Forms W-2, 1099 */
            ADD64(L[64], L[64], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L65") == 0) {
            /* '06 estimated payments + amnt applied from '05 */
            ADD64(L[65], L[65], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L66a") == 0) {
            /* Earned income credit (EIC) */
            ADD64(L[66], L[66], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L67") == 0) {
            /* Excess soc. sec. + tier 1 RRTA tax withheld, pg 60 */
            ADD64(L[67], L[67], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L68") == 0) {
            /* Additional child tax credit. Attach Form 8812 */
            ADD64(L[68], L[68], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L69") == 0) {
            /* Amnt paid in filing extension req. (page 60) */
            ADD64(L[69], L[69], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L70") == 0) {
            /* Other payments forms 2439, 4136, 8885. */
            ADD64(L[70], L[70], line.amt, &set);
        }
        else if (mystrcmp(line.line_name, "L71") == 0) {
            /* Credit for telephone excise tax paid. Attach Form 8913 */
            ADD64(L[71], L[71], line.amt, &set);
            break; //to go to the next return!!
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

void action(char * capgain[])
{
    int i;
    dec_t temp1, temp2, temp3, temp4;
    int temp1_int, temp2_int, temp3_int, temp4_int;

    printL6_L12();
    
    //-------------------------------------------------------------------------------------------
    get_cap_gains( "L13", capgain );	 /* Capital gains. (Schedule-D) */
    
    for (i=7; i<=21; i++) ADD64(L[22], L[22], L[i], &set);
    
    //-------------------------------------------------------------------------------------------

    if (ISZERO64(L[33]) == 0) {
        /* Student loan interest calculation pg 33. */ 
        dec_t ws[20];
        MIN64(ws[1], L[33], P_2500, &set);
        COPY64(ws[2], L[22]);

        ADD64(temp1, L[23], L[24], &set);
        ADD64(temp2, L[25], L[28], &set);
        ADD64(temp3, L[29], L[30], &set);
        ADD64(temp4, L[31], L[32], &set);
        ADD64(ws[3], L[34], temp1, &set);
        ADD64(temp1, temp2, temp3, &set);
        ADD64(temp1, temp1, temp4, &set);
        ADD64(ws[3], ws[3], temp1, &set);

        SUBTRACT64(ws[4], ws[2], ws[3], &set);

        if (status == MARRIED_FILLING_JOINTLY) COPY64(ws[5], P_105000);
        else COPY64(ws[5], P_50000);
        LT64(temp1_int, ws[5], ws[4], &set);
        if (temp1_int) {
	    SUBTRACT64(ws[6], ws[4], ws[5], &set);
            if (status == MARRIED_FILLING_JOINTLY) {DIVIDE64(ws[7], ws[6], P_30000, &set);}
            else {DIVIDE64(ws[7], ws[6], P_15000, &set);}
            LT64(temp1_int, P_1, ws[7], &set);
            if (temp1_int) COPY64(ws[7], P_1);
	        MULTIPLY64(ws[8], ws[1], ws[7], &set);
        }
        else {ZERO64(ws[8]);}
        SUBTRACT64(ws[9], ws[1], ws[8], &set);
        COPY64(L[33], ws[9]);
    }

    for (i=23; i<=35; i++) {ADD64(L[36], L[36], L[i], &set);}

    SUBTRACT64(L[37], L[22], L[36], &set);

    /* Taxes and Credits section. */
    COPY64(L[38], L[37]);
    
            
    COPY64(SchedA[2], L[38]);
    MULTIPLY64(SchedA[3], P_P075, SchedA[2], &set);
    SUBTRACT64(SchedA[4], SchedA[1], SchedA[3], &set);
    NotLessThanZero(&SchedA[4], SchedA[4]);
    
    ADD64(temp1, SchedA[5], SchedA[6], &set);
    ADD64(temp2, SchedA[7], SchedA[8], &set);
    ADD64(SchedA[9], temp1, temp2, &set);

    ADD64(temp1, SchedA[10], SchedA[11], &set);
    ADD64(temp2, SchedA[12], SchedA[13], &set);
    ADD64(SchedA[14], temp1, temp2, &set);

    ADD64(temp1, SchedA[15], SchedA[16], &set);
    ADD64(SchedA[18], temp1, SchedA[17], &set);

    ADD64(temp1, SchedA[20], SchedA[21], &set);
    ADD64(SchedA[23], temp1, SchedA[22], &set);

    COPY64(SchedA[24], L[38]);
    MULTIPLY64(SchedA[25], P_P02, SchedA[24], &set);

    SUBTRACT64(SchedA[26], SchedA[23], SchedA[25], &set);
    NotLessThanZero(&SchedA[26], SchedA[26]);

    //-------------------------------------------------------------------------------------------
    
    mystrcpy(outarr[arr_index++], "");
    LT64(temp1_int, P_150500, L[38], &set);
    LT64(temp2_int, P_75250, L[38], &set);
        
    /*
    if (((ISNEGATIVE64(temp1)) && (status!=MARRIED_FILLING_SEPARAT))
        || ((ISNEGATIVE64(temp2)) && (status==MARRIED_FILLING_SEPARAT))) {
    */
    if ((temp1_int && (status!=MARRIED_FILLING_SEPARAT))
        || (temp2_int && (status==MARRIED_FILLING_SEPARAT))) {
        /* Yes, may be limited. */
        /* Itemized work sheet - Line 28 from Sched A, pgs A-7. */
        dec_t ws[90];
            
        for (i=0; i<90; i++) ZERO64(ws[i]);

        ADD64(temp1, SchedA[4], SchedA[9], &set);
        ADD64(temp2, SchedA[14], SchedA[18], &set);
        ADD64(temp3, SchedA[19], SchedA[26], &set);
        ADD64(temp1, temp1, temp2, &set);
        ADD64(temp2, temp3, SchedA[27], &set);
        ADD64(ws[1], temp1, temp2, &set);

        ADD64(temp1, SchedA[4], SchedA[13], &set);
        ADD64(temp2, SchedA[15], SchedA[19], &set);
        ADD64(temp1, temp1, SchedA[27], &set);
        ADD64(ws[2], temp1, temp2, &set);
            
        LT64(temp1_int, ws[2], ws[1], &set);
        if (temp1_int) { /*yes1*/
	    SUBTRACT64(ws[3], ws[1], ws[2], &set);
            MULTIPLY64(ws[4], P_P8, ws[3], &set);
            COPY64(ws[5], L[38]);
                
            if (status!=MARRIED_FILLING_SEPARAT) {
                COPY64(ws[6], P_150500);
            }
            else {
                COPY64(ws[6], P_75250);
            }
            LT64(temp1_int, ws[6], ws[5], &set);
            if (temp1_int) { /*yes2*/
	        SUBTRACT64(ws[7], ws[5], ws[6], &set);
                MULTIPLY64(ws[8], P_P03, ws[7], &set);
                MIN64(ws[9], ws[4], ws[8], &set);
                DIVIDE64(ws[10], ws[9], P_3, &set);
                SUBTRACT64(ws[11], ws[9], ws[10], &set);
                SUBTRACT64(ws[12], ws[1], ws[11], &set);
                COPY64(SchedA[28], ws[12]);
                COPY64(ws_1, ws[1]);
                COPY64(ws_12, ws[12]);
                mysprintf(outarr[arr_index-1],"(t) Sched-A Itemized Deductions Work Sheet (pg A-7) limits deduction between = ");
                //mysprintf(outarr[arr_index-1],"Sched-A Itemized Deductions Work Sheet (pg A-7) limits deduction from %6.2f, to %6.2f\n",
                //        atof(TOSTRING64(ws[1], word)), atof(TOSTRING64(ws[12], word)));
            }
            else { /*no2*/
                COPY64(SchedA[28], ws[1]);
                mysprintf(outarr[arr_index-1], "(c)  Deductions not limited.\n");
            }
        }
        else { /*no1*/
            COPY64(SchedA[28], ws[1]);
            mysprintf(outarr[arr_index-1], "(c)  Deductions not limited.\n");
        }
    }
    else { /* Deductions not limited */
        mysprintf(outarr[arr_index-1], "(c)  Deductions not limited.\n");
        ADD64(temp1, SchedA[4], SchedA[9], &set);
        ADD64(temp2, SchedA[14], SchedA[18], &set);
        ADD64(temp3, SchedA[19], SchedA[26], &set);
        ADD64(temp1, temp1, temp2, &set);
        ADD64(temp2, temp3, SchedA[27], &set);
        ADD64(SchedA[28], temp1, temp2, &set);
    }

    COPY64(L[40], SchedA[28]);

    LT64(temp1_int, ZERO, L[40], &set);
    if (temp1_int) itemize = Yes; else itemize = No;
    
    mystrcpy(outarr[arr_index++], "");
    if ((status==SINGLE) || (status==MARRIED_FILLING_SEPARAT)) { 		/* Compare to std deduction for '06. */
        LT64(temp2_int, L[40], P_5150, &set);
        if (temp2_int) {
	    COPY64(L[40], P_5150);
            mysprintf(outarr[arr_index-1], "(c) Use standard deduction.\n");
            itemize = 0;}
    }
    else if ((status==MARRIED_FILLING_JOINTLY) || (status==WIDOW)) {
        LT64(temp2_int, L[40], P_10300, &set);
        if (temp2_int) {
	    COPY64(L[40], P_10300);
            mysprintf(outarr[arr_index-1], "(c) Use standard deduction.\n");
            itemize = 0;}
    }
    else if (status==HEAD_OF_HOUSEHOLD) {
        LT64(temp2_int, L[40], P_7550, &set);
        if (temp2_int) {
	    COPY64(L[40], P_7550);
            mysprintf(outarr[arr_index-1], "(c) Use standard deduction.\n");
            itemize = 0;}
    }
    else 
        { myprintf("Case (Line 40) not handled.\n"); exit(1); }
    

    SUBTRACT64(L[41], L[38], L[40], &set);

    mystrcpy(outarr[arr_index++], "");
    LT64(temp1_int, L[38], P_112875, &set);
    EQ64(temp2_int, L[38], P_112875, &set);
    if (temp1_int || temp2_int) {
        MULTIPLY64(L[42], P_3300, L[6], &set);
    }
    else { /* Worksheet pg 36. */
        dec_t denom;
        dec_t ws[100];
        COPY64(denom, P_2500);
        
        switch (status) {
        case SINGLE:
            COPY64(ws[4], P_150500);
            break;
        case MARRIED_FILLING_JOINTLY:  
        case WIDOW:
            COPY64(ws[4], P_225750);
            break;
        case MARRIED_FILLING_SEPARAT:
            COPY64(ws[4], P_112875);
            COPY64(denom, P_1250);
            break;
        case HEAD_OF_HOUSEHOLD:
            COPY64(ws[4], P_188150);
            break;
        default:
            ZERO64(ws[4]);
        }
        
        LT64(temp1_int, L[38], ws[4], &set);
        EQ64(temp2_int, L[38], ws[4], &set);
        if (temp1_int || temp2_int) {
	    MULTIPLY64(L[42], P_3300, L[6], &set);
        }
        else {
            MULTIPLY64(ws[2], P_3300, L[6], &set);
            COPY64(ws[3], L[38]);
            SUBTRACT64(ws[5], ws[3], ws[4], &set);
            
            LT64(temp1_int, P_122500, ws[5], &set);
            LT64(temp2_int, P_61250, ws[5], &set);
	        
            /*
	    if ((ISNEGATIVE64(temp1)) ||
                ((ISNEGATIVE64(temp2)) && (status == MARRIED_FILLING_SEPARAT))) {
            */
            if ((temp1_int) ||
                ((temp2_int) && (status == MARRIED_FILLING_SEPARAT))) {
	        mysprintf(outarr[arr_index-1], "(c) Exemptions limited by excessive income ... (Exemptions Worksheet, pg 36)\n");
                MULTIPLY64(L[42], P_1100, L[6], &set);
            }
            else {
                if (CHECK_ORIG) {
                    myprintf("[1] Exact code as original software (for checking purposes)\n");
                    DIVIDE64(temp2, ws[5], denom, &set);
                    ADD64(ws[6], temp2, P_P999999, &set);
                    SET_ROUNDMODE(set, RTZ);    // 0.5 rounds up
                    TOINTEGRAL64(ws[6], ws[6], &set);
                    SET_ROUNDMODE(set, RNE);  //defualt
                    //ws[6] = (int)( ws[5] / denom + 0.999999 );
                }
                else {
                    DIVIDE64(ws[6], ws[5], denom, &set);
                    //set.round = DEC_ROUND_UP;    // round away from 0
                     SET_ROUNDMODE(set, RPI);
                    TOINTEGRAL64(ws[6], ws[6], &set);
                    //set.round = RNE;  //defualt
                     SET_ROUNDMODE(set, RNE);
                }
                MULTIPLY64(ws[7], P_P02, ws[6], &set);
                MULTIPLY64(ws[8], ws[2], ws[7], &set);
                DIVIDE64(ws[9], ws[8], P_1P5, &set);
                SUBTRACT64(ws[10], ws[2], ws[9], &set);
                COPY64(L[42], ws[10]);
            }
        }
    }

    SUBTRACT64(L[43], L[41], L[42], &set);
    NotLessThanZero(&L[43], L[43]);

    /* Calculate the tax...  on L43 */
    TaxRateFunction( &L[44], L[43], status );

    LT64(temp1_int, ZERO, L9b, &set);
    LT64(temp2_int, ZERO, L[13], &set);
    LT64(temp3_int, ZERO, SchedD[15], &set);
    LT64(temp4_int, ZERO, SchedD[16], &set);

    /*
    if ((!Do_SDTW) && (!Do_QDCGTW) &&
        ((ISNEGATIVE64(temp1)) || (ISNEGATIVE64(temp2)) || ((ISNEGATIVE64(temp3)) && (ISNEGATIVE64(temp4))) ))
    */
    if ((!Do_SDTW) && (!Do_QDCGTW) &&
        ((temp1_int) || (temp2_int) || ((temp3_int) && (temp4_int)) ))
        Do_QDCGTW = Yes;

    if (Do_QDCGTW)    capgains_qualdividends_worksheets( status, L9b );
    else if (Do_SDTW) sched_D_tax_worksheet( status, L9b );

    AlternativeMinimumTax( &L[45], status, itemize );
    ADD64(L[46], L[44], L[45], &set);
  
    for (i=47; i<=55; i++) ADD64(L[56], L[56], L[i], &set);

    SUBTRACT64(L[57], L[46], L[56], &set);
    NotLessThanZero(&L[57], L[57]);
    
    for (i=57; i<=62; i++) ADD64(L[63], L[63], L[i], &set);
    for (i=64; i<=71; i++) ADD64(L[72], L[72], L[i], &set);
}

void ending(char * outfname)
{
    //dec_t temp1;
    int temp1_int;
    arr_index=0;

    showline(13);
    ShowLineNonZero(14);
    ShowLineNonZero(15);
    ShowLineNonZero(16);
    ShowLineNonZero(17);
    ShowLineNonZero(18);
    ShowLineNonZero(19);
    ShowLineNonZero(20);
    ShowLineNonZero(21);
    showline_wmsg(22,"total income");
    ShowLineNonZero(23);
    ShowLineNonZero(24);
    ShowLineNonZero(25);
    ShowLineNonZero(26);
    ShowLineNonZero(27);
    ShowLineNonZero(28);
    ShowLineNonZero(29);
    ShowLineNonZero(30);
    ShowLineNonZero(31);
    ShowLineNonZero(32);
    ShowLineNonZero(33);
    ShowLineNonZero(34);
    ShowLineNonZero(35);
    showline(36);
    showline_wmsg(37,"adjusted gross income");
    showline(38);
    shownum(39);
    
    /* Schedule A */
    showschedA(1);
    showschedA(2);
    showschedA(3);
    showschedA(4);
    showschedA(5);
    showschedA(6);
    showschedA(7);
    showschedA(8);
    showschedA(9);
    showschedA(10);
    showschedA(11);
    showschedA(12);
    showschedA(13);
    showschedA(14);
    showschedA(15);
    showschedA(16);
    showschedA(17);
    showschedA(18);
    showschedA(19);
    showschedA(20);
    showschedA(21);
    showschedA(22);
    showschedA(23);
    showschedA(24);
    showschedA(25);
    showschedA(26);
    showschedA(27);

    myfprintf(outfile, "%s", outarr[arr_index++]);
    if (mystrstr(outarr[arr_index-1], "(t)") != 0) {
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
        {
            decimal64 myout;
            decimal64FromNumber(&myout, &ws_1, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\t");
            decimal64FromNumber(&myout, &ws_12, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif 
 #if defined(WIDTH128)
        {
            decimal128 myout;
            decimal128FromNumber(&myout, &ws_1, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\t");
            decimal128FromNumber(&myout, &ws_12, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
#else
	myfwrite(&ws_1, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\t");
        myfwrite(&ws_12, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n");
#endif
    }
    showschedA(28);
    myfprintf(outfile, "%s", outarr[arr_index++]);
    if (mystrstr(outarr[arr_index-1], "(t)") != 0) {
#if defined(DECNUMBER_ARBITRARY)        
 #if defined(WIDTH64)
        {
            decimal64 myout;
            decimal64FromNumber(&myout, &ws_1, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\t");
            decimal64FromNumber(&myout, &ws_12, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            decimal128FromNumber(&myout, &ws_1, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\t");
            decimal128FromNumber(&myout, &ws_12, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
#else
	myfwrite(&ws_1, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\t");
        myfwrite(&ws_12, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n");
#endif
    }

    if (itemize) myfprintf(outfile,"(c) Itemizing.\n");

    showline(40);
    showline(41);
    myfprintf(outfile, "%s", outarr[arr_index]);
    if (mystrstr(outarr[arr_index], "(t)") != 0) {
#if defined(DECNUMBER_ARBITRARY)        
 #if defined(WIDTH64)
        {
            decimal64 myout;
            decimal64FromNumber(&myout, &ws_1, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\t");
            decimal64FromNumber(&myout, &ws_12, &set);
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            decimal128FromNumber(&myout, &ws_1, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\t");
            decimal128FromNumber(&myout, &ws_12, &set);
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
#else
	myfwrite(&ws_1, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\t");
        myfwrite(&ws_12, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n");
#endif
    }

    showline( 42 );
    showline_wmsg( 43, "Taxable income" );
    
    if (Do_QDCGTW) {
        myfprintf(outfile,"(c) Doing 'Qualified Dividends and Captial Gain tax Worksheet', page 34.\n");
    }
    else if (Do_SDTW) {
        myfprintf(outfile,"(c) Doing 'Schedule D Tax Worksheet', page D9.\n");
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
        {
            decimal64 myout;
            decimal64FromNumber(&myout, &schedD_ws_13, &set);
            myfprintf(outfile,"(d)   Sched-D tax Worksheet line 13 = ");
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            decimal64FromNumber(&myout, &schedD_ws_14, &set);
            myfprintf(outfile,"\n(d)   Sched-D tax Worksheet line 14 = ");
            myfwrite(&myout, sizeof(dec_t), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            decimal128FromNumber(&myout, &schedD_ws_13, &set);
            myfprintf(outfile,"(d)   Sched-D tax Worksheet line 13 = ");
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            decimal128FromNumber(&myout, &schedD_ws_14, &set);
            myfprintf(outfile,"\n(d)   Sched-D tax Worksheet line 14 = ");
            myfwrite(&myout, sizeof(dec_t), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
#else
	myfprintf(outfile,"(d)   Sched-D tax Worksheet line 13 = ");
        myfwrite(&schedD_ws_13, sizeof(dec_t), 1, outfile);
        myfprintf(outfile,"\n(d)   Sched-D tax Worksheet line 14 = ");
        myfwrite(&schedD_ws_14, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n");
#endif
        //myfprintf(outfile,"(d)   Sched-D tax Worksheet line 13 = %6.2f\n", schedD_ws_13);
        //myfprintf(outfile,"(d)   Sched-D tax Worksheet line 14 = %6.2f\n", schedD_ws_14);
    }
    
    showline_wmsg( 44, "Tax" );
    if (ISZERO64(L[45])) myfprintf(outfile,"(c)  (Not subject to Alternative Minimum Tax.)\n");
    ShowLineNonZero(45);
    showline(46);
    ShowLineNonZero(47);
    ShowLineNonZero(48);
    ShowLineNonZero(49);
    ShowLineNonZero(50);
    ShowLineNonZero(51);
    ShowLineNonZero(52);
    ShowLineNonZero(53);
    ShowLineNonZero(54);
    ShowLineNonZero(55);
    showline(56);
    showline(57);
    
    //-------------------------------------------------------------------------------------------
    /* Other Taxes section. */

    ShowLineNonZero(58);
    ShowLineNonZero(59);
    ShowLineNonZero(60);
    ShowLineNonZero(61);
    ShowLineNonZero(62);
    showline_wmsg(63,"total tax");
    
    //-------------------------------------------------------------------------------------------
    /* Payments section. */

    //myfprintf(outfile, "%s = %6.2f\n", "L64", atof(TOSTRING64(L[64], word)) );
    showline(64);
    ShowLineNonZero(65);
    ShowLineNonZero(66);
    ShowLineNonZero(67);
    ShowLineNonZero(68);
    ShowLineNonZero(69);
    ShowLineNonZero(70);
    ShowLineNonZero(71);
    showline_wmsg(72,"total payments");

    //-------------------------------------------------------------------------------------------
    /* Refund or Owe section. */

    LT64(temp1_int, L[63], L[72], &set);
    if (temp1_int) {
        SUBTRACT64(L[73], L[72], L[63], &set);
#if defined(DECNUMBER_ARBITRARY)        
 #if defined(WIDTH64)
        {
            decimal64 myout;
            decimal64FromNumber(&myout, &L[73], &set);
            myfprintf(outfile, "(m) L73 = ");
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "  REBATE!!!\n");

            decimal64FromNumber(&myout, &L[74], &set);
            myfprintf(outfile,"(d) L74a = ");
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            decimal128FromNumber(&myout, &L[73], &set);
            myfprintf(outfile, "(m) L73 = ");
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "  REBATE!!!\n");

            decimal128FromNumber(&myout, &L[74], &set);
            myfprintf(outfile,"(d) L74a = ");
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\n");
        }
 #endif
#else
	myfprintf(outfile,"(m) L73 = ");
        myfwrite(&L[73], sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "  REBATE!!!\n");

        myfprintf(outfile,"(d) L74a = ");
        myfwrite(&L[74], sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n");
#endif
        //myfprintf(outfile,"L73 = %6.2f  REBATE!!!\n", atof(TOSTRING64(L[73],word)) );
        //myfprintf(outfile,"L74a = %6.2f \n", atof(TOSTRING64(L[73],word)) );
    }
    else {
        SUBTRACT64(L[76], L[63], L[72], &set);
#if defined(DECNUMBER_ARBITRARY)        
 #if defined(WIDTH64)
        {
            decimal64 myout;
            decimal64FromNumber(&myout, &L[76], &set);
            myfprintf(outfile, "(m) L76 = ");
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "  YOU OWE!!!\n");
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            decimal128FromNumber(&myout, &L[76], &set);
            myfprintf(outfile, "(m) L76 = ");
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "  YOU OWE!!!\n");
        }
 #endif
#else
	myfprintf(outfile,"(m) L76 = ");
        myfwrite(&L[76], sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "  YOU OWE!!!\n");
#endif
        //myfprintf(outfile,"L76 = %6.2f  YOU OWE !!!\n", atof(TOSTRING64(L[76],word)) );
    }
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

void TaxRateFormula( dec_t * tax, dec_t x, int status )  /* Returns tax due. */
{			/* Values taken from page 84 of 1040 Intsructions. */
    dec_t brkpt[4][7];
    dec_t txrt[4][7];
    dec_t sum, temp1;
    int temp1_int;
    int     bracket=0;

    ZERO64(sum);
    
    /* Single */
    ZERO64(brkpt[0][0]);
    COPY64(brkpt[0][1], P_7550);
    COPY64(brkpt[0][2], P_30650);
    COPY64(brkpt[0][3], P_74200);
    COPY64(brkpt[0][4], P_154800);
    COPY64(brkpt[0][5], P_336550);
    COPY64(brkpt[0][6], P_9e9);
    
    /* Married, filing jointly. */
    ZERO64(brkpt[1][0]);
    COPY64(brkpt[1][1], P_15100);
    COPY64(brkpt[1][2], P_61300);
    COPY64(brkpt[1][3], P_123700);
    COPY64(brkpt[1][4], P_188450);
    COPY64(brkpt[1][5], P_336550);
    COPY64(brkpt[1][6], P_9e9);
    
    /* Married, filing separate. */
    ZERO64(brkpt[2][0]);
    COPY64(brkpt[2][1], P_7550);
    COPY64(brkpt[2][2], P_30650);
    COPY64(brkpt[2][3], P_61850);
    COPY64(brkpt[2][4], P_94225);
    COPY64(brkpt[2][5], P_168275);
    COPY64(brkpt[2][6], P_9e9);

    /* Head of Household. */
    ZERO64(brkpt[3][0]);
    COPY64(brkpt[3][1], P_10750);
    COPY64(brkpt[3][2], P_41050);
    COPY64(brkpt[3][3], P_106000);
    COPY64(brkpt[3][4], P_171650);
    COPY64(brkpt[3][5], P_336550);
    COPY64(brkpt[3][6], P_9e9);

    /* Single */
    COPY64(txrt[0][0], P_P1);
    COPY64(txrt[0][1], P_P15);
    COPY64(txrt[0][2], P_P25);
    COPY64(txrt[0][3], P_P28);
    COPY64(txrt[0][4], P_P33);
    COPY64(txrt[0][5], P_P35);
    ZERO64(txrt[0][6]);
    
    /* Married, filing jointly. */
    COPY64(txrt[1][0], P_P1);
    COPY64(txrt[1][1], P_P15);
    COPY64(txrt[1][2], P_P25);
    COPY64(txrt[1][3], P_P28);
    COPY64(txrt[1][4], P_P33);
    COPY64(txrt[1][5], P_P35);
    ZERO64(txrt[1][6]);
        
    /* Married, filing separate. */
    COPY64(txrt[2][0], P_P1);
    COPY64(txrt[2][1], P_P15);
    COPY64(txrt[2][2], P_P25);
    COPY64(txrt[2][3], P_P28);
    COPY64(txrt[2][4], P_P33);
    COPY64(txrt[2][5], P_P35);
    ZERO64(txrt[2][6]);
    
    /* Head of Household. */
    COPY64(txrt[3][0], P_P1);
    COPY64(txrt[3][1], P_P15);
    COPY64(txrt[3][2], P_P25);
    COPY64(txrt[3][3], P_P28);
    COPY64(txrt[3][4], P_P33);
    COPY64(txrt[3][5], P_P35);
    ZERO64(txrt[3][6]);

    if (status==WIDOW) status = MARRIED_FILLING_JOINTLY;  /* Handle case of widow(er). */
    status = status - 1;  /* Arrays start at zero; not one. */
    
    LT64(temp1_int, brkpt[status][bracket+1], x, &set);
    while (temp1_int) {
        SUBTRACT64(temp1, brkpt[status][bracket+1], brkpt[status][bracket], &set);
        MULTIPLY64(temp1, temp1, txrt[status][bracket], &set);
        ADD64(sum, sum, temp1, &set);
        bracket = bracket + 1;
        LT64(temp1_int, brkpt[status][bracket+1], x, &set);
    }
      
    SUBTRACT64(temp1, x, brkpt[status][bracket], &set);
    MULTIPLY64(temp1, temp1, txrt[status][bracket], &set);
    ADD64((*tax), temp1, sum, &set);
}


void TaxRateFunction( dec_t * tax, dec_t income, int status )     /* Emulates table lookup or function appropriately. */
{
    dec_t x, dx;
    //dec_t temp1, temp2, temp3;
    dec_t temp1;
    int temp1_int, temp2_int, temp3_int;
    dec_t k; //was an int
    
    LT64(temp1_int, income, P_100000, &set);
    if (temp1_int) { /* Quantize to match tax-table exactly. */
   
        LT64(temp1_int, income, P_5, &set);
        LT64(temp2_int, income, P_25, &set);
        LT64(temp3_int, income, P_3000, &set);

        /*
	if (ISNEGATIVE64(temp1))      { COPY64(x, P_5); }
        else if (ISNEGATIVE64(temp2)) { COPY64(x, P_10); }
        else if (ISNEGATIVE64(temp3)) { COPY64(x, P_25); }
        else {                          COPY64(x, P_50); }
        */
	if (temp1_int)      { COPY64(x, P_5); }
        else if (temp2_int) { COPY64(x, P_10); }
        else if (temp3_int) { COPY64(x, P_25); }
        else {                          COPY64(x, P_50); }
        
        if (CHECK_ORIG) {
            myprintf("[2] Exact code as original software (for checking purposes)\n");
            MULTIPLY64(dx, P_P5, x, &set);
            DIVIDE64(k, income, x, &set);
            SET_ROUNDMODE(set, RNA);    // 0.5 rounds up
            TOINTEGRAL64(k, k, &set);
            SET_ROUNDMODE(set, RNE);  //defualt
            MULTIPLY64(temp1, x, k, &set);
            ADD64(x, temp1, dx, &set);
            TaxRateFormula( tax, x, status );

            SET_ROUNDMODE(set, RNA);    // 0.5 rounds up
            TOINTEGRAL64((*tax), (*tax), &set);
            SET_ROUNDMODE(set, RNE);  //defualt
            
            //k = income / x;
            //x = x * (float)k + dx;
            //tx = (int)(TaxRateFormula( x, status ) + 0.5);
        }
        else {
            MULTIPLY64(dx, P_P5, x, &set);
            DIVIDE64(k, income, x, &set);
            SET_ROUNDMODE(set, RTZ);    // 0.5 rounds up
            TOINTEGRAL64(k, k, &set);
            SET_ROUNDMODE(set, RNE);  //defualt
            MULTIPLY64(x, x, k, &set);
            ADD64(x, x, dx, &set);
            TaxRateFormula( tax, x, status );
        }
    }
    else
        TaxRateFormula( tax, income, status );
}



/*-----------------------------------------------------------------*/
/* Qualified Dividends and Capital Gain Tax Worksheet for Line 44. */
/*  From page 38 of instructions.				   */
/*-----------------------------------------------------------------*/
void capgains_qualdividends_worksheets( int status, dec_t L9b )
{
    dec_t ws[50];
    int j, skipto16=0;
    //dec_t temp1;
    int temp1_int;

    for (j=0; j<50; j++) ZERO64(ws[j]);
    COPY64(ws[1], L[43]);
    COPY64(ws[2], L9b);
    if (Do_SchedD){
        MIN64(ws[3], SchedD[15], SchedD[16], &set);
        NotLessThanZero(&ws[3], ws[3]);
    }
    else
        COPY64(ws[3], L[13]);
    ADD64(ws[4], ws[2], ws[3], &set);
    ZERO64(ws[5]);  /* Investment interest expense, form 4952, usually 0. */
    
    SUBTRACT64(ws[6], ws[4], ws[5], &set);
    NotLessThanZero(&ws[6], ws[6]);
    SUBTRACT64(ws[7], ws[1], ws[6], &set);
    NotLessThanZero(&ws[7], ws[7]);
    switch (status) {
    case SINGLE:
    case MARRIED_FILLING_SEPARAT: MIN64(ws[8], ws[1], P_30650, &set); break;
    case MARRIED_FILLING_JOINTLY:
    case WIDOW:                   MIN64(ws[8], ws[1], P_61300, &set); break;
    case HEAD_OF_HOUSEHOLD:       MIN64(ws[8], ws[1], P_41050, &set); break;
    }
    
    LT64(temp1_int, ws[7], ws[8], &set);
    if (temp1_int) {
        COPY64(ws[9], ws[7]);
        SUBTRACT64(ws[10], ws[8], ws[9], &set);
        MULTIPLY64(ws[11], P_P05, ws[10], &set);
        EQ64(temp1_int, ws[6], ws[10], &set);
        if (temp1_int) skipto16 = Yes;
	    else MIN64(ws[12], ws[1], ws[6], &set);
    }
    else MIN64(ws[12], ws[1], ws[6], &set);

    if (!skipto16) {
        COPY64(ws[13], ws[10]);
        SUBTRACT64(ws[14], ws[12], ws[13], &set);
        MULTIPLY64(ws[15], P_P15, ws[14], &set);
    }
    TaxRateFunction( &ws[16], ws[7], status );
    ADD64(ws[17], ws[11], ws[15], &set);
    ADD64(ws[17], ws[17], ws[16], &set);
    TaxRateFunction( &ws[18], ws[1], status );
    MIN64(ws[19], ws[17], ws[18], &set);
    COPY64(L[44], ws[19]);
}


void AlternativeMinimumTax( dec_t * amt, int status, int itemized )
{ /* Worksheet calculations, page 39. */
    int i;
    dec_t WS[100];
    //dec_t temp1, temp2;
    dec_t temp1;
    int temp1_int, temp2_int;
    
    ZERO64(*amt);
 
    for (i=0; i<100; i++) ZERO64(WS[i]);
    if (itemized) {
        COPY64(WS[1], L[41]);
        MULTIPLY64(temp1, P_P025, L[38], &set);
        MIN64(WS[2], SchedA[4], temp1, &set);
        ADD64(WS[3], SchedA[9], SchedA[26], &set);
        ADD64(WS[4], WS[1], WS[2], &set);
        ADD64(WS[4], WS[4], WS[3], &set);
    }
    else COPY64(WS[4], L[38]);
    ADD64(WS[5], L[10], L[21], &set);
    SUBTRACT64(WS[6], WS[4], WS[5], &set);
    ZERO64(WS[7]);	/* Amount from Form 8914, usually zero. */
    SUBTRACT64(WS[8], WS[6], WS[7], &set);
    switch (status) {
    case SINGLE:
    case HEAD_OF_HOUSEHOLD:        COPY64(WS[9], P_42500); break;
    case MARRIED_FILLING_JOINTLY:
    case WIDOW:                    COPY64(WS[9], P_62550); break;
    case MARRIED_FILLING_SEPARAT:  COPY64(WS[9], P_31275); break;
    }
    LT64(temp1_int, WS[8], WS[9], &set);
    EQ64(temp2_int, WS[8], WS[9], &set);
    if (temp1_int || temp2_int) {
        //ZERO64(temp1);  /* You do not owe AMT. */
        //return temp1;
        ZERO64(*amt); /* You do not owe AMT. */
        return;
    }
    else SUBTRACT64(WS[10], WS[8], WS[9], &set);
    switch (status) {
    case SINGLE:
    case HEAD_OF_HOUSEHOLD:       COPY64(WS[11], P_112500); break;
    case MARRIED_FILLING_JOINTLY:
    case WIDOW:                   COPY64(WS[11], P_150000); break;
    case MARRIED_FILLING_SEPARAT: COPY64(WS[11], P_75000); break;
    }
    LT64(temp1_int, WS[11], WS[8], &set);
    if (temp1_int) {
        SUBTRACT64(WS[12], WS[8], WS[11], &set);
        MULTIPLY64(WS[13], P_P25, WS[12], &set);
        LT64(temp1_int, WS[9], WS[13], &set);
        if (temp1_int)
	    COPY64(WS[13], WS[9]);
        ADD64(WS[14], WS[10], WS[13], &set);
    }
    else COPY64(WS[14], WS[10]);

    LT64(temp1_int, P_175000, WS[14], &set);
    LT64(temp2_int, P_87500, WS[14], &set);
        
    /*
    if (((status!=MARRIED_FILLING_SEPARAT) && (ISNEGATIVE64(temp1))) ||
        ((status==MARRIED_FILLING_SEPARAT) && (ISNEGATIVE64(temp2)))) {
    */
    if (((status!=MARRIED_FILLING_SEPARAT) && (temp1_int)) ||
        ((status==MARRIED_FILLING_SEPARAT) && (temp2_int))) {
        //Note, the myAMT value has been hardcoded here.. Change this to read from
        // some file or standard input
        COPY64((*amt), P_100);
        
        //float myAMT;
        //char word[2000];
        //printf("You may owe AMT. Fill out form 6251.\nEnter your AMT tax ? ");
        //scanf("%f",&myAMT);
        //mysprintf(word, "%f", myAMT);
        //FROMSTRING64(AMT, word, &set);
    }
    else {
        MULTIPLY64(WS[15], P_P26, WS[14], &set);
        SUBTRACT64(WS[16], L[44], L[47], &set);
        LT64(temp1_int, WS[16], WS[15], &set);
        if (temp1_int) {
	    //Note, the myAMT value has been hardcoded here.. Change this to read from
            // some file or standard input
            COPY64((*amt), P_100);

            //float myAMT;
            //char word[2000];
            //printf("You may owe AMT. Fill out form 6251.\nEnter your AMT tax ? ");
            //scanf("%f",&myAMT);
            //mysprintf(word, "%f", myAMT);
            //FROMSTRING64(AMT, word, &set);
        }
        else ZERO64(*amt);
    }
    return;
}

void ImportFederalReturnData( char *fedlogfile, struct FedReturnData *fed_data )
{
    FILE *infile;
    char fline[1000], tmpstr[1000], word[1000];
    int linenum;

    for (linenum=0; linenum<MAX_LINES; linenum++) {
        ZERO64(fed_data->fedline[linenum]);
        ZERO64(fed_data->schedD[linenum]);
    }
    infile = myfopen(fedlogfile, "r");
    if (infile==0) {printf("Error: Could not open federal return '%s'\n", fedlogfile);}
    fed_data->Itemized = 1; /* Set initial default values. */
    read_line(infile,fline);  linenum = 0;
    while (!myfeof(infile)) {
        if (mystrstr(fline,"Use standard deduction.")!=0) fed_data->Itemized = 0;
        next_word(fline, tmpstr, word, " \t=");
        if ((mystrstr(word,"L")==word) && (mystrstr(fline," = ")!=0)) {
            if (mysscanf(&word[1],"%d",&linenum)!=1) myprintf("Error: Reading fed line number '%s%s'\n",word,fline);
            next_word(fline, tmpstr, word, " \t=");
            FROMSTRING64(fed_data->fedline[linenum], word, &set);
            //if (mysscanf(word,"%f", &fed_data->fedline[linenum])!=1) 
            //printf("Error: Reading fed line %d '%s%s'\n",linenum,word,fline);
	    //if (verbose) myprintf("FedLin[%d] = %2.2f\n", linenum, atof(TOSTRING64(fed_data->fedline[linenum], word)));
            TOSTRING64(fed_data->fedline[linenum], word);
	    if (verbose) myprintf("FedLin[%d] = %2.2f\n", linenum, atof(word));
        }
        if ((mystrstr(word,"D")==word) && (mystrstr(fline," = ")!=0)) {
            if (mysscanf(&word[1],"%d",&linenum)!=1) myprintf("Error: Reading fed line number '%s%s'\n",word,fline);
            next_word(fline, tmpstr, word, " \t=");
            FROMSTRING64(fed_data->schedD[linenum], word, &set);
            //if (mysscanf(word,"%f", &fed_data->schedD[linenum])!=1) 
            //printf("Error: Reading fed schedD %d '%s%s'\n",linenum,word,fline);
            //if (verbose) myprintf("FedLin[%d] = %2.2f\n", linenum, atof(TOSTRING64(fed_data->schedD[linenum], word)));
            TOSTRING64(fed_data->schedD[linenum], word);
	    if (verbose) myprintf("FedLin[%d] = %2.2f\n", linenum, atof(word));
        }
        read_line(infile,fline);
    }
    myfclose(infile);
}


void CapitalLossCarryOverWorksheet( char *fedlogfile, struct FedReturnData *LastYearsReturn )
{ /* From page D-7. */
    dec_t ws[50];
    int k;

    ImportFederalReturnData( fedlogfile, LastYearsReturn );
    for (k=0; k<50; k++) ZERO64(ws[k]);
    COPY64(ws[1], LastYearsReturn->fedline[41]);
    ZERO64(ws[2]);	/* Hurricane Katrina relief, usually zero, else enter 2005 Form 8914 line 2 amt. */
    SUBTRACT64(ws[3], ws[1], ws[2], &set);
    if (ISNEGATIVE64(LastYearsReturn->schedD[21]))
        MINUS64(ws[4], LastYearsReturn->schedD[21], &set);
    ADD64(ws[5], ws[3], ws[4], &set); /* "Combine" lines 1 and 2. */
    NotLessThanZero(&ws[5], ws[5]);
    MIN64(ws[6], ws[4], ws[5], &set);

    if (ISNEGATIVE64(LastYearsReturn->schedD[7])) {
        MINUS64(ws[7], LastYearsReturn->schedD[7], &set);
        if (ISPOSITIVE64(LastYearsReturn->schedD[15]) && (ISZERO64(LastYearsReturn->schedD[15])==0))
            COPY64(ws[8], LastYearsReturn->schedD[15]);
        ADD64(ws[9], ws[6], ws[8], &set);
        SUBTRACT64(ws[10], ws[7], ws[9], &set);
        NotLessThanZero(&ws[10], ws[10]);
        if (ISPOSITIVE64(ws[10]) && (ISZERO64(ws[10])==0))
            COPY64(SchedD[6], ws[10]);
    }
    else ZERO64(ws[7]);

    if (ISNEGATIVE64(LastYearsReturn->schedD[15]))
        MINUS64(ws[11], LastYearsReturn->schedD[15], &set);

    if (ISPOSITIVE64(LastYearsReturn->schedD[7]) && (ISZERO64(LastYearsReturn->schedD[7])==0))
        ws[12] = LastYearsReturn->schedD[7];
    SUBTRACT64(ws[13], ws[6], ws[7], &set);
    NotLessThanZero(&ws[13], ws[13]);
    ADD64(ws[14], ws[12], ws[13], &set);
    SUBTRACT64(ws[15], ws[11], ws[14], &set);
    NotLessThanZero(&ws[15], ws[15]);

    if (ISPOSITIVE64(ws[15]) && (ISZERO64(ws[15])==0))
        COPY64(SchedD[14], ws[15]);
}


/****************************************************************/
/* Get_Cap_Gains - Get and calculate gains.			*/
/* Like "get_params", but must get transaction dates.		*/
/* Expect entries in double pairs. 				*/
/*   buy_amnt   date 						*/
/*   sell_amnt  date 						*/
/*								*/
/****************************************************************/

void get_cap_gains( char *emssg, char * capgain[])
{
    char *LastYearsOutFile=0;
    int i;
    
    //if (Do_SchedD) myfprintf(outfile," Cap Gains Schedule-D\n");
    //-------------------------------------------------------------------------------------------
    if (LastYearsOutFile!=0)
        CapitalLossCarryOverWorksheet( LastYearsOutFile, &LastYearsReturn );
    
    if (ISPOSITIVE64(SchedD[6]) && (ISZERO64(SchedD[6])==0)) {
        myfprintf(outfile,"(c) Warning: D6 Carryover Loss must be NEGATIVE.\n");
        MINUS64(SchedD[6], SchedD[6], &set);
    }
    if (ISPOSITIVE64(SchedD[14]) && (ISZERO64(SchedD[14])==0)) {
        myfprintf(outfile,"(c) Warning: D14 Carryover Loss must be NEGATIVE.\n");
        MINUS64(SchedD[14], SchedD[14], &set);
    }
    if ((ISZERO64(SchedD[13])==0) || (ISZERO64(SchedD[6])==0) || (ISZERO64(SchedD[14])==0)) {
        myfprintf(outfile,"(c)  Cap Gains/Losses Schedule-D\n");
        Do_SchedD = Yes;}

    for (i=0; i<10; i++) {
        myfprintf(outfile, capgain[i]); 
        if (mystrstr(capgain[i], "(d)") != 0) {
#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
            {
                decimal64 myout;
                decimal64FromNumber(&myout, &capgain_amt[i], &set);
                myfwrite(&myout, sizeof(decimal64), 1, outfile);
                myfprintf(outfile, "\n");
            }
 #endif
 #if defined(WIDTH128)
            {
                decimal128 myout;
                decimal128FromNumber(&myout, &capgain_amt[i], &set);
                myfwrite(&myout, sizeof(decimal128), 1, outfile);
                myfprintf(outfile, "\n");
            }
 #endif
#else
            myfwrite(&capgain_amt[i], sizeof(dec_t), 1, outfile);
            myfprintf(outfile, "\n");
#endif
        }
    }
    
    //-------------------------------------------------------------------------------------------

    if (Do_SchedD) { /*Sched-D*/
        int i;
        Do_QDCGTW = Yes;	/* Tentatively set to do: Qualified Dividends and Captial Gain tax Worksheet. */
        ADD64(stcg, stcg, SchedD[6], &set);
        COPY64(SchedD[7], stcg);
        ADD64(SchedD[15], ltcg, SchedD[13], &set);
        ADD64(SchedD[15], SchedD[15], SchedD[14], &set);
        ADD64(SchedD[16], SchedD[7], SchedD[15], &set);
        
#if defined(DECNUMBER_ARBITRARY)        
 #if defined(WIDTH64)
        {
            decimal64 myout;
            decimal64FromNumber(&myout, &stcg, &set);
            myfprintf(outfile, "(d) 	Total Short-term Gains = ");
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            decimal64FromNumber(&myout, &ltcg, &set);
            myfprintf(outfile, "\n(d) 	Total Long-term Gains = ");
            myfwrite(&myout, sizeof(decimal64), 1, outfile);
            myfprintf(outfile, "\n");
            for (i = 3; i < 17; i++) {
                decimal64FromNumber(&myout, &SchedD[i], &set);
                if (i == 7) {
                    myfprintf(outfile, "(m)  D%d = ", i);
                    myfwrite(&myout, sizeof(decimal64), 1, outfile);
                    myfprintf(outfile,"		Net short-term capital gain or loss\n");
                }
                else if (i == 15) {
                    myfprintf(outfile, "(m)  D%d = ", i);
                    myfwrite(&myout, sizeof(decimal64), 1, outfile);
                    myfprintf(outfile,"		Net long-term capital gain or loss\n");
                }
                else if (i == 8 || i == 9 || i == 11 || i == 12) {
                    //do nothing
                }
                else {
                    myfprintf(outfile, "(d)  D%d = ", i);
                    myfwrite(&myout, sizeof(decimal64), 1, outfile);
                    myfprintf(outfile, "\n");
                }
            }
        }
 #endif
 #if defined(WIDTH128)
        {
            decimal128 myout;
            decimal128FromNumber(&myout, &stcg, &set);
            myfprintf(outfile, "(d) 	Total Short-term Gains = ");
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            decimal128FromNumber(&myout, &ltcg, &set);
            myfprintf(outfile, "\n(d) 	Total Long-term Gains = ");
            myfwrite(&myout, sizeof(decimal128), 1, outfile);
            myfprintf(outfile, "\n");
            for (i = 3; i < 17; i++) {
                decimal128FromNumber(&myout, &SchedD[i], &set);
                if (i == 7) {
                    myfprintf(outfile, "(m)  D%d = ", i);
                    myfwrite(&myout, sizeof(decimal128), 1, outfile);
                    myfprintf(outfile,"		Net short-term capital gain or loss\n");
                }
                else if (i == 15) {
                    myfprintf(outfile, "(m)  D%d = ", i);
                    myfwrite(&myout, sizeof(decimal128), 1, outfile);
                    myfprintf(outfile,"		Net long-term capital gain or loss\n");
                }
                else if (i == 8 || i == 9 || i == 11 || i == 12) {
                    //do nothing
                }
                else {
                    myfprintf(outfile, "(d)  D%d = ", i);
                    myfwrite(&myout, sizeof(decimal128), 1, outfile);
                    myfprintf(outfile, "\n");
                }
            }
        }
 #endif
#else
	myfprintf(outfile, "(d) 	Total Short-term Gains = ");
        myfwrite(&stcg, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n(d) 	Total Long-term Gains = ");
        myfwrite(&ltcg, sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n");
        for (i = 3; i < 17; i++) {
            if (i == 7) {
                myfprintf(outfile, "(m)  D%d = ", i);
                myfwrite(&SchedD[7], sizeof(dec_t), 1, outfile);
                myfprintf(outfile,"		Net short-term capital gain or loss\n");
            }
            else if (i == 15) {
                myfprintf(outfile, "(m)  D%d = ", i);
                myfwrite(&SchedD[15], sizeof(dec_t), 1, outfile);
                myfprintf(outfile,"		Net long-term capital gain or loss\n");
            }
            else if (i == 8 || i == 9 || i == 11 || i == 12) {
                //do nothing
            }
            else {
                myfprintf(outfile, "(d)  D%d = ", i);
                myfwrite(&SchedD[i], sizeof(dec_t), 1, outfile);
                myfprintf(outfile, "\n");
            }
        }
#endif
        
        //myfprintf(outfile,"	Total Short-term Gains = %6.2f\n	Total Long-term Gains = %6.2f\n", atof(TOSTRING64(stcg, word)), atof(TOSTRING64(ltcg, word)));
        //myfprintf(outfile," D3 = %6.2f\n", atof(TOSTRING64(SchedD[3], word)) );
        //myfprintf(outfile," D4 = %6.2f\n", atof(TOSTRING64(SchedD[4], word)) );
        //myfprintf(outfile," D5 = %6.2f\n", atof(TOSTRING64(SchedD[5], word)) );
        //myfprintf(outfile," D6 = %6.2f\n", atof(TOSTRING64(SchedD[6], word)) );
        
        //myfprintf(outfile," D7 = %6.2f		Net short-term capital gain or loss\n", atof(TOSTRING64(SchedD[7], word)) );
        //myfprintf(outfile," D10 = %6.2f\n", atof(TOSTRING64(SchedD[10], word)) );
        //myfprintf(outfile," D13 = %6.2f\n", atof(TOSTRING64(SchedD[13], word)) );
        //myfprintf(outfile," D14 = %6.2f\n", atof(TOSTRING64(SchedD[14], word)) );
        //myfprintf(outfile," D15 = %6.2f		Net long-term capital gain or loss\n", atof(TOSTRING64(SchedD[15], word)) );
        //myfprintf(outfile," D16 = %6.2f\n", atof(TOSTRING64(SchedD[16], word)) );
        
        /* Part ||| */
        if (ISPOSITIVE64(SchedD[16]) || ISZERO64(SchedD[16])) { /*gain*/
            COPY64(L[13], SchedD[16]);

            if (ISPOSITIVE64(SchedD[15]) && (ISZERO64(SchedD[15])==0) &&
                ISPOSITIVE64(SchedD[16]) && (ISZERO64(SchedD[16])==0)) { /* Lines 18-21 */
                dec_t wsd[50];
                dec_t temp1, temp2, temp3;
                
                myfprintf(outfile,"(c)  D17 = yes\n");

                /* '28% Rate Gain Worksheet' on instructions page D-7. */
                ZERO64(wsd[1]);	/* Gain or losses from "Collectibles" only.  Usually zero. */
                ZERO64(wsd[2]);	/* Any 1202 exclusions, usually 0.0. */
                ZERO64(wsd[3]);	/* Any collectibles on forms 4684, 6245, 6781, 8824. Usually no. */
                ZERO64(wsd[4]);	/* Any collectibles 1099-Div box 2d, 2439 box 1d, or K-1's. Usually no. */
                COPY64(wsd[5], SchedD[14]);
                
                if (ISNEGATIVE64(SchedD[7])) COPY64(wsd[6], SchedD[7]);
                else                         ZERO64(wsd[6]);

                ADD64(temp1,  wsd[1], wsd[2], &set);
                ADD64(temp2,  wsd[3], wsd[4], &set);
                ADD64(temp3,  wsd[5], wsd[6], &set);
                ADD64(wsd[7], temp1,  temp2,  &set);
                ADD64(wsd[7], wsd[7], temp3,  &set);
                NotLessThanZero(&wsd[7], wsd[7]);
                COPY64(SchedD[18], wsd[7]);

#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
                {
                    decimal64 myout;
                    decimal64FromNumber(&myout, &SchedD[18], &set);
                    myfprintf(outfile, "(d)  D18 = ");
                    myfwrite(&myout, sizeof(decimal64), 1, outfile);
                    decimal64FromNumber(&myout, &SchedD[19], &set);
                    myfprintf(outfile, "\n(d)  D19 = ");
                    myfwrite(&myout, sizeof(decimal64), 1, outfile);
                    myfprintf(outfile, "\n");
                }
 #endif
 #if defined(WIDTH128)
                {
                    decimal128 myout;
                    decimal128FromNumber(&myout, &SchedD[18], &set);
                    myfprintf(outfile, "(d)  D18 = ");
                    myfwrite(&myout, sizeof(decimal128), 1, outfile);
                    decimal128FromNumber(&myout, &SchedD[19], &set);
                    myfprintf(outfile, "\n(d)  D19 = ");
                    myfwrite(&myout, sizeof(decimal128), 1, outfile);
                    myfprintf(outfile, "\n");
                }
 #endif
#else
		myfprintf(outfile, "(d)  D18 = ");
                myfwrite(&SchedD[18], sizeof(dec_t), 1, outfile);
                myfprintf(outfile, "\n(d)  D19 = ");
                myfwrite(&SchedD[19], sizeof(dec_t), 1, outfile);
                myfprintf(outfile, "\n");
#endif
                //myfprintf(outfile," D18 = %6.2f\n", atof(TOSTRING64(SchedD[18], word)) );

                /* 'Unrecaptured Section 1250 Gain Worksheet' on page D7, usually 0. */
                //myfprintf(outfile," D19 = %6.2f\n", atof(TOSTRING64(SchedD[19], word)) );

                if (ISZERO64(SchedD[18]) && ISZERO64(SchedD[19])) {
                    myfprintf(outfile,"(c)  D20 = yes\n");
                    // myprintf("Complete 'Qualified Dividends and Captial Gain tax Worksheet', instructions page 38.\n");
                    Do_QDCGTW = Yes;
                }
                else {
                    myfprintf(outfile,"(c)  D20 = no\n");
                    // myprintf("Complete 'Schedule D Tax Worksheet', instructions page D-10.\n");
                    Do_SDTW = Yes;
                    Do_QDCGTW = No;
                }
                doline22 = 0;
            }
            else {
                myfprintf(outfile, "(c)  D17 = no\n");
                doline22 = Yes;
            }
        }
        else { /*loss*/	/* Schedule-D line 21. Skip here from line 16 if a loss. */
            dec_t maxloss;
            //dec_t temp;
	    int temp_int;
            
            if (status==MARRIED_FILLING_SEPARAT) COPY64(maxloss, N_1500);
            else                                 COPY64(maxloss, N_3000);

            LT64(temp_int, SchedD[16], maxloss, &set);
            if (temp_int) COPY64(SchedD[21], maxloss);
	        else COPY64(SchedD[21], SchedD[16]);

#if defined(DECNUMBER_ARBITRARY)
 #if defined(WIDTH64)
            {
                decimal64 myout;
                decimal64FromNumber(&myout, &SchedD[21], &set);
                myfprintf(outfile, "(d)  D21 = ");
                myfwrite(&myout, sizeof(decimal64), 1, outfile);
                myfprintf(outfile, "\n");
            }
 #endif
 #if defined(WIDTH128)
            {
                decimal128 myout;
                decimal128FromNumber(&myout, &SchedD[21], &set);
                myfprintf(outfile, "(d)  D21 = ");
                myfwrite(&myout, sizeof(decimal128), 1, outfile);
                myfprintf(outfile, "\n");
            }
 #endif
#else
	    myfprintf(outfile, "(d)  D21 = ");
            myfwrite(&SchedD[21], sizeof(dec_t), 1, outfile);
            myfprintf(outfile, "\n");
#endif
            //myfprintf(outfile," D21 = %6.2f\n", atof(TOSTRING64(SchedD[21], word)) );
            COPY64(L[13], SchedD[21]);
            doline22 = Yes;
        }

        if (doline22) {
            if (ISPOSITIVE64(L9b) && (ISZERO64(L9b)==0)) {
                myfprintf(outfile,"(c)  D22 = yes\n");
                // myprintf("Complete 'Qualified Dividends and Captial Gain tax Worksheet', instructions page 34.\n");
                Do_QDCGTW = Yes;	
            }
            else
                myfprintf(outfile,"(c)  D22 = no\n");
        }

    } /*Sched-D*/
}


/*----------------------------------------------*/
/* 'Schedule D Tax Worksheet', page D9.		*/
/*----------------------------------------------*/
void sched_D_tax_worksheet( int status, dec_t L9b )
{
    dec_t ws[100];
    int k;
    dec_t temp1, temp2;
    int temp1_int, temp2_int;
        
    for (k=0; k<100; k++) ZERO64(ws[k]);

    COPY64(ws[1], L[43]);
    COPY64(ws[2], L9b);
    ZERO64(ws[3]);	/* Form 4952, line 4g. Usually 0.0. */
    ZERO64(ws[4]);	/* Form 4952, line 4e. Usually 0.0. */
    SUBTRACT64(ws[5], ws[3], ws[4], &set);
    NotLessThanZero(&ws[5], ws[5]);
    SUBTRACT64(ws[6], ws[2], ws[5], &set);
    NotLessThanZero(&ws[6], ws[6]);
    MIN64(ws[7], SchedD[15], SchedD[16], &set);
    MIN64(ws[8], ws[3], ws[4], &set);
    SUBTRACT64(ws[9], ws[7], ws[8], &set);
    NotLessThanZero(&ws[9], ws[9]);
    ADD64(ws[10], ws[6], ws[9], &set);
    ADD64(ws[11], SchedD[18], SchedD[19], &set);
    MIN64(ws[12], ws[9], ws[11], &set);
    SUBTRACT64(ws[13], ws[10], ws[12], &set);
    SUBTRACT64(ws[14], ws[1], ws[13], &set);
    NotLessThanZero(&ws[14], ws[14]);
    //schedD_ws_13 = atof(TOSTRING64(ws[13], word));
    //schedD_ws_14 = atof(TOSTRING64(ws[14], word));
    COPY64(schedD_ws_13, ws[13]);
    COPY64(schedD_ws_14, ws[14]);

    switch (status) {
    case SINGLE:
    case MARRIED_FILLING_SEPARAT: MIN64(ws[15], ws[1], P_30650, &set); break;
    case MARRIED_FILLING_JOINTLY:
    case WIDOW:                   MIN64(ws[15], ws[1], P_61300, &set); break;
    case HEAD_OF_HOUSEHOLD:       MIN64(ws[15], ws[1], P_41050, &set); break;
    }
    MIN64(ws[16], ws[14], ws[15], &set);

    SUBTRACT64(ws[17], ws[1], ws[10], &set);
    NotLessThanZero(&ws[17], ws[17]);

    MAX64(ws[18], ws[16], ws[17], &set);
    SUBTRACT64(ws[19], ws[15], ws[16], &set);
    MULTIPLY64(ws[20], P_P05, ws[19], &set);
    
    NE64(temp1_int, ws[1], ws[15], &set);
    if (temp1_int) {
        MIN64(ws[21], ws[1], ws[13], &set);
        COPY64(ws[22], ws[19]);
        SUBTRACT64(ws[23], ws[21], ws[22], &set);
        NotLessThanZero(&ws[23], ws[23]);
        MULTIPLY64(ws[24], P_P15, ws[23], &set);
	LT64(temp1_int, ZERO, SchedD[19], &set);
        LT64(temp2_int, ZERO, SchedD[18], &set);
        if (temp1_int) {
	    MIN64(ws[25], ws[9], SchedD[19], &set);
            ADD64(ws[26], ws[10], ws[18], &set);
            COPY64(ws[27], ws[1]);
            SUBTRACT64(ws[28], ws[26], ws[27], &set);
            NotLessThanZero(&ws[28], ws[28]);
            SUBTRACT64(ws[29], ws[25], ws[28], &set);
            NotLessThanZero(&ws[29], ws[29]);
            MULTIPLY64(ws[30], P_P25, ws[29], &set);
        }
        //if (ISNEGATIVE64(temp2)) {
        if (temp2_int) {
            ADD64(temp1, ws[18], ws[19], &set);
            ADD64(temp2, ws[23], ws[29], &set);
            ADD64(ws[31], temp1, temp2, &set);
            SUBTRACT64(ws[32], ws[1], ws[31], &set);
            MULTIPLY64(ws[33], P_P28, ws[32], &set);
        }
    }
    TaxRateFunction( &ws[34], ws[18], status );
    ADD64(temp1, ws[20], ws[24], &set);
    ADD64(temp2, ws[30], ws[33], &set);
    ADD64(temp1, temp1, ws[34], &set);
    ADD64(ws[35], temp1, temp2, &set);
    TaxRateFunction( &ws[36], ws[1], status );
    MIN64(ws[37], ws[35], ws[36], &set);
    COPY64(L[44], ws[37]);
}

void showschedA( int linenum )
{
    //char word[2000];
#if defined(DECNUMBER_ARBITRARY)    
 #if defined(WIDTH64)
    if (ISPOSITIVE64(SchedA[linenum]) && (ISZERO64(SchedA[linenum]) == 0)) {
        decimal64 myout;
        decimal64FromNumber(&myout, &SchedA[linenum], &set);
        myfprintf(outfile, "(d)  A%d = ", linenum);
        myfwrite(&myout, sizeof(decimal64), 1, outfile);
        myfprintf(outfile, "\n");
    }
 #endif
 #if defined(WIDTH128)
    if (ISPOSITIVE64(SchedA[linenum]) && (ISZERO64(SchedA[linenum]) == 0)) {
        decimal128 myout;
        decimal128FromNumber(&myout, &SchedA[linenum], &set);
        myfprintf(outfile, "(d)  A%d = ", linenum);
        myfwrite(&myout, sizeof(decimal128), 1, outfile);
        myfprintf(outfile, "\n");
    }
 #endif
#else
    if (ISPOSITIVE64(SchedA[linenum]) && (ISZERO64(SchedA[linenum]) == 0)) {
        myfprintf(outfile, "(d)  A%d = ", linenum);
        myfwrite(&SchedA[linenum], sizeof(dec_t), 1, outfile);
        myfprintf(outfile, "\n");
    }
#endif
    //if (ISPOSITIVE64(SchedA[linenum]) && (ISZERO64(SchedA[linenum]) == 0))
    //    myfprintf(outfile," A%d = %6.2f\n", linenum, atof(TOSTRING64(SchedA[linenum], word)));
}

void printL6_L12()
{
    shownum(6);
    showline(7);
#if defined(DECNUMBER_ARBITRARY)    
 #if defined(WIDTH64)
    {
        decimal64 myout;
        decimal64FromNumber(&myout, &L[8], &set);
        myfprintf(outfile, "(d) L8a = ");
        myfwrite(&myout, sizeof(decimal64), 1, outfile);
        myfprintf(outfile, "\n");
    }
 #endif
 #if defined(WIDTH128)
    {
        decimal128 myout;
        decimal128FromNumber(&myout, &L[8], &set);
        myfprintf(outfile, "(d) L8a = ");
        myfwrite(&myout, sizeof(decimal128), 1, outfile);
        myfprintf(outfile, "\n");
    }
 #endif
#else
    myfprintf(outfile, "(d) L8a = ");
    myfwrite(&L[8], sizeof(dec_t), 1, outfile);
    myfprintf(outfile, "\n");
#endif
    //tempf = atof(TOSTRING64(L[8],word));
    //myfprintf(outfile, "L8a = %6.2f\n", tempf);
    showline(9);
#if defined(DECNUMBER_ARBITRARY)    
 #if defined(WIDTH64)
    {
        decimal64 myout;
        decimal64FromNumber(&myout, &L9b, &set);
        myfprintf(outfile, "(d) L9b = ");
        myfwrite(&myout, sizeof(decimal64), 1, outfile);
        myfprintf(outfile, "\n");
    }
 #endif
 #if defined(WIDTH128)
    {
        decimal128 myout;
        decimal128FromNumber(&myout, &L9b, &set);
        myfprintf(outfile, "(d) L9b = ");
        myfwrite(&myout, sizeof(decimal128), 1, outfile);
        myfprintf(outfile, "\n");
    }
 #endif
#else
    myfprintf(outfile, "(d) L9b = ");
    myfwrite(&L9b, sizeof(dec_t), 1, outfile);
    myfprintf(outfile, "\n");
#endif
    //tempf = atof(TOSTRING64(L9b,word));
    //myfprintf(outfile, "L9b = %6.2f\n", tempf);
    showline(10);
    showline(11);
    showline(12);
       
}
