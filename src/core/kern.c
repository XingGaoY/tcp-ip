#include <unistd.h>
#include "netif.h"
#include "util.h"
#include "net.h"
#include <pthread.h>

struct op{
  int argc;
  char **argv;
};

void userapp(void *a){
  struct op *arg = (struct op*)a;
  int c;
  while ( (c = getopt(arg->argc, arg->argv, "U")) != EOF) {
    printf("%d ", c);
    switch (c) {
      case 'U':
        break;
      case '?':
        printf("unrecognized option");
    }
  }
}

FILE *logout;

int main(int argc, char** argv){
  pthread_t USR_APP_THREAD;

  logout = fopen("log", "w+");
  setbuf(logout, NULL);

  struct op arg = {argc, argv};

  pthread_create(&USR_APP_THREAD, NULL, (void*)userapp, &arg);

  netdev_init();
  inet_init();

  netdev_listen();
}
