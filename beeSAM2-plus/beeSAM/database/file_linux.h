//#define	__M_FILE_NAME_file_linux_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of at_port module for beeSAM project.
File name:at_port.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_file_linux_H__
#define	__M_FILE_NAME_file_linux_H__

#include "common.h"
#include "data_common.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_FILE_ABS_PATH_LEN			256

#define MUI_DATA_DIR_NAME_LEN_MAX		32
#define MUI_DATA_FILE_NAME_LEN_MAX		32

#define MUI_DATA_CREAT_MODE				0777

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	uint8_t				ui8Len;		//数据路径实际长度(这个数据时为了加快产生文件路径而定义的)
	char				achPath[MUI_FILE_ABS_PATH_LEN];
} S_DirAbsPath;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int mk_multi_dir (const char *pchPath, mode_t tMode);
extern int del_empty_dir (const char *pchPath);
extern int del_dir (const char *pchPath);
extern int file_gen_abs_path (const char *pchPath, mode_t tMode, char *pchAbsPath, size_t tSize);

extern int chandler_file_size_get (const char *pchFileName);
extern int file_read_by_fd (int iFd, size_t tOffset, void *pvBuf, size_t tSize);
extern int file_write_by_fd (int iFd, size_t tOffset, const void *pvBuf, size_t tLen);
extern int file_read (const char *pchFileName, size_t tOffset, void *pvBuf, size_t tSize);
extern int file_write (const char *pchFileName, size_t tOffset, const void *pvBuf, size_t tLen);
extern int file_del (const char *pchFileName);
extern int file_cp (const char *pchDestFile, const char *pchSrcFile);
extern int file_rename (const char *pchDestFile, const char *pchSrcFile);
extern int file_name_swap (const char *pchFile1, const char *pchFile2);
extern int file_exclusive_read (const char *pchFileName, size_t tOffset, void *pvBuf, size_t tSize, sem_t *ptMutx, uint32_t ui32WaitMs);
extern int file_exclusive_write (const char *pchFileName, size_t tOffset, const void *pvBuf, size_t tLen, sem_t *ptMutx, uint32_t ui32WaitMs);

extern int data_idx_cmp (const void *pv1, const void *pv2);
extern int data_id_cmp (const void *pv1, const void *pv2);
extern int data_num_max_get_by_type (uint16_t ui16Type);
extern int data_root_path_cfg (const char *pchPath);
extern const char* data_root_path_get (void);
extern int data_root_path_len_get (void);
extern const char* data_factory_file_path_get (char *pchPath, size_t tSize);
extern const char* data_dev_state_backup_file_path_get (char *pchPath, size_t tSize);
extern const char* data_dir_path_get_by_type (uint16_t ui16Type, char *pchPath, size_t tSize);
extern const char* data_file_path_get (uint16_t ui16Type, uint16_t ui16Id, char *pchPath, size_t tSize);
extern const char* data_extend_file_path_get (uint16_t ui16Type, uint16_t ui16Id, char *pchPath, size_t tSize);
extern int data_id_scan_by_type (uint16_t ui16Type, uint16_t *pui16Buf, uint16_t ui16Capacity);
extern int data_id_exclusive_scan_by_type (uint16_t ui16Type, uint16_t *pui16Buf, uint16_t ui16Capacity, sem_t *ptMutx, uint32_t ui32WaitMs);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_file_linux_H__".
