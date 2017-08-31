#include "inet_common.h"
#include <stdlib.h>

struct proto_ops inet_stream_ops = {
  .family = PF_INET,
  .release = NULL,
  .bind = NULL,
  .connect = NULL,
  .accept = NULL,
  .listen = NULL,
  .shutdown = NULL
  //.sendmsg = NULL,
  //.recvmsg = NULL
};
