#include "app.h"
#include <stdio.h>

void udp_server(){
  struct __sockaddr addr;
  char buf[100];  

printf("UDP server...\n");
  addr.port = 1800;
  int sockfd = raw_socket(_SOCK_DGRAM);
  raw_bind(sockfd, &addr);
  printf("UDP server binded...\n");
  while(1){
    raw_recv(sockfd, buf, 100);
  }
}
