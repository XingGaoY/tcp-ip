#ifndef _PROTO_H_
#define _PROTO_H_

#include "net.h"

enum {
  IPPROTO_IP = 0,		/* Dummy protocol for TCP		*/
  IPPROTO_ICMP = 1,		/* Internet Control Message Protocol	*/
  IPPROTO_IGMP = 2,		/* Internet Group Management Protocol	*/
  IPPROTO_TCP = 6,		/* Transmission Control Protocol	*/
  IPPROTO_UDP = 17,		/* User Datagram Protocol		*/
  IPPROTO_RAW	 = 255,		/* Raw IP packets			*/
  IPPROTO_MAX
};

/* This is used to register socket interfaces for IP protocols.  */
struct inet_protosw {
        /* These two fields form the lookup key.  */
	unsigned short	 type;	   /* This is the 2nd argument to socket(2). */
	int		 protocol; /* This is the L4 protocol number.  */

	struct proto	 *prot;
	struct proto_ops *ops;
};

#endif // _PROTO_H_
