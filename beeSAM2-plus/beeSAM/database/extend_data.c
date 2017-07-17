/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of extend_data module for beeSAM project.
File name:extend_data.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "extend_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define EXTEND_DATA_DBG

#ifndef EXTEND_DATA_PUTC
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_PUTC(Char)			PUTC(Char)
#else
#define EXTEND_DATA_PUTC(Char)
#endif
#endif

#ifndef EXTEND_DATA_PUTS
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define EXTEND_DATA_PUTS(Str, Len)
#endif
#endif

#ifndef EXTEND_DATA_PUTASCC
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_PUTASCC(Char)			PUTASCC(Char)
#else
#define EXTEND_DATA_PUTASCC(Char)
#endif
#endif

#ifndef EXTEND_DATA_PUTASCS
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define EXTEND_DATA_PUTASCS(Str, Len)
#endif
#endif

#ifndef EXTEND_DATA_PRINT
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define EXTEND_DATA_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef EXTEND_DATA_DYN_PUTC
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_DYN_PUTC(Dbg, Char)		DYN_PUTC((M_DBG_MODULE_EXTEND_DATA|Dbg), Char)
#else
#define EXTEND_DATA_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef EXTEND_DATA_DYN_PUTS
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_DYN_PUTS(Dbg, Str, Len)	DYN_PUTS((M_DBG_MODULE_EXTEND_DATA|Dbg), Str, Len)
#else
#define EXTEND_DATA_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef EXTEND_DATA_DYN_PUTASCC
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_EXTEND_DATA|Dbg), Char)
#else
#define EXTEND_DATA_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef EXTEND_DATA_DYN_PUTASCS
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_EXTEND_DATA|Dbg), Str, Len)
#else
#define EXTEND_DATA_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef EXTEND_DATA_DYN_PRINT
#ifdef EXTEND_DATA_DBG
#define EXTEND_DATA_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_EXTEND_DATA|Dbg), Fmt, ##Arg)
#else
#define EXTEND_DATA_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MI_EXTEND_ROM_SEARCH_NEW_OFFSET	(-5000000)	//保证ExtendId文件不会超过这个尺寸
#define MUI_EXTEND_RAM_WAIT_TIME_MS_MAX	3000
#define MUI_EXTEND_ROM_WAIT_TIME_MS_MAX	3000

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	uint32_t			ui32Total;
	uint32_t			ui32Len;
} S_ExtendRomDescr;

typedef struct
{
	uint32_t			ui32Id;
	uint32_t			ui32Len;
} S_ExtendIdDescr;

typedef struct
{
	uint8_t				ui8Data;
} S_ExtendStateData;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//static S_ExtendStateData s_sExtendStateData;
static sem_t s_tExtendRamMutex, s_tExtendRomMutex;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static void s_extend_rom_show (int iFd);
static int s_extend_rom_assign_id_by_fd (int iFd, uint32_t *pui32NewExtId);
static int s_extend_rom_id_search_by_fd (int iFd, uint32_t ui32DestExtId, uint32_t *pui32DataLen);
static int s_extend_rom_insert_id_by_fd (int iFd, size_t tOffset, uint32_t ui32ExtId, const void *pvBuf, size_t tLen);
static int s_extend_rom_cut_id_by_fd (int iFd, size_t tOffset, size_t tLen);
static int s_extend_rom_dir_del (void);
static int s_extend_data_ext_id_add (uint16_t ui16Type, uint16_t ui16Id, const void *pvBuf, size_t tLen, uint32_t *pui32NewExtId);
static int s_extend_data_ext_id_del (uint16_t ui16Type, uint16_t ui16Id, uint32_t ui32ExtId);
static int s_extend_data_ext_id_read (uint16_t ui16Type, uint16_t ui16Id, uint32_t ui32ExtId, void *pvBuf, size_t tSize);
static int s_extend_data_del (uint16_t ui16Type, uint16_t ui16Id);

//-------------------------------------------------------------------------------------------------*
/*
                               ExtId&Data:1                      ExtId&Data:2             ...
[ui32Total][ ui32Len ][ui32ExtId1|ui32DataLen1|Data1...][ui32ExtId2|ui32DataLen2|Data2...]...
ui32Len = sizeof(ExtId&Data:1) + sizeof(ExtId&Data:2) + ... + sizeof(ExtId&Data:n)
*/

//-------------------------------------------------------------------------------------------------*
/*	设备RAM(数据表)显示函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		NULL
;	(注意)
*/
static void s_extend_rom_show (int iFd)
{
#if 1
	S_ExtendRomDescr sRom;
	S_ExtendIdDescr sExtId;
	size_t tOffset;
	uint32_t ui32i;
	int iRet;

	iRet = file_read_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_show]Call 'file_read_by_fd(&sRom)' fail.\r\n");
		return;
	}

	EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_rom_show]ui32Total=%u, ui32Len=%u\r\n", sRom.ui32Total, sRom.ui32Len);

	tOffset = sizeof(S_ExtendRomDescr);
	for (ui32i=0; ui32i<sRom.ui32Total; ui32i++, tOffset+=(sizeof(S_ExtendIdDescr)+sExtId.ui32Len))
	{
		iRet = file_read_by_fd(iFd, tOffset, &sExtId, sizeof(S_ExtendIdDescr));
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_show]Call 'file_read_by_fd(&sExtId)' fail.\r\n");
			return;
		}

		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Offset=%u, ui32Id=%u, ui32Len=%u\r\n", tOffset, sExtId.ui32Id, sExtId.ui32Len);
	}
#endif
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	扩展ROM分配ID函数
;	(功能)
;	(参数)
;		iFd:			要搜索的文件句柄
;		pui32NewExtId:	要分配的ID回写指针
;	(返回)
;		int:			成功分配,返回在ROM中可插入数据的的实际偏移量(Bytes)
;						如果失败返回负数
;	(注意)
*/
static int s_extend_rom_assign_id_by_fd (int iFd, uint32_t *pui32NewExtId)
{
	S_ExtendRomDescr sRom;
	S_ExtendIdDescr sExtId;
	size_t tOffset;
	uint32_t ui32ExtId, ui32i;
	int iRet;

	ASSERT(0 < iFd);
	ASSERT(NULL != pui32NewExtId);

	iRet = file_read_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_assign_id_by_fd]Read ROM description error.\r\n");
		return iRet;
	}

	ui32ExtId  = 0;
	tOffset = sizeof(S_ExtendRomDescr);
	for (ui32i=0; ui32i<sRom.ui32Total; ui32i++, ui32ExtId++)
	{
		iRet = file_read_by_fd(iFd, tOffset, &sExtId, sizeof(S_ExtendIdDescr));
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_assign_id_by_fd]Read extend ID description error.\r\n");
			return iRet;
		}

		if (ui32ExtId < sExtId.ui32Id)
		{
			//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_rom_assign_id_by_fd]Assign new extend ID:%u\r\n", ui32ExtId);
			break;
		}

		tOffset += (sizeof(S_ExtendIdDescr)+sExtId.ui32Len);
	}

	*pui32NewExtId = ui32ExtId;

	return (int)tOffset;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	扩展ROM搜索函数
;	(功能)
;	(参数)
;		iFd:			要搜索的文件句柄
;		ui32DestExtId:	要找的用ExtId
;		pui32DataLen:	数据长度返回指针
;	(返回)
;		int:			成功找到,返回在ROM中的实际偏移量(Bytes)
;						如果未找到,则返回可插入偏移位置+MI_EXTEND_ROM_SEARCH_NEW_OFFSET
;						异常返回EXTEND_ERR_ROM_ACCESS_FAIL
;	(注意)
*/
static int s_extend_rom_id_search_by_fd (int iFd, uint32_t ui32DestExtId, uint32_t *pui32DataLen)
{
	S_ExtendRomDescr sRom;
	S_ExtendIdDescr sExtId;
	size_t tOffset;
	uint32_t ui32i;
	int iRet;

	//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_rom_id_search_by_fd]Will search extend ID:%u\r\n", ui32DestExtId);
	//s_extend_rom_show(iFd);

	iRet = file_read_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
	if (iRet < 0)
	{
		return iRet;
	}

	tOffset = sizeof(S_ExtendRomDescr);
	for (ui32i=0; ui32i<sRom.ui32Total; ui32i++, tOffset+=(sizeof(S_ExtendIdDescr)+sExtId.ui32Len))
	{
		iRet = file_read_by_fd(iFd, tOffset, &sExtId, sizeof(S_ExtendIdDescr));
		if (iRet < 0)
		{
			return iRet;
		}

		if (sExtId.ui32Id == ui32DestExtId)
		{
			//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_rom_id_search_by_fd]Have find extend ID:%u on [%u]\r\n", ui32DestExtId, tOffset);
			break;
		}
		else if (sExtId.ui32Id > ui32DestExtId)
		{
			//已经找到比本ExtId还大的ID,可判定后面再也找不到了
			return ((int)MI_EXTEND_ROM_SEARCH_NEW_OFFSET+(int)tOffset);
		}
	}

	//ASSERT((ui32Offset-sizeof(S_ExtendRomDescr)) <= ui32Len);
	if (ui32i >= sRom.ui32Total)
	{
		//未找到指定的ui32DestExtId
		return ((int)MI_EXTEND_ROM_SEARCH_NEW_OFFSET+(int)tOffset);
	}

	*pui32DataLen = sExtId.ui32Len;

	return (int)tOffset;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	扩展ROM插入ID函数
;	(功能)
;	(参数)
;		iFd:		要搜索的文件句柄
;		tOffset:	插入的偏移位置
;		ui32ExtId:	要插入的ExtId
;		pvBuf:		要插入ExtId对应数据缓存
;		tLen:		要插入ExtId对应数据长度
;	(返回)
;		int:		成功插入,返回插入数据的长度
;	(注意)
*/
static int s_extend_rom_insert_id_by_fd (int iFd, size_t tOffset, uint32_t ui32ExtId, const void *pvBuf, size_t tLen)
{
	uint8_t aui8Buf[1024];
	S_ExtendRomDescr sRom;
	S_ExtendIdDescr sExtId;
	size_t tMoveTail, tMoveLen, tMoveOffset, tDeltaLen;
	int iRet;

	ASSERT(0 < iFd);
	ASSERT(NULL != pvBuf);
	ASSERT(0 < tLen);

	//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_rom_insert_id_by_fd]Will insert extend ID:%u\r\n", ui32ExtId);
	//s_extend_rom_show(iFd);

	iRet = file_read_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_insert_id_by_fd]Read ROM description fail.\r\n");
		return iRet;
	}

	ASSERT(tOffset <= sizeof(S_ExtendRomDescr)+(size_t)sRom.ui32Len);

	tMoveTail	= sizeof(S_ExtendRomDescr) + (size_t)sRom.ui32Len;
	tMoveLen	= tMoveTail - tOffset;
	tMoveOffset	= sizeof(S_ExtendIdDescr) + tLen;
	while (tMoveLen > 0)
	{
		if (tMoveLen > sizeof(aui8Buf))
		{
			tDeltaLen = sizeof(aui8Buf);
		}
		else
		{
			tDeltaLen = tMoveLen;
		}
		tMoveTail -= tDeltaLen;
		iRet = file_read_by_fd(iFd, tMoveTail, aui8Buf, tDeltaLen);
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_insert_id_by_fd]Read buffer fail.\r\n");
			return iRet;
		}
		iRet = file_write_by_fd(iFd, tMoveTail+tMoveOffset, aui8Buf, tDeltaLen);
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_insert_id_by_fd]Write buffer fail.\r\n");
			return iRet;
		}
		tMoveLen -= tDeltaLen;
	}

	sExtId.ui32Id  = ui32ExtId;
	sExtId.ui32Len = (uint32_t)tLen;
	iRet = file_write_by_fd(iFd, tOffset, &sExtId, sizeof(S_ExtendIdDescr));
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_insert_id_by_fd]Write extend ID description fail.\r\n");
		return iRet;
	}
	iRet = file_write_by_fd(iFd, tOffset+sizeof(S_ExtendIdDescr), pvBuf, tLen);
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_insert_id_by_fd]Write extend ID data fail.\r\n");
		return iRet;
	}

	sRom.ui32Total++;
	sRom.ui32Len += (uint32_t)(sizeof(S_ExtendIdDescr)+tLen);
	iRet = file_write_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_insert_id_by_fd]Write ROM description fail.\r\n");
		return iRet;
	}

	return (int)tLen;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	扩展ROM剪切函数
;	(功能)
;	(参数)
;		iFd:		要搜索的文件句柄
;		tOffset:	插入的偏移位置
;		tLen:		要插入ExtId对应数据长度(不包括S_ExtendIdDescr的长度)
;	(返回)
;		int:		成功剪切,返回剪切数据的长度
;	(注意)
*/
static int s_extend_rom_cut_id_by_fd (int iFd, size_t tOffset, size_t tLen)
{
	uint8_t aui8Buf[1024];
	S_ExtendRomDescr sRom;
	S_ExtendIdDescr sExtId;
	size_t tMoveHead, tMoveLen, tMoveOffset, tDeltaLen;
	int iRet;

	ASSERT(0 < iFd);
	ASSERT(0 < tLen);

	//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_rom_cut_id_by_fd]Will cut data from extend file, Offset=%u, tLen=%u.\r\n", tOffset, tLen);
	//s_extend_rom_show(iFd);

	iRet = file_read_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_cut_id_by_fd]Read ROM description fail.\r\n");
		return iRet;
	}

	iRet = file_read_by_fd(iFd, tOffset, &sExtId, sizeof(S_ExtendIdDescr));
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_cut_id_by_fd]Read extend ID description fail.\r\n");
		return iRet;
	}

	ASSERT(sExtId.ui32Len == tLen);
	//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_rom_cut_id_by_fd]Will cut extend ID %u.\r\n", sExtId.ui32Id);

	tMoveOffset	= sizeof(S_ExtendIdDescr) + tLen;
	tMoveHead	= tOffset + tMoveOffset;
	tMoveLen	= sizeof(S_ExtendRomDescr) + (size_t)sRom.ui32Len - tMoveHead;
	while (tMoveLen > 0)
	{
		if (tMoveLen > sizeof(aui8Buf))
		{
			tDeltaLen = sizeof(aui8Buf);
		}
		else
		{
			tDeltaLen = tMoveLen;
		}
		iRet = file_read_by_fd(iFd, tMoveHead, aui8Buf, tDeltaLen);
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_cut_id_by_fd]Read buffer fail.\r\n");
			return iRet;
		}
		iRet = file_write_by_fd(iFd, tMoveHead-tMoveOffset, aui8Buf, tDeltaLen);
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_cut_id_by_fd]Write buffer fail.\r\n");
			return iRet;
		}
		tMoveHead += tDeltaLen;
		tMoveLen  -= tDeltaLen;
	}

	sRom.ui32Total--;
	sRom.ui32Len -= (uint32_t)(sizeof(S_ExtendIdDescr)+tLen);
	iRet = file_write_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_rom_cut_id_by_fd]Write ROM description fail.\r\n");
		return iRet;
	}

	return (int)tLen;
}
//-------------------------------------------------------------------------------------------------*
static int s_extend_rom_dir_del (void)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	//删除无数据的目录
	if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_EXTEND, achPath, sizeof(achPath)))
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_extend_rom_dir_del]Can't get path of extendice type %04hx.\r\n", MUI_DATA_TYPE_EXTEND);
		return FILE_S_ADDR_ERR;
	}
	iRet = del_empty_dir(achPath);
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_extend_rom_dir_del]Can't delete empty dir for extendice type %04hx.\r\n", MUI_DATA_TYPE_EXTEND);
		return iRet;
	}

	return 0;
}

static int s_extend_data_ext_id_add (uint16_t ui16Type, uint16_t ui16Id, const void *pvBuf, size_t tLen, uint32_t *pui32NewExtId)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	S_ExtendRomDescr sRom;
	size_t tOffset;
	int iFd, iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(NULL != pvBuf);
	ASSERT(0 < tLen);
	ASSERT(NULL != pui32NewExtId);

	if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_EXTEND, achPath, sizeof(achPath)))
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Can't get path of data type %04hx\r\n", MUI_DATA_TYPE_EXTEND);
		return FILE_S_ADDR_ERR;
	}
	iRet = mk_multi_dir(achPath, MUI_DATA_CREAT_MODE);
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Can't execute mkdir for data type %04hx\r\n", MUI_DATA_TYPE_EXTEND);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_extend_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_ADDR_ERR;
	}

	if (access(achPath, F_OK) != 0)
	{
		iFd = open(achPath, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		if (iFd < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_INFO, "[s_extend_data_ext_id_add]Call function 'open(%s)' fail, errno:%d\r\n", achPath, errno);
			return iFd;
		}

		sRom.ui32Total = 0;
		sRom.ui32Len   = 0;
		iRet = file_write_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Call function 'file_write_by_fd()' fail.\r\n");
		}
		else
		{
			tOffset = sizeof(S_ExtendRomDescr);
			*pui32NewExtId = 0;
		}
		//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_data_ext_id_add]Have create extend file.\r\n");
		//s_extend_rom_show(iFd);
	}
	else
	{
		iFd = open(achPath, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		if (iFd < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Call function 'open(%s)' fail, errno:%d\r\n", achPath, errno);
			return iFd;
		}

		iRet = s_extend_rom_assign_id_by_fd(iFd, pui32NewExtId);
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Call function 's_extend_rom_assign_id_by_fd()' fail.\r\n");
		}
		else
		{
			tOffset = (size_t)iRet;
		}
	}

	if (iRet >= 0)
	{
		iRet = s_extend_rom_insert_id_by_fd(iFd, tOffset, *pui32NewExtId, pvBuf, tLen);
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Call function 's_extend_rom_insert_id_by_fd()' fail.\r\n");
		}
		//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_data_ext_id_add]Have insert data to extend file.\r\n");
		//s_extend_rom_show(iFd);
	}

	close(iFd);

	return iRet;
}

static int s_extend_data_ext_id_del (uint16_t ui16Type, uint16_t ui16Id, uint32_t ui32ExtId)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	S_ExtendRomDescr sRom;
	uint32_t ui32Len;
	uint8_t ui8Del = 0;
	int iFd, iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_data_ext_id_del]Will delete ui16Type=%u, ui16Id=%u, ui32ExtId=%u\r\n", ui16Type, ui16Id, ui32ExtId);
	//s_extend_rom_show(iFd);

	if (NULL == data_extend_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_del]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_ADDR_ERR;
	}

	iFd = open(achPath, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_INFO, "[s_extend_data_ext_id_del]Call function 'open(%s)' fail, errno:%d\r\n", achPath, errno);
		return iFd;
	}

	iRet = s_extend_rom_id_search_by_fd(iFd, ui32ExtId, &ui32Len);
	if (iRet >= 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_INFO, "[s_extend_data_ext_id_del]Offset=%d, Len=%u\r\n", iRet, ui32Len);
		iRet = s_extend_rom_cut_id_by_fd(iFd, (size_t)iRet, (size_t)ui32Len);
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Call function 's_extend_rom_cut_id_by_fd()' fail.\r\n");
		}

		iRet = file_read_by_fd(iFd, 0, &sRom, sizeof(S_ExtendRomDescr));
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_add]Call function 'file_read_by_fd()' fail.\r\n");
		}

		if (0 == sRom.ui32Total)
		{
			ui8Del = 1;
		}
	}
	else
	{
		//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_data_ext_id_del]Call function 's_extend_rom_id_search_by_fd()' fail.\r\n");
	}

	close(iFd);

	if (ui8Del != 0)
	{
		iRet = s_extend_data_del(ui16Type, ui16Id);
	}

	return iRet;
}

static int s_extend_data_ext_id_read (uint16_t ui16Type, uint16_t ui16Id, uint32_t ui32ExtId, void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint32_t ui32Len;
	int iFd, iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(NULL != pvBuf);
	ASSERT(0 < tSize);

	if (NULL == data_extend_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_del]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_ADDR_ERR;
	}

	iFd = open(achPath, O_RDONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_INFO, "[s_extend_data_ext_id_read]Call function 'open(%s)' fail, errno:%d\r\n", achPath, errno);
		return iFd;
	}

	do
	{
		iRet = s_extend_rom_id_search_by_fd(iFd, ui32ExtId, &ui32Len);
		if (iRet < 0)
		{
			//EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_data_ext_id_read]Call function 's_extend_rom_id_search_by_fd()' fail.\r\n");
			break;
		}
		if (tSize < ui32Len)
		{
			iRet = FILE_S_BUF_ERR;
		}

		iRet = file_read_by_fd(iFd, (size_t)iRet+sizeof(S_ExtendIdDescr), pvBuf, ui32Len);
		if (iRet < 0)
		{
			EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_read]Call function 'file_read_by_fd()' fail.\r\n");
			break;
		}
	} while (0);

	close(iFd);

	return iRet;
}

static int s_extend_data_del (uint16_t ui16Type, uint16_t ui16Id)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (NULL == data_extend_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_ext_id_del]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_del(achPath);
	if (iRet < 0)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_extend_data_del]Can't delete device data-%s from ROM.\r\n", achPath);
		return iRet;
	}

	if (0 > s_extend_rom_dir_del())
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_extend_data_del]Call 's_extend_rom_dir_del(%04hx)' fail.\r\n", MUI_DATA_TYPE_EXTEND);
	}

	return iRet;
}




int extend_data_ext_id_add (uint16_t ui16Type, uint16_t ui16Id, const void *pvBuf, size_t tLen, uint32_t *pui32ExtId)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(NULL != pvBuf);
	ASSERT(0 < tLen);
	ASSERT(NULL != pui32ExtId);

	if (0 > sem_wait_ms(&s_tExtendRomMutex, MUI_EXTEND_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_EXTEND_ROM_WAIT_TIME_MS_MAX)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[extend_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tExtendRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_extend_data_ext_id_add(ui16Type, ui16Id, pvBuf, tLen, pui32ExtId);

	sem_post(&s_tExtendRomMutex);

	return iRet;
}
int extend_data_ext_id_del (uint16_t ui16Type, uint16_t ui16Id, uint32_t ui32ExtId)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tExtendRomMutex, MUI_EXTEND_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_EXTEND_ROM_WAIT_TIME_MS_MAX)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[extend_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tExtendRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_extend_data_ext_id_del(ui16Type, ui16Id, ui32ExtId);

	sem_post(&s_tExtendRomMutex);

	return iRet;
}
int extend_data_ext_id_read (uint16_t ui16Type, uint16_t ui16Id, uint32_t ui32ExtId, void *pvBuf, size_t tSize)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tExtendRomMutex, MUI_EXTEND_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_EXTEND_ROM_WAIT_TIME_MS_MAX)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[extend_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tExtendRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_extend_data_ext_id_read(ui16Type, ui16Id, ui32ExtId, pvBuf, tSize);

	sem_post(&s_tExtendRomMutex);

	return iRet;
}

int extend_data_del (uint16_t ui16Type, uint16_t ui16Id)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tExtendRomMutex, MUI_EXTEND_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_EXTEND_ROM_WAIT_TIME_MS_MAX)
	{
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[extend_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tExtendRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_extend_data_del(ui16Type, ui16Id);

	sem_post(&s_tExtendRomMutex);

	return iRet;
}

int extend_data_init (void)
{
	int iRet;

	iRet = sem_init(&s_tExtendRomMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = sem_init(&s_tExtendRamMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return iRet;
	}

	EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_INFO, "[extend_data_init]Extend data initialize successfully.\r\n");

	return 0;
}

int extend_data_test (void)
{
	char achBuf[1024];
	uint32_t ui32ExtId;
	int i, iLen;

	for (i=0; i<100; i++)
	{
		iLen = snprintf(achBuf, sizeof(achBuf), "Extend ID:%d ++++", i);
		extend_data_ext_id_add(MUI_DATA_TYPE_SONOS, MUI_DATABASE_ID_START, achBuf, iLen, &ui32ExtId);
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Have added Extend ID:%u\r\n", ui32ExtId);
	}

	for (ui32ExtId=0; ui32ExtId<200; ui32ExtId++)
	{
		iLen = extend_data_ext_id_read(MUI_DATA_TYPE_SONOS, MUI_DATABASE_ID_START, ui32ExtId, achBuf, sizeof(achBuf));
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Extend ID:%u[Len=%d]\r\n%s\r\n", ui32ExtId, iLen, achBuf);
	}

	for (ui32ExtId=10; ui32ExtId<50; ui32ExtId++)
	{
		iLen = extend_data_ext_id_del(MUI_DATA_TYPE_SONOS, MUI_DATABASE_ID_START, ui32ExtId);
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Have deleted Extend ID:%u\r\n", ui32ExtId);
	}

	for (ui32ExtId=0; ui32ExtId<200; ui32ExtId++)
	{
		iLen = extend_data_ext_id_read(MUI_DATA_TYPE_SONOS, MUI_DATABASE_ID_START, ui32ExtId, achBuf, sizeof(achBuf));
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Extend ID:%u[Len=%d]\r\n%s\r\n", ui32ExtId, iLen, achBuf);
	}

	for (i=0; i<100; i++)
	{
		iLen = snprintf(achBuf, sizeof(achBuf), "Extend ID:#%d ++++", i);
		extend_data_ext_id_add(MUI_DATA_TYPE_SONOS, MUI_DATABASE_ID_START, achBuf, iLen, &ui32ExtId);
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Have added Extend ID:#%u\r\n", ui32ExtId);
	}

	for (ui32ExtId=0; ui32ExtId<200; ui32ExtId++)
	{
		iLen = extend_data_ext_id_read(MUI_DATA_TYPE_SONOS, MUI_DATABASE_ID_START, ui32ExtId, achBuf, sizeof(achBuf));
		EXTEND_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "Extend ID:%u[Len=%d]\r\n%s\r\n", ui32ExtId, iLen, achBuf);
	}

	return 0;
}

