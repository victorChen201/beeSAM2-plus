//#define	__M_FILE_NAME_extend_data_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of extend_data module for beeSAM project.
File name:extend_data.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_extend_data_H__
#define	__M_FILE_NAME_extend_data_H__

#include "common.h"
#include "data_common.h"
#include "file_linux.h"

typedef struct
{
	uint16_t	ui16Type;
	uint16_t	ui16Id;
	uint32_t	ui32ExtId;
} S_ExtendIdIdx;

extern int extend_data_ext_id_add (uint16_t ui16Type, uint16_t ui16Id, const void *pvBuf, size_t tLen, uint32_t *pui32ExtId);
extern int extend_data_ext_id_del (uint16_t ui16Type, uint16_t ui16Id, uint32_t ui32ExtId);
extern int extend_data_ext_id_read (uint16_t ui16Type, uint16_t ui16Id, uint32_t ui32ExtId, void *pvBuf, size_t tSize);
extern int extend_data_del (uint16_t ui16Type, uint16_t ui16Id);
extern int extend_data_init (void);
extern int extend_data_test (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_extend_data_H__".
