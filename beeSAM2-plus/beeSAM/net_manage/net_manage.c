/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of net_manage module for beeSAM project.
File name:net_manage.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "net_manage.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define NET_MANAGE_DBG

#ifndef NET_MANAGE_PUTC
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_PUTC(Char)			PUTC(Char)
#else
#define NET_MANAGE_PUTC(Char)
#endif
#endif

#ifndef NET_MANAGE_PUTS
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define NET_MANAGE_PUTS(Str, Len)
#endif
#endif

#ifndef NET_MANAGE_PUTASCC
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_PUTASCC(Char)			PUTASCC(Char)
#else
#define NET_MANAGE_PUTASCC(Char)
#endif
#endif

#ifndef NET_MANAGE_PUTASCS
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define NET_MANAGE_PUTASCS(Str, Len)
#endif
#endif

#ifndef NET_MANAGE_PRINT
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define NET_MANAGE_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef NET_MANAGE_DYN_PUTC
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_NET_MANAGE|Dbg), Char)
#else
#define NET_MANAGE_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef NET_MANAGE_DYN_PUTS
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_NET_MANAGE|Dbg), Str, Len)
#else
#define NET_MANAGE_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef NET_MANAGE_DYN_PUTASCC
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_NET_MANAGE|Dbg), Char)
#else
#define NET_MANAGE_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef NET_MANAGE_DYN_PUTASCS
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_NET_MANAGE|Dbg), Str, Len)
#else
#define NET_MANAGE_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef NET_MANAGE_DYN_PRINT
#ifdef NET_MANAGE_DBG
#define NET_MANAGE_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_NET_MANAGE|Dbg), Fmt, ##Arg)
#else
#define NET_MANAGE_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif


//-------------------------------------------------------------------------------------------------*
/*	Net(TCP)HTTP报文接收函数
;	(功能)根据索引检查接管理组中指定的链接,如果超时则清理它.
;	(参数)
;		psConntCtrl:	指向连接控制结构体的指针
;		pArg:			指向接收缓存控制结构的指针
;	(返回)
;		int:		 	MUI_HTTP_DEAL_WAIT_MESSAGE:	等待接收报文
;						MUI_HTTP_DEAL_RECVED_LINE:	已接收请求行
;						MUI_HTTP_DEAL_RECVED_HEAD:	已接收请求头
;						MUI_HTTP_DEAL_RECVED_BODY:	已接收请求体
;						MUI_HTTP_DEAL_PARSE_ERR:	分析出现致命错误
;	(注意)
*/
int net_http_message_recv(S_ConntCtrl *psConntCtrl, S_HttpArg *pArg)
{
	long lTmp;
	uint8_t *pui8Tmp, *pui8UrlTail;

	ASSERT(psConntCtrl != NULL);
	ASSERT(pArg != NULL);
	ASSERT(pArg->tLen > 0);

	psConntCtrl->tLen += pArg->tLen;

	switch (psConntCtrl->sHttpInfo.ui8Schedule)
	{
 		case MUI_HTTP_DEAL_WAIT_MESSAGE:
			psConntCtrl->sHttpInfo.pui8Line = psConntCtrl->pui8Buf;
			pui8Tmp = mem_search(psConntCtrl->sHttpInfo.pui8Line, psConntCtrl->tLen, "HTTP/1.1\r\n", sizeof("HTTP/1.1\r\n")-1);
			if (NULL == pui8Tmp)
			{
				return psConntCtrl->sHttpInfo.ui8Schedule;
			}
			if ('/' == *(pui8Tmp-2))
			{
				pui8UrlTail = pui8Tmp - 2;	////指向URL最后一个'/',即忽略多余的'/'
			}
			else
			{
				pui8UrlTail = pui8Tmp - 1;	//指向URL后面的空格
			}
			psConntCtrl->sHttpInfo.pui8Head = pui8Tmp + sizeof("HTTP/1.1\r\n") - 1;
			//Identify method
			if (NULL != mem_search(psConntCtrl->sHttpInfo.pui8Line, sizeof("POST ")-1, "POST ", sizeof("POST ")-1))
			{
				psConntCtrl->sHttpInfo.ui8Method  = MUI_HTTP_METHOD_POST;
				psConntCtrl->sHttpInfo.pchUrl	  = (char*)(psConntCtrl->sHttpInfo.pui8Line + sizeof("POST ") - 1);
				psConntCtrl->sHttpInfo.ui16UrlLen = (uint16_t)(pui8UrlTail - (uint8_t*)psConntCtrl->sHttpInfo.pchUrl);
			}
			else if (NULL != mem_search(psConntCtrl->sHttpInfo.pui8Line, sizeof("GET ")-1, "GET ", sizeof("GET ")-1))
			{
				psConntCtrl->sHttpInfo.ui8Method  = MUI_HTTP_METHOD_GET;
				psConntCtrl->sHttpInfo.pchUrl	  = (char*)(psConntCtrl->sHttpInfo.pui8Line + sizeof("GET ") - 1);
				psConntCtrl->sHttpInfo.ui16UrlLen = (uint16_t)(pui8UrlTail - (uint8_t*)psConntCtrl->sHttpInfo.pchUrl);
			}
			else if (NULL != mem_search(psConntCtrl->sHttpInfo.pui8Line, sizeof("PUT ")-1, "PUT ", sizeof("PUT ")-1))
			{
				psConntCtrl->sHttpInfo.ui8Method  = MUI_HTTP_METHOD_PUT;
				psConntCtrl->sHttpInfo.pchUrl	  = (char*)(psConntCtrl->sHttpInfo.pui8Line + sizeof("PUT ") - 1);
				psConntCtrl->sHttpInfo.ui16UrlLen = (uint16_t)(pui8UrlTail - (uint8_t*)psConntCtrl->sHttpInfo.pchUrl);
			}
			else if (NULL != mem_search(psConntCtrl->sHttpInfo.pui8Line, sizeof("DELETE ")-1, "DELETE ", sizeof("DELETE ")-1))
			{
				psConntCtrl->sHttpInfo.ui8Method  = MUI_HTTP_METHOD_DELETE;
				psConntCtrl->sHttpInfo.pchUrl	  = (char*)(psConntCtrl->sHttpInfo.pui8Line + sizeof("DELETE ") - 1);
				psConntCtrl->sHttpInfo.ui16UrlLen = (uint16_t)(pui8UrlTail - (uint8_t*)psConntCtrl->sHttpInfo.pchUrl);
			}
			else
			{
				return psConntCtrl->sHttpInfo.ui8Schedule;
			}
			psConntCtrl->sHttpInfo.ui8Schedule = MUI_HTTP_DEAL_RECVED_LINE;
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Have parsed request line\r\n");
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]------------------------------------------\r\n");
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Method:%d\r\n", psConntCtrl->sHttpInfo.ui8Method);
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]URL:");
			//NET_CORTEX_M4_PUTS(psConntCtrl->sHttpInfo.pchUrl, (size_t)psConntCtrl->sHttpInfo.pui8Head-(size_t)psConntCtrl->sHttpInfo.pchUrl-sizeof("HTTP/1.1\r\n"));
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

 		case MUI_HTTP_DEAL_RECVED_LINE:
			pui8Tmp = mem_search(psConntCtrl->sHttpInfo.pui8Head, psConntCtrl->tLen-(psConntCtrl->sHttpInfo.pui8Head-psConntCtrl->sHttpInfo.pui8Line), "\r\n\r\n", sizeof("\r\n\r\n")-1);
			if (NULL == pui8Tmp)
			{
				return psConntCtrl->sHttpInfo.ui8Schedule;
			}
			psConntCtrl->sHttpInfo.pui8Body = pui8Tmp + sizeof("\r\n\r\n") - 1;
			//Identify Content-Length
			pui8Tmp = mem_search(psConntCtrl->sHttpInfo.pui8Head, pui8Tmp-psConntCtrl->sHttpInfo.pui8Head, "Content-Length:", sizeof("Content-Length:")-1);
			if (NULL != pui8Tmp)
			{
				pui8Tmp += (sizeof("Content-Length:") - 1);
				lTmp = atol((char*)pui8Tmp);
				if (lTmp < 0)
				{
					//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Got wrong Content-Length:%ld\r\n", lTmp);
					return MUI_HTTP_DEAL_PARSE_ERR;
				}
				else if (0 == lTmp)
				{
					psConntCtrl->sHttpInfo.ui8Schedule = MUI_HTTP_DEAL_RECVED_BODY;
					//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Have parsed request head\r\n");
					//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]This HTTP message does not include body.\r\n");
					return psConntCtrl->sHttpInfo.ui8Schedule;
				}
				else
				{
					psConntCtrl->sHttpInfo.ui32BodyLen = (uint32_t)lTmp;
					psConntCtrl->sHttpInfo.ui8Schedule = MUI_HTTP_DEAL_RECVED_HEAD;
				}
			}
			else
			{
				psConntCtrl->sHttpInfo.ui8Schedule = MUI_HTTP_DEAL_RECVED_BODY;
				//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Have parsed request head\r\n");
				//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]This HTTP message does not include body.\r\n");
				return psConntCtrl->sHttpInfo.ui8Schedule;
			}
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Have parsed request head\r\n");
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]------------------------------------------\r\n");
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Content-Length:%u\r\n", psConntCtrl->sHttpInfo.ui32BodyLen);

 		case MUI_HTTP_DEAL_RECVED_HEAD:
			if (psConntCtrl->sHttpInfo.ui32BodyLen <= (psConntCtrl->tLen-(psConntCtrl->sHttpInfo.pui8Body-psConntCtrl->pui8Buf)))
			{
				psConntCtrl->sHttpInfo.ui8Schedule = MUI_HTTP_DEAL_RECVED_BODY;
			}
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Have parsed request body\r\n");
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]------------------------------------------\r\n");
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Body:\r\n");
			//NET_CORTEX_M4_PUTS((char*)psConntCtrl->sHttpInfo.pui8Body, psConntCtrl->sHttpInfo.ui32BodyLen);
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

			return psConntCtrl->sHttpInfo.ui8Schedule;

 		default:	//MUI_HTTP_DEAL_RECVED_BODY:
			//NET_CORTEX_M4_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[net_http_message_recv]Input a wrong schedule parameter.\n");
			return MUI_HTTP_DEAL_PARSE_ERR;
	}
}
//-------------------------------------------------------------------------------------------------*

