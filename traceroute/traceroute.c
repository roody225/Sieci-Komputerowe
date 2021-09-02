//Andrzej Tkaczyk 309181
#include"traceroute.h"
#include"icmp_checksum.h"

void sendEchoReq(int sockfd, uint16_t id, uint16_t seq, const char *ip_addr){
  struct icmp header;
  header.icmp_type = ICMP_ECHO;
  header.icmp_code = 0;
  header.icmp_hun.ih_idseq.icd_id = id;
  header.icmp_hun.ih_idseq.icd_seq = seq;
  header.icmp_cksum = 0;
  header.icmp_cksum = compute_icmp_checksum(
    (u_int16_t*)&header, sizeof(header));

  struct sockaddr_in recipient;
  bzero (&recipient, sizeof(recipient));
  recipient.sin_family = AF_INET;
  inet_pton(AF_INET, ip_addr, &recipient.sin_addr);

  int ttl = seq;
  setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

  ssize_t bytes_sent = sendto(
    sockfd,
    &header,
    sizeof(header),
    0,
    (struct sockaddr*)&recipient,
    sizeof(recipient)
  );

  if(bytes_sent != sizeof(header)){
    fprintf(stderr, "send error\n");
  }
}

int isValidIpv4(char *x){
  struct sockaddr_in sa;
  int result = inet_pton(AF_INET, x, &(sa.sin_addr));
  return result != 0;
}

int read1(int sockfd, uint16_t id, uint16_t seq, struct res *result, char *target){
  struct sockaddr_in sender;
  socklen_t sender_len = sizeof(sender);
  u_int8_t buffer[IP_MAXPACKET];

  ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
    (struct sockaddr*)&sender, &sender_len);
  if (packet_len < 0){
		fprintf(stderr, "recvfrom error\n");
		exit(0);
	}

  inet_ntop (AF_INET, &(sender.sin_addr), result->ip_str, sizeof(result->ip_str));

  struct ip *ip_header = (struct ip*) buffer;

  u_int8_t *icmp_packet = buffer + 4 * ip_header->ip_hl;
  u_int8_t *origin = icmp_packet + 8;
  struct ip *origin_ip_header = (struct ip*) origin;
  u_int8_t *origin_icmp = origin + 4 * origin_ip_header->ip_hl;
  struct icmp *origin_icmp_header = (struct icmp*) origin_icmp;
  uint16_t my_id = origin_icmp_header->icmp_id;
  uint16_t my_seq = origin_icmp_header->icmp_seq;
  
  if(strcmp(result->ip_str, target) == 0){
    struct icmp *foo = (struct icmp*) icmp_packet;
    if(foo->icmp_id != id || foo->icmp_seq != seq){
      return 0;
    }
    return 2;
  } else{
    return (my_id == id && my_seq == seq);
  }
}