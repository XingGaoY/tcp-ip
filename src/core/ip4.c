#include "ip4.h"
#include "util.h"
#include "icmp.h"
#include "etharp.h"

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
  uint16_t iphdr_hlen;

  iphdr = (struct ip_hdr*)p;

  printf("-----\n");
  printf("Incoming an IP datagram...\n");
  printf("version = %01x  header length = %01x  TOS = %02x  total length = %04x  identification = %04x\noffset = %04x  TTL = %02x  proto = %02x  header checksum = %04x\n", 
							IPH_V(iphdr),
							IPH_HL(iphdr),
							IPH_TOS(iphdr),
							(unsigned int)PP_HTONS(IPH_LEN(iphdr)),
							IPH_ID(iphdr),
							IPH_OFFSET(iphdr),
							IPH_TTL(iphdr),
							IPH_PROTO(iphdr),
							IPH_CHKSUM(iphdr));
  printf("src's ip = ");
  print_ip_addr(&iphdr->src);
  printf("\tdst's ip = ");
  print_ip_addr(&iphdr->dest);

  iphdr_hlen = IPH_HL(iphdr);
  iphdr_hlen *= 4;

  /* Here comes some checks of header
     we only do the checksum check now */
  printf("\nchecksum calculated: %02x\n", checksum(iphdr, iphdr_hlen));
  /* If checksum not in consistence, drop the datagram silently */
  if(checksum(iphdr, iphdr_hlen) != 0)
    return;

  /* Send to upper layers */
  switch(IPH_PROTO(iphdr)){
    case IP_PROTO_ICMP:
      icmp_input(p);
  }
}

void ip4_output(char *p, struct ip4_addr src, struct ip4_addr dst){
  struct ip_hdr *iphdr = (struct ip_hdr*)p;
 
  iphdr->_ttl = IPH_TTL(iphdr) - 1;
  iphdr->src = src;
  iphdr->dest = dst;

  iphdr->_chksum = 0;
  iphdr->_chksum = checksum(iphdr, IPH_HL(iphdr)*4);
  etharp_output(p, &dst, (unsigned int)PP_HTONS(IPH_LEN(iphdr)));
}
