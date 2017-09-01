#include "af_inet.h"

int inet_bind(struct socket *sock, struct __sockaddr *uaddr, int addr_len){
  struct sock *sk = sock->sk;
  struct inet_opt *inet = inet_sk(sk);
  unsigned short sport;

  if(sk->sk_prot->bind)
    sk->sk_prot->bind(sk, uaddr, addr_len);

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
    .type = _SOCK_DGRAM,
    .protocol = IPPROTO_UDP,
    .prot = &udp_prot,
    .ops = &inet_dgram_ops,
  }
};

int inet_init(void){
  struct inet_protosw *q;

  udp_init();

  return 1;
}
