//#define	__M_FILE_NAME_config_data_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of config_data module for beeSAM project.
File name:config_data.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_config_data_H__
#define	__M_FILE_NAME_config_data_H__

#include "common.h"
#include "data_common.h"
#include "file_linux.h"

extern int config_data_ram_read (size_t tOffset, void *pvBuf, size_t tSize);
extern int config_data_ram_write (size_t tOffset, const void *pvBuf, size_t tSize);
extern int config_data_rom_read (size_t tOffset, void *pvBuf, size_t tSize);
extern int config_data_rom_write (size_t tOffset, const void *pvBuf, size_t tSize);
extern int config_data_ram_rom_write (size_t tRamOffset, const void *pvRamBuf, size_t tRamSize, size_t tRomOffset, const void *pvRomBuf, size_t tRomSize);
extern int config_data_init (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_config_data_H__".
