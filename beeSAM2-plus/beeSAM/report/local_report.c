#include "local_report.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define LOCAL_REPORT_DBG

#ifndef LOCAL_REPORT_PUTC
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_PUTC(Char)			PUTC(Char)
#else
#define LOCAL_REPORT_PUTC(Char)
#endif
#endif

#ifndef LOCAL_REPORT_PUTS
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define LOCAL_REPORT_PUTS(Str, Len)
#endif
#endif

#ifndef LOCAL_REPORT_PUTASCC
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_PUTASCC(Char)			PUTASCC(Char)
#else
#define LOCAL_REPORT_PUTASCC(Char)
#endif
#endif

#ifndef LOCAL_REPORT_PUTASCS
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define LOCAL_REPORT_PUTASCS(Str, Len)
#endif
#endif

#ifndef LOCAL_REPORT_PRINT
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define LOCAL_REPORT_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef LOCAL_REPORT_DYN_PUTC
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_LOCAL_REPORT|Dbg), Char)
#else
#define LOCAL_REPORT_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef LOCAL_REPORT_DYN_PUTS
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_LOCAL_REPORT|Dbg), Str, Len)
#else
#define LOCAL_REPORT_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef LOCAL_REPORT_DYN_PUTASCC
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_LOCAL_REPORT|Dbg), Char)
#else
#define LOCAL_REPORT_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef LOCAL_REPORT_DYN_PUTASCS
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_LOCAL_REPORT|Dbg), Str, Len)
#else
#define LOCAL_REPORT_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef LOCAL_REPORT_DYN_PRINT
#ifdef LOCAL_REPORT_DBG
#define LOCAL_REPORT_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_LOCAL_REPORT|Dbg), Fmt, ##Arg)
#else
#define LOCAL_REPORT_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define		MUI_LOCAL_REPORT_RING_BUF_SIZE			(32*1024)
#define		MUI_LOCAL_REPORT_MSG_LEN_MAX			(10*1024)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_LOCAL_REPORT_BUF_SEND
static uint8_t s_aui8LocalReportMsgBuf[MUI_LOCAL_REPORT_RING_BUF_SIZE];
static S_RingBuffer s_aui8LocalReportRingBuf;
static pthread_t s_tLocalReportDealThread; /* 线程ID*/
static sem_t s_tLocalReportSem;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_LOCAL_REPORT_BUF_SEND
static void* s_local_report_deal_task (void *pArg);
#endif

#ifdef M_LOCAL_REPORT_BUF_SEND
//-------------------------------------------------------------------------------------------------*
/*	本地上报处理任务函数
;	(功能)将发送环形缓冲区的报文发送出去
;	(参数)
;		pArg:	传给函数运行的参数
;	(返回)
;		NULL
;	(注意)
*/
static void* s_local_report_deal_task (void *pArg)
{
	uint8_t aui8Buf[MUI_LOCAL_REPORT_MSG_LEN_MAX];
	size_t tLen;
	int iRet;

	while (1)
	{
		if (0 > sem_wait_ms(&s_tLocalReportSem, 10000))
		{
			LOCAL_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_local_report_deal_task]Wait for s_tLocalReportSem timeouts.\r\n");
			//其它功能
			continue;
		}

		if (!ring_buf_if_empty(&s_aui8LocalReportRingBuf))
		{
			//LOCAL_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_local_report_deal_task]Will send local report message.\r\n");
			iRet = msg_ring_buf_rd(&s_aui8LocalReportRingBuf, aui8Buf, sizeof(aui8Buf), &tLen);
			if (iRet != (int)(sizeof(uint16_t)+tLen))
			{
				LOCAL_REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_local_report_deal_task]Read local report buffer fail!\r\n");
			}
			else
			{
				//aui8Buf[tLen] = '\0';
				//tLen++;

				LOCAL_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_local_report_deal_task]Message length=%u\r\n", tLen);
				LOCAL_REPORT_DYN_PUTS(M_DBG_LEVEL_DEBUG,(const char*)aui8Buf, tLen);
				LOCAL_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");

				iRet = multicast_send(aui8Buf, tLen);
				if (iRet < 0)
				{
					LOCAL_REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_local_report_deal_task]Call 'multicast_send()' fail!\r\n");
				}
			}
		}
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------*
#endif
//-------------------------------------------------------------------------------------------------*
/*	本地上报初始化函数
;	(功能)对本地上报模块进行初始化
;	(参数)
;		NULL
;	(返回)
;		NULL
;	(注意)
*/
int local_report_init (void)
{
	int iRet = 0;

#ifdef M_LOCAL_REPORT_BUF_SEND
	memset(&s_aui8LocalReportMsgBuf, 0, sizeof(s_aui8LocalReportMsgBuf));
	if (true != ring_buf_init(&s_aui8LocalReportRingBuf, &s_aui8LocalReportMsgBuf[0], &s_aui8LocalReportMsgBuf[sizeof(s_aui8LocalReportMsgBuf)-1]))
	{
		return -1;
	}

	iRet = sem_init(&s_tLocalReportSem, 0, 0);
	if (iRet < 0)
	{
		LOCAL_REPORT_DYN_PRINT(M_DBG_LEVEL_ERR,"[local_report_init]s_tLocalReportSem initialize fail, errno:%d\r\n", errno);
		return iRet;
	}

	iRet = pthread_create(&s_tLocalReportDealThread, NULL, s_local_report_deal_task, NULL);
	if (iRet < 0)
	{
		LOCAL_REPORT_DYN_PRINT(M_DBG_LEVEL_ERR,"[local_report_init]Call 'pthread_create()' fail, errno:%d\r\n", errno);
		return iRet;
	}
#endif

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	本地上报发送函数
;	(功能)将报文写入本地上报环形缓冲区中
;	(参数)
;		pvMsg:		指向写入报文数据的指针
;		tLen:		报文数据的长度
;	(返回)
;		int:		实际写入的数据的字节数,负数表示出错
;	(注意)
*/
int local_report_send (const void *pvMsg, size_t tLen)
{
#ifdef M_LOCAL_REPORT_BUF_SEND
	int iRet;

	ASSERT(pvMsg != NULL);
	ASSERT(0 < tLen);

	//iRet = msg_ring_buf_wr(&s_aui8LocalReportRingBuf, pvMsg, tLen);
	if (iRet > (int)sizeof(uint16_t))
	{
		sem_post(&s_tLocalReportSem);
	}

	return iRet;
#else
	return multicast_send(pvMsg, tLen);
#endif
}
//-------------------------------------------------------------------------------------------------*

