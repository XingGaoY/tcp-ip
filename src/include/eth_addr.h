#ifndef _ETH_ADDR_H_
#define _ETH_ADDR_H_

#include"def.h"

struct eth_addr{
  uint8_t addr[ETH_HWADDR_LEN];
};

#endif //_ETH_ADDR_H_
