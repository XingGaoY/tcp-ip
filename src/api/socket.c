#include "socket.h"
#include "net.h"
#include "af_inet.h"

#define SIZEOF_SOCK_LIST 10

static struct socket *sock_list[SIZEOF_SOCK_LIST];

int sock_create(int type, struct socket **res){
  struct socket *sock;
  int sock_fd;

  // In linux, socket is tied with an inode in socket-fs
  // And set inode info then
  // Here, we only alloc a socket instead
  // And organize them with a socket list
  sock = (struct socket *)malloc(sizeof(struct socket));

  for(sock_fd = 0; sock_fd < SIZEOF_SOCK_LIST; sock_fd++){
    if(sock_list[sock_fd]){
      sock_list[sock_fd] = sock;
      break;
    }
  }

  if(sock_fd == SIZEOF_SOCK_LIST){
    printf("No available socket\n");
    sock_fd = -1;
  }

  sock->type = type;
  inet_create(sock);

  return sock_fd;
}

int raw_socket(int type){
  int retval;
  struct socket *sock;
  
  if((retval = sock_create(type, &sock)) != -1){

  }

  return retval;
}

int raw_bind(){
  return 1;
}
