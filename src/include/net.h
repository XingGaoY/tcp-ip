#ifndef _NET_H_
#define _NET_H_

#include "sock.h"
#include "ip4.h"

enum socket_state{
  SS_FREE = 0,			/* not allocated		*/
  SS_UNCONNECTED,		/* unconnected to any socket	*/
  SS_CONNECTING,		/* in process of connecting	*/
  SS_CONNECTED,			/* connected to socket		*/
  SS_DISCONNECTING		/* in process of disconnecting	*/
};

struct socket{
  enum socket_state  state;
  const struct proto_ops  *ops;

  struct sock *sk;
  unsigned short type;
};

struct proto_ops{
	int		(*release)   (struct socket *sock);
	int		(*bind)	     (struct socket *sock,
				      struct __sockaddr *uaddr);
	int		(*connect)   (struct socket *sock,
				      struct __sockaddr *vaddr,
				      int sockaddr_len, int flags);
	int		(*accept)    (struct socket *sock,
				      struct socket *newsock, int flags);
	int		(*listen)    (struct socket *sock);
	int		(*shutdown)  (struct socket *sock, int flags);
	int		(*sendmsg)   (struct socket *sock, void *buf, int len, 
                                      const struct __sockaddr *daddr);
	int		(*recvmsg)   (struct socket *sock, void *buf, int len);
};

#endif // _NET_H_
