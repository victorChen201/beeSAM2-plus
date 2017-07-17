/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of sonos module for beeSAM project.
File name:sonos.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "at_port.h"
#include "sonos_drv.h"
#include "dev_if.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SONOS_DRV_DBG

#ifndef SONOS_DRV_PUTC
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_PUTC(Char)			PUTC(Char)
#else
#define SONOS_DRV_PUTC(Char)
#endif
#endif

#ifndef SONOS_DRV_PUTS
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SONOS_DRV_PUTS(Str, Len)
#endif
#endif

#ifndef SONOS_DRV_PUTASCC
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_PUTASCC(Char)			PUTASCC(Char)
#else
#define SONOS_DRV_PUTASCC(Char)
#endif
#endif

#ifndef SONOS_DRV_PUTASCS
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SONOS_DRV_PUTASCS(Str, Len)
#endif
#endif

#ifndef SONOS_DRV_PRINT
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SONOS_DRV_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SONOS_DRV_DYN_PUTC
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SONOS_DRV|Dbg), Char)
#else
#define SONOS_DRV_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SONOS_DRV_DYN_PUTS
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SONOS_DRV|Dbg), Str, Len)
#else
#define SONOS_DRV_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SONOS_DRV_DYN_PUTASCC
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SONOS_DRV|Dbg), Char)
#else
#define SONOS_DRV_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SONOS_DRV_DYN_PUTASCS
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SONOS_DRV|Dbg), Str, Len)
#else
#define SONOS_DRV_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SONOS_DRV_DYN_PRINT
#ifdef SONOS_DRV_DBG
#define SONOS_DRV_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SONOS_DRV|Dbg), Fmt, ##Arg)
#else
#define SONOS_DRV_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_SONOS_DRV_SSDP_M_SEARCH_INTERVAL	3U
#define MUI_SONOS_DRV_SSDP_PARSE_BUF_SIZE		256
#define MUI_SONOS_DRV_SSDP_SEND_BUF_SIZE		1024
#define MUI_SONOS_DRV_NET_COM_BUF_SIZE			2440
#define MUI_SONOS_DRV_CONNECT_TIME_OUT_MAX   	3
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	S_AttrModelId			sModelId;
	S_ZbAttrNwkAddr			sNwkAddr;
	S_ZbAttrEp				sEp;
} S_SonosInfo;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	uint32_t			track_num;
	uint32_t 			trackduration_time;
	char				track_url[256];
} S_SonosmusicPositioninfo;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static uint16_t s_ui16SonosDrvJoinDuration = 0;
static uint16_t s_ui16BroadCastingInterval = MUI_SONOS_DRV_SSDP_M_SEARCH_INTERVAL;
static sem_t s_tSonosDrvMutex;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int find_and_update_sonoses_eth_attr (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint8_t ui8MaxNum);
static int sonos_drv_new_dev_deal (const S_SonosInfo* psSonosInfo);
static int sonos_drv_answer_verify (const char *pchMsg, size_t tLen, S_SonosInfo* psSonosInfo);
static int sonos_drv_notify_verify (const char *pchMsg, size_t tLen, S_SonosInfo* psSonosInfo);
static int sonos_drv_m_search_send (const S_SocketCfg *psDest);

static int sonos_drv_net_connect (const S_DevZbAttr* psDevNetAttr, S_SocketCfg* psSocketCfg);
static int sonos_drv_net_close (const S_SocketCfg* psSocketCfg);
static int sonos_drv_net_request (const S_SocketCfg* psSocketCfg, const char* pchCmd, size_t tLen, char* pchAnswer, size_t tSize);
static int sonos_drv_send_init_message (const S_SocketCfg* psSocketCfg,char *achMac);
static int sonos_drv_send_play_message (const S_SocketCfg* psSocketCfg, uint8_t ui8Play);
static int sonos_drv_send_url_message (const S_SocketCfg* psSocketCfg, const char* pchUrl, size_t tLen);
static int sonos_drv_send_seek_message (const S_SocketCfg* psSocketCfg, uint32_t ui32Seek);
static int sonos_drv_send_volume_message (const S_SocketCfg* psSocketCfg, uint16_t ui16Volume);
static int sonos_drv_send_clear_list_message (const S_SocketCfg* psSocketCfg);
static int sonos_drv_send_GetPositionInfo_message (const S_SocketCfg* psSocketCfg);
static int sonos_drv_send_remove_url_message (const S_SocketCfg* psSocketCfg,uint32_t ui32removenum);
static int sonos_drv_send_subscribe_MediaServer_ContentDirectory_event_message (const S_SocketCfg* psSocketCfg);
static int sonos_drv_send_subscribe_ZoneGroupTopology_event_message (const S_SocketCfg* psSocketCfg);
static int sonos_drv_send_subscribe_MediaRenderer_AVTransport_event_message (const S_SocketCfg* psSocketCfg);
static int sonos_drv_send_subscribe_MediaRenderer_RenderingControl_event_message (const S_SocketCfg* psSocketCfg);
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*


//return number of devices that have been update.
static int find_and_update_sonoses_eth_attr (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint8_t ui8MaxNum)
{
	uint8_t ui8Idx = 0;
	int iRet;

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[find_and_update_sonoses_eth_attr]Will find device:%s-%s\r\n", psModelId->str, psNwkAddr->ip);

	if (0 < (iRet=update_sonoses_zb_nwkaddr(psModelId, psNwkAddr, &psDevIdx[ui8Idx], ui8MaxNum-ui8Idx)))
	{
		ui8Idx += (uint8_t)iRet;
		if (ui8Idx >= ui8MaxNum)
		{
			return ui8Idx;
		}
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[find_and_update_sonoses_eth_attr]Have found out %u sonoses that need update IP address.\r\n", ui8Idx);

	return ui8Idx;
}
//-------------------------------------------------------------------------------------------------*
/*	Sonos Answer 验证函数
;	(功能)
;	(参数)
;		psSonosInfo:	Sonos信息数据指针
;	(返回)
;		int
;	(注意)
*/
static int sonos_drv_new_dev_deal (const S_SonosInfo* psSonosInfo)
{
	S_DevIdx asDevIdx[MUI_AT_DEV_EP_NUM_MAX];	//!!!Chandler:此数组应该取决于系统一个MAC地址所关联设备的最大数,目前以switch设备为最多,可关联4个
	U_DevExtInfo uDevExtInfo;
	int iRet;

	ASSERT(psSonosInfo != NULL);

	iRet = find_and_update_sonoses_eth_attr(&psSonosInfo->sModelId, &psSonosInfo->sNwkAddr, asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));
	if (iRet > 0)
	{
		return iRet;
	}

	memset(&uDevExtInfo, 0, sizeof(uDevExtInfo));
	iRet = add_dev(MUI_DATA_TYPE_SONOS, &psSonosInfo->sModelId, &psSonosInfo->sNwkAddr, &psSonosInfo->sEp, &uDevExtInfo);
	if (iRet < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_new_dev_deal]Call add_dev() error: %d!\r\n", errno);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos Answer 验证函数
;	(功能)
;	(参数)
;		pchMsg:			校验的报文
;		tLen:			报文长度
;		psSonosInfo:	回传的Sonos设备信息
;	(返回)
;		int:			<0:出错(包括异常及等待超时)
;						 0:不匹配
;						 1:ZonePlayer
;						 2:ZoneDevice1
;						 3:ZoneDevice2
;	(注意)
*/
static int sonos_drv_answer_verify (const char *pchMsg, size_t tLen, S_SonosInfo* psSonosInfo)
{
	char achBuf[MUI_SONOS_DRV_SSDP_PARSE_BUF_SIZE];
	const char *pchCur, *pchTmp;
	int iTmp;

	ASSERT(pchMsg != NULL);
	ASSERT(tLen > 0);
	ASSERT(psSonosInfo != NULL);

	/*
	HTTP/1.1 200 OK
	CACHE-CONTROL: max-age = 1800
	EXT:
	LOCATION: http://192.168.1.3:1400/xml/device_description.xml
	SERVER: Linux UPnP/1.0 Sonos/27.2-80271 (ZPS3)
	ST: urn:schemas-upnp-org:device:ZonePlayer:1
	USN: uuid:RINCON_B8E937326ABC01400::urn:schemas-upnp-org:device:ZonePlayer:1
	X-RINCON-BOOTSEQ: 49
	X-RINCON-HOUSEHOLD: Sonos_fbj1vjlQDEEWxEpapniBuXJtih
	*/

	// ANSWER
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Will verify 'ANSWER'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "HTTP/1.1 200 OK\r\n", tLen)))
	{
		pchCur = pchTmp;
	}
	else
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_answer_verify]: Haven't find 'ANSWER'\r\n");
		return 0;
	}

	// LOCATION
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Will verify 'LOCATION'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nLOCATION", tLen)))
	{
		pchCur = pchTmp+2;
	}
	else if (NULL != (pchTmp=strnstr(pchMsg, "\r\nLocation", tLen)))
	{
		pchCur = pchTmp+2;	//忽略之前的\r\n
	}
	if (NULL == pchCur)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Haven't find 'LOCATION'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_answer_verify]: Call 'sub_str_get_util_rn(LOCATION)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_answer_verify]: SubStr=%s\r\n", achBuf);
	iTmp -= (sizeof("LOCATION: ")-1);
	if (NULL == (pchCur=strnstr(achBuf, "http://", iTmp)))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Haven't find 'http://'.\r\n");
		return 0;
	}
	pchCur += (sizeof("http://")-1);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: SubStr=%s\r\n", pchCur);
	if (5 != sscanf((const char*)pchCur, "%hu.%hu.%hu.%hu:%hu",
					&psSonosInfo->sEp.port, &psSonosInfo->sEp.port, &psSonosInfo->sEp.port, &psSonosInfo->sEp.port,
					&psSonosInfo->sEp.port))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: IP address format wrong.\r\n");
		return 0;
	}
	iTmp -= (sizeof("http://")-1);
	if (NULL == (pchTmp=strnstr(pchCur, ":", iTmp)))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Haven't find ':'.\r\n");
		return 0;
	}
	memset(&psSonosInfo->sNwkAddr.ip, 0, sizeof(psSonosInfo->sNwkAddr.ip));
	memcpy(&psSonosInfo->sNwkAddr.ip, pchCur, pchTmp-pchCur);
	//psSonosInfo->sEp.port
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: IP address: '%s:%u'\r\n", psSonosInfo->sNwkAddr.ip, psSonosInfo->sEp.port);

	// SERVER
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Will verify 'SERVER'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nSERVER", tLen)))
	{
		pchCur = pchTmp+2;
	}
	if (NULL == pchCur)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Haven't find 'SERVER'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Call 'sub_str_get_util_rn(SERVER)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_answer_verify]: SubStr=%s\r\n", achBuf);
	if (NULL == strnstr(achBuf, "Sonos/", iTmp))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Haven't find 'Sonos/'.\r\n");
		return 0;
	}

	// USN
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_answer_verify]: Will verify 'USN'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nUSN", tLen)))
	{
		pchCur = pchTmp+2;
	}
	if (NULL == pchCur)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Haven't find 'USN'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Call 'sub_str_get_util_rn(USN)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: SubStr=%s\r\n", achBuf);
	memcpy(psSonosInfo->sModelId.str, &pchCur[18], MUI_DEV_MAC_ADDR_LEN);
	psSonosInfo->sModelId.str[MUI_DEV_MAC_ADDR_LEN] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: sModelId.str=%s\r\n", psSonosInfo->sModelId.str);

	// ST
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_answer_verify]: Will verify 'ST'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nST", tLen)))
	{
		pchCur = pchTmp+2;
	}
	else if (NULL != (pchTmp=strnstr(pchMsg, "\r\nSt", tLen)))
	{
		pchCur = pchTmp+2;
	}
	if (NULL == pchCur)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Haven't find 'ST'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_answer_verify]: Call 'sub_str_get_util_rn(NT)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_answer_verify]: SubStr=%s\r\n", achBuf);
	if (NULL == (pchTmp=strnstr(achBuf, "urn:schemas-upnp-org:device:", iTmp)))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_verify]: Haven't find 'urn:schemas-upnp-org:device:'\r\n");
		return 0;
	}
	else
	{
		pchTmp += (sizeof("urn:schemas-upnp-org:device:")-1);
		if (NULL != strnstr(pchTmp, "ZonePlayer", tLen-(pchTmp-pchMsg)))
		{
			return 1;
		}
		else if (NULL != strnstr(pchTmp, "ZoneDevice1", tLen-(pchTmp-pchMsg)))
		{
			return 2;
		}
		else if (NULL != strnstr(pchTmp, "ZoneDevice2", tLen-(pchTmp-pchMsg)))
		{
			return 3;
		}

		return 0;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos Notify 验证函数
;	(功能)
;	(参数)
;		pchMsg:			校验的报文
;		tLen:			报文长度
;		psSonosInfo:	回传的Sonos设备信息
;	(返回)
;		int:			<0:出错(包括异常及等待超时)
;						 0:不匹配
;						 1:ConnectionManager
;						 2:RenderingControl
;						 3:GroupRenderingControl
;						 4:AVTransport
;						 5:Queue
;	(注意)
*/
static int sonos_drv_notify_verify (const char *pchMsg, size_t tLen, S_SonosInfo* psSonosInfo)
{
	char achBuf[MUI_SONOS_DRV_SSDP_PARSE_BUF_SIZE];
	const char *pchCur, *pchTmp;
	int iTmp;

	ASSERT(pchMsg != NULL);
	ASSERT(tLen > 0);
	ASSERT(psSonosInfo != NULL);

	/*
	NOTIFY * HTTP/1.1
	HOST: 239.255.255.250:1900
	CACHE-CONTROL: max-age = 1800
	LOCATION: http://192.168.1.120:1400/xml/device_description.xml
	NT: urn:schemas-upnp-org:service:ConnectionManager:1
	NTS: ssdp:alive
	SERVER: Linux UPnP/1.0 Sonos/28.1-83040 (ZPS3)
	USN: uuid:RINCON_B8E937326ABC01400_MS::urn:schemas-upnp-org:service:ConnectionManager:1
	X-RINCON-BOOTSEQ: 124
	X-RINCON-HOUSEHOLD: Sonos_2CBXxVoVoyPJUypXIacOmPHnqk
	*/

	// NOTIFY
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Will verify 'NOTIFY'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "NOTIFY", tLen)))
	{
		pchCur = pchTmp;
	}
	else
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find 'NOTIFY'\r\n");
		return 0;
	}

	// LOCATION
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_notify_verify]: Will verify 'LOCATION'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nLOCATION", tLen)))
	{
		pchCur = pchTmp+2;
	}
	else if (NULL != (pchTmp=strnstr(pchMsg, "\r\nLocation", tLen)))
	{
		pchCur = pchTmp+2;	//忽略之前的\r\n
	}
	if (NULL == pchCur)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find 'LOCATION'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Call 'sub_str_get_util_rn(LOCATION)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: SubStr=%s\r\n", achBuf);
	iTmp -= (sizeof("LOCATION: ")-1);
	if (NULL == (pchCur=strnstr(achBuf, "http://", iTmp)))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find 'http://'.\r\n");
		return 0;
	}
	pchCur += (sizeof("http://")-1);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: SubStr=%s\r\n", pchCur);
	if (5 != sscanf((const char*)pchCur, "%hu.%hu.%hu.%hu:%hu",
					&psSonosInfo->sEp.port, &psSonosInfo->sEp.port, &psSonosInfo->sEp.port, &psSonosInfo->sEp.port,
					&psSonosInfo->sEp.port))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: IP address format wrong.\r\n");
		return 0;
	}
	iTmp -= (sizeof("http://")-1);
	if (NULL == (pchTmp=strnstr(pchCur, ":", iTmp)))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find ':'.\r\n");
		return 0;
	}
	memset(&psSonosInfo->sNwkAddr.ip, 0, sizeof(psSonosInfo->sNwkAddr.ip));
	memcpy(&psSonosInfo->sNwkAddr.ip, pchCur, pchTmp-pchCur);
	//psSonosInfo->sEp.port
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: IP address: '%s:%u'\r\n", psSonosInfo->sNwkAddr.ip, psSonosInfo->sEp.port);

	// SERVER
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Will verify 'SERVER'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nSERVER", tLen)))
	{
		pchCur = pchTmp+2;
	}
	if (NULL == pchCur)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find 'SERVER'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Call 'sub_str_get_util_rn(SERVER)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_notify_verify]: SubStr=%s\r\n", achBuf);
	if (NULL == strnstr(achBuf, "Sonos/", iTmp))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find 'Sonos/'.\r\n");
		return 0;
	}

	// USN
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Will verify 'USN'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nUSN", tLen)))
	{
		pchCur = pchTmp+2;
	}
	if (NULL == pchCur)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find 'USN'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_notify_verify]: Call 'sub_str_get_util_rn(USN)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: SubStr=%s\r\n", achBuf);
	memcpy(psSonosInfo->sModelId.str, &pchCur[18], MUI_DEV_MAC_ADDR_LEN);
	psSonosInfo->sModelId.str[MUI_DEV_MAC_ADDR_LEN] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: sModelId.str=%s\r\n", psSonosInfo->sModelId.str);

	// NT
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Will verify 'NT'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nNT", tLen)))
	{
		pchCur = pchTmp+2;
	}
	else if (NULL != (pchTmp=strnstr(pchMsg, "\r\nNt", tLen)))
	{
		pchCur = pchTmp+2;
	}
	if (NULL == pchCur)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find 'NT'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_notify_verify]: Call 'sub_str_get_util_rn(NT)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[sonos_drv_notify_verify]: SubStr=%s\r\n", achBuf);
	if (NULL == (pchTmp=strnstr(achBuf, "urn:schemas-upnp-org:service:", iTmp)))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_notify_verify]: Haven't find 'urn:schemas-upnp-org:service:'\r\n");
		return 0;
	}
	else
	{
		pchTmp += (sizeof("urn:schemas-upnp-org:service:")-1);
		if (NULL != strnstr(pchTmp, "ConnectionManager", tLen-(pchTmp-pchMsg)))
		{
			return 1;
		}
		else if (NULL != strnstr(pchTmp, "RenderingControl", tLen-(pchTmp-pchMsg)))
		{
			return 2;
		}
		else if (NULL != strnstr(pchTmp, "GroupRenderingControl", tLen-(pchTmp-pchMsg)))
		{
			return 3;
		}
		else if (NULL != strnstr(pchTmp, "AVTransport", tLen-(pchTmp-pchMsg)))
		{
			return 4;
		}
		else if (NULL != strnstr(pchTmp, "Queue", tLen-(pchTmp-pchMsg)))
		{
			return 5;
		}

		return 0;
	}


	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	SSDP M-Search 发送函数
;	(功能)
;	(参数)
;		psDest:	要发送信息的Socket配置结构信息
;		eType:	通知类型
;	(返回)
;		int:	-1:出错(包括异常及等待超时)
;				 0:正常完成
;	(注意)
*/
static int sonos_drv_m_search_send (const S_SocketCfg *psDest)
{
	char achBuf[MUI_SONOS_DRV_SSDP_SEND_BUF_SIZE];
	int iIdx;

	ASSERT(psDest != NULL);

	iIdx = 0;
	// M-SEARCH
	iIdx += sprintf(&achBuf[iIdx], "M-SEARCH * HTTP/1.1\r\n");
	iIdx += sprintf(&achBuf[iIdx], "HOST: %s:%d\r\n", MSTR_SSDP_MULTICAST_IP, MUI_SSDP_MULTICAST_PORT);
	iIdx += sprintf(&achBuf[iIdx], "MAN: \"ssdp:discover\"\r\n");
	iIdx += sprintf(&achBuf[iIdx], "MX: 1\r\n");
	iIdx += sprintf(&achBuf[iIdx], "ST: urn:schemas-upnp-org:device:ZonePlayer:1\r\n");
	//iIdx += sprintf(&achBuf[iIdx], "X-RINCON-HOUSEHOLD: Sonos_u1b8HyxytYmDtFETUDctJZhNtj\r\n");
	//iIdx += sprintf(&achBuf[iIdx], "X-RINCON-HOUSEHOLD: N/A\r\n");
	iIdx += sprintf(&achBuf[iIdx], "\r\n");

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_m_search_send]: #########################################\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_m_search_send]: Will send HTTP message to IP-'%s', Port-'%d':\r\n", inet_ntoa(psDest->sSocketAdd.sin_addr), ntohs(psDest->sSocketAdd.sin_port));
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_m_search_send]: #########################################\r\n");

	// Start send message
	return sendto(psDest->iSocketFd, achBuf, iIdx, 0, (struct sockaddr *)&psDest->sSocketAdd, psDest->tSockLen);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos M-Search 函数
;	(功能)进行Sonos M-Search广播.
;	(参数)
;		psSocketCfg:	SSDP socket参数指针
;	(返回)
;		int
;	(注意)
*/
int sonos_drv_m_search_broadcasting (const S_SocketCfg *psSocketCfg)
{
	static int iCnt = MUI_SONOS_DRV_SSDP_M_SEARCH_INTERVAL;

	if ((0==s_ui16SonosDrvJoinDuration) || (0==s_ui16BroadCastingInterval))
	{
		iCnt = MUI_SONOS_DRV_SSDP_M_SEARCH_INTERVAL;
		return 0;
	}

	if (0 > sem_wait_ms(&s_tSonosDrvMutex, 1000))	//最多等待1秒
	{
		return 0;
	}
	s_ui16SonosDrvJoinDuration = (s_ui16SonosDrvJoinDuration>=MUI_SSDP_SELECT_WAIT_S) ? (s_ui16SonosDrvJoinDuration-MUI_SSDP_SELECT_WAIT_S) : 0;
	sem_post(&s_tSonosDrvMutex);

	if (iCnt < ((s_ui16BroadCastingInterval-1)/MUI_SSDP_SELECT_WAIT_S))
	{
		iCnt++;
		return 0;
	}

	iCnt = 0;

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_m_search_broadcasting]: Will broadcasting Sonos's M-Search.\r\n");

	return sonos_drv_m_search_send(psSocketCfg);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos Answer 处理函数
;	(功能)进行Sonos Answer分析处理.
;	(参数)
;		psSocketCfg:	SSDP socket参数指针
;		pchMsg:			报文指针
;		tLen:			报文长度
;	(返回)
;		int
;	(注意)
*/
int sonos_drv_answer_deal (const S_SocketCfg *psSocketCfg, const char *pchMsg, size_t tLen)
{
	S_SonosInfo sSonosInfo;
	int iRet = 0;

	UNUSED(psSocketCfg);

	iRet = sonos_drv_answer_verify(pchMsg, tLen, &sSonosInfo);
	if (iRet == 1)
	{
		//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_deal]: Have find a sonos device.\r\n");
		//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_deal]: Sonos's IP-'%s', Port-'%u'.\r\n", sSonosInfo.sNwkAddr.ip, sSonosInfo.sEp.port);
		if (s_ui16SonosDrvJoinDuration > 0)
		{
			iRet = sonos_drv_new_dev_deal(&sSonosInfo);
		}
	}
	else
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_deal]: It is not answer of sonos's device.\r\n");
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos Notify 处理函数
;	(功能)进行Sonos Notify分析处理.
;	(参数)
;		psSocketCfg:	SSDP socket参数指针
;		pchMsg:			报文指针
;		tLen:			报文长度
;	(返回)
;		int
;	(注意)
*/
int sonos_drv_notify_deal (const S_SocketCfg *psSocketCfg, const char *pchMsg, size_t tLen)
{
	S_SonosInfo sSonosInfo;
	int iRet = 0;

	UNUSED(psSocketCfg);

	iRet = sonos_drv_notify_verify(pchMsg, tLen, &sSonosInfo);
	if (iRet == 1)
	{
		//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_deal]: Have find a sonos device.\r\n");
		//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_deal]: Sonos's IP-'%s', Port-'%u'.\r\n", sSonosInfo.sNwkAddr.ip, sSonosInfo.sEp.port);
		if (s_ui16SonosDrvJoinDuration > 0)
		{
			iRet = sonos_drv_new_dev_deal(&sSonosInfo);
		}
	}
	else
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_answer_deal]: It is not answer of sonos's device.\r\n");
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos 加入(网络)函数
;	(功能)启动Sonos加网处理.
;	(参数)
;		ui16Duration:	加网时间(从下发参数到加网结束的秒数,0表示立即结束)
;	(返回)
;		int				返回实际的延时时间(加入设置失败,则返回当前剩余时间)
;	(注意)
*/
int sonos_drv_join (uint16_t ui16Duration)
{
	if (0 > sem_wait_ms(&s_tSonosDrvMutex, 1000))	//最多等待1秒
	{
		return s_ui16SonosDrvJoinDuration;
	}

	SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_join]: Set Sonos join duration to %u.\r\n", ui16Duration);
	s_ui16SonosDrvJoinDuration = ui16Duration;

	sem_post(&s_tSonosDrvMutex);

	return ui16Duration;
}

//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos socket连接函数
;	(功能)Sonos socket connect.
;	(参数)
;		S_DevZbAttr: IP PORT ,S_SocketCfg:socketfd
;	(返回)
;		int
;	(注意)
*/

static int sonos_drv_net_connect (const S_DevZbAttr* psDevNetAttr, S_SocketCfg* psSocketCfg)
{
	int iRet,iMaxFd;
	struct timeval sTime;
	fd_set sRdFdSet;
	int len=sizeof(int);
	int error;
	ASSERT(NULL != psDevNetAttr);
	ASSERT(NULL != psSocketCfg);

	//Creat socket for TCP
	memset(psSocketCfg, 0, sizeof(S_SocketCfg));
	psSocketCfg->iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (psSocketCfg->iSocketFd < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]: Can not creat socket for sonos, Error:%d\r\n", errno);
		return psSocketCfg->iSocketFd;
	}
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"ssdp_notify_task: Creat socket successful!\r\n");


	fcntl(psSocketCfg->iSocketFd,F_SETFL, O_NONBLOCK);

	//Set sSocketCfg
	psSocketCfg->sSocketAdd.sin_family	    = AF_INET;
	psSocketCfg->sSocketAdd.sin_addr.s_addr = inet_addr(psDevNetAttr->nwkaddr.ip);
	psSocketCfg->sSocketAdd.sin_port		= htons(psDevNetAttr->ep.port);
	psSocketCfg->tSockLen					= sizeof(struct sockaddr_in);
	iRet = connect(psSocketCfg->iSocketFd, (struct sockaddr *)(&psSocketCfg->sSocketAdd), sizeof(struct sockaddr));
	if(iRet == -1)
	{
		 //SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]:++++++++++++++++[1]+++++++++++++++\r\n");
		// SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]: connect for sonos, Error:%d\r\n", errno);
		//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]:+++++++++++++++++++++++++++++++\r\n");
		 if (errno == EINPROGRESS)
		 {
			sTime.tv_sec  = MUI_SONOS_DRV_CONNECT_TIME_OUT_MAX;
			sTime.tv_usec = 0;
			FD_ZERO(&sRdFdSet);
	        FD_SET(psSocketCfg->iSocketFd, &sRdFdSet);
			iMaxFd = psSocketCfg->iSocketFd;
			if(select(iMaxFd+1,NULL,&sRdFdSet,NULL,&sTime)>0)
			{
				//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]:++++++++++++++[2]+++++++++++++++++%d\r\n",iRet);
               //下面的一句一定要，主要针对防火墙
                getsockopt(psSocketCfg->iSocketFd, SOL_SOCKET, SO_ERROR, &error, &len);
                if(error==0)
                {
					iRet = 1;
					//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]:++++++++++++++[3]+++++++++++++++++%d\r\n",iRet);
                }
                else
                {

					iRet = -1;
					SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]:%d\r\n",iRet);
                }

            }
			else
			{

				iRet = -1;//timeout or error happen
				SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]:timeout or error happen %d\r\n",iRet);
			}
		 }
	}
	else
	{
		iRet = 1;
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_connect]:%d\r\n",iRet);
	}
	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos socket连接的关闭函数
;	(功能)Sonos socket connect close.
;	(参数)
;		S_SocketCfg:socketfd
;	(返回)
;		int
;	(注意)
*/
static int sonos_drv_net_close (const S_SocketCfg* psSocketCfg)
{
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_INFO,"[sonos_drv_net_close]\r\n");
	return close(psSocketCfg->iSocketFd);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos socket连接的关闭函数
;	(功能)Sonos socket connect close.
;	(参数)
;		S_SocketCfg:socketfd
;	(返回)
;		int
;	(注意)
*/
static int sonos_drv_net_request (const S_SocketCfg* psSocketCfg, const char* pchCmd, size_t tLen, char* pchAnswer, size_t tSize)
{
	struct timeval sTime;
	fd_set sRdFdSet;
	int iMaxFd, iRet;

	ASSERT(NULL != psSocketCfg);
	ASSERT(NULL != pchCmd);
	ASSERT(tLen > 0);
	ASSERT(NULL != pchAnswer);
	ASSERT(tSize > 0);

	iRet = send(psSocketCfg->iSocketFd, pchCmd , tLen, 0);
	if (iRet < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_request]: Call 'send' error: %d\r\n", errno);
		return iRet;
	}

	//Timeout setting
	sTime.tv_sec  = 3;
	sTime.tv_usec = 0;

	//Initialize read file descriptor set
	FD_ZERO(&sRdFdSet);
	FD_SET(psSocketCfg->iSocketFd, &sRdFdSet);
	iMaxFd = psSocketCfg->iSocketFd;

	iRet = select(iMaxFd+1, &sRdFdSet, NULL, NULL, &sTime);
	if (iRet <= 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_request]: Call 'select' error: %d\r\n", errno);
		return -1;
	}
	if (!FD_ISSET(psSocketCfg->iSocketFd, &sRdFdSet))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_net_request]: Have not received answer from sonos\r\n");
		return -1;
	}

	iRet = recv(psSocketCfg->iSocketFd, pchAnswer, tSize, 0);
	if ((iRet<=0) || ((size_t)iRet>=tSize))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_request]: iRet = %d\r\n", iRet);
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_ERR,"[sonos_drv_net_request]: Call 'read' error: %d\r\n", errno);
		return -1;
	}

	pchAnswer[iRet] = '\0';
	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos socket连接的关闭函数
;	(功能)Sonos socket connect close.
;	(参数)
;		S_SocketCfg:socketfd
;	(返回)
;		int
;	(注意)
*/
//-------------------------------------------------------------------------------------------------*
static int sonos_drv_send_device_description_message (const S_DevZbAttr* psDevNetAttr,char *achMac)
{
	char achBuf1[10240];
	int iIdx, iRet;
	char *pchTmp;
	char achtmp[48],achtmp1[48];

	S_SocketCfg sSocketCfg;
	//listion gena notify
	do
	{
	iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
	if (iRet < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_device_description_message]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
		return iRet;
	}
	iIdx = 0;
	iIdx += sprintf(&achBuf1[iIdx], "GET /xml/device_description.xml HTTP/1.1\r\n"
				        	       "CONNECTION: close\r\n"
				            	   "ACCEPT: */*\r\n"
				            	   "ACCEPT-ENCODING: gzip\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Sonos\r\n"
								   "\r\n",
								   psDevNetAttr->nwkaddr.ip,psDevNetAttr->ep.port);

	iRet = sonos_drv_net_request(&sSocketCfg, achBuf1, iIdx, achBuf1, sizeof(achBuf1));
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);
	if (iRet < 0)
	{
		return iRet;
	}
	pchTmp = strnstr(achBuf1, "<UDN>", iRet);
	if (NULL == pchTmp)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_device_description_message] Can not find '</serialNum>'.\r\n");
		return -1;
	}
	if (1!=sscanf(pchTmp+sizeof("<UDN>uuid:")-1, "%[^<]",achtmp))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_device_description_message]\r\n");
		return -1;
	}
	if (2!=sscanf(achtmp, "%[^_]%[_a-zA-Z0-9]",achtmp1,achMac))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_device_description_message]Can not find MAC \r\n");
		return -1;
	}
	SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_device_description_message]  MAC [%s]\r\n",achMac);



	iRet = sonos_drv_net_close(&sSocketCfg);
	if (iRet < 0)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_device_description_message]: There something wrong when close connect with sonos. Error: %d.\r\n", errno);
		return iRet;
	}
	}while(0);


 	return iRet;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	Sonos socket无状态的初始化
;	(功能)sonos_drv_send_init_message
;	(参数)
;		S_SocketCfg:socketfd
;	(返回)
;		int
;	(注意)
*/
//-------------------------------------------------------------------------------------------------*
static int sonos_drv_send_init_message (const S_SocketCfg* psSocketCfg,char *achMac)
{
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE], achBody[1024];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	int iIdx, iRet, iBodyLen;

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_init_message]  MAC [%s]\r\n",achMac);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}
	/*iBodyLen = sprintf(achBody, "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetAVTransportURI xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><CurrentURI>x-rincon-queue:RINCON_B"
							"%s#%u</CurrentURI><CurrentURIMetaData></CurrentURIMetaData></u:SetAVTransportURI></s:Body></s:Envelope>",
							"8E9373C8C4201400", 0);//8E937326ABC01400 //8E9373C8C4201400*/
	iBodyLen = sprintf(achBody, "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetAVTransportURI xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><CurrentURI>x-rincon-queue:RINCON"
								"%s#%u</CurrentURI><CurrentURIMetaData></CurrentURIMetaData></u:SetAVTransportURI></s:Body></s:Envelope>",
								achMac,0);//8E937326ABC01400(yongai) //8E9373C8C4201400
	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n"
				        	       "CONNECTION: close\r\n"
				            	   "ACCEPT-ENCODING: gzip\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %u\r\n"
								   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
					    		   "SOAPACTION: \"urn:schemas-upnp-org:service:AVTransport:1#SetAVTransportURI\"\r\n"
					     		   "\r\n%s",
								   iBodyLen, achBody);

	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_init_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;
}

//-------------------------------------------------------------------------------------------------*
/*	Sonos 播放停止控制
;	(功能)Sonos play or pause
;	(参数)
;		S_SocketCfg:socketfd  ui8Play: 0 pause, else play
;	(返回)
;		int
;	(注意)
*/
static int sonos_drv_send_play_message (const S_SocketCfg* psSocketCfg, uint8_t ui8Play)
{
	const char achPlayBody[] = "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><Speed>1</Speed></u:Play></s:Body></s:Envelope>";
	const char achPauseBody[] = "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Pause xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Pause></s:Body></s:Envelope>";
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	int iIdx, iRet;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}

	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n"
								   "CONNECTION: close\r\n"
								   "ACCEPT-ENCODING: gzip\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));
	if (0 == ui8Play)
	{
		iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %u\r\n"
									   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
									   "SOAPACTION: \"urn:schemas-upnp-org:service:AVTransport:1#Pause\"\r\n"
									   "\r\n%s",
									   sizeof(achPauseBody)-1, achPauseBody);
	}
	else
	{
		iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %u\r\n"
									   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
									   "SOAPACTION: \"urn:schemas-upnp-org:service:AVTransport:1#Play\"\r\n"
									   "\r\n%s",
									   sizeof(achPlayBody)-1, achPlayBody);
	}

	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_play_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;
}
//return queue length
static int sonos_drv_send_url_message (const S_SocketCfg* psSocketCfg, const char* pchUrl, size_t tLen)
{
	const char achBodyHead[] = "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:AddURIToQueue xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><EnqueuedURI>";
	const char achBodyTail[] = "</EnqueuedURI><EnqueuedURIMetaData>[truncated]&lt;DIDL-Lite xmlns:dc=&quot;http://purl.org/dc/elements/1.1/&quot; xmlns:upnp=&quot;urn:schemas-upnp-org:metadata-1-0/upnp/&quot; xmlns:r=&quot;urn:schemas-rinconnetworks-com:metadata-1-0/&quot; xmlns=&quot;urn:schemas-upnp-or</EnqueuedURIMetaData><DesiredFirstTrackNumberEnqueued>0</DesiredFirstTrackNumberEnqueued><EnqueueAsNext>1</EnqueueAsNext></u:AddURIToQueue></s:Body></s:Envelope>";
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE], achBody[1024];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	char *pchTmp;
	int iIdx, iRet, iBodyLen;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_url_message] Origin URL: #########################################################\r\n");
	//SONOS_DRV_PUTS(pchUrl, tLen);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_url_message] Origin URL: #########################################################\r\n");

	iBodyLen = 0;
	memcpy(&achBody[iBodyLen], achBodyHead, sizeof(achBodyHead)-1);
	iBodyLen += (sizeof(achBodyHead)-1);
	memcpy(&achBody[iBodyLen], pchUrl, tLen);
	iBodyLen += tLen;
	memcpy(&achBody[iBodyLen], achBodyTail, sizeof(achBodyTail)-1);
	iBodyLen += (sizeof(achBodyTail)-1);
	achBody[iBodyLen] = '\0';

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_url_message] URL: *********************************************************\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBody);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_url_message] URL: *********************************************************\r\n");

	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n"
				                   "CONNECTION: close\r\n"
				                   "ACCEPT-ENCODING: gzip\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %u"
				    		   	   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
				    		   	   "SOAPACTION: \"urn:schemas-upnp-org:service:AVTransport:1#AddURIToQueue\"\r\n"
				    		   	   "\r\n%s",
				    		   	   iBodyLen, achBody);

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_url_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}



	pchTmp = strnstr(achBuf, "<NewQueueLength>", iRet);
	if (NULL == pchTmp)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_url_message] Can not find '<NewQueueLength>'.\r\n");
		return -1;
	}
	if ((1!=sscanf(pchTmp+sizeof("<NewQueueLength>")-1, "%d", &iRet)) || (iRet<0))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_url_message] Can not get queue lenght of sonos.\r\n");
		return -1;
	}

	return iRet;
}

static int sonos_drv_send_seek_message (const S_SocketCfg* psSocketCfg, uint32_t ui32Seek)
{
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE], achBody[1024];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	int iIdx, iRet, iBodyLen;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}

	iBodyLen = sprintf(achBody, "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Seek xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><Unit>TRACK_NR</Unit><Target>"
								"%u</Target></u:Seek></s:Body></s:Envelope>",
								ui32Seek);

	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n"
								   "CONNECTION: close\r\n"
								   "ACCEPT-ENCODING: gzip\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %u\r\n"
								   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
								   "SOAPACTION: \"urn:schemas-upnp-org:service:AVTransport:1#Seek\"\r\n"
								   "\r\n%s",
								   iBodyLen, achBody);

	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_url_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;
}

static int sonos_drv_send_volume_message (const S_SocketCfg* psSocketCfg, uint16_t ui16Volume)
{
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE], achBody[1024];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	int iIdx, iRet, iBodyLen;

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_volume_message]****************************\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"ui16Volume=%u\r\n", ui16Volume);

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}

	if(ui16Volume > MUI_SONOS_STATE_VOLUME_MAX)
	{
		ui16Volume = MUI_SONOS_STATE_VOLUME_MAX;
	}

	iBodyLen = sprintf(achBody, "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetVolume xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel><DesiredVolume>"
								"%u</DesiredVolume></u:SetVolume></s:Body></s:Envelope>",
								ui16Volume);

	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n"
				               	   "CONNECTION: close\r\n"
				               	   "ACCEPT-ENCODING: gzip\r\n"
				               	   "HOST: %s:%d\r\n"
				               	   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
				               	   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %d\r\n"
								   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
								   "SOAPACTION: \"urn:schemas-upnp-org:service:RenderingControl:1#SetVolume\"\r\n"
								   "\r\n%s",
								   iBodyLen, achBody);


	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_volume_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;
}

static int sonos_drv_send_clear_list_message (const S_SocketCfg* psSocketCfg)
{
	const char achClearListBody[] = "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:RemoveAllTracks xmlns:u=\"urn:schemas-sonos-com:service:Queue:1\"><QueueID>0</QueueID><UpdateID>0</UpdateID></u:RemoveAllTracks></s:Body></s:Envelope>";
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	int iIdx, iRet;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}

	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "POST /MediaRenderer/Queue/Control HTTP/1.1\r\n"
				      	 		   "CONNECTION: close\r\n"
				      	 		   "ACCEPT-ENCODING: gzip\r\n"
				      	 		   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %u\r\n"
								   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
								   "SOAPACTION: \"urn:schemas-sonos-com:service:Queue:1#RemoveAllTracks\"\r\n"
								   "\r\n%s",
								   sizeof(achClearListBody)-1, achClearListBody);


	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_clear_list_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;

}

static int sonos_drv_send_GetPositionInfo_message (const S_SocketCfg* psSocketCfg)
{
	const char achClearListBody[] = "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:GetPositionInfo xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:GetPositionInfo></s:Body></s:Envelope>";
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	char *pchTmp;
	int sec,min,hur;
	int iIdx, iRet;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}


	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n"
				      	 		   "CONNECTION: close\r\n"
				      	 		   "ACCEPT-ENCODING: gzip\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %u\r\n"
								   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
								   "SOAPACTION: \"urn:schemas-upnp-org:service:AVTransport:1#GetPositionInfo\"\r\n"
								   "\r\n%s",
								   sizeof(achClearListBody)-1, achClearListBody);


	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_GetPositionInfo_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

	pchTmp = strnstr(achBuf, "<Track>", iRet);
	if (NULL == pchTmp)
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_GetPositionInfo_message] Can not find '<Track>'.\r\n");
		return -1;
	}

	if ((4!=sscanf(pchTmp+sizeof("<Track>")-1, "%d</Track><TrackDuration>%d:%d:%d", &iRet,&hur,&min,&sec)) || (iRet<0))
	{
		SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_send_GetPositionInfo_message] Can not get queue lenght of sonos.\r\n");
		return -1;
	}
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_GetPositionInfo_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[%d]-[%d]:[%d]:[%d]\r\n",iRet,hur,min,sec);
 	return iRet;
}


static int sonos_drv_send_remove_url_message (const S_SocketCfg* psSocketCfg,uint32_t ui32removenum)
{
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE],achBody[1024];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	int iIdx, iRet,iBodyLen;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}

	iBodyLen = sprintf(achBody, "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:RemoveTrackRangeFromQueue xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><UpdateID>0</UpdateID><StartingIndex>"
								"%d</StartingIndex><NumberOfTracks>1</NumberOfTracks></u:RemoveTrackRangeFromQueue></s:Body></s:Envelope>",
								ui32removenum);

	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n"
				      	 		   "CONNECTION: close\r\n"
				      	 		   "ACCEPT-ENCODING: gzip\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CONTENT-LENGTH: %u\r\n"
								   "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
								   "SOAPACTION: \"urn:schemas-upnp-org:service:AVTransport:1#RemoveTrackRangeFromQueue\"\r\n"
								   "\r\n%s",
								   iBodyLen, achBody);


	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_remove_url_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;

}

static int sonos_drv_send_subscribe_MediaServer_ContentDirectory_event_message (const S_SocketCfg* psSocketCfg)
{
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1],*ipaddr;
	int iIdx, iRet;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}
	ipaddr=net_listen_ip_addr_get ();
	SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaServer_ContentDirectory_event_message]%s\r\n", ipaddr);
	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "SUBSCRIBE /MediaServer/ContentDirectory/Event HTTP/1.1\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CALLBACK: <http://%s:3400/notify>\r\n"
								   "NT: upnp:event\r\n"
								   "TIMEOUT: Second-3600\r\n"
								   "\r\n",
								   ipaddr);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaServer_ContentDirectory_event_message]%s\r\n", achBuf);

	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaServer_ContentDirectory_event_message] Received message: \r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;

}

static int sonos_drv_send_subscribe_ZoneGroupTopology_event_message (const S_SocketCfg* psSocketCfg)
{
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	const char *ipaddr;
	int iIdx, iRet;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}
	ipaddr=net_listen_ip_addr_get ();
	SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_ZoneGroupTopology_event_message]%s\r\n", ipaddr);
	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "SUBSCRIBE /ZoneGroupTopology/Event HTTP/1.1\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CALLBACK: <http://%s:3400/notify>\r\n"
								   "NT: upnp:event\r\n"
								   "TIMEOUT: Second-3600\r\n"
								   "\r\n",
								   ipaddr);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_ZoneGroupTopology_event_message]%s\r\n", achBuf);

	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_remove_url_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;

}

static int sonos_drv_send_subscribe_MediaRenderer_AVTransport_event_message (const S_SocketCfg* psSocketCfg)
{
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	const char *ipaddr;
	int iIdx, iRet;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}
	ipaddr=net_listen_ip_addr_get ();
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaRenderer_AVTransport_event_message]%s\r\n", ipaddr);
	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "SUBSCRIBE /MediaRenderer/AVTransport/Event HTTP/1.1\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CALLBACK: <http://%s:3400/notify>\r\n"
								   "NT: upnp:event\r\n"
								   "TIMEOUT: Second-3600\r\n"
								   "\r\n",
								   ipaddr);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaRenderer_AVTransport_event_message]%s\r\n", achBuf);

	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaRenderer_AVTransport_event_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%s\r\n", achBuf);

 	return iRet;

}

static int sonos_drv_send_subscribe_MediaRenderer_RenderingControl_event_message (const S_SocketCfg* psSocketCfg)
{
	char achBuf[MUI_SONOS_DRV_NET_COM_BUF_SIZE];
	char achIPAddr[MUI_STR_IP_ADDR_LEN+1];
	const char *ipaddr;
	int iIdx, iRet;

	// Calculate HOST
	if (NULL == inet_ntop(psSocketCfg->sSocketAdd.sin_family, &psSocketCfg->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}
	ipaddr=net_listen_ip_addr_get ();
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaRenderer_RenderingControl_event_message]%s\r\n", ipaddr);
	iIdx = 0;
	iIdx += sprintf(&achBuf[iIdx], "SUBSCRIBE /MediaRenderer/RenderingControl/Event HTTP/1.1\r\n"
								   "HOST: %s:%d\r\n"
								   "USER-AGENT: Linux UPnP/1.0 Sonos/28.1-83040 (WDCR:Microsoft Windows NT 5.1.2600 Service Pack 3)\r\n",
								   achIPAddr, ntohs(psSocketCfg->sSocketAdd.sin_port));

	iIdx += sprintf(&achBuf[iIdx], "CALLBACK: <http://%s:3400/notify>\r\n"
								   "NT: upnp:event\r\n"
								   "TIMEOUT: Second-3600\r\n"
								   "\r\n",
								   ipaddr);
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaRenderer_RenderingControl_event_message]%s\r\n", achBuf);

	iRet = sonos_drv_net_request(psSocketCfg, achBuf, iIdx, achBuf, sizeof(achBuf));
	if (iRet < 0)
	{
		return iRet;
	}

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_subscribe_MediaRenderer_RenderingControl_event_message] Received message: ================================================\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%s\r\n", achBuf);

 	return iRet;

}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	同步Sonos Driver模块的函数
;	(功能)  同步Sonos Driver模块
;	(参数)
;	(返回)
;	(注意)
*/
int sonos_drv_dev_send_subscribe_gena (const S_DevZbAttr* psDevNetAttr)
{
		S_SocketCfg sSocketCfg;
		int iRet;
		//listion gena notify
		do
		{
		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_dev_gena_synchro]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}
		iRet = sonos_drv_send_subscribe_MediaRenderer_AVTransport_event_message(&sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_dev_gena_synchro]: Can not send play message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
		iRet = sonos_drv_net_close(&sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_dev_gena_synchro]: There something wrong when close connect with sonos. Error: %d.\r\n", errno);
			return iRet;
		}

		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_dev_gena_synchro]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}
		iRet = sonos_drv_send_subscribe_MediaRenderer_RenderingControl_event_message(&sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_dev_gena_synchro]: Can not send play message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
		}while(0);
	return sonos_drv_net_close(&sSocketCfg);
}
//-------------------------------------------------------------------------------------------------*
/*	初始化Sonos Driver模块的函数
;	(功能)  初始化Sonos Driver模块
;	(参数)
;	(返回)
;	(注意)
*/
int sonos_drv_init (void)
{
	int iRet;

	iRet = sem_init(&s_tSonosDrvMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return -1;
	}
	sonos_net_manage_init();
	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	初始化Sonos Driver模块的函数
;	(功能)  初始化Sonos Driver模块
;	(参数)
;	(返回)
;	(注意)
*/
int sonos_drv_dev_init (const S_DevZbAttr* psDevNetAttr)
{
	UNUSED(psDevNetAttr);

	return 0;
}

//..............................................................................
/*	初始化Sonos Driver模块的函数
;	(功能)  初始化Sonos Driver模块
;	(参数)
;	(返回)
;	(注意)
*/
int sonos_drv_play (const S_DevZbAttr* psDevNetAttr, uint8_t ui8Play)
{
	S_SocketCfg sSocketCfg;
	int iRet;
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[sonos_drv_play] ##################[ play = %hhu  ]###########################\r\n",ui8Play);
	ASSERT(NULL != psDevNetAttr);

	do
	{
		iRet = sonos_drv_dev_send_subscribe_gena (psDevNetAttr);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_play]:sonos_drv_dev_send_subscribe_gena send fail.\r\n");
			return iRet;
		}
		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_play]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}
		iRet = sonos_drv_send_play_message(&sSocketCfg, ui8Play);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_play]: Can not send play message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
		iRet = sonos_drv_net_close(&sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_play]: There something wrong when close connect with sonos. Error: %d.\r\n", errno);
			return iRet;
		}


		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_play]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}
		iRet = sonos_drv_send_GetPositionInfo_message(&sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_play]: Can not send GetPositionInfo message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
	}while(0);
	return sonos_drv_net_close(&sSocketCfg);
}
//..............................................................................
/*	初始化Sonos Driver模块的函数
;	(功能)  初始化Sonos Driver模块
;	(参数)
;	(返回)
;	(注意)
*/
int sonos_drv_set_url (const S_DevZbAttr* psDevNetAttr, const char* pchUrl, size_t tLen)
{
	S_SocketCfg sSocketCfg;
	uint32_t ui32Seek;
	int iRet;
	char achMac[18];

	ASSERT(NULL != psDevNetAttr);
	do
	{
		iRet = sonos_drv_dev_send_subscribe_gena (psDevNetAttr);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]:sonos_drv_dev_send_subscribe_gena send fail.\r\n");
			return iRet;
		}
		iRet = sonos_drv_send_device_description_message(psDevNetAttr,achMac);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]:sonos_drv_send_device_description_message MAC fail.\r\n");
			return iRet;
		}
		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}
		iRet = sonos_drv_send_init_message(&sSocketCfg,achMac);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: Can not send INIT message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
		iRet = sonos_drv_net_close(&sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: There something wrong when close connect with sonos. Error: %d.\r\n", errno);
			return iRet;
		}

		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}
		iRet = sonos_drv_send_url_message(&sSocketCfg, pchUrl, tLen);
		ui32Seek = iRet;
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: Can not send URL message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
		iRet = sonos_drv_net_close(&sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: There something wrong when close connect with sonos. Error: %d.\r\n", errno);
			return iRet;
		}

		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}
		iRet = sonos_drv_send_seek_message(&sSocketCfg, ui32Seek);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: Can not send seek message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
		iRet = sonos_drv_net_close(&sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: There something wrong when close connect with sonos. Error: %d.\r\n", errno);
			return iRet;
		}

		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}
		iRet = sonos_drv_send_play_message(&sSocketCfg, 1);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_url]: Can not send play message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
	} while (0);

	return sonos_drv_net_close(&sSocketCfg);
}
//..............................................................................
/*	初始化Sonos Driver模块的函数
;	(功能)  初始化Sonos Driver模块
;	(参数)
;	(返回)
;	(注意)
*/
int sonos_drv_set_volume (const S_DevZbAttr* psDevNetAttr, uint16_t ui16Volume)
{
	S_SocketCfg sSocketCfg;
	int iRet;

	ASSERT(NULL != psDevNetAttr);

	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[sonos_drv_send_volume_message]****************************\r\n");
	//SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_DEBUG,"ui16Volume=%u\r\n", ui16Volume);
	if (ui16Volume > MUI_SONOS_STATE_VOLUME_MAX)
	{
		ui16Volume = MUI_SONOS_STATE_VOLUME_MAX;
	}
	do
	{
		iRet = sonos_drv_dev_send_subscribe_gena (psDevNetAttr);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_volume]:sonos_drv_dev_send_subscribe_gena.\r\n");
			return iRet;
		}
		iRet = sonos_drv_net_connect(psDevNetAttr, &sSocketCfg);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_volume]: Can not setup link to sonos:%s:%u.\r\n", psDevNetAttr->nwkaddr.ip, psDevNetAttr->ep.port);
			return iRet;
		}

		iRet = sonos_drv_send_volume_message(&sSocketCfg, ui16Volume);
		if (iRet < 0)
		{
			SONOS_DRV_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[sonos_drv_set_volume]: Can not send volume message.\r\n");
			close(sSocketCfg.iSocketFd);
			return iRet;
		}
	}while(0);
	return sonos_drv_net_close(&sSocketCfg);
}
//-------------------------------------------------------------------------------------------------*

