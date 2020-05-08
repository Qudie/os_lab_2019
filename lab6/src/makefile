all: server client MultModulo
MultModulo: MultModulo.c MultModulo.h
	gcc -o MultModulo.o -c MultModulo.c -std=c99
client: client.c MultModulo
	gcc -o client.out client.c MultModulo.o -lpthread -lm -std=c99
server: server.c MultModulo
	gcc -o server.out server.c MultModulo.o -lpthread -lm -std=c99

