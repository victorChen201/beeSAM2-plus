//#define	__M_FILE_NAME_dev_data_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of dev_data module for beeSAM project.
File name:dev_data.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_dev_data_H__
#define	__M_FILE_NAME_dev_data_H__

#include "common.h"
#include "data_common.h"
#include "file_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int dev_data_total_get (void);
extern int dev_data_idx_list_get (uint32_t *pui32Buf, uint16_t ui16Capacity);
extern int dev_data_num_get_by_type (uint16_t ui16Type);
extern int dev_data_id_list_get_by_type (uint16_t ui16Type, uint16_t *pui16Buf, uint16_t ui16Capacity);
extern int dev_data_assign_id_by_type (uint16_t ui16Type);
extern int dev_data_ram_read (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize);
extern int dev_data_ram_write (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize);
extern int dev_data_rom_read (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize);
extern int dev_data_rom_write (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize);
extern int dev_data_ram_rom_write (uint16_t ui16Type, uint16_t ui16Id, size_t tRomOffset, const void *pvRomBuf, size_t tRomSize, size_t tRamOffset, const void *pvRamBuf, size_t tRamSize);
extern int dev_data_add (uint16_t ui16Type, const void *pvRomBuf, size_t tRomSize, const void *pvRamBuf, size_t tRamSize);
extern int dev_data_del (uint16_t ui16Type, uint16_t ui16Id);
extern int dev_data_del_by_type (uint16_t ui16Type);
extern int dev_data_del_all (void);
extern int dev_data_swap_id (uint16_t ui16Type, uint16_t ui16DestId, uint16_t ui16SrcId);
extern int dev_data_set_new_id (uint16_t ui16Type, uint16_t ui16NewId, uint16_t ui16Id);
extern int dev_data_state_sync (void);
extern int dev_data_init (void);
extern int dev_data_test (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_dev_data_H__".
