CC=gcc
CFLAGS=-I.

all: server client

server: lib server.c
	$(CC) server.c -L./ -lservfac -lpthread -o server.o

client: lib client.c
	$(CC) client.c -L./ -lservfac -lpthread -o client.o

lib: factorial.c
	$(CC) -c factorial.c -o fact.o
	ar rcs libservfac.a fact.o

clean: 
	rm -r *.o *.a *.out -f
	clear
