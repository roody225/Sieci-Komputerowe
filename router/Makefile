CC=gcc
FLAGS=-std=gnu17 -Wall -Wextra

main: main.o router.o
		$(CC) $(FLAGS) -o router main.o router.o

main.o: main.c
		$(CC) $(FLAGS) -o main.o -c main.c

router.o: router.c router.h
		$(CC) $(FLAGS) -o router.o -c router.c 

clean:
		rm -f main.o router.o

distclean:
		rm -f main.o router.o router
