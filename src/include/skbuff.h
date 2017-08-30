#ifndef _SKBUFF_H_
#define _SKBUFF_H_

#include "def.h"

#define MAX_FRAME_LEN 100 * sizeof(char)

/* The structure used to tranmit msg from different layers */
struct sk_buff{
  struct sk_buff *next;
  struct sk_buff *prev;

  struct sock *sk;

  void *tranport_header;
  void *network_header;
  void *mac_header;

  char cb[48];			// Control block to save private info

  unsigned int len;		// The data part len in SKB. Changed when SKB transmitted from layer to layer. 

  unsigned int data_len;
  unsigned int mac_len;

  unsigned int truesize;

  void *head, *data, *tail, *end;
};

struct sk_buff_head{
  struct sk_buff *next;
  struct sk_buff *prev;

  uint32_t qlen;
  // a lock is ignored as it is unithread now
};

/* I do not permit data frame with varied length, so no size parameter now */
struct sk_buff *alloc_skb();
#endif // _SKBUFF_H_
