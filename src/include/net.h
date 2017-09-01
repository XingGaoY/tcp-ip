#ifndef _NET_H_
#define _NET_H_

#define _SOCK_STREAM 0
#define _SOCK_DGRAM  1
#define _SOCK_RAW    2

#define PF_INET 0

#define AF_INET 0

#include "socket.h"

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
  short type;
};

struct proto_ops{
	int		family;
	int		(*release)   (struct socket *sock);
	int		(*bind)	     (struct socket *sock,
				      struct __sockaddr *myaddr,
				      int sockaddr_len);
	int		(*connect)   (struct socket *sock,
				      struct __sockaddr *vaddr,
				      int sockaddr_len, int flags);
	int		(*accept)    (struct socket *sock,
				      struct socket *newsock, int flags);
	int		(*listen)    (struct socket *sock, int len);
	int		(*shutdown)  (struct socket *sock, int flags);
	//int		(*sendmsg)   (struct kiocb *iocb, struct socket *sock,
	//			      struct msghdr *m, int total_len);
	//int		(*recvmsg)   (struct kiocb *iocb, struct socket *sock,
	//			      struct msghdr *m, int total_len,
	//			      int flags);
};

static int inet_init(void);

#endif // _NET_H_
