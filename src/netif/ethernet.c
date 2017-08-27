#include"ethernet.h"
#include"etharp.h"
#include"util.h"

struct eth_addr netif_hwaddr;
struct ip4_addr netif_ip;

/* ethernet_input -- process received ethernet frams.
 * @param buf char* the frame received actually
 */
int ethernet_input(char *buf){
  //omit type check and specific prot
  //simply parse header now
  struct eth_hdr *ethhdr = (struct eth_hdr*)buf;
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
