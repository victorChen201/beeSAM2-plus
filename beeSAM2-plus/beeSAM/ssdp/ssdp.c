/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of ssdp module for beeSAM project.
File name:ssdp.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/


#include "ssdp.h"
#include "sonos_drv.h"
#include "misc.h"

#ifdef M_SSDP_FEATUE
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//#define SSDP_DBG

#ifndef SSDP_PUTC
#ifdef SSDP_DBG
#define SSDP_PUTC(Char)			PUTC(Char)
#else
#define SSDP_PUTC(Char)
#endif
#endif

#ifndef SSDP_PUTS
#ifdef SSDP_DBG
#define SSDP_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SSDP_PUTS(Str, Len)
#endif
#endif

#ifndef SSDP_PUTASCC
#ifdef SSDP_DBG
#define SSDP_PUTASCC(Char)			PUTASCC(Char)
#else
#define SSDP_PUTASCC(Char)
#endif
#endif

#ifndef SSDP_PUTASCS
#ifdef SSDP_DBG
#define SSDP_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SSDP_PUTASCS(Str, Len)
#endif
#endif

#ifndef SSDP_PRINT
#ifdef SSDP_DBG
#define SSDP_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SSDP_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SSDP_DYN_PUTC
#ifdef SSDP_DBG
#define SSDP_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SSDP|Dbg), Char)
#else
#define SSDP_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SSDP_DYN_PUTS
#ifdef SSDP_DBG
#define SSDP_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SSDP|Dbg), Str, Len)
#else
#define SSDP_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SSDP_DYN_PUTASCC
#ifdef SSDP_DBG
#define SSDP_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SSDP|Dbg), Char)
#else
#define SSDP_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SSDP_DYN_PUTASCS
#ifdef SSDP_DBG
#define SSDP_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SSDP|Dbg), Str, Len)
#else
#define SSDP_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SSDP_DYN_PRINT
#ifdef SSDP_DBG
#define SSDP_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SSDP|Dbg), Fmt, ##Arg)
#else
#define SSDP_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_SSDP_SYS_INFO_MAX				50
#define MUI_SSDP_UUID_SIZE					36
#define MUI_SSDP_PARSE_BUF_SIZE				256
#define MUI_SSDP_HA_SEND_BUF_SIZE			1024
#define MUI_SSDP_RECV_BUF_SIZE				1024

#define MUI_SSDP_HA_NOTIFY_INTERVAL			50		//秒为单位

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//SSDP通知类型
typedef enum
{
	ESSDPNT_UUID = 0,
	ESSDPNT_DevInfo,
	ESSDPNT_RootDev
} E_SSDPNotifyType;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static const char s_achSysInfo[] = {"Linux/3.10.12"};
static const char s_achUUID[]    = {"2f402f80-da50-11e1-9b23-100123456789"};

static pthread_t s_sSSDPMessageListenThread;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int ssdp_ha_notify_alive_send (const S_SocketCfg *psDest, E_SSDPNotifyType eType);	//SSDP NOTIFY alive发送函数
static int ssdp_ha_notify_bye_send (const S_SocketCfg *psDest);	//SSDP NOTIFY bye发送函数
static int ssdp_ha_m_search_verfiy (const char *pchMsg, size_t tLen);	//SSDP M-SEARCH验证函数
static int ssdp_ha_m_search_answer (const S_SocketCfg *psDest, E_SSDPNotifyType eType);	//SSDP M-SEARCH 回答函数

static int ssdp_message_verfiy (const char *pchMsg, size_t tLen);	//SSDP 报文验证函数
static int ssdp_broadcasting (const S_SocketCfg *psSocketCfg);
static int ssdp_m_search_deal (const S_SocketCfg *psSocketCfg, void* pvBuf, size_t tLen);
static int ssdp_notify_deal (const S_SocketCfg *psSocketCfg, void* pvBuf, size_t tLen);
static int ssdp_answer_deal (const S_SocketCfg *psSocketCfg, void* pvBuf, size_t tLen);
static void* ssdp_message_listen_task (void *pArg);			/*ssdp_message_listen_task(简单服务发现协议)等待查询任务函数*/

//-------------------------------------------------------------------------------------------------*
/*	SSDP NOTIFY alive发送函数
;	(功能)
;	(参数)
;		psDest:		要发送信息的Socket配置结构信息
;		eType:		通知类型
;	(返回)
;		int:		-1:出错(包括异常及等待超时)
;					 0:正常完成
;	(注意)
*/
static int ssdp_ha_notify_alive_send (const S_SocketCfg *psDest, E_SSDPNotifyType eType)
{
	char achBuf[MUI_SSDP_HA_SEND_BUF_SIZE];
	char achIPAddr[16];
	int iIdx;

	ASSERT(psDest != NULL);

	iIdx = 0;
	// NOTIFY
	iIdx += sprintf(&achBuf[iIdx], "NOTIFY * HTTP/1.1\r\n");
	// HOST
	if (NULL == inet_ntop(psDest->sSocketAdd.sin_family, &psDest->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}
	iIdx += sprintf(&achBuf[iIdx], "HOST: %s:%d\r\n", achIPAddr, ntohs(psDest->sSocketAdd.sin_port));
	// CACHE-CONTROL
	iIdx += sprintf(&achBuf[iIdx], "CACHE-CONTROL: max-age=%d\r\n", 100);
	// LOCATION
	iIdx += sprintf(&achBuf[iIdx], "LOCATION: http://%s:%d/description.xml\r\n", net_listen_ip_addr_get(), 80);

	// SERVER
	iIdx += sprintf(&achBuf[iIdx], "SERVER: %s, UPnP/1.0, IpBridge/0.1\r\n", s_achSysInfo);
	// NTS
	iIdx += sprintf(&achBuf[iIdx], "NTS: ssdp:alive\r\n");

	switch (eType)
	{
	case ESSDPNT_UUID:
		// NT
		iIdx += sprintf(&achBuf[iIdx], "NT: uuid:%s\r\n", s_achUUID);
		// USN
		iIdx += sprintf(&achBuf[iIdx], "USN: uuid:%s\r\n", s_achUUID);
		break;
	case ESSDPNT_DevInfo:
		// NT
		iIdx += sprintf(&achBuf[iIdx], "NT: urn:schemas-upnp-org:device:basic:1\r\n");
		// USN
		iIdx += sprintf(&achBuf[iIdx], "USN: uuid:%s\r\n", s_achUUID);
		break;
	default: //ESSDPNT_RootDev:
		// NT
		iIdx += sprintf(&achBuf[iIdx], "NT: upnp:rootdevice\r\n");
		// USN
		iIdx += sprintf(&achBuf[iIdx], "USN: uuid:%s::upnp:rootdevice\r\n", s_achUUID);
		break;
	}

	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_alive_send]: #########################################\r\n");
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_alive_send]: Will send HTTP message to IP-'%s', Port-'%d':\r\n", inet_ntoa(psDest->sSocketAdd.sin_addr), ntohs(psDest->sSocketAdd.sin_port));
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%s\r\n", achBuf);
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_alive_send]: #########################################\r\n");

	// Start send message
	return sendto(psDest->iSocketFd, achBuf, iIdx, 0, (struct sockaddr *)&psDest->sSocketAdd, psDest->tSockLen);
}
//..............................................................................
/*	SSDP NOTIFY bye发送函数
;	(功能)
;	(参数)
;		psDest:		要发送信息的Socket配置结构信息
;	(返回)
;		int:		-1:出错(包括异常及等待超时)
;					 0:正常完成
;	(注意)
*/
static int ssdp_ha_notify_bye_send (const S_SocketCfg *psDest)
{
	char achBuf[MUI_SSDP_HA_SEND_BUF_SIZE];
	char achIPAddr[16];
	int iIdx;

	ASSERT(psDest != NULL);

	iIdx = 0;
	// NOTIFY
	iIdx += sprintf(&achBuf[iIdx], "NOTIFY * HTTP/1.1\r\n");
	// HOST
	if (NULL == inet_ntop(psDest->sSocketAdd.sin_family, &psDest->sSocketAdd.sin_addr.s_addr, achIPAddr, sizeof(achIPAddr)))
	{
		return -1;
	}
	iIdx += sprintf(&achBuf[iIdx], "HOST: %s:%d\r\n", achIPAddr, ntohs(psDest->sSocketAdd.sin_port));
	// SERVER
	iIdx += sprintf(&achBuf[iIdx], "SERVER: %s, UPnP/1.0, IpBridge/0.1\r\n", s_achSysInfo);
	// NTS
	iIdx += sprintf(&achBuf[iIdx], "NTS: ssdp:byebye\r\n");
	// NT
	iIdx += sprintf(&achBuf[iIdx], "NT: uuid:%s\r\n", s_achUUID);
	// USN
	iIdx += sprintf(&achBuf[iIdx], "USN: uuid:%s\r\n", s_achUUID);

	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_bye_send]: #########################################\r\n");
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_bye_send]: Will send HTTP message to IP-'%s', Port-'%d':\r\n", inet_ntoa(psDest->sSocketAdd.sin_addr), ntohs(psDest->sSocketAdd.sin_port));
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%s\r\n", achBuf);
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_bye_send]: #########################################\r\n");

	// Start send message
	return sendto(psDest->iSocketFd, achBuf, iIdx, 0, (struct sockaddr *)&psDest->sSocketAdd, psDest->tSockLen);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	SSDP M-SEARCH验证函数
;	(功能)
;	(参数)
;		pchMsg:		校验的报文
;		tLen:		报文长度
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:不匹配
;					 1:查询所有设备(ssdp:all)
;					 2:根设备匹配(upnp:rootdevice)
;					 3:UUID匹配(uuid:device-UUID)
;					 4:设备类型匹配(urn:schemas-upnp-org:device:device-Type:version)
;	(注意)
*/
static int ssdp_ha_m_search_verfiy (const char *pchMsg, size_t tLen)
{
	char achBuf[MUI_SSDP_PARSE_BUF_SIZE];
	char achTmp[MUI_SSDP_UUID_SIZE+10];	//uuid:UUID\r\n
	const char *pchCur, *pchTmp;
	int iTmp;

	ASSERT(pchMsg != NULL);
	UNUSED(tLen);

	// M-SEARCH
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Will verify 'M-SEARCH'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "M-SEARCH", tLen)))
	{
		pchCur = pchTmp;
	}
	else
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Haven't find 'M-SEARCH'\r\n");
		return 0;
	}
	/*
	iTmp = sub_str_get_util_rn(pchMsg, achBuf, MUI_SSDP_PARSE_BUF_SIZE-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: SubStr=%s\r\n", achBuf);
	if (NULL == strstr(pchMsg, "\r\nHTTP/1.1\r\n"))
	{
		return 0;
	}
	*/

	// HOST
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Will verify 'HOST'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nHOST", tLen)))
	{
		pchCur = pchTmp+2;
	}
	else if (NULL != (pchTmp=strnstr(pchMsg, "\r\nHost", tLen)))
	{
		pchCur = pchTmp+2;	//忽略之前的\r\rn
	}
	if (NULL == pchCur)
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Haven't find 'HOST'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Call 'sub_str_get_util_rn(HOST)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: SubStr=%s\r\n", achBuf);
	if (NULL == strnstr(achBuf, "239.255.255.250:1900\r\n", iTmp))
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Haven't find '239.255.255.250:1900'\r\n");
		return 0;
	}

	// MAN
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Will verify 'MAN'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nMAN", tLen)))
	{
		pchCur = pchTmp+2;
	}
	else if (NULL != (pchTmp=strnstr(pchMsg, "\r\nMan", tLen)))
	{
		pchCur = pchTmp+2;
	}
	if (NULL == pchCur)
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Haven't find 'MAN'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Call 'sub_str_get_util_rn(MAN)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: SubStr=%s\r\n", achBuf);
	if (NULL == strnstr(achBuf, "\"ssdp:discover\"\r\n", iTmp))
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Haven't find '\"ssdp:discover\"'\r\n");
		return 0;
	}

	// MX
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Will verify 'MX'\r\n");

	// ST
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Will verify 'ST'\r\n");
	pchCur = NULL;
	if (NULL != (pchTmp=strnstr(pchMsg, "\r\nST", tLen)))
	{
		pchCur = pchTmp+2;
	}
	if (NULL == pchCur)
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Haven't find 'ST'\r\n");
		return 0;
	}
	iTmp = sub_str_get_util_rn(pchCur, achBuf, tLen-(pchCur-pchMsg));
	if (iTmp < 0)
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Call 'sub_str_get_util_rn(ST)' fail\r\n");
		return iTmp;
	}
	achBuf[iTmp] = '\0';
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: SubStr=%s\r\n", achBuf);
	sprintf(achTmp, "uuid:%s\r\n", s_achUUID);
	if (NULL != strnstr(achBuf, "ssdp:all\r\n", iTmp))
	{
		return 1;	//1:查询所有设备(ssdp:all)
	}
	else if (NULL != strnstr(achBuf, "upnp:rootdevice\r\n", iTmp))
	{
		return 2;	//2:根设备匹配(upnp:rootdevice)
	}
	else if (NULL != strnstr(achBuf, achTmp, iTmp))
	{
		return 3;	//3:UUID匹配(uuid:device-UUID)
	}
	else if (NULL != strnstr(achBuf, "urn:schemas-upnp-org:device:basic:1\r\n", iTmp))
	{
		return 4;	//4:设备类型匹配(urn:schemas-upnp-org:device:device-Type:version)
	}

	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_verify]: Haven't find any ST destination\r\n");

	return 0;
}
//..............................................................................
/*	SSDP M-SEARCH 回答函数
;	(功能)
;	(参数)
;		psDest:		要发送信息的Socket配置结构信息
;		eType:		通知类型
;	(返回)
;		int:		-1:出错(包括异常及等待超时)
;					 0:正常完成
;	(注意)
*/
static int ssdp_ha_m_search_answer (const S_SocketCfg *psDest, E_SSDPNotifyType eType)
{
	char achBuf[MUI_SSDP_HA_SEND_BUF_SIZE];
	int iIdx;

	ASSERT(psDest != NULL);

	iIdx = 0;
	// HTTP OK
	iIdx += sprintf(&achBuf[iIdx], "HTTP/1.1 200 OK\r\n");
	// CACHE-CONTROL
	iIdx += sprintf(&achBuf[iIdx], "CACHE-CONTROL: max-age=%d\r\n", 100);
	// EXT
	iIdx += sprintf(&achBuf[iIdx], "EXT:\r\n");
	// LOCATION
	iIdx += sprintf(&achBuf[iIdx], "LOCATION: http://%s:%d/description.xml\r\n", net_listen_ip_addr_get(), 80);
	// SERVER
	iIdx += sprintf(&achBuf[iIdx], "SERVER: %s, UPnP/1.0, IpBridge/0.1\r\n", s_achSysInfo);

	switch (eType)
	{
	case ESSDPNT_UUID:
		// ST
		iIdx += sprintf(&achBuf[iIdx], "ST: uuid:%s\r\n", s_achUUID);
		// USN
		iIdx += sprintf(&achBuf[iIdx], "USN: uuid:%s\r\n", s_achUUID);
		break;
	case ESSDPNT_DevInfo:
		// ST
		iIdx += sprintf(&achBuf[iIdx], "ST: urn:schemas-upnp-org:device:basic:1\r\n");
		// USN
		iIdx += sprintf(&achBuf[iIdx], "USN: uuid:%s\r\n", s_achUUID);
		break;
	default: //ESSDPNT_RootDev:
		// ST
		iIdx += sprintf(&achBuf[iIdx], "ST: upnp:rootdevice\r\n");
		// USN
		iIdx += sprintf(&achBuf[iIdx], "USN: uuid:%s::upnp:rootdevice\r\n", s_achUUID);
		break;
	}

	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_answer]: #########################################\r\n");
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_answer]: Will send HTTP message to IP-'%s', Port-'%d':\r\n", inet_ntoa(psDest->sSocketAdd.sin_addr), ntohs(psDest->sSocketAdd.sin_port));
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%s\r\n", achBuf);
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_m_search_answer]: #########################################\r\n");

	// Start send message
	return sendto(psDest->iSocketFd, achBuf, iIdx, 0, (struct sockaddr *)&psDest->sSocketAdd, psDest->tSockLen);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	ssdp HA Notify 广播函数
;	(功能)进行SSDP广播.
;	(参数)
;		psSocketCfg:	SSDP socket参数指针
;	(返回)
;		int
;	(注意)
*/
static int ssdp_ha_notify_broadcasting (const S_SocketCfg *psSocketCfg)
{
	static int iCnt = MUI_SSDP_HA_NOTIFY_INTERVAL;
	int iRet;

	if (iCnt < ((MUI_SSDP_HA_NOTIFY_INTERVAL/MUI_SSDP_SELECT_WAIT_S)))
	{
		iCnt++;
		return 0;
	}

	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_broadcasting]: Will broadcasting HA Notify.\r\n");

	iCnt = 0;

	iRet = ssdp_ha_notify_alive_send(psSocketCfg, ESSDPNT_RootDev);
	if (iRet < 0)
	{
		//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_broadcasting]: Broadcast rootdevice error!\r\n");
	}
	iRet = ssdp_ha_notify_alive_send(psSocketCfg, ESSDPNT_RootDev);
	if (iRet < 0)
	{
		//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_broadcasting]: Broadcast rootdevice error!\r\n");
	}
	iRet = ssdp_ha_notify_alive_send(psSocketCfg, ESSDPNT_UUID);
	if (iRet < 0)
	{
		//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_broadcasting]: Broadcast uuid error!\r\n");
	}
	iRet = ssdp_ha_notify_alive_send(psSocketCfg, ESSDPNT_UUID);
	if (iRet < 0)
	{
		//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_broadcasting]: Broadcast uuid error!\r\n");
	}
	iRet = ssdp_ha_notify_alive_send(psSocketCfg, ESSDPNT_DevInfo);
	if (iRet < 0)
	{
		//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_broadcasting]: Broadcast upnp error!\r\n");
	}
	iRet = ssdp_ha_notify_alive_send(psSocketCfg, ESSDPNT_DevInfo);
	if (iRet < 0)
	{
		//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_ha_notify_broadcasting]: Broadcast upnp error!\r\n");
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	SSDP 报文验证函数
;	(功能)
;	(参数)
;		pchMsg:		校验的报文
;		tLen:		报文长度
;	(返回)
;		int:		<0:出错(包括异常及等待超时)
;					 0:M-Search
;					 1:Notify
;					 2:Answer
;					 *:Invalid
;	(注意)
*/
static int ssdp_message_verfiy (const char *pchMsg, size_t tLen)
{
	ASSERT(pchMsg != NULL);

	// M-SEARCH
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_verfiy]: Will verify 'M-SEARCH'\r\n");
	if (NULL != strnstr(pchMsg, "M-SEARCH * HTTP/1.1\r\n", tLen))
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_verfiy]: Have find 'M-SEARCH'\r\n");
		return 0;
	}

	// NOTIFY
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_verfiy]: Will verify 'NOTIFY'\r\n");
	if (NULL != strnstr(pchMsg, "NOTIFY * HTTP/1.1\r\n", tLen))
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_verfiy]: Have find 'NOTIFY'\r\n");
		return 1;
	}

	// ANSWER
	//SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_verfiy]: Will verify 'NOTIFY'\r\n");
	if (NULL != strnstr(pchMsg, "HTTP/1.1 200 OK\r\n", tLen))
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_verfiy]: Have find 'ANSWER'\r\n");
		return 2;
	}

	return -1;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	ssdp 广播函数
;	(功能)进行SSDP广播.
;	(参数)
;		psSocketCfg:	SSDP socket参数指针
;	(返回)
;		int
;	(注意)
*/
static int ssdp_broadcasting (const S_SocketCfg *psSocketCfg)
{
	S_SocketCfg sSocketCfg;

	ASSERT(psSocketCfg != NULL);

	memcpy(&sSocketCfg, psSocketCfg, sizeof(sSocketCfg));
	sSocketCfg.sSocketAdd.sin_family	  = AF_INET;
	sSocketCfg.sSocketAdd.sin_addr.s_addr = inet_addr(MSTR_SSDP_MULTICAST_IP);
	sSocketCfg.sSocketAdd.sin_port		  = htons(MUI_SSDP_MULTICAST_PORT);
	sSocketCfg.tSockLen					  = sizeof(struct sockaddr_in);

	ssdp_ha_notify_broadcasting(&sSocketCfg);
	sonos_drv_m_search_broadcasting(&sSocketCfg);

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	ssdp M-Search 处理函数
;	(功能)处理收到的M-Search报文.
;	(参数)
;		psSocketCfg:	SSDP socket参数指针
;		pvBuf:			SSDP 报文指针
;		tLen:			SSDP 报文长度
;	(返回)
;		int
;	(注意)
*/
static int ssdp_m_search_deal (const S_SocketCfg *psSocketCfg, void* pvBuf, size_t tLen)
{
	int iRet;

	iRet = ssdp_ha_m_search_verfiy((const char*)pvBuf, tLen);
	if ((1 <=iRet) && (iRet <= 4))
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_m_search_deal]: Will send HA m-search answer!\r\n");

		iRet = ssdp_ha_m_search_answer(psSocketCfg, ESSDPNT_RootDev);
		if (iRet < 0)
		{
			printf("[ssdp_m_search_deal]: Answer rootdevice error: '%d'!\r\n", errno);
		}
		iRet = ssdp_ha_m_search_answer(psSocketCfg, ESSDPNT_UUID);
		if (iRet < 0)
		{
			printf("[ssdp_m_search_deal]: Answer uuid error: '%d'!\r\n", errno);
		}
		iRet = ssdp_ha_m_search_answer(psSocketCfg, ESSDPNT_DevInfo);
		if (iRet < 0)
		{
			printf("[ssdp_m_search_deal]: Answer upnp error: '%d'!\r\n", errno);
		}

		iRet = ssdp_ha_m_search_answer(psSocketCfg, ESSDPNT_RootDev);
		if (iRet < 0)
		{
			printf("[ssdp_m_search_deal]: Answer rootdevice error: '%d'!\r\n", errno);
		}
		iRet = ssdp_ha_m_search_answer(psSocketCfg, ESSDPNT_UUID);
		if (iRet < 0)
		{
			printf("[ssdp_m_search_deal]: Answer uuid error: '%d'!\r\n", errno);
		}
		iRet = ssdp_ha_m_search_answer(psSocketCfg, ESSDPNT_DevInfo);
		if (iRet < 0)
		{
			printf("[ssdp_m_search_deal]: Answer upnp error: '%d'!\r\n", errno);
		}
	}

	return 0;
}
//..............................................................................
/*	ssdp Notify 处理函数
;	(功能)处理收到的M-Search报文.
;	(参数)
;		psSocketCfg:	SSDP socket参数指针
;		pvBuf:			SSDP 报文指针
;		tLen:			SSDP 报文长度
;	(返回)
;		int
;	(注意)
*/
static int ssdp_notify_deal (const S_SocketCfg *psSocketCfg, void* pvBuf, size_t tLen)
{
	sonos_drv_notify_deal(psSocketCfg, pvBuf, tLen);

	return 0;
}
//..............................................................................
/*	ssdp M-Search 处理函数
;	(功能)处理收到的M-Search报文.
;	(参数)
;		psSocketCfg:	SSDP socket参数指针
;		pvBuf:			SSDP 报文指针
;		tLen:			SSDP 报文长度
;	(返回)
;		int
;	(注意)
*/
static int ssdp_answer_deal (const S_SocketCfg *psSocketCfg, void* pvBuf, size_t tLen)
{
	sonos_drv_answer_deal(psSocketCfg, pvBuf, tLen);

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	ssdp_message_listen_task(简单服务发现协议)等待查询线程函数
;	(功能)等待SSDP报文.
;	(参数)
;		pArg:	参数指针
;	(返回)
;		void*	返回指针
;	(注意)
*/
static void* ssdp_message_listen_task (void *pArg)
{
	char achBuf[MUI_SSDP_RECV_BUF_SIZE];
	struct sockaddr_in sLocalAddr;
	struct ip_mreq sMreq;
	S_SocketCfg sSocketCfg;
	struct timeval sTime;
	fd_set sRdFdSet;
	int iMaxFd, iFd, iRet, iTmp;
	uint8_t ui8LedState = 0;

	UNUSED(pArg);

	//Creat socket for UDP
	iFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (iFd < 0)
	{
		printf("ssdp_message_listen_task: Creat the socket fail, error number: '%d'.\r\n", errno);
		abort();
	}
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ssdp_message_listen_task: Creat socket successful!\r\n");

	//Socket:可再用地址
	iTmp = 1;
	iRet = setsockopt(iFd, SOL_SOCKET,SO_REUSEADDR, &iTmp, sizeof(iTmp));
	if (iRet < 0)
	{
		printf("ssdp_message_listen_task: Set reuse address fail, error number: '%d'.\r\n", errno);
		abort();
	}
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ssdp_message_listen_task: Set reuse address successful!\r\n");
#if 1
	//多播设置:TTL值
	iTmp = 10;	//最多转发10次
	iRet = setsockopt(iFd, IPPROTO_IP, IP_MULTICAST_TTL, &iTmp, sizeof(iTmp));
	if (iRet < 0)
	{
		printf("ssdp_message_listen_task: Set multicast TTL fail, error number: '%d'.\r\n", errno);
		abort();
	}
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ssdp_message_listen_task: Set multicast TTL successful!\r\n");

	//多播设置:LOOP设置(是否回送)
	iTmp = 0;	//禁止本地环回
	iRet = setsockopt(iFd, IPPROTO_IP, IP_MULTICAST_LOOP, &iTmp, sizeof(iTmp));
	if (iRet < 0)
	{
		printf("ssdp_message_listen_task: Set multicast LOOP fail, error number: '%d'.\r\n", errno);
		abort();
	}
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ssdp_message_listen_task: Set multicast LOOP successful!\r\n");
#endif
	//绑定Socket地址
	//Set socket address
	bzero(&sLocalAddr, sizeof(sLocalAddr));
	sLocalAddr.sin_family	   = AF_INET;
	sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sLocalAddr.sin_port		   = htons(MUI_SSDP_MULTICAST_PORT);
	iRet = bind(iFd, (struct sockaddr *)&sLocalAddr, sizeof(struct sockaddr));
	if (iRet < 0)
	{
		printf("ssdp_message_listen_task: Bind the socket address fail, error number: '%d'.\r\n", errno);
		abort();
	}
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ssdp_message_listen_task: Bind LocalAddr successful!\r\n");

	//将本地地址加入多播组
	sMreq.imr_multiaddr.s_addr = inet_addr(MSTR_SSDP_MULTICAST_IP);
	sMreq.imr_interface.s_addr = inet_addr(net_listen_ip_addr_get());
	iRet = setsockopt(iFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &sMreq, sizeof(sMreq));
	if (iRet < 0)
	{
		printf("ssdp_message_listen_task: Add multicast member fail, error number: '%d'.\r\n", errno);
		abort();
	}
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ssdp_message_listen_task: Add multicast member successful!\r\n");

	//设置sSocketCfg
	bzero(&sSocketCfg, sizeof(sSocketCfg));
	sSocketCfg.iSocketFd = iFd;
	sSocketCfg.sSocketAdd.sin_family	  = AF_INET;
	sSocketCfg.sSocketAdd.sin_addr.s_addr = inet_addr(MSTR_SSDP_MULTICAST_IP);
	sSocketCfg.sSocketAdd.sin_port		  = htons(MUI_SSDP_MULTICAST_PORT);
	sSocketCfg.tSockLen					  = sizeof(struct sockaddr_in);

	//等待报文
	while (1)
	{
		//Timeout setting
		sTime.tv_sec  = MUI_SSDP_SELECT_WAIT_S;
		sTime.tv_usec = 0;

		//Initialize read file descriptor set
		FD_ZERO(&sRdFdSet);
		FD_SET(iFd, &sRdFdSet);
		iMaxFd = iFd;

		iTmp = select(iMaxFd+1, &sRdFdSet, NULL, NULL, &sTime);
		if (iTmp < 0)
		{
			SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]: Call 'select' error: %d\r\n", errno);
			if (0 != get_led_light_mode())
			{
				if (0 != button_state_get(MUI_GPIO_PIN_FUNC_KEY))
				{
					led_light_ctrl(MUI_GPIO_PIN_PWR_LED, ui8LedState);
					led_light_ctrl(MUI_GPIO_PIN_NET_LED, ui8LedState);
					led_light_ctrl(MUI_GPIO_PIN_ZIGBEE_LED, ui8LedState);
					ui8LedState = !ui8LedState;
				}
			}
			continue;
		}
		else if (0 == iTmp)
		{
			SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]:Wait for select timeout.\r\n");

			ssdp_broadcasting(&sSocketCfg);

			continue;
		}
		else
		{
			if (FD_ISSET(iFd, &sRdFdSet))
			{
				iTmp = recvfrom(iFd, achBuf, sizeof(achBuf), 0 , (struct sockaddr *)&sSocketCfg.sSocketAdd, &sSocketCfg.tSockLen);
				if (iTmp < 0)
				{
					SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]: Call 'recvfrom' error: %d\r\n", errno);
				}
				else
				{
					achBuf[iTmp] = '\0';

					SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]: =========================================\r\n");
					SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]: Receive message from IP-'%s', Port-'%d'\r\n", inet_ntoa(sSocketCfg.sSocketAdd.sin_addr), ntohs(sSocketCfg.sSocketAdd.sin_port));
					SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%s\r\n", achBuf);
					SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]: =========================================\r\n");

					//解析报文
					switch (ssdp_message_verfiy(achBuf, iTmp))
					{
						case 0:
							SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]: Got M-Search message.\r\n");
							ssdp_m_search_deal(&sSocketCfg, achBuf, iTmp);
							break;
						case 1:
							SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]: Got Notify message.\r\n");
							ssdp_notify_deal(&sSocketCfg, achBuf, iTmp);
							break;
						case 2:
							SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_message_listen_task]: Got Answer message.\r\n");
							ssdp_answer_deal(&sSocketCfg, achBuf, iTmp);
							break;
						default:
							break;
					}
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	SSDP初始化函数
;	(功能)
;	(参数)
;		pchSysInfo:	系统信息指针
;		pchUUID:	UUID信息指针
;	(返回)
;		int:		-1:出错(包括异常及等待超时)
;					 0:正常完成
;	(注意)
*/
int ssdp_init (void)
{
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Will run ssdp_init\r\n");

	int iRet;

	iRet = pthread_create(&s_sSSDPMessageListenThread, NULL, ssdp_message_listen_task, NULL);
	if (iRet < 0)
	{
		SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ssdp_init]: Can not create task 'ssdp_message_listen_task'.\r\n");
		return iRet;
	}
	SSDP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ssdp_init\r\n");

	return 0;
}


//-------------------------------------------------------------------------------------------------*
/*	SSDP退出等待函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:			 <0:出错(包括异常及等待超时)
;						>=0:正常退出
;	(注意)
*/
int ssdp_quit_wait (void)
{
	int iRet;

	iRet = pthread_join(s_sSSDPMessageListenThread, NULL);
	if (iRet != 0)
	{
		printf("[ssdp_quit_wait]Error joining thread 'ssdp_message_listen_task'.\r\n");
		abort();
	}

	return iRet;
}

//-------------------------------------------------------------------------------------------------*
#endif
