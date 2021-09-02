CC=gcc
FLAGS=-std=gnu17 -Wall -Wextra -O3

main: main.o transport.o
		$(CC) $(FLAGS) -o transport main.o transport.o

main.o: main.c
		$(CC) $(FLAGS) -o main.o -c main.c

router.o: transport.c transport.h
		$(CC) $(FLAGS) -o transport.o -c transport.c 

clean:
		rm -f main.o transport.o

distclean:
		rm -f main.o transport.o transport