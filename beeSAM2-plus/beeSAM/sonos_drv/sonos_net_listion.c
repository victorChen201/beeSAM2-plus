#include "sonos_drv.h"

//-------------------------------------------------------------------------------------------------*
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SONOS_NET_LISTEN_DBG

#ifndef SONOS_NET_LISTEN_PUTC
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_PUTC(Char)			PUTC(Char)
#else
#define SONOS_NET_LISTEN_PUTC(Char)
#endif
#endif

#ifndef SONOS_NET_LISTEN_PUTS
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SONOS_NET_LISTEN_PUTS(Str, Len)
#endif
#endif

#ifndef SONOS_NET_LISTEN_PUTASCC
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_PUTASCC(Char)			PUTASCC(Char)
#else
#define SONOS_NET_LISTEN_PUTASCC(Char)
#endif
#endif

#ifndef SONOS_NET_LISTEN_PUTASCS
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SONOS_NET_LISTEN_PUTASCS(Str, Len)
#endif
#endif

#ifndef SONOS_NET_LISTEN_PRINT
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SONOS_NET_LISTEN_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SONOS_NET_LISTEN_DYN_PUTC
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SONOS_NET_LISTEN|Dbg), Char)
#else
#define SONOS_NET_LISTEN_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SONOS_NET_LISTEN_DYN_PUTS
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SONOS_NET_LISTEN|Dbg), Str, Len)
#else
#define SONOS_NET_LISTEN_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SONOS_NET_LISTEN_DYN_PUTASCC
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SONOS_NET_LISTEN|Dbg), Char)
#else
#define SONOS_NET_LISTEN_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SONOS_NET_LISTEN_DYN_PUTASCS
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SONOS_NET_LISTEN|Dbg), Str, Len)
#else
#define SONOS_NET_LISTEN_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SONOS_NET_LISTEN_DYN_PRINT
#ifdef SONOS_NET_LISTEN_DBG
#define SONOS_NET_LISTEN_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SONOS_NET_LISTEN|Dbg), Fmt, ##Arg)
#else
#define SONOS_NET_LISTEN_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//-------------------------------------------------------------------------------------------------*

//-------------------------------------------------------------------------------------------------*
#define MUI_SONOS_ALLOW_CONNECT_MAX			10
#define MUI_SONOS_RECV_BUF_SIZE				(1024*64)

#define MUI_SONOS_HTTP_DEAL_RECVED_BODY     3
#define MUI_SONOS_HTTP_DEAL_PARSE_ERR       255
typedef struct
{
	int				ui8Schedule;	//处理进度
	//int				ui8Method;		//方法
	//char				*pchUrl;		//URL
	//int			     ui32BodyLen;	//Body长度
	//int				*pui8Line;		//Line缓冲区
	//int				*pui8Head;		//Head缓冲区
	//int				*pui8Body;		//Body缓冲区
} sonosS_HttpInfo;
typedef struct
{
	int					iConntFd;		/*连接描述符*/
	uint16_t             ui16Id;
	time_t				tTicks;			/*记录的滴答数*/
	int				    *pui8Buf;		/*报文缓冲区*/
	size_t				tLen;			//总报文长度(多个segment总和)

	sonosS_HttpInfo			sHttpInfo;		//HTTP信息
} sonosS_ConntCtrl;
//HTTP信息
typedef struct
{
	size_t				tLen;			/* 报文长度 */
} sonosS_HttpArg;
//-------------------------------------------------------------------------------------------------*
static int sonoss_iSocketRcvFd   = -1;
static int sonoss_iSocketSendFd  = -1;
uint8_t sonos_clear[1024] ={ 0 };
static sonosS_ConntCtrl sonoss_asConntCtrl[MUI_SONOS_ALLOW_CONNECT_MAX];
char  sonosnotifybuf[MUI_SONOS_RECV_BUF_SIZE];
static int s_aui8RecvBuf[MUI_SONOS_ALLOW_CONNECT_MAX][MUI_SONOS_RECV_BUF_SIZE];	/* 接收buf*/
static pthread_t sonoss_sRcvSocketListenThread, sonoss_sHttpMessageDealThread;	/* 线程ID*/
//static sem_t sonoss_tSem;

//-------------------------------------------------------------------------------------------------*
static int sonos_socket_close (void);
static int sonos_socket_init (void);
static int sonos_socket_open (void);
static int sonos_socket_close (void);
static int sonos_net_connt_add (int iFd, uint8_t sonosid);
static int sonos_net_connt_del (int iFd);
static int sonos_net_connt_del_by_idx (int iIdx);
static int sonos_net_connt_update_tick_by_idx (int iIdx);
static int sonos_net_connt_timeout_clean_by_idx (int iIdx);
static void* sonos_rcv_net_manage_listen_task (void *pArg);
static void* sonos_send_net_notify_ok ();
int sonos_net_manage_init ();
int sonos_net_manage_quit_wait (void);
int sonos_drv_get_synchrogena_state(uint16_t ui16Id);
//-------------------------------------------------------------------------------------------------*

//-------------------------------------------------------------------------------------------------*
/*	socket初始化函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:	 <0:出错(包括异常及等待超时)
;				>=0:正常完成
;	(注意)
*/
static int sonos_socket_init (void)
{
	int i;

	sonoss_iSocketRcvFd  = -1;
	sonoss_iSocketSendFd = -1;

	memset(sonoss_asConntCtrl, 0, sizeof(sonoss_asConntCtrl));
	for (i=0; i<MUI_SONOS_ALLOW_CONNECT_MAX; i++)
	{
		sonoss_asConntCtrl[i].iConntFd = -1;
	}

	return sonos_socket_close();
}

//..............................................................................
/*	socket打开函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:	 <0:出错(包括异常及等待超时)
;				>=0:正常完成
;	(注意)
*/
static int sonos_socket_open (void)
{
	struct sockaddr_in sSocketAdd;
	int iRet, iOpt;

	//Creat receive socket for TCP
	sonoss_iSocketRcvFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sonoss_iSocketRcvFd < 0)
	{
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_socket_open]Creat receive socket fail, errno:%d.\r\n", errno);
		return sonoss_iSocketRcvFd;
	}
	iOpt = 1;
	setsockopt(sonoss_iSocketRcvFd, SOL_SOCKET, SO_REUSEADDR, &iOpt, sizeof(iOpt));	//为防止errno:98的出现,允许某个地址的摸个端口被多次绑定.

	//Set receive socket address
	memset(&sSocketAdd, 0, sizeof(sSocketAdd));
	sSocketAdd.sin_family	   = AF_INET;
	sSocketAdd.sin_addr.s_addr = inet_addr(net_listen_ip_addr_get());//sonoss_achNetInfo.s_achListenIP
	sSocketAdd.sin_port		   = htons(3400);//sonoss_achNetInfo.s_ui16ListenPort
	//绑定receive socket地址
	iRet = bind(sonoss_iSocketRcvFd, (struct sockaddr *)&sSocketAdd, sizeof(struct sockaddr));
	if (iRet < 0)
	{
		close(sonoss_iSocketRcvFd);
		sonoss_iSocketRcvFd = -1;
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_socket_open]Bind the receive socket address fail, errno:%d.\r\n", errno);
		return iRet;
	}

	//监听receive socket,允许的队列长度为8
	iRet = listen(sonoss_iSocketRcvFd, 8);
	if (iRet < 0)
	{
		close(sonoss_iSocketRcvFd);
		sonoss_iSocketRcvFd = -1;
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_socket_open]Listen the receive socket fail, errno:%d.\r\n", errno);
		return iRet;
	}

	return iRet;
}
//..............................................................................
/*	socket关闭函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:	 <0:出错(包括异常及等待超时)
;				>=0:正常完成
;	(注意)
*/
static int sonos_socket_close (void)
{
	int iRet = 0;

	if (sonoss_iSocketRcvFd >= 0)
	{
		iRet = close(sonoss_iSocketRcvFd);
		if (0 == iRet)
		{
			sonoss_iSocketRcvFd = -1;
		}
		else
		{
			SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_socket_close]Close FD-%d fail, errno:%d", sonoss_iSocketRcvFd, errno);
		}
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Socket(TCP)增加链接函数
;	(功能)向链接管理组中增加一个新的链接.
;	(参数)
;		iFd:	新增链接的文件描述符
;	(返回)
;		int:	大于等于0的整数,即该链接在管理组中的索引(成功); 负数(失败)
;	(注意)
*/
static int sonos_net_connt_add (int iFd, uint8_t sonosid)
{
	int i;

	//ASSERT(iFd >= 0);

	for (i=0; i<MUI_SONOS_ALLOW_CONNECT_MAX; i++)
	{
		if (sonoss_asConntCtrl[i].iConntFd < 0)
		{
			sonoss_asConntCtrl[i].iConntFd = iFd;
			sonoss_asConntCtrl[i].ui16Id= sonosid;
			time(&sonoss_asConntCtrl[i].tTicks);
			sonoss_asConntCtrl[i].pui8Buf = s_aui8RecvBuf[i];
			SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_net_connt_add]Have added connect sonoss_asConntCtrl[%d], FD=%d, Ticks=%lu\r\n", i, iFd, sonoss_asConntCtrl[i].tTicks);

			return i;
		}
	}

	return -1;
}

//..............................................................................
/*	Socket(TCP)删除链接函数
;	(功能)从链接管理组中删除一个链接.
;	(参数)
;		iFd:		要删除链接的文件描述符
;	(返回)
;		int:		大于等于0的整数,即该链接在管理组中的索引(成功); 负数(失败)
;	(注意)
*/
static int sonos_net_connt_del (int iFd)
{
	int i;

	////ASSERT(iFd >= 0);

	for (i=0; i<MUI_SONOS_ALLOW_CONNECT_MAX; i++)
	{
		if (iFd == sonoss_asConntCtrl[i].iConntFd)
		{
			SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_net_connt_del]Have deleted connect sonoss_asConntCtrl[%d], FD=%d\r\n", i, iFd);
			close(sonoss_asConntCtrl[i].iConntFd);
			memset(&sonoss_asConntCtrl[i], 0, sizeof(sonosS_ConntCtrl));
			sonoss_asConntCtrl[i].iConntFd = -1;

			return i;
		}
	}

	return -1;
}
//..............................................................................
/*	Socket(TCP)根据索引删除链接函数
;	(功能)根据索引从链接管理组中删除一个链接.
;	(参数)
;		iIdx:		要删除链接的索引
;	(返回)
;		int:		大于等于0的整数,即该链接的文件描述符(成功); 负数(失败)
;	(注意)
*/
static int sonos_net_connt_del_by_idx (int iIdx)
{
	int iTmp;

	////ASSERT(iIdx < MUI_SONOS_ALLOW_CONNECT_MAX);

	if (sonoss_asConntCtrl[iIdx].iConntFd >= 0)
	{
		iTmp = sonoss_asConntCtrl[iIdx].iConntFd;
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_net_connt_del_by_idx]Have deleted connect sonoss_asConntCtrl[%d], FD=%d\r\n", iIdx, iTmp);
		close(sonoss_asConntCtrl[iIdx].iConntFd);
		memset(&sonoss_asConntCtrl[iIdx], 0, sizeof(sonosS_ConntCtrl));
		sonoss_asConntCtrl[iIdx].iConntFd = -1;

		return iTmp;
	}

	return -1;
}
//..............................................................................
/*	Socket(TCP)根据索引更新链接Ticks函数
;	(功能)根据索引更新链接管理组中指定的链接的Ticks.
;	(参数)
;		iIdx:		要更新的链接的索引
;	(返回)
;		int:		大于等于0的整数,即该链接的文件描述符(成功); 负数(失败)
;	(注意)
*/
static int sonos_net_connt_update_tick_by_idx (int iIdx)
{
	////ASSERT(iIdx < MUI_SONOS_ALLOW_CONNECT_MAX);

	if (sonoss_asConntCtrl[iIdx].iConntFd >= 0)
	{
		time(&sonoss_asConntCtrl[iIdx].tTicks);

		return sonoss_asConntCtrl[iIdx].iConntFd;
	}

	return -1;
}
//..............................................................................
/*	Socket(TCP)根据索引清理超时链接函数
;	(功能)根据索引检查接管理组中指定的链接,如果超时则清理它.
;	(参数)
;		iIdx:		要检查的链接的索引
;	(返回)
;		int:		 1:超时,且已被清理
;					 0:未超时
;					-1:无效的索引
;	(注意)
*/
static int sonos_net_connt_timeout_clean_by_idx (int iIdx)
{
	time_t tTicks;

	////ASSERT(iIdx < MUI_SONOS_ALLOW_CONNECT_MAX);

	if (sonoss_asConntCtrl[iIdx].iConntFd >= 0)
	{
		//任何已建立连接都必须检查是否超时(60s).
		//tTicks = sonoss_asConntCtrl[iIdx].tTicks;
		//time(&sonoss_asConntCtrl[iIdx].tTicks);
		time(&tTicks);
		if ((/*sonoss_asConntCtrl[iIdx].tTicks-tTicks*/tTicks - sonoss_asConntCtrl[iIdx].tTicks) > 60)	//!!!此处可能会带来潜在的兼容性问题,如果time_t是64位的,而如果所在系统不支持64位数据四则运算的话,则会出现混乱
		{
			//超过指定间隔时间未收过任何数据的链接,强行关闭它.
			SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_net_connt_timeout_clean_by_idx]Have closed sonoss_asConntCtrl[%d], FD=%d\r\n", iIdx, sonoss_asConntCtrl[iIdx].iConntFd);
			sonos_net_connt_del_by_idx(iIdx);
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	接收Socket监听任务函数
;	(功能)监听指定的端口,等待(请求的)报文.
;	(参数)
;		pArg:	传给函数运行的参数
;	(返回)
;		NULL
;	(注意)
*/
static void* sonos_rcv_net_manage_listen_task (void *pArg)
{
	struct sockaddr_in sClientAdd;
	socklen_t tRcvLen;
	struct timeval sTime;
	fd_set sRdFdSet;
	int iMaxFd, iFd, iLen, iTmp,i;
	int sonosidtmp;
	sonosS_HttpArg harg;
	char *ipaddr,ipbuf[MUI_DEV_IP_ADDR_LEN+1]={0};
	S_ZbAttrNwkAddr sNwkAddr;
	S_ZbAttrEp sEp;

	const char buf[MUI_SONOS_RECV_BUF_SIZE]= "HTTP/1.1 200 OK\r\n"
									   "Server: Linux UPnP/1.0 Sonos/27.2-81200 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n"
									   "Connection: close\r\n"
									   "\r\n";
	//UNUSED(pArg);

	while (sonoss_iSocketRcvFd > 0)
	{
		//Timeout setting
		sTime.tv_sec  = 10;
		sTime.tv_usec = 0;

		//Initialize read file descriptor set
		FD_ZERO(&sRdFdSet);
		FD_SET(sonoss_iSocketRcvFd, &sRdFdSet);
		iMaxFd = sonoss_iSocketRcvFd;

		for (i=0; i<MUI_SONOS_ALLOW_CONNECT_MAX; i++)
		{
			if ((sonoss_asConntCtrl[i].iConntFd >= 0) && (sonoss_asConntCtrl[i].sHttpInfo.ui8Schedule != MUI_SONOS_HTTP_DEAL_RECVED_BODY))
			{
				if (0 <= fcntl(sonoss_asConntCtrl[i].iConntFd, F_GETFL))
				{
					iFd = sonoss_asConntCtrl[i].iConntFd;
					FD_SET(iFd, &sRdFdSet);
					if (iFd > iMaxFd)
					{
						iMaxFd = iFd;
					}
				}
				else
				{
					SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_rcv_net_manage_listen_task]sonoss_asConntCtrl[%d].iConntFd=%d was closed, now will delete it.\r\n", i, sonoss_asConntCtrl[i].iConntFd);
					sonos_net_connt_del_by_idx(i);
				}
			}
		}

		iTmp = select(iMaxFd+1, &sRdFdSet, NULL, NULL, &sTime);
		if (iTmp < 0)
		{
			//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_rcv_net_manage_listen_task]Call 'select' fail, errno:%d\r\n", errno);
			//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_rcv_net_manage_listen_task]sonoss_iSocketRcvFd=%d\r\n", sonoss_iSocketRcvFd);
			//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_rcv_net_manage_listen_task]------------------------------------------\r\n");
			//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_rcv_net_manage_listen_task]Bad file number list as bellow:\r\n");
			for (i=0; i<MUI_SONOS_ALLOW_CONNECT_MAX; i++)
			{
				if (sonoss_asConntCtrl[i].iConntFd >= 0)
				{
					if (0 > fcntl(sonoss_asConntCtrl[i].iConntFd, F_GETFL))
					{
						SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]sonoss_asConntCtrl[%d].iConntFd=%d, but can't find this file number.\r\n", i, sonoss_asConntCtrl[i].iConntFd);
					}
				}
			}
		}
		else if (0 == iTmp)
		{
			SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]Call 'select' time out, now will close all of timeout connections.\r\n");
			for (i=0; i<MUI_SONOS_ALLOW_CONNECT_MAX; i++)
			{
				if (sonoss_asConntCtrl[i].sHttpInfo.ui8Schedule != MUI_SONOS_HTTP_DEAL_RECVED_BODY)
					sonos_net_connt_timeout_clean_by_idx(i);
			}
		}
		else //(iRet > 0)
		{
			SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_rcv_net_manage_listen_task]Call 'select', have find some FD ready.\r\n");

			//Check every FD in the set
			for (i=0; i<MUI_SONOS_ALLOW_CONNECT_MAX; i++)
			{
				if ((sonoss_asConntCtrl[i].iConntFd >= 0) && (sonoss_asConntCtrl[i].sHttpInfo.ui8Schedule != MUI_SONOS_HTTP_DEAL_RECVED_BODY))
				{
					if (FD_ISSET(sonoss_asConntCtrl[i].iConntFd, &sRdFdSet))
					{
						iFd = sonoss_asConntCtrl[i].iConntFd;
						//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_rcv_net_manage_listen_task]Have received message from sonoss_asConntCtrl[%d], FD=%d\r\n", i, iFd);
						iLen = read(iFd, &sonoss_asConntCtrl[i].pui8Buf[sonoss_asConntCtrl[i].tLen], MUI_SONOS_RECV_BUF_SIZE-sonoss_asConntCtrl[i].tLen);
						if (iLen <= 0)
						{
							//Find the connection has broken, then close this FD.
							sonos_net_connt_del_by_idx(i);
							SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_rcv_net_manage_listen_task]Have deleted sonoss_asConntCtrl[%d], FD=%d\r\n", i, iFd);
						}
						else
						{
							//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_rcv_net_manage_listen_task] receive message successful from sonoss_asConntCtrl[%d], FD=%d\r\n", i, iFd);
							//Have got message, will deal it.
							sonos_net_connt_update_tick_by_idx(i);/* 收到一次包后就更新一次对iFd tick*/
							harg.tLen = iLen;
							//由于前面read函数已经将接受到的报文续接到指定缓冲区,在此处,仅仅传入续接的报文长度即可.

							//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]:%s\r\n",s_aui8RecvBuf[i]);

							memcpy(&sonosnotifybuf,s_aui8RecvBuf[i],MUI_SONOS_RECV_BUF_SIZE);
							//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]:%s\r\n",sonosnotifybuf);
							send(iFd,buf,sizeof(buf),0);

							iTmp = sonos_drv_get_synchrogena_state(sonoss_asConntCtrl[i].ui16Id);//,MUI_SONOS_RECV_BUF_SIZE
							/*if (MUI_SONOS_HTTP_DEAL_RECVED_BODY == iTmp)
							{
								SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]Have received all message into sonoss_asConntCtrl[%d], FD=%d\r\n", i, iFd);
								sem_post(&sonoss_tSem);
							}
							else if (MUI_SONOS_HTTP_DEAL_PARSE_ERR == iTmp)
							{
								//Maybe should to delete this connect.
								SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"---------------------++++++++++++++++++++++++++\r\n");
							}*/
						}
					}
					else
					{
						sonos_net_connt_timeout_clean_by_idx(i);
					}
				}
			}

			//Check whether a new connection request comes
			if (FD_ISSET(sonoss_iSocketRcvFd, &sRdFdSet))
			{
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_rcv_net_manage_listen_task]Have received connect request\r\n");
				tRcvLen = sizeof(sClientAdd);
				iFd = accept(sonoss_iSocketRcvFd, (struct sockaddr*)&sClientAdd, &tRcvLen);
				if (iFd < 0)
				{
					SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_rcv_net_manage_listen_task]Can not creat connect with %#x:%d\r\n", ntohl(sClientAdd.sin_addr.s_addr), ntohs(sClientAdd.sin_port));
					continue;
				}



				ipaddr=inet_ntoa(sClientAdd.sin_addr);
				strcpy(ipbuf,ipaddr);
				memcpy(sNwkAddr.ip, ipbuf, sizeof(sNwkAddr.ip));
				sEp.port = 1400;
				sonosidtmp = chk_sonos_zb_nwkaddr_ep(&sNwkAddr, &sEp);
				if (sonosidtmp < 0)
				{
					SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_rcv_net_manage_listen_task]Can not find Sonos with IP:%s\r\n", sNwkAddr.ip);
					continue;
				}
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]sonosidtmp = %d\r\n",sonosidtmp);
				//Add to connected control group.
				iTmp = sonos_net_connt_add(iFd, (uint8_t)sonosidtmp);
				if (iTmp < 0)
				{
					//Connected links have overflow, must close current link.
					SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]Connect nummber have overflow, now have to break connect request from %#x:%d.", ntohl(sClientAdd.sin_addr.s_addr), ntohs(sClientAdd.sin_port));
					close(iFd);
					continue;
				}

				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++++++++++++++++\r\n");
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]Have accept connect request from %#x:%d\r\n", ntohl(sClientAdd.sin_addr.s_addr), ntohs(sClientAdd.sin_port));
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_rcv_net_manage_listen_task]Have accept connect request from %s\r\n", ipbuf);
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++++++++++++++++\r\n");
			}
		}
	}

	return NULL;
}

static void* sonos_send_net_notify_ok ()
{
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_send_net_notify_ok]$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Socket管理初始化函数
;	(功能)
;	(参数)
;		pchListenIP:	收报文IP地址
;		ui16ListenPort:	收报文端口
;		pchReportIP:	发报文IP地址
;		ui16ReportPort:	发报文端口
;	(返回)
;		int:			 <0:出错(包括异常及等待超时)
;						>=0:正常完成
;	(注意)
*/
//int sonos_net_manage_init (const char *pchListenIP, int ui16ListenPort, const char *pchReportIP, int ui16ReportPort)
int sonos_net_manage_init ()
{
	int iRet;
/*
	iRet = sem_init(&sonoss_tSem, 0, 0);
	if (iRet < 0)
	{
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_net_manage_init]Initialize 'sonoss_tSem' fail, errno:%d.\r\n", errno);
		return iRet;
	}
*/
	iRet = sonos_socket_init();
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = sonos_socket_open();
	if (iRet < 0)
	{
		return iRet;
	}


/*
	iRet = pthread_create(&sonoss_sHttpMessageDealThread, NULL, sonos_send_net_notify_ok, NULL);
	if (iRet < 0)
	{
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_net_manage_init]Create thread 'net_manage_api_deal_task' fail, errno:%d.\r\n", errno);
		sonos_socket_close();
		return iRet;
	}
*/
	iRet = pthread_create(&sonoss_sRcvSocketListenThread, NULL, sonos_rcv_net_manage_listen_task, NULL);
	if (iRet < 0)
	{
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_net_manage_init]Create thread 'sonos_rcv_net_manage_listen_task' fail, errno:%d.\r\n", errno);
		sonos_socket_close();
		return iRet;
	}

	SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"sonos_net_manage_init!\r\n");

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Socket管理退出等待函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:			 <0:出错(包括异常及等待超时)
;						>=0:正常退出
;	(注意)
*/
int sonos_net_manage_quit_wait (void)
{
	int iRet;
/*
	iRet = pthread_join(sonoss_sRcvSocketListenThread, NULL);
	if (iRet != 0)
	{
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_net_manage_quit_wait]Joining thread 'sonos_rcv_net_manage_listen_task' fail, errno:%d\r\n", errno);
		abort();
	}*/

	iRet = pthread_join(sonoss_sHttpMessageDealThread, NULL);
	if (iRet != 0)
	{
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_net_manage_quit_wait]Joining thread 'net_manage_api_deal_task' fail, errno:%d\r\n", errno);
		abort();
	}

	iRet = sonos_socket_close();

	return iRet;
}

int sonos_drv_get_synchrogena_state(uint16_t ui16Id)//, size_t tLen
{
	char *pchTmp,*pchTmp1,*pchTmp2;
	int iRet;
	char chstateplay[128];
	char chstateurltmp[1024];
	uint16_t statevolume = 0;
	uint8_t	 uint8_play = 1;
	uint8_t	 uint8_pause = 0;
	//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state]=======%s\r\n",sonosnotifybuf);

	pchTmp = strstr(sonosnotifybuf, "TransportState val=&quot;");//TransportState val=&quot;
	pchTmp2 = strstr(sonosnotifybuf, "CurrentTrackURI val=&quot;");
	pchTmp1 = strstr(sonosnotifybuf,"Volume channel=&quot;Master&quot; val=&quot;");
	//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state]%s\r\n",pchTmp);
	//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state]%s\r\n",pchTmp2);
	//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state]%s\r\n",pchTmp1);
	if((pchTmp!=NULL)||(pchTmp2!=NULL))
	{
		if(pchTmp!=NULL)
		{
			iRet = sscanf(pchTmp+sizeof("TransportState val=&quot;")-1, "%s&quot;/&gt;&lt;CurrentPlayMode", chstateplay);
			if(0==strcmp(chstateplay,"PLAYING&quot;/&gt;&lt;CurrentPlayMode"))
			{
				iRet = write_sonos_state_play(ui16Id, &uint8_play);
				//iRet = File_Write_Data(MUI_DATA_TYPE_SONOS, ui16Id, &uint8_play, M_OFFSET(U_DevStateSave, sonos_state.state.play), M_SIZEOF(U_DevStateSave, sonos_state.state.play), M_MEM_TYPE_RAM);
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++++++++++++++++\r\n");
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] state playing\r\n");
			}
			else if(0==strcmp(chstateplay,"PAUSED_PLAYBACK&quot;/&gt;&lt;CurrentPlayMode"))
			{
				iRet = write_sonos_state_play(ui16Id, &uint8_pause);
				//iRet =  File_Write_Data(MUI_DATA_TYPE_SONOS, ui16Id, &uint8_pause, M_OFFSET(U_DevStateSave, sonos_state.state.play), M_SIZEOF(U_DevStateSave, sonos_state.state.play), M_MEM_TYPE_RAM);
				//set_sonos_state_play(ui16Id, &uint8_pause);
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++++++++++++++++\r\n");
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] state pause_back\r\n");
			}
			else
			{
				iRet = write_sonos_state_play(ui16Id, &uint8_pause);
				//iRet =  File_Write_Data(MUI_DATA_TYPE_SONOS, ui16Id, &uint8_pause, M_OFFSET(U_DevStateSave, sonos_state.state.play), M_SIZEOF(U_DevStateSave, sonos_state.state.play), M_MEM_TYPE_RAM);
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++++++++++++++++\r\n");
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] state stop or none\r\n");
			}
		}
		if(pchTmp2!=NULL)
		{
			memcpy(&MUI_SONOS_MUSIC_URL,sonos_clear,sizeof(MUI_SONOS_MUSIC_URL));
			iRet = sscanf(pchTmp2+sizeof("CurrentTrackURI val=&quot;")-1, "%[^;]%[^&]",MUI_SONOS_MUSIC_URL,chstateurltmp);
			if(iRet==2)
			{
				strcat(MUI_SONOS_MUSIC_URL,chstateurltmp);
				if(0==strcmp(MUI_SONOS_MUSIC_URL,"&quot;/"))
				{
					//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] %d\r\n",iRet);
					//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] %s\r\n",MUI_SONOS_MUSIC_URL);
					memcpy(&MUI_SONOS_MUSIC_URL,sonos_clear,sizeof(MUI_SONOS_MUSIC_URL));
				}
				//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] %s\r\n",MUI_SONOS_MUSIC_URL);
			}
			else
			{
				SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] %s\r\n",MUI_SONOS_MUSIC_URL);
				SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] %d\r\n",iRet);
			}
		}

	}
	if(pchTmp1!=NULL)
	{
		iRet = sscanf(pchTmp1+sizeof("Volume channel=&quot;Master&quot; val=&quot;")-1, "%hd",&statevolume);
		if((statevolume>=0)||(statevolume<=100))
		{
			iRet = write_sonos_state_volume(ui16Id, &statevolume);
			//iRet = File_Write_Data(MUI_DATA_TYPE_SONOS, ui16Id, &statevolume, M_OFFSET(U_DevStateSave, sonos_state.state.volume), M_SIZEOF(U_DevStateSave, sonos_state.state.volume), M_MEM_TYPE_RAM);
			//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++++++++++++++++\r\n");
			//SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state] volume =  %d\r\n", statevolume);
		}
	}
	/*
	else
	{
		SONOS_NET_LISTEN_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_get_synchrogena_state]can't find state\r\n");
		return MUI_SONOS_HTTP_DEAL_PARSE_ERR;
	}*/
	return MUI_SONOS_HTTP_DEAL_RECVED_BODY;
	memcpy(sonosnotifybuf,sonos_clear,MUI_SONOS_RECV_BUF_SIZE);
}
/*
int main()
{
		sonos_net_manage_init();
		while(1)
		{;}
}*/
