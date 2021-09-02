//Andrzej Tkaczyk, 309 181
#include "router.h"

void printTable(int n, int size, struct record table[]){
  char addr[20];

  for (int i=0; i < size; i++) {
    if (table[i].last_seen >= 5) {
      table[i].distance = UINT32_MAX;
      if (i >= n) {
        continue;
      }
    }
    inet_ntop (AF_INET, &(table[i].ipv4), addr, sizeof(addr));
    printf("%s/%hhd ", addr, table[i].mask_num);

    if (table[i].distance == UINT32_MAX) {
      printf("unreachable ");
    } else {
      printf("distance %d ", table[i].distance);
    }
    if (table[i].is_directly_connected){
      printf("connected directly\n");
    } else {
      inet_ntop (AF_INET, &(table[i].via), addr, sizeof(addr));
      printf("via %s\n", addr);
    }
  }
}

int send1(int socket, struct record to, struct record value){
  uint8_t buffer[9];
  *((struct in_addr *)buffer) = value.ipv4;
  buffer[4] = value.mask_num;
  *((uint32_t *)(buffer+5)) = htonl(value.distance + to.distance);

  ssize_t buffer_size = 9;

  struct in_addr broadcast;
  broadcast.s_addr = htonl(ntohl(to.via.s_addr) | (~to.mask));

  struct sockaddr_in recipent;
	bzero (&recipent, sizeof(recipent));
	recipent.sin_family = AF_INET;
	recipent.sin_port = htons(54321);
	recipent.sin_addr = broadcast;

  ssize_t bytes_sent = sendto(socket, buffer, buffer_size, 0, (struct sockaddr*) &recipent, sizeof(recipent));
  if (bytes_sent != buffer_size) {
    return 1;
  }
  return 0;
}

int read1(int socket, struct record *result){
  struct sockaddr_in sender;
  socklen_t sender_len = sizeof(sender);
  u_int8_t buffer[IP_MAXPACKET+1];

  ssize_t datagram_len = recvfrom(socket, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
  if (datagram_len != 9) {
    return 0;
  }

  struct in_addr *tmp_addr = (struct in_addr *)buffer;
  uint8_t *tmp_mask_num = (uint8_t *)(buffer+4);
  uint32_t *tmp_distance = (uint32_t *)(buffer+5);
  
  result->ipv4 = *tmp_addr;
  result->mask_num = *tmp_mask_num;
  result->mask = ~ ((1 << (32 - result->mask_num)) - 1);
  result->distance = ntohl(*tmp_distance);
  result->via = sender.sin_addr;
  result->is_directly_connected = 0;
  result->last_seen = 0;

  return 1;
}
int add1(struct record directly[], int n, struct record dist_vec[], int vec_size, struct record result){
  for (int i=0; i < n; i++) {
    if (directly[i].ipv4.s_addr == htonl(ntohl(result.via.s_addr) & result.mask)) {
      directly[i].last_seen = 0;
      dist_vec[i].last_seen = 0;
    }
    if (directly[i].ipv4.s_addr == result.ipv4.s_addr && directly[i].mask_num == result.mask_num) {
      return vec_size;
    }
  }
  for (int i=0; i < vec_size; i++) {
    if (dist_vec[i].ipv4.s_addr == result.ipv4.s_addr && dist_vec[i].mask_num == result.mask_num) {
      if (result.distance < dist_vec[i].distance || result.via.s_addr == dist_vec[i].via.s_addr) {
        dist_vec[i].distance = result.distance;
        dist_vec[i].via = result.via;
      }
      dist_vec[i].last_seen = 0;
      return vec_size;
    }
  }
  dist_vec[vec_size] = result;
  return vec_size + 1;
}

void update_unreachable(struct record to, struct record dist_vec[], int vec_size){
  for (int i=0; i < vec_size; i++) {
    if (to.ipv4.s_addr == htonl(ntohl(dist_vec[i].via.s_addr) & dist_vec[i].mask)) {
      dist_vec[i].distance = UINT32_MAX;
    }
  }
}