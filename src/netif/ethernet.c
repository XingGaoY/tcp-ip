#include"ethernet.h"
#include"etharp.h"
#include"util.h"
#include"ip4.h"

/* ethernet_input -- process received ethernet frams.
 * @param frame char* the frame received actually
 */
int ethernet_input(struct sk_buff *skb){
  //omit type check and specific prot
  //simply parse header now
  struct eth_hdr *ethhdr = (struct eth_hdr*)skb->data;
  skb->mac_header = skb->data;

  skb_pull(skb, SIZEOF_ETH_HDR);
  uint16_t type;
  
  if(lwip_htons(ethhdr->type) != ETHTYPE_IPV6){		//eliminate some rubbish frame
    fprintf(logout, "==============================\n");
    fprintf(logout, "ethernet_input:\n");
    fprintf(logout, "dest = ");
    print_eth_addr(&ethhdr->dest);
    fprintf(logout, "%8ssrc = ", "");
    print_eth_addr(&ethhdr->src);
    fprintf(logout, "%8stype = %04x\n", "", lwip_htons(ethhdr->type));
  }

  type = PP_HTONS(ethhdr->type);
  
  switch(type){
    case ETHTYPE_ARP:
      etharp_input(skb);
      break;

    case ETHTYPE_IP:
      ip4_input(skb);
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
int ethernet_output(struct sk_buff *skb, const struct eth_addr *src, const struct eth_addr *dst, uint16_t eth_type){
  struct eth_hdr *ethhdr = (struct eth_hdr*)malloc(SIZEOF_ETH_HDR);
  uint16_t eth_type_be = lwip_htons(eth_type);
  
  ethhdr->type = eth_type_be;
  ethhdr->dest = *dst;
  ethhdr->src = *src;

  skb_add_hdr(skb, ethhdr, SIZEOF_ETH_HDR);
  skb->mac_header = skb->data;

  free(ethhdr);

  /* pad to min ethernet frame length */
  if(skb->len < MIN_ETH_LEN){
    skb->tail += MIN_ETH_LEN - skb->len;
    skb->len = MIN_ETH_LEN;
  }

  netdev_xmit(skb);

  return 1;
}
