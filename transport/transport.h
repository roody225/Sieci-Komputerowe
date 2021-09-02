//Andrzej Tkaczyk, 309 181
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <unistd.h>

#define WSIZE 100
#define PSIZE 1000

int min(int a, int b);

int prepare_requests(int *starts, int *sizes, int start, int size);
int send1(int sockfd, struct in_addr ipv4, int port, int start, int size);
int read1(int sockfd, struct in_addr ipv4, int *recived, int *starts, uint8_t window[WSIZE][PSIZE+1]);
void manage_writes(char *filename, int *recived, int *sizes, uint8_t window[WSIZE][PSIZE+1]);
void rewrite(int *start, int *recived, int *starts, int *sizes, uint8_t window[WSIZE][PSIZE+1]);
