#ifndef _UTIL_H_
#define _UTIL_H_

#include "ip4_addr.h"
#include "eth_addr.h"
#include "skbuff.h"

void print_ip_addr(const ip4_addr *addr);

void print_ip_addr2(struct ip4_addr2 *addr);

void print_eth_addr(struct eth_addr *addr);

uint16_t checksum(void *dataptr, int len);

uint16_t pseudo_chksum(struct sk_buff *skb, ip4_addr src, ip4_addr dest, uint8_t proto);

#endif // _UTIL_H_
