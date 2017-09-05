#include "tcp.h"
#include "util.h"
#include "ip4.h"

// Whatever, process header first
void tcp_rcv(struct sk_buff *skb){
  struct tcp_hdr *tcphdr;
  struct ip_hdr *iphdr;
  iphdr = (struct ip_hdr *)skb->network_header;

  tcphdr = (struct tcp_hdr *)skb->data;
  skb->transport_header = skb->data;

  printf("--------------------\n");
  printf("Received an TCP packet...\n");
  printf("src port = %x  dest port = %x  seq num = %x  ack num = %x  TCP hdr len = %x\nflags = %02x  win size = %x  chksum = %04x  urg ptr = %x\n", 
                                                          tcphdr->src, tcphdr->dest, 
                                                          tcphdr->seq, tcphdr->ack,
                                                          TCPH_HDRLEN(tcphdr), TCPH_FLAGS(tcphdr), 
                                                          tcphdr->wnd, tcphdr->chksum,
                                                          tcphdr->urgp);
  printf("checksum: %x\n", pseudo_chksum(skb, iphdr->src, iphdr->dest, IPH_PROTO(iphdr)));
  skb_pull(skb, SIZEOF_TCP_HDR);
}

struct proto tcp_prot = {
};
