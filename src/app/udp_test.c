#include "app.h"
#include <stdio.h>
#include <string.h>
#include <memory.h>

#define dst_ip 0x83caa8c0 // 192.168.202.131
#define dst_port 1800

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
    int len = raw_recv(sockfd, buf, 100);
    printf("=====\nMSG Received:\n");
    for(int i=0; i<len; i++){
      if(i%8 == 0) printf("\n");
      printf("%c", buf[i]);
    }
    printf("\n=====\n");
  }
}

void udp_client(){
  struct __sockaddr saddr, daddr;
  char buf[100];  

  printf("UDP client...\n");
  daddr.port = dst_port;
  daddr.ip_addr = dst_ip;
  saddr.port = 1800;
  int sockfd = raw_socket(_SOCK_DGRAM);
  raw_bind(sockfd, &saddr);
  printf("UDP client binded...\n");
  while(1){
    memset(buf, 0, 100);
    fgets(buf, 100, stdin);
    int len = strlen(buf);

    raw_send(sockfd, buf, len, &daddr);
  }
}
