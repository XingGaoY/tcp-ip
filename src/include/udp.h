#ifndef _UDP_H_
#define _UDP_H_

#include "ip4_addr.h"
#include "skbuff.h"

#define SIZEOF_UDP_HDR 8

struct udp_hdr{
  uint16_t src;
  uint16_t dest;
  uint16_t len;
  uint16_t chksum;
  char data[];
};

/** This is the common part of all PCB types. It needs to be at the
   beginning of a PCB type definition. It is located here so that
   changes to this common part are made in one location instead of
   having to change all PCB structs. */
#define IP_PCB \
  /* ip addresses in network byte order */ \
  struct ip4_addr local_ip; \
  struct ip4_addr remote_ip; \
   /* Socket options */  \
  uint8_t so_options;      \
   /* Type Of Service */ \
  uint8_t tos;              \
  /* Time To Live */     \
  uint8_t ttl;               


/* the UDP protocol control block */
struct udp_pcb{
  IP_PCB;
  struct udp_pcb *next;
  uint16_t local_port, remote_port;
};

/* the first pcb of the pcb list */
extern struct udp_pcb *udp_pcbs;

void udp_input(struct sk_buff *skb);

#endif // _UDP_H_
