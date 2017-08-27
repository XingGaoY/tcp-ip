//TODO timer to update arp table

#include"etharp.h"
#include"util.h"

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
         printf("Found arp entry index: %d\n", i);
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
    perror("Not enough room for new art entry\n");
    return -1;
  }

  printf("New arp entry index: %d\n", i);
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
  printf("Updating arp table...\n");

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
  struct eth_hdr *frame;
  struct etharp_hdr *hdr;
  int len;
  len = SIZEOF_ETH_HDR + SIZEOF_ETHARP_HDR;
  char f[len];

  frame = (struct eth_hdr *)f;
  hdr = (struct etharp_hdr *)frame->payload;

  hdr->opcode = PP_HTONS(opcode);
  hdr->hwtype = PP_HTONS(HWTYPE_ETHERNET);
  hdr->proto = PP_HTONS(ETHTYPE_IP);
  hdr->hwlen = ETH_HWADDR_LEN;
  hdr->protolen = sizeof(struct ip4_addr);

  hdr->shwaddr = *hwsrc_addr;
  hdr->dhwaddr = *hwdst_addr;
  IPADDR2_COPY(&hdr->sipaddr, ipsrc_addr);
  IPADDR2_COPY(&hdr->dipaddr, ipdst_addr);

  ethernet_output(frame, ethsrc_addr, ethdst_addr, ETHTYPE_ARP, len);
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
  etharp_update_arp_entry(&sipaddr, &(hdr->shwaddr));

  /* Now work on the message */
  switch(hdr->opcode){
  /* ARP request */
    case PP_HTONS(ARP_REQUEST):
    /* ARP request. If it asked for our address, we send out a
     * reply. In any case, we time-stamp any existing ARP entry,
     * and possibly send out an IP packet that was queued on it. */
      if(for_us){
      printf("ARP for us, sending a response...\n");
        /* send ARP response */
      etharp_raw(&netif->hwaddr, &hdr->shwaddr, &netif->hwaddr, &netif->ipaddr, &hdr->shwaddr, &sipaddr, ARP_REPLY);
      }
      else
        /* ignore the unconfigured condition */
        printf("ARP request not for us, ignored");
      break;
    default:
      break;
  }
}