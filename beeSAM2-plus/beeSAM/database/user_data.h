//#define	__M_FILE_NAME_user_data_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of user_data module for beeSAM project.
File name:user_data.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_user_data_H__
#define	__M_FILE_NAME_user_data_H__

#include "common.h"
#include "data_common.h"
#include "file_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int user_data_ram_read (size_t tOffset, void *pvBuf, size_t tSize);
extern int user_data_ram_write (size_t tOffset, const void *pvBuf, size_t tSize);
extern int user_data_rom_read (size_t tOffset, void *pvBuf, size_t tSize);
extern int user_data_rom_write (size_t tOffset, const void *pvBuf, size_t tSize);
extern int user_data_ram_rom_write (size_t tRamOffset, const void *pvRamBuf, size_t tRamSize, size_t tRomOffset, const void *pvRomBuf, size_t tRomSize);
extern int user_data_total_get (void);
extern int user_data_get_by_idx (uint16_t ui16Idx, S_UserData *psUserData);
extern int user_data_verify (const char *pchUserName, size_t tLen);
extern int user_data_add (const char *pchUserName, size_t tUserNameLen, const char *pchUserDevName, size_t tUserDevNameLen);
extern int user_data_del (const char *pchUserName, size_t tLen);
extern int user_data_del_all (void);
extern int user_data_init (void);
extern int user_data_test (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_user_data_H__".
