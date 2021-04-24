CFLAGS = -Wall -g

.PHONY: all clean

all: copy lab4

copy: copy.o
	cc -o copy copy.o

lab4: lab4.o
	cc -o lab4 lab4.o

clean:
	rm copy copy.o lab4 lab4.o