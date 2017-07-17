//#define	__M_FILE_NAME_at_port_linux_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of at_port module for beeSAM project.
File name:at_port_linux.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_at_port_linux_H__
#define	__M_FILE_NAME_at_port_linux_H__

#include "common.h"
#include "ring_buffer.h"

extern int at_cmd_sys_init (const char *pchDev);

extern int at_port_reset (void);
extern int at_port_state_set (uint8_t ui8State);
extern int at_port_state_get (void);

extern int at_port_wr (const void *pvBuf, size_t tLen);
extern int at_cmd_buf_wr(const void *pvCmd, size_t tCmdLen);
extern int at_cmd_query (const void *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs);
extern int at_cmd_answer_verify (const void *pvAnswer, size_t tAnswerLen);

extern int at_cmd_sys_quit_wait (void);
extern void at_port_clean (void);

extern int at_cmd_to_easy_mode(const uint8_t ui8);
extern int get_easy_mode();

extern int at_port_block_wr (void *pvBuf, size_t tLen, uint32_t ui32WaitMs);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_at_port_linux_H__".
