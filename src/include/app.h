#ifndef _APP_H_
#define _APP_H_

#include "socket.h"

void userapp();

void THREAD_APP(int argc, char **argv);

void udp_server();

void udp_client();

void tcp_server();

#endif // _APP_H_
