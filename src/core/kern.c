#include <unistd.h>
#include "netif.h"
#include "util.h"
#include "af_inet.h"
#include "app.h"

FILE *logout;

int main(int argc, char** argv){
  if(argc < 2){
    printf("Too few argument.\n");
    exit(0);
  }

  logout = fopen("log", "w+");
  setbuf(logout, NULL);

  netdev_init();
  inet_init();
  
  THREAD_APP(argc, argv);

  netdev_listen();
}
