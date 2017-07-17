//#define	__M_FILE_NAME_sys_time_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_sys_time_H__
#define	__M_FILE_NAME_sys_time_H__

#include "common.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct _SysTime
{
	uint16_t m_year;
	uint8_t m_mon;
	uint8_t m_mday;
	uint8_t m_wday;
	uint8_t m_hur;
	uint8_t m_min;
	uint8_t m_sec;
} S_SysTime;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#include "sys_time_linux.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

extern int cacl_day_by_date (const S_SysTime* psTime);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_sys_time_H__".
