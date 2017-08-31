#include "inet_common.h"
#include "sock.h"
#include "proto.h"
#include "udp.h"

int inet_bind(struct socket *sock, struct _sockaddr *uaddr, int addr_len){
  struct sock *sk = sock->sk;

  if(sk->sk_prot->bind)
    sk->sk_prot->bind(sk, uaddr, addr_len);
  return 1;
}

struct proto_ops inet_dgram_ops = {
  .family = PF_INET,
  .release = NULL,
  .bind = inet_bind,
  .connect = NULL,
  .accept = NULL,
  .listen = NULL,
  .shutdown = NULL
  //.sendmsg = NULL,
  //.recvmsg = NULL
};

static struct inet_protosw inetsw_array[] =
{
  {
    .type = SOCK_DGRAM,
    .protocol = IPPROTO_UDP,
    .prot = &udp_prot,
    .ops = &inet_dgram_ops,
  }
};
