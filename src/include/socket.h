#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdint.h>

typedef short int _sa_family_t;

struct __sockaddr {
  _sa_family_t		sa_family;	/* address family, AF_xxx	*/
  uint32_t		sa_data;	/* protocol address, suppose legal ip addr	*/
  unsigned short int  	port;
};

int raw_bind();

#endif // _SOCKET_H_
