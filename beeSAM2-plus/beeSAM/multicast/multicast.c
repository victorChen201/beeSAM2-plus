/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of multicast module for beeSAM project.
File name:multicast.c
Description��

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/


#include "multicast.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MULTICAST_DBG

#ifndef MULTICAST_PUTC
#ifdef MULTICAST_DBG
#define MULTICAST_PUTC(Char)			PUTC(Char)
#else
#define MULTICAST_PUTC(Char)
#endif
#endif

#ifndef MULTICAST_PUTS
#ifdef MULTICAST_DBG
#define MULTICAST_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define MULTICAST_PUTS(Str, Len)
#endif
#endif

#ifndef MULTICAST_PUTASCC
#ifdef MULTICAST_DBG
#define MULTICAST_PUTASCC(Char)			PUTASCC(Char)
#else
#define MULTICAST_PUTASCC(Char)
#endif
#endif

#ifndef MULTICAST_PUTASCS
#ifdef MULTICAST_DBG
#define MULTICAST_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define MULTICAST_PUTASCS(Str, Len)
#endif
#endif

#ifndef MULTICAST_PRINT
#ifdef MULTICAST_DBG
#define MULTICAST_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define MULTICAST_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef MULTICAST_DYN_PUTC
#ifdef MULTICAST_DBG
#define MULTICAST_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_MULTICAST|Dbg), Char)
#else
#define MULTICAST_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef MULTICAST_DYN_PUTS
#ifdef MULTICAST_DBG
#define MULTICAST_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_MULTICAST|Dbg), Str, Len)
#else
#define MULTICAST_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef MULTICAST_DYN_PUTASCC
#ifdef MULTICAST_DBG
#define MULTICAST_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_MULTICAST|Dbg), Char)
#else
#define MULTICAST_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef MULTICAST_DYN_PUTASCS
#ifdef MULTICAST_DBG
#define MULTICAST_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_MULTICAST|Dbg), Str, Len)
#else
#define MULTICAST_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef MULTICAST_DYN_PRINT
#ifdef MULTICAST_DBG
#define MULTICAST_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_MULTICAST|Dbg), Fmt, ##Arg)
#else
#define MULTICAST_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static S_SocketCfg s_sMulticastSocketCfg;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int multicast_establish_link (void);

//-------------------------------------------------------------------------------------------------*
/*	multicast_establish_link(�򵥷�����Э��)�ȴ���ѯ�̺߳���
;	(����)�ȴ�MULTICAST����.
;	(����)
;		pArg:	����ָ��
;	(����)
;		void*	����ָ��
;	(ע��)
*/
static int multicast_establish_link (void)
{
	struct sockaddr_in sLocalAddr;
	struct ip_mreq sMreq;
	int iFd, iRet, iTmp;

	//Creat socket for UDP
	iFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (iFd < 0)
	{
		printf("multicast_establish_link: Creat the socket fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	MULTICAST_DYN_PRINT(M_DBG_LEVEL_DEBUG, "multicast_establish_link: Creat socket successful!\r\n");

	//Socket:�����õ�ַ
	iTmp = 1;
	iRet = setsockopt(iFd, SOL_SOCKET, SO_REUSEADDR, &iTmp, sizeof(iTmp));
	if (iRet < 0)
	{
		printf("multicast_establish_link: Set reuse address fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	MULTICAST_DYN_PRINT(M_DBG_LEVEL_DEBUG, "multicast_establish_link: Set reuse address successful!\r\n");
#if 1
	//�ಥ����:TTLֵ
	iTmp = 10;	//���ת��10��
	iRet = setsockopt(iFd, IPPROTO_IP, IP_MULTICAST_TTL, &iTmp, sizeof(iTmp));
	if (iRet < 0)
	{
		printf("multicast_establish_link: Set multicast TTL fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	MULTICAST_DYN_PRINT(M_DBG_LEVEL_DEBUG, "multicast_establish_link: Set multicast TTL successful!\r\n");

	//�ಥ����:LOOP����(�Ƿ����)
	iTmp = 1;	//ʹ�ܱ��ػ���
	iRet = setsockopt(iFd, IPPROTO_IP, IP_MULTICAST_LOOP, &iTmp, sizeof(iTmp));
	if (iRet < 0)
	{
		printf("multicast_establish_link: Set multicast LOOP fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	MULTICAST_DYN_PRINT(M_DBG_LEVEL_DEBUG, "multicast_establish_link: Set multicast LOOP successful!\r\n");
#endif

	//��Socket��ַ
	//Set socket address
	bzero(&sLocalAddr, sizeof(sLocalAddr));
	sLocalAddr.sin_family	   = AF_INET;
	sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sLocalAddr.sin_port		   = htons(MUI_MULTICAST_SRC_PORT);
	iRet = bind(iFd, (struct sockaddr *)&sLocalAddr, sizeof(struct sockaddr));
	if (iRet < 0)
	{
		printf("multicast_establish_link: Bind the socket address fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	MULTICAST_DYN_PRINT(M_DBG_LEVEL_DEBUG, "multicast_establish_link: Bind LocalAddr successful!\r\n");
/*
	//�����ص�ַ����ಥ��
	sMreq.imr_multiaddr.s_addr = inet_addr(MSTR_MULTICAST_DEST_IP);
	sMreq.imr_interface.s_addr = inet_addr(net_listen_ip_addr_get());
	iRet = setsockopt(iFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &sMreq, sizeof(sMreq));
	if (iRet < 0)
	{
		printf("multicast_establish_link: Add multicast member fail, error number: '%d'.\r\n", errno);
		return -1;
	}
	MULTICAST_DYN_PRINT(M_DBG_LEVEL_DEBUG, "multicast_establish_link: Add multicast member successful!\r\n");
*/
	//����sSocketCfg
	bzero(&s_sMulticastSocketCfg, sizeof(s_sMulticastSocketCfg));
	s_sMulticastSocketCfg.iSocketFd = iFd;
	s_sMulticastSocketCfg.sSocketAdd.sin_family		 = AF_INET;
	s_sMulticastSocketCfg.sSocketAdd.sin_addr.s_addr = inet_addr(MSTR_MULTICAST_DEST_IP);
	s_sMulticastSocketCfg.sSocketAdd.sin_port		 = htons(MUI_MULTICAST_DEST_PORT);
	s_sMulticastSocketCfg.tSockLen					 = sizeof(struct sockaddr_in);

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	MULTICAST��ʼ������
;	(����)
;	(����)
;		pchSysInfo:	ϵͳ��Ϣָ��
;		pchUUID:	UUID��Ϣָ��
;	(����)
;		int:		-1:����(�����쳣���ȴ���ʱ)
;					 0:�������
;	(ע��)
*/
int multicast_init (void)
{
	//MULTICAST_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Will run multicast_init\r\n");

	int iRet;

	iRet = multicast_establish_link();
	if (iRet < 0)
	{
		MULTICAST_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[multicast_init]: Call 'multicast_establish_link' fail.\r\n");
		return iRet;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	MULTICAST NOTIFY alive���ͺ���
;	(����)
;	(����)
;		psDest:		Ҫ������Ϣ��Socket���ýṹ��Ϣ
;		eType:		֪ͨ����
;	(����)
;		int:		-1:����(�����쳣���ȴ���ʱ)
;					 0:�������
;	(ע��)
*/
int multicast_send (const void *pvBuf, size_t tLen)
{
	int iRet;

	// Start send message
	iRet = sendto(s_sMulticastSocketCfg.iSocketFd, pvBuf, tLen, 0, (struct sockaddr *)&s_sMulticastSocketCfg.sSocketAdd, s_sMulticastSocketCfg.tSockLen);
	if (iRet < 0)
	{
		MULTICAST_DYN_PRINT(M_DBG_LEVEL_ERR, "[multicast_send]: Call 'sendto(...)' fail, errno:%d\r\n", errno);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*
#if 0
int multicast_test (void)
{
	char a[]={"1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\r\n"
			  "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\r\n"
			  "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\r\n"
			  "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\r\n"
			  "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\r\n"
			  "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\r\n"
			 };

	multicast_send(a, sizeof(a));
}
#endif
