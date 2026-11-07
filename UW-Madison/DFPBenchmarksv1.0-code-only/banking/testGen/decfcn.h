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
// =============================================================================
// VERSION AND CVS INFO
// Id: $Id: decfcn.h,v 1.1 2009-04-12 05:53:49 mjanders Exp $
// Date: $Date: 2009-04-12 05:53:49 $
// Author: $Author: mjanders $
// =============================================================================

#ifndef DECFCN_H
#define DECFCN_H
#ifdef COUNTING
#include <stdio.h>
#include <string.h>
#define INVALID -1;
//#include "decimal64.h"
//#include "decDouble.h"

void decimal_printout(FILE *fd);
int extract_exp(int test, unsigned char *msd_a);
int add_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn);
int add_minmax_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn);
int add_rescale_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn);
int add_toint_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn);
int mul_check(decNumber *an, decNumber *bn);
#endif
#ifdef GPROF
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int myprintf(const char *format, ...);
int myfprintf(FILE *stream, const char *format, ...);
int mysprintf(char *str, const char *format, ...);
int mysscanf(const char *str, const char *format, ...);
FILE *myfopen(const char *path, const char *mode);
int myfclose(FILE *fp);
size_t myfread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t myfwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int myfeof(FILE *stream);
char *mystrcpy(char *dest, const char *src);
char *mystrcat(char *dest, const char *src);
int mystrcmp(const char *s1, const char *s2);
char *mystrstr(const char *haystack, const char *needle);
size_t mystrlen(const char *s);
int myatoi(const char *nptr);
double myatof(const char *nptr);
time_t mytime(time_t *t);
int mygetc(FILE *stream);
char *myfgets(char *s, int size, FILE *stream);
void *mycalloc(size_t nmemb, size_t size);
void myfree(void *ptr);
int myremove(const char *pathname);
void myexit(int status);
int myferror(FILE *stream);
int myfflush(FILE *stream);
void *mymemset(void *s, int c, size_t n);
void *mymalloc(size_t size);
int mymemcmp(const void *s1, const void *s2, size_t n);
void *mymemcpy(void *dest, const void *src, size_t n);

#else
#define myprintf(format...) printf(format)
#define myfprintf(stream...) fprintf(stream)
#define mysprintf(str...) sprintf(str)
#define mysscanf(str...) sscanf(str)
#define myfopen(path, mode) fopen(path, mode)
#define myfclose(fp) fclose(fp)
#define myfread(ptr, size, nmemb, stream) fread(ptr, size, nmemb, stream)
#define myfwrite(ptr, size, nmemb, stream) fwrite(ptr, size, nmemb, stream)
#define myfeof(stream) feof(stream)
#define mystrcpy(dest, src) strcpy(dest, src)
#define mystrcat(dest, src) strcat(dest, src)
#define mystrcmp(s1, s2) strcmp(s1, s2)
#define mystrstr(haystack, needle) strstr(haystack, needle)
#define mystrlen(s) strlen(s)
#define myatoi(nptr) atoi(nptr)
#define myatof(nptr) atof(nptr)
#define mytime(t) time(t)
#define mygetc(stream) getc(stream)
#define myfgets(s, size, stream) fgets(s, size, stream)
#define mycalloc(nmemb, size) calloc(nmemb, size)
#define myfree(ptr) free(ptr)
#define myremove(pathname) remove(pathname)
#define myexit(status) exit(status)
#define myferror(stream) ferror(stream)
#define myfflush(stream) fflush(stream)
#define mymemset(s, c, n) memset(s, c, n)
#define mymalloc(size) malloc(size)
#define mymemcmp(s1, s2, n) memcmp(s1, s2, n)
#define mymemcpy(dest, src, n) memcpy(dest, src, n)
#endif
#endif
