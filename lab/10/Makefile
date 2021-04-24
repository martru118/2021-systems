.PHONY : all clean
CFLAGS = -g -w -Wall

all : prog1 prog2

prog1 : prog1.o
	cc -o prog1 prog1.o -lcrypto
	
prog2 : prog2.o
	cc -o prog2 prog2.o -lcrypto

clean :
	rm prog1 prog2 *.o

