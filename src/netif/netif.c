#include "netif.h"
#include "tun.h"
#include "ethernet.h"
#include "dev_setup.h"
#include "util.h"
#include "skbuff.h"
#include "etharp.h"

struct netif *netif;

/* Set up ipaddr and hwaddr */
void netdev_init(){
  /* As all the patch sent by tun0 will be received by us
   * We pretend we have our own ipaddr and hwaddr 
   */
  netif = (struct netif*)malloc(sizeof(struct netif));
  memset(netif, 0, sizeof(struct netif));
  netif->ipaddr = IP_ADDR;

  struct eth_addr hw = HW_ADDR;
  memcpy(&netif->hwaddr, &hw, sizeof(hw));

  char *dev = (char*)malloc(IFNAMSIZ);
  strcpy(dev, "tap0");

  netif->net_fd = tun_alloc(dev);

  if(set_up_dev(dev) != 0)
    perror("Unable to set up device");

  etharp_init();

  fprintf(logout, "My hardware addr = ");
  print_eth_addr(&netif->hwaddr);
  fprintf(logout, "\tMy ip addr = ");
  print_ip_addr(&netif->ipaddr);
  fprintf(logout, "\n");
}

void netdev_listen(){
  // No method to destroy skbuff now, remember to destroy it
  char *buf = (char*)malloc(MAX_FRAME_LEN);

  while(1){
    struct sk_buff *skb = alloc_skb();
    memset(buf, 0, MAX_FRAME_LEN);
    read(netif->net_fd, buf, MAX_FRAME_LEN);

    skb_add_data(skb, buf, MAX_FRAME_LEN); 

    fprintf(logout, "=====\nEthernet frame:\n");
    for(int i=0; i<100; i++){
      if(i%8 == 0) fprintf(logout, "\n");
      fprintf(logout, "%02x", (unsigned char)buf[i]);
    }
    fprintf(logout, "\n=====\n");
    
    ethernet_input(skb);
  }
}

void netdev_xmit(struct sk_buff *skb){
  fprintf(logout, "=====\nEthernet frame sent:\n");
  char *buf = (char *)skb->data;
  for(int i=0; i<100; i++){
    if(i%8 == 0) fprintf(logout, "\n");
    fprintf(logout, "%02x", (unsigned char)buf[i]);
  }
  fprintf(logout, "\n=====\n");

  tun_write(skb->data, skb->len);
}
