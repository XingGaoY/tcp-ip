#ifndef _DEF_H_
#define _DEF_H_

#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

extern FILE *logout;

/*-------------------------------*/
/* some config, move out someday */
#define verbose
#define ARP_TABLE_SIZE  10
#define MAX_FRAME_LEN 100
#define INIT_TTL 32
/*-------------------------------*/

#ifndef ETH_HWADDR_LEN
#define ETH_HWADDR_LEN 6
#endif

#define PP_HTONS(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))
#define PP_HTONL(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))

/* lwip_htons -- Convert an u16_t from host to network byte order.
 * @param n u16_t in host byte order
 * @return n in network byte order
 */
uint16_t lwip_htons(uint16_t n);

#endif
