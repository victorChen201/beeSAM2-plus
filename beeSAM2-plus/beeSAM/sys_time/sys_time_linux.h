//#define	__M_FILE_NAME_sys_time_manage_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_sys_time_manage_H__
#define	__M_FILE_NAME_sys_time_manage_H__

#include "common.h"
#include "sys_time.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern uint32_t sys_get_ms (void);
extern void get_sys_time (S_SysTime *psTime);
extern void update_sys_time (const S_SysTime *psTime);
extern void msleep (uint32_t ui32Ms);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_sys_time_manage_H__".
