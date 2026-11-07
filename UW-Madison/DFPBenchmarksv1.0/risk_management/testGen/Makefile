#----------------------------------------
# Makefile
# The makefile for telco
# Decimal Floating-Point Benchmark
# Makefile to generate the code
# Developed by Liang-Kai Wang
# Department of Electrical and Computer Engineering
# University of Wisconsin-Madison
# $Revision: 1.2 $

ifndef WIDTH
WIDTH = 64
endif #width not defined

ifeq ($(WIDTH), 64)
WIDTH_DEF = -D WIDTH64 
endif

ifeq ($(WIDTH), 128)
WIDTH_DEF = -D WIDTH128
endif

ifndef DEC_DATATYPE 
DEC_DATATYPE = DECNUMBER_ARBITRARY
endif

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
DEC_DATATYPE_DEF = -D DECNUMBER_ARBITRARY
endif

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED) 
DEC_DATATYPE_DEF = -D DECNUMBER_FIXED  # decFloat library
endif 

ifeq ($(DEC_DATATYPE), BID) 
DEC_DATATYPE_DEF = -D BID -D DECIMAL_GLOBAL_EXCEPTION_FLAGS=1  	# intel library
endif # fixed

ifeq ($(DEC_DATATYPE), DEC128) 
DEC_DATATYPE_DEF = -D DEC128 # decFloat library
endif 

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
DEC_LIBS = ../LINUXDecNum.a
endif #DECNUMBER_ARBITRARY

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED) 
DEC_LIBS = ../LINUXDecFloats.a
endif #DECNUMBER_FIXED

ifeq ($(DEC_DATATYPE), DEC128) 
DEC_LIBS = ../LINUXDecFloats.a
endif #DECNUMBER_FIXED

ifeq ($(DEC_DATATYPE), BID)
DEC_LIBS = ../libbid_glbflags.a
endif #BID


TARG   = CONVERTER.$(DEC_DATATYPE)

all: asciiConverter

asciiConverter: asciiConverter.c
	gcc asciiConverter.c $(DEC_LIBS) $(WIDTH_DEF) $(DEC_DATATYPE_DEF) -o $(TARG)

clean:
	rm -f CONVERTER.* 

#################################################################
#  $Author: mjanders $ - The last CVS user to commit the file.
#  $Date: 2009-03-20 20:10:18 $ - The date of the last commit.
#  $Name:  $ - The tag name
#  $Log: Makefile,v $
#  Revision 1.2  2009-03-20 20:10:18  mjanders
#  Added 128-bit mode
#
#  Revision 1.1  2008-07-31 05:19:04  mjanders
#  Compiles ascii converter
#
#  Revision 1.2  2008/07/31 05:13:28  mjanders
#  Added BID functionality
#
#  Revision 1.1  2008/07/30 05:25:36  mjanders
#  Initial submission. Makefile for test generation.
#
#  Revision 1.2  2008/07/29 05:34:48  mjanders
#  Fixed make clean.
#
#  Revision 1.1  2008/07/29 05:34:03  mjanders
#  Makefile for test generation. Initial add.
#
#  Revision 1.1  2008/06/20 03:19:29  mjanders
#  Compiles sample test converter. (ascii => BID)
#
#  Revision 1.4  2008/06/10 15:15:21  mjanders
#  Added support for BID in Makefile
#
#  Revision 1.3  2008/05/03 20:32:48  mjanders
#  Removed outdated -I and -L options from Makefile
#
#  Revision 1.2  2008/04/25 01:18:36  mjanders
#  Fixed errors that arose from moving over to decFloats module.
#
#  Revision 1.1.1.1  2008/04/22 01:03:10  mjanders
#  tax preparation benchmark - initially working (small bug) with decDouble
#
#  Revision 1.6  2007/05/11 02:35:51  jhalani
#  Added NC state tax file updates
#
#  Revision 1.5  2007/05/04 16:32:54  lwang
#  new Makefile
#
#  Revision 1.4  2007/05/04 06:08:39  jhalani
#  Added CA state tax code and updated some bugs in the US tax software
#
#  Revision 1.3  2007/03/18 01:13:51  jhalani
#  Added test bench generator and did some minor changes to asciiConverter.c and taxsolve_US_1040_2006.c files
#
#  Revision 1.2  2007/03/15 22:08:20  lwang
#  check-in, LK
#
#  Revision 1.1.1.1  2007/03/13 03:55:38  lwang
#  initial check-in for Tax management
#
#################################################################
