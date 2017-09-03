#include "app.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

int 	udp     = 0;
int	server  = 0;

void userapp(){
  if(udp && server){
    udp_server();
  }
  else if(udp && !server){
    udp_client();
  }
}

void THREAD_APP(int argc, char **argv){
  pthread_t app_thread;
  char c;

  while ( (c = getopt(argc, argv, "us")) != EOF) {
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

  pthread_create(&app_thread, NULL, (void*)userapp, NULL);
}
