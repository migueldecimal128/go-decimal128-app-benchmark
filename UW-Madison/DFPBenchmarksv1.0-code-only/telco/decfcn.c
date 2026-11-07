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

#include "decfcn.h"
#ifdef COUNTING
extern int mul_total, div_total, add_total, add_minmax_total, add_other_total;
extern int mul_rounded, div_rounded, add_rounded, add_other_rounded;
//the below one can run on fast pass adder
extern int add_no_exp_diff;
extern int add_minmax_no_exp_diff;
extern int add_other_no_exp_diff;
//this one may run on fast pass
extern int add_no_right_shifted_diff_exp;
extern int add_minmax_no_right_shifted_diff_exp;
extern int add_other_no_right_shifted_diff_exp;
//
extern int add_op_zero;

//the below can run on fast pass mul
extern int mul_lt_eq_16_coeff_on_result;
extern int sig_length_multiplier[16];

void decimal_printout(FILE *fd){
  fprintf(fd, "Rounded Result: Add = %d, Rescale/toInt = %d, Multiply = %d, Divide = %d\n",
          add_rounded, add_other_rounded, mul_rounded, div_rounded);
  fprintf(fd, "Total Add = %d, Total Min/max = %d, Total Rescale/toInt = %d, Multiply = %d, Divide = %d\n",
          add_total, add_minmax_total, add_other_total, mul_total, div_total);
  fprintf(fd, "Add/sub with no exponent difference = %d\n",
          add_no_exp_diff);
  fprintf(fd, "Add/sub with no right shift and different exponent = %d\n",
          add_no_right_shifted_diff_exp);
  fprintf(fd, "Add/sub with zero op = %d\n", add_op_zero);
  fprintf(fd, "Rescale/toInt with no exponent difference = %d\n",
          add_other_no_exp_diff);
  fprintf(fd, "Rescale/toInt with no right shift and different exponent = %d\n",
          add_other_no_right_shifted_diff_exp);
  fprintf(fd, "Minmax with no right shift and different exponent = %d\n", 
	  add_minmax_no_right_shifted_diff_exp);
  fprintf(fd, "Minmax with no exponent difference = %d\n",
          add_minmax_no_exp_diff);
  fprintf(fd, "MPY with the result less than or equal to 16 digits = %d\n"\
          , mul_lt_eq_16_coeff_on_result);
  fprintf(fd, "multiplier coefficient length:\n");
  fprintf(fd, "sig[1] = %d, sig[2] = %d, sig[3] = %d, sig[4] = %d\n",
          sig_length_multiplier[0], sig_length_multiplier[1],
          sig_length_multiplier[2], sig_length_multiplier[3]);
  fprintf(fd, "sig[5] = %d, sig[6] = %d, sig[7] = %d, sig[8] = %d\n",
          sig_length_multiplier[4], sig_length_multiplier[5],
          sig_length_multiplier[6], sig_length_multiplier[7]);
  fprintf(fd, "sig[9] = %d, sig[10] = %d, sig[11] = %d, sig[12] = %d\n",
          sig_length_multiplier[8], sig_length_multiplier[9],
          sig_length_multiplier[10], sig_length_multiplier[11]);
  fprintf(fd, "sig[13] = %d, sig[14] = %d, sig[15] = %d, sig[16] = %d\n",
          sig_length_multiplier[12], sig_length_multiplier[13],
          sig_length_multiplier[14], sig_length_multiplier[15]);
}
int extract_exp(int test, unsigned char *msd_a){
  int exp = 0;
  if (test!=0){
    exp = 512 * ((msd_a[1] & 0x10)!=0) +
      256 * ((msd_a[1] & 0x08)!=0);
  }
  else {
    exp = 512 * ((msd_a[1] & 0x40)!=0) +
      256 * ((msd_a[1] & 0x20)!=0);
  }
  //0x04 map to 0x0kkd_dmdd, where 'm' is for significand
  exp = exp +
    128 * ((msd_a[1] & 0x02)!=0) +
    64 * ((msd_a[1] & 0x01)!=0) +
    32 * ((msd_a[0] & 0x80)!=0) +
    16 * ((msd_a[0] & 0x40)!=0) +
    8 * ((msd_a[0] & 0x20)!=0) +
    4 * ((msd_a[0] & 0x10)!=0) +
    2 * ((msd_a[0] & 0x08)!=0) +
    ((msd_a[0] & 0x04)!=0);
  return exp;
}
int add_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn){
  unsigned char msd_a[2], msd_b[2];
  int test;
  int expA, expB;
  if (decNumberIsNaN(an) || decNumberIsNaN(bn) ||
      decNumberIsInfinite(an) || decNumberIsInfinite(bn)){
    return INVALID;
  }
  //
  memcpy(&msd_a[1], &(a->bytes[7]), sizeof(unsigned char));
  memcpy(&msd_a[0], &(a->bytes[6]), sizeof(unsigned char));
  memcpy(&msd_b[1], &(b->bytes[7]), sizeof(unsigned char));
  memcpy(&msd_b[0], &(b->bytes[6]), sizeof(unsigned char));
  msd_a[1] = msd_a[1] & 0x7f; //to get rid of the sign
  test = ((msd_a[1] & 0x40)!=0)&&((msd_a[1] & 0x20)!=0);
  expA = extract_exp(test, msd_a);

  test = ((msd_b[1] & 0x40)!=0)&&((msd_b[1] & 0x20)!=0);
  expB = extract_exp(test, msd_b);
  if (decNumberIsZero(an) || decNumberIsZero(bn))
    add_op_zero++;
  else {
    if (expA==expB)
      add_no_exp_diff++;
    else if (expA-expB>0){
      if ((DECIMAL64_Pmax-an->digits)>=expA-expB)
	add_no_right_shifted_diff_exp++;
    }
    else {
      if ((DECIMAL64_Pmax-bn->digits)>=expB-expA)
	add_no_right_shifted_diff_exp++;
    }
  }
  return 0;
}
int add_minmax_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn){
  unsigned char msd_a[2], msd_b[2];
  int test;
  int expA, expB;
  if (decNumberIsNaN(an) || decNumberIsNaN(bn) ||
      decNumberIsInfinite(an) || decNumberIsInfinite(bn)){
    return INVALID;
  }
  //
  memcpy(&msd_a[1], &(a->bytes[7]), sizeof(unsigned char));
  memcpy(&msd_a[0], &(a->bytes[6]), sizeof(unsigned char));
  memcpy(&msd_b[1], &(b->bytes[7]), sizeof(unsigned char));
  memcpy(&msd_b[0], &(b->bytes[6]), sizeof(unsigned char));
  msd_a[1] = msd_a[1] & 0x7f; //to get rid of the sign
  test = ((msd_a[1] & 0x40)!=0)&&((msd_a[1] & 0x20)!=0);
  expA = extract_exp(test, msd_a);
  test = ((msd_b[1] & 0x40)!=0)&&((msd_b[1] & 0x20)!=0);
  expB = extract_exp(test, msd_b);
  if (expA==expB)
    add_minmax_no_exp_diff++;
  else if (expA-expB>0){
    if ((DECIMAL64_Pmax-an->digits)>=expA-expB)
      add_minmax_no_right_shifted_diff_exp++;
  }
  else {
    if ((DECIMAL64_Pmax-bn->digits)>=expB-expA)
      add_minmax_no_right_shifted_diff_exp++;
  }
  return 0;
}
int add_rescale_check(decimal64 *a, decimal64 *b, decNumber *an, 
		      decNumber *bn){
  unsigned char msd_a[2], msd_b[2];
  int test;
  int expA, expB;
  if (decNumberIsNaN(an) || decNumberIsNaN(bn) ||
      decNumberIsInfinite(an) || decNumberIsInfinite(bn)){
    return INVALID;
  }
  //
  memcpy(&msd_a[1], &(a->bytes[7]), sizeof(unsigned char));
  memcpy(&msd_a[0], &(a->bytes[6]), sizeof(unsigned char));
  memcpy(&msd_b[1], &(b->bytes[7]), sizeof(unsigned char));
  memcpy(&msd_b[0], &(b->bytes[6]), sizeof(unsigned char));
  msd_a[1] = msd_a[1] & 0x7f; //to get rid of the sign
  test = ((msd_a[1] & 0x40)!=0)&&((msd_a[1] & 0x20)!=0);
  expA = extract_exp(test, msd_a);
  test = ((msd_b[1] & 0x40)!=0)&&((msd_b[1] & 0x20)!=0);
  expB = extract_exp(test, msd_b);
  if (expA==expB)
    add_other_no_exp_diff++;
  else if (expA-expB>0){
    if ((DECIMAL64_Pmax-an->digits)>=expA-expB)
      add_other_no_right_shifted_diff_exp++;
  }
  else {
    if ((DECIMAL64_Pmax-bn->digits)>=expB-expA)
      add_other_no_right_shifted_diff_exp++;
  }
  return 0;
}
int add_toint_check(decimal64 *a, decimal64 *b, decNumber *an, decNumber *bn){
  unsigned char msd_a[2], msd_b[2];
  int test;
  int expA, expB;
  if (decNumberIsNaN(an) || decNumberIsNaN(bn) ||
      decNumberIsInfinite(an) || decNumberIsInfinite(bn)){
    return INVALID;
  }
  //
  memcpy(&msd_a[1], &(a->bytes[7]), sizeof(unsigned char));
  memcpy(&msd_a[0], &(a->bytes[6]), sizeof(unsigned char));
  memcpy(&msd_b[1], &(b->bytes[7]), sizeof(unsigned char));
  memcpy(&msd_b[0], &(b->bytes[6]), sizeof(unsigned char));
  msd_a[1] = msd_a[1] & 0x7f; //to get rid of the sign
  test = ((msd_a[1] & 0x40)!=0)&&((msd_a[1] & 0x20)!=0);
  expA = extract_exp(test, msd_a);
  test = ((msd_b[1] & 0x40)!=0)&&((msd_b[1] & 0x20)!=0);
  expB = extract_exp(test, msd_b);
  if (expA==expB)
    add_other_no_exp_diff++;
  else if (expA-expB>0){
    if ((DECIMAL64_Pmax-an->digits)>=expA-expB)
      add_other_no_right_shifted_diff_exp++;
  }
  else {
    add_other_rounded++;
    if ((DECIMAL64_Pmax-bn->digits)>=expB-expA)
      add_other_no_right_shifted_diff_exp++;
  }
  return 0;
}
int mul_check(decNumber *an, decNumber *bn){
  if (decNumberIsNaN(an) || decNumberIsNaN(bn) ||
      decNumberIsInfinite(an) || decNumberIsInfinite(bn)){
    return INVALID;
  }
  if (an->digits + bn->digits<=16)
    mul_lt_eq_16_coeff_on_result++;
  sig_length_multiplier[bn->digits-1]++;
  return 0;
}
#endif
//end of COUNTING
#ifdef GPROF
int myprintf(const char *format, ...){
  int retval;
  va_list ap;
  va_start(ap, format);
  retval = vprintf(format, ap);
  va_end(ap);
  return retval;
}
int myfprintf(FILE *stream, const char *format, ...){
  int retval;
  va_list ap;
  va_start(ap, format);
  retval = vfprintf(stream, format, ap);
  va_end(ap);
  return retval;
}
int mysprintf(char *str, const char *format, ...){
  int retval;
  va_list ap;
  va_start(ap, format);
  retval = vsprintf(str, format, ap);
  va_end(ap);
  return retval;
}
#include <stdarg.h>
int mysscanf(const char *str, const char *format, ...){
  int retval;
  va_list ap;
  va_start(ap, format);
  retval = vsscanf(str, format, ap);
  va_end(ap);
  return retval;
}
FILE *myfopen(const char *path, const char *mode){
  return fopen(path, mode);
}
int myfclose(FILE *fp){
  return fclose(fp);
}
size_t myfread(void *ptr, size_t size, size_t nmemb, FILE *stream){
  return fread(ptr, size, nmemb, stream);
}
size_t myfwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
  return fwrite(ptr, size, nmemb, stream);
}
int myfeof(FILE *stream){
  return feof(stream);
}
char *mystrcpy(char *dest, const char *src){
  return strcpy(dest, src);
}
char *mystrcat(char *dest, const char *src){
  return strcat(dest, src);
}
int mystrcmp(const char *s1, const char *s2){
  return strcmp(s1, s2);
}
char *mystrstr(const char *haystack, const char *needle){
  return strstr(haystack, needle);
}
size_t mystrlen(const char *s){
  return strlen(s);
}
int myatoi(const char *nptr){
  return atoi(nptr);
}
time_t mytime(time_t *t){
  return time(t);
}
int mygetc(FILE *stream){
  return getc(stream);
}
double myatof(const char *nptr){
  return atof(nptr);
}
char *myfgets(char *s, int size, FILE *stream){
  return fgets(s, size, stream);
}
void *mycalloc(size_t nmemb, size_t size){
  return (void *)calloc(nmemb, size);
}
void myfree(void *ptr){
  free(ptr);
}
int myremove(const char *pathname){
  return remove(pathname);
}
void myexit(int status){
  exit(status);
}
int myferror(FILE *stream){
  return ferror(stream);
}
int myfflush(FILE *stream){
  return fflush(stream);
}
void *mymemcpy(void *dest, const void *src, size_t n){
  return memcpy(dest, src, n);
}
#endif
