#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<linux/if.h>
#include<linux/if_tun.h>
#include<sys/ioctl.h>

#define exe_length 10
#define ip_route "10.0.0.0/24"
#define ip_addr "10.0.0.5/24"

int run_ip_set(char *arg[]){
  int ret = 0, pid;
  pid = fork();
  if(pid == 0){
    ret = execvp("ip", arg);
    exit(ret);
  }
  else
    wait(&ret);
  return ret;
}

int set_up_dev(char *dev){
  int ret = 0;
  char *arg0[] = {"ip", "link", "set", "dev", dev, "up", NULL};
  if((ret = run_ip_set(arg0)) != 0){
    printf("Unable to set link up");
    return ret;
  }

  char *arg1[] = {"ip", "route", "add", "dev", dev, ip_route, NULL};
  if((ret = run_ip_set(arg1)) != 0){
    printf("Unable to set up route");
    return ret;
  }

  char *arg2[] = {"ip", "address", "add", "dev", dev, ip_addr, NULL};
  if((ret = run_ip_set(arg2)) != 0){
    printf("Unable to set up dev addr");
    return ret;
  }
}

int tun_alloc(char *dev){
  struct ifreq ifr;
  int fd, err;

  memset(&ifr, 0, sizeof(ifr));

  if((fd = open("/dev/net/tun", O_RDWR)) < 0){
    printf("Unable to open \"/dev/net/tun\"");	
    //Maybe make one rather than a error exit
    exit(1);
  }    

  // IFF_TUN - TUN device to operate ip packets
  ifr.ifr_flags = IFF_TUN;

  if(*dev)
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);

  if((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0){
    printf("ERR: Could not ioctl tun: %s\n", strerror(errno));
    close(fd);
    exit(1);
  }

  strcpy(dev, ifr.ifr_name);
  return fd;
}

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
  }
}
