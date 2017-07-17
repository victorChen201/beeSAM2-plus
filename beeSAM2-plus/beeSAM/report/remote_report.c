#include "remote_report.h"
#include "sys_time.h"
#include "config.h"

#ifdef M_REMOTE_COMMUNICATION
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define REMOTE_REPORT_DBG

#ifndef REMOTE_REPORT_PUTC
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_PUTC(Char)			PUTC(Char)
#else
#define REMOTE_REPORT_PUTC(Char)
#endif
#endif

#ifndef REMOTE_REPORT_PUTS
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define REMOTE_REPORT_PUTS(Str, Len)
#endif
#endif

#ifndef REMOTE_REPORT_PUTASCC
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_PUTASCC(Char)			PUTASCC(Char)
#else
#define REMOTE_REPORT_PUTASCC(Char)
#endif
#endif

#ifndef REMOTE_REPORT_PUTASCS
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define REMOTE_REPORT_PUTASCS(Str, Len)
#endif
#endif

#ifndef REMOTE_REPORT_PRINT
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define REMOTE_REPORT_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef REMOTE_REPORT_DYN_PUTC
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_REMOTE_REPORT|Dbg), Char)
#else
#define REMOTE_REPORT_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef REMOTE_REPORT_DYN_PUTS
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_REMOTE_REPORT|Dbg), Str, Len)
#else
#define REMOTE_REPORT_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef REMOTE_REPORT_DYN_PUTASCC
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_REMOTE_REPORT|Dbg), Char)
#else
#define REMOTE_REPORT_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef REMOTE_REPORT_DYN_PUTASCS
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_REMOTE_REPORT|Dbg), Str, Len)
#else
#define REMOTE_REPORT_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef REMOTE_REPORT_DYN_PRINT
#ifdef REMOTE_REPORT_DBG
#define REMOTE_REPORT_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_REMOTE_REPORT|Dbg), Fmt, ##Arg)
#else
#define REMOTE_REPORT_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define		MUI_REMOTE_SEND_RING_BUF_SIZE			(64*1024)
#define		MUI_REMOTE_RECV_RING_BUF_SIZE			(64*1024)


#define		REMOTE_API_SEND_LEN_MAX					(64*1024)
#define		REMOTE_API_RECV_LEN_MAX					(32*1024)

#define 	MSTR_REMOTE_API_DST_ID					"dstid"
#define		MSTR_REMOTE_API_SRC_ID					"srcid"
#define 	MSTR_REMOTE_API_SQN						"sqn"
#define		MSTR_REMOTE_API_MESSAGE					"message"
#define 	MSTR_REMOTE_API_ANSWER					"answer"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct _Sequence
{
	uint32_t ui32Id;
	PFun_RemoteReportCallback pFunAnswer;
	PFun_RemoteReportCallback pFunError;
	uint32_t ui32Start;
	uint32_t ui32WaitMs;
} S_RemoteRptSeq;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static uint8_t s_aui8RemoteSendBuf[MUI_REMOTE_SEND_RING_BUF_SIZE];	//Զ�̷��ͻ��λ����
static S_RingBuffer s_sRemoteSendRingBuf;							//Զ�̷��ͻ��λ���ؿ��ƿ�
static uint8_t s_aui8RemoteRecvBuf[MUI_REMOTE_RECV_RING_BUF_SIZE];	//Զ�̽��ջ��λ����
static S_RingBuffer s_sRemoteRecvRingBuf;							//Զ�̽��ջ��λ���ؿ��ƿ�

static uint32_t s_ui32RemoteApiSqn = 0;
static pthread_t s_sRemoteApiDealThread; /* �߳�ID*/
static sem_t s_tRemoteApiSem;

static S_RemoteRptSeq asRemoteRptSeqBuf[20];

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static uint32_t s_remote_report_sqn_gen (void);
static int s_remote_msg_send (const void *pvMsg, size_t tLen, uint32_t ui32DestId, uint32_t ui32Sqn);
static void* s_remote_api_deal_task (void *pArg);

#if 0
static int save_quence (PFun_RemoteReportCallback pFunAnswer, PFun_RemoteReportCallback pFunError, uint32_t waitms)
{
	int i = 0;

	//if (waitms != 0)
	{
		s_remote_report_sqn_gen();
		for (i = 0; i < 20; i++)
		{
			if (sequence[i].id == 0)
			{
				sequence[i].id = s_ui32RemoteApiSqn;
				sequence[i].time_t = sys_get_ms();
				sequence[i].pFunAnswer = pFunAnswer;
				sequence[i].pFunError = pFunError;
				sequence[i].waitms = waitms;
				return 1;
			}
		}
		if (i == 20)
		{
			REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "***********************sequence full!\r\n");
			return -1;
		}
	}
	return -1;
}

static int check_sequence (const uint32_t sqnid)
{
	int i = 0;
	for (i = 0; i < 20; i++)
	{
		if (sequence[i].id == sqnid)
		{
			sequence[i].id = 0;
			sequence[i].time_t = 0;
			return 1;
		}
	}
	if (i == 20)
	{
		return -1;
	}
	return 1;
}
#endif

static uint32_t s_remote_report_sqn_gen (void)
{
	return s_ui32RemoteApiSqn++;
}

//-------------------------------------------------------------------------------------------------*
/*	Զ�̱��ķ��ͺ�������
;	(����)��Զ�̱��İ�ָ����ʽ��װ�����Զ�̷��ͻ��λ�����
;	(����)
;		pvMsg:		Ҫ���͵ı���ָ��
;		tLen:		Ҫ���͵ı��ĳ���
;		ui32DestId:	Ҫ���͵�Ŀ��ID
;		ui32Sqn:	���ĵ����к�
;	(����)
;		NULL
;	(ע��)
*/
static int s_remote_msg_send (const void *pvMsg, size_t tLen, uint32_t ui32DestId, uint32_t ui32Sqn)
{
	S_StrDevId sDevId;
	S_BufUnit asBufUnit[4];
	uint8_t aui8Buf[70];	//{"dstid":1234567890,"srcid":1234567891,"sqn":1111111111,"message":
	uint16_t ui16Len;
	int iRet;

	ASSERT(pvMsg != NULL);
	ASSERT(0 < tLen);

	REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_msg_send]tMsgLen = %u\r\n", tLen);
	REMOTE_REPORT_DYN_PUTS(M_DBG_LEVEL_DEBUG, (const char*)pvMsg, tLen);
	REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	iRet = read_config_info_dev_id(&sDevId);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = snprintf(aui8Buf, sizeof(aui8Buf), "{\"dstid\":%u,\"srcid\":%s,\"sqn\":%u,\"message\":", ui32DestId, sDevId.str, ui32Sqn);
	if ((iRet<0) || (iRet>=(sizeof(aui8Buf)-1)))
	{
		return -1;
	}

	if (ring_buf_remain(&s_sRemoteSendRingBuf) < (sizeof(uint16_t)+iRet+tLen+1))
	{
		REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_WARNING,"[s_remote_msg_send]Ring buffer is full!\r\n");
		return -1;
	}

	// ���뻺���Event���ݸ�ʽ����
	// |  2 Bytes  |                                         len                                          |
	// |  Buf Len  |  Buf Data({"dstid":1234567890,"srcid":1234567891,"sqn":1111111111,"message":pvBuf})  |
	ui16Len = (uint16_t)(iRet + tLen + sizeof("}") - 1);
	asBufUnit[0].pvBuf = &ui16Len;
	asBufUnit[0].tLen  = sizeof(uint16_t);
	asBufUnit[1].pvBuf = aui8Buf;
	asBufUnit[1].tLen  = iRet;
	asBufUnit[2].pvBuf = pvMsg;
	asBufUnit[2].tLen  = tLen;
	asBufUnit[3].pvBuf = "}";
	asBufUnit[3].tLen  = sizeof("}") - 1;

	iRet = (int)ring_buf_asm_wr(&s_sRemoteSendRingBuf, asBufUnit, sizeof(asBufUnit)/sizeof(asBufUnit[0]));

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Զ��API����������
;	(����)����Զ�̷��ͼ����ܻ��α��Ļ�����,��ʱ������.
;	(����)
;		pArg:	�����������еĲ���
;	(����)
;		NULL
;	(ע��)
*/
static void* s_remote_api_deal_task (void *pArg)
{
#if (REMOTE_API_SEND_LEN_MAX > REMOTE_API_RECV_LEN_MAX)
	uint8_t aui8Buf[REMOTE_API_SEND_LEN_MAX];
#else
	uint8_t aui8Buf[REMOTE_API_RECV_LEN_MAX];
#endif
	size_t tLen;
	int iRet;

	while (1)
	{
		if (0 > sem_wait_ms(&s_tRemoteApiSem, 10000))
		{
			REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_api_deal_task]Wait for s_tRemoteApiSem timeouts.\r\n");
			//��������
			continue;
		}

		if (!ring_buf_if_empty(&s_sRemoteRecvRingBuf))
		{
			//REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_api_deal_task]Have receive remote API message.\r\n");
			iRet = msg_ring_buf_rd(&s_sRemoteRecvRingBuf, aui8Buf, sizeof(aui8Buf), &tLen);
			if (iRet != (int)(sizeof(uint16_t)+tLen))
			{
				REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_api_deal_task]Read remote API receive buffer error!\r\n");
			}
			else
			{
				aui8Buf[tLen] = '\0';
				tLen++;
				iRet = remote_api_deal(aui8Buf, tLen);
				if (iRet < 0)
				{
					REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_api_deal_task]Call 'remote_api_deal()' fail!\r\n");
				}
			}
		}

		if (!ring_buf_if_empty(&s_sRemoteSendRingBuf))
		{
			//REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_api_deal_task]Will send remote API message.\r\n");
			iRet = msg_ring_buf_rd(&s_sRemoteSendRingBuf, aui8Buf, sizeof(aui8Buf), &tLen);
			if (iRet != (int)(sizeof(uint16_t)+tLen))
			{
				REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_api_deal_task]Read remote API send buffer fail!\r\n");
			}
			else
			{
				aui8Buf[tLen] = '\0';
				tLen++;
				iRet = remote_net_send_data((const char*)aui8Buf, tLen);
				if (iRet < 0)
				{
					REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_api_deal_task]Call 'remote_net_send_data()' fail!\r\n");
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------*


//-------------------------------------------------------------------------------------------------*
int remote_api_init (void)
{
	int iRet;

	memset(&s_aui8RemoteRecvBuf, 0, sizeof(s_aui8RemoteRecvBuf));
	if (true != ring_buf_init(&s_sRemoteRecvRingBuf, &s_aui8RemoteRecvBuf[0], &s_aui8RemoteRecvBuf[sizeof(s_aui8RemoteRecvBuf)-1]))
	{
		return -1;
	}
	memset(&s_aui8RemoteSendBuf, 0, sizeof(s_aui8RemoteSendBuf));
	if (true != ring_buf_init(&s_sRemoteSendRingBuf, &s_aui8RemoteSendBuf[0], &s_aui8RemoteSendBuf[sizeof(s_aui8RemoteSendBuf)-1]))
	{
		return -1;
	}

	iRet = sem_init(&s_tRemoteApiSem, 0, 0);
	if (iRet < 0)
	{
		REMOTE_REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "s_tRemoteApiSem init failed!\r\n");
		return iRet;
	}

	iRet = pthread_create(&s_sRemoteApiDealThread, NULL, s_remote_api_deal_task, NULL);
	if (iRet < 0)
	{
		return -1;
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
int remote_msg_receive (const void *pvMsg, size_t tLen)
{
	int iRet;

	ASSERT(pvMsg != NULL);
	ASSERT(0 < tLen);

	iRet = msg_ring_buf_wr(&s_sRemoteRecvRingBuf, pvMsg, tLen);
	if (iRet > (int)sizeof(uint16_t))
	{
		sem_post(&s_tRemoteApiSem);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Զ��API�ش���
;	(����)����Զ��API�Ļش��İ�ָ����ʽ��װ�����Զ�̷��ͻ��λ�����
;	(����)
;		pvMsg:		Ҫ���͵ı���ָ��
;		tLen:		Ҫ���͵ı��ĳ���
;		ui32DestId:	Ҫ���͵�Ŀ��ID
;		ui32Sqn:	���ĵ����к�
;	(����)
;		NULL
;	(ע��)
*/
int remote_api_answer (const void *pvMsg, size_t tLen, uint32_t ui32DestId, uint32_t ui32Sqn)
{
	ASSERT(pvMsg != NULL);
	ASSERT(0 < tLen);

	return s_remote_msg_send(pvMsg, tLen, ui32DestId, ui32Sqn);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Զ���ϱ����ͺ���
;	(����)����Զ��API�Ļش��İ�ָ����ʽ��װ�����Զ�̷��ͻ��λ�����
;	(����)
;		pvMsg:		Ҫ���͵ı���ָ��
;		tLen:		Ҫ���͵ı��ĳ���
;		pfunAnswer:	�յ�Ӧ���ĵĴ�����ָ��
;		pfunError:	�յ������ĵĴ�����ָ��
;		ui32WaitMs:	��ʱms��
;	(����)
;		NULL
;	(ע��)
*/
int remote_report_send (const void *pvMsg, size_t tLen, PFun_RemoteReportCallback pfunAnswer, PFun_RemoteReportCallback pfunError, uint32_t ui32WaitMs)
{
	uint32_t ui32DestId, ui32Sqn;
	int iRet;

	ASSERT(pvMsg != NULL);
	ASSERT(0 < tLen);

	ui32DestId = MUI_CLOUD_ID_DB_SERVER;
	ui32Sqn = s_remote_report_sqn_gen();

	if ((0!=ui32WaitMs) && ((NULL!=pfunAnswer)||(NULL!=pfunError)))
	{
		//��¼��������
	}

	iRet = s_remote_msg_send(pvMsg, tLen, ui32DestId, ui32Sqn);
	if (iRet > (int)sizeof(uint16_t))
	{
		sem_post(&s_tRemoteApiSem);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
#endif

