#include <stdlib.h>
#include <stdio.h>
#include "dbg_linux.h"
#include "net_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_DBG_INFO_SRC_PORT			38437
#define MUI_DBG_INFO_DEST_PORT			38537

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static unsigned char s_uchDbgInfoOutputMode = 0;
static unsigned int s_uiDbgInfoMask = M_DBG_MODULE_ALL;
//static unsigned int s_uiDbgInfoMask = M_DBG_MODULE_REMOTE_NET_MANAGE;
//static unsigned int s_uiDbgInfoLevel = M_DBG_LEVEL_WARNING;
static unsigned int s_uiDbgInfoLevel = M_DBG_LEVEL_DEBUG;

#ifdef M_DBG_BUF_SEND
static unsigned char s_auchDbgInfoBuf[10 * 1024];
static S_RingBuffer s_sDbgInfoRingBuf;
static sem_t s_tDbgInfoSem;
static pthread_t s_sDbgInfoDealThread;
#endif
static S_SocketCfg s_sDbgSocketCfg;
static sem_t s_tDbgInfoWrMux;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int s_dbg_udp_link_establish (const char *pchDbgIp, uint16_t ui16Port);
static int s_dbg_info_send (const void *pvBuf, size_t tLen);
#ifdef M_DBG_BUF_SEND
static void* s_dbg_info_deal_task (void *pArg);
#endif

#if 1
//-------------------------------------------------------------------------------------------------*
/*	s_dbg_udp_link_establish(简单服务发现协议)等待查询线程函数
;	(功能)等待MULTICAST报文.
;	(参数)
;		pArg:	参数指针
;	(返回)
;		void*	返回指针
;	(注意)
*/
static int s_dbg_udp_link_establish (const char *pchDbgIp, uint16_t ui16Port)
{
	//struct sockaddr_in sLocalAddr;
	int iFd, iRet, iTmp;

	//Creat socket for UDP
	iFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (iFd < 0)
	{
		printf("[s_dbg_udp_link_establish]Creat the socket fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	printf("[s_dbg_udp_link_establish]Creat socket successful!\r\n");

	//Socket:可再用地址
	iTmp = 1;
	iRet = setsockopt(iFd, SOL_SOCKET, SO_REUSEADDR, &iTmp, sizeof(iTmp));
	if (iRet < 0)
	{
		printf("[s_dbg_udp_link_establish]Set reuse address fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	printf("[s_dbg_udp_link_establish]Set reuse address successful!\r\n");
	/*
	//绑定Socket地址
	//Set socket address
	bzero(&sLocalAddr, sizeof(sLocalAddr));
	sLocalAddr.sin_family	   = AF_INET;
	sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sLocalAddr.sin_port		   = htons(MUI_DBG_INFO_SRC_PORT);
	iRet = bind(iFd, (struct sockaddr *)&sLocalAddr, sizeof(struct sockaddr));
	if (iRet < 0)
	{
		printf("s_dbg_udp_link_establish: Bind the socket address fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	printf(M_DBG_LEVEL_DEBUG, "s_dbg_udp_link_establish: Bind LocalAddr successful!\r\n");
	*/
	//设置sSocketCfg
	bzero(&s_sDbgSocketCfg, sizeof(s_sDbgSocketCfg));
	s_sDbgSocketCfg.iSocketFd				   = iFd;
	s_sDbgSocketCfg.sSocketAdd.sin_family	   = AF_INET;
	s_sDbgSocketCfg.sSocketAdd.sin_addr.s_addr = inet_addr(pchDbgIp);
	s_sDbgSocketCfg.sSocketAdd.sin_port		   = htons(ui16Port);
	s_sDbgSocketCfg.tSockLen				   = sizeof(struct sockaddr_in);

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	调试信息发送函数
;	(功能)
;	(参数)
;		pvBuf:		要发送的数据缓存
;		tLen:		发送数据的大小
;	(返回)
;		int:		发送的字节数
;	(注意)
*/
static int s_dbg_info_send (const void *pvBuf, size_t tLen)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tDbgInfoWrMux, 1000+1))	//最多阻塞(MUI_AT_QUERY_WAIT_MAX+1)
	{
		return -1;
	}

#ifdef M_DBG_BUF_SEND
	iRet = (int)ring_buf_wr(&s_sDbgInfoRingBuf, pvBuf, tLen);
#else
	iRet = sendto(s_sDbgSocketCfg.iSocketFd, pvBuf, tLen, 0, (struct sockaddr *)&s_sDbgSocketCfg.sSocketAdd, s_sDbgSocketCfg.tSockLen);
#endif
	if (iRet < (int)tLen)
	{
		printf("[s_dbg_info_send]Send debug info fail.\r\n");
	}
	sem_post(&s_tDbgInfoWrMux);
#ifdef M_DBG_BUF_SEND
	sem_post(&s_tDbgInfoSem);
#endif

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
#ifdef M_DBG_BUF_SEND
//-------------------------------------------------------------------------------------------------*
/*	本地上报处理任务函数
;	(功能)将发送环形缓冲区的报文发送出去
;	(参数)
;		pArg:	传给函数运行的参数
;	(返回)
;		NULL
;	(注意)
*/
static void* s_dbg_info_deal_task (void *pArg)
{
	uint8_t aui8Buf[1024];
	size_t tLen;
	int iRet;

	UNUSED(pArg);

	while (1)
	{
		if (0 > sem_wait_ms(&s_tDbgInfoSem, 10000))
		{
			printf("[s_dbg_info_deal_task]Wait for s_tDbgInfoSem timeouts.\r\n");
			//其它功能
			continue;
		}

		while (0 < (tLen=ring_buf_rd(&s_sDbgInfoRingBuf, aui8Buf, sizeof(aui8Buf))))
		{
		#if 0
			for (iRet=0; (size_t)iRet<tLen; iRet++)
			{
				putchar(aui8Buf[iRet]);
			}
		#else
			iRet = sendto(s_sDbgSocketCfg.iSocketFd, aui8Buf, tLen, 0, (struct sockaddr *)&s_sDbgSocketCfg.sSocketAdd, s_sDbgSocketCfg.tSockLen);
			if (iRet < 0)
			{
				printf("[s_dbg_info_deal_task]Call 'sendto()' fail, errno:%d\r\n", errno);
			}
		#endif
		}
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------*
#endif
#endif
//-------------------------------------------------------------------------------------------------*
/*	DBG系统初始化函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:		 <0:出错(包括异常及等待超时)
;					>=0:正常完成
;	(注意)
*/
int dbg_sys_init (const char *pchDbgIp)
{
	int iRet = 0;

	if ((NULL!=pchDbgIp) && ('\0'!=pchDbgIp[0]))
	{
		s_uchDbgInfoOutputMode = 1;
	#ifdef M_DBG_INFO
		#ifdef M_DBG_BUF_SEND
		memset(&s_auchDbgInfoBuf, 0, sizeof(s_auchDbgInfoBuf));
		if (true != ring_buf_init(&s_sDbgInfoRingBuf, &s_auchDbgInfoBuf[0], &s_auchDbgInfoBuf[sizeof(s_auchDbgInfoBuf)-1]))
		{
			printf("[dbg_sys_init]Call 'ring_buf_init()' fail.\r\n");
			return -1;
		}
		#endif
		iRet = sem_init(&s_tDbgInfoWrMux, 0, 1);	//初始化为1
		if (iRet < 0)
		{
			return iRet;
		}
		#ifdef M_DBG_BUF_SEND
		iRet = sem_init(&s_tDbgInfoSem, 0, 0);		//初始化为0
		if (iRet < 0)
		{
			return iRet;
		}
		#endif
		iRet = s_dbg_udp_link_establish(pchDbgIp, MUI_DBG_INFO_DEST_PORT);
		if (iRet < 0)
		{
			return iRet;
		}
		#ifdef M_DBG_BUF_SEND
		iRet = pthread_create(&s_sDbgInfoDealThread, NULL, s_dbg_info_deal_task, NULL);
		if (iRet < 0)
		{
			printf("[dbg_sys_init]Call 'pthread_create()' fail, errno:%d\r\n", errno);
			return iRet;
		}
		#endif
	#endif
	}
	else
	{
		s_uchDbgInfoOutputMode = 0;
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
#ifdef M_DBG_INFO
unsigned char dbg_info_mode_get (void)
{
	return s_uchDbgInfoOutputMode;
}
//-------------------------------------------------------------------------------------------------*
/*	DBG_PutC 函数
;	(功能)输出字符.
;	(参数)
;		Ch:		要打印的字符
;	(返回)
;		int:	负数:出错
;				其它:被打印的字符的ASCII值
;	(注意)
*/
int DBG_PutC (int Ch)
{
	int iTmp;
	char chTmp;

	if (0 == dbg_info_mode_get())
	{
		return putchar(Ch);
	}
	else
	{
		chTmp = (char)Ch;
		iTmp = s_dbg_info_send(&chTmp, sizeof(char));
		if (iTmp < 0)
		{
			return iTmp;
		}

		return Ch;
	}
}
//..............................................................................
/*	DBG_PutS 函数
;	(功能)输出指定长度的字符串.
;	(参数)
;		pchStr:	要打印的字符串
;		iLen:	要打印的字符串长度
;	(返回)
;		int:	负数:出错
;				其它:被打印的字符的个数
;	(注意)
*/
int DBG_PutS (const char *pchStr, int iLen)
{
	int iTmp = iLen;

	ASSERT(pchStr != NULL);
	ASSERT(iLen > 0);

	if (0 == dbg_info_mode_get())
	{
		while (iLen-- > 0)
		{
			PUTC(*pchStr++);
		}
	}
	else
	{
		iTmp = s_dbg_info_send(pchStr, iLen);
	}

	return iTmp;
}
//..............................................................................
/*	DBG_PutAsciiC 函数
;	(功能)输出ASCII字符,特殊字符以<???>的形式输出.
;	(参数)
;		Ch:		要打印的字符
;	(返回)
;		int:	负数:出错
;				其它:被打印的字符的ASCII值
;	(注意)
*/
int DBG_PutAsciiC (int Ch)
{
	int iTmp;

	switch (Ch)
	{
	case 0:
		iTmp = DBG_PutS("<NUL>", 5);
		break;
	case 1:
		iTmp = DBG_PutS("<SOH>", 5);
		break;
	case 2:
		iTmp = DBG_PutS("<STX>", 5);
		break;
	case 3:
		iTmp = DBG_PutS("<ETX>", 5);
		break;
	case 4:
		iTmp = DBG_PutS("<EOT>", 5);
		break;
	case 5:
		iTmp = DBG_PutS("<ENQ>", 5);
		break;
	case 6:
		iTmp = DBG_PutS("<ACK>", 5);
		break;
	case 7:
		iTmp = DBG_PutS("<BEL>", 5);
		break;
	case 8:
		iTmp = DBG_PutS("<BS>", 4);
		break;
	case 9:
		iTmp = DBG_PutS("<HT>", 4);
		break;
	case 10:
		iTmp = DBG_PutS("<LF>", 4);
		break;
	case 11:
		iTmp = DBG_PutS("<VT>", 4);
		break;
	case 12:
		iTmp = DBG_PutS("<FF>", 4);
		break;
	case 13:
		iTmp = DBG_PutS("<CR>", 4);
		break;
	case 14:
		iTmp = DBG_PutS("<SO>", 4);
		break;
	case 15:
		iTmp = DBG_PutS("<SI>", 4);
		break;
	case 16:
		iTmp = DBG_PutS("<DLE>", 5);
		break;
	case 17:
		iTmp = DBG_PutS("<DC1>", 5);
		break;
	case 18:
		iTmp = DBG_PutS("<DC2>", 5);
		break;
	case 19:
		iTmp = DBG_PutS("<DC3>", 5);
		break;
	case 20:
		iTmp = DBG_PutS("<DC4>", 5);
		break;
	case 21:
		iTmp = DBG_PutS("<NAK>", 5);
		break;
	case 22:
		iTmp = DBG_PutS("<SYN>", 5);
		break;
	case 23:
		iTmp = DBG_PutS("<ETB>", 5);
		break;
	case 24:
		iTmp = DBG_PutS("<CAN>", 5);
		break;
	case 25:
		iTmp = DBG_PutS("<EM>", 4);
		break;
	case 26:
		iTmp = DBG_PutS("<SUB>", 5);
		break;
	case 27:
		iTmp = DBG_PutS("<ESC>", 5);
		break;
	case 28:
		iTmp = DBG_PutS("<FS>", 4);
		break;
	case 29:
		iTmp = DBG_PutS("<GS>", 4);
		break;
	case 30:
		iTmp = DBG_PutS("<RS>", 4);
		break;
	case 31:
		iTmp = DBG_PutS("<US>", 4);
		break;
	case 127:
		iTmp = DBG_PutS("<DEL>", 5);
		break;
	default:
		iTmp = PUTC(Ch);
		break;
	}

	if (iTmp < 0)
	{
		return iTmp;
	}

	return Ch;
}
//..............................................................................
/*	DBG_PutAsciiS 函数
;	(功能)输出ASCII字符串,特殊字符以<???>的形式输出.
;	(参数)
;		pchStr:	要打印的字符串
;		iLen:	字符串长度
;	(返回)
;		int:	已打印字符串的长度
;	(注意)
*/
int DBG_PutAsciiS (const char *pchStr, int iLen)
{
	int iRemain = iLen;
	int iTmp;

	ASSERT(pchStr != NULL);
	ASSERT(iLen > 0);

	while (iRemain--)
	{
		iTmp = DBG_PutAsciiC((int)*pchStr);
		if (iTmp < 0)
		{
			return iTmp;
		}
		pchStr++;
	}

	return (int)(iLen - iRemain);
}
//..............................................................................
/*	DBG_Print 函数
;	(功能)按要求格式输出字符串.
;	(参数)
;		pchFmt:	格式
;		...:	可变参数
;	(返回)
;		int:	已打印字符串的长度
;	(注意)
*/
int DBG_Print (const char *pchFmt, ...)
{
    char achBuf[5*1024];
	va_list	Args;
	int iLen;

	ASSERT(pchFmt != NULL);

	va_start(Args, pchFmt);
	iLen = vsnprintf(achBuf, sizeof(achBuf), pchFmt, Args);
	va_end(Args);

	return s_dbg_info_send(achBuf, iLen);
}
//-------------------------------------------------------------------------------------------------*
#endif
//-------------------------------------------------------------------------------------------------*
/*	dbg_info_mask_get() 函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		unsigned int:	s_uiDbgInfoMask
;	(注意)
*/
unsigned int dbg_info_mask_get (void)
{
	return s_uiDbgInfoMask;
}
//..............................................................................
/*	dbg_info_mask_set() 函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		unsigned int:	s_uiDbgInfoMask
;	(注意)
*/
unsigned int dbg_info_mask_set (unsigned int uiMask)
{
	return (s_uiDbgInfoMask=uiMask);
}
//..............................................................................
/*	dbg_info_level_get() 函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		unsigned int:	s_uiDbgInfoLevel
;	(注意)
*/
unsigned int dbg_info_level_get (void)
{
	return s_uiDbgInfoLevel;
}
//..............................................................................
/*	dbg_info_level_set() 函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		unsigned int:	s_uiDbgInfoLevel
;	(注意)
*/
unsigned int dbg_info_level_set (unsigned int uiLevel)
{
	return (s_uiDbgInfoLevel=uiLevel);
}
//-------------------------------------------------------------------------------------------------*
