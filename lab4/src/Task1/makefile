CC=gcc
CFLAGS=-I.
all:  parallel_min_max process_memory
parallel_min_max : utils find_min_max utils.h find_min_max.h parallel_min_max.c
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)
utils : utils.h utils.c
	$(CC) -o utils.o -c utils.c $(CFLAGS)
find_min_max : utils.h find_min_max.h find_min_max.c
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)
process_memory : process_memory.c
	$(CC) process_memory.c $(CFLAGS)
