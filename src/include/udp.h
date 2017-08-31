#ifndef _UDP_H_
#define _UDP_H_

#include "ip4_addr.h"
#include "skbuff.h"
#include "sock.h"
#include "list.h"

#define SIZEOF_UDP_HDR 8
#define UDP_HTABLE_SIZE 10

struct udp_hdr{
  uint16_t src;
  uint16_t dest;
  uint16_t len;
  uint16_t chksum;
  char data[];
};

/* the UDP protocol control block */
struct udp_sock{
  struct inet_sock inet;
  unsigned int corkflag;	// whether pending the dgram to 64Kb before sending
};

/* the first pcb of the pcb list */
extern struct hlist_head udp_hash[UDP_HTABLE_SIZE];

void udp_init();
void udp_input(struct sk_buff *skb);

#endif // _UDP_H_
