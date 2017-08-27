#include"util.h"

void print_ip_addr(const struct ip4_addr *addr){
  uint8_t segment[4];
  for(int i = 0; i < 4; i++){
    segment[i] = ((addr->addr)>>(i*8))&0xff;
  }
  printf("%8d.%8d.%8d.%8d\n", segment[0], segment[1], segment[2], segment[3]);
}

void print_ip_addr2(struct ip4_addr2 *addr){
  uint8_t segment[4];
  for(int i = 0; i < 4; i++){
    segment[i] = ((addr->addrw[i/2])>>((i%2)*8))&0xff;
  }
  printf("%8d.%8d.%8d.%8d\n", segment[0], segment[1], segment[2], segment[3]);
}

void print_eth_addr(struct eth_addr *addr){
  printf("%8x:%8x:%8x:%8x:%8x:%8x\n", (unsigned)addr->addr[0], (unsigned)addr->addr[1], (unsigned)addr->addr[2],(unsigned)addr->addr[3], (unsigned)addr->addr[4], (unsigned)addr->addr[5]);
}
