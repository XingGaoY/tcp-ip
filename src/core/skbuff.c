#include "skbuff.h"

void *skb_add_hdr(struct sk_buff *skb, void *hdr, int len){  
  memcpy(skb_push(skb, len), hdr, len);
  return skb->data;
}

void *skb_push(struct sk_buff *skb, int len){
  skb->data -= len;
  skb->len += len;

  if(skb->data < skb->head){
    printf("Data exceed the upper limit of data head\n");
  }

  return skb->data;
}

void *skb_pull(struct sk_buff *skb, int len){
  skb->data += len;
  skb->len -= len;

  if(skb->data > skb->tail){
    printf("No data in skb data field\n");
  }

  return skb->data;
}

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
