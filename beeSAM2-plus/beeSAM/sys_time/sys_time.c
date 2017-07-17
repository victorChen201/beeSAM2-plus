/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of sys_time module for beeSAM project.
File name:sys_time.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "sys_time.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SYS_TIME_DBG

#ifndef SYS_TIME_PUTC
#ifdef SYS_TIME_DBG
#define SYS_TIME_PUTC(Char)			PUTC(Char)
#else
#define SYS_TIME_PUTC(Char)
#endif
#endif

#ifndef SYS_TIME_PUTS
#ifdef SYS_TIME_DBG
#define SYS_TIME_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SYS_TIME_PUTS(Str, Len)
#endif
#endif

#ifndef SYS_TIME_PUTASCC
#ifdef SYS_TIME_DBG
#define SYS_TIME_PUTASCC(Char)			PUTASCC(Char)
#else
#define SYS_TIME_PUTASCC(Char)
#endif
#endif

#ifndef SYS_TIME_PUTASCS
#ifdef SYS_TIME_DBG
#define SYS_TIME_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SYS_TIME_PUTASCS(Str, Len)
#endif
#endif

#ifndef SYS_TIME_PRINT
#ifdef SYS_TIME_DBG
#define SYS_TIME_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SYS_TIME_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SYS_TIME_DYN_PUTC
#ifdef SYS_TIME_DBG
#define SYS_TIME_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SYS_TIME|Dbg), Char)
#else
#define SYS_TIME_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SYS_TIME_DYN_PUTS
#ifdef SYS_TIME_DBG
#define SYS_TIME_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SYS_TIME|Dbg), Str, Len)
#else
#define SYS_TIME_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SYS_TIME_DYN_PUTASCC
#ifdef SYS_TIME_DBG
#define SYS_TIME_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SYS_TIME|Dbg), Char)
#else
#define SYS_TIME_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SYS_TIME_DYN_PUTASCS
#ifdef SYS_TIME_DBG
#define SYS_TIME_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SYS_TIME|Dbg), Str, Len)
#else
#define SYS_TIME_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SYS_TIME_DYN_PRINT
#ifdef SYS_TIME_DBG
#define SYS_TIME_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SYS_TIME|Dbg), Fmt, ##Arg)
#else
#define SYS_TIME_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static const uint8_t s_aui81stDayOfMonth[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

//-------------------------------------------------------------------------------------------------*
/*	根据日期计算周日期(即星期几)函数
;	(功能).
;	(参数)
;		sTime:	指向日期结构体的指针
;	(返回)
;		int:	周日期(即星期几)[]
;	(注意)
*/

int cacl_day_by_date (const S_SysTime* psTime)
{
	uint16_t ui16Y = psTime->m_wday;

	ui16Y -= (psTime->m_mon < 3);

	return (int)((ui16Y + ui16Y/4 - ui16Y/100 + ui16Y/400 + s_aui81stDayOfMonth[psTime->m_mon-1] + psTime->m_mday) % 7);
}
//-------------------------------------------------------------------------------------------------*
