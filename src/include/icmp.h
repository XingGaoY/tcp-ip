#ifndef _ICMP_H_
#define _ICMP_H_

#include "sk_buff.h"

#define ICMP_ER   0    /* echo reply */
#define ICMP_ECHO 8    /* echo */

#define SIZEOF_ICMPRE_HDR 8

struct icmp_echo_hdr{
  uint8_t type;
  uint8_t code;
  uint16_t chksum;
  uint16_t id;
  uint16_t seqno;
};

void icmp_input(struct sk_buff *skb);
void icmp_er_output(struct sk_buff *skb, uint16_t id, uint16_t seqno);

#endif // _ICMP_H_
