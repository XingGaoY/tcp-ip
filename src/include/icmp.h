#ifndef _ICMP_H_
#define _ICMP_H_

#include <stdint.h>

#define ICMP_ER   0    /* echo reply */
#define ICMP_ECHO 8    /* echo */

struct icmp_echo_hdr{
  uint8_t type;
  uint8_t code;
  uint16_t chksum;
  uint16_t id;
  uint16_t seqno;
};

void icmp_input(char *p);

#endif // _ICMP_H_
