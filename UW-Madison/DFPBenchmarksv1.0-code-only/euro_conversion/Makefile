#----------------------------------------
# Makefile
# The makefile for banking system
# Decimal Floating-Point Benchmark
# Makefile to generate the code
# Developed by Liang-Kai Wang
# Department of Electrical and Computer Engineering
# University of Wisconsin-Madison
# $Revision: 1.16 $

# Record the current directory
PWD = $(shell pwd)
# SIMPLESCALAR directory
SIMPLESCALAR = ../simplescalar_toolset/simplesim-3.0/
LN = ln
MAKE = make

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
endif # not defined

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
DEC_DATATYPE_DEF = -D DECNUMBER_ARBITRARY
endif

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED) 
DEC_DATATYPE_DEF = -D DECNUMBER_FIXED  # decFloat library
endif 

ifeq ($(DEC_DATATYPE), BID) 
DEC_DATATYPE_DEF = -D BID -D DECIMAL_GLOBAL_EXCEPTION_FLAGS=1  	# intel library
endif # fixed

ifeq ($(DEC_DATATYPE), NATIVE_GCC_DPD) 
DEC_DATATYPE_DEF = -D NATIVE_GCC_DPD 		# Native gcc DPD 
endif # Native gcc DPD 

ifeq ($(DEC_DATATYPE), NATIVE_GCC_BID) 
DEC_DATATYPE_DEF = -D NATIVE_GCC_BID -D DECIMAL_GLOBAL_EXCEPTION_FLAGS=0 -fno-strict-aliasing  # Native gcc BID 
endif # Native gcc BID

ifdef PROFILE

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c main.c decfcn.c
endif

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED)
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c decDouble.c decQuad.c main.c decfcn.c
endif

ifeq ($(DEC_DATATYPE), BID)
SRCS = main.c decfcn.c
endif #BID

ifeq ($(DEC_DATATYPE), NATIVE_GCC_DPD)
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c decDouble.c decQuad.c main.c decfcn.c
endif 

ifeq ($(DEC_DATATYPE), NATIVE_GCC_BID)
SRCS = main.c decfcn.c
endif 

ifeq ($(PROFILE), GPROF)
PG = -pg 
PROFILE_MACRO = -D GPROF
else
ifeq ($(PROFILE), VTUNE)
PG = -g
PROFILE_MACRO =
else
PG = -g
PROFILE_MACRO =
endif #end of !vtune
endif #end of !gprof

else	#Not profile

ifeq ($(COUNTING), ON)
COUNT = -D COUNTING
else #COUNTING
COUNT = 
endif # COUNTING

SRCS = main.c decfcn.c
PROFILE_MACRO = 
PG = #-pg
endif	#end of GPROF

OBJS = $(SRCS:.c=.o)
OPT = -O3
SS_PATH = ../gcc-cross-compiler/sslittle-na-sstrix/include
UNIX  = $(WIDTH_DEF) $(DEC_DATATYPE_DEF) -D UNIX -U LINUX -U SIMPLESCALAR
LINUX = $(WIDTH_DEF) $(DEC_DATATYPE_DEF) -D LINUX -U UNIX -U SIMPLESCALAR $(PROFILE_MACRO) $(COUNT)
SS    = $(WIDTH_DEF) $(DEC_DATATYPE_DEF) -D SIMPLESCALAR -U LINUX -U UNIX

ifdef OS
ifeq ($(OS),LINUX)
CC   = gcc
ifndef PROFILE

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
DEC_LIBS = LINUXDecNum.a
endif

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED) 
DEC_LIBS = LINUXDecFloats.a
endif

ifeq ($(DEC_DATATYPE), BID)
DEC_LIBS = libbid_glbflags.a
endif # BID 

ifeq ($(DEC_DATATYPE), NATIVE_GCC_DPD) 
DEC_LIBS = LINUXDecFloats.a
endif #NATIVE_GCC_DPD

ifeq ($(DEC_DATATYPE), NATIVE_GCC_BID) 
DEC_LIBS = libbid_noglbflags.a
endif #NATIVE_GCC_BID

endif # PROFILE

LIBFLAG = $(LINUX)
SET  = set
else #!LINUX
CC   = ../gcc-cross-compiler/bin/sslittle-na-sstrix-gcc
CFLAG_SS = -I $(SS_PATH)
LIBFLAG = $(SS) 
DEC_LIBS = -lSSDecNum
SET = 0
ifeq ($(OS),SW)
OBJDUMP = ../gcc-cross-compiler/bin/sslittle-na-sstrix-objdump
SET = set
endif
ifeq ($(OS),HW)
ISAFLAG = -D HW
SET = set
endif #"$(OS)" "HW"
endif #OS= !LINUX
endif #ifdef(OS)

TARG   = Euro.$(OS).$(DEC_DATATYPE)
DIS = $(TARG).dis
CFLAG = -Wall $(PG) $(OPT) -static $(ISAFLAG) $(CFLAG_SS)
LIBS = $(DEC_LIBS)
DEBUG = #-D DEBUG -pg
LFLAG = $(LIBFLAG) $(DEBUG) 

ifneq ($(SET),set)
all:
	@echo '[ERROR] WRONG SWITCH: OS=$(OS), try OS=LINUX [PROFILE=GPROF|VTUNE|NO], OS=HW or OS=SW'
else #OS is correct
ifeq ($(OS),LINUX)
all: $(TARG)
	@echo '[INFO] Compilation with OS=$(OS), DEC_DATATYPE=$(DEC_DATATYPE), and PROFILE=$(PROFILE) is done'
	@echo 'Remember to set LD_LIBRARY_PATH to the directory of decNumber library'
else #not LINUX
ifdef PROFILE
all:
	@echo '[ERROR] PROFILE switch cannot work with SW or HW switch!!'
else
all: $(TARG)
	@echo '[INFO] Compilation with $(OS) and DEC_DATATYPE=$(DEC_DATATYPE) is done'
	@echo '######## Remember to set LD_LIBRARY_PATH to the directory of decNumber library ####'
endif #end of !PROFILE
endif #end of !LINUX
$(TARG): $(OBJS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS) $(PG)
ifeq ($(OS),HW)
	@(cd $(SIMPLESCALAR); $(LN) -s ../myPISA/arith.h; $(LN) -s ../myPISA/arith.def; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD); cd $(PWD))
endif
ifeq ($(OS),SW)
	$(OBJDUMP) -x -d -l $(TARG) > $(DIS)
	@(perl ss_fcn_gen.pl $(DIS) arith.def arith.h)
	@(cd $(SIMPLESCALAR); $(LN) -s $(PWD)/arith.def; $(LN) -s $(PWD)/arith.h; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD); cd $(PWD))
endif

endif #end of set
%.o: %.c
	$(CC) $(CFLAG) $(LFLAG) -c $< -o $@
clean:
	find . -name "*~" -exec rm -rf {} \;
	rm -f *.o *.s *~ core* \#*
	rm -f sim-outorder Euro.SW.* Euro.HW.* Euro.LINUX.*
#################################################################
#  $Author: mjanders $ - The last CVS user to commit the file.
#  $Date: 2009-04-21 06:09:27 $ - The date of the last commit.
#  $Name:  $ - The tag name
#  $Log: Makefile,v $
#  Revision 1.16  2009-04-21 06:09:27  mjanders
#  Running simulations. Updating libraries. Compiling without flags
#
#  Revision 1.15  2009-04-19 21:28:44  mjanders
#  Updated decnumber and Intel libraries
#
#  Revision 1.14  2009-04-16 04:39:22  mjanders
#  Small changes made during simulations
#
#  Revision 1.13  2009-04-14 19:34:04  mjanders
#  O3
#
#  Revision 1.12  2009-03-16 01:13:09  mjanders
#  Added 128-bit modes
#
#  Revision 1.11  2009-01-08 21:07:35  mjanders
#  Comment DEBUG by default
#
#  Revision 1.10  2008/11/19 06:15:33  mjanders
#  Fixed issue with DECIMAL_GLOBAL_EXCEPTION_FLAGS
#
#  Revision 1.9  2008/09/24 07:32:03  mjanders
#  Added NATIVE_GCC_BID support
#
#  Revision 1.8  2008/07/23 04:43:46  mjanders
#  *** empty log message ***
#
#  Revision 1.7  2008/05/19 21:41:27  mjanders
#  Updated Makefile so that test_gen.c can be compiled to generate both
#  BID and decimal64 data files
#
#  Revision 1.6  2008/05/19 20:00:22  mjanders
#  Added BID library functionality
#
#  Revision 1.5  2008/05/03 20:30:54  mjanders
#  Removed outdated -I and -L options from Makefile
#
#  Revision 1.4  2008/04/25 17:34:31  stsen
#  turned off DEBUG
#
#  Revision 1.3  2008/04/24 03:05:44  stsen
#  added switch for COUNTING=[ON|OFF]
#
#  Revision 1.2  2008/04/23 17:04:50  stsen
#  added functionality for DECNUMBER_TYPE=DECNUMBER_[FIXED|ARBITRARY]
#
#  Revision 1.1.1.1  2008/04/15 23:33:39  stsen
#  initial benchmark structure
#
#
#  Revision 1.4  2007/07/29 17:06:11  lwang
#  update version with decDouble support
#
#  Revision 1.3  2007/07/28 22:50:56  lwang
#  workable version
#
#  Revision 1.2  2007/03/20 17:40:04  lwang
#  test check-in
#
#  Revision 1.1.1.1  2007/03/13 03:49:13  lwang
#  Euro currency conversion, simplified version
#
#  Revision 1.8  2006/11/21 03:02:43  lwang
#  Major Revision with new technique to access database
#
#  Revision 1.7  2006/11/19 23:23:09  lwang
#  First draft of Banking System is done
#
#  Revision 1.6  2006/11/17 02:20:00  lwang
#  Credit Card benchmark added
#
#  Revision 1.5  2006/11/07 08:26:02  lwang
#  After spliting CD and Checking database
#
#  Revision 1.4  2006/11/07 05:20:10  lwang
#  Name of the functions changed. Before spliting CD and CK
#
#  Revision 1.3  2006/11/03 02:20:04  lwang
#  major revision
#
#  Revision 1.2  2006/10/31 11:45:22  lwang
#  minor update
#
#  Revision 1.1  2006/10/28 01:54:18  lwang
#  Banking System Initial Check-in
#
#  Revision 1.3  2006/10/19 02:33:25  lwang
#  extended with currency conversion, and checking/CD acct
#
#  Revision 1.2  2006/10/12 05:51:46  lwang
#  new version, bug free
#
#  Revision 1.1  2006/09/15 12:52:29  lwang
#  initial check-in for the CD microbenchmark
#
#  Revision 1.1  2006/07/31 06:46:22  lwang
#  initial check-in for the currency conversion benchmark.
#
#################################################################
