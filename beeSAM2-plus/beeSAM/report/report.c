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
/*	msg_ring_buf_rd���Ļ��λ�������������
;	(����)�ӻ��λ��������������һ������.
;	(����)
;		psRingBuf:	ָ���λ�����ƽṹ��
;		pvBuf:		ָ������������ݻ�������ָ��
;		tSize:		�������ĳߴ�
;		ptLen:		�������ݵĳ��Ȼ�дָ��
;	(����)
;		int:		ʵ�ʶ��������ݵ��ֽ���,������ʾ����
;	(ע��)
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
/*	msg_ring_buf_wr���Ļ��λ�����д�뺯��
;	(����)���λ�����д��һ����������.
;	(����)
;		psRingBuf:	ָ���λ�����ƽṹ��
;		pvMsg:		ָ��д����Ϣ���ݵ�ָ��
;		tLen:		��Ϣ���ݵĳ���
;	(����)
;		int:		ʵ��д������ݵ��ֽ���,������ʾ����
;	(ע��)
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

	// ���뻺���Event���ݸ�ʽ����
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
/*	send_buf_if_empty������Ƿ�Ϊ��(��ѯ)����
 ;	(����)���Zigbee������Ƿ�Ϊ��.
 ;	(����)
 ;		NULL
 ;	(����)
 ;		bool:	true:��; false:�ǿ�
 ;	(ע��)
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
/*	(����)JSON���ݷ���report����
;	(����)
;	(����)
;		ui8Method:	����(PUT/GET/DELETE/POST; ADD/DEL/MODIFY/CALL/ANSWER)
;		ui16Type:	�豸����
;		ui16Id:		�豸ID
;		psBody:		BODY��JSON�ṹ���
;	(����)
;		int:		ʵ�ʶ�ȡ�����ݵ��ֽ���,������ʾ����
;	(ע��)
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
/*	��װ�豸report(JSON��)����
;	(����)
;	(����)
;		ui8Method:	����(PUT/GET/DELETE/POST; ADD/DEL/MODIFY/CALL/ANSWER)
;		ui16Type:	�豸����
;		ui16Id:		�豸ID
;		ui8Mode:	ģʽ(bit0:State; bit1:ApiAttr; bit2:ZbAttr; bit3:ExtAttr)
;	(����)
;		int:		ʵ�ʶ�ȡ�����ݵ��ֽ���,������ʾ����
;	(ע��)
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
/*	��װ�豸report(JSON��)����
;	(����)
;	(����)
;		ui8Method:	����(PUT/GET/DELETE/POST; ADD/DEL/MODIFY/CALL/ANSWER)
;		ui16Type:	�豸����
;		ui16Id:		�豸ID
;		ui8State:	״ֵ̬
;	(����)
;		int:		ʵ�ʶ�ȡ�����ݵ��ֽ���,������ʾ����
;	(ע��)
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

