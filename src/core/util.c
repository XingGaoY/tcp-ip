#include "util.h"
#include <stdlib.h>

#define PSEU_LEN 12

struct pseudo_hdr{
  uint32_t saddr;
  uint32_t daddr;
  uint8_t zero;
  uint8_t proto;
  uint16_t len;
  char hdr[];
};

void print_ip_addr(const ip4_addr *addr){
  uint8_t segment[4];
  for(int i = 0; i < 4; i++){
    segment[i] = ((*addr)>>(i*8))&0xff;
  }
  fprintf(logout, "%d.%d.%d.%d", segment[0], segment[1], segment[2], segment[3]);
}

void print_ip_addr2(struct ip4_addr2 *addr){
  uint8_t segment[4];
  for(int i = 0; i < 4; i++){
    segment[i] = ((addr->addrw[i/2])>>((i%2)*8))&0xff;
  }
  fprintf(logout, "%d.%d.%d.%d", segment[0], segment[1], segment[2], segment[3]);
}

void print_eth_addr(struct eth_addr *addr){
  fprintf(logout, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned)addr->addr[0], (unsigned)addr->addr[1], (unsigned)addr->addr[2],(unsigned)addr->addr[3], (unsigned)addr->addr[4], (unsigned)addr->addr[5]);
}

uint16_t checksum(void *dataptr, int len){
  uint32_t sum = 0;
  uint16_t *ptr = dataptr;
  while(len > 1){
    sum += * ptr++;
    len -= 2;
  }
  if(len > 0)
    sum += * (uint8_t *)dataptr;
  while(sum>>16)
    sum = (sum & 0xffff) + (sum >> 16);

  return ~sum;
}

uint16_t pseudo_chksum(struct sk_buff *skb, ip4_addr src, ip4_addr dest, uint8_t proto){
  int len = skb->len;
  int plen = (len/2 + 1) * 2;
  void *p = malloc(PSEU_LEN + plen);
  memset(p, 0, PSEU_LEN + plen);
 
  struct pseudo_hdr *phdr;

  phdr = (struct pseudo_hdr *)p;
  
  memcpy(phdr->hdr, skb->transport_header, len);
  
  phdr->saddr = src;
  phdr->daddr = dest;
  phdr->proto = proto;
  phdr->len = PP_HTONS(len);

  return checksum(p, PSEU_LEN + plen);
}
