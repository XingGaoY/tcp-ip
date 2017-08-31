#ifndef _PROTO_H_
#define _PROTO_H_

#include "net.h"

/* This is used to register socket interfaces for IP protocols.  */
struct inet_protosw {
        /* These two fields form the lookup key.  */
	unsigned short	 type;	   /* This is the 2nd argument to socket(2). */
	int		 protocol; /* This is the L4 protocol number.  */

	struct proto	 *prot;
	struct proto_ops *ops;
};

#endif // _PROTO_H_
