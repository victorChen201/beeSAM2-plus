//#define	__M_FILE_NAME_net_linux_H__	//���α�ͷ�ļ�
#ifndef	__M_FILE_NAME_net_linux_H__
#define	__M_FILE_NAME_net_linux_H__

#include "common.h"

#include<sys/ioctl.h>
#include<sys/stat.h>
#include<sys/socket.h>

#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<net/if.h>
#include<fcntl.h>
#include<net/if_arp.h>
#include<net/route.h>
#include<linux/rtnetlink.h>



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define M_NET_TICK_MACRO

#define MUI_NET_SEND_IMMEDIATELY		0x02
#define MUI_NET_SEND_NOT_FINISH			0x01

#define MUI_ALLOW_CONNECT_MAX			10
#define MUI_API_RECV_BUF_SIZE			(1024*32)
#define MUI_API_ANSWER_BUF_SIZE			(1024*64)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//Socket����
typedef struct
{
	int						iSocketFd;		//Socket�ļ�������
	struct sockaddr_in		sSocketAdd;		//Socket��ַ����
	socklen_t				tSockLen;		//Socket��ַ����
} S_SocketCfg;
//����ӿ���Ϣ
typedef struct
{
	char s_achListenIP[16];
	uint16_t s_ui16ListenPort;
	char s_achReportIP[16];
	uint16_t s_ui16ReportPort;

	char s_achNetMask[16];
	char s_achGatewayIP[16];
	//struct in_addr			sIpAddr;		/*IP��ַ*/
	//char ip[16];
	//char netmask[16];						/* ����*/
	//char gateway[16];						/* ����*/
	//char broadcast[16];						/* �㲥�س�*/
	//char devname[32];						/* �豸*/
	char mac[32];							/* MAC�س�*/
} S_SockAddr;

//HTTP��Ϣ
typedef struct
{
	uint8_t				ui8Schedule;	//�������
	uint8_t				ui8Method;		//����
	char				*pchUrl;		//URL
	uint8_t				*pui8Line;		//Line������
	uint8_t				*pui8Head;		//Head������
	uint8_t				*pui8Body;		//Body������
	uint16_t			ui16UrlLen;		//URL����
	uint32_t			ui32BodyLen;	//Body����
} S_HttpInfo;

//���ӿ���
typedef struct
{
	int					iConntFd;		/*����������*/
	time_t				tTicks;			/*��¼�ĵδ���*/
	uint8_t				*pui8Buf;		/*���Ļ�����*/
	size_t				tLen;			//�ܱ��ĳ���(���segment�ܺ�)

	S_HttpInfo			sHttpInfo;		//HTTP��Ϣ
} S_ConntCtrl;

typedef struct
{
	size_t				tLen;			/* ���ĳ��� */
} S_HttpArg;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//extern	int net_manage_init (const char *pchListenIP, uint16_t ui16ListenPort, const char *pchReportIP, uint16_t ui16ReportPort);
extern	int net_manage_init (S_RunPara* netPara);
extern	int net_manage_quit_wait (void);
extern	int net_event_report (const void *pvBuf, size_t tLen);
extern int api_answer_send (S_ConntCtrl *psConntCtrl, const void *pvBuf, int tLen);
extern int api_answer_segment_send (S_ConntCtrl *psConntCtrl, const void *pvBuf, size_t tLen, uint8_t ui8Flg);

extern	const char* net_listen_ip_addr_get (void);
extern	uint16_t net_listen_port_get (void);
extern	const char* net_report_ip_addr_get (void);
extern	uint16_t net_report_port_get (void);
extern const char* net_mask_get (void);
extern const char* net_gw_ip_addr_get (void);
extern const char* net_mac_addr_get (void);

extern void net_manage_clean (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//extern const char* net_listen_ip_addr_get (void);
//extern uint16_t net_listen_port_get (void);
//extern const char* net_report_ip_addr_get (void);
//extern uint16_t net_report_port_get (void);

extern int if_info_get (const char *pchIfName, S_SockAddr *psIfInfo);
extern int get_gateway(char *gateway, char *ip);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_net_linux_H__".
