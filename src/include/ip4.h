#ifndef _IP4_H_
#define _IP4_H_

#include "ip4_addr.h"
#include "skbuff.h"
#include "ethernet.h"

#define SIZEOF_IP4_HDR sizeof(struct ip_hdr)

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
#define IPH_VHL_SET(hdr, v, hl) (hdr)->_v_hl = (uint8_t)((((v) << 4) | (hl)))

#define sk_for_each(__sk, node, list) \
	hlist_for_each_entry(__sk, node, list, sk_node)

struct inet_opt {
  uint32_t daddr;
  uint16_t dport;
  uint32_t saddr;
  uint16_t sport;
  uint16_t id;
  uint8_t ttl;
};

struct inet_sock {
  struct sock	  sk;
  struct inet_opt inet;
};

#define inet_sk(__sk) (&((struct inet_sock *)__sk)->inet)

void ip4_input(struct sk_buff *skb);
int ip_output_if_src(struct sk_buff *skb);

#endif // _IP4_H_
