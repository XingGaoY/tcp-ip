#include "udp.h"
#include "def.h"
#include "ip4.h"
#include "netif.h"

struct hlist_head udp_hash[UDP_HTABLE_SIZE];

struct sock *udp_sk_alloc(){
  struct udp_sock *udp_sk = (struct udp_sock *)malloc(sizeof(struct udp_sock));
  struct sock *sk = &udp_sk->inet.sk;

  pthread_spin_init(&sk->rcv_lock, PTHREAD_PROCESS_PRIVATE);
  pthread_spin_init(&sk->xmit_lock, PTHREAD_PROCESS_PRIVATE);

  sk->sk_receive_queue = (struct sk_buff_head *)malloc(sizeof(struct sk_buff_head));
  sk->sk_xmit_queue = (struct sk_buff_head *)malloc(sizeof(struct sk_buff_head));

  return sk;
}

static int udp_queue_rcv_skb(struct sock * sk, struct sk_buff *skb){
  skb_queue_tail(sk->sk_receive_queue, skb, &sk->rcv_lock);

  // sk_data_ready is used to generate irq or semaphore to wake up reading function
  // omit here, the app thread will read from skb queue
  return 0;
}

static int udp_queue_xmit_skb(struct sock * sk, struct sk_buff *skb){
  skb_queue_tail(sk->sk_xmit_queue, skb, &sk->rcv_lock);

  return 0;
}

struct sock *udp_sk_lookup(uint32_t saddr, uint16_t sport, uint32_t daddr, uint16_t dport){
  struct sock *sk, *result;
  struct hlist_node *node;
  int badness = -1;

  sk_for_each(sk, node, &udp_hash[dport & (UDP_HTABLE_SIZE - 1)]){
    struct inet_opt *inet = inet_sk(sk);
    int score = 1;
    if(inet->sport == dport){
      if (inet->saddr) {
        if (inet->saddr != daddr)
          continue;
        score+=2;
      }
      if (inet->daddr) {
        if (inet->daddr != saddr)
          continue;
        score+=2;
      }
      if (inet->dport) {
        if (inet->dport != sport)
          continue;
        score+=2;
      }
      if(score == 7) {
        result = sk;
        break;
      } else if(score > badness) {
        result = sk;
        badness = score;
      }
    }
  }

  return result;
}

static int udp_v4_get_port(struct sock *sk, unsigned short snum){
  struct hlist_head *head;
  struct hlist_node *node;
  struct sock *sk2;

  if(snum == 0){
  // If snum is not specified, we generate one
  }
  else{
    head = &udp_hash[snum &(UDP_HTABLE_SIZE - 1)];
    //we simply suppose that the sock will be removed after proc shut it down
    //so the only confliction is the snum with same mod
    sk_for_each(sk2, node, head)
      if(inet_sk(sk2)->sport == snum)
        goto fail;
  }
  head = &udp_hash[snum & (UDP_HTABLE_SIZE - 1)];
  hlist_add_head(&sk->sk_node, head);
  
  return 0;
fail:
  return -1;
}

void udp_rcv(struct sk_buff *skb){
  struct udp_hdr *udphdr;
  struct ip_hdr *iphdr;
  uint16_t sport, dport;
  uint32_t saddr, daddr;
  struct sock *sk;

  udphdr = (struct udp_hdr *)skb->data;
  iphdr = (struct ip_hdr *) skb->network_header;
  saddr = iphdr->src.addr;
  daddr = iphdr->dest.addr;
  sport = lwip_htons(udphdr->src);
  dport = lwip_htons(udphdr->dest);

  fprintf(logout, "--------------------\n");
  printf("Received an UDP packet...\n");
  fprintf(logout, "src port = %x  dest port = %x  UDP len = %x  UDP chksum = %04x\n", sport, dport, udphdr->len, udphdr->chksum);

  skb->transport_header = skb->data;
  skb_pull(skb, SIZEOF_UDP_HDR);
  /* No UDP chksum check for now, and of course do not send UDP with chksum as well */

  /* Loop through the udp_pcb, cmp the ip&port and the ip&port in pcb */
  /* and send datagram to the perfect match one or ... */

  /* UDP multicast is needed to be added */
  sk = udp_sk_lookup(saddr, sport, daddr, dport);

  if(sk){
    udp_queue_rcv_skb(sk, skb);
    //sock_put(sk);
    return;
  }

  kfree_skb(skb);

  /* Some extra check to send icmp back maybe */
    
}

int udp_recvmsg(struct sock *sk, void *buf, int len){
  struct sk_buff *skb = NULL;
  int copied;

  // Block if no data rcved in skb
  while(!skb)
    skb = skb_recv_datagram(sk);

  // the data larger then len will be dropped simply
  copied = skb->len;
  if(copied > len)
    copied = len;
  // no net addr will be copied as we do not allow different ip and port in both side

  memcpy(buf, skb->data, copied);
  kfree_skb(skb);
  skb = NULL;

  return copied;
}

// Of course, no UDP chksum generation now
//TODO No udp xmit queue, that is all the udp data not sent already will not change its dest info
int udp_sendmsg(struct sock *sk, void *buf, int len, const struct __sockaddr *_daddr){
  int retval = 0;
  int ulen = len;
  struct udp_hdr *udphdr;
  struct sk_buff *skb;
  struct inet_opt *inet;

  inet = inet_sk(sk);
  inet->daddr = _daddr->ip_addr;
  inet->dport = _daddr->port;

  // No length check now, suppose not larger than 64KB

  //TODO Check if pending now, go to do_append_data if pending
  // if(sk->pending)
  // goto do_append_data;

  ulen += SIZEOF_UDP_HDR;

  //TODO if no daddr, called connect before sending
  // get dest info from sk

  // No multicast now

  // Alloc skbuff now
  //TODO Check router to find MMU is needed
  skb = alloc_skb();
  skb_reserve(skb, MAX_UDP_HDR);
  skb_add_data(skb, buf, len);
  skb_push(skb, SIZEOF_UDP_HDR);
  skb->sk = sk;

  udphdr = skb->data;
  udphdr->src = PP_HTONS(inet->sport);
  udphdr->dest = PP_HTONS(inet->dport);
  udphdr->len = PP_HTONS(ulen);
  udphdr->chksum = 0x0000;

  retval = ip_output_if_src(skb);

  // This is only used in udp push pending frame
  //udp_queue_xmit_skb(sk, skb);
  
  return retval;
}

void udp_init(){
  for(int i = 0; i < UDP_HTABLE_SIZE; i++)
    INIT_HLIST_HEAD(&udp_hash[i]);
}

struct proto udp_prot = {
  .name     =   "UDP",
  .sk_alloc =   udp_sk_alloc,
  .sendmsg  =   udp_sendmsg,
  .recvmsg  =   udp_recvmsg,
  .get_port =	udp_v4_get_port,
};
