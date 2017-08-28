#include"netif.h"
#include"tun.h"

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
}

void netdev_xmit(char *buf, int len){
  printf("=====\nEthernet frame sent:\n");
  for(int i=0; i<len; i++){
    if(i%8 == 0) printf("\n");
    printf("%02x", (unsigned char)buf[i]);
  }
  printf("\n=====\n");

  tun_write(buf, len);
}
