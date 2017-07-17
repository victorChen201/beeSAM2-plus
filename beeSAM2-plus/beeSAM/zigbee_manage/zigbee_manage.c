#include "zigbee_manage.h"
#include "config.h"
#include "api_deal.h"
#include "misc.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define ZIGBEE_MANAGE_DBG

#ifndef ZIGBEE_MANAGE_PUTC
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_PUTC(Char)			PUTC(Char)
#else
#define ZIGBEE_MANAGE_PUTC(Char)
#endif
#endif

#ifndef ZIGBEE_MANAGE_PUTS
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define ZIGBEE_MANAGE_PUTS(Str, Len)
#endif
#endif

#ifndef ZIGBEE_MANAGE_PUTASCC
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_PUTASCC(Char)			PUTASCC(Char)
#else
#define ZIGBEE_MANAGE_PUTASCC(Char)
#endif
#endif

#ifndef ZIGBEE_MANAGE_PUTASCS
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define ZIGBEE_MANAGE_PUTASCS(Str, Len)
#endif
#endif

#ifndef ZIGBEE_MANAGE_PRINT
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define ZIGBEE_MANAGE_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef ZIGBEE_MANAGE_DYN_PUTC
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_ZIGBEE_MANAGE|Dbg), Char)
#else
#define ZIGBEE_MANAGE_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef ZIGBEE_MANAGE_DYN_PUTS
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_ZIGBEE_MANAGE|Dbg), Str, Len)
#else
#define ZIGBEE_MANAGE_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef ZIGBEE_MANAGE_DYN_PUTASCC
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_ZIGBEE_MANAGE|Dbg), Char)
#else
#define ZIGBEE_MANAGE_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef ZIGBEE_MANAGE_DYN_PUTASCS
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_ZIGBEE_MANAGE|Dbg), Str, Len)
#else
#define ZIGBEE_MANAGE_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef ZIGBEE_MANAGE_DYN_PRINT
#ifdef ZIGBEE_MANAGE_DBG
#define ZIGBEE_MANAGE_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_ZIGBEE_MANAGE|Dbg), Fmt, ##Arg)
#else
#define ZIGBEE_MANAGE_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define AT_PORT_CTRL_DBG_CHK

#define MUI_AT_PORT_EVENT_LEN_MAX				64
#define MUI_AT_PORT_EVENT_BUF_SIZE				800

#define AT_CMD_JNNWK_TIMEOUT					MUI_AT_QUERY_WAIT_MAX
#define ZIGBEE_DEFAULT_CH						(0x0b)


#if 1
#define M_AT_HEADER_IF_IS_READY(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderREADY, sizeof(s_aui8AtHeaderREADY)-1))
#define M_AT_HEADER_IF_IS_NEWDEVU(_pStr)		(0==memcmp(_pStr, s_aui8AtHeaderNEWDEVU, sizeof(s_aui8AtHeaderNEWDEVU)-1))
#define M_AT_HEADER_IF_IS_REPORTU(_pStr)		(0==memcmp(_pStr, s_aui8AtHeaderREPORTU, sizeof(s_aui8AtHeaderREPORTU)-1))
#define M_AT_HEADER_IF_IS_ZEREQ(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderZEREQ, sizeof(s_aui8AtHeaderZEREQ)-1))
#define M_AT_HEADER_IF_IS_ZSCHN(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderZSCHN, sizeof(s_aui8AtHeaderZSCHN)-1))
#define M_AT_HEADER_IF_IS_DEVLEFT(_pStr)		(0==memcmp(_pStr, s_aui8AtHeaderDEVLEFT, sizeof(s_aui8AtHeaderDEVLEFT)-1))
#define M_AT_HEADER_IF_IS_DBG(_pStr)			(0==memcmp(_pStr, s_aui8AtHeaderDBG, sizeof(s_aui8AtHeaderDBG)-1))
#else
#define M_AT_HEADER_IF_IS_READY(_pStr)			(*((uint32_t*)s_aui8AtHeaderREADY) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_NEWDEVU(_pStr)		(*((uint32_t*)s_aui8AtHeaderNEWDEVU) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_REPORTU(_pStr)		(*((uint32_t*)s_aui8AtHeaderREPORTU) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_ZEREQ(_pStr)			(*((uint32_t*)s_aui8AtHeaderZEREQ) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_ZSCHN(_pStr)			(*((uint32_t*)s_aui8AtHeaderZSCHN) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_DEVLEFT(_pStr)		(*((uint32_t*)s_aui8AtHeaderDEVLEFT) == *((uint32_t*)(_pStr)))
#define M_AT_HEADER_IF_IS_DBG(_pStr)			(*((uint32_t*)s_aui8AtHeaderDBG) == *((uint32_t*)(_pStr)))
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	uint8_t		mode;
	uint16_t	duration;
	uint32_t	ms_ticks;
} S_JoinDevCtrl;

static const uint8_t s_aui8AtHeaderREADY[]	 = {MSTR_AT_HEADER_READY};
static const uint8_t s_aui8AtHeaderNEWDEVU[] = {MSTR_AT_HEADER_NEWDEVU};
static const uint8_t s_aui8AtHeaderREPORTU[] = {MSTR_AT_HEADER_REPORTU};
static const uint8_t s_aui8AtHeaderZEREQ[]	 = {MSTR_AT_HEADER_ZEREQ};
static const uint8_t s_aui8AtHeaderZSCHN[]	 = {MSTR_AT_HEADER_ZSCHN};
static const uint8_t s_aui8AtHeaderDEVLEFT[] = {MSTR_AT_HEADER_DEVLEFT};
static const uint8_t s_aui8AtHeaderIMGREQ[]	 = {MSTR_AT_HEADER_IMGREQ};
static const uint8_t s_aui8AtHeaderIMGRDY[]  = {MSTR_AT_HEADER_IMGRDY};
static const uint8_t s_aui8AtHeaderDBG[]	 = {MSTR_AT_HEADER_DBG};
static const uint8_t s_aui8AtHeaderTESTMODE[]= {MSTR_AT_HEADER_TESTMODE};

static char s_achPrintCmdBuf[MUI_AT_PORT_CMD_LEN_MAX+1];

static uint8_t s_aui8EventBuf[MUI_AT_PORT_EVENT_BUF_SIZE];	//事件数据环形缓冲池
static S_RingBuffer s_sEventRingBuf;						//事件数据环形缓冲池控制块
static uint8_t s_aui8AtCmdBuf[MUI_AT_CMD_BUF_SIZE];			//命令环形缓冲池
static S_RingBuffer s_sAtCmdRingBuf;						//命令环形缓冲池控制块

//static int G_iRes_no = 0;
//复位状态标志位
//static int G_iReset_status = 0;
//static uint32_t s_ui32ZigbeeQueryTicks = 0;
static S_JoinDevCtrl sJoinDevCtrl = {0, 0, 0};


static pthread_t s_sZigbeeCmdEventDealThread, s_sZigbeeAtCmdQueryThread;	/* 线程ID*/
static sem_t s_tZbModuleReadySem, s_tZbOrgQueryMutex, s_tATEventSem, s_tATCmdEventSem;
struct timespec sTime_t;

//static int set_join_time(uint16_t duration);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int zigbee_origin_query (const void *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs);
static int zigbee_origin_query_repeat (const char *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs,
									   uint32_t ui32Repeat, uint32_t ui32DlyMs);
static int zigbee_version_verify (char *pchUzipVer, size_t tSize);
static int zigbee_mac_verify (char *pchUzipMac, size_t tSize);
static int zigbee_sn_verify (char *pchUzipSn, size_t tSize);

static int zigbee_get_net_status (void);
static int zigbee_net_setup (void);
static int zigbee_join_dev_startup (uint8_t ui8Duration);
static int zigbee_event_verify (const uint8_t *pui8Buf, size_t tLen);
static int zigbee_event_buf_rd (void *pvEventBuf, size_t tBufSize, size_t *ptLen, uint8_t *pui8Type);
static int zigbee_cmd_buf_rd (void *pvCmdBuf, size_t tBufSize, size_t *ptLen, uint16_t *pui16DevType, uint16_t *pui16DevId);
static int zigbee_cmd_buf_wr (const void *pvCmd, size_t tCmdLen, uint16_t ui16DevType, uint16_t ui16DevId);
static bool zigbee_cmd_buf_if_empty (void);
static void* zigbee_cmd_event_deal_task (void *pArg);
static int zigbee_get_dev_type (const S_ZbAttrNwkAddr *psNwkAddr, uint8_t ui8MaxEpNum, S_ZbAttrEp *psZbEp, uint8_t *pui8EpCnt, U_DevExtInfo *puExtInfo);
static void* zigbee_at_cmd_query_task (void *pArg);
static int zigbee_dev_upgrade(const void *pvEvent, size_t tLen);
static int zigbee_dev_upgrade_result(const void *pvEvent, size_t tLen);
static int zigbee_uzip_upgrade(const void *pvEvent, size_t tLen);


//-------------------------------------------------------------------------------------------------*
/*	Zigbee(命令)原始查询函数
;	(功能)
;	(参数)
;		pvCmd:		AT命令字符串
;		tCmdLen:	AT命令长度
;		pvBuf:		应答缓存
;		tBufSize:	应答缓存的大小,即读取数据的最大尺寸
;		ui32WaitMs:	等待的超时毫秒数
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:没有回应
;					>0:回应数据的长度
;	(注意)
*/
static int zigbee_origin_query (const void *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs)
{
	int iRet;

	ASSERT(pvCmd != NULL);
	ASSERT(pvBuf != NULL);
	ASSERT((0<tCmdLen) && (tCmdLen<=MUI_AT_PORT_CMD_LEN_MAX));
	ASSERT(tBufSize > 0);
	ASSERT(ui32WaitMs <= MUI_AT_QUERY_WAIT_MAX);

	iRet = at_cmd_query(pvCmd, tCmdLen, pvBuf, tBufSize, ui32WaitMs);
	if(iRet <= 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_origin_query]at_cmd_query  filed\r\n");
	}

	return iRet;
}
//..............................................................................
/*	Zigbee(命令)原始重复查询函数
;	(功能)
;	(参数)
;		pvCmd:		AT命令字符串
;		pvBuf:		应答缓存
;		tCmdLen:	AT命令长度
;		tBufSize:	应答缓存的大小,即读取数据的最大尺寸
;		ui32WaitMs:	等待的超时毫秒数
;		ui32Repeat:	如果查询不成功的情况下,重复查询的次数
;		ui32DlyMs:	重复查询之间的延时(只收不是超时的情况下,才需要延时)
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:没有回应
;					>0:回应数据的长度
;	(注意)
*/
static int zigbee_origin_query_repeat (const char *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs,
									   uint32_t ui32Repeat, uint32_t ui32DlyMs)
{
	int iRet = 0;

	ASSERT(pvCmd != NULL);
	ASSERT((0<tCmdLen) && (tCmdLen<=MUI_AT_PORT_CMD_LEN_MAX));
	ASSERT(pvBuf != NULL);
	ASSERT(tBufSize > 0);
	ASSERT(ui32WaitMs <= MUI_AT_QUERY_WAIT_MAX);

	while (ui32Repeat-- > 0)
	{
		iRet = zigbee_origin_query(pvCmd, tCmdLen, pvBuf, tBufSize, ui32WaitMs);
		if (iRet > 0)
		{
			/*if (0 != at_cmd_answer_verify(pvBuf, iRet))
			{
				ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]at_cmd_answer_verify fail or AT answer error!\r\n");
				continue;
			}*/
			if (0 == at_cmd_answer_verify(pvBuf, iRet))
				break;

			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]at_cmd_answer_verify fail or AT answer error!\r\n");
		}
		if (ui32Repeat>0 && iRet!=AT_PORT_ERR_AT_QUERY_OT)
		{
			msleep(ui32DlyMs);
		}
	}
	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Zigbee版本信息校验的函数
;	(功能)  校验读到Zigbee版本和系统中保存的Zigbee版本信息
;	(参数)
;		pchUzipVer:	版本信息缓存指针
;		tSize:	缓存大小
;	(返回)
;		int:	<0: 出错(AT应答格式出错)
;				=0: 与系统中版本一致
;				>0: 版本不一致,已更新
;	(注意)
*/
static int zigbee_version_verify (char *pchUzipVer, size_t tSize)
{
	uint8_t aui8Answer[MUI_AT_ANSWER_LEN_ATI0];
	char achUzipVer[50] = {0};
	S_CfgUzipVer sCfgUzipVer;
	int iRet;

	iRet = zigbee_print_query(aui8Answer, sizeof(aui8Answer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ATI0_S"\r\n");
    if (iRet <= 0)
    {
		return -1;
    }
    iRet = at_cmd_answer_verify(aui8Answer, iRet);
    if (iRet != 0)
    {
		return -1;
    }
	if (1!=sscanf((char*)aui8Answer, "I:%s\r\n", achUzipVer))
	{
		return -1;
	}
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_INFO, "[zigbee_version_verify]uZip version:%s\r\n", achUzipVer);

	iRet = strlen(achUzipVer);
	if ((NULL!=pchUzipVer) && (tSize>(size_t)iRet))
	{
		strncpy(pchUzipVer, achUzipVer, iRet);
	}

	if ((size_t)iRet >= sizeof(sCfgUzipVer.str))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[zigbee_version_verify]uZip version is too long.\r\n");
		return -1;
	}

	iRet = read_config_info_uzip_ver(&sCfgUzipVer);
	if (iRet > 0)
	{
		if (0 == strncmp(achUzipVer, sCfgUzipVer.str, sizeof(sCfgUzipVer.str)))
		{
			return 0;
		}
		strncpy(sCfgUzipVer.str, achUzipVer, sizeof(sCfgUzipVer.str));
		iRet = write_config_info_uzip_ver(&sCfgUzipVer);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Zigbee MAC信息校验的函数
;	(功能)  校验读到Zigbee MAC和系统中保存的Zigbee MAC信息
;	(参数)
;		pchUzipMac:	MAC信息缓存指针
;		tSize:		缓存大小
;	(返回)
;		int:	<0: 出错(AT应答格式出错)
;				=0: 与系统中版本一致
;				>0: 版本不一致,已更新
;	(注意)
*/
static int zigbee_mac_verify (char *pchUzipMac, size_t tSize)
{
	uint8_t aui8Answer[MUI_AT_ANSWER_LEN_MACLR];
	char achUzipMac[50] = {0};
	S_CfgUzipMac sCfgUzipMac;
	int iRet;

	iRet = zigbee_print_query(aui8Answer, sizeof(aui8Answer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_R_MAC_UZIP_S"\r\n");
    if (iRet <= 0)
    {
		return -1;
    }
    iRet = at_cmd_answer_verify(aui8Answer, iRet);
    if (iRet != 0)
    {
		return -1;
    }
	if (1!=sscanf((char*)aui8Answer, "$MACLR:%s\r\n", achUzipMac))
	{
		return -1;
	}
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_INFO, "[zigbee_mac_verify]uZip MAC:%s\r\n", achUzipMac);

	iRet = strlen(achUzipMac);
	if ((NULL!=pchUzipMac) && (tSize>(size_t)iRet))
	{
		strncpy(pchUzipMac, achUzipMac, iRet);
	}

	if ((size_t)iRet >= sizeof(sCfgUzipMac.str))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[zigbee_mac_verify]uZip MAC is too long.\r\n");
		return -1;
	}

	iRet = read_config_info_uzip_mac(&sCfgUzipMac);
	if (iRet > 0)
	{
		if (0 == strncmp(achUzipMac, sCfgUzipMac.str, sizeof(sCfgUzipMac.str)))
		{
			return 0;
		}
		strncpy(sCfgUzipMac.str, achUzipMac, sizeof(sCfgUzipMac.str));
		iRet = write_config_info_uzip_mac(&sCfgUzipMac);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Zigbee SN信息校验的函数
;	(功能)  校验读到Zigbee SN和系统中保存的Zigbee SN信息
;	(参数)
;		pchUzipSn:	SN信息缓存指针
;		tSize:		缓存大小
;	(返回)
;		int:	<0: 出错(AT应答格式出错)
;				=0: 与系统中版本一致
;				>0: 版本不一致,已更新
;	(注意)
*/
static int zigbee_sn_verify (char *pchUzipSn, size_t tSize)
{
	uint8_t aui8Answer[MUI_AT_ANSWER_LEN_SNLR];
	char achUzipSn[50] = {0};
	S_CfgUzipSn sCfgUzipSn;
	int iRet;

	iRet = zigbee_print_query(aui8Answer, sizeof(aui8Answer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_R_SN_UZIP_S"\r\n");
    if (iRet <= 0)
    {
		return -1;
    }
    iRet = at_cmd_answer_verify(aui8Answer, iRet);
    if (iRet != 0)
    {
		return -1;
    }
	if (1!=sscanf((char*)aui8Answer, "$SNLR:%s\r\n", achUzipSn))
	{
		return -1;
	}
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_INFO, "[zigbee_sn_verify]uZip SN:%s\r\n", achUzipSn);

	iRet = strlen(achUzipSn);
	if ((NULL!=pchUzipSn) && (tSize>(size_t)iRet))
	{
		strncpy(pchUzipSn, achUzipSn, iRet);
	}

	if ((size_t)iRet >= sizeof(sCfgUzipSn.str))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[zigbee_sn_verify]uZip SN is too long.\r\n");
		return -1;
	}

	iRet = read_config_info_uzip_sn(&sCfgUzipSn);
	if (iRet > 0)
	{
		if (0 == strncmp(achUzipSn, sCfgUzipSn.str, sizeof(sCfgUzipSn.str)))
		{
			return 0;
		}
		strncpy(sCfgUzipSn.str, achUzipSn, sizeof(sCfgUzipSn.str));
		iRet = write_config_info_uzip_sn(&sCfgUzipSn);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*  查询Zigbee网络状态的函数
;	(功能)  查询Zigbee网络状态
;	(参数)
;	(返回)
;		int:	 <0: 无网络或出错(AT应答格式出错)
;				>=0: 有网络
;	(注意)
 */
static int zigbee_get_net_status (void)
{
	char aui8Answer[MUI_AT_ANSWER_LEN_DNS]="";
 	uint8_t ui8Channel;
	uint16_t ui16Tmp, ui16PanId;
	int iRet;

	iRet = zigbee_print_query(aui8Answer, sizeof(aui8Answer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_DNS_S"\r\n");
	if (iRet <= 0)
	{
		return -1;
	}
	iRet = at_cmd_answer_verify(aui8Answer, iRet);
	if (iRet != 0)
	{
		return -1;
	}
	iRet = sscanf(aui8Answer, "$NWK:%02hhx,%04hx,%04hx\r\n", &ui8Channel, &ui16Tmp, &ui16PanId);
	if (iRet != 3)
	{
		return -1;
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	创建Zigbee网络的函数
;	(功能)  创建Zigbee网络
;	(参数)
;	(返回)
;		int:	 <0: 创建网络失败或出错(AT命令失败)
;				>=0: 创建Zigbee网络成功
;	(注意)
*/
static int zigbee_net_setup (void)
{
	uint8_t aui8Answer[MUI_AT_ANSWER_LEN_JNNWK];
	int iRet;

	iRet = zigbee_get_net_status();
	if (iRet >= 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_net_setup]Have setup network.\r\n");
		return iRet;
	}
	iRet = zigbee_print_query(aui8Answer, sizeof(aui8Answer), AT_CMD_JNNWK_TIMEOUT, AT_CMD_JNNWK_S"\r\n");
	if (iRet <= 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_net_setup]"AT_CMD_JNNWK_S" command failed.\r\n");
		return -1;
	}
	iRet = at_cmd_answer_verify(aui8Answer, iRet);
	if (iRet != 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_net_setup]"AT_CMD_JNNWK_S" command answer wrong.\r\n");
		return -1;
	}

	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_net_setup]iRet = %d\r\n", iRet);

	return iRet;
}

//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	zigbee(网关)添加设备启动函数
;	(功能)  启动添加设备并设置持续时间
;	(参数)
;		ui8Duration:	0:		 关闭添加设备
;						(1~254): 添加设备持续的秒数
;						255:	 一直添加设备
;	(返回)
;		int:			 <0: 出错(AT应答格式出错)
;						>=0: 设置成功
;	(注意)
*/
static int zigbee_join_dev_startup (uint8_t ui8Duration)
{
	return zigbee_print_queue_query(MUI_DATA_TYPE_INVALID, MUI_DEV_ID_INVALID, AT_CMD_JNEN_S"=%02x\r\n", ui8Duration);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	zigbee 事件(报文)验证函数
;	(功能)
;	(参数)
;		pui8Buf:	缓存
;		tLen:		报文大小
;	(返回)
;		int:		<0:	不是Event
;					 0:	MUI_AT_EVENT_TYPE_READY
;					 1:	MUI_AT_EVENT_TYPE_NEWDEVU
;					 2:	MUI_AT_EVENT_TYPE_REPORTU
;					 3:	MUI_AT_EVENT_TYPE_ZEREQ
;					 4:	MUI_AT_EVENT_TYPE_ZSCHN
;					 5:	MUI_AT_EVENT_TYPE_DEVLEFT
;					 6:	MUI_AT_EVENT_TYPE_DBG
;					 7:MUI_AT_EVENT_TYPE_TESTMODE
;	(注意)
*/
static int zigbee_event_verify (const uint8_t *pui8Buf, size_t tLen)
{
	UNUSED(tLen);

	if (M_AT_HEADER_IF_IS_READY(pui8Buf))
	{
		return MUI_AT_EVENT_TYPE_READY;
 	}
	else if (M_AT_HEADER_IF_IS_NEWDEVU(pui8Buf))
	{
		return MUI_AT_EVENT_TYPE_NEWDEVU;
	}
	else if (M_AT_HEADER_IF_IS_REPORTU(pui8Buf))
	{
		return MUI_AT_EVENT_TYPE_REPORTU;
 	}
	else if (M_AT_HEADER_IF_IS_ZEREQ(pui8Buf))
	{
		return MUI_AT_EVENT_TYPE_ZEREQ;
 	}
	else if (M_AT_HEADER_IF_IS_ZSCHN(pui8Buf))
	{
		return MUI_AT_EVENT_TYPE_ZSCHN;
 	}
	else if (M_AT_HEADER_IF_IS_DEVLEFT(pui8Buf))
	{
		return MUI_AT_EVENT_TYPE_DEVLEFT;
 	}
#ifdef AT_PORT_CTRL_DBG_CHK
	else if (M_AT_HEADER_IF_IS_DBG(pui8Buf))
	{
		return MUI_AT_EVENT_TYPE_DBG;
 	}
#endif
	else if (M_AT_HEADER_IF_IS_DBG(pui8Buf))
	{
		return MUI_AT_EVENT_TYPE_DBG;
 	}

	return AT_PORT_ERR_UNKNOW_EVENT;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Zigbee 事件缓存读取函数
;	(功能)从缓冲区读出(收到的)事件数据.
;	(参数)
;		pvEventBuf:	指向读出事件数据缓存区的指针
;		tBufSize:	缓存区的尺寸
;		ptLen:		事件数据的长度回写指针
;		pui8Type:	事件类型回写指针
;	(返回)
;		int:		实际读出的数据的字节数,负数表示出错
;	(注意)
;
*/
static int zigbee_event_buf_rd (void *pvEventBuf, size_t tBufSize, size_t *ptLen, uint8_t *pui8Type)
{
	uint8_t ui8Tmp;
	size_t tTmp;
	size_t tLen = 0;

	ASSERT(pvEventBuf != NULL);
	ASSERT(tBufSize > 0);
	ASSERT(ptLen != NULL);
	ASSERT(pui8Type != NULL);

	// 存入缓存的Event数据格式如下
	// |  1 Bytes   |  1 Bytes  |  Buf Len  |
	// | Event Type |  Buf Len  |  Buf Data |

	tTmp  = ring_buf_rd(&s_sEventRingBuf, pui8Type, sizeof(uint8_t));
	tLen += tTmp;
	if (sizeof(uint8_t) != tTmp)
	{
		return (int)tLen;
	}

	tTmp  = ring_buf_rd(&s_sEventRingBuf, &ui8Tmp, sizeof(uint8_t));
	tLen += tTmp;
	if (sizeof(uint8_t) != tTmp)
	{
		return (int)tLen;
	}
	*ptLen = (size_t)ui8Tmp;

	if (tBufSize < *(ptLen))
	{
		tTmp  = ring_buf_rd(&s_sEventRingBuf, pvEventBuf, tBufSize);
		tLen += tTmp;
		ring_buf_drop(&s_sEventRingBuf, *(ptLen)-tBufSize);
	}

	tTmp  = ring_buf_rd(&s_sEventRingBuf, pvEventBuf, *(ptLen));
	tLen += tTmp;

	return (int)tLen;
}
//..............................................................................
/*	Zigbee命令缓存读取函数
;	(功能)从命令缓冲区读出最早的一条命令.
;	(参数)
;		pvCmdBuf:		指向读出命令数据缓存区的指针
;		tBufSize:		缓存区的尺寸
;		ptLen:			事件数据的长度回写指针
;		pui16DevType:	设备类型回写指针
;		pui16DevId:		设备ID回写指针
;	(返回)
;		int:			实际读出的数据的字节数,负数表示出错
;	(注意)
;
*/
static int zigbee_cmd_buf_rd (void *pvCmdBuf, size_t tBufSize, size_t *ptLen, uint16_t *pui16DevType, uint16_t *pui16DevId)
{
	uint8_t ui8Tmp;
	size_t tTmp, tLen = 0;

	ASSERT(NULL != pvCmdBuf);
	ASSERT(0 < tBufSize);
	ASSERT(NULL != ptLen);
	ASSERT(NULL != pui16DevType);
	ASSERT(NULL != pui16DevId);

	// 存入缓存的Cmd数据格式如下
	// |  2 Bytes  | 2 Bytes |  1 Bytes  |  Buf Len  |
	// |  DevType  |  DevID  |  Buf Len  |  Buf Data |

	tTmp  = ring_buf_rd(&s_sAtCmdRingBuf, pui16DevType, sizeof(uint16_t));
	tLen += tTmp;
	if (sizeof(uint16_t) != tTmp)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_buf_rd]ring_buf_rd read DevType failed.\r\n");
		return (int)tLen;
	}

	tTmp  = ring_buf_rd(&s_sAtCmdRingBuf, pui16DevId, sizeof(uint16_t));
	tLen += tTmp;
	if (sizeof(uint16_t) != tTmp)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_buf_rd]ring_buf_rd read DevId failed.\r\n");
		return (int)tLen;
	}

	tTmp  = ring_buf_rd(&s_sAtCmdRingBuf, &ui8Tmp, sizeof(uint8_t));
	tLen += tTmp;
	if (sizeof(uint8_t) != tTmp)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_buf_rd]ring_buf_rd read ui8CmdLen failed.\r\n");
		return (int)tLen;
	}
	*ptLen = (size_t)ui8Tmp;

	if (tBufSize < *(ptLen))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[zigbee_cmd_buf_rd]Buffer not enough, will drop AT command.\r\n");
		ring_buf_drop(&s_sAtCmdRingBuf, *ptLen);
		return ZIGBEE_MANAGE_BUF_ERROR;
	}

	tTmp  = ring_buf_rd(&s_sAtCmdRingBuf, pvCmdBuf, *ptLen);
	tLen += tTmp;

	return (int)tLen;
}
//..............................................................................
/*	Zigbee命令缓存写入函数
;	(功能)向缓冲区写入一条命令数据.
;	(参数)
;		pvCmd:			指向读出事件数据缓存区的指针
;		tCmdLen:		缓存区的尺寸
;		ui16DevType:	设备类型
;		ui16DevId:		设备ID
;	(返回)
;		int:			实际写入的数据的字节数,负数表示出错
;	(注意)
;
*/
static int zigbee_cmd_buf_wr (const void *pvCmd, size_t tCmdLen, uint16_t ui16DevType, uint16_t ui16DevId)
{
	S_BufUnit asBufUnit[4];
	uint8_t ui8Len;
	int iRet;

	ASSERT(pvCmd != NULL);
	ASSERT((0<tCmdLen) && (tCmdLen<=MUI_AT_PORT_CMD_LEN_MAX));

	if (ring_buf_remain(&s_sAtCmdRingBuf) < (sizeof(uint16_t)+sizeof(uint16_t)+sizeof(uint8_t)+tCmdLen))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_buf_wr] ring buf is full!!!!\r\n");
		return -1;
	}

	// 存入缓存的Cmd数据格式如下
	// |  2 Bytes  | 2 Bytes |  1 Bytes  |  Buf Len  |
	// |  DevType  |  DevID  |  Buf Len  |  Buf Data |
	ui8Len = (uint8_t)tCmdLen;
	asBufUnit[0].pvBuf = &ui16DevType;
	asBufUnit[0].tLen  = sizeof(uint16_t);
	asBufUnit[1].pvBuf = &ui16DevId;
	asBufUnit[1].tLen  = sizeof(uint16_t);
	asBufUnit[2].pvBuf = &ui8Len;
	asBufUnit[2].tLen  = sizeof(uint8_t);
	asBufUnit[3].pvBuf = pvCmd;
	asBufUnit[3].tLen  = tCmdLen;

	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_buf_wr] ui16DevType=%u, ui8DevId=%u, tCmdLen=%u\r\n", ui16DevType, ui16DevId, tCmdLen);
	ZIGBEE_MANAGE_DYN_PUTS(M_DBG_LEVEL_DEBUG, (const char*)pvCmd, tCmdLen);
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	iRet = (int)ring_buf_asm_wr(&s_sAtCmdRingBuf, asBufUnit, sizeof(asBufUnit)/sizeof(asBufUnit[0]));
	sem_post(&s_tATCmdEventSem);
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ring buf wr->:%u\r\n", sys_get_ms());

	return iRet;
}
//..............................................................................
/*	Zigbee命令缓存是否为空(查询)函数
;	(功能)检查Zigbee命令缓存是否为空.
;	(参数)
;		NULL
;	(返回)
;		bool:	true:空; false:非空
;	(注意)
;
*/
static bool zigbee_cmd_buf_if_empty (void)
{
	return ring_buf_if_empty(&s_sAtCmdRingBuf);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT端口事件处理任务函数
;	(功能)监听事件数据缓存区,及时处理已收到的事件报文.
;	(参数)
;		pArg:	传给函数运行的参数
;	(返回)
;		NULL
;	(注意)
*/
static void* zigbee_at_cmd_query_task (void *pArg)
{
#if (MUI_AT_PORT_EVENT_LEN_MAX > MUI_AT_PORT_ANSWER_LEN_MAX)
	#define MUI_ZCEDT_TMP_BUF_LEN	MUI_AT_PORT_EVENT_LEN_MAX
#else
	#define MUI_ZCEDT_TMP_BUF_LEN	MUI_AT_PORT_ANSWER_LEN_MAX
#endif
	uint8_t aui8Buf[MUI_ZCEDT_TMP_BUF_LEN+1];	//增加的1个字节用于存储'\0'
	uint8_t aui8CmdBuf[MUI_AT_PORT_CMD_LEN_MAX+1];
	uint16_t ui16DevType, ui16DevId;
	size_t tLen;
	int iRet;

	UNUSED(pArg);

	while(1)
	{
		if (0 == at_port_state_get())
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_at_cmd_query_task]Will reset uZip.\r\n");
			at_port_reset();

			if (0 > sem_wait_ms(&s_tZbModuleReadySem, 30000))	//最多等待30秒
			{
				ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_at_cmd_query_task]Wait Zigbee module ready overtime, errno:%d\r\n", errno);
				continue;
			}

			msleep(1000);

			zigbee_version_verify(NULL, 0);
			zigbee_sn_verify(NULL, 0);
			zigbee_mac_verify(NULL, 0);

			iRet = zigbee_net_setup();
			if (iRet < 0)
			{
				ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_at_cmd_query_task]Setup Zigbee network failed.\r\n");
				at_port_state_set(0);
				continue;
			}
			else
			{
				led_light_ctrl(MUI_GPIO_PIN_ZIGBEE_LED, 1);
			}
		}

		if (0 > sem_wait_ms(&s_tATCmdEventSem, 10000))
	   	{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_at_cmd_query_task]Wait for s_tATCmdEventSem timeouts.\r\n");
			//其它功能
			continue;
	   	}

		tLen = 0;
		iRet = zigbee_cmd_buf_rd(aui8CmdBuf, sizeof(aui8CmdBuf), &tLen, &ui16DevType, &ui16DevId);
		if (iRet == (int)(sizeof(uint16_t)+sizeof(uint16_t)+sizeof(uint8_t)+tLen))
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]Will call 'at_cmd_query'.\r\n");

			iRet = at_cmd_query(aui8CmdBuf, tLen, aui8Buf, sizeof(aui8Buf), MUI_AT_QUERY_WAIT_NORMAL_MAX);
			if(iRet <= 0)
			{
				iRet = deal_dev_at_result(ui16DevType, ui16DevId, -1);	//超时或出错
				if (iRet < 0)
				{
					ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]deal_dev_at_result failed\r\n");
				}

				ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]at_cmd_query fail or no answer!\r\n");
			}
			else
			{
				if (0 != at_cmd_answer_verify(aui8Buf, iRet))
				{
					iRet = deal_dev_at_result(ui16DevType, ui16DevId, -2);	//AT命令执行出错
					if (iRet < 0)
					{
						ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]deal_dev_at_result failed\r\n");
					}

					ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]at_cmd_answer_verify fail or AT answer error!\r\n");
				}
				/*else
				{
					iRet = deal_dev_at_result(ui16DevType, ui8DevId, 0);		//AT命令执行OK
					if (iRet < 0)
					{
						ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]deal_dev_at_result failed\r\n");
					}
				}*/
			}
		}
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	AT端口事件处理任务函数
;	(功能)监听事件数据缓存区,及时处理已收到的事件报文.
;	(参数)
;		pArg:	传给函数运行的参数
;	(返回)
;		NULL
;	(注意)
*/
static void* zigbee_cmd_event_deal_task (void *pArg)
{
#if (MUI_AT_PORT_EVENT_LEN_MAX > MUI_AT_PORT_ANSWER_LEN_MAX)
	#define MUI_ZCEDT_TMP_BUF_LEN	MUI_AT_PORT_EVENT_LEN_MAX
#else
	#define MUI_ZCEDT_TMP_BUF_LEN	MUI_AT_PORT_ANSWER_LEN_MAX
#endif
	uint8_t aui8Buf[MUI_ZCEDT_TMP_BUF_LEN+1];	//增加的1个字节用于存储'\0'
	uint8_t ui8EventType;
	size_t tLen;
	int iRet;

	UNUSED(pArg);

	//释放uZip复位
	//ioport_set_pin_level(uZIP_RST_GPIO, uZIP_RST_ACTIVE_LEVEL);

	while (1)
	{
	   	if (0 > sem_wait_ms(&s_tATEventSem, 10000))
	   	{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]Wait for s_tATEventSem timeouts.\r\n");
			//其它功能
			/*
			x_modem  模式处理
			*/
			continue;
	   	}

		/*
		if (x_modem)
		{
		}
		*/

		tLen = 0;
		iRet = zigbee_event_buf_rd(aui8Buf, sizeof(aui8Buf), &tLen, &ui8EventType);
		if (iRet == (int)(sizeof(uint8_t)+sizeof(uint8_t)+tLen))
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]Will deal zigbee event.\r\n");
			//取消x_modem
			aui8Buf[tLen] = '\0';	//转换成字符串
			switch (ui8EventType)
			{
				case MUI_AT_EVENT_TYPE_READY:
					zigbee_module_ready();
					break;
				case MUI_AT_EVENT_TYPE_NEWDEVU:
					zigbee_new_dev_deal(aui8Buf, tLen);
					break;
				case MUI_AT_EVENT_TYPE_REPORTU:
					zigbee_report_deal(aui8Buf, tLen);
					break;
				case MUI_AT_EVENT_TYPE_ZEREQ:
					zigbee_zereq_deal(aui8Buf, tLen);
					break;
				case MUI_AT_EVENT_TYPE_ZSCHN:
					zigbee_zschn_deal(aui8Buf, tLen);
					break;
				case MUI_AT_EVENT_TYPE_DEVLEFT:
					zigbee_dev_left_deal(aui8Buf, tLen);
					break;
				case MUI_AT_EVENT_TYPE_IMGREQ:
					zigbee_dev_upgrade(aui8Buf, tLen);
					break;
				case MUI_AT_EVENT_TYPE_IMGRDY:
					zigbee_dev_upgrade_result(aui8Buf, tLen);
					break;
				case MUI_AT_EVENT_TYPE_DBG:
					zigbee_debug_info_deal(aui8Buf, tLen);
					break;
				case MUI_AT_EVENT_TYPE_UZIPUPGRADE:
					zigbee_uzip_upgrade(aui8Buf, tLen);
				default:
					break;
			}

			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_cmd_event_deal_task]Have dealt zigbee event.\r\n");
		}
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
static int zigbee_get_dev_type (const S_ZbAttrNwkAddr *psNwkAddr, uint8_t ui8MaxEpNum, S_ZbAttrEp *psZbEp, uint8_t *pui8EpCnt, U_DevExtInfo *puExtInfo)
{
	char achAnswerAcep[MUI_AT_ANSWER_LEN_ACEP];
	char achAnswer[MUI_AT_ANSWER_LEN_SIMPLEDESC];
	int iRet, iLen, i;
	const char *pchAcep, *pch=NULL;
	uint8_t ui8AllCnt, ui8EpCnt,ui8ClustIdCnt;
	uint16_t ui16ProfileId;
	uint16_t ui16TypeId;
	uint16_t ui16Tmp1, ui16Tmp2;

	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]Will check type of this device.\r\n");

	iLen = zigbee_print_query_repeat(achAnswerAcep, sizeof(achAnswerAcep), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ACEP_S"=%s\r\n", psNwkAddr->str);
	if (iLen <= 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]$ACEP fail.\r\n");
		return iLen;
	}
	iRet = at_cmd_answer_verify(achAnswerAcep, iLen);
	if (iRet < 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]$ACEP not answer OK.\r\n");
		return iRet;
	}
	pchAcep = strnstr(achAnswerAcep, MSTR_AT_HEADER_ACEP, iLen);
	if (NULL == pchAcep)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]Can not find $ACEP:\r\n");
		return AT_PORT_ERR_UNEXPECT_ANSWER;
	}
	if (1 != sscanf(pchAcep, MSTR_AT_HEADER_ACEP"%02hhx", &ui8AllCnt))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]Can not get count of end point.\r\n");
		return AT_PORT_ERR_UNEXPECT_ANSWER;
	}

	ui8EpCnt = 0;
	for (i=0; i<(int)ui8AllCnt; i++)
	{
		memcpy(psZbEp[ui8EpCnt].str, pchAcep+sizeof(MSTR_AT_HEADER_ACEP)-1+3+(i*3), 2);
		psZbEp[ui8EpCnt].str[2] = '\0';

		iLen = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
										 AT_CMD_SIMPDESC_S"=%s,%s\r\n", psNwkAddr->str, psZbEp[ui8EpCnt].str);
		if (iLen <= 0)
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]"AT_CMD_SIMPDESC_S" command error.\r\n");
			continue;
		}
		iRet = at_cmd_answer_verify(achAnswer, iLen);
		if (iRet < 0)
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]"AT_CMD_SIMPDESC_S" not answer OK.\r\n");
			continue;
		}
		pch = strnstr(achAnswer, MSTR_AT_HEADER_SIMPDESC, iLen);
		if (NULL == pch)
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]"AT_CMD_SIMPDESC_S" answer can not find "MSTR_AT_HEADER_SIMPDESC".\r\n");
			continue;
		}
		if (2 != sscanf(pch, MSTR_AT_HEADER_SIMPDESC"%*2s,%04hx,%04hx", &ui16ProfileId, &ui16TypeId))
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]"AT_CMD_SIMPDESC_S" answer parameter error.\r\n");
			continue;
		}

		if ((ui16ProfileId!=SUPPORT_PROFILE_ID_1) && (ui16ProfileId!=SUPPORT_PROFILE_ID_2))
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]Profile ID error.\r\n");
			continue;
		}
		if (0x0850 == ui16TypeId)
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]Type ID error.\r\n");
			continue;
		}

		//----------------------------------------
		//Just for intercel bulb
		/*
		2014/8/1 11:52 It would be comment because Jason tell us this bug has been deleted.
		if (SHADE_DEVICE_ID == ui16TypeId)
		{
			if (0 == memcmp(pch+sizeof(MSTR_AT_HEADER_SIMPDESC)-1+16, "07", 2))
			{
				if (0 == memcmp(pch+sizeof(MSTR_AT_HEADER_SIMPDESC)-1+49, "0300", 4))
				{
					ui16ProfileId = SUPPORT_PROFILE_ID_2;
				}
			}
		}
		*/
		//----------------------------------------

		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]Have got one end point:.\r\n");

		ui8EpCnt++;
		if (ui8EpCnt > ui8MaxEpNum)
		{
			return ZIGBEE_MANAGE_EP_NUM_OVERFLOW;
		}
	}

	if (0 == ui8EpCnt)
	{
		return ZIGBEE_MANAGE_UNRECGN_DEC;
	}
	*pui8EpCnt = ui8EpCnt;

	if ((COLOR_LIGHT_DEVICE_ID==ui16TypeId && SUPPORT_PROFILE_ID_2==ui16ProfileId) ||
		(EXCOLOR_LIGHT_DEVICE_ID==ui16TypeId) ||
		(COLOR_DIMM_LIGHT_DEVICE_ID==ui16TypeId) ||
		(COLOR_TEMP_DEVICE_ID==ui16TypeId))
	{
		if (COLOR_TEMP_DEVICE_ID == ui16TypeId)
		{
			puExtInfo->light_ext_info.colormode = MUI_LIGHT_COLOR_MODE_CT;
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]:This is a CT light.\r\n");
		}
		else
		{
			puExtInfo->light_ext_info.colormode = MUI_LIGHT_COLOR_MODE_XY;
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]:This is a extend color light.\r\n");
		}
		//puExtInfo->light_ext_info.colormode = MUI_LIGHT_COLOR_MODE_CT;
		return MUI_DATA_TYPE_LIGHT;
	}
	else if (OCCUP_SENSOR_DEVICE_ID == ui16TypeId)
	{
		return MUI_DATA_TYPE_OCC_SENSOR;
	}
	else if ((SIMPLE_SENSOR_DEVICE_ID==ui16TypeId) || (RULE_ZONE_DEVICE_ID==ui16TypeId))
	{
		return MUI_DATA_TYPE_BIN_SENSOR;
	}
	else if (THERMOSTAT_DEVICE_ID == ui16TypeId)
	{
		return MUI_DATA_TYPE_TH_SENSOR;
	}
	else if (LEVEL_CTRL_OUTPUT_DEVICE_ID == ui16TypeId)
	{
		return MUI_DATA_TYPE_LEVEL_CTRL;
	}
	else if (RULE_WARN_DEVICE_ID == ui16TypeId)
	{
		return MUI_DATA_TYPE_WARN_DEV;
	}
	else if ((ON_OFF_PLUG_DEVICE_ID==ui16TypeId) ||
			 (DIMM_PLUG_DEVICE_ID==ui16TypeId) ||
			 (SMART_PLUG_DEVICE_ID==ui16TypeId) ||
			 (MA_POWER_DEVICE_ID==ui16TypeId))
	{
		return MUI_DATA_TYPE_PLUG;
	}
	else if ((ON_OFF_SWITCH_DEVICE_ID==ui16TypeId) ||
			 (LEVEL_CTRL_SWITCH_DEVICE_ID==ui16TypeId) ||
			 (ON_OFF_LIGHT_DEVICE_ID==ui16TypeId) ||
			 (DIMM_LIGHT_DEVICE_ID==ui16TypeId) ||
			 (DIMM_SWITCH_DEVICE_ID==ui16TypeId) ||
			 (COLOR_DIMM_SWITCH_DEVICE_ID==ui16TypeId))
	{
        if (DIMM_LIGHT_DEVICE_ID == ui16TypeId)
		{
            if ((3!=sscanf(pch, MSTR_AT_HEADER_SIMPDESC"%*15s,%02hhx,%*9s,%04hx,%04hx", &ui8ClustIdCnt, &ui16Tmp1, &ui16Tmp2)) ||
                ((0x06!=ui8ClustIdCnt)||(ON_OFF_CLUSTER_ID!=ui16Tmp1)||(LEVEL_CLUSTER_ID!=ui16Tmp2)))
            {
                ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]Can't recognize type of switch.\r\n");
				puExtInfo->light_ext_info.colormode = MUI_SWITCH_CTRL_MODE_ON_OFF;
			}
			else
			{
                ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]CtrlMode=MUI_SWITCH_CTRL_MODE_MUTI.\r\n");
                puExtInfo->light_ext_info.colormode = MUI_SWITCH_CTRL_MODE_MUTI;
			}
		}
		else
		{
            ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_get_dev_type]CtrlMode=MUI_SWITCH_CTRL_MODE_ON_OFF.\r\n");
            puExtInfo->light_ext_info.colormode = MUI_SWITCH_CTRL_MODE_ON_OFF;
		}

		return MUI_DATA_TYPE_SWITCH;
	}
	else if ((SHADE_DEVICE_ID==ui16TypeId) && (SUPPORT_PROFILE_ID_1==ui16ProfileId))
	{
		return MUI_DATA_TYPE_SHADE;
	}
	else if (ON_OFF_LIGHT_SWITCH_DEVICE_ID==ui16TypeId)
	{
		return MUI_DATA_TYPE_PANEL;
	}
	else
	{
		return ZIGBEE_MANAGE_UNRECGN_DEC;
	}
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	初始化Zigbee模块的函数
;	(功能)  初始化Zigbee模块
;	(参数)
;		pchDev:		AT命令设备
;	(返回)
;		int:		 <0:出错(包括异常及等待超时)
;					>=0:正常完成
;	(注意)
*/
int zigbee_manager_init (const char *pchDev)
{
	int iRet;

	memset(&s_aui8EventBuf, 0, sizeof(s_aui8EventBuf));
	if (true != ring_buf_init(&s_sEventRingBuf, &s_aui8EventBuf[0], &s_aui8EventBuf[sizeof(s_aui8EventBuf)-1]))
	{
		return ZIGBEE_MANAGE_ERR_INIT_FAIL;
	}
	memset(&s_aui8AtCmdBuf, 0, sizeof(s_aui8AtCmdBuf));
	if (true != ring_buf_init(&s_sAtCmdRingBuf, &s_aui8AtCmdBuf[0], &s_aui8AtCmdBuf[sizeof(s_aui8AtCmdBuf)-1]))
	{
		return ZIGBEE_MANAGE_ERR_INIT_FAIL;
	}

	iRet = sem_init(&s_tZbModuleReadySem, 0, 0);	//初始化为0
	if (iRet < 0)
	{
		return ZIGBEE_MANAGE_ERR_INIT_FAIL;
	}
	iRet = sem_init(&s_tZbOrgQueryMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return ZIGBEE_MANAGE_ERR_INIT_FAIL;
	}
	iRet = sem_init(&s_tATEventSem, 0, 0);	//初始化为0
	if (iRet < 0)
	{
		return ZIGBEE_MANAGE_ERR_INIT_FAIL;
	}

	iRet = at_cmd_sys_init(pchDev);
	if (iRet < 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_manager_init]Call 'at_cmd_sys_init' fail, errno:%d\r\n", errno);
		return iRet;
	}

	iRet = pthread_create(&s_sZigbeeCmdEventDealThread, NULL, zigbee_cmd_event_deal_task, NULL);
	if (iRet < 0)
	{
		return ZIGBEE_MANAGE_ERR_INIT_FAIL;
	}
	iRet = pthread_create(&s_sZigbeeAtCmdQueryThread, NULL, zigbee_at_cmd_query_task, NULL);
	if (iRet < 0)
	{
		return ZIGBEE_MANAGE_ERR_INIT_FAIL;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------*

/*	初始化Zigbee模块信号量的函数
;	(功能)  初始化Zigbee模块信号量
;	(参数)
;	(返回)
;
;	(注意)
*/
void zigbee_module_ready (void)
{
	at_port_state_set(1);
	sem_post(&s_tZbModuleReadySem);
}
//-------------------------------------------------------------------------------------------------*
/*	Zigbee(命令)查询函数
;	(功能)
;	(参数)
;		pui8Buf:		缓存(实际报文从&pui8Buf[MUI_AT_PORT_EVENT_HEAD_LEN]开始)
;		tLen:			报文大小(从&pui8Buf[MUI_AT_PORT_EVENT_HEAD_LEN]开始的报文长度)
;		ui8EventType:	事件类型
;	(返回)
;		int:			<0:不是Event
;						MUI_AT_EVENT_TYPE_READY					0
;						MUI_AT_EVENT_TYPE_NEWDEVU				1
;						MUI_AT_EVENT_TYPE_REPORTU				2
;						MUI_AT_EVENT_TYPE_ZEREQ					3
;						MUI_AT_EVENT_TYPE_ZSCHN					4
;						MUI_AT_EVENT_TYPE_DEVLEFT				5
;						MUI_AT_EVENT_TYPE_DBG					6
;						MUI_AT_EVENT_TYPE_TESTMODE				7
;						>=0xFF:不需要处理的事件
;	(注意)
*/
int zigbee_event_checkout (uint8_t *pui8Buf, size_t tLen, uint8_t ui8EventType)
{
	S_BufUnit asBufUnit[3];
	uint8_t ui8Len;
	int iRet;

	ASSERT(NULL != pui8Buf);
	ASSERT(tLen <= MUI_AT_PORT_ISR_BUF_SIZE);

	if (ring_buf_remain(&s_sEventRingBuf) < (MUI_AT_PORT_EVENT_HEAD_LEN+tLen))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[zigbee_event_checkout] EventRingBuf is full!!!!\r\n");
		return -1;
	}

	// 存入缓存的Event数据格式如下
	// |  1 Bytes   |  1 Bytes  |  Buf Len  |
	// | Event Type |  Buf Len  |  Buf Data |
	ui8Len = (uint8_t)tLen;
	asBufUnit[0].pvBuf = &ui8EventType;
	asBufUnit[0].tLen  = sizeof(uint8_t);
	asBufUnit[1].pvBuf = &ui8Len;
	asBufUnit[1].tLen  = sizeof(uint8_t);
	asBufUnit[2].pvBuf = pui8Buf;
	asBufUnit[2].tLen  = tLen;

	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_event_checkout] ui8EventType=%u, tLen=%u\r\n", ui8EventType, tLen);
	ZIGBEE_MANAGE_DYN_PUTS(M_DBG_LEVEL_DEBUG, (const char*)pui8Buf, tLen);
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	iRet = ring_buf_asm_wr(&s_sEventRingBuf, asBufUnit, sizeof(asBufUnit)/sizeof(asBufUnit[0]));
	sem_post(&s_tATEventSem);

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Zigbee(命令)查询函数
;	(功能)
;	(参数)
;		pvCmd:		AT命令字符串
;		tCmdLen:	AT命令长度
;		pvBuf:		应答缓存
;		tBufSize:	应答缓存的大小,即读取数据的最大尺寸
;		ui32WaitMs:	等待的超时毫秒数
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:没有回应
;					>0:回应数据的长度
;	(注意)
*/
int zigbee_query (const void *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs)
{
	int iRet;

	ASSERT(pvCmd != NULL);
	ASSERT(pvBuf != NULL);
	ASSERT((0<tCmdLen) && (tCmdLen<=MUI_AT_PORT_CMD_LEN_MAX));
	ASSERT(tBufSize > 0);
	ASSERT(ui32WaitMs <= MUI_AT_QUERY_WAIT_MAX);

	if (0 > sem_wait_ms(&s_tZbOrgQueryMutex, MUI_AT_QUERY_WAIT_NORMAL_MAX+1))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_query] Take semaphore 's_tZbOrgQueryMutex' filed!\r\n");
		return AT_PORT_ERR_SEM_FAIL;
	}
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_query] Have got semaphore 's_tZbOrgQueryMutex'\r\n");

	iRet = zigbee_origin_query(pvCmd, tCmdLen, pvBuf, tBufSize, ui32WaitMs);

	sem_post(&s_tZbOrgQueryMutex);
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_query] Have released semaphore 's_tZbOrgQueryMutex'\r\n");

	return iRet;
}
//..............................................................................
/*	Zigbee(命令)原始重复查询函数
;	(功能)
;	(参数)
;		pvCmd:		AT命令字符串
;		pvBuf:		应答缓存
;		tCmdLen:	AT命令长度
;		tBufSize:	应答缓存的大小,即读取数据的最大尺寸
;		ui32WaitMs:	等待的超时毫秒数
;		ui32Repeat:	如果查询不成功的情况下,重复查询的次数
;		ui32DlyMs:	重复查询之间的延时(只收不是超时的情况下,才需要延时)
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:没有回应
;					>0:回应数据的长度
;	(注意)
*/
int zigbee_query_repeat (const char *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs,
						 uint32_t ui32Repeat, uint32_t ui32DlyMs)
{
	int iRet;

	ASSERT(pvCmd != NULL);
	ASSERT((0<tCmdLen) && (tCmdLen<=MUI_AT_PORT_CMD_LEN_MAX));
	ASSERT(pvBuf != NULL);
	ASSERT(tBufSize > 0);
	ASSERT(ui32WaitMs <= MUI_AT_QUERY_WAIT_MAX);

	if (0 > sem_wait_ms(&s_tZbOrgQueryMutex, MUI_AT_QUERY_WAIT_NORMAL_MAX+1))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_query_repeat] Take semaphore 's_tZbOrgQueryMutex' filed!\r\n");
		return AT_PORT_ERR_SEM_FAIL;
	}
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_query_repeat] Have got semaphore 's_tZbOrgQueryMutex'\r\n");

	iRet = zigbee_origin_query_repeat(pvCmd, tCmdLen, pvBuf, tBufSize, ui32WaitMs, ui32Repeat, ui32DlyMs);

	sem_post(&s_tZbOrgQueryMutex);
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_query_repeat] Have released semaphore 's_tZbOrgQueryMutex'\r\n");

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Zigbee(命令)带打印功能的查询函数
;	(功能)
;	(参数)
;		pvBuf:		应答缓存
;		tBufSize:	应答缓存的大小,即读取数据的最大尺寸
;		ui32WaitMs:	等待的超时毫秒数
;		pchFmt:		打印命令的格式
;		...:		打印命令的参数
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:没有回应
;					>0:回应数据的长度
;	(注意)
*/
int zigbee_print_query (void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs, const char *pchFmt, ...)
{
	va_list	Args;
	int iRet;

	ASSERT(pvBuf != NULL);
	ASSERT(tBufSize > 0);
	ASSERT(ui32WaitMs <= MUI_AT_QUERY_WAIT_MAX);
	ASSERT(pchFmt != 0);

	if (0 > sem_wait_ms(&s_tZbOrgQueryMutex, MUI_AT_QUERY_WAIT_NORMAL_MAX+1))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_print_query] Take semaphore 's_tZbOrgQueryMutex' filed!\r\n");
		return AT_PORT_ERR_SEM_FAIL;
	}
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_print_query] Have got semaphore 's_tZbOrgQueryMutex'\r\n");

	va_start(Args, pchFmt);
	iRet = vsnprintf(s_achPrintCmdBuf, sizeof(s_achPrintCmdBuf), pchFmt, Args);
	if (iRet > 0)
	{
		iRet = zigbee_origin_query(s_achPrintCmdBuf, iRet, pvBuf, tBufSize, ui32WaitMs);
	}
	else
	{
		iRet = ZIGBEE_MANAGE_PRINT_BUF_ERROR;
	}
	va_end(Args);

	sem_post(&s_tZbOrgQueryMutex);
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_print_query] Have released semaphore 's_tZbOrgQueryMutex'\r\n");

	return iRet;
}
//..............................................................................
/*	Zigbee(命令)带打印功能的查询函数
;	(功能)
;	(参数)
;		pvBuf:		应答缓存
;		tBufSize:	应答缓存的大小,即读取数据的最大尺寸
;		ui32WaitMs:	等待的超时毫秒数
;		pchFmt:		打印命令的格式
;		...:		打印命令的参数
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:没有回应
;					>0:回应数据的长度
;	(注意)
*/
int zigbee_print_query_repeat (void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs, uint32_t ui32Repeat, uint32_t ui32DlyMs,
							   const char *pchFmt, ...)
{
	va_list	Args;
	int iRet;

	ASSERT(pvBuf != NULL);
	ASSERT(tBufSize > 0);
	ASSERT(ui32WaitMs <= MUI_AT_QUERY_WAIT_MAX);
	ASSERT(pchFmt != 0);

	if (0 > sem_wait_ms(&s_tZbOrgQueryMutex, MUI_AT_QUERY_WAIT_NORMAL_MAX+1))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_print_query_repeat] Take semaphore 's_tZbOrgQueryMutex' filed!\r\n");
		return AT_PORT_ERR_SEM_FAIL;
	}
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_print_query_repeat] Have got semaphore 's_tZbOrgQueryMutex'\r\n");

	va_start(Args, pchFmt);
	iRet = vsnprintf(s_achPrintCmdBuf, sizeof(s_achPrintCmdBuf), pchFmt, Args);
	if (iRet > 0)
	{
		iRet = zigbee_origin_query_repeat(s_achPrintCmdBuf, iRet, pvBuf, tBufSize, ui32WaitMs, ui32Repeat, ui32DlyMs);
	}
	else
	{
		iRet = ZIGBEE_MANAGE_PRINT_BUF_ERROR;
	}
	va_end(Args);

	sem_post(&s_tZbOrgQueryMutex);
	//ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_print_query_repeat] Have released semaphore 's_tZbOrgQueryMutex'\r\n");

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Zigbee(命令)缓存写入函数
;	(功能)
;	(参数)
;		ui16Type:	设备类型
;		ui16Id:		设备ID
;		pvCmd:		AT命令字符串
;		tCmdLen:	AT命令长度
;	(返回)
;		int:		<0:出错(写入数据出错)
;					>0:回应数据的长度
;	(注意)
*/
int zigbee_queue_query(uint16_t ui16Type, uint16_t ui16Id, const void *pvCmd, size_t tCmdLen)
{
	ASSERT(pvCmd != NULL);
	ASSERT((0<tCmdLen) && (tCmdLen<=MUI_AT_PORT_CMD_LEN_MAX));
	return zigbee_cmd_buf_wr(pvCmd, tCmdLen, ui16Type, ui16Id);
}
//..............................................................................
/*	Zigbee(命令)带打印功能的缓存写入函数
;	(功能)
;	(参数)
;		ui16Type:	设备类型
;		ui16Id:		设备ID
;		pchFmt:		打印命令的格式
;		...:		打印命令的参数
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:没有回应
;					>0:回应数据的长度
;	(注意)
*/
int zigbee_print_queue_query (const uint16_t ui16Type, const uint16_t ui16Id, const char *pchFmt, ...)
{
	va_list	Args;
	int iRet;
	ASSERT(pchFmt != 0);
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_print_queue_query]ui16Type=%u, ui16Id=%u\r\n", ui16Type, ui16Id);
	if (0 > sem_wait_ms(&s_tZbOrgQueryMutex, MUI_AT_QUERY_WAIT_NORMAL_MAX+1))
	{
		return AT_PORT_ERR_SEM_FAIL;
	}
	va_start(Args, pchFmt);
	iRet = vsnprintf(s_achPrintCmdBuf, sizeof(s_achPrintCmdBuf), pchFmt, Args);
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_print_queue_query]AT command:%s\r\n", s_achPrintCmdBuf);
	if (iRet > 0)
	{
		iRet = zigbee_queue_query(ui16Type, ui16Id, s_achPrintCmdBuf, iRet);
		msleep(50);
	}
	else
	{
		iRet = ZIGBEE_MANAGE_PRINT_BUF_ERROR;
	}
	va_end(Args);

	sem_post(&s_tZbOrgQueryMutex);

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
int zigbee_join_continue(void)
{
	if (sJoinDevCtrl.mode == 0)
		return -1;

	uint32_t ui32Tmp;
	ui32Tmp = sys_get_ms();
	if ((ui32Tmp - sJoinDevCtrl.ms_ticks) > ((MUI_ADD_DEV_DURATION_SEC+10) * 1000))
	{
		sJoinDevCtrl.mode = 0;
		return -1;
	}
	else if ((ui32Tmp - sJoinDevCtrl.ms_ticks) > ((MUI_ADD_DEV_DURATION_SEC + 1) * 1000))
	{
		if (0 > zigbee_join_dev_startup(sJoinDevCtrl.duration))
		{
			zigbee_join_dev_startup(sJoinDevCtrl.duration);
		}
		sJoinDevCtrl.ms_ticks = ui32Tmp;
	}
	return sJoinDevCtrl.duration;
}

int zigbee_join_time (uint8_t ui8Mode, uint16_t ui16Duration)
{
	int iRet;

	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_join_time]sJoinDevCtrl.duration=%02x\r\n", sJoinDevCtrl.duration);
	if ((0xfe<=ui16Duration) && (ui16Duration<0xffff))
	{
		ui16Duration = 0xfe;
	}
	else if (0xffff == ui16Duration)
	{
		ui16Duration = 0xff;
	}

	sJoinDevCtrl.mode = ui8Mode;
	if (0 == ui8Mode)
	{
		sJoinDevCtrl.duration = ui16Duration;
	}
	else
	{
		sJoinDevCtrl.duration = MUI_ADD_DEV_DURATION_SEC; //In mode 1, the join time is 30 seconds.
		sJoinDevCtrl.ms_ticks = sys_get_ms();
	}
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_join_time] will start, sJoinDevCtrl.duration=%02x\r\n", sJoinDevCtrl.duration);
	iRet = zigbee_join_dev_startup(sJoinDevCtrl.duration);
	if (0 > iRet)
	{
		iRet = zigbee_join_dev_startup(sJoinDevCtrl.duration);
	}

	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_join_time]Mode=%d, Duration=%d\r\n", sJoinDevCtrl.mode, sJoinDevCtrl.duration);

	return iRet;
}

//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
int zigbee_dev_del(const S_AttrModelId *psModelId)
{
	char achAnswer[MUI_AT_ANSWER_LEN_LVREQ];
	int iRet;

	ASSERT(NULL != psModelId);
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_del]&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\r\n");
	//Delete Zigbee device
 	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX,
							  AT_CMD_LVREQ_S"=%s\r\n", psModelId->str);
	if (iRet <= 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_del]$LVREQ fail.\r\n");
		return iRet;
	}
	iRet = at_cmd_answer_verify(achAnswer, iRet);
	if (iRet < 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_del]$LVREQ not answer OK.\r\n");
		return iRet;
	}

	return iRet;
}

int zigbee_new_dev_deal (const void *pvEvent, size_t tLen)
{
	S_AttrModelId sModelId;
	S_ZbAttrNwkAddr sNwkAddr;
	S_ZbAttrEp asEpList[MUI_AT_DEV_EP_NUM_MAX];
	S_DevIdx asDevIdx[MUI_AT_DEV_EP_NUM_MAX];	//!!!Chandler:此数组应该取决于系统一个MAC地址所关联设备的最大数,目前以switch设备为最多,可关联4个
	U_DevExtInfo uDevExtInfo;
	uint16_t ui16DevType, uiIdx;
	uint8_t ui8Cnt = 0;
	char *pch;
	int iRet;

	//Example Event:
	//$NEWDEVU:04FE,001BC50340000F01
	pch = strnstr((const char*)pvEvent, MSTR_AT_HEADER_NEWDEVU, tLen);
	if (NULL == pch)
	{
		return ZIGBEE_MANAGE_EVENT_ERROR;
	}
	strncpy(sNwkAddr.str, pch+(sizeof(MSTR_AT_HEADER_NEWDEVU)-1), MUI_DEV_ZB_NWK_ADDR_LEN);
	sNwkAddr.str[MUI_DEV_ZB_NWK_ADDR_LEN] = '\0';
	strncpy(sModelId.str, pch+(sizeof(MSTR_AT_HEADER_NEWDEVU)-1)+MUI_DEV_ZB_NWK_ADDR_LEN+1, MUI_DEV_ATTR_MODEL_ID_LEN_MAX);
	sModelId.str[MUI_DEV_ATTR_MODEL_ID_LEN_MAX] = '\0';

	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]This device information - %s:%s.\r\n", sModelId.str, sNwkAddr.str);

	iRet = update_devs_nwkaddr_by_type_modelid(&sModelId, &sNwkAddr, asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));
	if (iRet > 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]Have found %d devices with same MAC address.\r\n", iRet);
		msleep(1000);
		ui8Cnt = (uint8_t)iRet;
		for (uiIdx=0; uiIdx<ui8Cnt; uiIdx++)
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]Will rebind device - %d:%d\r\n", asDevIdx[uiIdx].ui16Type, asDevIdx[uiIdx].ui16Id);
			iRet = rebind_dev(asDevIdx[uiIdx].ui16Type, asDevIdx[uiIdx].ui16Id);	//Rebind this device.
			if (iRet < 0)
			{
				ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]Can't rebind this device - %d:%d\r\n", asDevIdx[uiIdx].ui16Type, asDevIdx[uiIdx].ui16Id);
			}
		}

		//绑定设备之后,再检查冲突短地址
		check_confilct_zb_nwkaddr(&sModelId, &sNwkAddr, asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));

		return (int)ui8Cnt;
	}
	else
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]Will add this device.\r\n");
		msleep(1000);
		iRet = zigbee_get_dev_type(&sNwkAddr, MUI_AT_DEV_EP_NUM_MAX, asEpList, &ui8Cnt, &uDevExtInfo);
		if ((iRet<0) || (ui8Cnt==0))
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]Call zigbee_get_dev_type() error!\r\n");
			zigbee_dev_del(&sModelId);
		}
		else
		{
			//加设备之前,先检查冲突短地址
			check_confilct_zb_nwkaddr(&sModelId, &sNwkAddr, asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));

			ui16DevType = (uint16_t)iRet;

			for (uiIdx=0; uiIdx<ui8Cnt; uiIdx++)
			{
				if (MUI_DATA_TYPE_TH_SENSOR == ui16DevType)
				{
					iRet = add_dev(MUI_DATA_TYPE_TEMP_SENSOR, &sModelId, &sNwkAddr, &asEpList[uiIdx], &uDevExtInfo);
					if (iRet < 0)
					{
						ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]Call add_dev() error: %d!\r\n", iRet);
						zigbee_dev_del(&sModelId);
						return iRet;
					}

					iRet = add_dev(MUI_DATA_TYPE_HUM_SENSOR, &sModelId, &sNwkAddr, &asEpList[uiIdx], &uDevExtInfo);
					if (iRet < 0)
					{
						ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]Call add_dev() error: %d!\r\n", iRet);
						zigbee_dev_del(&sModelId);
						return iRet;
					}
				}
				else
				{
					iRet = add_dev(ui16DevType, &sModelId, &sNwkAddr, &asEpList[uiIdx], &uDevExtInfo);
					if (iRet < 0)
					{
						ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_new_dev_deal]Call add_dev() error: %d!\r\n", iRet);
						zigbee_dev_del(&sModelId);
						return iRet;
					}
				}
			}
		}

		return iRet;
	}
}

int zigbee_report_deal (const void *pvEvent, size_t tLen)
{
	S_ZbAttrNwkAddr sNwkAddr;
	S_ZbAttrEp sEp;
	uint16_t ui16ClusterId, ui16Val;
	uint8_t ui8Val;
	const char *pch;
	int iId;

	ASSERT(NULL != pvEvent);
	ASSERT(tLen <= MUI_AT_PORT_EVENT_LEN_MAX);

	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Received REPORTU.\r\n");
	ZIGBEE_MANAGE_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, pvEvent, tLen);
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	//Example Event:
	//$REPORTU:0003,0A,0001,0021,64
	//$REPORTU:0003,0A,0006,0000,00
	//$REPORTU:0016,01,0008,0000,01
	//$REPORTU:0010,0D,000F,0000,00
	//$REPORTU:0011,11,0402,0000,09BB
	//$REPORTU:0011,11,0405,0000,142F
	//$REPORTU:0010,0D,0406,0000,00
	pch = strnstr((const char*)pvEvent, MSTR_AT_HEADER_REPORTU, tLen);
	if (NULL == pch)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Can not find header of REPORTU.\r\n");
		return -1;
	}
	if (3 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%4s,%2s,%04hx", sNwkAddr.str, sEp.str, &ui16ClusterId))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Format of REPORTU wrong.\r\n");
		return -1;
	}
	sNwkAddr.str[MUI_DEV_ZB_NWK_ADDR_LEN] = '\0';
	sEp.str[MUI_DEV_ZB_EP_LEN] = '\0';
	switch (ui16ClusterId)
	{
	case POWER_CONFIG_CLUSTER_ID:	//All sensors
		//$REPORTU:0003,0A,0001,0021,64
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Power of all sensors.\r\n");
		if (0 < (iId=search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_OCC_SENSOR, &sNwkAddr, &sEp)))
		{
			if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
			{
				return AT_PORT_ERR_UNEXPECT_ANSWER;
			}
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]occ_sensor:power-ID=%d, Value=%d.\r\n", iId, ui8Val);
			return update_occ_sensor_state_power((uint8_t)iId, &ui8Val);
		}
		else if (0 < (iId=search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_BIN_SENSOR, &sNwkAddr, &sEp)))
		{
			if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
			{
				return AT_PORT_ERR_UNEXPECT_ANSWER;
			}
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]bin_sensor:power-ID=%d, Value=%d.\r\n", iId, ui8Val);
			return update_bin_sensor_state_power((uint8_t)iId, &ui8Val);
		}
		else if (0 < (iId=search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_TEMP_SENSOR, &sNwkAddr, &sEp)))
		{
			if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
			{
				return AT_PORT_ERR_UNEXPECT_ANSWER;
			}
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]temp_sensor:power-ID=%d, Value=%d.\r\n", iId, ui8Val);
			return update_temp_sensor_state_power((uint8_t)iId, &ui8Val);
		}
		else if (0 < (iId=search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_HUM_SENSOR, &sNwkAddr, &sEp)))
		{
			if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
			{
				return AT_PORT_ERR_UNEXPECT_ANSWER;
			}
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]hum_sensor:power-ID=%d, Value=%d.\r\n", iId, ui8Val);
			return update_hum_sensor_state_power((uint8_t)iId, &ui8Val);
		}

		return AT_PORT_ERR_UNKNOW_EVENT;
	case ON_OFF_CLUSTER_ID:	//Light, Plug
		//$REPORTU:0003,0A,0006,0000,00
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Light or Plug or Switch.\r\n");
		if (0 < (iId=search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_LIGHT, &sNwkAddr, &sEp)))
		{
			if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
			{
				return AT_PORT_ERR_UNEXPECT_ANSWER;
			}
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]light:on-ID=%d, Value=%d.\r\n", iId, ui8Val);
			return update_light_state_on((uint8_t)iId, &ui8Val);
		}
		else if (0 < (iId=search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_PLUG, &sNwkAddr, &sEp)))
		{
			if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
			{
				return AT_PORT_ERR_UNEXPECT_ANSWER;
			}
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]plug:on-ID=%d, Value=%d.\r\n", iId, ui8Val);
			return update_plug_state_on((uint8_t)iId, &ui8Val);
		}
		else if (0 < (iId=search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_SWITCH, &sNwkAddr, &sEp)))
		{
			if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
			{
				return AT_PORT_ERR_UNEXPECT_ANSWER;
			}
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]switch:on-ID=%d, Value=%d.\r\n", iId, ui8Val);
			return update_switch_state_on((uint8_t)iId, &ui8Val);
		}

		return AT_PORT_ERR_UNKNOW_EVENT;
	case LEVEL_CLUSTER_ID:
		//$REPORTU:0016,01,0008,0000,01
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Switch.\r\n");
		if (0 < (iId=search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_SWITCH, &sNwkAddr, &sEp)))
		{
			if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hx", &ui16Val))
			{
				return AT_PORT_ERR_UNEXPECT_ANSWER;
			}
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]switch:level-ID=%d, Value=%d.\r\n", iId, ui16Val);
			return update_switch_state_level((uint8_t)iId, &ui16Val);
		}

		return AT_PORT_ERR_UNKNOW_EVENT;
	case OCC_SENSOR_CLUSTER_ID:
		//$REPORTU:0010,0D,0406,0000,00
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Occupancy Sensor.\r\n");
		iId = search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_OCC_SENSOR, &sNwkAddr, &sEp);
		if (iId < 0)
		{
			return iId;
		}
		if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Can not get value.\r\n");
			return AT_PORT_ERR_UNEXPECT_ANSWER;
		}
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]occ_sensor:on-ID=%d, Value=%d.\r\n", iId, ui8Val);
		return update_occ_sensor_state_on(iId, &ui8Val);
	case BIN_SENSOR_CLUSTER_ID:
		//$REPORTU:0010,0D,000F,0000,00
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Binary Sensor.\r\n");
		iId = search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_BIN_SENSOR, &sNwkAddr, &sEp);
		if (iId < 0)
		{
			return iId;
		}
		if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%02hhx", &ui8Val))
		{
			return AT_PORT_ERR_UNEXPECT_ANSWER;
		}
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]bin_sensor:on-ID=%d, Value=%d.\r\n", iId, ui8Val);
		return update_bin_sensor_state_on((uint8_t)iId, &ui8Val);
	case TEMP_SENSOR_CLUSTER_ID:
		//$REPORTU:0011,11,0402,0000,09BB
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Temperature Sensor.\r\n");
		iId = search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_TEMP_SENSOR, &sNwkAddr, &sEp);
		if (iId < 0)
		{
			return iId;
		}
		if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%04hx", &ui16Val))
		{
			return AT_PORT_ERR_UNEXPECT_ANSWER;
		}
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]temp_sensor:temp-ID=%d, Value=%d.\r\n", iId, ui16Val);
		return update_temp_sensor_state_temp((uint8_t)iId, &ui16Val);
	case HUM_SENSOR_CLUSTER_ID:
		//$REPORTU:0011,11,0405,0000,142F
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Humudity Sensor.\r\n");
		iId = search_dev_by_type_nwkaddr_ep(MUI_DATA_TYPE_HUM_SENSOR, &sNwkAddr, &sEp);
		if (iId < 0)
		{
			return iId;
		}
		if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_REPORTU"%*17s,%04hx", &ui16Val))
		{
			return AT_PORT_ERR_UNEXPECT_ANSWER;
		}
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]hum_sensor:hum-ID=%d, Value=%d.\r\n", iId, ui16Val);
		return update_hum_sensor_state_hum((uint8_t)iId, &ui16Val);
	default:
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_report_deal]Unknow event.\r\n");
		return AT_PORT_ERR_UNKNOW_EVENT;
	}
}

int zigbee_zereq_deal (const void *pvEvent, size_t tLen)
{
#if 1
	UNUSED(pvEvent);
	UNUSED(tLen);

	return -1;
#else
	char achAnswer[MUI_AT_ANSWER_LEN_ADDGR];
	S_AttrModelId sModelId;
	S_ZbAttrNwkAddr sNwkAddr;
	U_DevState uDevState;
	S_ZbAttrEp asEpList[MUI_AT_DEV_EP_NUM_MAX];
	uint16_t ui16DevType, ui8EpCnt, uiIdx;
	char *pch;
	int iRet;

	//
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ZERESP_S"=%s,01,00,01\r\n", sNwkAddr.str);

	return iRet;
#endif
}

int zigbee_zschn_deal (const void *pvEvent, size_t tLen)
{
	S_ZbAttrNwkAddr sNwkAddr;
	S_ZbAttrEp sEp;
	uint16_t ui16Val, ui16DevType;
	uint8_t ui8Val;
	const char *pch;
	int iRet, iId;

	ASSERT(NULL != pvEvent);
	ASSERT(tLen <= MUI_AT_PORT_EVENT_LEN_MAX);


	pch = strnstr((const char*)pvEvent, MSTR_AT_HEADER_ZSCHN, tLen);
	if (NULL == pch)
	{
		return AT_PORT_ERR_FORMAT_ERR;
	}
	if (2 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_ZSCHN"%4s,%2s", sNwkAddr.str, sEp.str))
	{
		return AT_PORT_ERR_FORMAT_ERR;
	}
	sNwkAddr.str[MUI_DEV_ZB_NWK_ADDR_LEN] = '\0';

	iId = search_dev_by_nwkaddr_ep(&sNwkAddr, &sEp, &ui16DevType);
	if (iId < 0)
	{
		return iId;
	}

	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_OCC_SENSOR:
		iRet = read_occ_sensor_state_count(iId, &ui16Val);
		if (iRet < 0)
		{
			return iRet;
		}
		ui16Val += 2;	//Netvox 的占位传感器只会发占位事件,不会有状态的变化,因此每次事件就当作两次状态的变化,OFF->ON->OFF
		iRet = set_occ_sensor_state_count(iId, &ui16Val);
		if (iRet < 0)
		{
			return iRet;
		}
		ui8Val = 0;
		return set_occ_sensor_state_on(iId, &ui8Val);
	case MUI_DATA_TYPE_BIN_SENSOR:
		//Event Example:
		//$ZSCHN:0009,01,6430,01
		if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_ZSCHN"%*4s,%*2s,%*3s%1hhx", &ui8Val))
		{
			return AT_PORT_ERR_FORMAT_ERR;
		}
		return set_bin_sensor_state_on(iId, &ui8Val);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		//Event Example:
		//$ZSCHN:0009,01,6430,01
		if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_ZSCHN"%*4s,%*2s,%04hx", &ui16Val))
		{
			return AT_PORT_ERR_FORMAT_ERR;
		}
		return set_temp_sensor_state_temp(iId, &ui16Val);
	case MUI_DATA_TYPE_HUM_SENSOR:
		//Event Example:
		//$ZSCHN:0009,01,6430,01
		if (1 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_ZSCHN"%*4s,%*2s,%04hx", &ui16Val))
		{
			return AT_PORT_ERR_FORMAT_ERR;
		}
		return set_hum_sensor_state_hum(iId, &ui16Val);
	default:
		return AT_PORT_ERR_UNKNOW_EVENT;
	}
}

int zigbee_dev_left_deal (const void *pvEvent, size_t tLen)
{
#if 1
	UNUSED(pvEvent);
	UNUSED(tLen);

	return -1;
#else
	char achAnswer[MUI_AT_ANSWER_LEN_ADDGR];
	S_AttrModelId sModelId;
	S_ZbAttrNwkAddr sNwkAddr;
	U_DevState uDevState;
	S_ZbAttrEp asEpList[MUI_AT_DEV_EP_NUM_MAX];
	uint16_t ui16DevType, ui8EpCnt, uiIdx;
	char *pch;
	int iRet;

	//
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ZERESP_S"=%s,01,00,01\r\n", sNwkAddr.str);

	return iRet;
#endif
}

int zigbee_debug_info_deal (const void *pvEvent, size_t tLen)
{
	UNUSED(pvEvent);
	UNUSED(tLen);

	return -1;
}

static int zigbee_dev_upgrade(const void *pvEvent, size_t tLen)
{
	int i,iRet = 0;
	uint8_t num_of_byte;
	uint16_t image_type;
	uint32_t file_offset;
	char data[130] = "";
	char cdatabuf[130] = "";
	char achAnswer[100] = "";
	char upgradefilepath[50]="";


	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_upgrade]*************[%s]\r\n",(const char*)pvEvent);

	if (3 != sscanf((const char*)pvEvent, "$IMGREQ:%04hx,%08x,%02hhx", &image_type, &file_offset,&num_of_byte))
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"1111111111111111111111111111111111");
		return AT_PORT_ERR_FORMAT_ERR;

	}
	//判断类型

	// recv uzip '$IMGREQ',then  answer uzip 'AT$IMGACK'
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_upgrade][%04hx  %08x  %02hhx]\r\n",image_type,file_offset,num_of_byte);
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), 10000, "AT$IMGACK=%04hx,%02hhx\r\n",image_type,num_of_byte);
	if (iRet <= 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_upgrade]AT$IMGACK fail.\r\n");
		return iRet;
	}
	iRet = at_cmd_answer_verify(achAnswer, iRet);
	if (iRet < 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_upgrade]AT$IMGACK not answer OK.\r\n");
		return iRet;
	}

	// read device upgrade file  and  send to uzip
	read_Device_Upgrade_filepath(upgradefilepath);
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_upgrade][upgradefilepath  = %s].\r\n",upgradefilepath);
	//find_upgrade_file_by_upgrade_device_type(image_type,upgradefilepath);默认读取固定文件，不需要用户设置.
	iRet = file_exclusive_read(upgradefilepath,file_offset,cdatabuf,num_of_byte,NULL,0);
	if (iRet < 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_dev_upgrade]read device upgrade file fail.\r\n");
		return iRet;
	}
	data[0] = 0xAA;
	data[1] = num_of_byte;
	memcpy(data+2,cdatabuf,num_of_byte);
	for (i=0; i<iRet+2; i++)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%02hhx ", data[i]);
		if (0 == ((i+1)%16))
		{
			ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
		}
	}
	//iRet = at_port_block_wr(data,sizeof(data), 10000);

	iRet = zigbee_origin_query (data,sizeof(data), achAnswer, 100,10000);
	if (iRet <= 0)
	{
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"222222222222222222222222222222");
		ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[Uzip_AT_updata_deal]send device upgrade data  fail.\r\n");
		return iRet;
	}
	return iRet;
}

static int zigbee_dev_upgrade_result(const void *pvEvent, size_t tLen)
{
	const char *pch;
	int iRet = 0;
	uint16_t state ;
	char addr[9] = "";

	pch = strnstr((const char*)pvEvent, MSTR_AT_HEADER_ZSCHN, tLen);
	if (NULL == pch)
	{
		return AT_PORT_ERR_FORMAT_ERR;
	}
	if (2 != sscanf((const char*)pvEvent, MSTR_AT_HEADER_IMGRDY",%hd,%8s", &state, addr))
	{
		return AT_PORT_ERR_FORMAT_ERR;
	}
	addr[8] = '\0';
	//成功
	return iRet;
}

static int zigbee_uzip_upgrade(const void *pvEvent, size_t tLen)
{
	ASSERT(NULL != pvEvent);

	UNUSED(tLen);

	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[zigbee_uzip_upgrade]state =[");
	ZIGBEE_MANAGE_DYN_PUTASCS(M_DBG_LEVEL_DEBUG, (const char*)pvEvent, 1);
	ZIGBEE_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "]\r\n");

	return uzip_xmodem_upgrade(*(const char*)pvEvent);
}

