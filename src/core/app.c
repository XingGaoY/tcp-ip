#include "app.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

int 	udp     = 0;
int	server  = 0;

void userapp(op_t *a){
  char c;

  while ( (c = getopt(a->argc, a->argv, "us")) != EOF) {
    switch (c) {
      case 'u':
        udp = 1;
        break;
      case 's':
        server = 1;
        break;
      case '?':
        printf("unrecognized option");
        break;
    }
  }
  if(udp && server){
    udp_server();
  }
}

void THREAD_APP(op_t arg){
  pthread_t app_thread;

  pthread_create(&app_thread, NULL, (void*)userapp, &arg);
}
