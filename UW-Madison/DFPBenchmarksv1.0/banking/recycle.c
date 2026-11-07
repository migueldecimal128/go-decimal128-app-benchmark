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
// Id: $Id: recycle.c,v 1.3 2009-01-06 18:39:12 mjanders Exp $
// Date: $Date: 2009-01-06 18:39:12 $
// Author: $Author: mjanders $
// =============================================================================

/*
--------------------------------------------------------------------
By Bob Jenkins, September 1996.  recycle.c
You may use this code in any way you wish, and it is free.  No warranty.

This manages memory for commonly-allocated structures.
It allocates RESTART to REMAX items at a time.
Timings have shown that, if malloc is used for every new structure,
  malloc will consume about 90% of the time in a program.  This
  module cuts down the number of mallocs by an order of magnitude.
This also decreases memory fragmentation, and freeing structures
  only requires freeing the root.
--------------------------------------------------------------------
*/

#include "dfp_includes.h"
#include "recycle.h"
#include "decfcn.h"
reroot *remkroot(size)
size_t  size;
{
   reroot *r = (reroot *)remalloc(sizeof(reroot), "recycle.c, root");
   r->list = (recycle *)0;
   r->trash = (recycle *)0;
   r->size = align(size);
   r->logsize = RESTART;
   r->numleft = 0;
   return r;
}

void  refree(r)
struct reroot *r;
{
   recycle *temp;
   temp = r->list;
   if (temp) while (r->list)
   {
      temp = r->list->next;
      myfree((char *)r->list);
      r->list = temp;
   }
   myfree((char *)r);
   return;
}

/* to be called from the macro renew only */
char  *renewx(r)
struct reroot *r;
{
   recycle *temp;
   if (r->trash)
   {  /* pull a node off the trash heap */
      temp = r->trash;
      r->trash = temp->next;
      (void)mymemset((void *)temp, 0, r->size);
   }
   else
   {  /* allocate a new block of nodes */
      r->numleft = r->size*((ub4)1<<r->logsize);
      if (r->numleft < REMAX) ++r->logsize;
      temp = (recycle *)remalloc(sizeof(recycle) + r->numleft, 
				 "recycle.c, data");
      temp->next = r->list;
      r->list = temp;
      r->numleft-=r->size;
      temp = (recycle *)((char *)(r->list+1)+r->numleft);
   }
   return (char *)temp;
}

char   *remalloc(len, purpose)
size_t  len;
char   *purpose;
{
  char *x = (char *)mymalloc(len);
  if (!x)
  {
    myfprintf(stderr, "malloc of %d failed for %s\n", 
	      len, purpose);
    myexit(SUCCESS);
  }
  return x;
}

