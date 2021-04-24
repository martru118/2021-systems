.PHONY : all clean

all : server client

server : server.o
	cc -o server server.o

client : client.o
	cc -o client client.o

clean :
	rm client server client.o server.o
