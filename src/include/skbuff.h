#ifndef _SKBUFF_H_
#define _SKBUFF_H_

#include "def.h"
#include "sock.h"

/* The structure used to tranmit msg from different layers */
struct sk_buff{
  struct sk_buff *next;
  struct sk_buff *prev;

  struct sock *sk;

  void *transport_header;
  void *network_header;
  void *mac_header;

  char cb[48];			// Control block to save private info

  unsigned int len;		// The data part len in SKB. Changed when SKB transmitted from layer to layer. 

  void *head, *data, *tail, *end;
};

struct sk_buff_head{
  struct sk_buff *next;
  struct sk_buff *prev;

  uint32_t qlen;
  // a lock is ignored as it is unithread now
};

void *skb_push(struct sk_buff *skb, int len);
void *skb_pull(struct sk_buff *skb, int len);
void *skb_add_hdr(struct sk_buff *skb, void *hdr, int len);
void *skb_add_data(struct sk_buff *skb, void *data, int len);

/* I do not permit data frame with varied length, so no size parameter now */
struct sk_buff *alloc_skb();
void kfree_skb(struct sk_buff *skb);
#endif // _SKBUFF_H_
