#include "dev_setup.h"
#include "tun.h"
#include "netif.h"
#include "ethernet.h"
#include "util.h"
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
  int tun_fd;
  char *buf = (char*)malloc(BUF_SIZE);
  
  char *dev = (char*)malloc(IFNAMSIZ);
  strcpy(dev, "tap0");

  logout = fopen("log", "w+");
  setbuf(logout, NULL);

  struct op arg = {argc, argv};

  pthread_create(&USR_APP_THREAD, NULL, (void*)userapp, &arg);

  tun_fd = tun_alloc(dev);

  if(set_up_dev(dev) != 0)
    perror("Unable to set up device");

  netdev_init();

  fprintf(logout, "My hardware addr = ");
  print_eth_addr(&netif->hwaddr);
  fprintf(logout, "\tMy ip addr = ");
  print_ip_addr(&netif->ipaddr);
  fprintf(logout, "\n");

  while(1){
    memset(buf, 0, BUF_SIZE);
    read(tun_fd, buf, 128);
    /*fprintf(logout, "=====\nEthernet frame:\n");
    for(int i=0; i<128; i++){
      if(i%8 == 0) printf("\n");
      fprintf(logout, "%02x", (unsigned char)buf[i]);
    }
    fprintf(logout, "\n=====\n");
    */
    ethernet_input(buf);
  }
}
