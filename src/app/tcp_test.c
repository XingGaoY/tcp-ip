#include "app.h"
#include <stdio.h>

#define dst_ip 0x83caa8c0 // 192.168.202.131
#define dst_port 1800

void tcp_server(){
  struct __sockaddr addr;
  //char buf[100];  

  printf("TCP server...\n");
  addr.port = 1800;
  int sockfd = raw_socket(_SOCK_STREAM);
  raw_bind(sockfd, &addr);
  printf("TCP server binded...\n");
  raw_listen(sockfd);
  printf("TCP server set passive...\n");
}
