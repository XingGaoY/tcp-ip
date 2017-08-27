#ifndef _IP4_ADDR_H_
#define _IP4_ADDR_H_

#include"def.h"

struct ip4_addr{
  uint32_t addr;
};

/* 
 * Some compiler may make ip4_addr aligned, so use another ip addr def in ARP
 * but why?
 */
struct ip4_addr2{
  uint16_t addrw[2];
};

/** MEMCPY-like copying of IP addresses where addresses are known to be
 * 16-bit-aligned if the port is correctly configured (so a port could define
 * this to copying 2 u16_t's) - no NULL-pointer-checking needed. */
#ifndef IPADDR2_COPY
#define IPADDR2_COPY(dest, src) memcpy(dest, src, sizeof(struct ip4_addr))
#endif

//TODO::wrong location, move it somewhere else
struct eth_addr{
  uint8_t addr[ETH_HWADDR_LEN];
};

//netif, temperarily put it as some global variables
extern struct eth_addr netif_hwaddr;
extern struct ip4_addr netif_ip;

#endif //_IP4_ADDR_H_
