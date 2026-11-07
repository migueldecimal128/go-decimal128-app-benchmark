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

//----------------------------------------
// counter.h
// Currency Conversion Microbenchmark
// Decimal Floating-Point Benchmark
// This file is to provide the common operations used in the benchmark,
// Developed by Liang-Kai Wang
// Department of Electrical and Computer Engineering
// University of Wisconsin-Madison
// $Revision: 1.2 $

#ifndef COUNTER_H
#define COUNTER_H

#ifndef SIMPLESCALAR
#ifndef GPROF
#ifdef UNIX
#include "libcpc.h"
#include <errno.h>
#include <sys/lwp.h>
int sparc_test_and_set(void);
#endif //# UNIX

#ifdef LINUX
//#include <asm/timex.h>
//=r means the variables are in the register, =m means they are in memory
//=r takes less time since it is in register
#define ASM_MACRO(timel, timeh) \
        __asm__ __volatile__("rdtsc; mov %%eax, %0; mov %%edx, %1" \
                             : "=r" (timel), "=r" (timeh) \
                             : \
                             :"%eax", "%edx")
double showsDiff(unsigned starth, unsigned startl,
                 unsigned endh, unsigned endl);

#endif //# LINUX
#endif //#!GPROF
#endif //!SIMPLESCALAR

#endif
/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2008-04-25 01:18:36 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: counter.h,v $
  Revision 1.2  2008-04-25 01:18:36  mjanders
  Fixed errors that arose from moving over to decFloats module.

  Revision 1.1.1.1  2008/04/22 01:03:10  mjanders
  tax preparation benchmark - initially working (small bug) with decDouble

  Revision 1.1  2007/03/15 22:08:20  lwang
  check-in, LK

  Revision 1.2  2006/11/22 01:40:31  lwang
  comment added

*/
