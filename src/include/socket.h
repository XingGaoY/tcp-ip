#ifndef _SOCKET_H_
#define _SOCKET_H_

typedef int _sa_family_t;

struct _sockaddr {
	_sa_family_t	sa_family;	/* address family, AF_xxx	*/
	char		sa_data[14];	/* 14 bytes of protocol address	*/
};

/*
 *	As we do 4.4BSD message passing we use a 4.4BSD message passing
 *	system, not 4.3. Thus msg_accrights(len) are now missing. They
 *	belong in an obscure libc emulation or the bin.
 */
 
//struct msghdr {
//	void	*	msg_name;	/* Socket name			*/
//	int		msg_namelen;	/* Length of name		*/
//	struct iovec *	msg_iov;	/* Data blocks			*/
//        /__kernel_size_t	msg_iovlen;	/* Number of blocks		*/
//	void 	*	msg_control;	/* Per protocol magic (eg BSD file descriptor passing) */
//	__kernel_size_t	msg_controllen;	/* Length of cmsg list */
//	unsigned	msg_flags;
//};

#endif // _SOCKET_H_
