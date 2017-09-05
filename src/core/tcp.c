#include "tcp.h"
#include "util.h"
#include "ip4.h"
#include "list.h"

#define TCP_BHASH_SIZE 10
#define TCP_EHASH_SIZE 10

struct bind_bucket{
  uint16_t port;
  struct hlist_node bind_node;
  struct sock *sk;
};
#define bbucket_for_each(__bbucket, node, list) \
	hlist_for_each_entry(__bbucket, node, list, bind_node)

static struct hlist_head tcp_lhash[TCP_LISTEN_HTABLE_SIZE];
static struct hlist_head tcp_bhash[TCP_BHASH_SIZE];
static struct hlist_head tcp_ehash[TCP_EHASH_SIZE];

struct sock *tcp_sk_alloc(){
  struct tcp_sock *tcp_sk = (struct tcp_sock *)malloc(sizeof(struct tcp_sock));
  return &tcp_sk->inet.sk;
}

void tcp_output(struct sock *sk, void *buf, int len, int type);
/**
 * The tcp establish finite machine
 */
static void tcp_listen_input(struct sk_buff *skb){
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

    inet->dport = skb->sport;
    inet->daddr = skb->saddr;
    /* Send back an syn and ack dseq+1 */
    tcp_output(sk, NULL, 0, SYN_SENT);
  }
}

//This function is exactly same with udp_v4_get_port
//But "maybe" I will add some more utils so just don't coalesce
static int tcp_v4_get_port(struct sock *sk, unsigned short snum){
  struct bind_bucket new_bbucket, *bbucket;
/**
 * Similar with udp, in linux, the port is organized in a hash list
 * we do not permit port reuse and the the port num will be removed
 * after the sock disconnected
 */
  struct hlist_head *head;
  struct hlist_node *node;

  if(snum == 0){
  // we are suppose to generate one, more complex in tcp than udp
  }
  else{
    head = &tcp_bhash[snum &(TCP_BHASH_SIZE - 1)];
    bbucket_for_each(bbucket, node, head)
      if(bbucket->port == snum)
        goto fail;
  }

  new_bbucket.port = snum;
  new_bbucket.sk = sk;

  head = &tcp_bhash[snum &(TCP_BHASH_SIZE -1)];
  hlist_add_head(&new_bbucket.bind_node, head);

  /* finish binding, these lines should not be here, 
   * as listen will need to get port as well */
  struct tcp_opt *tcpsk = tcp_sk(sk);
  tcpsk->state = CLOSED;
  tcpsk->sseq = 0;
  tcpsk->dseq = 0;

  return 0;
fail:
  return 1;
}

static struct sock *tcp_lookup_ebhash(struct sk_buff *skb){
  struct hlist_node *node;
  struct sock *sk, *res_sk = NULL;

  // lookup lhash
  sk_for_each(sk, node, &tcp_lhash[skb->sport & (TCP_LISTEN_HTABLE_SIZE - 1)]){
    struct inet_opt *inet = inet_sk(sk);
    // only permit exact match now
    if(inet->sport == skb->sport && inet->saddr == skb->saddr){
      res_sk = sk;
      return res_sk;
    }
  }

  // lookup bhash
  sk_for_each(sk, node, &tcp_ehash[skb->sport & (TCP_LISTEN_HTABLE_SIZE - 1)]){
    struct inet_opt *inet = inet_sk(sk);
    // only permit exact match now
    if(inet->sport == skb->sport && inet->saddr == skb->saddr){
      res_sk = sk;
      return res_sk;
    }
  }
  return res_sk;
}

static int tcp_v4_do_rcv(struct sk_buff *skb){
  struct tcp_opt *tcpsk = tcp_sk(skb->sk);

  switch(tcpsk->state){
    case(LISTEN):
      tcp_listen_input(skb);
      break;
  }
  return 1;
}

// Whatever, process header first
void tcp_rcv(struct sk_buff *skb){
  struct tcp_hdr *tcphdr;
  struct ip_hdr *iphdr;
  uint32_t daddr;
  uint16_t dport;
  struct tcp_cb *tcpcb;
  struct sock *res_sk = NULL;

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

  skb->saddr = daddr;
  skb->sport = dport;

  /**
   * In lwip, demultiplex is processed by first search active connection list,
   * then TIME-WAIT list, and listen list finally
   */
  
  /* search the listen hash to find a match  */
  res_sk = tcp_lookup_ebhash(skb);
  
  if(res_sk != NULL){
    skb->sk = res_sk;
    tcp_v4_do_rcv(skb);
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

void tcp_init(){
  for(int i = 0; i < TCP_LISTEN_HTABLE_SIZE; i++)
    INIT_HLIST_HEAD(&tcp_lhash[i]);

  for(int i = 0; i < TCP_BHASH_SIZE; i++)
    INIT_HLIST_HEAD(&tcp_bhash[i]);
}

struct proto tcp_prot = {
  .name     =    "TCP",
  .sk_alloc =    tcp_sk_alloc,
  .get_port =    tcp_v4_get_port
};
