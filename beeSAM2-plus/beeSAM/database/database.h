//#define	__M_FILE_NAME_database_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of database module for beeSAM project.
File name:database.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_database_H__
#define	__M_FILE_NAME_database_H__

#include "common.h"
#include "data_common.h"
#include "file_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int read_factory_info (S_FactoryInfo *psFactoryInfo);
extern int read_factory_info_sn (S_FactorySn *psFactorySn);
extern int read_factory_info_mac (S_StrMacAddr *psFactoryMac);
extern int read_factory_info_wifi_mac (S_StrMacAddr *psFactoryWifiMac);
//................................................
extern int write_factory_info_sn (const S_FactorySn *psFactorySn);
extern int write_factory_info_mac (const S_StrMacAddr *psFactoryMac);
extern int write_factory_info_wifimac (const S_StrMacAddr *psFactoryWifiMac);

extern int factory_file_init (void);

extern int database_init (void);
extern int database_read (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize, uint8_t ui8Mode);
extern int database_write (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize, uint8_t ui8Mode);
extern int database_multi_write (uint16_t ui16Type, uint16_t ui16Id, size_t tRamOffset, const void *pvRamBuf, size_t tRamSize, size_t tRomOffset, const void *pvRomBuf, size_t tRomSize, uint8_t ui8Mode);
extern int database_del_all (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_database_H__".

