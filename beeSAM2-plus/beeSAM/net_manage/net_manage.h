//#define	__M_FILE_NAME_net_manage_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_net_manage_H__
#define	__M_FILE_NAME_net_manage_H__

#include "common.h"
#include "net_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//#define M_NET_MANAGE_DBG_EXTERN

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_HTTP_DEAL_WAIT_MESSAGE		0		//等待接收报文
#define MUI_HTTP_DEAL_RECVED_LINE		1		//已接收请求行
#define MUI_HTTP_DEAL_RECVED_HEAD		2		//已接收请求头
#define MUI_HTTP_DEAL_RECVED_BODY		3		//已接收请求体
#define MUI_HTTP_DEAL_PARSE_ERR			255		//分析出现致命错误

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int net_http_message_recv(S_ConntCtrl *psConntCtrl, S_HttpArg *pArg);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_net_manage_H__".
