/************************************************************************/
/* TaxSolve_Routines.c - General purpose reusable routines for making	*/
/*  tax programs.  These routines are not specific to any particular	*/
/*  tax form or country.  This file is usually compiled-with, linked-	*/
/*  with, or included-in a form-specific program.			*/
/* 									*/
/* Copyright (C) 2003, 2004 - Aston Roberts				*/
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
/* Aston Roberts 1-1-2004	aston_roberts@yahoo.com			*/
/************************************************************************/

#include "dfp_datatypes.h"
#include "dfp_includes.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
//#include "decimal64.h"
#include "taxsolve_routines.h"
#include "operator.h"
#include "decAsm.h"

#if defined(NATIVE_GCC_BID)
extern _IDEC_flags flags;      // working flags 
#endif

//static context_t *setp = &set;

//FILE *infile=0;	 /* Main input file to be used for reading tax input data. */
//FILE *outfile=0; /* Main output file. */
//int verbose=0;	 /* Declare and set the "verbosity" flag. */

/********************************************************************************/
/* Input routines. 								*/
/********************************************************************************/


/*.......................................................................
  .     NEXT_WORD - accepts a line of text, and returns with the        .
  . next word in that text in the third parameter, the original line    .
  . is shortened from the beginning so that the word is removed.        .
  . If the line encountered is empty, then the word returned will be    .
  . empty.                                                              .
  . NEXTWORD can parse on an arbitrary number of delimiters, and it 	.
  . returns everthing that was cut away in the second parameter.	.
  . Parameters:								.
  .   line - input character string, on output shortened by word.	.
  .   pre_trash - white-space or delimiters skipped before word.	.
  .   word - output of this routine, single word, without delimiters.   .
  .   delim - list of delimiters, whitepace chars, etc..		.
  .......................................................................*/
void next_word( char *line, char *pre_trash, char *word, char *delim )
{
 int i=0, j=0, k=0, m=0, flag=1;

 /* Eat away preceding garbage */
 while ((line[i]!='\0') && (flag))
  {
   j = 0;
   while ((delim[j]!='\0') && (line[i]!=delim[j])) j = j + 1;
   if (line[i]==delim[j]) 
    { pre_trash[k++] = line[i++]; }
   else  flag = 0;
  }

 while ((line[i]!='\0') && (!flag))
  {
   word[m++] = line[i++];
   if (line[i]!='\0')
    {
     j = 0;
     while ((delim[j]!='\0') && (line[i]!=delim[j])) j = j + 1;
     if (line[i]==delim[j]) flag = 1;
    }
  }

 /* Shorten line. */
 j = 0;
 while (line[i]!='\0') { line[j++] = line[i++]; }
 /* Terminate the char-strings. */
 line[j] = '\0';
 pre_trash[k] = '\0';
 word[m] = '\0';
}


/************************************************************************/
/* get_date - Returns days from 1-1-1980, for use in capital gains 	*/
/* calculations to determine short/long type.				*/
/* Probably more accurate than needed.  Usually just need to know if 	*/
/* buy/sell dates differ by more or less than 1 year.			*/
/************************************************************************/
int get_date(char *word, char *emssg )	/* Returns days from 1-1-1980. */
{ /* For use in capital gains calculations to determine short/long type. */
 char word1[500], word2[500], owrd[1000];
 int month, day, year, days;

 /* Expect month-day-year, 3-3-01 */
 mystrcpy(owrd,word);

 next_word( word, word2, word1, "-_/ 	");
 if ((mysscanf(word1,"%d",&month)!=1) || (month>12)) {myprintf("ERROR: Bad month '%s' on '%s'\n",owrd,emssg); exit(1);}
 next_word( word, word2, word1, "-_/ 	");
 if ((mysscanf(word1,"%d",&day)!=1) || (day>31)) {myprintf("ERROR: Bad day '%s' on '%s'\n",owrd,emssg); exit(1);}
 next_word( word, word2, word1, "-_/ 	");
 if (mysscanf(word1,"%d",&year)!=1) {myprintf("ERROR: Bad year '%s' on '%s'\n",owrd,emssg); exit(1);}
 if (year>99) year = year - 1900;  /* Handle case where four-digit year was specified. */
 if (year<80) year = year + 100;   /* Y-2k, assume years less than 80 are in 2000's, not 1900's. */
 
 if ((year<85) || (year>115)) myprintf("Warning:  Unusual year in '%s' .  Use mm-dd-yy date like 5-23-02.\n", owrd);

 switch (month)
  {
   case 1: days = 0; break;
   case 2: days = 31; break;
   case 3: days = 59; break;
   case 4: days = 90; break;
   case 5: days = 120; break;
   case 6: days = 151; break;
   case 7: days = 181; break;
   case 8: days = 212; break;
   case 9: days = 243; break;
   case 10: days = 273; break;
   case 11: days = 304; break;
   case 12: days = 334; break;
   default: myprintf("ERROR: Bad month '%d'\n",month); exit(1); break;
  } 

 /* Assumes all years have 365-days. */
 days = days + day + 365 * (year - 80) - 1;
 return days;
}

void read_line( FILE *infile, char *line )
{
    int i=0;
    do  line[i++] = mygetc(infile);  while ((!myfeof(infile)) && (line[i-1] != '\n'));
    line[i-1] = '\0';
}


/* Show a line-number and it's value. */
void showline( int j )
{
    char word[2000];
#if defined(DECNUMBER_ARBITRARY)    
    {
 #if defined(WIDTH64)
        decimal64 myout;
        decimal64FromNumber(&myout, &L[j], &set);
        myfprintf(outfile, "(d) L%d = ", j);
        myfwrite(&myout, sizeof(decimal64), 1, outfile);
        myfprintf(outfile, "\n");
 #endif
 #if defined(WIDTH128)
        decimal128 myout;
        decimal128FromNumber(&myout, &L[j], &set);
        myfprintf(outfile, "(d) L%d = ", j);
        myfwrite(&myout, sizeof(decimal128), 1, outfile);
        myfprintf(outfile, "\n");
 #endif

    }
#else
    myfprintf(outfile, "(d) L%d = ", j);
    myfwrite(&L[j], sizeof(dec_t), 1, outfile);
    myfprintf(outfile, "\n");
#endif
    //myfprintf(outfile, "L%d = %6.2f\n", j, atof(TOSTRING64(L[j],word)));
}

/* Show an integer valued line. */
void shownum( int j )
{
    float temp = 0;
    char word[2000];
    //temp = myatof(TOSTRING64(L[j],word));
    TOSTRING64(L[j], word);
    temp = myatof(word);
    
    myfprintf(outfile, "(c) L%d = %d\n", j, (int)temp);
}

/* Show line only if non-zero. */
void ShowLineNonZero( int j )
{ if (ISZERO64(L[j])==0) showline( j ); }

/* Show-Line with a message. */
void showline_wmsg( int j, char *msg )
{
    char word[2000];
#if defined(DECNUMBER_ARBITRARY)    
    {
 #if defined(WIDTH64)
        decimal64 myout;
        decimal64FromNumber(&myout, &L[j], &set);
        myfprintf(outfile, "(m) L%d = ", j);
        myfwrite(&myout, sizeof(decimal64), 1, outfile);
        myfprintf(outfile, "		%s\n", msg);
 #endif
 #if defined(WIDTH128)
        decimal128 myout;
        decimal128FromNumber(&myout, &L[j], &set);
        myfprintf(outfile, "(m) L%d = ", j);
        myfwrite(&myout, sizeof(decimal128), 1, outfile);
        myfprintf(outfile, "		%s\n", msg);
 #endif
    }
#else
    myfprintf(outfile, "(m) L%d = ", j);
    myfwrite(&L[j], sizeof(dec_t), 1, outfile);
    myfprintf(outfile, "		%s\n", msg);
#endif
    //myfprintf(outfile,"L%d = %6.2f		%s\n", j, atof(TOSTRING64(L[j],word)), msg);
}

/* For worksheet calculations, indent and show special line character. */
void showline_wrksht( char wrksht, int j, float *x )
{ myfprintf(outfile," %c%d = %6.2f\n", wrksht, j, x[j]); }

void NotLessThanZero( dec_t * b, dec_t a )
{
    if (ISNEGATIVE64(a)) 
    {
     ZERO64((*b));
    }
    else {
     COPY64((*b), a);
    }
}

void Display_File( char *filename )
{
    FILE *infile;
    char line[500];

    infile = myfopen(filename,"r");
    if (infile==0) {myprintf("Could not open %s\n", filename); return;}
    myfgets(line, 500, infile);
    while (!myfeof(infile)) {
        myprintf("%s", line);
        myfgets(line, 500, infile);
    } 
    myfclose(infile);
}
