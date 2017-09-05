#ifndef _IP4_ADDR_H_
#define _IP4_ADDR_H_

#include"def.h"

typedef uint32_t ip4_addr;

/* 
 * Some compiler may make ip4_addr aligned, so use another ip addr def in ARP
 * but why?
 */
struct ip4_addr2{
  uint16_t addrw[2];
};

/** MEMCPY-like copying of IP addresses where addresses are known to be
 * 16-bit-aligned if the port is correctly configured (so a port could define
 * this to copying 2 u16_t's) - no NULL-pointer-checking needed. */
#ifndef IPADDR2_COPY
#define IPADDR2_COPY(dest, src) memcpy(dest, src, sizeof(ip4_addr))
#endif

#endif //_IP4_ADDR_H_
