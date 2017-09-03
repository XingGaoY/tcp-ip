#include "socket.h"
#include "net.h"
#include "af_inet.h"

#define SIZEOF_SOCK_LIST 10

static struct socket *sock_list[SIZEOF_SOCK_LIST];

/* sockfd is simplified as the index of socket array */
int sock_create(int type){
  struct socket *sock;
  int sock_fd;

  // In linux, socket is tied with an inode in socket-fs
  // And set inode info then
  // Here, we only alloc a socket instead
  // And organize them with a socket list
  sock = (struct socket *)malloc(sizeof(struct socket));

  for(sock_fd = 0; sock_fd < SIZEOF_SOCK_LIST; sock_fd++){
    if(!sock_list[sock_fd]){
      sock_list[sock_fd] = sock;
      break;
    }
  }

  if(sock_fd == SIZEOF_SOCK_LIST){
    printf("No available socket\n");
    sock_fd = -1;
  }

  sock->type = type;
  // create the relevant sock, further initialize socket and sock
  inet_create(sock);

  return sock_fd;
}

/** raw_socket -- create a socket and return its sock_fd
 * @type: the net connet type(dgram or stream)
 */
int raw_socket(int type){
  int sock_fd;
  
  sock_fd = sock_create(type);

  return sock_fd;
}

int raw_bind(int sock_fd, struct __sockaddr *addr){
  struct socket *sock;
  if((sock = sock_list[sock_fd]) != NULL){
    sock->ops->bind(sock, addr);
  }
  return 1;
}


// No _from now, just a copy of src ip & port
int raw_recv(int sock_fd, void *buf, int len){
  struct socket *sock;
  int retval = 0;
  
  if((sock = sock_list[sock_fd]) != NULL){
    retval = sock->ops->recvmsg(sock, buf, len);
  }
  return retval;
}

//TODO This fun should be called after bind which is slightly different from POSIX now
// All the msg_hdr process is ignored so no further sys_msgsend()
int raw_send(int sock_fd, void *buf, int len, 
             const struct __sockaddr *dest_addr){
  struct socket *sock;
  int retval = 0;

  if((sock = sock_list[sock_fd]) != NULL){
    retval = sock->ops->sendmsg(sock, buf, len, dest_addr);
  }
  return retval;
}
