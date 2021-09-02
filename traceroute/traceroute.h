//Andrzej Tkaczyk 309181
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<string.h>
#include<unistd.h>

struct res {
  char ip_str[20];
  float rtt;
};

void sendEchoReq(int sockfd, uint16_t id, uint16_t seq, const char *ip_addr);
int isValidIpv4(char *x);
int read1(int sockfd, uint16_t id, uint16_t seq, struct res *result, char *target);
