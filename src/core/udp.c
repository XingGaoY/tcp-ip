#include "udp.h"
#include "def.h"
#include "ip4.h"

struct hlist_head udp_hash[UDP_HTABLE_SIZE];

struct sock *udp_sk_alloc(){
  struct udp_sock *udp_sk = (struct udp_sock *)malloc(sizeof(struct udp_sock));

  return &udp_sk->inet.sk;
}

static int udp_queue_rcv_skb(struct sock * sk, struct sk_buff *skb){
  skb_queue_tail(sk->sk_receive_queue, skb);

  // sk_data_ready is used to generate irq or semaphore to wake up reading function
  // omit here, the app thread will read from skb queue
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
  return 1;
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

void udp_init(){
  for(int i = 0; i < UDP_HTABLE_SIZE; i++)
    INIT_HLIST_HEAD(&udp_hash[i]);
}

struct proto udp_prot = {
  .name     =   "UDP",
  .sk_alloc =   udp_sk_alloc,
  .recvmsg  =   udp_recvmsg,
  .get_port =	udp_v4_get_port,
};
