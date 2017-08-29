#include"dev_setup.h"
#include"tun.h"
#include"netif.h"
#include"ethernet.h"
#include"util.h"

int main(int argc, char** argv){
  int tun_fd;
  char *buf = (char*)malloc(BUF_SIZE);
  
  char *dev = (char*)malloc(IFNAMSIZ);
  strcpy(dev, "tap0");

  tun_fd = tun_alloc(dev);

  if(set_up_dev(dev) != 0)
    perror("Unable to set up device");

  netdev_init();

  printf("My hardware addr = ");
  print_eth_addr(&netif->hwaddr);
  printf("\tMy ip addr = ");
  print_ip_addr(&netif->ipaddr);
  printf("\n");

  while(1){
    memset(buf, 0, BUF_SIZE);
    read(tun_fd, buf, 128);
    /*printf("=====\nEthernet frame:\n");
    for(int i=0; i<128; i++){
      if(i%8 == 0) printf("\n");
      printf("%02x", (unsigned char)buf[i]);
    }
    printf("\n=====\n");
    */
    ethernet_input(buf);
  }
}
