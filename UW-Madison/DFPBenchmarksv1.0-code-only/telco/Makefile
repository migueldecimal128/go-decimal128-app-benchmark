#----------------------------------------
# Makefile
# The makefile for banking system
# Decimal Floating-Point Benchmark
# Makefile to generate the code
# Developed by Liang-Kai Wang
# Department of Electrical and Computer Engineering
# University of Wisconsin-Madison
# $Revision: 1.18 $

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
DEC_DATATYPE_DEF = -D NATIVE_GCC_DPD 	# Native gcc DPD 
endif # Native gcc DPD 

ifeq ($(DEC_DATATYPE), NATIVE_GCC_BID) 
DEC_DATATYPE_DEF = -D NATIVE_GCC_BID -D DECIMAL_GLOBAL_EXCEPTION_FLAGS=0 -fno-strict-aliasing  # Native gcc BID 
endif # Native gcc BID

ifdef PROFILE

ifeq ($(DEC_DATATYPE), DECNUMBER_ARBITRARY)
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c cTelco754r.c decfcn.c 
endif

ifeq ($(DEC_DATATYPE), DECNUMBER_FIXED)
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c decDouble.c decQuad.c decSingle.c  cTelco754r.c decfcn.c
endif

ifeq ($(DEC_DATATYPE), BID)
SRCS = cTelco754r.c decfcn.c libbid_glbflags.a
endif 

ifeq ($(DEC_DATATYPE), NATIVE_GCC_DPD)
SRCS = decContext.c  decimal128.c  decimal32.c  decimal64.c  decNumber.c decPacked.c decDouble.c decQuad.c decSingle.c  cTelco754r.c decfcn.c
endif

ifeq ($(DEC_DATATYPE), NATIVE_GCC_BID)
SRCS = cTelco754r.c decfcn.c
endif 

ifeq ($(PROFILE), GPROF)
PG = -pg
PROFILE_MACRO = -D GPROF
else
ifeq ($(PROFILE), VTUNE)
PG = #-g
PROFILE_MACRO =
else
PG = #-g
PROFILE_MACRO =
endif #end of !vtune
endif #end of !gprof

else    #Not profile

ifeq ($(TIMING), ON)
TIME = -D TIMING
else #TIMING
TIME = 
endif # TIMING

ifeq ($(COUNTING), ON)
COUNT = -D COUNTING
else #COUNTING
COUNT = 
endif # COUNTING

SRCS =  cTelco754r.c decfcn.c
PG =
PROFILE_MACRO =
endif #end of GPROF

#This is to swap the input file format
SWAP_SRCS = swap.c

OBJS = $(SRCS:.c=.o)
SWAP_OBJS = $(SWAP_SRCS:.c=.o)
OPT = -O3
#DEBUG = -D DEBUG
SS_PATH = ../Banking/gcc/gcc-cross-compiler/sslittle-na-sstrix/include
UNIX  = -D UNIX -U LINUX -U SIMPLESCALAR
LINUX = $(WIDTH_DEF) $(DEC_DATATYPE_DEF) -D LINUX -U UNIX -U SIMPLESCALAR $(GPROF_MACRO) $(COUNT) $(TIME)
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
CC   = ../gcc-cross-compiler/bin/sslittle-na-sstrix-gcc
OBJDUMP = ../gcc-cross-compiler/bin/sslittle-na-sstrix-objdump
SET = set
endif
ifeq ($(OS),HW)
ISAFLAG = -D HW
SET = set
endif #HW
endif #!LINUX
endif #OS

TARG   = Telco.$(OS).$(DEC_DATATYPE)
DIS = $(TARG).dis
SWAP_TARG   = Swap.$(OS).$(DEC_DATATYPE)

CFLAG = -Wall $(PG) $(OPT) -static -D DEC64 $(ISAFLAG) $(CFLAG_SS)

LIBS = $(DEC_LIBS) $(UNIX_COUNTER)
LFLAG = $(LIBFLAG) $(DEBUG) 

ifneq ($(SET),set)
all:
	@echo '[ERROR] WRONG SWITCH: OS=$(OS), try OS=LINUX [PROFILE=GPROF|VTUNE|NO], OS=HW or OS=SW'
else #OS is correct
ifeq ($(OS),LINUX)
all: $(TARG) $(TEST_TARG)
	@echo '[INFO] Compilation with OS=$(OS), DEC_DATATYPE=$(DEC_DATATYPE), and PROFILE=$(PROFILE) is done'
	#@echo 'Remember to set LD_LIBRARY_PATH to the directory of decNumber library'
else #not LINUX
ifdef PROFILE
all:
	@echo '[ERROR] PROFILE switch cannot work with SW or HW switch!!'
else
all: $(TARG)
	@echo '[INFO] Compilation with $(OS) and DEC_DATATYPE=$(DEC_DATATYPE) is done'
	#@echo '######## Remember to set LD_LIBRARY_PATH to the directory of decNumber library ####'
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
#ifeq ($(OS),LINUX)
#$(SWAP_TARG): $(SWAP_OBJS)
#	$(CC) -o $(SWAP_TARG) $(SWAP_OBJS) $(LIBS)
#endif #end of LINUX

endif #end of set
%.o: %.c
	$(CC) $(CFLAG) $(LFLAG) -c $< -o $@
clean:
	find . -name "*~" -exec rm -rf {} \;
	rm -f *.o *.s *~ core* \#*
	rm -f sim-outorder Telco.SW.* Telco.HW.* Telco.LINUX.*

#################################################################
#  $Author: mjanders $ - The last CVS user to commit the file.
#  $Date: 2009-04-21 06:09:44 $ - The date of the last commit.
#  $Name:  $ - The tag name
#  $Log: Makefile,v $
#  Revision 1.18  2009-04-21 06:09:44  mjanders
#  Running simulations. Updating libraries. Compiling without flags
#
#  Revision 1.17  2009-03-15 06:24:33  mjanders
#  Added 128 bit support
#
#  Revision 1.16  2009-01-08 21:07:35  mjanders
#  Comment DEBUG by default
#
#  Revision 1.15  2009/01/02 17:19:34  mjanders
#  Added native_gcc_bid rounding
#
#  Revision 1.14  2008/11/19 06:00:34  mjanders
#  Fixed mistake in global flags
#
#  Revision 1.13  2008/09/24 07:40:23  mjanders
#  Cosmetic changes
#
#  Revision 1.12  2008/09/23 07:42:21  mjanders
#  Changed over to new BID libraries. Normal BID library has global flags.
#  The BID library we are using with native gcc types does not have global flags.
#  This required a define of DECIMAL_GLOBAL_FLAGS too.
#
#  Revision 1.11  2008/09/21 23:17:57  mjanders
#  Added native gcc types
#
#  Revision 1.10  2008/06/10 03:54:16  mjanders
#  Got rid of some references to DECNUMBER_TYPE and replaced with DEC_DATATYPE
#
#  Revision 1.9  2008/06/05 03:02:40  mjanders
#  Fixed SRCS variable to work with PROFILE=GPROF
#
#  Revision 1.8  2008/06/04 15:53:32  mjanders
#  Fixed Makefile so that default 'make OS=LINUX' would work.
#
#  Revision 1.7  2008/05/27 04:49:10  stsen
#  added benchmark timing via RDTSC
#
#  Revision 1.6  2008/05/19 19:12:09  mjanders
#  Fixed bug in SRCS if/else block
#
#  Revision 1.5  2008/05/19 00:33:58  mjanders
#  Added BID functionality
#
#  Revision 1.4  2008/05/03 20:35:27  mjanders
#  Removed outdated -I and -L options from Makefile
#
#  Revision 1.3  2008/04/24 23:13:16  mjanders
#  Converted telco benchmark to use decFloats. Changed the Makefile to
#  define DECNUMBER_FIXED in this case. Added COUNTING switch to the Makefile.
#  Updated const.h to make dec_t be a decDouble if DECNUMBER_FIXED is defined.
#  copied over the update operator.h file and fixed the TOINTEGRAL bug.
#
#  Updated cTelco754r.c on lines 170 and 166 to fix the mymemcpy issue for
#  the new datatypes. I also added in a declaration of add_op_zero to the
#  top of this file because it caused a compilation error when COUNTING=ON.
#
#  Revision 1.2  2008/04/23 21:25:06  mjanders
#  Changed Makefile to look for static libraries instead of dynamic. This benchmark
#  works for both arbitrary and fixed DecNumber libraries.
#
#  Revision 1.1.1.1  2008/04/23 21:17:29  mjanders
#  Initial entry of telco tarball.
#
#  Revision 1.1.1.1  2007/03/13 04:03:42  lwang
#  initial checkin for telco
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
