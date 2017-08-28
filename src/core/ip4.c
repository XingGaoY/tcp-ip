#include "ip4.h"

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
  printf("Received an IP datagram");
}
