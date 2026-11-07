// =============================================================================
//  arith.h
//  header file for defining operations for simpleScalar
// =============================================================================
//  Original Author : Liang-Kai Wang, 5/2007
//  Maintainer:       Chuck Tsen, 2/2009
// =============================================================================
//  This file is the header for an interface with simplescalar (make OS=HW)
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
// Id: $Id: arith.h,v 1.2 2009-02-02 22:44:37 stsen Exp $
// Date: $Date: 2009-02-02 22:44:37 $
// Author: $Author: stsen $
// =============================================================================


enum arith_op{
DECCONTEXTDEFAULT,
DECCONTEXTSETSTATUS,
DECCONTEXTSETSTATUSFROMSTRING,
DECCONTEXTSTATUSTOSTRING,
DECIMAL64FROMNUMBER,
DECIMAL64FROMSTRING,
DECIMAL64TOENGSTRING,
DECIMAL64TONUMBER,
DECIMAL64TOSTRING,
DECNUMBERABS,
DECNUMBERADD,
DECNUMBERCOMPARE,
DECNUMBERCOPY,
DECNUMBERDIVIDE,
DECNUMBERDIVIDEINTEGER,
DECNUMBEREXP,
DECNUMBERFROMSTRING,
DECNUMBERLN,
DECNUMBERLOG10,
DECNUMBERMAX,
DECNUMBERMIN,
DECNUMBERMINUS,
DECNUMBERMULTIPLY,
DECNUMBERNORMALIZE,
DECNUMBERPLUS,
DECNUMBERPOWER,
DECNUMBERQUANTIZE,
DECNUMBERREMAINDER,
DECNUMBERREMAINDERNEAR,
DECNUMBERRESCALE,
DECNUMBERSAMEQUANTUM,
DECNUMBERSQUAREROOT,
DECNUMBERSUBTRACT,
DECNUMBERTOENGSTRING,
DECNUMBERTOINTEGRALVALUE,
DECNUMBERTOSTRING,
DECNUMBERTRIM,
DECNUMBERVERSION,
DECNUMBERZERO,
EXIT,
DECARI
};
#define MAXFUNCTIONLENGTH 45
struct operation {
  tick_t sim_cycle;
  tick_t hit; //how many hits so far
  tick_t sim_cycle_commit;  md_addr_t addr; //the location of the operation 
  md_addr_t return_PC; //the current return address
  tick_t sim_ic; //the accumulated instruction count
  char name[MAXFUNCTIONLENGTH];
};
void op_init();
void op_end();
int check_jal_commit(md_addr_t jmp_addr, md_addr_t ret_addr, tick_t cycle_in);
int check_jr_commit(sword_t r31);
void show_myOp();
