#ifndef _IP4_H_
#define _IP4_H_

#include "ip4_addr.h"

struct ip_hdr{
  uint8_t _v_hl;
  uint8_t _tos;
  uint16_t _len;
  uint16_t _id;
  uint16_t _offset;
  uint8_t _ttl;
  uint8_t _proto;
  uint16_t _chksum;
  struct ip4_addr src;
  struct ip4_addr dest;
};

/* Macros to get struct ip_hdr fields: */
#define IPH_V(hdr)  ((hdr)->_v_hl >> 4)
#define IPH_HL(hdr) ((hdr)->_v_hl & 0x0f)

/* Macros to set struct ip_hdr fields: */
#define IPH_VHL_SET(hdr, v, hl) (hdr)->_v_hl = (u8_t)((((v) << 4) | (hl)))

void ip4_input(char *p);

#endif // _IP4_H_
