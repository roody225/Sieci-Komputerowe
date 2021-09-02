//Andrzej Tkaczyk, 309 181
#include "transport.h"

int main(int argc, char *argv[]){

  char addr_str[16];
  int port;
  char filename[30];
  int size;
  struct in_addr ipv4;
  int start = 0;
  int to_send = 0;
  int recived[WSIZE];
  int starts[WSIZE];
  int sizes[WSIZE];
  uint8_t window[WSIZE][PSIZE+1];
  for (int i=0; i<WSIZE; i++) {
    recived[i] = 0;
  }
  if (argc != 5) {
    fprintf(stderr, "input error\n");
  }
  strcpy(addr_str, argv[1]);
  port = atoi(argv[2]);
  strcpy(filename, argv[3]);
  size = atoi(argv[4]);
	FILE *f = fopen(filename, "w");
	fclose(f);
  
  int success = inet_pton(AF_INET, addr_str, &ipv4);
  if (success == 0) {
    fprintf(stderr, "input error\n");
    exit(1);
  }
  if (port < 1101 || port > 65534) {
    fprintf(stderr, "input error\n");
    exit(1);
  }
  if (size < 0 || size > 10001000) {
    fprintf(stderr, "input error\n");
    exit(1);
  }

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "socket error\n"); 
		return 1;
	}
	
	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "bind error\n"); 
		return 1;
	}

	while (start < size) {
    to_send = prepare_requests(starts, sizes, start, size);
		for (int i=0; i<to_send; i++) {
      int failed = send1(sockfd, ipv4, port, starts[i], sizes[i]);
      if (failed) {
        fprintf(stderr, "send error\n");
        exit(1);
      }
    }

    time_t seconds = 1;
    time_t useconds = 0;
    struct timeval tv; tv.tv_sec = seconds; tv.tv_usec = useconds;
		
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
		
		int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);

    if (ready < 0) {
      fprintf(stderr, "select error\n");
      exit(1);
    } else if (ready == 0) {
      continue;
    } else {
			int success = read1(sockfd, ipv4, recived, starts, window);
      if (success) {
        if (recived[0]) {
          manage_writes(filename, recived, sizes, window);
          rewrite(&start, recived, starts, sizes, window);
			    printf("completed: %d / %d \n", start, size);
        }
      }
    }
  }
  
  return 0;
}
