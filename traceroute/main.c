//Andrzej Tkaczyk  309181
#include"traceroute.h"

int main(int argc, char **argv){
  if(argc == 1){
    fprintf(stderr, "nie podano adresu ipv4\n");
    exit(1);
  }
  if(argc > 2){
    fprintf(stderr, "podano zbyt wiele argumentów\n");
    exit(1);
  }

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
		fprintf(stderr, "socket error\n"); 
		exit(1);
	}

  uint16_t pid = getpid();

  char *ip_addr = argv[1];
  if(!isValidIpv4(ip_addr)){
    fprintf(stderr, "podano niepoprawny adres ipv4\n");
    exit(1);
  }

  char responses[31][3][20];
  int utos = 1e6;
  struct res result;
  int notFoundYet = 1;
  int seconds = 1;

  for(uint16_t i=1; i<31 && notFoundYet; i++){
    // if(i<10){
    //   printf(" ");
    // }
    printf("%2d: ", i);
    for(int j=0; j<3; j++){
      sendEchoReq(sockfd, pid, i, ip_addr);
    }
    int recived = 0;
    struct timeval tv; tv.tv_sec = seconds; tv.tv_usec = 0;
    
    struct timeval  start, end;
    gettimeofday(&start, NULL);
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    double dif = 0;
    double avg_time = 0;

    while(dif < seconds && recived < 3){
      int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
      gettimeofday(&end, NULL);
      dif = (double) (end.tv_usec - start.tv_usec) / utos +
        (double) (end.tv_sec - start.tv_sec);
      if(ready < 0){
        fprintf(stderr, "select error\n");
        exit(1);
      } else if(ready){
        int success = read1(sockfd, pid, i, &result, ip_addr);
        if(success){
          avg_time += dif;
          strcpy(responses[i][recived], result.ip_str);
          if(strcmp(result.ip_str, ip_addr) == 0){
            notFoundYet = 0;
          }
          recived += 1;
        }
      }
      tv.tv_usec = seconds * utos - dif * utos;
    }
    if(recived == 0){
      printf("*\n");
    } else{
      printf("%s", responses[i][0]);
      for(int j=1; j<recived; j++){
        int flag = 1;
        for(int k=0; k<j; k++){
          if(strcmp(responses[i][j],  responses[i][k]) == 0){
            flag = 0;
            break;
          }
        }
        if(flag){
          printf("    %s  ", responses[i][j]);
        }
      }
      for(uint32_t j=0; j<20 - strlen(responses[i][recived-1]); j++){
        printf(" ");
      }
      if(recived == 3){
        printf("avg time: %.3f ms\n", avg_time/3 * 1000);
      } else{
        printf("avg time: ???\n");
      }
    }
  }

  return 0;
}