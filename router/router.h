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

struct record{
  struct in_addr ipv4;
  struct in_addr via;
  uint32_t distance;
  uint32_t mask;
  uint8_t mask_num;
  uint8_t is_directly_connected;
  uint8_t last_seen;
};

void printTable(int n, int size, struct record table[]);
int send1(int socket, struct record to, struct record value);
int read1(int socket, struct record *result);
int add1(struct record directly[], int n, struct record dist_vec[], int vec_size, struct record result);
void update_unreachable(struct record to, struct record dist_vec[], int vec_size);