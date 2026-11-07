#----------------------------------------
# Makefile
# The makefile for banking system
# Decimal Floating-Point Benchmark
# Makefile to generate the code
# Developed by Liang-Kai Wang
# Department of Electrical and Computer Engineering
# University of Wisconsin-Madison
# $Revision: 1.8 $

ifndef WIDTH
WIDTH = 64
endif
ifeq ($(WIDTH), 64)
 WIDTH_DEF = -D WIDTH64
endif
ifeq ($(WIDTH), 128)
 WIDTH_DEF = -D WIDTH128
endif

DECNUMPATH = ../../../decNumberLib
SRCS = recycle.c lookupa.c hashtab.c deposit.c main.c basic_fcn.c rate.c mortgage.c credit_card.c decfcn.c
OBJS = $(SRCS:.c=.o)
OPT = -O3
DEC = 64
DECDPUN = 4

DEBUG = #-D TEST -D INQUIRY -D DEBUG
SS_PATH = ../../../gcc/gcc-cross-compiler/sslittle-na-sstrix/include
UNIX  = -D UNIX -U LINUX -U SIMPLESCALAR
LINUX = -D LINUX -U UNIX -U SIMPLESCALAR
SS    = -D SIMPLESCALAR -U LINUX -U UNIX

ifdef OS
ifeq ($(OS),SS)
TARG   = SS.Banking.TestGen.DEC$(DEC)
TARG_CONV   = CONVERT.SS.DEC$(DEC)
ifeq ($(CC),cc)
CC   = ../../../gcc/gcc-cross-compiler/bin/sslittle-na-sstrix-gcc
endif # CC=cc
CFLAG_SS = -I $(SS_PATH)
LIBFLAG = $(SS) 
DEC_LIBS = -lSSDecNum
UNIX_COUNTER = 
DIS = $(TARG).dis
OBJDUMP = ../../../gcc/gcc-cross-compiler/bin/sslittle-na-sstrix-objdump
ISAFLAG = -D HW
else # endof: ifeq ($(OS),SS)
CC   = gcc
ifeq ($(OS),UNIX)
DEC_LIBS = -lUNIXDecNum
TARG   = Banking.TestGen.UNIX.DEC$(DEC)
TARG_CONV   = CONVERT.UNIX.DEC$(DEC)
LIBFLAG = $(UNIX)
UNIX_COUNTER = -lcpc
CFLAG_SS = 
else # endof: ifeq ($(OS),UNIX)
ifndef DEC_DATATYPE
DEC_DATATYPE=DECNUMBER_ARBITRARY
endif
ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED)
DEC_LIBS = LINUXDecFloats.a
else
ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
DEC_LIBS = LINUXDecNum.a
else
DEC_DEF = -D DECIMAL_GLOBAL_EXCEPTION_FLAGS=1 -D BID #intel library 
DEC_LIBS = libbid_glbflags.a LINUXDecNum.a
endif # end: DECNUMBER_ARBITRARY
endif # end DECNUMBER_FIXED
TARG   = LINUX.Banking.TestGen.DEC$(DEC)
TARG_CONV   = CONVERT.LINUX.DEC$(DEC)
LIBFLAG = $(LINUX)
UNIX_COUNTER =
CFLAG_SS = 
endif #"$(OS)" "UNIX"
endif # ifeq CC=cc
endif # ifdef OS
#CFLAG = -Wall -g $(OPT) -static -D DEC$(DEC) $(ISAFLAG) -I $(DECNUMPATH) $(CFLAG_SS)
CFLAG = -Wall -g $(OPT) -static  -D DEC$(DEC) $(ISAFLAG) $(WIDTH_DEF) $(CFLAG_SS) -D ${DEC_DATATYPE} ${DEC_DEF} -g
GFLAG = -pg -D GPROF

#LIBS = $(DEC_LIBS) $(UNIX_COUNTER) -L $(DECNUMPATH)
LIBS = $(DEC_LIBS) $(UNIX_COUNTER)

LFLAG = $(LIBFLAG) $(DEBUG) 

CONDOR = #condor_compile
ifdef OS
all: $(TARG)
$(TARG): $(OBJS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS) -lm -g
ifeq ($(OS),SS)
	$(OBJDUMP) -x -d -l $(TARG) > $(DIS)
endif
endif
NON_OS:
	@echo "[ERROR] Please specify the OS. OS = [UNIX|LINUX|SS]"
%.o: %.c
	$(CC) $(CFLAG) $(LFLAG) -c $< -o $@
clean:
	rm -f *.o *.s *~ core* log* acct_list_* 
