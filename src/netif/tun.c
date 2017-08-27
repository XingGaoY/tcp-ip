#include "tun.h"

static int tunfd;

int tun_alloc(char *dev){
  struct ifreq ifr;
  int fd, err;

  memset(&ifr, 0, sizeof(ifr));

  if((fd = open("/dev/net/tap", O_RDWR)) < 0){
    printf("Unable to open \"/dev/net/tap\"");	
    //Maybe make one rather than a error exit
    exit(1);
  }    

  // IFF_TAP - TAP device to operate Ethernet frames
  // IFF_NO_PI - Do not provide packet information
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

  if(*dev)
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);

  if((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0){
    printf("ERR: Could not ioctl tun: %s\n", strerror(errno));
    close(fd);
    exit(1);
  }

  strcpy(dev, ifr.ifr_name);
  tunfd = fd;
  return fd;
}

void tun_write(char *buf, int len){
  write(tunfd, buf, len);
}
