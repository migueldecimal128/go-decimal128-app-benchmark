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
#ifndef TAXSOLVE_ROUTINES_H
#define TAXSOLVE_ROUTINES_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "decimal64.h"
#include "dfp_datatypes.h"
#include "operator.h"
#include "decfcn.h"

#define MAX_LINES 1000

dec64_t L[MAX_LINES];	/* Declare the Line entry variables. */

extern context_t set;
extern FILE *infile;	 /* Main input file to be used for reading tax input data. */
extern FILE *outfile; /* Main output file. */
extern int verbose;	 /* Declare and set the "verbosity" flag. */


/********************************************************************************/
/* Input routines. 								*/
/********************************************************************************/

void next_word( char *line, char *pre_trash, char *word, char *delim );

int get_date(char *word, char *emssg );	/* Returns days from 1-1-1980. */

void read_line( FILE *infile, char *line );

/* Show a line-number and it's value. */
void showline( int j );

/* Show an integer valued line. */
void shownum( int j );

/* Show line only if non-zero. */	/* Depricated in favor of ShowLineNonZero (clearer name). */
//void ShowLine( int j );

/* Show line only if non-zero. */
void ShowLineNonZero( int j );

/* Show-Line with a message. */
void showline_wmsg( int j, char *msg );

/* For worksheet calculations, indent and show special line character. */
void showline_wrksht( char wrksht, int j, float *x );

void NotLessThanZero( dec64_t * b, dec64_t a );
void Display_File( char *filename );

#endif
