/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of remote_net_manage module for beeSAM project.
File name:remote_net_manage.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "wsclient_manager.h"

#ifdef M_REMOTE_COMMUNICATION
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define REMOTE_NET_MANAGE_DBG

#ifndef REMOTE_NET_MANAGE_PUTC
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_PUTC(Char)			PUTC(Char)
#else
#define REMOTE_NET_MANAGE_PUTC(Char)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_PUTS
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define REMOTE_NET_MANAGE_PUTS(Str, Len)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_PUTASCC
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_PUTASCC(Char)			PUTASCC(Char)
#else
#define REMOTE_NET_MANAGE_PUTASCC(Char)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_PUTASCS
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define REMOTE_NET_MANAGE_PUTASCS(Str, Len)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_PRINT
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define REMOTE_NET_MANAGE_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_DYN_PUTC
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_REMOTE_NET_MANAGE|Dbg), Char)
#else
#define REMOTE_NET_MANAGE_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_DYN_PUTS
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_REMOTE_NET_MANAGE|Dbg), Str, Len)
#else
#define REMOTE_NET_MANAGE_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_DYN_PUTASCC
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_REMOTE_NET_MANAGE|Dbg), Char)
#else
#define REMOTE_NET_MANAGE_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_DYN_PUTASCS
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_REMOTE_NET_MANAGE|Dbg), Str, Len)
#else
#define REMOTE_NET_MANAGE_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef REMOTE_NET_MANAGE_DYN_PRINT
#ifdef REMOTE_NET_MANAGE_DBG
#define REMOTE_NET_MANAGE_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_REMOTE_NET_MANAGE|Dbg), Fmt, ##Arg)
#else
#define REMOTE_NET_MANAGE_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define  MSTR_REMOTE_NET_BEAT_REQUEST	"BEATPACK:request"
#define  MSTR_REMOTE_NET_BEAT_RESPONSE	"BEATPACK:response"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static uint8_t s_ui8BeatInterval = 10;
static uint8_t s_ui8RemoteNetBeatUnresponseCnt = 0;
static wsclient *s_psRemoteNetWsClient = NULL;
static pthread_t s_sRemoteNetDeamonThread;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int onclose (wsclient *c);
int onerror (wsclient *c, wsclient_error *err);
int onmessage (wsclient *c, wsclient_message *msg);
int onopen (wsclient *c);
static int s_remote_net_create_websocket_connect (const char *pchAddr, uint16_t ui16Port);
static int s_remote_net_send_beat_message (S_StrDevId *psDevId);
static void* s_remote_net_deamon_task (void *pArg);


int onclose (wsclient *c)
{
	fprintf(stderr, "onclose called: %d\n", c->sockfd);
	return 0;
}

int onerror (wsclient *c, wsclient_error *err)
{
	fprintf(stderr, "onerror: (%d): %s\n", err->code, err->str);
	if (err->extra_code)
	{
		errno = err->extra_code;
		//perror("recv");
	}
	return 0;
}

int onmessage (wsclient *c, wsclient_message *msg)
{
	s_ui8RemoteNetBeatUnresponseCnt = 0;

	if (0 == strcmp(msg->payload, MSTR_REMOTE_NET_BEAT_RESPONSE))
	{
		//REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[onmessage]Have received beat response message.\r\n");
		return 0;
	}

	//REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[onmessage]Have received API message.\r\n");
	remote_msg_receive(msg->payload, (size_t)msg->payload_len);

	return 0;
}

int onopen (wsclient *c)
{
	S_StrDevId sDevId;
	char achBuf[100];
	int iRet;

	iRet = read_config_info_dev_id(&sDevId);
	if (iRet < 0)
	{
		snprintf(achBuf, sizeof(achBuf), "{\"client_sn\":99999999999999}");
	}
	else
	{
		snprintf(achBuf, sizeof(achBuf), "{\"client_sn\":%s}", sDevId.str);
	}
	libwsclient_send(c, achBuf);

	return 0;
}

static int s_remote_net_create_websocket_connect (const char *pchAddr, uint16_t ui16Port)
{
	char achUri[256];

	snprintf(achUri, sizeof(achUri), "%s:%u", pchAddr, ui16Port);
	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_net_create_websocket_connect]URI=%s\r\n", achUri);
	//开始创建连接
	s_psRemoteNetWsClient = libwsclient_new(achUri);
	if (!s_psRemoteNetWsClient)
	{
		REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_net_create_websocket_connect]Call 'libwsclient_new(%s)' fail.\r\n", achUri);
		return -1;
	}
	libwsclient_onopen(s_psRemoteNetWsClient, &onopen);
	libwsclient_onmessage(s_psRemoteNetWsClient, &onmessage);
	libwsclient_onerror(s_psRemoteNetWsClient, &onerror);
	libwsclient_onclose(s_psRemoteNetWsClient, &onclose);
	libwsclient_helper_socket(s_psRemoteNetWsClient, "wsclien.sock");
	//libwsclient_helper_socket(s_psRemoteNetWsClient, "test.sock");
	libwsclient_run(s_psRemoteNetWsClient);

	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_net_create_websocket_connect]Have create client object\r\n");
	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "s_psRemoteNetWsClient->URI = %s\r\n", s_psRemoteNetWsClient->URI);
	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "s_psRemoteNetWsClient->sockfd = %u\r\n", s_psRemoteNetWsClient->sockfd);
	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "s_psRemoteNetWsClient->flags = %u\r\n", s_psRemoteNetWsClient->flags);

	return 0;
}


static int s_remote_net_send_beat_message (S_StrDevId *psDevId)
{
	char achBuf[64];
	int iRet;

	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_net_send_beat_message]Send beat message.\r\n");

	iRet = snprintf(achBuf, sizeof(achBuf), MSTR_REMOTE_NET_BEAT_REQUEST"{%s}", psDevId->str);
	ASSERT(iRet < sizeof(achBuf));
	iRet = libwsclient_send(s_psRemoteNetWsClient, achBuf);
	s_ui8RemoteNetBeatUnresponseCnt++;

	return iRet;
}

//-------------------------------------------------------------------------------------------------*
/*	远程网络守护任务函数
;	(功能)守护远程网络
;	(参数)
;		pArg:	传给函数运行的参数
;	(返回)
;		NULL
;	(注意)
*/
static void* s_remote_net_deamon_task (void *pArg)
{
	S_StrDevId sDevId;
	S_CfgProxyAddr sCfgProxyAddr;
	uint16_t ui16ProxyPort;
	uint8_t ui8PortalServices;
	uint8_t ui8BeatInterval;
	int iRet;

	ui8BeatInterval = *((uint8_t*)pArg);

	while (NULL == s_psRemoteNetWsClient)
	{
		//REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_net_deamon_task]Try to create connection.\r\n");

		iRet = read_config_info_portal_services(&ui8PortalServices);
		if (iRet < 0)
		{
			REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_net_deamon_task]Call 'read_config_info_portal_services()' fail.\r\n");
			continue;
		}
		if (0 == ui8PortalServices)
		{
			msleep((uint32_t)ui8BeatInterval * 1000);
			continue;
		}
		iRet = read_config_info_proxy_addr(&sCfgProxyAddr);
		if (iRet < 0)
		{
			REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_net_deamon_task]Call 'read_config_info_proxy_addr()' fail.\r\n");
			continue;
		}
		if ('\0' == sCfgProxyAddr.str[0])
		{
			REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_remote_net_deamon_task]Proxy address invalid.\r\n");
			continue;
		}
		iRet = read_config_info_proxy_port(&ui16ProxyPort);
		if (iRet < 0)
		{
			REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_net_deamon_task]Call 'read_config_info_proxy_port()' fail.\r\n");
			continue;
		}

		iRet = s_remote_net_create_websocket_connect(sCfgProxyAddr.str, ui16ProxyPort);
		if (iRet < 0)
		{
			REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_remote_net_deamon_task]Proxy address invalid.\r\n");
		}

		REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_net_deamon_task]Have create websocket connection.\r\n");
		break;
	}

	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_net_deamon_task]######s_psRemoteNetWsClient->sockfd=%d\r\n", s_psRemoteNetWsClient->sockfd);

	while (1)
	{
		//send beat message
		iRet = read_config_info_dev_id(&sDevId);
		if (iRet < 0)
		{
			REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_remote_net_deamon_task]Call 'read_config_info_dev_id()' fail.\r\n");
		}
		else
		{
			s_remote_net_send_beat_message(&sDevId);
		}

		REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_remote_net_deamon_task]ui8BeatInterval=%u\r\n", ui8BeatInterval);
		sleep(ui8BeatInterval);
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	远程网络管理初始化函数
;	(功能)
;	(参数)
;		ui8BeatInterval:	心跳间隔时间(单位s)
;	(返回)
;		int:				 <0:出错(包括异常及等待超时)
;							>=0:正常完成
;	(注意)
*/
int remote_net_manage_init (uint8_t ui8BeatInterval)
{
	int iRet;

	if ((ui8BeatInterval<1) || (20<ui8BeatInterval))
	{
		ui8BeatInterval = 10;
	}

	s_ui8BeatInterval = ui8BeatInterval;

	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[remote_net_manage_init]s_ui8BeatInterval=%u\r\n", s_ui8BeatInterval);

	iRet = pthread_create(&s_sRemoteNetDeamonThread, NULL, s_remote_net_deamon_task, &s_ui8BeatInterval);

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
int remote_net_send_data (const char *pchData, size_t tLen)
{
	UNUSED(tLen);

	if ((NULL==s_psRemoteNetWsClient) || (0==s_psRemoteNetWsClient->sockfd))
	{
		REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_ERR, "[remote_net_send_data]Remote net does not connected.\r\n");
		return -1;
	}

	/*
	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[remote_net_send_data]pchData length = %u\r\n", tLen);
	REMOTE_NET_MANAGE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%s\r\n", (const char*)pchData);
	*/

	return libwsclient_send(s_psRemoteNetWsClient, pchData);
}

void remote_net_quit_wait (void)
{
	libwsclient_finish(s_psRemoteNetWsClient);
	free(s_psRemoteNetWsClient);
	s_psRemoteNetWsClient = NULL;
}

#endif
