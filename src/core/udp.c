#include "udp.h"
#include "def.h"

struct hlist_head udp_hash[UDP_HTABLE_SIZE];

void udp_input(struct sk_buff *skb){
  struct udp_hdr *udphdr;
  uint16_t src, dest;
  struct udp_sock udpsock;

  udphdr = (struct udp_hdr *)skb->data;
  src = lwip_htons(udphdr->src);
  dest = lwip_htons(udphdr->dest);

  fprintf(logout, "--------------------\n");
  printf("Received an UDP packet...\n");
  fprintf(logout, "src port = %x  dest port = %x  UDP len = %x  UDP chksum = %04x\n", src, dest, udphdr->len, udphdr->chksum);

  skb->transport_header = skb->data;
  skb_pull(skb, SIZEOF_UDP_HDR);

  /* No UDP chksum check for now, and of course do not send UDP with chksum as well */

  /* Loop through the udp_pcb, cmp the ip&port and the ip&port in pcb */
  /* and send datagram to the perfect match one or ... */

  /* UDP multicast is needed to be added */

}
