#include"dev_setup.h"
#include"tun.h"

int main(int argc, char** argv){
  int tun_fd;
  char buf[128];
  
  char *dev = (char*)malloc(IFNAMSIZ);
  strcpy(dev, "tap0");

  tun_fd = tun_alloc(dev);

  if(set_up_dev(dev) != 0)
    perror("Unable to set up device");

  while(1){
    read(tun_fd, buf, 128);
    for(int i=0; i<128; i++){
      if(i%8 == 0) printf("\n");
      printf("%02x", (unsigned char)buf[i]);
    }
  }
}
