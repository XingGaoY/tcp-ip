#ifndef _APP_H_
#define _APP_H_

#include "socket.h"

typedef struct op{
  int argc;
  char **argv;
}op_t;

void userapp(op_t *a);

void THREAD_APP();

void udp_server();

#endif // _APP_H_
