/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of sys_time_linux module for beeSAM project.
File name: sys_time_linux.c
Description:

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "sys_time_linux.h"
#include "sys_time.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SYS_TIME_LINUX_DBG

#ifndef SYS_TIME_LINUX_PUTC
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_PUTC(Char)			PUTC(Char)
#else
#define SYS_TIME_LINUX_PUTC(Char)
#endif
#endif

#ifndef SYS_TIME_LINUX_PUTS
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SYS_TIME_LINUX_PUTS(Str, Len)
#endif
#endif

#ifndef SYS_TIME_LINUX_PUTASCC
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_PUTASCC(Char)			PUTASCC(Char)
#else
#define SYS_TIME_LINUX_PUTASCC(Char)
#endif
#endif

#ifndef SYS_TIME_LINUX_PUTASCS
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SYS_TIME_LINUX_PUTASCS(Str, Len)
#endif
#endif

#ifndef SYS_TIME_LINUX_PRINT
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SYS_TIME_LINUX_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SYS_TIME_LINUX_DYN_PUTC
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SYS_TIME_LINUX|Dbg), Char)
#else
#define SYS_TIME_LINUX_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SYS_TIME_LINUX_DYN_PUTS
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SYS_TIME_LINUX|Dbg), Str, Len)
#else
#define SYS_TIME_LINUX_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SYS_TIME_LINUX_DYN_PUTASCC
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SYS_TIME_LINUX|Dbg), Char)
#else
#define SYS_TIME_LINUX_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SYS_TIME_LINUX_DYN_PUTASCS
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SYS_TIME_LINUX|Dbg), Str, Len)
#else
#define SYS_TIME_LINUX_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SYS_TIME_LINUX_DYN_PRINT
#ifdef SYS_TIME_LINUX_DBG
#define SYS_TIME_LINUX_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SYS_TIME_LINUX|Dbg), Fmt, ##Arg)
#else
#define SYS_TIME_LINUX_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#if 0
//-------------------------------------------------------------------------------------------------*
/*	socket初始化函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:	 <0:出错(包括异常及等待超时)
;				>=0:正常完成
;	(注意)
*/
static int socket_init (void)
{
	int i;

	s_iSocketRcvFd  = -1;
	s_iSocketSendFd = -1;

	memset(s_asConntCtrl, 0, sizeof(s_asConntCtrl));
	for (i=0; i<MUI_ALLOW_CONNECT_MAX; i++)
	{
		s_asConntCtrl[i].iConntFd = -1;
	}

	socket_close();

	return 0;
}
//-------------------------------------------------------------------------------------------------*
#endif

uint32_t sys_get_ms(void)
{
	uint32_t ui32Ms;

	struct timeval tv;
	gettimeofday(&tv,NULL);
	ui32Ms = (uint32_t)tv.tv_sec;

	return (ui32Ms*1000) + (uint32_t)(tv.tv_usec/1000);
}

void get_sys_time (S_SysTime *psTime)
{
	time_t tNow;
	struct tm sTm;

	time(&tNow);
	localtime_r(&tNow, &sTm);

	psTime->m_sec  = (uint8_t)sTm.tm_sec;
	psTime->m_min  = (uint8_t)sTm.tm_min;
	psTime->m_hur  = (uint8_t)sTm.tm_hour;
	psTime->m_mday = (uint8_t)sTm.tm_mday;
	psTime->m_wday = (uint8_t)sTm.tm_wday;
	psTime->m_mon  = (uint8_t)sTm.tm_mon;
	psTime->m_year = (uint8_t)sTm.tm_year + 1900;
}

void update_sys_time (const S_SysTime *psTime)
{
	UNUSED(psTime);
	char achtimebuf[40];
	sprintf(achtimebuf,"date -s \"%hu-%hhu-%hhu %hhu:%hhu:%hhu\"",psTime->m_year,psTime->m_mon,psTime->m_mday,psTime->m_hur,psTime->m_min,psTime->m_sec);
	//printf("############[%s]####################\r\n",achtimebuf);
	system(achtimebuf);
}

void msleep (uint32_t ui32Ms)
{
	usleep(ui32Ms * 1000);
}

