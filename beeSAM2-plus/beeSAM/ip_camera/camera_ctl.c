#include "ip_camera.h"

int bspls_ctl_ip_camera(const char *username, const char *pwd, const char* sn, const char* url, const char* c_ip, const int cmd)
{
   Camera_Msg C_Msg;
   memset( &C_Msg, 0, sizeof(Camera_Msg));

   int Msg_Id =0;
   int Msg_Ret = 0;

   C_Msg.msgtype = CAMERA_MSG_TYPE;
   C_Msg.cmd = cmd;
   memcpy( C_Msg.msg_name, username, strlen(username));
   memcpy( C_Msg.msg_pwd, pwd, strlen( pwd));
   memcpy( C_Msg.msg_sn, sn, strlen(sn));
   memcpy( C_Msg.msg_url, url, strlen(url));
   memcpy( C_Msg.msg_c_ip, c_ip, strlen(c_ip));

   printf("name = %s pwd=%s sn=%s url=%s ip=%s\r\n", C_Msg.msg_name, C_Msg.msg_pwd, C_Msg.msg_sn, C_Msg.msg_url, C_Msg.msg_c_ip);

   Msg_Id = msgget( CAMERA_MSG_KEY ,IPC_EXCL);;

   if( Msg_Id < 0 )
   {
        Msg_Id = msgget( CAMERA_MSG_KEY, IPC_CREAT|0666);
	if( Msg_Id < 0 )
	{
	   printf("Msg Is Not Exist\r\n");
	   return Msg_Id;
	}
    }

   Msg_Ret = msgsnd( Msg_Id,&C_Msg,sizeof(Camera_Msg),IPC_NOWAIT);
   if(  Msg_Ret < 0 )
   {
       printf("Msg Snd Error\r\n");
       return Msg_Ret;
   }
  return 0;
}




