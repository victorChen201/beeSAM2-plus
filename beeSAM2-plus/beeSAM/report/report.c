#include "report.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define REPORT_DBG

#ifndef REPORT_PUTC
#ifdef REPORT_DBG
#define REPORT_PUTC(Char)			PUTC(Char)
#else
#define REPORT_PUTC(Char)
#endif
#endif

#ifndef REPORT_PUTS
#ifdef REPORT_DBG
#define REPORT_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define REPORT_PUTS(Str, Len)
#endif
#endif

#ifndef REPORT_PUTASCC
#ifdef REPORT_DBG
#define REPORT_PUTASCC(Char)			PUTASCC(Char)
#else
#define REPORT_PUTASCC(Char)
#endif
#endif

#ifndef REPORT_PUTASCS
#ifdef REPORT_DBG
#define REPORT_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define REPORT_PUTASCS(Str, Len)
#endif
#endif

#ifndef REPORT_PRINT
#ifdef REPORT_DBG
#define REPORT_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define REPORT_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef REPORT_DYN_PUTC
#ifdef REPORT_DBG
#define REPORT_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_REPORT|Dbg), Char)
#else
#define REPORT_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef REPORT_DYN_PUTS
#ifdef REPORT_DBG
#define REPORT_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_REPORT|Dbg), Str, Len)
#else
#define REPORT_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef REPORT_DYN_PUTASCC
#ifdef REPORT_DBG
#define REPORT_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_REPORT|Dbg), Char)
#else
#define REPORT_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef REPORT_DYN_PUTASCS
#ifdef REPORT_DBG
#define REPORT_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_REPORT|Dbg), Str, Len)
#else
#define REPORT_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef REPORT_DYN_PRINT
#ifdef REPORT_DBG
#define REPORT_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_REPORT|Dbg), Fmt, ##Arg)
#else
#define REPORT_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//-------------------------------------------------------------------------------------------------*
/*	msg_ring_buf_rd报文环形缓冲区读出函数
;	(功能)从环形缓冲区读出最早的一条报文.
;	(参数)
;		psRingBuf:	指向环形缓存控制结构体
;		pvBuf:		指向读出报文数据缓存区的指针
;		tSize:		缓存区的尺寸
;		ptLen:		报文数据的长度回写指针
;	(返回)
;		int:		实际读出的数据的字节数,负数表示出错
;	(注意)
;
*/
int msg_ring_buf_rd (S_RingBuffer *psRingBuf, void *pvBuf, size_t tSize, size_t *ptLen)
{
	uint16_t ui16MsgLen;
	size_t tTmp, tLen;

	ASSERT(psRingBuf != NULL);
	ASSERT(pvBuf != NULL);
	ASSERT(tSize > 0);
	ASSERT(ptLen != NULL);

	tLen = 0;
	tTmp = ring_buf_rd(psRingBuf, &ui16MsgLen, sizeof(uint16_t));
	tLen += tTmp;
	if (sizeof(uint16_t) != tTmp)
	{
		return (int)tLen;
	}
	*ptLen = (size_t)ui16MsgLen;

	if (tSize >= *ptLen)
	{
		tTmp = ring_buf_rd(psRingBuf, pvBuf, *ptLen);
		tLen += tTmp;
	}
	else
	{
		tTmp = ring_buf_rd(psRingBuf, pvBuf, tSize);
		tLen += tTmp;
		ring_buf_drop(psRingBuf, *ptLen - tSize);
	}

	return (int)tLen;
}
//..............................................................................
/*	msg_ring_buf_wr报文环形缓冲区写入函数
;	(功能)向环形缓冲区写入一条报文数据.
;	(参数)
;		psRingBuf:	指向环形缓存控制结构体
;		pvMsg:		指向写入消息数据的指针
;		tLen:		消息数据的长度
;	(返回)
;		int:		实际写入的数据的字节数,负数表示出错
;	(注意)
;
*/
int msg_ring_buf_wr (S_RingBuffer *psRingBuf, const void *pvMsg, size_t tLen)
{
	S_BufUnit asBufUnit[2];
	uint16_t ui16Len;
	int iRet;

	ASSERT(psRingBuf != NULL);
	ASSERT(pvMsg != NULL);
	ASSERT(0 < tLen);

	if (ring_buf_remain(psRingBuf) < (sizeof(uint16_t)+tLen))
	{
		REPORT_DYN_PRINT(M_DBG_LEVEL_ERR,"[remote_send_buf_wr]Ring buffer is full!\r\n");
		return -1;
	}

	// 存入缓存的Event数据格式如下
	// |  2 Bytes  |    len     |
	// |  Buf Len  |  Buf Data  |
	ui16Len = (uint16_t)tLen;
	asBufUnit[0].pvBuf = &ui16Len;
	asBufUnit[0].tLen  = sizeof(uint16_t);
	asBufUnit[1].pvBuf = pvMsg;
	asBufUnit[1].tLen  = tLen;

	REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[msg_ring_buf_wr]tMsgLen = %u\r\n", tLen);
	REPORT_DYN_PUTS(M_DBG_LEVEL_DEBUG, (const char*)pvMsg, tLen);
	REPORT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	iRet = (int)ring_buf_asm_wr(psRingBuf, asBufUnit, sizeof(asBufUnit)/sizeof(asBufUnit[0]));

	return iRet;
}
//-------------------------------------------------------------------------------------------------*

//..............................................................................
/*	send_buf_if_empty命令缓存是否为空(查询)函数
 ;	(功能)检查Zigbee命令缓存是否为空.
 ;	(参数)
 ;		NULL
 ;	(返回)
 ;		bool:	true:空; false:非空
 ;	(注意)
 ;
 */

int report_msg_send (const uint8_t *pbuf, const size_t tlen, PFun_RemoteReportCallback pFunAnswer, PFun_RemoteReportCallback pFunError, uint32_t waitms)
{
	ASSERT(pbuf != NULL);
	ASSERT(tlen != 0);
	int iRet = 0;

	iRet = local_report_send(pbuf, tlen);
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = remote_report_send(pbuf, tlen, pFunAnswer, pFunError, waitms);
	if (iRet < 0)
	{
		return iRet;
	}
	return iRet;
}


//-------------------------------------------------------------------------------------------------*
/*	(基于)JSON数据发送report函数
;	(功能)
;	(参数)
;		ui8Method:	方法(PUT/GET/DELETE/POST; ADD/DEL/MODIFY/CALL/ANSWER)
;		ui16Type:	设备类型
;		ui16Id:		设备ID
;		psBody:		BODY的JSON结构体包
;	(返回)
;		int:		实际读取到数据的字节数,负数表示出错
;	(注意)
*/
int send_report_with_json (uint8_t ui8Method, uint16_t ui16Type, uint16_t ui16Id, cJSON *psBody)
{
	char *pchJson;
	int iRet;

	ASSERT(NULL != psBody);

	iRet = json_assemble_report_with_body(ui8Method, ui16Type, ui16Id, &pchJson, psBody);
	if (iRet < 0)
	{
		REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[send_report_with_json]Call 'json_assemble_report_with_body()' fail.\r\n");
		return iRet;
	}

	iRet = report_msg_send(pchJson, iRet, NULL, NULL, 0);
	json_free(pchJson);
	if (iRet < 0)
	{
		REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[send_report_with_json]Call 'report_msg_send()' fail.\r\n");
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	组装设备report(JSON包)函数
;	(功能)
;	(参数)
;		ui8Method:	方法(PUT/GET/DELETE/POST; ADD/DEL/MODIFY/CALL/ANSWER)
;		ui16Type:	设备类型
;		ui16Id:		设备ID
;		ui8Mode:	模式(bit0:State; bit1:ApiAttr; bit2:ZbAttr; bit3:ExtAttr)
;	(返回)
;		int:		实际读取到数据的字节数,负数表示出错
;	(注意)
*/
int send_dev_report (uint8_t ui8Method, uint16_t ui16Type, uint16_t ui16Id, uint8_t ui8Mode)
{
	cJSON *psBody;
	int iRet;

	iRet = json_obj_assemble_dev_data(ui16Type, ui16Id, ui8Mode, &psBody);
	if (iRet < 0)
	{
		REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[send_dev_report]Call 'json_obj_assemble_dev_data()' fail.\r\n");
		return iRet;
	}

	iRet = send_report_with_json(ui8Method, ui16Type, ui16Id, psBody);
	if (iRet < 0)
	{
		REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[send_dev_report]Call 'send_report_with_json()' fail.\r\n");
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	组装设备report(JSON包)函数
;	(功能)
;	(参数)
;		ui8Method:	方法(PUT/GET/DELETE/POST; ADD/DEL/MODIFY/CALL/ANSWER)
;		ui16Type:	设备类型
;		ui16Id:		设备ID
;		ui8State:	状态值
;	(返回)
;		int:		实际读取到数据的字节数,负数表示出错
;	(注意)
*/
int send_dev_state_report (uint8_t ui8Method, uint16_t ui16Type, uint16_t ui16Id, uint8_t ui8State)
{
	cJSON *psBody;
	int iRet;

	iRet = json_obj_assemble_dev_state(ui16Type, ui16Id, ui8State, &psBody);
	if (iRet < 0)
	{
		REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[send_dev_state_report]Call 'json_obj_assemble_dev_data()' fail.\r\n");
		return iRet;
	}

	iRet = send_report_with_json(ui8Method, ui16Type, ui16Id, psBody);
	if (iRet < 0)
	{
		REPORT_DYN_PRINT(M_DBG_LEVEL_ERR, "[send_dev_state_report]Call 'send_report_with_json()' fail.\r\n");
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*

