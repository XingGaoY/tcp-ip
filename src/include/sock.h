#ifndef _SOCK_H_
#define _SOCK_H_

#include "def.h"
#include "list.h"
#include "socket.h"

struct sk_buff_head;
struct socket;

struct proto;

struct sock{
  struct hlist_node sk_node;
  unsigned short sk_type;
  struct proto *sk_prot;
  struct sk_buff_head *sk_receive_queue;
  struct sk_buff_head *sk_write_queue;
};

struct proto {
  char name[10];
  struct sock *(*sk_alloc)();
  int (*bind)(struct sock *sk, struct __sockaddr *uaddr, int addr_len);
  int (*recvmsg)(struct sock *sk, void *buf, int len);
  int (*get_port)(struct sock *sk, unsigned short sport);
};

#endif // _SOCK_H_
