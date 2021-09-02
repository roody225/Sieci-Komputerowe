//Andrzej Tkaczyk, 309 181
#include "router.h"

int main(){
  int n;
  struct record directly_connected[10];
  struct record distance_vector[20];
  // tutaj powinny być jakieś wskaźniki alokowane malloc-iem i realloc-owane gdy tablica będzie się rozrastała,
  // ale wydaje mi się, że jest to niepotrzebne komplikowanie kodu (nie o to chodzi w zadaniu)

  scanf("%d", &n);
  for (int i=0; i < n; i++) {
    char address[20];
    char tmp[20];
    scanf("%s %s %d", address, tmp, &directly_connected[i].distance);
    
    int it = 0;
    while (address[it] != '/') {
      tmp[it] = address[it];
      it++;
    }
    tmp[it] = 0;
    int success = inet_pton(AF_INET, tmp, &directly_connected[i].ipv4);
    if (success == 0) {
      fprintf(stderr, "input error\n");
      exit(1);
    }

    directly_connected[i].mask_num = address[it+1] - '0';
    if (address[it+2] != 0) {
      directly_connected[i].mask_num *= 10;
      directly_connected[i].mask_num += address[it+2] - '0';
    }
    directly_connected[i].mask = ~ ((1 << (32 - directly_connected[i].mask_num)) - 1);
    
    directly_connected[i].via = directly_connected[i].ipv4;
    directly_connected[i].ipv4.s_addr = htonl(ntohl(directly_connected[i].ipv4.s_addr) & directly_connected[i].mask);

    directly_connected[i].is_directly_connected = 1;
    directly_connected[i].last_seen = 0;

    distance_vector[i].ipv4 = directly_connected[i].ipv4;
    distance_vector[i].mask = directly_connected[i].mask;
    distance_vector[i].mask_num = directly_connected[i].mask_num;
    distance_vector[i].is_directly_connected = 1;
    distance_vector[i].distance = directly_connected[i].distance;
    distance_vector[i].last_seen = 0;
  }

  int vector_size = n;

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "socket error\n"); 
		return 1;
	}
  int broadcastPermission = 1;
  setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission));

  struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(54321);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "bind error\n"); 
		return 1;
	}

  for (;;) {
    for (int i=0; i < n; i++) {
      directly_connected[i].last_seen += 1;
      if (directly_connected[i].last_seen >=5) {
        update_unreachable(directly_connected[i], distance_vector, vector_size);
      }
      for (int j=0; j < vector_size; j++){
        if (i == 0) {
          distance_vector[j].last_seen += 1;
        }
        if (send1(sockfd, directly_connected[i], distance_vector[j])) {
          distance_vector[i].distance = UINT32_MAX;
        } else {
          distance_vector[i].distance = directly_connected[i].distance;
        }
      }
    }

    printf("table:\n");
    printTable(n, vector_size, distance_vector);
    printf("\n");

    time_t seconds = 4;
    struct timeval tv; tv.tv_sec = seconds; tv.tv_usec = 0;
    struct timeval  start, end;
    gettimeofday(&start, NULL);
    gettimeofday(&end, NULL);
    
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);

    while (end.tv_sec - start.tv_sec < seconds) {
      int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
      gettimeofday(&end, NULL);

      if (ready < 0) {
        fprintf(stderr, "select error\n");
        exit(1);
      } else if (ready) {
        struct record result;
        int success = read1(sockfd, &result);
        if (success) {
          vector_size = add1(directly_connected, n, distance_vector, vector_size, result);
        }
        tv.tv_sec = seconds - (end.tv_sec - start.tv_sec);
      }
    }
  }

  return 0;
}