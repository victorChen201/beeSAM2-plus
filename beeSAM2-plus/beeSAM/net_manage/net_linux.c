/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of net_linux module for beeSAM project.
File name: net_linux.c
Description:

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "net_linux.h"
#include "net_manage.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define NET_LINUX_DBG

#ifndef NET_LINUX_PUTC
#ifdef NET_LINUX_DBG
#define NET_LINUX_PUTC(Char)			PUTC(Char)
#else
#define NET_LINUX_PUTC(Char)
#endif
#endif

#ifndef NET_LINUX_PUTS
#ifdef NET_LINUX_DBG
#define NET_LINUX_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define NET_LINUX_PUTS(Str, Len)
#endif
#endif

#ifndef NET_LINUX_PUTASCC
#ifdef NET_LINUX_DBG
#define NET_LINUX_PUTASCC(Char)			PUTASCC(Char)
#else
#define NET_LINUX_PUTASCC(Char)
#endif
#endif

#ifndef NET_LINUX_PUTASCS
#ifdef NET_LINUX_DBG
#define NET_LINUX_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define NET_LINUX_PUTASCS(Str, Len)
#endif
#endif

#ifndef NET_LINUX_PRINT
#ifdef NET_LINUX_DBG
#define NET_LINUX_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define NET_LINUX_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef NET_LINUX_DYN_PUTC
#ifdef NET_LINUX_DBG
#define NET_LINUX_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_NET_LINUX|Dbg), Char)
#else
#define NET_LINUX_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef NET_LINUX_DYN_PUTS
#ifdef NET_LINUX_DBG
#define NET_LINUX_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_NET_LINUX|Dbg), Str, Len)
#else
#define NET_LINUX_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef NET_LINUX_DYN_PUTASCC
#ifdef NET_LINUX_DBG
#define NET_LINUX_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_NET_LINUX|Dbg), Char)
#else
#define NET_LINUX_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef NET_LINUX_DYN_PUTASCS
#ifdef NET_LINUX_DBG
#define NET_LINUX_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_NET_LINUX|Dbg), Str, Len)
#else
#define NET_LINUX_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef NET_LINUX_DYN_PRINT
#ifdef NET_LINUX_DBG
#define NET_LINUX_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_NET_LINUX|Dbg), Fmt, ##Arg)
#else
#define NET_LINUX_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif


#if 0
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MSTR_SOCKET_LISTEN_IP_DEFT		"127.0.0.1"
#define MUI_SOCKET_LISTEN_PORT_DEFT		7890

#define MSTR_SOCKET_REPORT_IP_DEFT		"127.0.0.1"
#define MUI_SOCKET_REPORT_PORT_DEFT		7891

#define MUI_ALLOW_CONNECT_MAX			10
#define MUI_API_RECV_BUF_SIZE				(1024*3)
#define MUI_API_ANSWER_BUF_SIZE				(1024*64)
#endif




//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
/*static char s_achNetMask[16];
static char s_achGatewayIP[16];
static char s_achListenIP[16];
static uint16_t s_ui16ListenPort;
static char s_achReportIP[16];
static uint16_t s_ui16ReportPort;*/
static S_SockAddr s_achNetInfo;

static int s_iSocketRcvFd  = -1;										/* SOCKET 接收文件描述符*/
static int s_iSocketSendFd = -1;										/* SOCKET 发送文件描述符*/
static pthread_t s_sRcvSocketListenThread, s_sHttpMessageDealThread;	/* 线程ID*/
static sem_t s_tSem;													/* 信号量*/

static S_ConntCtrl s_asConntCtrl[MUI_ALLOW_CONNECT_MAX];				/* 协议控制结构*/
static uint8_t s_aui8RecvBuf[MUI_ALLOW_CONNECT_MAX][MUI_API_RECV_BUF_SIZE];	/* 接收buf*/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int socket_init (void);
static int socket_open (void);
static int socket_close (void);

static int net_connt_add (int iFd);
static int net_connt_add_by_idx (int iIdx, int iFd);

static int net_connt_del (int iFd);
static int net_connt_del_by_idx (int iIdx);

static int net_connt_rst_by_idx (int iIdx);
static int net_connt_update_tick_by_idx (int iIdx);

static int net_connt_timeout_clean_by_idx (int iIdx);

//static int socket_http_message_recv(S_ConntCtrl *psConntCtrl, int iLen);
static void* net_manage_api_listen_task (void *pArg);
static void* net_manage_api_deal_task (void *pArg);

static S_SockAddr *get_net_mamage_netinfo(void);

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
static int socket_init (void)
{
	int i;

	s_iSocketRcvFd  = -1;
	s_iSocketSendFd = -1;

	memset(s_asConntCtrl, 0, sizeof(s_asConntCtrl));
	for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
	{
		s_asConntCtrl[i].iConntFd = -1;
	}

	socket_close();

	return 0;
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
static int socket_open (void)
{
	struct sockaddr_in sSocketAdd;
	int iRet, iOpt;

	//Creat receive socket for TCP
	s_iSocketRcvFd = socket(AF_INET, SOCK_STREAM, 0);
	if (s_iSocketRcvFd < 0)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[socket_open]Creat receive socket fail, errno:%d.\r\n", errno);
		return s_iSocketRcvFd;
	}
	iOpt = 1;
	setsockopt(s_iSocketRcvFd, SOL_SOCKET, SO_REUSEADDR, &iOpt, sizeof(iOpt));	//为防止errno:98的出现,允许某个地址的摸个端口被多次绑定.

	//Set receive socket address
	memset(&sSocketAdd, 0, sizeof(sSocketAdd));
	sSocketAdd.sin_family	   = AF_INET;
	sSocketAdd.sin_addr.s_addr = inet_addr(s_achNetInfo.s_achListenIP);
	sSocketAdd.sin_port		   = htons(s_achNetInfo.s_ui16ListenPort);

	//绑定receive socket地址
	iRet = bind(s_iSocketRcvFd, (struct sockaddr *)&sSocketAdd, sizeof(struct sockaddr));
	if (iRet < 0)
	{
		close(s_iSocketRcvFd);
		s_iSocketRcvFd = -1;
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[socket_open]Bind the receive socket address fail, errno:%d.\r\n", errno);
		return iRet;
	}

	//监听receive socket,允许的队列长度为8
	iRet = listen(s_iSocketRcvFd, 8);
	if (iRet < 0)
	{
		close(s_iSocketRcvFd);
		s_iSocketRcvFd = -1;
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[socket_open]Listen the receive socket fail, errno:%d.\r\n", errno);
		return iRet;
	}

	return 0;
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
static int socket_close (void)
{
	int iRet = 0;

	if (s_iSocketRcvFd >= 0)
	{
		iRet = close(s_iSocketRcvFd);
		if (0 == iRet)
		{
			s_iSocketRcvFd = -1;
		}
		else
		{
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[socket_close]Close FD-%d fail, errno:%d", s_iSocketRcvFd, errno);
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
static int net_connt_add (int iFd)
{
	int i;

	ASSERT(iFd >= 0);

	for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
	{
		if (s_asConntCtrl[i].iConntFd < 0)
		{
			s_asConntCtrl[i].iConntFd = iFd;
			time(&s_asConntCtrl[i].tTicks);
			s_asConntCtrl[i].pui8Buf = s_aui8RecvBuf[i];
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_connt_add]Have added connect s_asConntCtrl[%d], FD=%d, Ticks=%lu\r\n", i, iFd, s_asConntCtrl[i].tTicks);

			return i;
		}
	}

	return -1;
}
//..............................................................................
/*	Socket(TCP)根据索引增加链接函数
;	(功能)向链接管理组中增加一个新的链接.
;	(参数)
;		iIdx:	要增加链接数据的位置索引
;		iFd:	新增链接的文件描述符
;	(返回)
;		int:	大于等于0的整数,即该链接的文件描述符(成功); 负数(失败)
;	(注意)
*/
#if 0
static int net_connt_add_by_idx (int iIdx, int iFd)
{
	ASSERT(iIdx < MUI_ALLOW_CONNECT_MAX);
	ASSERT(iFd >= 0);

	if (s_asConntCtrl[iIdx].iConntFd < 0)
	{
		s_asConntCtrl[iIdx].iConntFd = iFd;
		time(&s_asConntCtrl[iIdx].tTicks);
		s_asConntCtrl[iIdx].pui8Buf = s_aui8RecvBuf[iIdx];
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_connt_add_by_idx]Have added connect s_asConntCtrl[%d], FD=%d, Ticks=%lu\r\n", iIdx, iFd, s_asConntCtrl[iIdx].tTicks);

		return iFd;
	}

	return -1;
}
#endif
//..............................................................................
/*	Socket(TCP)删除链接函数
;	(功能)从链接管理组中删除一个链接.
;	(参数)
;		iFd:		要删除链接的文件描述符
;	(返回)
;		int:		大于等于0的整数,即该链接在管理组中的索引(成功); 负数(失败)
;	(注意)
*/
static int net_connt_del (int iFd)
{
	int i;

	ASSERT(iFd >= 0);

	for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
	{
		if (iFd == s_asConntCtrl[i].iConntFd)
		{
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_connt_del]Have deleted connect s_asConntCtrl[%d], FD=%d\r\n", i, iFd);
			close(s_asConntCtrl[i].iConntFd);
			memset(&s_asConntCtrl[i], 0, sizeof(S_ConntCtrl));
			s_asConntCtrl[i].iConntFd = -1;

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
static int net_connt_del_by_idx (int iIdx)
{
	int iTmp;

	ASSERT(iIdx < MUI_ALLOW_CONNECT_MAX);

	if (s_asConntCtrl[iIdx].iConntFd >= 0)
	{
		iTmp = s_asConntCtrl[iIdx].iConntFd;
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_connt_del_by_idx]Have deleted connect s_asConntCtrl[%d], FD=%d\r\n", iIdx, iTmp);
		close(s_asConntCtrl[iIdx].iConntFd);
		memset(&s_asConntCtrl[iIdx], 0, sizeof(S_ConntCtrl));
		s_asConntCtrl[iIdx].iConntFd = -1;

		return iTmp;
	}

	return -1;
}
//..............................................................................
/*	Socket(TCP)根据索引复位链接函数
;	(功能)根据索引将链接管理组中指定链接复位.
;	(参数)
;		iIdx:		要复位链接的索引
;	(返回)
;		int:		大于等于0的整数,即该链接的文件描述符(成功); 负数(失败)
;	(注意)
*/
#if 0
static int net_connt_rst_by_idx (int iIdx)
{
	ASSERT(iIdx < MUI_ALLOW_CONNECT_MAX);

	if ((s_asConntCtrl[iIdx].iConntFd>=0) && (s_asConntCtrl[iIdx].sHttpInfo.ui8Schedule==MUI_HTTP_DEAL_RECVED_BODY))
	{
		time(&s_asConntCtrl[iIdx].tTicks);
		s_asConntCtrl[iIdx].pui8Buf = s_aui8RecvBuf[iIdx];
		s_asConntCtrl[iIdx].tLen    = 0;
	#if 1
		s_asConntCtrl[iIdx].sHttpInfo.ui8Method	  = MUI_HTTP_METHOD_VOID;
		s_asConntCtrl[iIdx].sHttpInfo.pchUrl	  = NULL;
		s_asConntCtrl[iIdx].sHttpInfo.pui8Line	  = NULL;
		s_asConntCtrl[iIdx].sHttpInfo.pui8Head	  = NULL;
		s_asConntCtrl[iIdx].sHttpInfo.pui8Body	  = NULL;
		s_asConntCtrl[iIdx].sHttpInfo.ui16UrlLen  = 0;
		s_asConntCtrl[iIdx].sHttpInfo.ui32BodyLen = 0;
		s_asConntCtrl[iIdx].sHttpInfo.ui8Schedule = MUI_HTTP_DEAL_WAIT_MESSAGE;
	#else
		//采用这种方式,因ui8Schedule被设置为MUI_HTTP_DEAL_WAIT_MESSAGE比结构体中其它变量设置得早,从而可能会引起竞争冲突.
		memset(&s_asConntCtrl[iIdx].sHttpInfo, 0, sizeof(s_asConntCtrl[iIdx].sHttpInfo));
	#endif
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_connt_rst_by_idx]Have reset connect s_asConntCtrl[%d], FD=%d\r\n", iIdx, s_asConntCtrl[iIdx].iConntFd);

		return s_asConntCtrl[iIdx].iConntFd;
	}

	return -1;
}
#endif
//..............................................................................
/*	Socket(TCP)根据索引更新链接Ticks函数
;	(功能)根据索引更新链接管理组中指定的链接的Ticks.
;	(参数)
;		iIdx:		要更新的链接的索引
;	(返回)
;		int:		大于等于0的整数,即该链接的文件描述符(成功); 负数(失败)
;	(注意)
*/
static int net_connt_update_tick_by_idx (int iIdx)
{
	ASSERT(iIdx < MUI_ALLOW_CONNECT_MAX);

	if (s_asConntCtrl[iIdx].iConntFd >= 0)
	{
		time(&s_asConntCtrl[iIdx].tTicks);

		return s_asConntCtrl[iIdx].iConntFd;
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
static int net_connt_timeout_clean_by_idx (int iIdx)
{
	time_t tTicks;

	ASSERT(iIdx < MUI_ALLOW_CONNECT_MAX);

	if (s_asConntCtrl[iIdx].iConntFd >= 0)
	{
		//任何已建立连接都必须检查是否超时(60s).
		//tTicks = s_asConntCtrl[iIdx].tTicks;
		//time(&s_asConntCtrl[iIdx].tTicks);
		time(&tTicks);
		if ((/*s_asConntCtrl[iIdx].tTicks-tTicks*/tTicks - s_asConntCtrl[iIdx].tTicks) > 60)	//!!!此处可能会带来潜在的兼容性问题,如果time_t是64位的,而如果所在系统不支持64位数据四则运算的话,则会出现混乱
		{
			//超过指定间隔时间未收过任何数据的链接,强行关闭它.
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_connt_timeout_clean_by_idx]Have closed s_asConntCtrl[%d], FD=%d\r\n", iIdx, s_asConntCtrl[iIdx].iConntFd);
			net_connt_del_by_idx(iIdx);
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
/*	接收Socket监听任务函数
;	(功能)监听指定的端口,等待(请求的)报文.
;	(参数)
;		pArg:	传给函数运行的参数
;	(返回)
;		NULL
;	(注意)
*/
static void* net_manage_api_listen_task (void *pArg)
{
	struct sockaddr_in sClientAdd;
	socklen_t tRcvLen;
	struct timeval sTime;
	fd_set sRdFdSet;
	int iMaxFd, iFd, iLen, iTmp,i;
	S_HttpArg harg;

	UNUSED(pArg);

	while (s_iSocketRcvFd > 0)
	{
		//Timeout setting
		sTime.tv_sec  = 10;
		sTime.tv_usec = 0;

		//Initialize read file descriptor set
		FD_ZERO(&sRdFdSet);
		FD_SET(s_iSocketRcvFd, &sRdFdSet);
		iMaxFd = s_iSocketRcvFd;

		for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
		{
			if ((s_asConntCtrl[i].iConntFd >= 0) && (s_asConntCtrl[i].sHttpInfo.ui8Schedule != MUI_HTTP_DEAL_RECVED_BODY))
			{
				//if (0 <= fcntl(s_asConntCtrl[i].iConntFd, F_GETFL))
				{
					iFd = s_asConntCtrl[i].iConntFd;
					FD_SET(iFd, &sRdFdSet);
					if (iFd > iMaxFd)
					{
						iMaxFd = iFd;
					}
				}
				//else
				//{
				//	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]s_asConntCtrl[%d].iConntFd=%d was closed, now will delete it.\r\n", i, s_asConntCtrl[i].iConntFd);
				//	net_connt_del_by_idx(i);
				//}
			}
		}

		iTmp = select(iMaxFd+1, &sRdFdSet, NULL, NULL, &sTime);
		if (iTmp < 0)
		{
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_api_listen_task]Call 'select' fail, errno:%d\r\n", errno);
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]s_iSocketRcvFd=%d\r\n", s_iSocketRcvFd);
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]------------------------------------------\r\n");
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Bad file number list as bellow:\r\n");
			for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
			{
				if (s_asConntCtrl[i].iConntFd >= 0)
				{
					//if (0 > fcntl(s_asConntCtrl[i].iConntFd, F_GETFL))
					//{
					//	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]s_asConntCtrl[%d].iConntFd=%d, but can't find this file number.\r\n", i, s_asConntCtrl[i].iConntFd);
					//}
				}
			}
		}
		else if (0 == iTmp)
		{
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Call 'select' time out, now will close all of timeout connections.\r\n");
			for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
			{
				if (s_asConntCtrl[i].sHttpInfo.ui8Schedule != MUI_HTTP_DEAL_RECVED_BODY)
				{
					net_connt_timeout_clean_by_idx(i);
				}
			}
		}
		else //(iRet > 0)
		{
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Call 'select', have find some FD ready.\r\n");

			//Check every FD in the set
			for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
			{
				if ((s_asConntCtrl[i].iConntFd >= 0) && (s_asConntCtrl[i].sHttpInfo.ui8Schedule != MUI_HTTP_DEAL_RECVED_BODY))
				{
					if (FD_ISSET(s_asConntCtrl[i].iConntFd, &sRdFdSet))
					{
						iFd = s_asConntCtrl[i].iConntFd;
						NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Have received message from s_asConntCtrl[%d], FD=%d\r\n", i, iFd);
						iLen = read(iFd, &s_asConntCtrl[i].pui8Buf[s_asConntCtrl[i].tLen], MUI_API_RECV_BUF_SIZE-s_asConntCtrl[i].tLen);
						if (iLen <= 0)
						{
							//Find the connection has broken, then close this FD.
							net_connt_del_by_idx(i);
							NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Have deleted s_asConntCtrl[%d], FD=%d\r\n", i, iFd);
						}
						else
						{
							NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task] receive message successful from s_asConntCtrl[%d], FD=%d\r\n", i, iFd);
							//Have got message, will deal it.
							net_connt_update_tick_by_idx(i);/* 收到一次包后就更新一次对iFd tick*/
							harg.tLen = iLen;
							//由于前面read函数已经将接受到的报文续接到指定缓冲区,在此处,仅仅传入续接的报文长度即可.
							iTmp = net_http_message_recv(&s_asConntCtrl[i], &harg);
							if (MUI_HTTP_DEAL_RECVED_BODY == iTmp)
							{
								NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Have received all message into s_asConntCtrl[%d], FD=%d\r\n", i, iFd);
								sem_post(&s_tSem);
							}
							else if (MUI_HTTP_DEAL_PARSE_ERR == iTmp)
							{
								//Maybe should to delete this connect.
							}
						}
					}
					else
					{
						net_connt_timeout_clean_by_idx(i);
					}
				}
			}

			//Check whether a new connection request comes
			if (FD_ISSET(s_iSocketRcvFd, &sRdFdSet))
			{
				NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Have received connect request\r\n");
				tRcvLen = sizeof(sClientAdd);
				iFd = accept(s_iSocketRcvFd, (struct sockaddr*)&sClientAdd, &tRcvLen);
				if (iFd < 0)
				{
					NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Can not creat connect with %#x:%d; errno:%d\r\n", ntohl(sClientAdd.sin_addr.s_addr), ntohs(sClientAdd.sin_port), errno);
					read(s_iSocketRcvFd, &iTmp, sizeof(iTmp));
					continue;
				}
				//Add to connected control group.
				iTmp = net_connt_add(iFd);
				if (iTmp < 0)
				{
					//Connected links have overflow, must close current link.
					NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Connect nummber have overflow, now have to break connect request from %#x:%d.", ntohl(sClientAdd.sin_addr.s_addr), ntohs(sClientAdd.sin_port));
					close(iFd);
					continue;
				}

				NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_listen_task]Have accept connect request from %#x:%d\r\n", ntohl(sClientAdd.sin_addr.s_addr), ntohs(sClientAdd.sin_port));
			}
		}
	}

	return NULL;
}


//..............................................................................
/*	HTTP报文处理任务函数
;	(功能)处理已经接收到的HTTP报文.
;	(参数)
;		pArg:	传给函数运行的参数
;	(返回)
;		NULL
;	(注意)
*/
static void* net_manage_api_deal_task (void *pArg)
{
	int iRet, i;

	UNUSED(pArg);

	while (1)
	{
		if (0 > sem_wait_ms(&s_tSem, 10000))
		{
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_deal_task]Wait for s_tSem timeouts.\r\n");
			continue;
		}

		do
		{
			for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
			{
				if ((s_asConntCtrl[i].iConntFd>=0) && (MUI_HTTP_DEAL_RECVED_BODY==s_asConntCtrl[i].sHttpInfo.ui8Schedule))
				{
					s_asConntCtrl[i].pui8Buf[s_asConntCtrl[i].tLen]='\0';
					/*
					NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_deal_task]Have receive %d bytes message as bellow:\r\n", s_asConntCtrl[i].tLen);
					NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_deal_task]!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n"
										"{\r\n"
										"%s""\r\n}\r\n", (char*)s_asConntCtrl[i].pui8Buf);
					*/
					iRet = api_deal(&s_asConntCtrl[i]);
					//NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_api_deal_task]Have processed API request and send out %d bytes answer.\r\n", iLen);
					//将相应的连接断开或重新置为等待接收报文状态
					net_connt_del_by_idx(i);	//net_connt_rst_by_idx(i);

					break;
				}
			}
		} while (i < MUI_ALLOW_CONNECT_MAX);
	}

	return NULL;
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
//int net_manage_init (const char *pchListenIP, uint16_t ui16ListenPort, const char *pchReportIP, uint16_t ui16ReportPort)
int net_manage_init (S_RunPara* netPara)
{
	int iRet;
	S_SockAddr *netInfo;

	ASSERT(NULL != netPara);

	netInfo=&s_achNetInfo;

	netInfo->s_ui16ListenPort = netPara->ui16ListenPort;
	strcpy(netInfo->s_achListenIP, netPara->achListenIp);

	netInfo->s_ui16ReportPort = netPara->ui16ReportPort;
	strcpy(netInfo->s_achReportIP, netPara->achReportIp);

	strcpy(netInfo->s_achNetMask, netPara->netmask);
	strcpy(netInfo->s_achGatewayIP, netPara->gateway);
	strcpy(netInfo->mac, netPara->mac);

	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_init] s_achListenIP=%s:%d\r\n", netInfo->s_achListenIP, netInfo->s_ui16ListenPort);
	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_init] s_achReportIP=%s:%d\r\n", netInfo->s_achReportIP, netInfo->s_ui16ReportPort);
	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_init] s_achNetMask=%s\r\n", netInfo->s_achNetMask);
	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_init] s_achGatewayIP=%s\r\n", netInfo->s_achGatewayIP);
	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_init] mac=%s\r\n", netInfo->mac);

	iRet = sem_init(&s_tSem, 0, 0);
	if (iRet < 0)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_init]Initialize 's_tSem' fail, errno:%d.\r\n", errno);
		return iRet;
	}

	iRet = socket_init();
	if (iRet < 0)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_init]Call 'socket_init' failed, errno:%d.\r\n", errno);
		return iRet;
	}

	iRet = socket_open();
	if (iRet < 0)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_init]Call 'socket_open' failed, errno:%d.\r\n", errno);
		return iRet;
	}

	iRet = pthread_create(&s_sHttpMessageDealThread, NULL, net_manage_api_deal_task, NULL);
	if (iRet < 0)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_init]Create thread 'net_manage_api_deal_task' fail, errno:%d.\r\n", errno);
		socket_close();
		return iRet;
	}

	iRet = pthread_create(&s_sRcvSocketListenThread, NULL, net_manage_api_listen_task, NULL);
	if (iRet < 0)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_init]Create thread 'net_manage_api_listen_task' fail, errno:%d.\r\n", errno);
		socket_close();
		return iRet;
	}

	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_init]net_manage_init successful!\r\n");

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
int net_manage_quit_wait (void)
{
	int iRet;

	iRet = pthread_join(s_sRcvSocketListenThread, NULL);
	if (iRet != 0)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_quit_wait]Joining thread 'net_manage_api_listen_task' fail, errno:%d\r\n", errno);
		abort();
	}

	iRet = pthread_join(s_sHttpMessageDealThread, NULL);
	if (iRet != 0)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_quit_wait]Joining thread 'net_manage_api_deal_task' fail, errno:%d\r\n", errno);
		abort();
	}

	iRet = socket_close();

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Socket事件上报函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:			 <0:出错
;						>=0:发送的字节数
;	(注意)
*/
int net_event_report (const void *pvBuf, size_t tLen)
{
	struct sockaddr_in sServerAdd;
	int iRet;

	if (NULL == pvBuf)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_event_report]pvBuf is NULL.\r\n");
		return -1;
	}
	if (0 == tLen)
	{
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_event_report]tLen == 0\r\n");
		return -1;
	}

	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_event_report]pvBuf==%u, tLen==%u\r\n", (size_t)pvBuf, tLen);
	NET_LINUX_PUTS((const char*)pvBuf, (int)tLen);

	//Creat send socket for TCP
	s_iSocketSendFd = socket(AF_INET, SOCK_STREAM, 0);
	if (s_iSocketSendFd < 0)
	{
		close(s_iSocketRcvFd);
		s_iSocketRcvFd = -1;
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_event_report]Creat send socket fail, errno:%d.", errno);
		return s_iSocketSendFd;
	}

	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_event_report]Report address:%s:%u\r\n", s_achNetInfo.s_achReportIP, s_achNetInfo.s_ui16ReportPort);
	memset(&sServerAdd, 0, sizeof(sServerAdd));
	sServerAdd.sin_family	   = AF_INET;
	sServerAdd.sin_addr.s_addr = inet_addr(s_achNetInfo.s_achReportIP);
	sServerAdd.sin_port		   = htons(s_achNetInfo.s_ui16ReportPort);

	iRet = connect(s_iSocketSendFd, (struct sockaddr *)(&sServerAdd), sizeof(struct sockaddr));
	if (iRet < 0)
	{
		close(s_iSocketSendFd);
		s_iSocketSendFd = -1;
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_manage_event_report]Connect to server fail, errno:%d.\r\n", errno);
		return iRet;
	}

	iRet = send(s_iSocketSendFd, pvBuf, tLen, 0);
	if (iRet < 0)
	{
		close(s_iSocketSendFd);
		s_iSocketSendFd = -1;
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[net_manage_event_report]Message send fail, errno:%d.\r\n", errno);
		return iRet;
	}

	close(s_iSocketSendFd);
	s_iSocketSendFd = -1;

	return iRet;	//返回实际发送的字节数.
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Socket监听IP地址获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		const char*:	监听IP地址字符串的指针
;	(注意)
*/
const char* net_listen_ip_addr_get (void)
{
	return s_achNetInfo.s_achListenIP;
}
//..............................................................................
/*	Socket监听Port获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		uint16_t:	监听Port值
;	(注意)
*/
uint16_t net_listen_port_get (void)
{
	return s_achNetInfo.s_ui16ListenPort;
}
//..............................................................................
/*	Socket上报IP地址获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		const char*:	监听IP地址字符串的指针
;	(注意)
*/
const char* net_report_ip_addr_get (void)
{
	return s_achNetInfo.s_achReportIP;
}
//..............................................................................
/*	Socket上报Port获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		uint16_t:	上报Port值
;	(注意)
*/
uint16_t net_report_port_get (void)
{
	return s_achNetInfo.s_ui16ReportPort;
}
//..............................................................................
/*	net API(接口)子网掩码获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		const char*:	子网掩码字符串的指针
;	(注意)
*/
const char* net_mask_get (void)
{
	return s_achNetInfo.s_achNetMask;
}
//..............................................................................
/*	net API(接口)网关IP地址获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		const char*:	网关IP地址字符串的指针
;	(注意)
*/
const char* net_gw_ip_addr_get (void)
{
	return s_achNetInfo.s_achGatewayIP;
}
//..............................................................................
/*	net API(接口)MAC地址获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		const char*:	MAC地址字符串的指针
;	(注意)
*/
const char* net_mac_addr_get (void)
{
	return s_achNetInfo.mac;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Socket管理(模块)清除(占用系统资源)函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		NULL
;	(注意)
*/
void net_manage_clean (void)
{
	socket_close();
}
//-------------------------------------------------------------------------------------------------*

//-------------------------------------------------------------------------------------------------*
/*	GetIfInfo获取网络接口信息函数
;	(功能)获取指定网路接口的信息.
;	(参数)
;		pchIfName:	网络接口名称
;		psIfInfo:	网络接口信息回写指针
;	(返回)
;		int:		-1:出错
;					 0:成功
;	(注意)
*/
int if_info_get (const char *pchIfName, S_SockAddr *psIfInfo)
{

#if 0
	struct ifaddrs *psIfAddr, *psIfAddrHeader=NULL;
	int iRet;

	ASSERT(pchIfName != NULL);
	ASSERT(psIfInfo != NULL);

	NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[if_info_get]Will run if_info_get.\r\n");

	iRet = getifaddrs(&psIfAddrHeader);
	if (-1 == iRet)
	{
		printf("[if_info_get]Execute getifaddrs fail, errno:%d.\r\n", errno);
		return iRet;
	}

	iRet = -1;
	for (psIfAddr = psIfAddrHeader; psIfAddr != NULL; psIfAddr = psIfAddr->ifa_next)
	{
		if (NULL == psIfAddr->ifa_addr)
		{
			continue;
		}
		if (AF_INET != psIfAddr->ifa_addr->sa_family)
		{
			continue;
		}
		if (0 != strncmp(pchIfName, psIfAddr->ifa_name, 60))	//为防止内存溢出,假定最大的IfName长度为60
		{
			continue;
		}

/*
		printf("\n>>> interfaceName: %s\n", ifa->ifa_name);

		sin = (struct sockaddr_in *)ifa->ifa_addr;
		printf(">>> ipAddress: %s\n", inet_ntoa(sin->sin_addr));

		sin = (struct sockaddr_in *)ifa->ifa_dstaddr;
		printf(">>> broadcast: %s\n", inet_ntoa(sin->sin_addr));

		sin = (struct sockaddr_in *)ifa->ifa_netmask;
		printf(">>> subnetMask: %s\n", inet_ntoa(sin->sin_addr));
*/
		/*
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "##################################################\r\n");
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "'%s' address family: %d%s\r\n", psIfAddr->ifa_name, psIfAddr->ifa_addr->sa_family,
			  (psIfAddr->ifa_addr->sa_family==AF_PACKET) ? "(AF_PACKET)" :
			  (psIfAddr->ifa_addr->sa_family==AF_INET) ? "(AF_INET)" :
			  (psIfAddr->ifa_addr->sa_family==AF_INET6) ? "(AF_INET6)" : "");
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "##################################################\r\n");
		*/
		memcpy(&psIfInfo->sIpAddr, &((struct sockaddr_in*)psIfAddr->ifa_addr)->sin_addr, sizeof(struct in_addr));
		/*
		char achAddrBuf[NI_MAXHOST];
		if (NULL == inet_ntop(AF_INET, &((struct sockaddr_in*)psIfAddr->ifa_addr)->sin_addr, achAddrBuf, sizeof(achAddrBuf)))
		{
			NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "inet_ntop() failed.\r\n");
			continue;
		}
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "1.address: <%s>\r\n", achAddrBuf);
		*/
		iRet = 0;
	}

	freeifaddrs(psIfAddrHeader);

	return iRet;

#endif

	struct ifreq *ifr, *ifend;
	struct ifconf ifc;
	struct ifreq ifs[16];

	struct sockaddr *address;
	struct sockaddr_in *address_in;

	int iRet=0;
	int sockfd;
	if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0 ){
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[if_info_get]Socket opening error!\r\n");
		iRet=-1;
		return iRet;
	}

	ifc.ifc_len = sizeof(ifs);
	ifc.ifc_req = ifs;


	if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0) {
		NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[if_info_get]Device Error=%d\r\n", errno);
		iRet=-1;
	} else {
		ifend = ifs + (ifc.ifc_len / sizeof(struct ifreq));


		for (ifr = ifc.ifc_req; ifr < ifend; ifr++) {

			if (ifr->ifr_addr.sa_family == AF_INET) {
				//printf("[Dev=%s]\r\n", ifr->ifr_name);

				//如果eth0 设备?
				if (0 == strncmp(pchIfName, ifr->ifr_name, 60)){

					if(ioctl(sockfd,SIOCGIFADDR,ifr)<0){
						NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[if_info_get]IP error=%d\r\n", errno);
					}
					else{
						address = &(ifr->ifr_addr);
						address_in = (struct sockaddr_in *) address;
						strcpy(psIfInfo->s_achListenIP, inet_ntoa(address_in->sin_addr));
						//printf("IP address is %s\n", psIfInfo->ip);
					}

					/*if(ioctl (sockfd, SIOCGIFBRDADDR, ifr) <0)
						printf("[Broadcast error=%d]\r\n", errno);
					else{
						address = &(ifr->ifr_broadaddr);
						address_in = (struct sockaddr_in *) address;
						strcpy(psIfInfo->broadcast, inet_ntoa(address_in->sin_addr));
						//printf("Broadcast is %s\n", psIfInfo->broadcast);
					}*/


					if(ioctl (sockfd, SIOCGIFNETMASK, ifr) <0){
						NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[if_info_get]Netmask error=%d\r\n", errno);
					}
					else{
						address = &(ifr->ifr_netmask);
						address_in = (struct sockaddr_in *) address;
						strcpy(psIfInfo->s_achNetMask, inet_ntoa(address_in->sin_addr));
						//printf("Netmask is %s\n", psIfInfo->netmask);
					}

					if (ioctl (sockfd, SIOCGIFHWADDR, ifr) < 0){
						NET_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[if_info_get]MAC error=%d\r\n", errno);
					}
					else{
						address_in = (struct sockaddr_in *)&ifr->ifr_netmask;
						sprintf(psIfInfo->mac, "%02x:%02x:%02x:%02x:%02x:%02x",
						(unsigned char)ifr->ifr_netmask.sa_data[0],
						(unsigned char)ifr->ifr_netmask.sa_data[1],
						(unsigned char)ifr->ifr_netmask.sa_data[2],
						(unsigned char)ifr->ifr_netmask.sa_data[3],
						(unsigned char)ifr->ifr_netmask.sa_data[4],
						(unsigned char)ifr->ifr_netmask.sa_data[5]);
						//printf("Mac address is %s\n", psIfInfo->mac);
					}
					iRet = 0;
					break;
				} else {
					iRet = -1;
					continue;
				}

			}

		}

	}
	close(sockfd);

	return iRet;

}
//-------------------------------------------------------------------------------------------------*
/* 此缓冲区是用来接收内核空间路由信息,不应过小,否则接收消息header失效;*/
#define BUFSIZE 4096//8192

struct route_info{
	u_int dstAddr;
	u_int srcAddr;
	u_int gateWay;
	char ifName[IF_NAMESIZE];
};


static int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
	struct nlmsghdr *nlHdr;
	int readLen = 0, msgLen = 0;

	do{
		//收到内核的应答
		if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0){
			perror("SOCK READ: ");
			return -1;
		}

		nlHdr = (struct nlmsghdr *)bufPtr;

		//检查header是否有效
		if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR)){
			perror("Error in recieved packet");
			return -1;
		}

		if(nlHdr->nlmsg_type == NLMSG_DONE){
			break;
		} else {
			bufPtr += readLen;
			msgLen += readLen;
		}

		if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0){
			break;
		}
	} while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));

	return msgLen;
}


static void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway, char *ip)
{
	char *tempBuf = NULL;
	int rtLen,iLen=0;
	char srcAddr[16];

	struct rtmsg *rtMsg;
	struct rtattr *rtAttr;

	struct in_addr dst;
	struct in_addr gate;

	tempBuf = (char *)malloc(100);
	rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

	// If the route is not for AF_INET or does not belong to main routing table
	//then return.
	if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
		return;

	rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
	rtLen = RTM_PAYLOAD(nlHdr);
	for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
		switch(rtAttr->rta_type) {
			case RTA_OIF:
				if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
				break;
			case RTA_GATEWAY:
				rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
				break;
			case RTA_PREFSRC:
				rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
				break;
			case RTA_DST:
				rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
				break;
		}
	}

	//dst.s_addr = rtInfo->dstAddr;
	//if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))

	//printf("devname:%s\r\n",rtInfo->ifName);

	/*gate.s_addr = rtInfo->gateWay;
	iLen=sprintf(gateway, (char *)inet_ntoa(gate));
	printf("iLen=%d, gateway:%s\r\n",iLen, gateway);*/

	gate.s_addr = rtInfo->srcAddr;
	iLen=sprintf(srcAddr, (char *)inet_ntoa(gate));
	//printf("iLen=%d src:%s\r\n",iLen,srcAddr);
	//printf("ip=%s\r\n", ip);

	gate.s_addr = rtInfo->gateWay;
	//printf("gateway=%s\r\n", (char *)inet_ntoa(gate));
	if (strcmp(srcAddr, ip)==0){							/* 如果IP地扯相等,则提取此链网关地扯*/
		sprintf(gateway, (char *)inet_ntoa(gate));
	}

	//gate.s_addr = rtInfo->dstAddr;
	//printf("dst:%s\r\n\n",(char *)inet_ntoa(gate));


	free(tempBuf);
	return;

}



int get_gateway(char *gateway, char *ip)
{
	char msgBuf[BUFSIZE];
	int sock, len, msgSeq = 0;

	struct nlmsghdr *nlMsg;
	struct rtmsg *rtMsg;
	struct route_info *rtInfo;

	if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0){
		perror("Socket Creation: ");
		return -1;
	}

	memset(msgBuf, 0, BUFSIZE);

	nlMsg = (struct nlmsghdr *)msgBuf;

	rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);

	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); 	/* Length of message. */
	nlMsg->nlmsg_type = RTM_GETROUTE; 						/* Get the routes from kernel routing table .  */

	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;		/* The message is a request for dump.  */
	nlMsg->nlmsg_seq = msgSeq++; 							/*Sequence of the message packet.  */
	nlMsg->nlmsg_pid = getpid(); 							/* PID of process sending the request.  */


	if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
		printf("Write To Socket Failed…\n");
		return -1;
	}

	if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
		printf("Read From Socket Failed…\n");
		return -1;
	}

	rtInfo = (struct route_info *)malloc(sizeof(struct route_info));

	for( ; NLMSG_OK(nlMsg,len); nlMsg = NLMSG_NEXT(nlMsg,len)){
		memset(rtInfo, 0, sizeof(struct route_info));
		parseRoutes(nlMsg, rtInfo, gateway, ip);
	}

	free(rtInfo);
	close(sock);

	return 0;
}

//-------------------------------------------------------------------------------------------------*

