#include "skbuff.h"

struct sk_buff *alloc_skb(){
  struct sk_buff *skb;
  void *data;

  skb = (struct sk_buff *)malloc(sizeof(struct sk_buff));
  data = malloc(MAX_FRAME_LEN);

  memset(skb, 0, sizeof(struct sk_buff));
  memset(data, 0, MAX_FRAME_LEN);

  skb->truesize = MAX_FRAME_LEN + sizeof(struct sk_buff);
  skb->head = data;
  skb->data = data;
  skb->tail = data;
  skb->end = data + MAX_FRAME_LEN;

  return skb;
}
