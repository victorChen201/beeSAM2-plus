#include "ip_camera.h"
#include <openssl/md5.h>

unsigned char Head2[] ="GET / HTTP/1.1\r\n"
"Accept: text/html, application/xhtml+xml, */*\r\n"
"Accept-Language: zh-CN\r\n"
"User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; WOW64; Trident/6.0; MALCJS)\r\n"
"Accept-Encoding: gzip, deflate\r\n"
"Host: 192.168.0.102\r\n"
"Authorization: Basic YWRtaW46MTIzNDU2\r\n"
"Connection: Keep-Alive\r\n"
"DNT: 1\r\n"
"\r\n";

unsigned char Head3[] ="GET /top.htm HTTP/1.1\r\n"
"Accept: text/html, application/xhtml+xml, */*\r\n"
"Accept-Language: zh-CN\r\n"
"User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; WOW64; Trident/6.0; MALCJS)\r\n"
"Accept-Encoding: gzip, deflate\r\n"
"Host: 192.168.0.102\r\n"
"DNT: 1\r\n"
"Connection: Keep-Alive\r\n"
"Authorization: Basic YWRtaW46\r\n"
"\r\n";



unsigned char Head4[] = "GET /image.jpg?cidx=367084173 HTTP/1.1\r\n"
"Accept: image/png, image/svg+xml, image/*;q=0.8, */*;q=0.5\r\n"
"Referer: http://192.168.0.102/top.htm\r\n"
"Accept-Language: zh-CN\r\n"
"User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; WOW64; Trident/6.0; MALCJS)\r\n"
"Accept-Encoding: gzip, deflate\r\n"
"Host: 192.168.0.102\r\n"
"DNT: 1\r\n"
"Connection: Keep-Alive\r\n"
"Authorization: Basic YWRtaW46\r\n"
"\r\n";



unsigned char Video_Get[] = "GET /dgvideo.cgi HTTP/1.0\r\n"
"User-Agent: admin\r\n"
"Authorization: Digest username=\"admin\",realm=\"DCS-930LB1\",nonce=\"72c82acaebb9576b994cb3d0dc83397a\",uri=\"/dgvideo.cgi\",qop=\"auth\",nc=00000001,cnonce=\"8be6120b4171f40e6ca3102d088cab97\",response=\"5af17d511d1f22375a6b70eb569fb4f4\"\r\n"
"\r\n";

unsigned char Audio_Get[] = "GET /dgaudio.cgi HTTP/1.0\r\n"
"User-Agent: admin\r\n"
"Authorization: Digest username=\"admin\",realm=\"DCS-930LB1\",nonce=\"72c82acaebb9576b994cb3d0dc83397a\",uri=\"/dgaudio.cgi\",qop=\"auth\",nc=00000001,cnonce=\"8be6120b4171f40e6ca3102d088cab97\",response=\"751fe69ca74df5c046c56d77dc63eaad\"\r\n"
"\r\n";


#define METHOD       "GET"
#define TAIL         "HTTP/1.0"
#define AGENT        "User-Agent: "
#define AUTHOR       "Authorization: "
#define REALM        "realm="
#define NONCE        "nonce="
#define URI          "uri="
#define NC           "nc="
#define QOP          "qop="
#define RESPON       "response="


unsigned char Camera_Key1[] = "Content-length:";




int on_picture_close(wsclient *c) {
	fprintf(stderr, "onclose called: %d\n", c->sockfd);
	return 0;
}



int on_picture_error(wsclient *c, wsclient_error *err) {
	fprintf(stderr, "onerror: (%d): %s\n", err->code, err->str);
	if(err->extra_code)
	{
		errno = err->extra_code;
		perror("recv");
	}
	return 0;
}



int on_picture_message(wsclient *c, wsclient_message *msg)
 {
        //PRINT("[on_picture_message]\n");
        if( 1 )
        {

        }
        else
        {

        }

	return 0;
}



int on_picture_open(wsclient *c)
{
	//PRINT("[on_picture_open]\n");
	return 0;
}






void Init_Pic_WsClient(wsclient ** c_ws,const char* uri)
{
   *c_ws = libwsclient_new(uri);

   if(!(*c_ws))
   {
	fprintf(stderr, "Unable to initialize new WS client.\n");
	exit(1);
   }
   libwsclient_onopen (       *c_ws, &on_picture_open);
   libwsclient_onmessage(     *c_ws, &on_picture_message);
   libwsclient_onerror(       *c_ws, &on_picture_error);
   libwsclient_onclose(       *c_ws, &on_picture_close);
   libwsclient_helper_socket( *c_ws, "test.sock");
   libwsclient_run(           *c_ws );

}


int Http_Connect_Close( unsigned char* req_packet, unsigned char* recv_buf, unsigned int req_len, unsigned rcv_len, unsigned char* key_word)
{
   int Camera_Net_Fd = 0;
   int Camera_Ret = 0;

   Camera_Net_Fd = socket(AF_INET,SOCK_STREAM,0);
   if( Camera_Net_Fd == -1 )
   {
    printf("create Camera_Net_Fd errpr \r\n");
    return Camera_Net_Fd;
   }
   struct sockaddr_in Camera_Addr;
   Camera_Addr.sin_family = AF_INET;
   Camera_Addr.sin_port = htons( CAMERA_PORT );
   Camera_Addr.sin_addr.s_addr = inet_addr(CAMERA_IP);

   Camera_Ret = connect( Camera_Net_Fd, (struct sockaddr*)&Camera_Addr, sizeof(Camera_Addr) );
   if( Camera_Ret < 0 )
   {
     printf("connect Camera_Ret error\r\n");
     return Camera_Ret;
   }

    int Camera_Recv_Len = 0;
    int Camera_Send_Len = 0;
    int Camera_Pos = 0;
    Camera_Send_Len = write( Camera_Net_Fd, req_packet, req_len);
    if( Camera_Send_Len < 0 )
    {
        return 0;
    }
    while( strstr( recv_buf, key_word) == NULL )
    {
      Camera_Recv_Len = read( Camera_Net_Fd, recv_buf + Camera_Pos, rcv_len);
      Camera_Pos += Camera_Recv_Len;
      printf("Camera_Recv_Len = %d \n",Camera_Recv_Len);
      if( Camera_Recv_Len < 0 )
      {
        return Camera_Recv_Len;
      }
   }
   close( Camera_Net_Fd );
   return Camera_Pos;
}


int copy_word( char* p, char* buf, int buf_len)
{
  int count = 0;
  int i=0;
  while( count!=2 )
  {
     if( *p == '"')
        count++;
     if( 1==count && *p!='"' )
       buf[i++] = *p;
     if( i>buf_len )
      return -1;
     p++;
  }
  return i;
}

int Author_Respon_Pack(const char* au_buf,char* respon_pack,int respon_len, const char* name,const  char* psw, const char* uri, const char* method)
{
  char realm[128] = {0};
  char qop[128] = {0};
  char nonce[128] = {0};
  char cnonce[128] = {0};
  char ha1[128] = {0};
  char ha2[128] = {0};
  char response[128] = {0};
  char tmp[1024] = {0};
  unsigned char hash[64] = {0};
  char* p_start = NULL;
  char* p_nop = NULL;
  char  one_item = 0;
  char  next_item = 0;
  int i=0;
  p_start = strstr( au_buf, "WWW-Authenticate:");

  if( NULL == p_start )
  {
    return -1;
  }
  p_nop = strstr( p_start, REALM);
  copy_word( p_nop, realm,128);

  p_nop = strstr( p_start, QOP);
  copy_word( p_nop, qop, 128);

  p_nop = strstr( p_start, NONCE);
  copy_word( p_nop, nonce, 128);

  sprintf( tmp,"%s:%s:%s",name,realm,psw);
  MD5( tmp, strlen(tmp), hash);

  for( i=0; i<strlen(hash); i++)
   sprintf( &ha1[i*2], "%2.2x", hash[i]);


  memset( hash, 0, sizeof(hash));
  memset( tmp, 0, sizeof(tmp));

  sprintf( tmp,"%s:%s",method,uri);
  MD5( tmp, strlen(tmp), hash);
  for( i=0; i<strlen(hash); i++)
   sprintf( &ha2[i*2], "%2.2x", hash[i]);

  memset( hash, 0, sizeof(hash));
  memset( tmp, 0, sizeof(tmp));

  int  seed = time(NULL);
  sprintf(tmp,"%dimsohandsome",seed);
  MD5( tmp, strlen(tmp), hash);
  for( i=0; i<strlen(hash); i++)
   sprintf( &cnonce[i*2], "%2.2x", hash[i]);

  memset( hash, 0, sizeof(hash));
  memset( tmp, 0, sizeof(tmp));

  sprintf( tmp,"%s:%s:%s:%s:%s:%s", ha1, nonce, "00000001", cnonce, qop, ha2);
  MD5( tmp, strlen(tmp), hash);
  for( i=0; i<strlen(hash); i++)
  sprintf( &response[i*2], "%2.2x", hash[i]);

  sprintf( respon_pack,"GET %s %s\r\nUser-Agent: %s\r\nAuthorization: Digest username=\"%s\",realm=\"%s\",nonce=\"%s\",uri=\"%s\",qop=\"%s\",nc=00000001,cnonce=\"%s\",response=\"%s\"\r\n\r\n",uri, TAIL, name, name,realm,nonce,uri,qop,cnonce,response);


  return 0;
}

int Get_Author( char* request_pack, int req_len, char* author_buf, int buf_size, char* key_word, const char* c_ip)
{

   int Camera_Net_Fd = 0;
   int Camera_Ret = 0;

   Camera_Net_Fd = socket(AF_INET,SOCK_STREAM,0);
   if( Camera_Net_Fd == -1 )
   {
    printf("create Camera_Net_Fd errpr \r\n");
    return Camera_Net_Fd;
   }
   struct sockaddr_in Camera_Addr;
   Camera_Addr.sin_family = AF_INET;
   Camera_Addr.sin_port = htons( CAMERA_PORT );
   Camera_Addr.sin_addr.s_addr = inet_addr( c_ip );

   Camera_Ret = connect( Camera_Net_Fd, (struct sockaddr*)&Camera_Addr, sizeof(Camera_Addr) );
   if( Camera_Ret < 0 )
   {
     printf("connect Camera_Ret error\r\n");
     return Camera_Ret;
   }

    int Camera_Recv_Len = 0;
    int Camera_Send_Len = 0;
    int Camera_Pos = 0;
    Camera_Send_Len = write( Camera_Net_Fd, request_pack, req_len);
    if( Camera_Send_Len < 0 )
    {
        return Camera_Send_Len;
    }
    while( strstr(author_buf, key_word)==NULL )
    {
       Camera_Recv_Len = read( Camera_Net_Fd, author_buf+Camera_Pos, buf_size);
       Camera_Pos += Camera_Recv_Len;
       if( Camera_Recv_Len < 0 )
       {
          return Camera_Recv_Len;
       }
       if( Camera_Pos > buf_size)
       {
          return -1;
       }

    }
    close( Camera_Net_Fd );
    return Camera_Pos;

}



int Http_Recv_Pic( Camera_Type* tmp,unsigned char* video, unsigned char* audio, int v_len, const char* c_ip,int a_len)
{
   int Camera_Net_Fd = 0;
   int Camera_Ret = 0;

   Camera_Net_Fd = socket(AF_INET,SOCK_STREAM,0);
   if( Camera_Net_Fd == -1 )
   {
    printf("create Camera_Net_Fd errpr \r\n");
    return Camera_Net_Fd;
   }
   struct sockaddr_in Camera_Addr;
   Camera_Addr.sin_family = AF_INET;
   Camera_Addr.sin_port = htons( CAMERA_PORT );
   Camera_Addr.sin_addr.s_addr = inet_addr(c_ip);

   Camera_Ret = connect( Camera_Net_Fd, (struct sockaddr*)&Camera_Addr, sizeof(Camera_Addr) );
   if( Camera_Ret < 0 )
   {
     printf("connect Camera_Ret error\r\n");
     return Camera_Ret;
   }
   int Pic_len = 0;
   int Pic_Pos = 0;
   int Rcv_len = 0;

   char Pic_Head[1024] = {0};
   char tmp_buf[ 1024*2] = {0};
   char Pic_Buf[1024*1024] = {0};

   int Con_len = strlen("Content-length: ");
   int Con_tlen = strlen("Content-type: image/jpeg\r\n\r\n");
   int head_pos = 0;


   write( Camera_Net_Fd, video, v_len);
   write( Camera_Net_Fd, audio, a_len);

   int i =0;
   while(1)
   {

     pthread_mutex_lock( &tmp->c_picmutex);
     Rcv_len = 0;
     Rcv_len = read( Camera_Net_Fd, tmp_buf, sizeof(tmp_buf));

     char *head_p = strstr( (char*)tmp_buf, "Content-length: ");
     char *end_p = strstr( (char*)tmp_buf, "Content-type: image/jpeg\r\n\r\n");

     if( head_p != NULL &&  end_p != NULL)
     {
        int len1 = (int)( head_p - (char*)tmp_buf);
        memcpy( Pic_Buf+Pic_Pos, tmp_buf, len1);

      //  printf("pos = %d\n",Pic_Pos);

        libwsclient_send_fragment( tmp->c_client, Pic_Buf , Pic_Pos+len1,  0x82 );

        memset( Pic_Buf, 0, sizeof(Pic_Buf));
        usleep(1000*10);

        Pic_Pos = 0;

	int head_len = (int)(end_p - head_p) + Con_tlen;

	memcpy( Pic_Head, head_p, head_len);
	//PRINT("+++++++++++++++++++++++++++%d\n", i++);


	char len_str[20] = {0};
	char *len_p = strstr( head_p,"\r\n");

	memcpy( len_str, head_p + Con_len, ((int)(len_p - head_p) - Con_len));
      //  printf("len_str = %s\n",len_str);

	Pic_len = atoi( len_str );


	int len2 = Rcv_len - len1 - head_len;
	memcpy( Pic_Buf + Pic_Pos ,end_p + Con_tlen, len2);
	Pic_Pos += len2;


	memset( tmp_buf, 0, sizeof(tmp_buf));
        continue;
    }
    if( head_p != NULL &&  end_p == NULL)
    {

       int len1 = (int)( head_p - (char*)tmp_buf);
       memcpy( Pic_Buf+Pic_Pos, tmp_buf, len1);
       Pic_Pos = 0;

       int len2 = Rcv_len - len1;
       memcpy( Pic_Head, head_p, len2);
       head_pos +=  len2;
       continue;
    }
    if( head_p == NULL &&  end_p != NULL)
    {

       int len1 = ( end_p - (char*)tmp_buf ) + Con_tlen;
       memcpy( Pic_Buf + head_pos, tmp_buf, len1);

       head_pos = 0;


       char *p2 = strstr( Pic_Head, "\r\n");
       char len_str[20] = {0};

       Pic_Pos = 0;
       memcpy( Pic_Buf+Pic_Pos, tmp_buf+len1, (Rcv_len - len1));
       Pic_Pos += len1;
       memset( tmp_buf, 0, sizeof(tmp_buf));
       continue;
    }

    memcpy( Pic_Buf+Pic_Pos, tmp_buf, Rcv_len);
    memset( tmp_buf, 0, sizeof(tmp_buf));
    Pic_Pos += Rcv_len;
    pthread_mutex_unlock( &tmp->c_picmutex);
  }



}



int Send_Media_Req( const char* usr_name, const char* pws, const char* uri, char* response, const char *ip,int size)
{
  char Req_Pack[200] = {0};
  char Rcv_Pack[2048] = {0};
  sprintf( Req_Pack,"GET %s %s\r\n%s%s\r\n\r\n", uri, TAIL, AGENT, usr_name);
  Get_Author( Req_Pack, strlen(Req_Pack), Rcv_Pack, sizeof(Rcv_Pack), "</html>\r\n", ip);
  Author_Respon_Pack( Rcv_Pack, response, size, usr_name, pws, uri,"GET");
  return strlen(response);
}



int bspls_get_pictute( Camera_Type* c_camera)
{

  Init_Pic_WsClient( &c_camera->c_client, c_camera->c_url );

 char video[1024] = {0};
 char audio[1024] = {0};
 Send_Media_Req( c_camera->c_name, c_camera->c_pwd, "/dgvideo.cgi", video, c_camera->c_ip,1024);
 Send_Media_Req( c_camera->c_name, c_camera->c_pwd, "/dgaudio.cgi", audio, c_camera->c_ip,1024);
 printf("%s\n",video);
 printf("%s\n",audio);
 Http_Recv_Pic( c_camera, video, audio,sizeof(video), c_camera->c_ip,sizeof(audio));

 libwsclient_finish( c_camera->c_client );

 return 0;
}



void*  get_pic_thread(void* par)
{
   Camera_Type* tmp = ( Camera_Type* )par;
   printf("%s %s %s %s\r\n", tmp->c_name, tmp->c_pwd, tmp->c_sn, tmp->c_url);
   bspls_get_pictute( tmp );
}


int main()
{
   Camera_Msg C_Msg;
   Camera_Type tmp[CMAERA_NUM] ;

   memset( &C_Msg, 0, sizeof(Camera_Msg) );
   memset( &tmp, 0, sizeof(tmp) );

   int Msg_Id =0;
   int Msg_Ret = 0;
   int i = 0;

   int old_state = 0;
   int old_type  = 0;

  int ret =  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, &old_state);
  printf("set state\r\n");
  if( ret!=0 )
  {
      printf("err1\r\n");
  }

      ret = pthread_setcanceltype(  PTHREAD_CANCEL_ASYNCHRONOUS, &old_type );
  if( ret!=0 )
  {
      printf("err1\r\n");
  }


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

   while(1)
   {

       Msg_Ret = msgrcv( Msg_Id, &C_Msg, sizeof(Camera_Msg), CAMERA_MSG_TYPE, 0);



      // printf("name = %s pwd=%s sn=%s url=%s ip=%s\r\n", C_Msg.msg_name, C_Msg.msg_pwd, C_Msg.msg_sn, C_Msg.msg_url, C_Msg.msg_c_ip);
       switch(C_Msg.cmd)
       {
          case CAMERA_GET_VIDEO:

           for( i=0; i<CMAERA_NUM; i++)
            {
               if( tmp[i].c_picid == 0)
                {
	          break;
	        }
            }
            if( i==CMAERA_NUM )
            {
               printf("The Camera Dev Too Many!!!\n");
               break;
            }
            strcpy( tmp[i].c_name, C_Msg.msg_name);
            strcpy( tmp[i].c_pwd, C_Msg.msg_pwd);
            strcpy( tmp[i].c_sn, C_Msg.msg_sn);
            strcpy( tmp[i].c_url, C_Msg.msg_url);
	    strcpy( tmp[i].c_ip, C_Msg.msg_c_ip);

            pthread_mutex_init( &tmp[i].c_picmutex, 0 );
            pthread_create( &tmp[i].c_picid, NULL, get_pic_thread, &tmp[i]);
            break;
          case CAMERA_CANCEL_GET_VIDEO:

           for( i=0; i<CMAERA_NUM; i++)
	    {
               if( (strcmp(tmp[i].c_url,C_Msg.msg_url) == 0) && (strcmp( tmp[i].c_ip, C_Msg.msg_c_ip) == 0) )
               break;
	    }
            if( i==CMAERA_NUM )
            {
		  printf("The Camera Dev Too Many!!!\n");
                  break;
            }
            if( tmp[i].c_picid != 0 )
            {

               int ret = pthread_cancel(tmp[i].c_picid);

               sleep(1);

               memset( &tmp[i], 0, sizeof(tmp[i]) );

               if( ret!=0 )
               {
                  printf("Cancel Thread Error\r\n");
                  continue;
               }
            }
            else
            {
                 printf("Thread No Exist!!!\r\n");
            }
            break;
         default:
            msgctl( Msg_Id, IPC_RMID, 0 );
            printf("No Such Cmd\r\n");
            return 0;
      }
   }

  msgctl( Msg_Id, IPC_RMID, 0 );
  return 0;

}






