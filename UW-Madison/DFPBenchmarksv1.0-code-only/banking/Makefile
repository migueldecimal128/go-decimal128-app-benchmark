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
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c recycle.c lookupa.c hashtab.c deposit.c main.c basic_fcn.c rate.c mortgage.c credit_card.c decfcn.c
endif

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED)
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c  decDouble.c decQuad.c decSingle.c recycle.c lookupa.c hashtab.c deposit.c main.c basic_fcn.c rate.c mortgage.c credit_card.c decfcn.c
endif

ifeq ($(DEC_DATATYPE), BID)
SRCS = recycle.c lookupa.c hashtab.c deposit.c main.c basic_fcn.c rate.c mortgage.c credit_card.c decfcn.c
endif #BID

ifeq ($(DEC_DATATYPE), NATIVE_GCC_DPD)
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c  decDouble.c decQuad.c decSingle.c recycle.c lookupa.c hashtab.c deposit.c main.c basic_fcn.c rate.c mortgage.c credit_card.c decfcn.c
endif 

ifeq ($(DEC_DATATYPE), NATIVE_GCC_BID)
SRCS = recycle.c lookupa.c hashtab.c deposit.c main.c basic_fcn.c rate.c mortgage.c credit_card.c decfcn.c
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

else #Not Profile

ifeq ($(COUNTING), ON)
COUNT = -D COUNTING
else #COUNTING
COUNT = 
endif # COUNTING

SRCS = recycle.c lookupa.c hashtab.c deposit.c main.c basic_fcn.c rate.c mortgage.c credit_card.c decfcn.c
PG =
endif #end of PROFILE

OBJS = $(SRCS:.c=.o)
OPT = -O3
#OPT =
DEBUG = #-D DEBUG
SS_PATH = ../gcc-cross-compiler/sslittle-na-sstrix/include
UNIX  = $(WIDTH_DEF) -D UNIX -U LINUX -U SIMPLESCALAR
LINUX = $(WIDTH_DEF) $(DEC_DATATYPE_DEF) -D LINUX -U UNIX -U SIMPLESCALAR $(PROFILE_MACRO) $(COUNT)
SS    = $(WIDTH_DEF) -D SIMPLESCALAR -U LINUX -U UNIX

ifdef OS
ifeq ($(OS),LINUX)
CC = gcc
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
endif #SW
ifeq ($(OS),HW)
ISAFLAG = -D HW
SET = set
endif #HW
endif #!LINUX
endif #OS

TARG = Banking.$(OS).$(DEC_DATATYPE)
DIS = $(TARG).dis
CFLAG = -Wall $(PG) $(OPT) -static -D DEC64 $(ISAFLAG) $(CFLAG_SS)
LIBS =  $(DEC_LIBS) $(UNIX_COUNTER)
LFLAG = $(LIBFLAG) $(DEBUG) 

ifneq ($(SET),set)
all:
	@echo '[ERROR] WRONG SWITCH: OS=$(OS), try OS=LINUX [PROFILE=GPROF|VTUNE|NO], OS=HW or OS=SW'
else # OS is correct
ifeq ($(OS),LINUX)
all: $(TARG)
	@echo '[INFO] Compilation with OS=$(OS), DEC_DATATYPE=$(DEC_DATATYPE), and PROFILE=$(PROFILE) is done'
	@echo 'Remember to set LD_LIBRARY_PATH to the directory of decNumber library'
else #not LINUX
ifdef PROFILE
all:
	@echo '[ERROR] PROFILE switch cannot work with SW or HW switch!!'
else #no Profile
all: $(TARG)
	@echo '[INFO] Compilation with OS=$(OS), DEC_DATATYPE=$(DEC_DATATYPE), and PROFILE=$(PROFILE) is done'
	@echo 'Remember to set LD_LIBRARY_PATH to the directory of decNumber library'
endif #end of !PROFILE
endif #end of !LINUX
$(TARG): $(OBJS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS) $(PG) -lm
ifeq ($(OS),HW)
	@(cd $(SIMPLESCALAR); $(LN) -s ../myPISA/arith.h; $(LN) -s ../myPISA/arith.def; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD); cd $(PWD))
endif
ifeq ($(OS),SW)
	$(OBJDUMP) -x -d -l $(TARG) > $(DIS)
	@(perl ss_fcn_gen.pl $(DIS) arith.def arith.h)
	@(cd $(SIMPLESCALAR); $(LN) -s $(PWD)/arith.def; $(LN) -s $(PWD)/arith.h; $(MAKE) clean; $(MAKE); mv sim-outorder $(PWD); cd $(PWD))
endif #SW
endif #end of set

%.o: %.c
	$(CC) $(CFLAG) $(LFLAG) -c $< -o $@
clean:
	find . -name "*~" -exec rm -rf {} \;
	rm -f *.o *.s *~ core* \#*
	rm -f sim-outorder Banking.SW.* Banking.HW.* Banking.LINUX.*
