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

/*                     US_1040_ss_calc.c                    */
/*                     Function version                     */
/*                                                          */
/*              Social Security Benefits Worksheet          */
/*                                                          */
/*             (Also Tier One Railroad Retirement)          */
/*                                                          */
/*                       12/21/2005                         */
/*                                                          */
/*  Array Numbers Follow IRS SS Worksheet Lines  1 - 18     */
/*     ssws[0] not used  --  ssws[1] = line 1  -  etc.      */
/*                                                          */
/*               E:\tax-proj\tax_project\f_sscalc.c   */
/*                1/27/2006 2:05:15 PM                */
/*    Form 1040 line numbers are  f1040_lines7, etc.       */
/*                                                          */

/*static float f_sscalc(int, float, float, float, float, float );   */

#include "US_1040_ss_calc.h"
/*   >>>>>    YOU MUST READ   US_1040_ss_calc.h    <<<<<<<  */
/*             for a working accurate program               */
//#include "dfp_includes.h"
#include  <stdio.h>
#include  <math.h>   

float f_sscalc( int file_status, float box5_1099_h, float box5_1099_w, float f1040_lines_7_21, float f1040_line_8b, float  f1040_lines_23_32 )
{

    float ssws[26];              /*   social security work sheet */
    float F1040_line_20b;
    int x;                   /* used in float compare compare functions (macros) to avoid SPLINT message.                 */


    /*       Initialize entries " Lines' array to zero           */

    {	
	int count = 0;
	while (count <  25 ) {
	    ssws[count] = 0.0000f;
	    ++count;
	}
    }

    /*          End of Initialization Section.                   */


    /*      Taxable Social Security Calculation Section         */

    ssws[1]  =  box5_1099_h + box5_1099_w;  /* also 1040 line 20a */
    ssws[2]  =  ssws[1] * 0.5000f;
    ssws[3]  =  f1040_lines_7_21;
    ssws[4]  =  f1040_line_8b;
    ssws[5]  =  ( ssws[2] + ssws[3] + ssws[4] );
    ssws[6]  =  f1040_lines_23_32; 

    /*   Line 7:                                   */
 
    ssws[7] = 0.00f;	          /* for splint only */
    x = isless( ssws[6], ssws[5] );
    if  (x == TRUE)
  	{
	    ssws[7]  = (ssws[5] - ssws[6] );
	} else {
	    ssws[19] = 0.00f;
	    F1040_line_20b  = 0;
	    goto done;
	}

    /*     Line 8                                            */

    ssws[8] = 0.00f;	          /* for splint only */
    if ( (file_status  ==  1) || (file_status  == 3) || (file_status  == 4) || (file_status  == 5) )  
	{ 
	    ssws[8] = 25000.00f; 
	}
    if (file_status  == 2  )
	{
	    ssws[8] = 32000.00f;
	}

    /*   Line 9:                                   */
    x = 9;
    x = isless(  ssws[8], ssws[7] );
    if ( x == TRUE ) {
	(ssws[9] = ( ssws[7] - ssws[8] ) );
    }  else {
	F1040_line_20b  = 0;
	goto done;
    }		

    /*     Line 10                                            */

    ssws[10] = 0.00f;	        /* for splint only */
    if ( (file_status  ==  1 ) || (file_status  ==  3) || (file_status  ==  4) || (file_status  ==  5) )  
	{ 
	    ssws[10] = 9000.00f; 
	}
    if (file_status  == 2 )
	{
	    ssws[10] = 12000.00f;
	}

    /*   Line 11:                                   */
    x = 0;
    x = isgreater(  ssws[10],  ssws[9]  );
    if  ( x == TRUE ) {
	ssws[11] = 0;
    } else {
	(ssws[11] =  ( ssws[9] - ssws[10] ) ); 
    }

    x = 0;
    x = islessequal(  ssws[11], 0.00 ); 
    if ( x == TRUE ) { ssws[11] = 0.00f; }

    /*   Line 12:                                   */

    ssws[12] = 0.00f;	          /* for splint only */
    x = isless( ssws[9], ssws[10]  );
    if ( x == TRUE ) {
	(ssws[12] =  ssws[9] );
    } else 

	x = 0;
    x = isless( ssws[10], ssws[9] );
    if ( x == TRUE ) {
	(ssws[12] =  ssws[10] );
    }
    if ( ssws[12] <= 00.0 ) { ( ssws[12] = 0.00f ); }

    ssws[13] = ( ssws[12] * 0.50f );

    /*   Line 14:                                   */

    ssws[14] = 0.00f;	          /* for splint only */
    x = 0;
    x = isless(  ssws[2], ssws[13] );	
    if ( x == TRUE ) {
	(ssws[14] =  ssws[2] );
    }
 
    x = isless( ssws[13], ssws[2]  );	
    x = 0;
    if ( x == TRUE ) {
	(ssws[14] =  ssws[13] );
    }
    if ( ssws[14] <= 0 ) { ( ssws[14] = 0.00f ); }

    ssws[15] =  ( ssws[11]  * 0.8500f  );
    ssws[15] =  ( ssws[15]  + 0.50f  );

    ssws[16] =  ( ssws[14]  + ssws[15] );

    ssws[17] =  ( ssws[1]   * 0.8500f  );
    ssws[17] =  ( ssws[17]  + 0.50f  );

    /*   Line 18:                                   */

    ssws[18] = 0.00f;	          /* for splint only */
    x = 0;
    x = isless(  ssws[16], ssws[17] );	
    if ( x == TRUE ) {
	ssws[18] =  ssws[16];
    }


    x = 0;
    x = isless(  ssws[17], ssws[16] );
    if ( x == TRUE ) {
	ssws[18] =  ssws[17];
    }
    if ( ssws[18] <= 0 ) {
 	( ssws[18] = 0.00f );
    }

    F1040_line_20b = ssws[18];

 done:

    /*              Debug Section                             */

#ifdef DEBUG
    printf ("\n\n>>> From Social Secutity Calc Function:  \n\n");
    {
	int count = 0;
	printf ("\n\n");
	while  ( count < 26 ) {
	    printf ("  Line %2d = %9.2f  \n", count, ssws[count] );
	    ++count;
	}
	printf ("\n\n");

	printf  ("\n\n  function FLine 20b Taxable SS Benefits $ %8.0f.00   \n\n", F1040_line_20b);
    }
#endif    /*  debug  */

    return (F1040_line_20b);

}

/*                      End Of Function                     */ 
/*  *****************************************************   */
