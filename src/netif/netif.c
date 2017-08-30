#include "netif.h"
#include "tun.h"
#include "ethernet.h"
#include "dev_setup.h"

struct netif *netif;

/* Set up ipaddr and hwaddr */
void netdev_init(){
  /* As all the patch sent by tun0 will be received by us
   * We pretend we have our own ipaddr and hwaddr 
   */
  netif = (struct netif*)malloc(sizeof(struct netif));
  memset(netif, 0, sizeof(struct netif));
  netif->ipaddr.addr = IP_ADDR;

  struct eth_addr hw = HW_ADDR;
  memcpy(&netif->hwaddr, &hw, sizeof(hw));

  char *dev = (char*)malloc(IFNAMSIZ);
  strcpy(dev, "tap0");

  netif->net_fd = tun_alloc(dev);

  if(set_up_dev(dev) != 0)
    perror("Unable to set up device");

  fprintf(logout, "My hardware addr = ");
  print_eth_addr(&netif->hwaddr);
  fprintf(logout, "\tMy ip addr = ");
  print_ip_addr(&netif->ipaddr);
  fprintf(logout, "\n");
}

void netdev_listen(){
  char *buf = (char*)malloc(MAX_FRAME_LEN);
  while(1){
    memset(buf, 0, MAX_FRAME_LEN);
    read(netif->net_fd, buf, 128);
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

void netdev_xmit(struct sk_buff *skb){
  /*printf("=====\nEthernet frame sent:\n");
  for(int i=0; i<len; i++){
    if(i%8 == 0) printf("\n");
    printf("%02x", (unsigned char)buf[i]);
  }
  printf("\n=====\n");*/

  tun_write(skb->data, skb->len);
}
