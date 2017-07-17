//#define	__M_FILE_NAME_net_manage_H__	//���α�ͷ�ļ�
#ifndef	__M_FILE_NAME_net_manage_H__
#define	__M_FILE_NAME_net_manage_H__

#include "common.h"
#include "net_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//#define M_NET_MANAGE_DBG_EXTERN

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_HTTP_DEAL_WAIT_MESSAGE		0		//�ȴ����ձ���
#define MUI_HTTP_DEAL_RECVED_LINE		1		//�ѽ���������
#define MUI_HTTP_DEAL_RECVED_HEAD		2		//�ѽ�������ͷ
#define MUI_HTTP_DEAL_RECVED_BODY		3		//�ѽ���������
#define MUI_HTTP_DEAL_PARSE_ERR			255		//����������������

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int net_http_message_recv(S_ConntCtrl *psConntCtrl, S_HttpArg *pArg);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_net_manage_H__".
