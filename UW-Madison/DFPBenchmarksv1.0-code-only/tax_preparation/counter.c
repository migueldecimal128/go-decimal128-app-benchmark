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
// $Revision: 1.1.1.1 $

#include "counter.h"

#ifndef SIMPLESCALAR
#ifndef GPROF
#ifdef UNIX
int sparc_test_and_set(void){
  int cpuver;
  char* setting = NULL;
  cpc_event_t event;
  if (cpc_version(CPC_VER_CURRENT) != CPC_VER_CURRENT) {
    printf ("application:library cpc version mismatch!");
    return 1;
  }
  if ((cpuver = cpc_getcpuver()) == -1) {
    printf ("no performance counter hardware!\n");
    return 1;
  }
  if ((setting = getenv("PERFEVENTS")) == NULL ) {
    setting = "pic0=Cycle_cnt,pic1=Instr_cnt";
  }
  if (cpc_strtoevent(cpuver, setting, &event) != 0) {
    printf("can't measure '%s' on this processor", setting);
    return 1;
  }
  setting = cpc_eventtostr(&event);
  if (cpc_access() == -1) {
    printf("can't access perf counters: %s", strerror(errno));
    return 1;
  }
  if (cpc_bind_event(&event, 0) == -1) {
    printf("can't bind lwp%d: %s", _lwp_self(), strerror(errno));
    return 1;
  }
  return 0;
}
#endif //# UNIX

#ifdef LINUX

double showsDiff(unsigned starth, unsigned startl,
		 unsigned endh, unsigned endl){
  unsigned hi, lo, borrow;
  lo = endl - startl;
  borrow = lo > endl;
  hi = endh - starth - borrow;
  return (double) hi * (1<<30) * 4 + lo;
}
#endif //# LINUX
#endif //#!GPROF
#endif //!SIMPLESCALAR

/*
  $Author: mjanders $ - The last CVS user to commit the file.
  $Date: 2008-04-22 01:03:10 $ - The date of the last commit.
  $Name:  $ - The tag name
  $Log: counter.c,v $
  Revision 1.1.1.1  2008-04-22 01:03:10  mjanders
  tax preparation benchmark - initially working (small bug) with decDouble

  Revision 1.1  2007/03/15 22:08:20  lwang
  check-in, LK

  Revision 1.2  2006/11/22 01:40:31  lwang
  comment added

*/
