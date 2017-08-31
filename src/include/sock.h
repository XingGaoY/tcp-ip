#ifndef _SOCK_H_
#define _SOCK_H_

#include "def.h"
#include "list.h"
struct sk_buff_head;

enum SK_SHUTDOWN{
  RCV_SHUTDOWN = 0,
  SEND_SHUTDOWN,
  SHUTDOWN_MASK
};

enum SK_NO_CHECK{
  UDP_CSUM_NOXMIT = 0,
  UDP_CSUM_NORCV,
  UDP_CSUM_DEFAULT
};

enum SK_USERLOCKS{
  SOCK_SNDBUF_LOCK = 0,
  SOCK_RCVBUF_LOCK,
  SOCK_BINDADDR_LOCK,
  SOCK_BINDPORT_LOCK
};

struct sock_common{
  int skc_family;
  struct hlist_node skc_node;
};

struct sock{
  struct sock_common __sk_common;
#define sk_family __sk_common.skc_family
#define sk_node __sk_common.skc_node
  //unsigned char sk_shutdown:2,
  //              sk_no_check:2,
  //              sk_userlocks:4;
  unsigned short sk_type;
  struct proto prot;
  struct sk_buff_head *sk_receive_queue;
  struct sk_buff_head *sk_write_queue;
};

struct inet_sock{
  struct sock sk;
  uint32_t daddr;
  uint16_t dport;
  uint32_t saddr;
  uint16_t sport;
  uint16_t id;
};

struct proto {

};

#endif // _SOCK_H_
