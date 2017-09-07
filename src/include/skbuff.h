#ifndef _SKBUFF_H_
#define _SKBUFF_H_

#include "def.h"
#include "sock.h"

/* The structure used to tranmit msg from different layers */
struct sk_buff{
  struct sk_buff *next;
  struct sk_buff *prev;

  struct sk_buff_head *list;

  struct sock *sk;

  int ip_proto;

  uint32_t saddr;
  uint16_t sport;
  uint32_t daddr;
  uint16_t dport;

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

void skb_reserve(struct sk_buff *skb, int len);
void *skb_push(struct sk_buff *skb, int len);
void *skb_pull(struct sk_buff *skb, int len);
void *skb_add_data(struct sk_buff *skb, void *data, int len);

/* I do not permit data frame with varied length, so no size parameter now */
struct sk_buff *alloc_skb();
void kfree_skb(struct sk_buff *skb);

void skb_queue_head_init(struct sk_buff_head *list);

struct sk_buff *skb_recv_datagram(struct sock *sk);

static inline struct sk_buff *skb_queue_front(struct sk_buff_head *list){
  struct sk_buff *next, *prev, *result;
  prev = (struct sk_buff *)list;
  next = prev->next;
  result = NULL;
  if(next != prev)
    result = next;
  return result;
}

static inline struct sk_buff *skb_dequeue(struct sk_buff_head *list, pthread_spinlock_t *lock)
{
  struct sk_buff *next, *prev, *result;

  pthread_spin_lock(lock);
  {
    prev = (struct sk_buff *) list;
    next = prev->next;
    result = NULL;
    if (next != prev) {
      result       = next;
      next	 = next->next;
      list->qlen--;
      next->prev   = prev;
      prev->next   = next;
      result->next = result->prev = NULL;
      result->list = NULL;
    }
  }
  pthread_spin_unlock(lock);
  return result;
}

//TODO The lock for the list may be needed
static inline void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk, 
                                  pthread_spinlock_t *lock)
{
  struct sk_buff *prev, *next;

  pthread_spin_lock(lock);
  {
    newsk->list = list;
    list->qlen++;
    next = (struct sk_buff *)list;
    prev = next->prev;
    newsk->next = next;
    newsk->prev = prev;
    next->prev  = prev->next = newsk;
  }
  pthread_spin_unlock(lock);
}

#endif // _SKBUFF_H_
