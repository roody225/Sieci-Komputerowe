//Andrzej Tkaczyk, 309 181
#include "transport.h"

int min(int a, int b){
  if (a < b) {
    return a;
  }
  return b;
}

int prepare_requests(int *starts, int *sizes, int start, int size){
  int prep = 0;
  while (start < size && prep < WSIZE) {
    int block_size = min(PSIZE, size - start);
    starts[prep] = start;
    sizes[prep] = block_size;
    start += block_size;
		prep++;
  }
  return prep;
}
int send1(int sockfd, struct in_addr ipv4, int port, int start, int size){
  char buffer[20];
  sprintf(buffer, "GET %d %d\n", start, size);
  ssize_t buffer_size = strlen(buffer);

  struct sockaddr_in recipent;
	bzero (&recipent, sizeof(recipent));
	recipent.sin_family = AF_INET;
	recipent.sin_port = htons(port);
	recipent.sin_addr = ipv4;

  ssize_t bytes_sent = sendto(sockfd, buffer, buffer_size, 0, (struct sockaddr*) &recipent, sizeof(recipent));
  if (bytes_sent != buffer_size) {
    return 1;
  }
  return 0;
}
int read1(int sockfd, struct in_addr ipv4, int *recived, int *starts, uint8_t window[WSIZE][PSIZE+1]){
  struct sockaddr_in sender;
  socklen_t sender_len = sizeof(sender);
  char buffer[IP_MAXPACKET+1];

  ssize_t datagram_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
  if (sender.sin_addr.s_addr != ipv4.s_addr) {
    return 0;
  }
  int start, size;
  sscanf(buffer, "DATA %d %d", &start, &size);
	int buffer_begin = 0;
  if (start < starts[0]) {
    return 0;
  }
  for (int i=0; i<WSIZE; i++) {
    if (start == starts[i]) {
      if (recived[i] == 0){
				while (buffer[buffer_begin] != '\n') {
					buffer_begin++;
				}
				buffer_begin++;
        recived[i] = 1;
        for (int j=start; j<start+size; j++) {
          window[i][j-start] = buffer[j-start+buffer_begin];
        }
      }
      break;
    }
  }
  return 1;
}
void manage_writes(char *filename, int *recived, int *sizes, uint8_t window[WSIZE][PSIZE+1]){
  FILE *f = fopen(filename, "a");
  for (int i=0; i<WSIZE && recived[i]==1; i++) {
    size_t written = fwrite(window[i], sizeof(uint8_t), sizes[i], f);
    if (written != sizes[i]) {
      fprintf(stderr, "write error\n");
      exit(1);
    }
  }
	fclose(f);
}
void rewrite(int *start, int *recived, int *starts, int *sizes, uint8_t window[WSIZE][PSIZE+1]){
  int n = 0;
  while (n < WSIZE && recived[n]==1) {
		recived[n] = 0;
    n++;
  }
  *start = starts[n-1] + sizes[n-1];
  for (int i=n; i<WSIZE; i++) {
    int ni = i-n;
    starts[ni] = starts[i];
    sizes[ni] = sizes[i];
		recived[ni] = recived[i];
		
    if (recived[i]) {
      for (int j=0; j<sizes[i]; j++) {
        window[ni][j] = window[i][j];
      }
    }
		recived[i] = 0;
  }
}
