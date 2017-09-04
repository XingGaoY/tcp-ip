#ifndef _TCP_H_
#define _TCP_H_

#include "def.h"
#include "skbuff.h"

#define SIZEOF_TCP_HDR 20

extern struct proto tcp_prot;

struct tcp_hdr{
  uint16_t src;
  uint16_t dest;
  uint32_t seq;
  uint32_t ack;
  uint16_t hdrlen_rsvd_flags;
  uint16_t wnd;
  uint16_t chksum;
  uint16_t urgp;
};

/* TCP header flags bits */
#define TCP_FIN 0x01U
#define TCP_SYN 0x02U
#define TCP_RST 0x04U
#define TCP_PSH 0x08U
#define TCP_ACK 0x10U
#define TCP_URG 0x20U
#define TCP_ECE 0x40U
#define TCP_CWR 0x80U

#define TCP_FLAGS 0x3fU

#define TCPH_HDRLEN(phdr) ((uint16_t)(lwip_htons((phdr)->hdrlen_rsvd_flags) >> 12))
#define TCPH_FLAGS(phdr)  ((uint16_t)(lwip_htons((phdr)->hdrlen_rsvd_flags) & TCP_FLAGS))

void tcp_rcv(struct sk_buff *skb);

#endif // _TCP_H_
