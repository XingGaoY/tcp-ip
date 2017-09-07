#include "af_inet.h"
#include "netif.h"

//TODO socket needs to be freed when err

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
  inet->saddr = netif->ipaddr;
  inet->sport = sport;
  inet->ttl = 32;
  inet->daddr = 0;
  inet->dport = 0;
  inet->id = 0;

  sk->sk_state = SK_CLOSED;
  //sk_dst_reset(sk);
  return 1;
}

int inet_listen(struct socket *sock){
  int retval;
  if(sock->state != SS_UNCONNECTED || sock->type != _SOCK_STREAM)
    return -1;

  struct sock *sk = sock->sk;
  retval = sk->sk_prot->listen(sk);

  return retval;
}

int inet_recvmsg(struct socket *sock, void *buf, int len){
//TODO a autobind is needed if not bind before
  int retval;

  struct sock *sk = sock->sk;
  retval = sk->sk_prot->recvmsg(sk, buf, len);

  return retval;
}

int inet_sendmsg(struct socket *sock, void *buf, int len, 
                 const struct __sockaddr *daddr){
  int retval;

  struct sock *sk = sock->sk;
  retval = sk->sk_prot->sendmsg(sk, buf, len, daddr);

  return retval;
}

struct proto_ops inet_dgram_ops = {
  .release = NULL,
  .bind = inet_bind,
  .connect = NULL,
  .accept = NULL,
  .listen = inet_listen,
  .shutdown = NULL,
  .sendmsg = inet_sendmsg,
  .recvmsg = inet_recvmsg
};

struct proto_ops inet_stream_ops = {
  .bind = inet_bind,
  .listen = inet_listen
};

struct inet_protosw inetsw_array[] =
{
  {
    .type = _SOCK_STREAM,
    .protocol = IPPROTO_TCP,
    .prot = &tcp_prot,
    .ops = &inet_stream_ops,
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
  pthread_spin_init(&sk->rcv_lock, PTHREAD_PROCESS_PRIVATE);
  pthread_spin_init(&sk->xmit_lock, PTHREAD_PROCESS_PRIVATE);

  sk->sk_receive_queue = (struct sk_buff_head *)malloc(sizeof(struct sk_buff_head));
  sk->sk_xmit_queue = (struct sk_buff_head *)malloc(sizeof(struct sk_buff_head));
  sk->sk_prot = proto->prot;
  sk->sk_type = proto->protocol;

  inet = inet_sk(sk);
  
  memset(inet, 0, sizeof(struct inet_opt));

  skb_queue_head_init(sk->sk_receive_queue);
  skb_queue_head_init(sk->sk_xmit_queue);

  sock->sk = sk;

  return 1;
}

int inet_init(void){

  udp_init();

  tcp_init();

  return 1;
}
