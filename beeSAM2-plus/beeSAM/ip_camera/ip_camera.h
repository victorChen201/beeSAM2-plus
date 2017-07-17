#ifndef __IP_CAMERA_H__
#define __IP_CAMERA_H__
#include<stdio.h>
#include<string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "wsclient.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>


#define CAMERA_IP               "192.168.0.100"
#define CAMERA_PORT             80
#define CAMERA_MSG_KEY          8023
#define CAMERA_MSG_TYPE         123

#define CAMERA_GET_VIDEO        1
#define CAMERA_CANCEL_GET_VIDEO 2

#define CMAERA_NUM              10

typedef struct __CAMER__MSG__
{
   long msgtype;
   int  cmd;
   char msg_name[32];
   char msg_pwd[32];
   char msg_sn[20];
   char msg_url[128];
   char msg_c_ip[20];
}Camera_Msg;


typedef struct __IP_CAMER__TYPE__
{
  char     c_name[32];
  char     c_pwd[32];
  char     c_sn[20];
  char     c_url[128];
  char     c_ip[20];
  wsclient* c_client;
  pthread_mutex_t c_picmutex;
  pthread_mutex_t c_setmutex;
  pthread_t  c_picid;
  pthread_t  c_setid;
}Camera_Type;


int bspls_ctl_ip_camera(const char *username, const char *pwd, const char* sn, const char* url, const char* c_ip, const int cmd);

#endif


