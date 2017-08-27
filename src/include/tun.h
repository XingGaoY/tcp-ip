#ifndef _TUN_H_
#define _TUN_H_

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/socket.h>
#include<linux/if.h>
#include<linux/if_tun.h>
#include<sys/ioctl.h>

int tun_alloc(char *dev);

void tun_write(char *buf, int len);

#endif //_TUN_H_
