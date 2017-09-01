#include "sock.h"

struct sock *sk_alloc(){
  struct sock *sk = malloc(sizeof(struct sock));
  if(sk){
    memset(sk, 0, sizeof(struct sock));
  }
  return sk;
}

