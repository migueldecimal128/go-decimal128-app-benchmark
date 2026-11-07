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
endif
ifeq ($(WIDTH), 64)
 WIDTH_DEF = -D WIDTH64
endif
ifeq ($(WIDTH), 128)
 WIDTH_DEF = -D WIDTH128
endif

ifndef DEC_DATATYPE 
DEC_DATATYPE = DECNUMBER_ARBITRARY
endif # not defined
ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
 DEC_DATATYPE_DEF = -D DECNUMBER_ARBITRARY
else 
 ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED) 
  DEC_DATATYPE_DEF = -D DECNUMBER_FIXED  # decFloat library
 else 
  DEC_DATATYPE_DEF = -D BID -D DECIMAL_GLOBAL_EXCEPTION_FLAGS=1 	# intel library
 endif # fixed
endif # arbitrary

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
 DEC_LIBS = ../LINUXDecNum.a
else
 ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED) 
  DEC_LIBS = ../LINUXDecFloats.a
 else 
  # BID
  DEC_LIBS = ../libbid_glbflags.a
 endif # FIXED 
endif # ARBITRARY 


TARG   = CONVERTER.$(DEC_DATATYPE)
TARG_ASC = TEST_GEN_ASCII

all: asciiConverter asciiGenerator

asciiGenerator: testGen.c
	gcc testGen.c -o $(TARG_ASC)

asciiConverter: asciiConverter.c
	gcc asciiConverter.c $(DEC_LIBS) $(WIDTH_DEF) $(DEC_DATATYPE_DEF) -o $(TARG)

clean:
	rm -f CONVERTER.* 
	rm -f TEST_GEN_ASCII

#################################################################
#  $Author: mjanders $ - The last CVS user to commit the file.
#  $Date: 2009-03-16 01:11:14 $ - The date of the last commit.
#  $Name:  $ - The tag name
#  $Log: Makefile,v $
#  Revision 1.2  2009-03-16 01:11:14  mjanders
#  Added 128 bit option
#
#  Revision 1.1  2008-07-31 02:09:26  mjanders
#  Makes test generation code
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
