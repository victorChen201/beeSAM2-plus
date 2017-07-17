//#define	__M_FILE_NAME_net_linux_H__	//屏蔽本头文件
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
//Socket配置
typedef struct
{
	int						iSocketFd;		//Socket文件描述符
	struct sockaddr_in		sSocketAdd;		//Socket地址描述
	socklen_t				tSockLen;		//Socket地址长度
} S_SocketCfg;
//网络接口信息
typedef struct
{
	char s_achListenIP[16];
	uint16_t s_ui16ListenPort;
	char s_achReportIP[16];
	uint16_t s_ui16ReportPort;

	char s_achNetMask[16];
	char s_achGatewayIP[16];
	//struct in_addr			sIpAddr;		/*IP地址*/
	//char ip[16];
	//char netmask[16];						/* 掩码*/
	//char gateway[16];						/* 网关*/
	//char broadcast[16];						/* 广播地扯*/
	//char devname[32];						/* 设备*/
	char mac[32];							/* MAC地扯*/
} S_SockAddr;

//HTTP信息
typedef struct
{
	uint8_t				ui8Schedule;	//处理进度
	uint8_t				ui8Method;		//方法
	char				*pchUrl;		//URL
	uint8_t				*pui8Line;		//Line缓冲区
	uint8_t				*pui8Head;		//Head缓冲区
	uint8_t				*pui8Body;		//Body缓冲区
	uint16_t			ui16UrlLen;		//URL长度
	uint32_t			ui32BodyLen;	//Body长度
} S_HttpInfo;

//连接控制
typedef struct
{
	int					iConntFd;		/*连接描述符*/
	time_t				tTicks;			/*记录的滴答数*/
	uint8_t				*pui8Buf;		/*报文缓冲区*/
	size_t				tLen;			//总报文长度(多个segment总和)

	S_HttpInfo			sHttpInfo;		//HTTP信息
} S_ConntCtrl;

typedef struct
{
	size_t				tLen;			/* 报文长度 */
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
