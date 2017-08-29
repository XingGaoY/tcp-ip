#include"util.h"

void print_ip_addr(const struct ip4_addr *addr){
  uint8_t segment[4];
  for(int i = 0; i < 4; i++){
    segment[i] = ((addr->addr)>>(i*8))&0xff;
  }
  printf("%d.%d.%d.%d", segment[0], segment[1], segment[2], segment[3]);
}

void print_ip_addr2(struct ip4_addr2 *addr){
  uint8_t segment[4];
  for(int i = 0; i < 4; i++){
    segment[i] = ((addr->addrw[i/2])>>((i%2)*8))&0xff;
  }
  printf("%d.%d.%d.%d", segment[0], segment[1], segment[2], segment[3]);
}

void print_eth_addr(struct eth_addr *addr){
  printf("%02x:%02x:%02x:%02x:%02x:%02x", (unsigned)addr->addr[0], (unsigned)addr->addr[1], (unsigned)addr->addr[2],(unsigned)addr->addr[3], (unsigned)addr->addr[4], (unsigned)addr->addr[5]);
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
