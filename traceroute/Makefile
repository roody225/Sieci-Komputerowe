CC=gcc
FLAGS=-std=gnu17 -Wall -Wextra

main: main.o traceroute.o icmp_checksum.o
		$(CC) $(FLAGS) -o traceroute main.o traceroute.o icmp_checksum.o 

main.o: main.c
		$(CC) $(FLAGS) -o main.o -c main.c

traceroute.o: traceroute.c traceroute.h
		$(CC) $(FLAGS) -o traceroute.o -c traceroute.c 

icmp_checksum.o: icmp_checksum.c icmp_checksum.h
		$(CC) $(FLAGS) -o icmp_checksum.o -c icmp_checksum.c

clean:
		rm -f traceroute.o icmp_checksum.o main.o

distclean:
		rm -f traceroute.o traceroute icmp_checksum.o main.o
