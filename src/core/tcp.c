#include "tcp.h"
#include "util.h"
#include "ip4.h"
#include "list.h"

static struct hlist_head tcp_listen_hash[TCP_LISTEN_HTABLE_SIZE];

void tcp_output(struct sock *sk, void *buf, int len, int type);
/**
 * The tcp establish finite machine
 */
static void tcp_listen_input(struct sk_buff *skb, uint32_t daddr, uint16_t dport){
  struct tcp_opt *tcpsk;
  struct tcp_cb *tcpcb;
  struct inet_opt *inet;
  struct sock *sk = skb->sk;

  inet = inet_sk(sk);
  tcpcb = (struct tcp_cb *)skb->cb;
  tcpsk = tcp_sk(sk);
  /* Check if it is the first sync packet */
  /* Do not permit simultaneous open now */
  if((tcpcb->flags & TCP_SYN) && (tcpsk->state == LISTEN)){
    printf("The listening port received an TCP connection request...\n");
    tcpsk->dseq = tcpcb->seq;
    tcpsk->state = SYN_SENT;

    inet->dport = dport;
    inet->daddr = daddr;
    /* Send back an syn and ack dseq+1 */
    tcp_output(sk, NULL, 0, SYN_SENT);
  }
}

// Whatever, process header first
void tcp_rcv(struct sk_buff *skb){
  struct tcp_hdr *tcphdr;
  struct ip_hdr *iphdr;
  uint32_t daddr;
  uint16_t dport;
  struct tcp_cb *tcpcb;
  struct sock *sk, *res_sk = NULL;
  struct hlist_node *node;

  iphdr = (struct ip_hdr *)skb->network_header;

  tcphdr = (struct tcp_hdr *)skb->data;
  skb->transport_header = skb->data;

  dport = lwip_htons(tcphdr->dest);
  daddr = iphdr->dest;

  printf("--------------------\n");
  printf("Received an TCP packet...\n");
  printf("src port = %x  dest port = %x  seq num = %x  ack num = %x  TCP hdr len = %x\nflags = %02x  win size = %x  chksum = %04x  urg ptr = %x\n", 
                                                          tcphdr->src, tcphdr->dest, 
                                                          tcphdr->seq, tcphdr->ack,
                                                          TCPH_HDRLEN(tcphdr), TCPH_FLAGS(tcphdr), 
                                                          tcphdr->wnd, tcphdr->chksum,
                                                          tcphdr->urgp);
  printf("checksum: %x\n", pseudo_chksum(skb, iphdr->src, daddr, IPH_PROTO(iphdr)));
  /*TODO Also, some check of tcp header is needed to be put here */
  skb_pull(skb, SIZEOF_TCP_HDR);
  /* Save the seq and ack of the skbuff into cb in the skbuff*/
  tcpcb = (struct tcp_cb *)skb->cb;
  tcpcb->seq = PP_HTONL(tcphdr->seq);
  tcpcb->ack = PP_HTONL(tcphdr->ack);
  tcpcb->flags = TCPH_FLAGS(tcphdr);

  /**
   * In lwip, demultiplex is processed by first search active connection list,
   * then TIME-WAIT list, and listen list finally
   */
  
  /* search the listen hash to find a match  */
  sk_for_each(sk, node, &tcp_listen_hash[dport & (TCP_LISTEN_HTABLE_SIZE - 1)]){
    struct inet_opt *inet = inet_sk(sk);
    // only permit exact match now
    if(inet->sport == dport && inet->saddr == daddr){
      res_sk = sk;
      break;
    }
  }
  
  if(res_sk != NULL){
    skb->sk = res_sk;
    tcp_listen_input(skb, daddr, dport);
  }
  else
    return;
}

/* alloc skbuff and add tcp header */
void tcp_output(struct sock *sk, void *buf, int len, int type){
  struct sk_buff *skb;
  struct tcp_hdr *tcphdr;
  struct inet_opt *inet = inet_sk(sk);
  struct tcp_opt *tcpsk = tcp_sk(sk);

  skb = alloc_skb();
  skb_reserve(skb, MAX_TCP_HDR);
  skb_add_data(skb, buf, len);
  skb_push(skb, SIZEOF_TCP_HDR);
  skb->transport_header = skb->data;
  skb->sk = sk;

  tcphdr = (struct tcp_hdr *)skb->data;
  tcphdr->src = PP_HTONS(inet->sport);
  tcphdr->dest = PP_HTONS(inet->dport);
  tcphdr->seq = PP_HTONL(tcpsk->sseq);
  tcphdr->ack = PP_HTONL(tcpsk->dseq + 1);
  tcphdr->wnd = PP_HTONS(0x1000);	//irrelevant now, set to 4096 temperarily

  switch(type){
    case SYN_SENT:
      TCPH_HDRLEN_FLAGS_SET(tcphdr, SIZEOF_TCP_HDR, TCP_SYN|TCP_ACK);
      break;
    default:
      break;
  }

  tcphdr->chksum = pseudo_chksum(skb, inet->saddr, inet->daddr, sk->sk_type);

  ip_output_if_src(skb);
}

struct proto tcp_prot = {

};

void tcp_init(){
  for(int i = 0; i < TCP_LISTEN_HTABLE_SIZE; i++)
    INIT_HLIST_HEAD(&tcp_listen_hash[i]);
}
