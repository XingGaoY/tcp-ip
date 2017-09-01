#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "def.h"

/**
 * No file system is used, and as one connection matches only one socket
 * So I simply create an array to hold the read and write msg circular queue
 */

typedef short int _sa_family_t;

struct __sockaddr {
  _sa_family_t		sa_family;	/* address family, AF_xxx	*/
  uint32_t		sa_data;	/* protocol address, suppose legal ip addr	*/
  unsigned short int  	port;
};

// All sockets are AF_INET, so rm domain field
int raw_socket(int type, int protocol);
int raw_bind();

#endif // _SOCKET_H_
