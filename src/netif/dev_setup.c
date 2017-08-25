#include "dev_setup.h"

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
  return ret;
}

