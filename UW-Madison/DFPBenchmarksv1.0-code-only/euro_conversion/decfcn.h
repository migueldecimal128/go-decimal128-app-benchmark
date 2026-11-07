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

#ifndef DECFCN_H
#define DECFCN_H
#include <stdarg.h>
#include <stdio.h>
int decfprintf(FILE *stream, int argc, const char *format, ...);

#ifdef COUNTING
#include <stdio.h>
#include <string.h>
#define INVALID -1;
#include "decimal64.h"

void decimal_printout(FILE *fd);
int extract_exp(int test, unsigned char *msd_a);
int add_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn);
int minmax_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn);
int rescale_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn);
int toint_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn);
int mul_check(decNumber *an, decNumber *bn);
#endif

#ifdef GPROF
#include <stdarg.h>
#include <stdio.h>
int myprintf(const char *format, ...);
int myfprintf(FILE *stream, 
	      const char *format, ...);
FILE *myfopen(const char *path, const char *mode);
int myfclose(FILE *fp);
size_t myfread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t myfwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int myfeof(FILE *stream);
#else 
#define myprintf(format...) printf(format)
#define myfprintf(stream...) fprintf(stream)
#define myfopen(path...) fopen(path)
#define myfclose(fp) fclose(fp)
#define myfread(ptr...) fread(ptr)
#define myfeof(stream) feof(stream)
#endif

#endif
