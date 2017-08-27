#include"etharp.h"
#include"util.h"

/** Try hard to create a new entry - we want the IP address to appear in
    the cache (even if this means removing an active entry or so). */
#define ETHARP_FLAG_TRY_HARD     1
#define ETHARP_FLAG_FIND_ONLY    2

/*
 * Update (or insert) a IP/MAC address pair in the ARP cache.
 */
void etharp_update_arp_entry(const struct ip4_addr *ipaddr, struct eth_addr *ethaddr, uint8_t flags){

}

/**
 * Send a raw ARP packet (opcode and all addresses can be modified)
 */
void
etharp_raw(const struct eth_addr *ethsrc_addr, const struct eth_addr *ethdst_addr,
           const struct eth_addr *hwsrc_addr, const struct ip4_addr *ipsrc_addr,
           const struct eth_addr *hwdst_addr, const struct ip4_addr *ipdst_addr,
           const uint16_t opcode){
}

void etharp_input(char *payload){
  struct etharp_hdr *hdr;
  struct ip4_addr sipaddr, dipaddr;

  /* Simply guess the ARP is for us */
  uint8_t for_us = 1;

  /* Parse the ARP packet */
  hdr = (struct etharp_hdr *)payload;

#ifdef verbose
  printf("Incoming ARP.\n");
  printf("hwtype: %16x\nproto: %16x\nhwlen: %8x\nprotolen: %8x\nopcode: %16x\n", (unsigned int)PP_HTONS(hdr->hwtype), (unsigned int)PP_HTONS(hdr->proto), hdr->hwlen, hdr->protolen, (unsigned int)PP_HTONS(hdr->opcode));
  printf("src's hwaddr:");
  print_eth_addr(&hdr->shwaddr);
  printf("src's ipaddr:");
  print_ip_addr2(&hdr->sipaddr);
  printf("dest's hwaddr:");
  print_eth_addr(&hdr->dhwaddr);
  printf("dest's ipaddr:");
  print_ip_addr2(&hdr->dipaddr);
#endif

  /* Copy struct ip4_addr2 to aligned ip4_addr, to support compilers without
   * structure packing (not using structure copy which breaks strict-aliasing rules). */
  IPADDR2_COPY(&sipaddr, &hdr->sipaddr);
  IPADDR2_COPY(&dipaddr, &hdr->dipaddr);

  /* ARP message directed to us?
      -> add IP address in ARP cache; assume requester wants to talk to us,
         can result in directly sending the queued packets for this host.
     ARP message not directed to us?
      ->  update the source IP address in the cache, if present */
  etharp_update_arp_entry(&sipaddr, &(hdr->shwaddr),
                   for_us ? ETHARP_FLAG_TRY_HARD : ETHARP_FLAG_FIND_ONLY);

  /* Now work on the message */
  switch(hdr->opcode){
  /* ARP request */
    case PP_HTONS(ARP_REQUEST):
    /* ARP request. If it asked for our address, we send out a
     * reply. In any case, we time-stamp any existing ARP entry,
     * and possibly send out an IP packet that was queued on it. */
      if(for_us){
      printf("ARP for us, sending a response...");
        /* send ARP response */
      etharp_raw(&netif_hwaddr, &hdr->shwaddr, &netif_hwaddr, &netif_ip, &hdr->shwaddr, &sipaddr, ARP_REPLY);
      }
      else
        /* ignore the unconfigured condition */
        printf("ARP request not for us, ignored");
      break;
    default:
      break;
  }
}
