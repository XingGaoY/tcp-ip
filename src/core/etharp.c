//TODO timer to update arp table

#include "etharp.h"
#include "util.h"

/** ARP states */
enum etharp_state{
  ETHARP_STATE_EMPTY = 0,
  ETHARP_STATE_STABLE
};

struct etharp_entry{
  struct ip4_addr ipaddr;
  struct eth_addr ethaddr;

  int state;
};

static struct etharp_entry arp_table[ARP_TABLE_SIZE];

/**
 * Search the ARP table for a matching or new table.
 */
static int etharp_find_entry(const struct ip4_addr *ipaddr){
  int i = 0, empty = ARP_TABLE_SIZE;
  /**
   * Trying to match the ipaddr
   * As we omit the arp entry state other than exist or not
   * No state check here 
   */
  for(i = 0; i < ARP_TABLE_SIZE; i++){
    int state = arp_table[i].state;
    
    if((empty == ARP_TABLE_SIZE) && (state == ETHARP_STATE_EMPTY))
      empty = i;
    else if(state != ETHARP_STATE_EMPTY){
      if(ipaddr && ipaddr->addr == arp_table[i].ipaddr.addr){
         fprintf(logout, "Found arp entry index: %d\n", i);
         return i;
      }
    }
  } 
 
  /* And now empty is the smallest index of empty entry */
  /* And we need to find a new one */
  if(empty < ARP_TABLE_SIZE)
    i = empty;
  else{
  /* An entry with old state is supposed to be discarded */
    printf("Not enough room for new art entry\n");
    return -1;
  }

  fprintf(logout, "New arp entry index: %d\n", i);
  arp_table[i].state = ETHARP_STATE_EMPTY;
  if(ipaddr)
    memcpy(&arp_table[i].ipaddr, ipaddr, sizeof(struct ip4_addr));
  return i;
}

/*
 * Update (or insert) a IP/MAC address pair in the ARP cache.
 */
void etharp_update_arp_entry(const struct ip4_addr *ipaddr, struct eth_addr *ethaddr){
  int i;
  fprintf(logout, "Updating arp table...\n");

  //TODO some check of ip addr type is omitted

  /* find or create ARP entry */
  i = etharp_find_entry(ipaddr);

  if(i < 0) perror("Unable to update an arp entry");

  arp_table[i].state = ETHARP_STATE_STABLE;
  memcpy(&arp_table[i].ethaddr, ethaddr, sizeof(struct eth_addr));
}

/**
 * Send a raw ARP packet (opcode and all addresses can be modified)
 */
void
etharp_raw(const struct eth_addr *ethsrc_addr, const struct eth_addr *ethdst_addr,
           const struct eth_addr *hwsrc_addr, const struct ip4_addr *ipsrc_addr,
           const struct eth_addr *hwdst_addr, const struct ip4_addr *ipdst_addr,
           const uint16_t opcode){
  struct sk_buff *skb;
  struct etharp_hdr *hdr;

  skb = alloc_skb();
  skb->data += MAX_ETHARP_HDR;

  hdr = (struct etharp_hdr *)malloc(SIZEOF_ETHARP_HDR);
  hdr->hwtype = PP_HTONS(HWTYPE_ETHERNET);
  hdr->proto = PP_HTONS(ETHTYPE_IP);
  hdr->hwlen = ETH_HWADDR_LEN;
  hdr->protolen = sizeof(struct ip4_addr);
  hdr->opcode = PP_HTONS(opcode);
  hdr->shwaddr = *hwsrc_addr;
  hdr->dhwaddr = *hwdst_addr;
  IPADDR2_COPY(&hdr->sipaddr, ipsrc_addr);
  IPADDR2_COPY(&hdr->dipaddr, ipdst_addr);

  skb_add_data(skb, hdr, SIZEOF_ETHARP_HDR);
  free(hdr);
  hdr = NULL;

  ethernet_output(skb, ethsrc_addr, ethdst_addr, ETHTYPE_ARP);
}

void etharp_input(struct sk_buff *skb){
  struct etharp_hdr *hdr;
  struct ip4_addr sipaddr, dipaddr;
  struct eth_addr shwaddr;

  /* Simply guess the ARP is for us */
  uint8_t for_us = 1;

  /* Parse the ARP packet */
  hdr = (struct etharp_hdr *)skb->data;

#ifdef verbose
  fprintf(logout, "-----\n");
  fprintf(logout, "Incoming ARP...\n");
  fprintf(logout, "hwtype = %x\tproto = %04x\thwlen = %x\tprotolen = %x\topcode = %x\n", (unsigned int)PP_HTONS(hdr->hwtype), (unsigned int)PP_HTONS(hdr->proto), hdr->hwlen, hdr->protolen, (unsigned int)PP_HTONS(hdr->opcode));
  fprintf(logout, "src's hwaddr = ");
  print_eth_addr(&hdr->shwaddr);
  fprintf(logout, "%8ssrc's ipaddr = ", "");
  print_ip_addr2(&hdr->sipaddr);
  fprintf(logout, "\ndest's hwaddr = ");
  print_eth_addr(&hdr->dhwaddr);
  fprintf(logout, "%8sdest's ipaddr = ", "");
  print_ip_addr2(&hdr->dipaddr);
  fprintf(logout, "\n");
#endif

  /* Copy struct ip4_addr2 to aligned ip4_addr, to support compilers without
   * structure packing (not using structure copy which breaks strict-aliasing rules). */
  IPADDR2_COPY(&sipaddr, &hdr->sipaddr);
  IPADDR2_COPY(&dipaddr, &hdr->dipaddr);
  shwaddr = hdr->shwaddr;

  /* ARP message directed to us?
      -> add IP address in ARP cache; assume requester wants to talk to us,
         can result in directly sending the queued packets for this host.
     ARP message not directed to us?
      ->  update the source IP address in the cache, if present */
  etharp_update_arp_entry(&sipaddr, &shwaddr);

  kfree_skb(skb);
  skb = NULL;

  /* Now work on the message */
  switch(hdr->opcode){
  /* ARP request */
    case PP_HTONS(ARP_REQUEST):
    /* ARP request. If it asked for our address, we send out a
     * reply. In any case, we time-stamp any existing ARP entry,
     * and possibly send out an IP packet that was queued on it. */
      if(for_us){
      fprintf(logout, "ARP for us, sending a response...\n");
        /* send ARP response */
      etharp_raw(&netif->hwaddr, &shwaddr, &netif->hwaddr, &netif->ipaddr, &shwaddr, &sipaddr, ARP_REPLY);
      }
      else
        /* ignore the unconfigured condition */
        fprintf(logout, "ARP request not for us, ignored");
      break;
    default:
      break;
  }
}

void etharp_output(struct sk_buff *skb, struct ip4_addr *dst){
  struct eth_addr *ethsrc_addr, *ethdst_addr;

  /* unicast or muilticast need to be checked */
  /* and check ipaddr outside local network or not is necessary */
  /* as we only use TAP, we simply check arp table instead */
  for(int i = 0; i < ARP_TABLE_SIZE; i++){
    if(dst->addr == arp_table[i].ipaddr.addr){
      ethdst_addr = &arp_table[i].ethaddr;

      fprintf(logout, "Found the dst hwaddr in ARP cache: ");
      print_eth_addr(ethdst_addr);
      fprintf(logout, "\n");
    }
  }
  ethsrc_addr = &netif->hwaddr;
  
  ethernet_output(skb, ethsrc_addr, ethdst_addr, ETHTYPE_IP);
}
