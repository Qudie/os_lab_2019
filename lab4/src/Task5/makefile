parallel_sum: utils sum parallel_sum.c
	gcc -o parallel_sum.out parallel_sum.c utils.o sum.o -lm -lpthread -std=c99
utils: utils.c utils.h
	gcc	-o utils.o -c utils.c -I. -std=c99
sum: sum.c sum.h
	gcc -o sum.o -c sum.c -std=c99
	
