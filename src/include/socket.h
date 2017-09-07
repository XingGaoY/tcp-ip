#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdint.h>

#define _SOCK_STREAM 0
#define _SOCK_DGRAM  1
#define _SOCK_RAW    2

/**
 * No file system is used, and as one connection matches only one socket
 * So I simply create an array to hold the read and write msg circular queue
 */

typedef short int _sa_family_t;

struct __sockaddr {
  uint32_t		ip_addr;	/* protocol address, suppose legal ip addr	*/
  unsigned short int  	port;
};

// All sockets are AF_INET, so rm domain field
int raw_socket(int type);
int raw_bind(int sock_fd, struct __sockaddr *addr);
// No backlog for now
int raw_listen(int sock_fd);
int raw_recv(int sock_fd, void *buf, int len);
int raw_send(int sock_fd, void *buf, int len, 
             const struct __sockaddr *dest_addr);
#endif // _SOCKET_H_
