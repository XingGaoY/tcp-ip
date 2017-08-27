#ifndef _DEV_SETUP_H_
#define _DEV_SETUP_H_

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>


#define exe_length 10
#define ip_route "10.0.0.0/24"

int run_ip_set(char *arg[]);

int set_up_dev(char *dev);

#endif /*_DEV_SETUP_H_*/
