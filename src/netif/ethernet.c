#include"ethernet.h"
#include"etharp.h"
#include"util.h"

/* ethernet_input -- process received ethernet frams.
 * @param frame char* the frame received actually
 */
int ethernet_input(char *frame){
  //omit type check and specific prot
  //simply parse header now
  struct eth_hdr *ethhdr = (struct eth_hdr*)frame;
  uint16_t type;

  printf("ethernet_input:\n");
  printf("dest:");
  print_eth_addr(&ethhdr->dest);
  printf("src:");
  print_eth_addr(&ethhdr->src);
  printf("type:%16x\n", lwip_htons(ethhdr->type));

  type = PP_HTONS(ethhdr->type);
  
  switch(type){
    case ETHTYPE_ARP:
      etharp_input(ethhdr->payload);
      break;
   
    default:
      break;
  }

  return 1;
}

/* ethernet_output -- fill in the ethernet header, before sending the frame.
 * @param payload the packet to send.
 * @param src the source MAC address to be copied into the ethernet header
 * @param dst the destination MAC address to be copied into the ethernet header
 * @param eth_type ethernet type (@ref eth_type)
 * @return 1 if the packet was sent, -1 on failure
 */
int ethernet_output(struct eth_hdr *frame, const struct eth_addr *src, const struct eth_addr *dst, uint16_t eth_type, int sizeof_frame){
  struct eth_hdr *ethhdr = (struct eth_hdr*)frame;
  uint16_t eth_type_be = lwip_htons(eth_type);
  
  ethhdr->type = eth_type_be;
  ethhdr->dest = *dst;
  ethhdr->src = *src;

  netdev_xmit((char*)frame, sizeof_frame);

  return 1;
}
