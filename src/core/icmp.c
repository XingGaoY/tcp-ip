#include "icmp.h"
#include "util.h"
#include "ethernet.h"
#include "ip4.h"
#include <stdio.h>

/** 
 * icmp_input -- handles icmp message
 * but only icmp echo is handled and send back echo reply
 */
void icmp_input(char *p){
  struct ip_hdr *ip = (struct ip_hdr *)p;
  struct icmp_echo_hdr *iecho;

  iecho = (struct icmp_echo_hdr *)ip->data;

  /* some check need to be done 
     but I just check the type */
  if(iecho->type != ICMP_ECHO){
    fprintf(logout, "icmp type not implemented yet: %x\n", iecho->type);
    return;
  }

  switch(iecho->type){
    case ICMP_ECHO:
      fprintf(logout, "-----\n");
      fprintf(logout, "Received an icmp echo...\n");
      fprintf(logout, "type: %02x  code: %02x  checksum: %04x  identifier: %04x  seqnum: %04x\n", iecho->type, iecho->code, iecho->chksum, iecho->id, iecho->seqno);

      fprintf(logout, "checksum calculated: %04x\n", checksum(iecho, PP_HTONS(IPH_LEN(ip)) - 4 * IPH_HL(ip)));
      /* If checksum not in consistence, drop the datagram silently */
      if(checksum(iecho, PP_HTONS(IPH_LEN(ip)) - 4 * IPH_HL(ip)) != 0)
        return;
      iecho->type = ICMP_ER;
      iecho->chksum = 0;
      iecho->chksum = checksum(iecho, PP_HTONS(IPH_LEN(ip)) - 4 * IPH_HL(ip));
      ip4_output(p, ip->dest, ip->src);
      break;
    default:
      break;
  }
}
