#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include "netif.h"
#include "skbuff.h"

#define SIZEOF_ETH_HDR sizeof(struct eth_hdr)
#define MIN_ETH_LEN 60

struct eth_hdr{
  struct eth_addr dest;
  struct eth_addr src;
  uint16_t type;
};

/**
 *  * @ingroup ethernet
 *   * A list of often ethtypes (although lwIP does not use all of them): */
enum eth_type {
  /** Internet protocol v4 */
  ETHTYPE_IP        = 0x0800U,
  /** Address resolution protocol */
  ETHTYPE_ARP       = 0x0806U, 
  /** Wake on lan */
  ETHTYPE_WOL       = 0x0842U,
  /** RARP */
  ETHTYPE_RARP      = 0x8035U,
  /** Virtual local area network */
  ETHTYPE_VLAN      = 0x8100U,
  /** Internet protocol v6 */
  ETHTYPE_IPV6      = 0x86DDU,
  /** PPP Over Ethernet Discovery Stage */
  ETHTYPE_PPPOEDISC = 0x8863U,
  /** PPP Over Ethernet Session Stage */
  ETHTYPE_PPPOE     = 0x8864U,
  /** Jumbo Frames */
  ETHTYPE_JUMBO     = 0x8870U,
  /** Process field network */
  ETHTYPE_PROFINET  = 0x8892U,
  /** Ethernet for control automation technology */
  ETHTYPE_ETHERCAT  = 0x88A4U,
  /** Link layer discovery protocol */
  ETHTYPE_LLDP      = 0x88CCU,
  /** Serial real-time communication system */
  ETHTYPE_SERCOS    = 0x88CDU,
  /** Media redundancy protocol */
  ETHTYPE_MRP       = 0x88E3U,
  /** Precision time protocol */
  ETHTYPE_PTP       = 0x88F7U,
  /** Q-in-Q, 802.1ad */
  ETHTYPE_QINQ      = 0x9100U
};

int ethernet_input(struct sk_buff *skb);
int ethernet_output(struct sk_buff *skb, const struct eth_addr *src, const struct eth_addr *dst, uint16_t eth_type);
void ip4_output_raw(struct sk_buff *skb, ip4_addr src, ip4_addr dst);

#endif //_ETHERNET_H_
