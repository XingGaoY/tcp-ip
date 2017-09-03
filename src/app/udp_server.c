#include "app.h"
#include <stdio.h>
#include <memory.h>

void udp_server(){
  struct __sockaddr addr;
  char buf[100];  

printf("UDP server...\n");
  addr.port = 1800;
  int sockfd = raw_socket(_SOCK_DGRAM);
  raw_bind(sockfd, &addr);
  printf("UDP server binded...\n");
  while(1){
    memset(buf, 0, 100);
    raw_recv(sockfd, buf, 100);
    printf("=====\nMSG Received:\n");
    for(int i=0; i<100; i++){
      if(i%8 == 0) printf("\n");
      printf("%02x", buf[i]);
    }
    printf("\n=====\n");
  }
}
