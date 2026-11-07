OPT = -O3

all: LINUXDecNum.a LINUXDecFloats.a

example7: example7.o decContext.o decQuad.o 
	gcc $(OPT) example7.o decContext.o decQuad.o -o example7.exe

example5: example5.o decContext.o decNumber.o decimal64.o
	gcc $(OPT) example5.o decNumber.o decContext.o decimal64.o -o example5.exe

example1: example1.o decContext.o decNumber.o
	gcc $(OPT) example1.o decNumber.o decContext.o -o example1.exe

example7.o: example7.c decQuad.h decContext.h
	gcc $(OPT) -c example7.c -o example7.o

example5.o: example5.c decimal64.h decNumber.h
	gcc $(OPT) -c example5.c -o example5.o

example1.o: example1.c decNumber.h decContext.h
	gcc $(OPT) -c example1.c -o example1.o

decNumber.o: decNumber.c decNumber.h decNumberLocal.h decContext.h
	gcc $(OPT) -c decNumber.c -o decNumber.o

decContext.o: decContext.c decContext.h decNumberLocal.h
	gcc $(OPT) -c decContext.c -o decContext.o

decimal64.o: decimal64.c decNumber.h decNumberLocal.h decimal64.h decContext.h
	gcc $(OPT) -c decimal64.c -o decimal64.o

decDouble.o: decDouble.c decContext.h decDouble.h decNumberLocal.h decCommon.c decDPD.h decBasic.c decQuad.h
	gcc $(OPT) -c decDouble.c -o decDouble.o

decimal128.o: decimal128.c decNumber.h decNumberLocal.h decimal128.h decContext.h
	gcc $(OPT) -c decimal128.c -o decimal128.o

decQuad.o: decQuad.c decContext.h decQuad.h decNumberLocal.h decCommon.c decDPD.h decBasic.c
	gcc $(OPT) -c decQuad.c -o decQuad.o

#decCommon.o: decCommon.c decDPD.h
#	gcc $(OPT) - c decCommon.c -o decCommon.o

clean:
	rm *.o *.exe

LINUXDecNum.a: decNumber.o decContext.o decimal64.o decimal128.o
	ar r LINUXDecNum.a decNumber.o decContext.o decimal64.o decimal128.o

LINUXDecFloats.a: decNumber.o decContext.o decimal64.o decDouble.o decimal128.o decQuad.o
	ar r LINUXDecFloats.a decNumber.o decContext.o decimal64.o decDouble.o decimal128.o decQuad.o
	
