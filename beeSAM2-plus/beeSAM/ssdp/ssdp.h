//#define	__M_FILE_NAME_ssdp_H__	//���α�ͷ�ļ�
#ifndef	__M_FILE_NAME_ssdp_H__
#define	__M_FILE_NAME_ssdp_H__

#include "common.h"
#include "net_manage.h"
#include "sys_time.h"
//#include "api_deal.h"

#define MSTR_SSDP_MULTICAST_IP				"239.255.255.250"
#define MUI_SSDP_MULTICAST_PORT				1900

#define MUI_SSDP_SELECT_WAIT_S				1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_SSDP_FEATUE
extern	int ssdp_init (void);	//SSDP��ʼ������.
extern	int ssdp_quit_wait (void);		//SSDP�˳��ȴ�����
#else
#define	ssdp_init()			(0)
#define ssdp_quit_wait()	(0)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_ssdp_H__".
