#----------------------------------------
# Makefile
# The makefile for banking system
# Decimal Floating-Point Benchmark
# Makefile to generate the code
# Developed by Liang-Kai Wang
# Department of Electrical and Computer Engineering
# University of Wisconsin-Madison
# $Revision: 1.9 $

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
DEC_DATATYPE_DEF = -D NATIVE_GCC_DPD # Native gcc DPD 
endif # Native gcc DPD 

ifeq ($(DEC_DATATYPE), NATIVE_GCC_BID) 
DEC_DATATYPE_DEF = -D NATIVE_GCC_BID -D DECIMAL_GLOBAL_EXCEPTION_FLAGS=0 -fno-strict-aliasing  # Native gcc BID 
endif

## This is for profiling including gprof and vtune
ifdef PROFILE

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
 SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c taxsolve_routines.c taxsolve_US_1040_2006.c counter.c decfcn.c
 CA_SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c taxsolve_routines.c taxsolve_CA_540_2006.c counter.c decfcn.c
 NC_SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c taxsolve_routines.c taxsolve_NC_D400_2006.c counter.c decfcn.c
endif

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED)
 SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c  decDouble.c decQuad.c decSingle.c taxsolve_routines.c taxsolve_US_1040_2006.c counter.c decfcn.c
 CA_SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decDouble.c decNumber.c decPacked.c taxsolve_routines.c taxsolve_CA_540_2006.c counter.c decfcn.c
 NC_SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decDouble.c decNumber.c decPacked.c taxsolve_routines.c taxsolve_NC_D400_2006.c counter.c decfcn.c
endif 

ifeq ($(DEC_DATATYPE), BID)
 SRCS = taxsolve_routines.c taxsolve_US_1040_2006.c decfcn.c
 CA_SRCS = taxsolve_routines.c taxsolve_CA_540_2006.c counter.c decfcn.c
 NC_SRCS = taxsolve_routines.c taxsolve_NC_D400_2006.c counter.c decfcn.c
endif #BID

ifeq ($(DEC_DATATYPE), NATIVE_GCC_DPD)
 SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c  decDouble.c decQuad.c decSingle.c taxsolve_routines.c taxsolve_US_1040_2006.c counter.c decfcn.c
 CA_SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c taxsolve_routines.c taxsolve_CA_540_2006.c counter.c decfcn.c
 NC_SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c taxsolve_routines.c taxsolve_NC_D400_2006.c counter.c decfcn.c
endif 

ifeq ($(DEC_DATATYPE), NATIVE_GCC_BID)
 SRCS = taxsolve_routines.c taxsolve_US_1040_2006.c decfcn.c
 CA_SRCS = taxsolve_routines.c taxsolve_CA_540_2006.c counter.c decfcn.c
 NC_SRCS = taxsolve_routines.c taxsolve_NC_D400_2006.c counter.c decfcn.c
endif #BID

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
else    #Not profile

ifeq ($(COUNTING), ON)
COUNT = -D COUNTING
else #COUNTING
COUNT = 
endif # COUNTING

SRCS = taxsolve_routines.c taxsolve_US_1040_2006.c counter.c decfcn.c
CA_SRCS = taxsolve_routines.c taxsolve_CA_540_2006.c counter.c decfcn.c
NC_SRCS = taxsolve_routines.c taxsolve_NC_D400_2006.c counter.c decfcn.c
PG =
GPROF_MACRO =
endif #end of GPROF
OBJS = $(SRCS:.c=.o)
CA_OBJS = $(CA_SRCS:.c=.o)
NC_OBJS = $(NC_SRCS:.c=.o)
OPT = -O3
DEBUG = #-D DEBUG
SS_PATH = ../gcc-cross-compiler/sslittle-na-sstrix/include
UNIX  = -D UNIX -U LINUX -U SIMPLESCALAR
LINUX = $(WIDTH_DEF) $(DEC_DATATYPE_DEF) -D LINUX -U UNIX -U SIMPLESCALAR $(PROFILE_MACRO) $(COUNT)
SS    = -D SIMPLESCALAR -U LINUX -U UNIX

ifdef OS
ifeq ($(OS),LINUX)
CC   = gcc
ifndef PROFILE

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
DEC_LIBS = LINUXDecNum.a
endif #DECNUMBER_ARBITRARY

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED) 
DEC_LIBS = LINUXDecFloats.a
endif #DECNUMBER_FIXED

ifeq ($(DEC_DATATYPE), BID)
DEC_LIBS = libbid_glbflags.a
endif #BID

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
endif # end of SW
ifeq ($(OS),HW)
ISAFLAG = -D HW
SET = set
endif #HW
endif #!LINUX
endif #OS

TARG   = Tax.FED.$(OS).$(DEC_DATATYPE)
CA_TARG = Tax.CA.$(OS).$(DEC_DATATYPE)
NC_TARG = Tax.NC.$(OS).$(DEC_DATATYPE)
DIS = $(TARG).dis
CA_DIS = $(CA_TARG).dis
NC_DIS = $(NC_TARG).dis
#CFLAG = -Wall $(PG) $(OPT) -static -D DEC64 $(ISAFLAG) -I $(DECNUMPATH) $(CFLAG_SS)
CFLAG = -Wall $(PG) $(OPT) -static -D DEC64 $(ISAFLAG) $(CFLAG_SS)

LIBS = $(DEC_LIBS)
LFLAG = $(LIBFLAG) $(DEBUG) 

ifneq ($(SET),set)
all:
	@echo '[ERROR] WRONG SWITCH: OS=$(OS), try OS=LINUX [PROFILE=GPROF|VTUNE|NO], OS=HW or OS=SW'
else 
ifeq ($(OS),LINUX)
all: $(TARG) $(CA_TARG) $(NC_TARG)
	@echo '[INFO] Compilation with OS=$(OS), DEC_DATATYPE=$(DEC_DATATYPE), and PROFILE=$(PROFILE) is done'
	@echo 'Remember to set LD_LIBRARY_PATH to the directory of decNumber library'
else
ifdef PROFILE
all:
	@echo '[ERROR] GPROF switch cannot work with SW or HW switch!!'
else
all: $(TARG) $(CA_TARG) $(NC_TARG)
	@echo '[INFO] Compilation with $(OS) and DEC_DATATYPE=$(DEC_DATATYPE) is done'
	@echo '       sim-outorder-FED, sim-outorder-CA, and sim-outorder-NC are generated'
	@echo '######## Remember to set LD_LIBRARY_PATH to the directory of decNumber library ####'
endif #end of PROFILE
endif #end of !LINUX
$(TARG): $(OBJS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS) $(PG)
ifeq ($(OS),HW)
	@(cd $(SIMPLESCALAR); $(LN) -s ../myPISA/arith.h; $(LN) -s ../myPISA/arith.def; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD)/sim-outorder-HW-FED; cd $(PWD))
endif
ifeq ($(OS),SW)
	$(OBJDUMP) -x -d -l $(TARG) > $(DIS)
	perl ss_fcn_gen.pl $(DIS) arith.def arith.h
	@(cd $(SIMPLESCALAR); $(LN) -s $(PWD)/arith.def; $(LN) -s $(PWD)/arith.h; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD)/sim-outorder-SW-FED; cd $(PWD))
endif
$(CA_TARG): $(CA_OBJS)
	$(CC) -o $(CA_TARG) $(CA_OBJS) $(LIBS) $(PG)
ifeq ($(OS),HW)
	@(cd $(SIMPLESCALAR); $(LN) -s ../myPISA/arith.h; $(LN) -s ../myPISA/arith.def; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD)/sim-outorder-HW-CA; cd $(PWD))
endif
ifeq ($(OS),SW)
	$(OBJDUMP) -x -d -l $(CA_TARG) > $(CA_DIS)
	perl ss_fcn_gen.pl $(CA_DIS) arith.def arith.h
	@(cd $(SIMPLESCALAR); $(LN) -s $(PWD)/arith.def; $(LN) -s $(PWD)/arith.h; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD)/sim-outorder-SW-CA; cd $(PWD))
endif
$(NC_TARG): $(NC_OBJS)
	$(CC) -o $(NC_TARG) $(NC_OBJS) $(LIBS) $(PG)
ifeq ($(OS),HW)
	@(cd $(SIMPLESCALAR); $(LN) -s ../myPISA/arith.h; $(LN) -s ../myPISA/arith.def; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD)/sim-outorder-HW-NC; cd $(PWD))
endif
ifeq ($(OS),SW)
	$(OBJDUMP) -x -d -l $(NC_TARG) > $(NC_DIS)
	@(perl ss_fcn_gen.pl $(NC_DIS) arith.def arith.h)
	@(cd $(SIMPLESCALAR); $(LN) -s $(PWD)/arith.def; $(LN) -s $(PWD)/arith.h; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD)/sim-outorder-SW-NC; cd $(PWD))
endif
endif #SET=set
%.o: %.c
	$(CC) $(CFLAG) $(LFLAG) -c $< -o $@
clean:
	find . -name "*~" -exec rm -rf {} \;
	rm -f dataset/result/*log_*
	rm -f *.o *.s *~ core* \#*
	rm -f Tax.FED.SW.* Tax.FED.LINUX.* Tax.FED.HW.* sim-outorder*
	rm -f Tax.CA.SW.* Tax.CA.LINUX.* Tax.CA.HW.* 
	rm -f Tax.NC.SW.* Tax.NC.LINUX.* Tax.NC.HW.* 
#################################################################
#  $Author: mjanders $ - The last CVS user to commit the file.
#  $Date: 2009-04-21 06:09:37 $ - The date of the last commit.
#  $Name:  $ - The tag name
#  $Log: Makefile,v $
#  Revision 1.9  2009-04-21 06:09:37  mjanders
#  Running simulations. Updating libraries. Compiling without flags
#
#  Revision 1.8  2009-04-16 07:01:13  mjanders
#  Small fixes made during simulations (actually pre-simulation verification)
#
#  Revision 1.7  2009-03-22 22:36:04  mjanders
#  Added 128 bit mode and fixed divide-by-zero problem in NC
#
#  Revision 1.6  2009-01-08 21:07:35  mjanders
#  Comment DEBUG by default
#
#  Revision 1.5  2008/11/19 06:04:50  mjanders
#  Cleaned this up and added native types
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
