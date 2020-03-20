CC=gcc
CFLAGS=-I.
all: utils find_min_max parallel_min_max sequential_min_max
exec:  sequential_min_max
	$(CC) -o exec_s -c exec_s.c
	$(CC) exec_s -o exec_s.out
	./exec_s.out 1 4
sequential_min_max : utils find_min_max utils.h find_min_max.h sequential_min_max.c
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils find_min_max utils.h find_min_max.h parallel_min_max.c
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils : utils.h utils.c
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max : utils.h find_min_max.h find_min_max.c
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max
