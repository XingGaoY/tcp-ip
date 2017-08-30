#ifndef _IP4_H_
#define _IP4_H_

#include "ip4_addr.h"
#include "sk_buff.h"

#define SIZEOF_IP4_HDR 20 

#define MAX_IP_HDR SIZEOF_ETH_HDR + SIZEOF_IP4_HDR

#define IP_PROTO_ICMP    1
#define IP_PROTO_IGMP    2
#define IP_PROTO_UDP     17
#define IP_PROTO_UDPLITE 136
#define IP_PROTO_TCP     6

struct ip_hdr{
  uint8_t _v_hl;
  uint8_t _tos;
  uint16_t _len;
  uint16_t _id;
  uint16_t _offset;
  uint8_t _ttl;
  uint8_t _proto;
  uint16_t _chksum;
  struct ip4_addr src;
  struct ip4_addr dest;
};

/* Data in ip_hdr that need to be saved to send back */
struct ip_cb{
  uint16_t _id;
  uint8_t _ttl;
};

/* Macros to get struct ip_hdr fields: */
#define IPH_V(hdr)  ((hdr)->_v_hl >> 4)
#define IPH_HL(hdr) ((hdr)->_v_hl & 0x0f)
#define IPH_TOS(hdr) ((hdr)->_tos)
#define IPH_LEN(hdr) ((hdr)->_len)
#define IPH_ID(hdr) ((hdr)->_id)
#define IPH_OFFSET(hdr) ((hdr)->_offset)
#define IPH_TTL(hdr) ((hdr)->_ttl)
#define IPH_PROTO(hdr) ((hdr)->_proto)
#define IPH_CHKSUM(hdr) ((hdr)->_chksum)

/* Macros to set struct ip_hdr fields: */
#define IPH_VHL_SET(hdr, v, hl) (hdr)->_v_hl = (u8_t)((((v) << 4) | (hl)))

/* Maintain the current ip datagram info */
struct ip_globals{
  struct ip_hdr *current_ip4_header;
  uint16_t current_ip_header_tot_len;
  struct ip4_addr current_iphdr_src;
  struct ip4_addr current_iphdr_dest;
};
extern struct ip_globals ip_data;

void ip4_input(struct sk_buff *skb);
void ip4_output(struct sk_buff *skb, struct ip4_addr src, struct ip4_addr dest);

#endif // _IP4_H_
