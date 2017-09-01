#include "socket.h"

int raw_socket(int domain, int type, int protocol){
  int retval;
  struct socket *sock;
  
  retval = sock_create(domain, type, protocol, &sock);
}

int raw_bind(){
  return 1;
}
