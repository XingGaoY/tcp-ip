#include "af_inet.h"

int inet_bind(struct socket *sock, struct __sockaddr *uaddr){
  struct sock *sk = sock->sk;
  struct inet_opt *inet = inet_sk(sk);
  unsigned short sport;

  // Check uaddr is needed(port&ipaddr)

  // Invoke TCP/UDP bind(get_port)
  sport = uaddr->port;

  //if(sport && snum < PROT_SOCK)

  // Suppose UDP server only, so we only have our port num now
  if(sk->sk_prot->get_port(sk, sport)){
    printf("ADDR IN USE");
    // release sock
  }
  // Add info to sock
  inet->sport = sport;
  inet->daddr = 0;
  inet->dport = 0;
  //sk_dst_reset(sk);
  return 1;
}

int inet_recvmsg(struct socket *sock, void *buf, int len){
  int retval;

  struct sock *sk = sock->sk;
  retval = sk->sk_prot->recvmsg(sk, buf, len);

  return retval;
}

struct proto_ops inet_dgram_ops = {
  .release = NULL,
  .bind = inet_bind,
  .connect = NULL,
  .accept = NULL,
  .listen = NULL,
  .shutdown = NULL,
  .sendmsg = NULL,
  .recvmsg = inet_recvmsg
};

struct inet_protosw inetsw_array[] =
{
  {
    .type = _SOCK_STREAM,
    .protocol = IPPROTO_TCP,
  },
  {
    .type = _SOCK_DGRAM,
    .protocol = IPPROTO_UDP,
    .prot = &udp_prot,
    .ops = &inet_dgram_ops,
  }
};

int inet_create(struct socket *sock){
  struct sock *sk;
  struct inet_protosw *proto = &inetsw_array[sock->type];
  struct inet_opt *inet;

  sock->state = SS_UNCONNECTED;
  sock->ops = proto->ops;
  sk = (struct sock *)proto->prot->sk_alloc();

  // need to check if it is the right proto

  sk->sk_prot = proto->prot;

  inet = inet_sk(sk);
  
  memset(inet, 0, sizeof(struct inet_opt));

  skb_queue_head_init(sk->sk_receive_queue);
  skb_queue_head_init(sk->sk_xmit_queue);

  sock->sk = sk;

  return 1;
}

int inet_init(void){

  udp_init();

  return 1;
}
