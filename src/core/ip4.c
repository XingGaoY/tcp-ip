#include "ip4.h"
#include "util.h"

/**
 * This function is called by the network interface device driver when
 * an IP packet is received. The function does the basic checks of the
 * IP header such as packet size being at least larger than the header
 * size etc. If the packet was not destined for us, the packet is
 * forwarded (using ip_forward). The IP checksum is always checked.
 *
 * Finally, the packet is sent to the upper layer protocol input function.
 *
 * @param p the received IP packet (p->payload points to IP header)
 */
void ip4_input(char *p){
  struct ip_hdr *iphdr;

  iphdr = (struct ip_hdr*)p;
  printf("Received an IP datagram:\n");

  printf("version: %01x\nheader length: %01x\nTOS: %02x\ntotal length: %04x\nidentification: %04x\noffset: %04x\nTTL: %02x\nproto: %02x\nheader checksum: %04x\n", 
							IPH_V(iphdr),
							IPH_HL(iphdr),
							IPH_TOS(iphdr),
							IPH_LEN(iphdr),
							IPH_ID(iphdr),
							IPH_OFFSET(iphdr),
							IPH_TTL(iphdr),
							IPH_PROTO(iphdr),
							IPH_CHKSUM(iphdr));
  printf("src's ip:");
  print_ip_addr(&iphdr->src);
  printf("dst's ip:");
  print_ip_addr(&iphdr->dest);

  /* Here comes some check of header
     we only do the checksum check now */


  /* Send to upper layers */
}
