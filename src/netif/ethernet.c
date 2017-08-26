#include"ethernet.h"
#include<stdio.h>

#define PP_HTONS(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))

#if !defined(lwip_htons)
/* lwip_htons -- Convert an u16_t from host- to network byte order.
 * @param n u16_t in host byte order
 * @return n in network byte order
 */
uint16_t
lwip_htons(uint16_t n)
{
	  return (uint16_t)PP_HTONS(n);
}
#endif /* lwip_htons */

/* ethernet_input -- process received ethernet frams.
 * @param buf char* the frame received actually
 */
int ethernet_input(char *buf){
  //omit type check and specific prot
  //simply parse header now
  struct eth_hdr *ethhdr = (struct eth_hdr*)buf;
  printf("ethernet_input:\ndest:%8x:%8x:%8x:%8x:%8x:%8x\nsrc:%8x:%8x:%8x:%8x:%8x:%8x\ntype:%16x\n", (unsigned)ethhdr->dest.addr[0], (unsigned)ethhdr->dest.addr[1], (unsigned)ethhdr->dest.addr[2],(unsigned)ethhdr->dest.addr[3], (unsigned)ethhdr->dest.addr[4], (unsigned)ethhdr->dest.addr[5], (unsigned)ethhdr->src.addr[0],  (unsigned)ethhdr->src.addr[1],  (unsigned)ethhdr->src.addr[2], (unsigned)ethhdr->src.addr[3],  (unsigned)ethhdr->src.addr[4],  (unsigned)ethhdr->src.addr[5], lwip_htons(ethhdr->type));
  
  return 1;
}
