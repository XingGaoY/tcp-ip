#ifndef _SOCK_H_
#define _SOCK_H_

#include "def.h"
#include "list.h"
#include "socket.h"
#include <pthread.h>

/* sock state */
enum sock_state{
  SK_LISTEN = 0,
  SK_CONNECTED,

  SK_CLOSED = 255,
};

struct sk_buff_head;
struct socket;

struct proto;

struct sock{
  struct hlist_node sk_node;
  unsigned short sk_type;
  struct proto *sk_prot;
  int sk_state;

  pthread_spinlock_t rcv_lock;
  pthread_spinlock_t xmit_lock;
  struct sk_buff_head *sk_receive_queue;
  struct sk_buff_head *sk_xmit_queue;
};

struct proto {
  char name[10];
  struct sock *    (*sk_alloc)       ();
  int              (*listen)	     (struct sock *sk);
  int              (*recvmsg)        (struct sock *sk, void *buf, int len);
  int              (*sendmsg)     (struct sock *sk, void *buf, int len, 
                                   const struct __sockaddr *_daddr);
  int              (*get_port)    (struct sock *sk, unsigned short sport);
};

#endif // _SOCK_H_
