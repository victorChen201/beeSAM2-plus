/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of at_port_linux module for beeSAM project.
File name:at_port_linux.c
Description��

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "at_port.h"
#include "at_port_linux.h"
#include "zigbee_manage.h"
#include "api_deal.h"
#include"uzipupgrade_xmoedm.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define AT_PORT_LINUX_DBG

#ifndef AT_PORT_LINUX_PUTC
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_PUTC(Char)			PUTC(Char)
#else
#define AT_PORT_LINUX_PUTC(Char)
#endif
#endif

#ifndef AT_PORT_LINUX_PUTS
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define AT_PORT_LINUX_PUTS(Str, Len)
#endif
#endif

#ifndef AT_PORT_LINUX_PUTASCC
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_PUTASCC(Char)			PUTASCC(Char)
#else
#define AT_PORT_LINUX_PUTASCC(Char)
#endif
#endif

#ifndef AT_PORT_LINUX_PUTASCS
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define AT_PORT_LINUX_PUTASCS(Str, Len)
#endif
#endif

#ifndef AT_PORT_LINUX_PRINT
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define AT_PORT_LINUX_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef AT_PORT_LINUX_DYN_PUTC
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_AT_PORT_LINUX|Dbg), Char)
#else
#define AT_PORT_LINUX_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef AT_PORT_LINUX_DYN_PUTS
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_AT_PORT_LINUX|Dbg), Str, Len)
#else
#define AT_PORT_LINUX_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef AT_PORT_LINUX_DYN_PUTASCC
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_AT_PORT_LINUX|Dbg), Char)
#else
#define AT_PORT_LINUX_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef AT_PORT_LINUX_DYN_PUTASCS
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_AT_PORT_LINUX|Dbg), Str, Len)
#else
#define AT_PORT_LINUX_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef AT_PORT_LINUX_DYN_PRINT
#ifdef AT_PORT_LINUX_DBG
#define AT_PORT_LINUX_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_AT_PORT_LINUX|Dbg), Fmt, ##Arg)
#else
#define AT_PORT_LINUX_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//#define MSTR_AT_PORT_DEV_DEFT					"/dev/ttyS1"
#define MSTR_AT_PORT_DEV_DEFT					"/dev/ttyUSB0"

//#define M_AT_PORT_BAUDRATE					B115200
#define M_AT_PORT_BAUDRATE						B38400

#define MUI_AT_PORT_EVENT_BUF_SIZE				4096

#define MUI_XMODEM_UPGRADE_EOT 				0x04
#define MUI_XMODEM_UPGRADE_ACK 				0x06
#define MUI_XMODEM_UPGRADE_NAK				0x15
#define MUI_XMODEM_UPGRADE_CAN 				0x18

#if 1
#define M_AT_LINE_IF_IS_FINISH(_pStrTail)		(0==memcmp(((uint8_t*)(_pStrTail)-sizeof(s_aui8AtFinishFlag)+2), s_aui8AtFinishFlag, sizeof(s_aui8AtFinishFlag)-1))

#define M_AT_HEADER_IF_IS_OK(_pStr)				(0==memcmp(_pStr, s_aui8AtHeaderOK, sizeof(s_aui8AtHeaderOK)-1))
#define M_AT_HEADER_IF_IS_ERROR(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderERROR, sizeof(s_aui8AtHeaderERROR)-1))

#define M_AT_HEADER_IF_IS_READY(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderREADY, sizeof(s_aui8AtHeaderREADY)-1))
#define M_AT_HEADER_IF_IS_NEWDEVU(_pStr)		(0==memcmp(_pStr, s_aui8AtHeaderNEWDEVU, sizeof(s_aui8AtHeaderNEWDEVU)-1))
#define M_AT_HEADER_IF_IS_REPORTU(_pStr)		(0==memcmp(_pStr, s_aui8AtHeaderREPORTU, sizeof(s_aui8AtHeaderREPORTU)-1))
#define M_AT_HEADER_IF_IS_ZEREQ(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderZEREQ, sizeof(s_aui8AtHeaderZEREQ)-1))
#define M_AT_HEADER_IF_IS_ZSCHN(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderZSCHN, sizeof(s_aui8AtHeaderZSCHN)-1))
#define M_AT_HEADER_IF_IS_DEVLEFT(_pStr)		(0==memcmp(_pStr, s_aui8AtHeaderDEVLEFT, sizeof(s_aui8AtHeaderDEVLEFT)-1))
#define M_AT_HEADER_IF_IS_DBG(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderDBG, sizeof(s_aui8AtHeaderDBG)-1))
#else
#define M_AT_LINE_IF_IS_FINISH(_pStrTail)		(*((uint16_t*)s_aui8AtFinishFlag) == *((uint16_t*)((uint8_t*)(_pStrTail)-sizeof(s_aui8AtFinishFlag)+2)))

#define M_AT_HEADER_IF_IS_OK(_pStr)				(*((uint16_t*)s_aui8AtHeaderOK) == *((uint16_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_ERROR(_pStr)			(*((uint32_t*)s_aui8AtHeaderERROR) == *((uint32_t*)(_pStr)))

#define M_AT_HEADER_IF_IS_READY(_pStr)			(*((uint32_t*)s_aui8AtHeaderREADY) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_NEWDEVU(_pStr)		(*((uint32_t*)s_aui8AtHeaderNEWDEVU) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_REPORTU(_pStr)		(*((uint32_t*)s_aui8AtHeaderREPORTU) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_ZEREQ(_pStr)			(*((uint32_t*)s_aui8AtHeaderZEREQ) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_ZSCHN(_pStr)			(*((uint32_t*)s_aui8AtHeaderZSCHN) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_DEVLEFT(_pStr)		(*((uint32_t*)s_aui8AtHeaderDEVLEFT) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_DBG(_pStr)			(*((uint32_t*)s_aui8AtHeaderDBG) == *((uint32_t*)(_pStr)))
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//AT�˿�У���ַ�
typedef struct
{
	const void		*pvStr;
	uint16_t		ui16Len;
} S_ATPortVerfStr;

//AT�˿�У���ַ�����
typedef struct
{
	S_ATPortVerfStr	*psVerfStr;
	uint16_t		ui16Num;
} S_ATPortVerfStrCtrl;

//AT����Ӧ������
typedef struct
{
	void		*pvBuf;
	size_t		tBufSize;
	size_t		tCnt;
} S_ATCmdAnswerCfg;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static const uint8_t s_aui8AtFinishFlag[]	  = {MSTR_AT_FIN_FLG};

static const uint8_t s_aui8AtHeaderOK[]		  = {MSTR_AT_HEADER_OK};
static const uint8_t s_aui8AtHeaderERROR[]	  = {MSTR_AT_HEADER_ERROR};

static const uint8_t s_aui8AtHeaderREADY[]    = {MSTR_AT_HEADER_READY};
static const uint8_t s_aui8AtHeaderNEWDEVU[]  = {MSTR_AT_HEADER_NEWDEVU};
static const uint8_t s_aui8AtHeaderREPORTU[]  = {MSTR_AT_HEADER_REPORTU};
static const uint8_t s_aui8AtHeaderZEREQ[]	  = {MSTR_AT_HEADER_ZEREQ};
static const uint8_t s_aui8AtHeaderZSCHN[]	  = {MSTR_AT_HEADER_ZSCHN};
static const uint8_t s_aui8AtHeaderDEVLEFT[]  = {MSTR_AT_HEADER_DEVLEFT};
static const uint8_t s_aui8AtHeaderIMGREQ[]	 = {MSTR_AT_HEADER_IMGREQ};
static const uint8_t s_aui8AtHeaderIMGRDY[]  = {MSTR_AT_HEADER_IMGRDY};
static const uint8_t s_aui8AtHeaderDBG[]	  = {MSTR_AT_HEADER_DBG};

static const uint8_t s_aui8ATAnswerOK[]       = {MSTR_AT_HEADER_OK"\r\n"};
static const uint8_t s_aui8ATAnswerErrorAny[] = {MSTR_AT_HEADER_ERROR"*\r\n"};

static char s_achATPortDev[64];
static int s_iAtPortFd = -1;
static struct termios s_sOldTIO, s_sCurTIO;

static volatile S_ATCmdAnswerCfg s_sAtCmdAnswerCfg;
static uint32_t s_ui32ATQueryTicks = 0;

static sem_t s_tATAnswerBufMutex, s_tATQueryMutex, s_tATAnswerSem;
static pthread_t s_sAtPortListenThread;

static uint8_t s_ui8ATPortAvailable  = 0;

static uint8_t s_easy_mode = 0;			//��zip����ʱ����������ģʽ����ֵ����1
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int at_port_init (void);
static int at_port_open (void);
static int at_port_close (void);
static int at_port_rd (void *pvBuf, size_t tBufSize);
static int at_port_block_rd (void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs);
int at_port_block_wr (void *pvBuf, size_t tLen, uint32_t ui32WaitMs);
static int at_cmd_answer_cfg (void *pvBuf, size_t tBufSize, size_t tCnt);
static int at_cmd_asnwer_write (const void *pvBuf, size_t tLen);
static void* at_cmd_answer_line_finish_verify (const void *pvAnswer, size_t tLen);
static int at_cmd_imd_query (const void *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs);
static void* at_port_listen_task (void *pArg);

//-------------------------------------------------------------------------------------------------*
/*	at_port��ʼ������
;	(����)
;	(ע��)
*/
static int at_port_init (void)
{
	s_iAtPortFd = -1;
	memset(&s_sOldTIO, 0, sizeof(s_sOldTIO));
	memset(&s_sCurTIO, 0, sizeof(s_sCurTIO));

	at_port_close();

	return 0;
}

//-------------------------------------------------------------------------------------------------*
/*	at_cmd_to_easy_mode����
;	(����)�л���AT����linstenģʽ 0Ϊ����ģʽ  1ΪUzip����ģʽ
;	(ע��)
*/
int at_cmd_to_easy_mode(const uint8_t ui8)
{
	if(s_easy_mode == ui8)
	{
		return s_easy_mode;
	}
	s_easy_mode = ui8;
	return s_easy_mode;
}
//-------------------------------------------------------------------------------------------------*
/*	get_easy_mode����
;	(����)��ȡs_easy_mode
;	(ע��)
*/
int get_easy_mode()
{
	return s_easy_mode;
}
//..............................................................................
/*	at_port�򿪺���
;	(����)
;	(����)
;		NULL
;	(����)
;		int:		 <0:����(�����쳣���ȴ���ʱ)
;					>=0:�豸�ļ���ʾ
;	(ע��)
*/
static int at_port_open (void)
{
	int iTmp;

	iTmp = open(s_achATPortDev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (iTmp < 0)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[at_port_open]Can not open %s, error:%d\r\n", s_achATPortDev, iTmp);
		if (s_iAtPortFd < 0)
		{
			s_iAtPortFd = iTmp;
		}
	}
	else
	{
		s_iAtPortFd = iTmp;

		// save current serial port settings
		tcgetattr(s_iAtPortFd, &s_sOldTIO);
		// clear struct for new port settings
		memset(&s_sCurTIO, 0, sizeof(s_sCurTIO));

		// BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
		// CS8     : 8n1 (8bit,no parity,1 stopbit)
		// CLOCAL  : local connection, no modem contol
		// CREAD   : enable receiving characters
		s_sCurTIO.c_cflag = M_AT_PORT_BAUDRATE | CS8 | CLOCAL | CREAD | HUPCL;
		//s_sCurTIO.c_cflag = M_AT_PORT_BAUDRATE | CS8 | CLOCAL | CREAD;

 		// IGNPAR  : ignore bytes with parity errors
 		// otherwise make device raw (no other input processing)
 		s_sCurTIO.c_iflag = IGNPAR;
 		//s_sCurTIO.c_iflag = 0;

		// Raw output.
		s_sCurTIO.c_oflag = 0;

		// disable all echo functionality, and don't send signals to calling program
		s_sCurTIO.c_lflag = 0;

		// initialize all control characters
		// default values can be found in /usr/include/termios.h, and are given
		// in the comments, but we don't need them here
		s_sCurTIO.c_cc[VTIME]    = 0;     // inter-character timer unused
		s_sCurTIO.c_cc[VMIN]     = 1;     // blocking read until 1 character arrives

		// now clean the modem line and activate the settings for the port
		tcflush(s_iAtPortFd, TCIFLUSH);
		tcsetattr(s_iAtPortFd, TCSANOW, &s_sCurTIO);
	}

	return s_iAtPortFd;
}
//..............................................................................
/*	at_port�رպ���
;	(����)
;	(ע��)
*/
static int at_port_close (void)
{
	int iRet;

	if (s_iAtPortFd < 0)
	{
		return s_iAtPortFd;
	}

	tcflush(s_iAtPortFd, TCIFLUSH);
	tcsetattr(s_iAtPortFd, TCSANOW, &s_sOldTIO);

	iRet = close(s_iAtPortFd);
	if (0 == iRet)
	{
		s_iAtPortFd = -1;
	}
	else
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[at_port_close]Close FD-%d fail, errno:%d", s_iAtPortFd, errno);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	at_port��ȡ����
;	(����)
;	(����)
;		pvBuf:		��ȡ���ݴ�ŵĻ���
;		tBufSize:	����Ĵ�С,����ȡ���ݵ����ߴ�
;	(����)
;		int:			ʵ�ʶ�ȡ�����ݵ��ֽ���,������ʾ����
;	(ע��)
*/
static int at_port_rd (void *pvBuf, size_t tBufSize)
{
	int iLen;

	ASSERT(pvBuf != NULL);

	if (s_iAtPortFd < 0)
	{
		return s_iAtPortFd;
	}

	iLen = read(s_iAtPortFd, pvBuf, tBufSize);

	if (iLen >= 0)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_rd]Read %d bytes:\r\n", iLen);
		AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
		AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, pvBuf, iLen);
		AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
	}
	else
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_rd]Read serial port error.\r\n");
	}

	return iLen;
}
//..............................................................................
/*	at_port����ʽ��ȡ����
;	(����)
;	(����)
;		pvBuf:		��ȡ���ݴ�ŵĻ���
;		tBufSize:	����Ĵ�С,����ȡ���ݵ����ߴ�
;		ui32WaitMs:		�ȴ��ĳ�ʱ������
;	(����)
;		int:			ʵ�ʶ�ȡ�����ݵ��ֽ���,������ʾ����
;	(ע��)
*/
static int at_port_block_rd (void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs)
{
	struct timeval sWait;
	fd_set sFd;
	int iLen;

	ASSERT(pvBuf != NULL);

	if (s_iAtPortFd < 0)
	{
		return s_iAtPortFd;
	}

	sWait.tv_sec  = ui32WaitMs/1000;
	ui32WaitMs	 %= 1000;
	sWait.tv_usec = ui32WaitMs*1000;

	FD_ZERO(&sFd);
	FD_SET(s_iAtPortFd, &sFd);
	if (0 < select(s_iAtPortFd+1, &sFd, NULL, NULL, &sWait))
	{
		iLen = read(s_iAtPortFd, pvBuf, tBufSize);

		if (iLen >= 0)
		{
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_block_rd]Read %d bytes:\r\n", iLen);
			AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
			AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, pvBuf, iLen);
			AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
		}
		else
		{
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_block_rd]Read serial port error.\r\n");
		}
	}
	else
	{
		iLen = -1;
	}

	return iLen;
}
//-------------------------------------------------------------------------------------------------*
/*	at_port����ʽд�뺯��
;	(����)
;	(����)
;		pvBuf:	д�����ݴ�ŵĻ���
;		tLen:	д�����ݵ��ֽ���
;		ui32WaitMs:	�ȴ��ĳ�ʱ������
;	(����)
;		int:		ʵ��д������ݵ��ֽ���,������ʾ����
;	(ע��)
*/
int at_port_block_wr (void *pvBuf, size_t tLen, uint32_t ui32WaitMs)
{
	int iRet;

	ASSERT(pvBuf != NULL);

	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_block_wr]pvBuf:%s\r\n", (char*)pvBuf);
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_block_wr]tLen=%u, ui32WaitMs=%u.\r\n", tLen, ui32WaitMs);

	if (s_iAtPortFd < 0)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[at_port_block_wr]s_iAtPortFd invalid.\r\n");
		return AT_PORT_ERR_ATPORTFD_FAIL;
	}

	if (0 > sem_wait_ms(&s_tATQueryMutex, ui32WaitMs))
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_WARNING, "[at_port_block_wr]Wait sem overtime.\r\n");
		return AT_PORT_ERR_SEM_FAIL;
	}

	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_block_wr]==================Data Start====================.\r\n");
	int i;
	for (i=0; i<(int)tLen; i++)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%02hhx ", ((uint8_t*)pvBuf)[i]);
		if (0 == ((i+1)%16))
		{
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
		}
	}
	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_block_wr]===================Data End=====================.\r\n");

	iRet = write(s_iAtPortFd, pvBuf, tLen);
	if (iRet < 0)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[at_port_block_wr]Call 'write()' fail, errno:%d.\r\n", errno);
	}

	sem_post(&s_tATQueryMutex);

	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_block_wr]Have write to AT port %d bytes.\r\n", iRet);

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT����Ӧ�����ú���
;	(����)����AT����Ӧ��ķ���ָ��
;	(����)
;		pvBuf:		AT����Ӧ�𻺴�ָ��
;		tBufSize:	AT����Ӧ�𻺴�ߴ�
;		tCnt:		AT����Ӧ��ʵ�ʳ���
;	(����)
;		int:	 <0:����
;				>=0:��ȷ
;	(ע��)
*/
static int at_cmd_answer_cfg (void *pvBuf, size_t tBufSize, size_t tCnt)
{
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_asnwer_cfg]Function starting....\r\n");
	sem_wait(&s_tATAnswerBufMutex);
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_asnwer_cfg]Have got semaphore 's_tATAnswerBufMutex'.\r\n");
	s_sAtCmdAnswerCfg.tBufSize = tBufSize;
	s_sAtCmdAnswerCfg.tCnt	   = tCnt;
	s_sAtCmdAnswerCfg.pvBuf	   = pvBuf;
	sem_post(&s_tATAnswerBufMutex);
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_asnwer_cfg]Have released semaphore 's_tATAnswerBufMutex'.\r\n");

	return 0;
}
//..............................................................................
/*	at_port����ʽд�뺯��
;	(����)
;	(����)
;		pvBuf:	Ӧ������ָ��
;		tLen:	Ӧ�����ݳ���
;	(����)
;		int:	 <0:����
;				>=0:Ӧ�����ݻ�д�ĳ���
;	(ע��)
*/
static int at_cmd_asnwer_write (const void *pvBuf, size_t tLen)
{
	int iRet;

	ASSERT(pvBuf != NULL);
	ASSERT(tLen > 0);

	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_asnwer_write]Function starting....\r\n");
	sem_wait(&s_tATAnswerBufMutex);
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_asnwer_write]Have got semaphore 's_tATAnswerBufMutex'.\r\n");
	if (s_sAtCmdAnswerCfg.pvBuf != NULL)
	{
		tLen = (tLen < s_sAtCmdAnswerCfg.tBufSize) ? tLen : s_sAtCmdAnswerCfg.tBufSize;
		memcpy((void*)s_sAtCmdAnswerCfg.pvBuf, pvBuf, tLen);
		s_sAtCmdAnswerCfg.tCnt= tLen;

		iRet = (int)tLen;
		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_asnwer_write]Have wrote answer to special buffer.\r\n");
		//AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
		//AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (const char*)pvBuf, tLen);
		//AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	}
	else
	{
		iRet = -1;
		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_asnwer_write]Haven't wrote answer to special buffer because the point equal NULL.\r\n");
	}
	sem_post(&s_tATAnswerBufMutex);
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_asnwer_write]Have released semaphore 's_tATAnswerBufMutex'.\r\n");

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT����Ӧ���н�����֤����
;	(����)����AT����Ӧ��ķ���ָ��
;	(����)
;		pvAnswer:	Ӧ����ָ��
;		tLen:		Ӧ���ĳ���
;	(����)
;		void*:	Ӧ������ָ��(�ɹ�); NULL(ʧ��)
;	(ע��)
*/
static void* at_cmd_answer_line_finish_verify (const void *pvAnswer, size_t tLen)
{
	const uint8_t *pui8Tmp = pvAnswer+tLen-1;

	ASSERT(pvAnswer != NULL);
	ASSERT(tLen > 0);

	if ((tLen>3) && (((*(pui8Tmp-1)=='\r')&&(*(pui8Tmp-0)=='\n')) || ((*(pui8Tmp-5)=='C')&&(*(pui8Tmp-4)=='C')&&(*(pui8Tmp-3)=='C')&&(*(pui8Tmp-2)=='C')&&(*(pui8Tmp-1)=='C')&&(*(pui8Tmp-0)=='C'))))
	{
		return (void*)pvAnswer;
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT����Ӧ���н�����֤����
;	(����)����AT����Ӧ��ķ���ָ��
;	(����)
;		psCfg:	AT����Ӧ������
;	(����)
;		void*:	Ӧ������ָ��(�ɹ�); NULL(ʧ��)
;	(ע��)
*/
static void* x_modem_key_word_verify (const void *pvAnswer, size_t tLen)
{
	const uint8_t *pui8Tmp = pvAnswer+tLen-1;

	ASSERT(pvAnswer != NULL);
	ASSERT(tLen > 0);
	if (('C'==(char)*(pui8Tmp-0)) || (MUI_XMODEM_UPGRADE_ACK==(char)*(pui8Tmp-0)) || (MUI_XMODEM_UPGRADE_CAN==(char)*(pui8Tmp-0))|| (MUI_XMODEM_UPGRADE_NAK==(char)*(pui8Tmp-0)))
	{
		/*AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
		AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (char*)pui8Tmp, 1);
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");*/
		return (void*)pui8Tmp;
	}
	else if ((tLen>3) && ((*(pui8Tmp-1)=='\r')&&(*(pui8Tmp-0)=='\n')))
	{
		return (void*)pvAnswer;
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------*

//-------------------------------------------------------------------------------------------------*
/*	AT����������ѯ����
;	(����)
;	(����)
;		pvCmd:		AT�����ַ���
;		tCmdLen:	AT�����(�˳��Ȳ�����AT��������'\0')
;		pvBuf:		Ӧ�𻺴�(ֻҪBuffer�Ĵ�С�㹻,����������Ӧ���ַ�����������'\0')
;		tBufSize:	Ӧ�𻺴�Ĵ�С,����ȡ���ݵ����ߴ�
;		ui32WaitMs:	�ȴ��ĳ�ʱ������
;	(����)
;		int:		<0:����(�����쳣,�ȴ���ʱ)
;					 0:Ӧ��Ϊ��
;					>0:��Ӧ���ݵĳ���
;	(ע��)
*/
static int at_cmd_imd_query (const void *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs)
{
	uint32_t ui32Ticks;
	int iRet;
	static int Aterrnum = 0;

	ASSERT(pvCmd != NULL);
	ASSERT(pvBuf != NULL);
	ASSERT((0<tCmdLen) && (tCmdLen<=MUI_AT_PORT_CMD_LEN_MAX));
	ASSERT(tBufSize > 0);
	ASSERT(ui32WaitMs <= MUI_AT_QUERY_WAIT_MAX);

	if (s_iAtPortFd < 0)
	{
		return AT_PORT_ERR_ATPORTFD_FAIL;
	}
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_imd_query]Have checked s_iAtPortFd.\r\n");

	if (0 > sem_wait_ms(&s_tATQueryMutex, MUI_AT_QUERY_WAIT_MAX+1))	//�������(MUI_AT_QUERY_WAIT_MAX+1)
	{
		return AT_PORT_ERR_SEM_FAIL;
	}
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_imd_query]Have checked s_tATQueryMutex.\r\n");

	//�������ϴ�AT����֮��ļ��,��̫��������ʱ
	ui32Ticks = sys_get_ms();
	ui32Ticks -= s_ui32ATQueryTicks;
	if (ui32Ticks < MUI_AT_QUERY_INTERVAL_MIN)
	{
		msleep(MUI_AT_QUERY_INTERVAL_MIN-ui32Ticks);
	}
	s_ui32ATQueryTicks = sys_get_ms();

	sem_trywait(&s_tATAnswerSem);	//Just clean s_tATAnswerSem;

	//��ʼ��AT����Ӧ������
	at_cmd_answer_cfg(pvBuf, tBufSize-1, 0);	//��һ���ֽ����ڴ��'\0'

	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_imd_query]->:%u\r\n", sys_get_ms());
	AT_PORT_LINUX_DYN_PUTS(M_DBG_LEVEL_DEBUG, "[", 1);
	AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (const char*)pvCmd, (int)tCmdLen);
	AT_PORT_LINUX_DYN_PUTS(M_DBG_LEVEL_DEBUG, "]\r\n", 3);

	iRet = write(s_iAtPortFd, pvCmd, tCmdLen);
	if (iRet > 0)
	{
	   	if (0 > sem_wait_ms(&s_tATAnswerSem, ui32WaitMs))
	   	{
			((uint8_t*)pvBuf)[0] = '\0';
			iRet = AT_PORT_ERR_AT_QUERY_OT;
			Aterrnum++;
			if(Aterrnum > 3)
			{
				s_ui8ATPortAvailable = 0;
			}
	   	}
		else
		{
			ASSERT(s_sAtCmdAnswerCfg.tCnt < tBufSize);
			((uint8_t*)pvBuf)[s_sAtCmdAnswerCfg.tCnt] = '\0';
		#if 0
			iRet = (s_sAtCmdAnswerCfg.tCnt>0) ? (int)s_sAtCmdAnswerCfg.tCnt : AT_PORT_ERR_AT_NO_ANSWER;
		#else
			iRet = (int)s_sAtCmdAnswerCfg.tCnt;
		#endif
		}
	}

	//���AT����Ӧ������
	at_cmd_answer_cfg(NULL, 0, 0);

	sem_post(&s_tATQueryMutex);

	if (iRet > 0)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_imd_query]{n=%d-", iRet);
		AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (const char*)pvBuf, iRet);
		AT_PORT_LINUX_DYN_PUTS(M_DBG_LEVEL_DEBUG, "}\r\n", 3);
		Aterrnum = 0;
		/*
		//for test
		Aterrnum++;
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "--------------Aterrnum = %d\r\n",Aterrnum);
		if(Aterrnum > 5)
		{
			s_ui8ATPortAvailable = 0;
			//reset uzip
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "start--------------Aterrnum = %d\r\n",Aterrnum);
			at_port_reset();
			Aterrnum = 0;
		}
		*/
	}
	else
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_imd_query]No answer!\r\n");
	}

	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_imd_query]<-:%u\r\n", sys_get_ms());

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT�˿ڼ���������
;	(����)����AT�˿�,ʵ���յ����ĵ�ת��.
;	(����)
;		pArg:	�����������еĲ���
;	(����)
;		NULL
;	(ע��)
*/
static void* at_port_listen_task (void *pArg)
{
	struct timeval sWait;
	struct timeval *psWait = NULL;
	fd_set sFd;

	S_ATPortVerfStr sVerfHeader[MUI_AT_EVENT_TYPE_IMGRDY+1];	//Ŀǰ��֧��$DBG�¼�
	S_ATPortVerfStrCtrl sVerfHeaderCtrl;
	S_ATPortVerfStr sVerfTail[2];
	S_ATPortVerfStrCtrl sVerfTailCtrl;

	uint8_t aui8Buf[256];
	uint8_t *pui8Wr     = aui8Buf;
	uint8_t *pui8Scan   = aui8Buf;

	uint8_t *pui8ScanTmp, *pui8Tmp;
	size_t tTmp;

	int iRet, i;

	(void)(pArg);

	ASSERT(sizeof(sVerfHeader)/sizeof(S_ATPortVerfStr) == MUI_AT_EVENT_TYPE_IMGRDY+1);

	//��ʼ����֤ͷ�ַ���
	sVerfHeader[MUI_AT_EVENT_TYPE_READY].pvStr     = s_aui8AtHeaderREADY;
	sVerfHeader[MUI_AT_EVENT_TYPE_READY].ui16Len   = sizeof(s_aui8AtHeaderREADY) - 1;
	sVerfHeader[MUI_AT_EVENT_TYPE_NEWDEVU].pvStr   = s_aui8AtHeaderNEWDEVU;
	sVerfHeader[MUI_AT_EVENT_TYPE_NEWDEVU].ui16Len = sizeof(s_aui8AtHeaderNEWDEVU) - 1;
	sVerfHeader[MUI_AT_EVENT_TYPE_REPORTU].pvStr   = s_aui8AtHeaderREPORTU;
	sVerfHeader[MUI_AT_EVENT_TYPE_REPORTU].ui16Len = sizeof(s_aui8AtHeaderREPORTU) - 1;
	sVerfHeader[MUI_AT_EVENT_TYPE_ZEREQ].pvStr     = s_aui8AtHeaderZEREQ;
	sVerfHeader[MUI_AT_EVENT_TYPE_ZEREQ].ui16Len   = sizeof(s_aui8AtHeaderZEREQ) - 1;
	sVerfHeader[MUI_AT_EVENT_TYPE_ZSCHN].pvStr     = s_aui8AtHeaderZSCHN;
	sVerfHeader[MUI_AT_EVENT_TYPE_ZSCHN].ui16Len   = sizeof(s_aui8AtHeaderZSCHN) - 1;
	sVerfHeader[MUI_AT_EVENT_TYPE_DEVLEFT].pvStr   = s_aui8AtHeaderDEVLEFT;
	sVerfHeader[MUI_AT_EVENT_TYPE_DEVLEFT].ui16Len = sizeof(s_aui8AtHeaderDEVLEFT) - 1;
	sVerfHeader[MUI_AT_EVENT_TYPE_IMGREQ].pvStr    = s_aui8AtHeaderIMGREQ;
	sVerfHeader[MUI_AT_EVENT_TYPE_IMGREQ].ui16Len  = sizeof(s_aui8AtHeaderIMGREQ) - 1;
	sVerfHeader[MUI_AT_EVENT_TYPE_IMGRDY].pvStr    = s_aui8AtHeaderIMGRDY;
	sVerfHeader[MUI_AT_EVENT_TYPE_IMGRDY].ui16Len  = sizeof(s_aui8AtHeaderIMGRDY) - 1;

	//��ʼ����֤ͷ�ַ�����
	sVerfHeaderCtrl.psVerfStr = sVerfHeader;
	sVerfHeaderCtrl.ui16Num   = (sizeof(sVerfHeader)/sizeof(S_ATPortVerfStr));

	//��ʼ����֤β�ַ���
	sVerfTail[0].pvStr   = s_aui8ATAnswerOK;
	sVerfTail[0].ui16Len = sizeof(s_aui8ATAnswerOK) - 1;
	sVerfTail[1].pvStr   = s_aui8ATAnswerErrorAny;
	sVerfTail[1].ui16Len = sizeof(s_aui8ATAnswerErrorAny) - 1;
	//��ʼ����֤β�ַ�����
	sVerfTailCtrl.psVerfStr = sVerfTail;
	sVerfTailCtrl.ui16Num   = (sizeof(sVerfTail)/sizeof(S_ATPortVerfStr));

	while (s_iAtPortFd > 0)
	{
		//��ʼ��select�ļ�����������
		FD_ZERO(&sFd);
		FD_SET(s_iAtPortFd, &sFd);

		//�ȴ�������������
		iRet = select(s_iAtPortFd+1, &sFd, NULL, NULL, psWait);
		if (iRet <= 0)
		{
			pui8Wr	 = aui8Buf;
			pui8Scan = aui8Buf;
			psWait	 = NULL;
			if (iRet < 0)
			{
				//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Select fail, errno:%d.\r\n", errno);
			}
			else
			{
				//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Select overtime.\r\n");
			}
			continue;
		}
		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Have checked input from at port.\r\n");

		//read����
		iRet = read(s_iAtPortFd, pui8Wr, sizeof(aui8Buf)-(pui8Wr-&aui8Buf[0]));
		if (iRet <= 0)
		{
			if (iRet < 0)
			{
				pui8Wr	 = aui8Buf;
				pui8Scan = aui8Buf;
				psWait	 = NULL;
				//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Got error when read at port, errno:%d.\r\n", errno);
			}
			else
			{
				sWait.tv_sec  = 1;
				sWait.tv_usec = 0;
				//�˴������޸�psWait
				AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Got 0 bytes from at port, pui8Wr=%u, aui8Buf=%u\r\n", (uint32_t)pui8Wr, (uint32_t)aui8Buf);
				AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]psWait=%u, psWait->tv_sec=%lu, psWait->tv_usec=%lu\r\n",
							  (uint32_t)psWait,
					          ((psWait!=NULL) ? psWait->tv_sec : 0),
					          ((psWait!=NULL) ? psWait->tv_usec : 0));
			}
			continue;
		}
		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Have read %d bytes from at port.\r\n", iRet);
		//AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
		//AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (const char*)pui8Wr, iRet);
		//AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
		pui8Wr += iRet;

		if (s_easy_mode)
		{
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "&&&&&&&&&&&& s_easy_mode \r\n");
			if (NULL == x_modem_key_word_verify(aui8Buf, pui8Wr-&aui8Buf[0]))
			{
				sWait.tv_sec  = 1;
				sWait.tv_usec = 0;
				psWait        = &sWait;
				continue;
			}
			pui8Tmp = pui8Wr-1;
			if (('C'==*pui8Tmp) || (MUI_XMODEM_UPGRADE_ACK==*pui8Tmp) || (MUI_XMODEM_UPGRADE_CAN==*pui8Tmp)|| (MUI_XMODEM_UPGRADE_NAK==*pui8Tmp))
			{
				//x_modem_answer_deal(*pui8Wr);
				AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]&&&&&&&&&&&&[1]&&&&&&&&&&&&\r\n");
				AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (char*)pui8Tmp, 1);
				AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
				//д���¼�
				zigbee_event_checkout(pui8Tmp, sizeof(uint8_t), MUI_AT_EVENT_TYPE_UZIPUPGRADE);
				//���б���
				pui8Wr--;

				if (&aui8Buf[0] == pui8Wr)
				{
					continue;
				}
			}
		}

		//����Ƿ���\r\n��β
		if (NULL == at_cmd_answer_line_finish_verify(aui8Buf, pui8Wr-&aui8Buf[0]))
		{
			sWait.tv_sec  = 1;
			sWait.tv_usec = 0;
			psWait        = &sWait;
			//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Have got string without 'FINISH' from at port:\r\n");
			//AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
			//AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (const char*)aui8Buf, (int)(pui8Wr-&aui8Buf[0]));
			//AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
			//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
			continue;
		}
		pui8Tmp = pui8Wr-1;
		if (*(pui8Tmp-0)=='C')
		{
			//x_modem_answer_deal(*pui8Wr);
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]&&&&&&&&&&&&[1]&&&&&&&&&&&&\r\n");
			AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (char*)pui8Tmp, 1);
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
			//д���¼�
			Uzip_xmodem_init_DataInfo();
			zigbee_event_checkout(pui8Tmp, sizeof(uint8_t), MUI_AT_EVENT_TYPE_UZIPUPGRADE);
			//���б���
			pui8Wr--;

			if (&aui8Buf[0] == pui8Wr)
			{
				continue;
			}
		}

		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Have got string with 'FINISH' from at port:\r\n");
		//AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
		AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (const char*)aui8Buf, (int)(pui8Wr-&aui8Buf[0]));
		//AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

		//���Ҳ���ȡ�ض�ͷ���ַ���
		if (pui8Wr > pui8Scan)
		{
			for (i=0; i<sVerfHeaderCtrl.ui16Num; i++)
			{
				pui8ScanTmp = pui8Scan;
				while (NULL != (pui8Tmp=mem_header_chk(pui8ScanTmp, pui8Wr-pui8ScanTmp, sVerfHeaderCtrl.psVerfStr[i].pvStr, sVerfHeaderCtrl.psVerfStr[i].ui16Len, &tTmp)))
				{
					//i==MUI_AT_EVENT_TYPE_READY:�յ�$READY����
					//i==MUI_AT_EVENT_TYPE_NEWDEVU:�յ�$NEWNODE����
					//i==MUI_AT_EVENT_TYPE_REPORTU:�յ�$REPORTU����
					//i==MUI_AT_EVENT_TYPE_ZEREQ:�յ�$ZEREQ����
					//i==MUI_AT_EVENT_TYPE_ZSCHN:�յ�$ZSCHN����
					//i==MUI_AT_EVENT_TYPE_DEVLEFT:�յ�$DEVLEFT����
					zigbee_event_checkout(pui8Tmp, tTmp, (uint8_t)i);

					//���б���
					memcpy(pui8Tmp, pui8Tmp+tTmp, pui8Wr-pui8Tmp-tTmp);
					pui8ScanTmp  = pui8Tmp;
					pui8Wr      -= tTmp;
				}
			}
			pui8Scan = pui8Wr;
		}

		//����Ƿ����ض��ַ�����β
		for (i=0; i<sVerfTailCtrl.ui16Num; i++)
		{
			if (NULL != (pui8Tmp=mem_tail_chk(aui8Buf, pui8Wr-&aui8Buf[0], sVerfTailCtrl.psVerfStr[i].pvStr, sVerfTailCtrl.psVerfStr[i].ui16Len, &tTmp)))
			{
				at_cmd_asnwer_write(pui8Tmp, tTmp);
				//POST the semaphore 's_tATAnswerSem'
				//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Have wrote answer to special buffer.\r\n");

				sem_post(&s_tATAnswerSem);

				pui8Wr   = aui8Buf;
				pui8Scan = aui8Buf;
				psWait   = NULL;
				break;
			}
		}
		if (i >= sVerfTailCtrl.ui16Num)
		{
			//û�ռ�⵽Ԥ�ڵĽ�β
			sWait.tv_sec  = 1;
			sWait.tv_usec = 0;
			psWait        = &sWait;
		}
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT����ϵͳ��ʼ������
;	(����)
;	(����)
;		pchDev:		AT�����豸
;	(����)
;		int:		 <0:����(�����쳣���ȴ���ʱ)
;					>=0:�������
;	(ע��)
*/
int at_cmd_sys_init (const char *pchDev)
{
	int iRet;

	ASSERT(NULL != pchDev);

	strcpy(s_achATPortDev, pchDev);

	iRet = at_port_init();
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = sem_init(&s_tATAnswerBufMutex, 0, 1);	//��ʼ��Ϊ1
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = sem_init(&s_tATQueryMutex, 0, 1);	//��ʼ��Ϊ1
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = sem_init(&s_tATAnswerSem, 0, 0);		//��ʼ��Ϊ0
	if (iRet < 0)
	{
		return iRet;
	}

	//��ʼ��Ϊat_port������
	s_ui8ATPortAvailable = 0;

	iRet = at_port_open();
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = pthread_create(&s_sAtPortListenThread, NULL, at_port_listen_task, NULL);
	if (iRet < 0)
	{
		at_port_close();
		return iRet;
	}

	s_ui32ATQueryTicks = sys_get_ms();

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	at port��λ����
;	(����)
;	(����)
;		NULL
;	(����)
;		int:	��λ���
;	(ע��)
*/
int at_port_reset (void)
{
#ifdef M_UZIP_USB_MODULE
	char achRstCmd[] = {AT_CMD_RST_S"\r\n"};
	char achAnswer[MUI_AT_ANSWER_LEN_DFT];

	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_reset]Will reset uZip.\r\n");
	at_port_state_set(0);
	return at_cmd_imd_query(achRstCmd, sizeof(achRstCmd)-1, achAnswer, sizeof(achAnswer), 0);
#else
	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_reset]Will reset uZip.\r\n");
	at_port_state_set(0);
	return zigbee_module_rst();
#endif
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT port����(״̬����)����
;	(����)
;	(����)
;		ui8State:	AT�˿��Ƿ����
;	(����)
;		int:		s_ui8ATPortAvailable
;	(ע��)
*/
int at_port_state_set (uint8_t ui8State)
{
	return (int)(s_ui8ATPortAvailable = ui8State);
}
//..............................................................................
/*	AT port״̬��ȡ����
;	(����)
;	(����)
;		NULL
;	(����)
;		int:	s_ui8ATPortAvailable
;	(ע��)
*/
int at_port_state_get (void)
{
	return (int)s_ui8ATPortAvailable;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	at_portд�뺯��
;	(����)
;	(����)
;		pvBuf:	д�����ݴ�ŵĻ���
;		tLen:	д�����ݵ��ֽ���
;	(����)
;		int:		ʵ��д������ݵ��ֽ���,������ʾ����
;	(ע��)
*/
int at_port_wr (const void *pvBuf, size_t tLen)
{
	int iLen;

	ASSERT(pvBuf != NULL);

	if (s_iAtPortFd < 0)
	{
		return s_iAtPortFd;
	}

	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_wr]Will write:");
	AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
	AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, pvBuf, tLen);
	AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	iLen = write(s_iAtPortFd, pvBuf, tLen);

	if (iLen >= 0)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_wr]Wrote %u bytes:\r\n", tLen);
		AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
		AT_PORT_LINUX_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, pvBuf, iLen);
		AT_PORT_LINUX_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	}
	else
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_wr]Wrote serial port error.\r\n");
	}

	return iLen;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT�����ѯ����
;	(����)
;	(����)
;		pvCmd:		AT�����ַ���
;		tCmdLen:	AT�����(�˳��Ȳ�����AT��������'\0')
;		pvBuf:		Ӧ�𻺴�(ֻҪBuffer�Ĵ�С�㹻,����������Ӧ���ַ�����������'\0')
;		tBufSize:	Ӧ�𻺴�Ĵ�С,����ȡ���ݵ����ߴ�
;		ui32WaitMs:	�ȴ��ĳ�ʱ������
;	(����)
;		int:		<0:����(�����쳣,�ȴ���ʱ)
;					 0:Ӧ��Ϊ��
;					>0:��Ӧ���ݵĳ���
;	(ע��)
*/
int at_cmd_query (const void *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs)
{
	int iLen;

	ASSERT(pvCmd != NULL);
	ASSERT(pvBuf != NULL);
	ASSERT((0<tCmdLen) && (tCmdLen<=MUI_AT_PORT_CMD_LEN_MAX));
	ASSERT(tBufSize > 0);
	ASSERT(ui32WaitMs <= MUI_AT_QUERY_WAIT_MAX);

	if (0 == s_ui8ATPortAvailable)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[at_cmd_query]s_ui8ATPortAvailable == %u\r\n", s_ui8ATPortAvailable);
		return AT_PORT_ERR_UZIP_BUSY;
	}

	iLen = at_cmd_imd_query(pvCmd, tCmdLen, pvBuf, tBufSize, ui32WaitMs);
	if (iLen > 0)
	{
		if (AT_PORT_UZIP_ERR_E100 == at_cmd_answer_verify(pvBuf, iLen))
		{
			iLen = at_cmd_imd_query(pvCmd, tCmdLen, pvBuf, tBufSize, ui32WaitMs);
		}
	}

	return iLen;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT����Ӧ��У�麯��
;	(����)
;	(����)
;		pvAnswer:	AT�����ַ���
;		tAnswerLen:	AT�����
;	(����)
;		int:		<0:����(ATӦ���ʽ�����uZip������)
;					 0:OK
;	(ע��)
*/
int at_cmd_answer_verify (const void *pvAnswer, size_t tAnswerLen)
{
	uint16_t ui16Err;
	const char *pchAnswer = (const char*)pvAnswer;

	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_answer_verify]Answer[%u]:<%u>", tAnswerLen, (size_t)pchAnswer);
	AT_PORT_LINUX_DYN_PUTS(M_DBG_LEVEL_DEBUG, pchAnswer, tAnswerLen);
	AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	ASSERT(pvAnswer != NULL);
	ASSERT(tAnswerLen >= (sizeof(MSTR_AT_HEADER_OK)-1+sizeof(MSTR_AT_FIN_FLG)-1));	//Ĭ����̵�Ӧ����"OK\r\n"

	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_answer_verify]Start ...\r\n");
	//AT_PORT_LINUX_DYN_PUTS(M_DBG_LEVEL_DEBUG, pvAnswer, tAnswerLen);
	//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\nString length=%d\r\n", tAnswerLen);

	if (M_AT_HEADER_IF_IS_OK(pchAnswer+tAnswerLen-(sizeof(MSTR_AT_HEADER_OK)-1+sizeof(MSTR_AT_FIN_FLG)-1)))
	{
		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_answer_verify]Checked OK.\r\n");
		return 0;
	}
	else if (NULL != (pchAnswer=strnstr(pchAnswer, MSTR_AT_HEADER_ERROR, tAnswerLen)))
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_answer_verify]Checked ERROR.\r\n");
		if (1 == sscanf(pchAnswer, MSTR_AT_HEADER_ERROR":%04hx", &ui16Err))	//ERROR:0009<CR><LF>
		{
			AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_answer_verify]ERROR:%u\r\n", ui16Err);
			return AT_PORT_UZIP_ERR_BASE-ui16Err;
		}
		return AT_PORT_UZIP_ERR_BASE;
	}
	else
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_cmd_answer_verify]Unknow error.\r\n");
		return AT_PORT_ERR_UNKNOW_ANSWER;
	}
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT����ϵͳ�˳��ȴ�����
;	(����)
;	(����)
;		NULL
;	(����)
;		int:			 <0:����(�����쳣���ȴ���ʱ)
;						>=0:�����˳�
;	(ע��)
*/
int at_cmd_sys_quit_wait (void)
{
	int iRet;

	iRet = pthread_join(s_sAtPortListenThread, NULL);
	if (iRet != 0)
	{
		AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[at_cmd_sys_quit_wait]Error joining thread 'at_port_listen_task'.\r\n");
		abort();
	}

	iRet = at_port_close();

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT�˿�(ģ��)���(ռ��ϵͳ��Դ)����
;	(����)
;	(����)
;		NULL
;	(����)
;		NULL
;	(ע��)
*/
void at_port_clean (void)
{
	at_port_close();
}
//-------------------------------------------------------------------------------------------------*

