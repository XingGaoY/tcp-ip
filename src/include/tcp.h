#ifndef _TCP_H_
#define _TCP_H_

#include "def.h"
#include "skbuff.h"
#include "sock.h"
#include "ip4.h"

#define SIZEOF_TCP_HDR 20
#define TCP_LISTEN_HTABLE_SIZE 10
#define MAX_TCP_HDR MAX_IP_HDR + SIZEOF_TCP_HDR

/**
 * Here, I just implement a simple tcp server with only three-handshake
 * which request the order socket(), bind() and then listen()
 * listen() will register in the sync_hlist and call the tcp_sync
 * procedure to finish setting up connection
 *
 * The implementation just imitate upd recv queue.
 */

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

/* TCP control block */
struct tcp_cb{
  uint8_t flags;
  uint32_t seq;
  uint32_t ack;
};

/* TCP protocol control block */
struct tcp_opt {
  int state :6;
  uint32_t sseq;
  uint32_t dseq;
};

struct tcp_sock {
  struct inet_sock inet;
  struct tcp_opt tcp;
};

#define tcp_sk(__sk) (&((struct tcp_sock *)__sk)->tcp)

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

#define TCPH_HDRLEN_FLAGS_SET(phdr, len, flags) (phdr)->hdrlen_rsvd_flags = (uint16_t)(lwip_htons((uint16_t)((len) << 12) | (flags)))

/* tcp state */
enum tcp_state{
  LISTEN = 0,
  SYN_SENT = 1,
  SYN_RECV = 2,
};

void tcp_init();

void tcp_rcv(struct sk_buff *skb);

#endif // _TCP_H_
