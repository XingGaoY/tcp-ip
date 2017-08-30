#ifndef _ETHARP_H_
#define _ETHARP_H_

#include "def.h"
#include "ethernet.h"
#include "ip4_addr.h"
#include "netif.h"
#include "skbuff.h"

#define SIZEOF_ETHARP_HDR 28
#define MAX_ETHARP_HDR SIZEOF_ETHARP_HDR+SIZEOF_ETH_HDR

struct etharp_hdr{
  uint16_t hwtype;
  uint16_t proto;
  uint8_t hwlen;
  uint8_t protolen;
  uint16_t opcode;
  struct eth_addr shwaddr;	//sender ethernet addr
  struct ip4_addr2 sipaddr;  //sender ip addr
  struct eth_addr dhwaddr;
  struct ip4_addr2 dipaddr;
};

/* ARP hwtype values */
enum etharp_hwtype{
  HWTYPE_ETHERNET = 1
};

/* ARP message types (opcodes) */
enum etharp_opcode{
  ARP_REQUEST = 1,
  ARP_REPLY = 2
};

void etharp_update_arp_entry(const struct ip4_addr *ipaddr, struct eth_addr *ethaddr);

void
etharp_raw(const struct eth_addr *ethsrc_addr, const struct eth_addr *ethdst_addr,
           const struct eth_addr *hwsrc_addr, const struct ip4_addr *ipsrc_addr,
           const struct eth_addr *hwdst_addr, const struct ip4_addr *ipdst_addr,
           const uint16_t opcode);

void etharp_input(struct sk_buff *skb);
void etharp_output(struct sk_buff *skb, struct ip4_addr *dst);

#endif //_ETHARP_H_
