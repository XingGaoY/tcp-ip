#ifndef _AF_INET_H_
#define _AF_INET_H_

// As no other network family, I moved it out to make it simple
#include "sock.h"
#include "udp.h"
#include "tcp.h"
#include "net.h"

#define PROTO_NUM 2

extern struct proto_ops inet_dgram_ops;
extern struct proto_ops inet_stream_ops;

enum {
  IPPROTO_IP = 0,		/* Dummy protocol for TCP		*/
  IPPROTO_ICMP = 1,		/* Internet Control Message Protocol	*/
  IPPROTO_IGMP = 2,		/* Internet Group Management Protocol	*/
  IPPROTO_IPIP = 4,		/* IPIP tunnels (older KA9Q tunnels use 94) */
  IPPROTO_TCP = 6,		/* Transmission Control Protocol	*/
  IPPROTO_EGP = 8,		/* Exterior Gateway Protocol		*/
  IPPROTO_PUP = 12,		/* PUP protocol				*/
  IPPROTO_UDP = 17,		/* User Datagram Protocol		*/
  IPPROTO_IDP = 22,		/* XNS IDP protocol			*/
  IPPROTO_RSVP = 46,		/* RSVP protocol			*/
  IPPROTO_GRE = 47,		/* Cisco GRE tunnels (rfc 1701,1702)	*/

  IPPROTO_IPV6	 = 41,		/* IPv6-in-IPv4 tunnelling		*/

  IPPROTO_ESP = 50,            /* Encapsulation Security Payload protocol */
  IPPROTO_AH = 51,             /* Authentication Header protocol       */
  IPPROTO_PIM    = 103,		/* Protocol Independent Multicast	*/

  IPPROTO_COMP   = 108,                /* Compression Header protocol */
  IPPROTO_SCTP   = 132,		/* Stream Control Transport Protocol	*/

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

extern struct inet_protosw inetsw_array[];
int inet_create(struct socket *sock);

int inet_init(void);
#endif // _AF_INET_H_
