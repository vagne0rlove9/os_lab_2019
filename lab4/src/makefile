CC=gcc
CFLAGS=-I.

all:  parallel_min_max process_memory

zoombie:
	$(CC) zoombie.c && ./a.out $(CFLAGS)

process_memory:
	$(CC) process_memory.c -o process_memory $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o parallel_min_max process_memory *.out