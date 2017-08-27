#ifndef _NETIF_H_
#define _NETIF_H_

#include"ip4_addr.h"

#define IP_ADDR 0x0400000a   //10.0.0.4
#define HW_ADDR {{0x0a,0x0b,0x0c,0x0d,0x0e,0x0f}}

struct netif{
  struct ip4_addr ipaddr;
  struct eth_addr hwaddr;
};

extern struct netif *netif;

void netdev_init();
void netdev_xmit(char *buf, int len);

#endif // _NETIF_H_
