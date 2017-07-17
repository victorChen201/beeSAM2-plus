/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of config_data module for beeSAM project.
File name:config_data.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "config_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define CONFIG_DATA_DBG

#ifndef CONFIG_DATA_PUTC
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_PUTC(Char)			PUTC(Char)
#else
#define CONFIG_DATA_PUTC(Char)
#endif
#endif

#ifndef CONFIG_DATA_PUTS
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define CONFIG_DATA_PUTS(Str, Len)
#endif
#endif

#ifndef CONFIG_DATA_PUTASCC
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_PUTASCC(Char)			PUTASCC(Char)
#else
#define CONFIG_DATA_PUTASCC(Char)
#endif
#endif

#ifndef CONFIG_DATA_PUTASCS
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define CONFIG_DATA_PUTASCS(Str, Len)
#endif
#endif

#ifndef CONFIG_DATA_PRINT
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define CONFIG_DATA_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef CONFIG_DATA_DYN_PUTC
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_DYN_PUTC(Dbg, Char)		DYN_PUTC((M_DBG_MODULE_CONFIG_DATA|Dbg), Char)
#else
#define CONFIG_DATA_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef CONFIG_DATA_DYN_PUTS
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_DYN_PUTS(Dbg, Str, Len)	DYN_PUTS((M_DBG_MODULE_CONFIG_DATA|Dbg), Str, Len)
#else
#define CONFIG_DATA_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef CONFIG_DATA_DYN_PUTASCC
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_CONFIG_DATA|Dbg), Char)
#else
#define CONFIG_DATA_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef CONFIG_DATA_DYN_PUTASCS
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_CONFIG_DATA|Dbg), Str, Len)
#else
#define CONFIG_DATA_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef CONFIG_DATA_DYN_PRINT
#ifdef CONFIG_DATA_DBG
#define CONFIG_DATA_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_CONFIG_DATA|Dbg), Fmt, ##Arg)
#else
#define CONFIG_DATA_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_CONFIG_RAM_WAIT_TIME_MS_MAX	3000
#define MUI_CONFIG_ROM_WAIT_TIME_MS_MAX	3000

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	uint8_t				ui8Data;
} S_ConfigStateData;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static S_ConfigStateData s_sConfigStateData;
static sem_t s_tConfigRamMutex, s_tConfigRomMutex;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static void s_config_ram_show (void);

//-------------------------------------------------------------------------------------------------*
/*	设备RAM(数据表)显示函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		NULL
;	(注意)
*/
static void s_config_ram_show (void)
{
#if 0
	int i;

	CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_config_ram_show]s_sConfigStateData.ui16ConfigTotal=%u\r\n", s_sConfigStateData.ui16ConfigTotal);
	for (i=0; i<(sizeof(s_sConfigStateData.aui16ConfigTypeIdx)/sizeof(s_sConfigStateData.aui16ConfigTypeIdx[0])); i++)
	{
		CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[%d]%u ", i, s_sConfigStateData.aui16ConfigTypeIdx[i]);
	}
	CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	for (i=0; i<s_sConfigStateData.ui16ConfigTotal; i++)
	{
		CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%d:%08x\r\n", i, s_sConfigStateData.asRecord[i].tConfigIdx);
	}
	CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_config_ram_show]====================END====================\r\n");
#endif
}
















int config_data_ram_read (size_t tOffset, void *pvBuf, size_t tSize)
{
	UNUSED(tOffset);
	UNUSED(pvBuf);
	UNUSED(tSize);

	return 0;
}

int config_data_ram_write (size_t tOffset, const void *pvBuf, size_t tSize)
{
	UNUSED(tOffset);
	UNUSED(pvBuf);
	UNUSED(tSize);

	return 0;
}

int config_data_rom_read (size_t tOffset, void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(S_ConfigAttrData));	//不能超出数据的最大边界

	if (NULL == data_file_path_get(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
	{
		CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_data_rom_read]Can't get path of device %04hx:%04hx.\r\n", MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_read(achPath, tOffset, pvBuf, tSize, &s_tConfigRomMutex, MUI_CONFIG_ROM_WAIT_TIME_MS_MAX);
}

int config_data_rom_write (size_t tOffset, const void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(S_ConfigAttrData));	//不能超出数据的最大边界

	if (NULL == data_file_path_get(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
	{
		CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_data_rom_write]Can't get path of device %04hx:%04hx.\r\n", MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_write(achPath, tOffset, pvBuf, tSize, &s_tConfigRomMutex, MUI_CONFIG_ROM_WAIT_TIME_MS_MAX);
}

int config_data_ram_rom_write (size_t tRamOffset, const void *pvRamBuf, size_t tRamSize, size_t tRomOffset, const void *pvRomBuf, size_t tRomSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	ASSERT(NULL != pvRamBuf);
	ASSERT(NULL != pvRomBuf);
	ASSERT((tRamOffset+tRamSize) <= sizeof(S_ConfigStateData));	//不能超出数据的最大边界
	ASSERT((tRomOffset+tRomSize) <= sizeof(S_ConfigAttrData));		//不能超出数据的最大边界

	if (0 > sem_wait_ms(&s_tConfigRomMutex, MUI_CONFIG_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_CONFIG_ROM_WAIT_TIME_MS_MAX)
	{
		CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[config_data_ram_rom_write]Wait semaphore %u over time.\r\n", (uint32_t)&s_tConfigRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tConfigRamMutex, MUI_CONFIG_RAM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_CONFIG_RAM_WAIT_TIME_MS_MAX)
	{
		CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[config_data_ram_rom_write]Wait semaphore %u over time.\r\n", (uint32_t)&s_tConfigRamMutex);
		sem_post(&s_tConfigRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	do
	{
		if (NULL == data_file_path_get(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
		{
			CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_config_ram_rom_write]Can't get path of device %04hx:%04hx.\r\n", MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START);
			iRet = FILE_S_ADDR_ERR;
			break;
		}
		iRet = file_write(achPath, tRomOffset, pvRomBuf, tRomSize);
		if (iRet < 0)
		{
			break;
		}
		//Write data to RAM

	} while (0);

	sem_post(&s_tConfigRamMutex);
	sem_post(&s_tConfigRomMutex);

	return iRet;
}

int config_data_init (void)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t aui16IdList[MUI_CONFIG_DATA_NUM_MAX];
	int iRet;

	iRet = sem_init(&s_tConfigRomMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = sem_init(&s_tConfigRamMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = data_id_exclusive_scan_by_type(MUI_DATA_TYPE_CONFIG, aui16IdList, MUI_CONFIG_DATA_NUM_MAX, &s_tConfigRomMutex, MUI_CONFIG_ROM_WAIT_TIME_MS_MAX);
	if ((iRet<MUI_CONFIG_DATA_NUM_MAX) || (MUI_DATABASE_ID_START!=aui16IdList[0]))
	{
		//Config文件未找到,现在创建该文件,并进行初始化
		if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_CONFIG, achPath, sizeof(achPath)))
		{
			CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_data_init]Can't get path of config type.\r\n");
			return FILE_S_ADDR_ERR;
		}
		iRet = mk_multi_dir(achPath, MUI_DATA_CREAT_MODE);
		if (iRet < 0)
		{
			CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_data_init]Can't execute mkdir for config.\r\n");
			return FILE_S_ADDR_ERR;
		}
		if (NULL == data_file_path_get(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
		{
			CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_data_init]Can't get path of config data.\r\n");
			return FILE_S_ADDR_ERR;
		}
		iRet = file_exclusive_write(achPath, 0, achPath, 0, &s_tConfigRomMutex, MUI_CONFIG_ROM_WAIT_TIME_MS_MAX);
		if (iRet < 0)
		{
			CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_data_init]Can't creat config data-%s to ROM.\r\n", achPath);
			return iRet;
		}
	}

	s_config_ram_show();

	CONFIG_DATA_DYN_PRINT(M_DBG_LEVEL_INFO, "[config_data_init]Configice data initialize successfully.\r\n");

	return 0;
}

int config_data_test (void)
{
	return 0;
}

