//#define	__M_FILE_NAME_remote_net_manage_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of at_port module for beeSAM project.
File name:remote_net_manage.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_remote_net_manage_H__
#define	__M_FILE_NAME_remote_net_manage_H__

#include "common.h"
#include "wsclient.h"
#include "sys_time.h"
#include "database.h"
#include "net_manage.h"

#ifdef M_REMOTE_COMMUNICATION
extern int remote_net_manage_init (uint8_t ui8BeatInterval);
extern int remote_net_send_data (const char *pchData, size_t tLen);
extern void remote_net_quit_wait (void);
#else
#define remote_net_manage_init(_ui8BeatInterval)	(void)(0)
#define remote_net_send_data(_pchData, _tLen)		(void)(0)
#define remote_net_quit_wait()						(void)(0)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_remote_net_manage_H__".

