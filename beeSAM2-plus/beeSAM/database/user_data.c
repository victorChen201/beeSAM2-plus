/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of user_data module for beeSAM project.
File name:user_data.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "user_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define USER_DATA_DBG

#ifndef USER_DATA_PUTC
#ifdef USER_DATA_DBG
#define USER_DATA_PUTC(Char)			PUTC(Char)
#else
#define USER_DATA_PUTC(Char)
#endif
#endif

#ifndef USER_DATA_PUTS
#ifdef USER_DATA_DBG
#define USER_DATA_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define USER_DATA_PUTS(Str, Len)
#endif
#endif

#ifndef USER_DATA_PUTASCC
#ifdef USER_DATA_DBG
#define USER_DATA_PUTASCC(Char)			PUTASCC(Char)
#else
#define USER_DATA_PUTASCC(Char)
#endif
#endif

#ifndef USER_DATA_PUTASCS
#ifdef USER_DATA_DBG
#define USER_DATA_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define USER_DATA_PUTASCS(Str, Len)
#endif
#endif

#ifndef USER_DATA_PRINT
#ifdef USER_DATA_DBG
#define USER_DATA_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define USER_DATA_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef USER_DATA_DYN_PUTC
#ifdef USER_DATA_DBG
#define USER_DATA_DYN_PUTC(Dbg, Char)		DYN_PUTC((M_DBG_MODULE_USER_DATA|Dbg), Char)
#else
#define USER_DATA_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef USER_DATA_DYN_PUTS
#ifdef USER_DATA_DBG
#define USER_DATA_DYN_PUTS(Dbg, Str, Len)	DYN_PUTS((M_DBG_MODULE_USER_DATA|Dbg), Str, Len)
#else
#define USER_DATA_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef USER_DATA_DYN_PUTASCC
#ifdef USER_DATA_DBG
#define USER_DATA_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_USER_DATA|Dbg), Char)
#else
#define USER_DATA_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef USER_DATA_DYN_PUTASCS
#ifdef USER_DATA_DBG
#define USER_DATA_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_USER_DATA|Dbg), Str, Len)
#else
#define USER_DATA_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef USER_DATA_DYN_PRINT
#ifdef USER_DATA_DBG
#define USER_DATA_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_USER_DATA|Dbg), Fmt, ##Arg)
#else
#define USER_DATA_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MI_USER_ROM_SEARCH_NEW_OFFSET	(-50000)
#define MUI_USER_RAM_WAIT_TIME_MS_MAX	3000
#define MUI_USER_ROM_WAIT_TIME_MS_MAX	3000

#define MUI_USER_COMMON_NUM_MAX			8
#if (MUI_USER_COMMON_NUM_MAX < 2)
	#error "MUI_USER_COMMON_NUM_MAX must more than 2."
#endif
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	uint16_t			ui16Idx;
	S_UserData			sUserData;
} S_UserCache;

//RAM数据
typedef struct
{
	S_UserCache			asUserCache[MUI_USER_COMMON_NUM_MAX];
} S_UserStateData;

//ROM数据
typedef struct
{
	uint16_t			ui16Total;
	S_UserData			asUserData[MUI_USER_NUM_MAX];
} S_UserAttrData;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static S_UserStateData s_sUserStateData;
static sem_t s_tUserRamMutex, s_tUserRomMutex;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static void s_user_ram_show (void);
static int s_user_ram_search (const char *pchUserName, size_t tLen, uint16_t ui16Start, uint16_t ui16End);
static int s_user_ram_idx_get_by_offset (uint16_t ui16Offset);
static int s_user_ram_time_updata_by_offset (uint16_t ui16Offset);
static int s_user_rom_get_by_fd (int iFd, uint16_t ui16Idx, S_UserData *psUserData);
static int s_user_rom_search_by_fd (int iFd, const char *pchUserName, size_t tLen, uint16_t ui16Start, uint16_t ui16End);
static int s_user_rom_bring_in_ram_by_fd (int iFd, uint16_t ui16Idx);
static int s_user_rom_insert_by_fd (int iFd, uint16_t ui16Idx, const S_UserData *psUserData);
static int s_user_rom_del_by_fd (int iFd, uint16_t ui16Idx);
static int s_user_rom_get_by_idx (uint16_t ui16Idx, S_UserData *psUserData);
static int s_user_rom_checkout (const char *pchUserName, size_t tLen);
static int s_user_rom_add (const char *pchUserName, size_t tUserNameLen, const char *pchUserDevName, size_t tUserDevNameLen);
static int s_user_rom_del (const char *pchName, size_t tLen);

//-------------------------------------------------------------------------------------------------*
/*	设备RAM(数据表)显示函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		NULL
;	(注意)
*/
static void s_user_ram_show (void)
{
#if 0
	char achPath[MUI_FILE_ABS_PATH_LEN];
	S_UserData sUserData;
	uint16_t ui16Total, ui16i;
	int iFd;

	data_file_path_get(MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START, achPath, sizeof(achPath));
	iFd = open(achPath, O_RDONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, sizeof(ui16Total));
	USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_ram_show]User total=%u\r\n", ui16Total);
	for (ui16i=0; ui16i<ui16Total; ui16i++)
	{
		file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+(size_t)ui16i*sizeof(sUserData), &sUserData, sizeof(sUserData));
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "<%u>%s:%s\r\n", ui16i, sUserData.name, sUserData.dev_name);
	}
	close(iFd);

	USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_ram_show]-------------------------------------------\r\n");

	for (ui16i=0; ui16i<MUI_USER_COMMON_NUM_MAX; ui16i++)
	{
		if ('\0' == s_sUserStateData.asUserCache[ui16i].sUserData.name[0])
		{
			break;
		}
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[%u]<%u>%s:%s\r\n", ui16i, s_sUserStateData.asUserCache[ui16i].ui16Idx, s_sUserStateData.asUserCache[ui16i].sUserData.name, s_sUserStateData.asUserCache[ui16i].sUserData.dev_name);
	}

	USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_ram_show]====================END====================\r\n");
#endif
}

//调整最后使用时间
static int s_user_ram_idx_get_by_offset (uint16_t ui16Offset)
{
	ASSERT(MUI_USER_COMMON_NUM_MAX > ui16Offset);
	ASSERT('\0' != s_sUserStateData.asUserCache[ui16Offset].sUserData.name[0]);

	return (int)s_sUserStateData.asUserCache[ui16Offset].ui16Idx;
}

//-------------------------------------------------------------------------------------------------*
//调整最后使用时间
static int s_user_ram_time_updata_by_offset (uint16_t ui16Offset)
{
	ASSERT(MUI_USER_COMMON_NUM_MAX > ui16Offset);
	ASSERT('\0' != s_sUserStateData.asUserCache[ui16Offset].sUserData.name[0]);

	time(&s_sUserStateData.asUserCache[ui16Offset].sUserData.last_use_time);
	return 0;
}

//-------------------------------------------------------------------------------------------------*
/*	用户RAM(数据表)搜索函数
;	(功能)
;	(参数)
;		pchUserName:	要找的用户名
;		tLen:			要找的用户名长度
;		ui16Start:		搜索范围的起始索引
;		ui16End:		搜索范围的结束索引(当ui16Start等于ui16End,则认为搜索范围为空,从这个意义上说,可以
;						认为ui16End指向的索引应比实际搜索范围多1)
;	(返回)
;		int:			成功找到,返回在RAM中的实际偏移量(以S_UserData单元计数)
;						异常返回USER_ERR_NO_FIND
;	(注意)
*/
static int s_user_ram_search (const char *pchUserName, size_t tLen, uint16_t ui16Start, uint16_t ui16End)
{
	char achDest[MUI_USER_NAME_LEN+1];
	uint16_t ui16i;

	ASSERT(NULL != pchUserName);
	ASSERT(MUI_USER_NAME_LEN >= tLen);
	ASSERT(ui16Start <= ui16End);	//Start<=End
	ASSERT(MUI_USER_COMMON_NUM_MAX >= ui16End);

	memcpy(achDest, pchUserName, tLen);
	memset(&achDest[tLen], 0, sizeof(achDest)-tLen);

	for (ui16i=ui16Start; ui16i<ui16End; ui16i++)
	{
		if ('\0' == s_sUserStateData.asUserCache[ui16i].sUserData.name[0])
		{
			break;	//发现name为空,即认为后面已经没有存储用户名了
		}
		else if (0 == memcmp(achDest, s_sUserStateData.asUserCache[ui16i].sUserData.name, sizeof(achDest)))
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_ram_search]Have found user name:<%d>%s on offset [%u]\r\n", s_user_ram_idx_get_by_offset(ui16i), achDest, ui16i);
			return (int)ui16i;
		}
	}

	USER_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_user_ram_search]Can't find user name:%s\r\n", achDest);

	return USER_ERR_NO_FIND;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	用户ROM(数据表)搜索函数
;	(功能)
;	(参数)
;		iFd:			要搜索的文件句柄
;		pchUserName:	要找的用户名
;		tLen:			要找的用户名长度
;		ui16Start:		搜索范围的起始索引
;		ui16End:		搜索范围的结束索引(当ui16Start等于ui16End,则认为搜索范围为空,从这个意义上说,可以
;						认为ui16End指向的索引应比实际搜索范围多1)
;	(返回)
;		int:			成功找到,返回在ROM中的实际偏移量(以S_UserData单元计数)
;						如果未找到,则返回可插入偏移位置+MI_USER_ROM_SEARCH_NEW_OFFSET
;						异常返回USER_ERR_ROM_ACCESS_FAIL
;	(注意)
*/
static int s_user_rom_search_by_fd (int iFd, const char *pchUserName, size_t tLen, uint16_t ui16Start, uint16_t ui16End)
{
	char achDest[MUI_USER_NAME_LEN+1];
	char achBuf[MUI_USER_NAME_LEN+1];
	uint16_t ui16M, ui16MOld;
	int iRet;

	ASSERT(USER_ERR_ROM_ACCESS_FAIL > (MI_USER_ROM_SEARCH_NEW_OFFSET+MUI_USER_NUM_MAX));
	ASSERT(0 <= iFd);
	ASSERT(NULL != pchUserName);
	ASSERT(0 < tLen);
	ASSERT(ui16Start <= ui16End);	//Start<=End
	ASSERT(MUI_USER_NUM_MAX >= ui16End);

	memcpy(achDest, pchUserName, tLen);
	memset(&achDest[tLen], 0, sizeof(achDest)-tLen);
	ui16MOld = ui16End;
	iRet  = -1;		//当ui16Start==ui16End,直接返回MI_USER_ROM_SEARCH_NEW_OFFSET+(int)ui16Start

	while (ui16MOld != (ui16M=((ui16Start+ui16End)>>1)))
	{
		ui16MOld = ui16M;
		if (0 > file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+((size_t)ui16M*sizeof(S_UserData))+M_OFFSET(S_UserData, name), achBuf, M_SIZEOF(S_UserData, name)))
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_search_by_fd]Call 'file_read_by_fd()' fail.\r\n");
			return USER_ERR_ROM_ACCESS_FAIL;
		}
		iRet  = memcmp(achDest, achBuf, sizeof(achDest));
		if (iRet > 0)
		{
			ui16Start = ui16M;
		}
		else if (iRet < 0)
		{
			ui16End = ui16M;
		}
		else
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_search_by_fd]Have find '%s' on <%u>.\r\n", achDest, ui16M);
			return (int)ui16M;
		}
	}
#if 1
	if (iRet > 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_search_by_fd]Can insert '%s' to index:%u\r\n", achDest, ui16M+1);
		return (MI_USER_ROM_SEARCH_NEW_OFFSET + 1 + (int)ui16M);
	}
	else
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_search_by_fd]Can insert '%s' to index:%u\r\n", achDest, ui16M);
		return (MI_USER_ROM_SEARCH_NEW_OFFSET + (int)ui16M);
	}
#else
	return USER_ERR_NO_FIND;
#endif
}
//-------------------------------------------------------------------------------------------------*

static int s_user_rom_bring_in_ram_by_fd (int iFd, uint16_t ui16Idx)
{
	int iRet, i;

	ASSERT(0 <= iFd);
	ASSERT(MUI_USER_NUM_MAX > ui16Idx);

	//检查RAM中数据是否已满,如已满,则需要将最后一个sUserData同步回ROM
	if ('\0' != s_sUserStateData.asUserCache[MUI_USER_COMMON_NUM_MAX-1].sUserData.name[0])
	{
		iRet = file_write_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+((size_t)s_sUserStateData.asUserCache[MUI_USER_COMMON_NUM_MAX-1].ui16Idx*sizeof(S_UserData))+M_OFFSET(S_UserData, last_use_time),
									  &s_sUserStateData.asUserCache[MUI_USER_COMMON_NUM_MAX-1].sUserData.last_use_time, M_SIZEOF(S_UserData, last_use_time));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[s_user_rom_bring_in_ram_by_fd]Call 'file_write_by_fd()' fail.\r\n");
		}
	}

	i = (int)MUI_USER_COMMON_NUM_MAX - 1 - 1;
	while (i >= 0)
	{
		if ('\0' != s_sUserStateData.asUserCache[i].sUserData.name[0])
		{
			memcpy(&s_sUserStateData.asUserCache[i+1], &s_sUserStateData.asUserCache[i], sizeof(s_sUserStateData.asUserCache[i+1]));
		}
		i--;
	}

	s_sUserStateData.asUserCache[0].ui16Idx = ui16Idx;
	iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+(size_t)ui16Idx*sizeof(S_UserData), &s_sUserStateData.asUserCache[0].sUserData, sizeof(s_sUserStateData.asUserCache[0].sUserData));
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_bring_in_ram_by_fd]Call 'file_write_by_fd()' fail.\r\n");
	}

	s_user_ram_time_updata_by_offset(0);//调整最后使用时间

	return iRet;
}

static int s_user_rom_insert_by_fd (int iFd, uint16_t ui16Idx, const S_UserData *psUserData)
{
	S_UserData sUserData;
	uint16_t ui16Total, ui16i;
	int iRet;

	ASSERT(0 <= iFd);
	ASSERT(MUI_USER_NUM_MAX > ui16Idx);
	ASSERT(NULL != psUserData);

	iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, sizeof(ui16Total));
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_insert_by_fd]Call 'file_read_by_fd()' error.\r\n");
		return iRet;
	}
	if (ui16Idx > ui16Total)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_insert_by_fd]Insert index error.\r\n");
		return USER_ERR_UNKONW_ERR;
	}
	//调整用户数据位置
	for (ui16i=ui16Total; ui16i>ui16Idx; ui16i--)
	{
		iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+((size_t)(ui16i-1)*sizeof(S_UserData)), &sUserData, sizeof(sUserData));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_insert_by_fd]Call 'file_read_by_fd()' fail.\r\n");
			return iRet;
		}
		iRet = file_write_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+((size_t)ui16i*sizeof(S_UserData)), &sUserData, sizeof(sUserData));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_insert_by_fd]Call 'file_write_by_fd()' fail.\r\n");
			return iRet;
		}
	}
	//在指定位置写入新用户数据
	iRet = file_write_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+((size_t)ui16Idx*sizeof(S_UserData)), psUserData, sizeof(sUserData));
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_insert_by_fd]Call 'file_write_by_fd()' fail.\r\n");
		return iRet;
	}
	//更新Total
	ui16Total++;
	iRet = file_write_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, M_SIZEOF(S_UserAttrData, ui16Total));
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_insert_by_fd]Call 'file_write_by_fd()' error.\r\n");
		return iRet;
	}
	//调整RAM中idx
	for (ui16i=0; ui16i<MUI_USER_COMMON_NUM_MAX; ui16i++)
	{
		if ('\0' == s_sUserStateData.asUserCache[ui16i].sUserData.name[0])
		{
			break;	//发现name为空,即认为后面已经没有存储用户名了
		}

		if (s_sUserStateData.asUserCache[ui16i].ui16Idx >= ui16Idx)
		{
			s_sUserStateData.asUserCache[ui16i].ui16Idx++;
		}
	}

	return (int)ui16Idx;
}

static int s_user_rom_del_by_fd (int iFd, uint16_t ui16Idx)
{
	S_UserData sUserData;
	uint16_t ui16Total, ui16i;
	int iRet;

	ASSERT(0 <= iFd);
	ASSERT(MUI_USER_NUM_MAX > ui16Idx);

	iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, sizeof(ui16Total));
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_del_by_fd]Call 'file_read_by_fd()' error.\r\n");
		return iRet;
	}
	if (ui16Idx >= ui16Total)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_del_by_fd]Insert index error.\r\n");
		return USER_ERR_UNKONW_ERR;
	}
	//调整用户数据位置
	for (ui16i=ui16Idx; ui16i<(ui16Total-1); ui16i++)
	{
		iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+((size_t)(ui16i+1)*sizeof(S_UserData)), &sUserData, sizeof(sUserData));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_del_by_fd]Call 'file_read_by_fd()' fail.\r\n");
			return iRet;
		}
		iRet = file_write_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+((size_t)ui16i*sizeof(S_UserData)), &sUserData, sizeof(sUserData));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_del_by_fd]Call 'file_write_by_fd()' fail.\r\n");
			return iRet;
		}
	}
	//更新Total
	ui16Total--;
	iRet = file_write_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, sizeof(ui16Total));
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_del_by_fd]Call 'file_write_by_fd()' error.\r\n");
		return iRet;
	}
	//删除RAM中数据
	ui16Total = MUI_USER_COMMON_NUM_MAX;
	for (ui16i=0; ui16i<MUI_USER_COMMON_NUM_MAX; ui16i++)
	{
		if ('\0' == s_sUserStateData.asUserCache[ui16i].sUserData.name[0])
		{
			ui16Total = ui16i;
			break;	//发现name为空,即认为后面已经没有存储用户名了
		}
		if (s_sUserStateData.asUserCache[ui16i].ui16Idx == ui16Idx)
		{
			break;
		}
	}
	while (ui16i < ui16Total)
	{
		if (('\0'!=s_sUserStateData.asUserCache[ui16i+1].sUserData.name[0]) && (MUI_USER_COMMON_NUM_MAX>(ui16i+1)))
		{
			memcpy(&s_sUserStateData.asUserCache[ui16i], &s_sUserStateData.asUserCache[ui16i+1], sizeof(s_sUserStateData.asUserCache[ui16i]));
		}
		else
		{
			memset(&s_sUserStateData.asUserCache[ui16i], 0, sizeof(s_sUserStateData.asUserCache[ui16i]));
			break;
		}
		ui16i++;
	}
	//调整RAM中idx
	for (ui16i=0; ui16i<MUI_USER_COMMON_NUM_MAX; ui16i++)
	{
		if ('\0' == s_sUserStateData.asUserCache[ui16i].sUserData.name[0])
		{
			break;	//发现name为空,即认为后面已经没有存储用户名了
		}

		if (s_sUserStateData.asUserCache[ui16i].ui16Idx > ui16Idx)
		{
			s_sUserStateData.asUserCache[ui16i].ui16Idx--;
		}
	}

	return (int)ui16Idx;
}

static int s_user_rom_get_by_idx (uint16_t ui16Idx, S_UserData *psUserData)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Total;
	int iRet, iFd;

	ASSERT(MUI_USER_NUM_MAX > ui16Idx);
	ASSERT(NULL != psUserData);

	if (NULL == data_file_path_get(MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_get_by_idx]Can't get path of device %04hx:%04hx.\r\n", MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START);
		return FILE_S_ADDR_ERR;
	}

	iFd = open(achPath, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_get_by_idx]Call function 'open(%s)' fail, errno:%d\r\n", achPath, errno);
		return iFd;
	}

	do
	{
		iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, sizeof(ui16Total));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_get_by_idx]Call 'file_read_by_fd()' error.\r\n");
			return iRet;
		}
		if (ui16Idx >= ui16Total)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_get_by_idx]User data index error.\r\n");
			return USER_ERR_NO_FIND;
		}
		iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, asUserData)+((size_t)ui16Idx*sizeof(S_UserData)), &psUserData, sizeof(S_UserData));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_get_by_idx]Call 'file_read_by_fd()' error.\r\n");
		}
	} while (0);

	close(iFd);

	return iRet;
}

static int s_user_rom_checkout (const char *pchUserName, size_t tLen)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Total, ui16Idx;
	int iRet, iFd;

	ASSERT(NULL != pchUserName);
	ASSERT(MUI_USER_NAME_LEN >= tLen);

	if (NULL == data_file_path_get(MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_checkout]Can't get path of device %04hx:%04hx.\r\n", MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START);
		return FILE_S_ADDR_ERR;
	}

	iFd = open(achPath, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_checkout]Call function 'open(%s)' fail, errno:%d\r\n", achPath, errno);
		return iFd;
	}

	do
	{
		ui16Total = 0;
		iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, sizeof(ui16Total));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_checkout]Call 'file_read_by_fd()' error.\r\n");
			break;
		}

		iRet = s_user_rom_search_by_fd(iFd, pchUserName, tLen, 0, ui16Total);
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_checkout]Call function 's_user_rom_search_by_fd()' fail.\r\n");
			break;
		}
		ui16Idx = (uint16_t)iRet;
		iRet = s_user_rom_bring_in_ram_by_fd(iFd, ui16Idx);	//导入到RAM中,并挤走最后一个
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_checkout]Call 's_user_rom_bring_in_ram_by_fd()' fail.\r\n");
			break;
		}

		iRet = (int)ui16Idx;
	} while (0);

	close(iFd);

	return iRet;
}

static int s_user_rom_add (const char *pchUserName, size_t tUserNameLen, const char *pchUserDevName, size_t tUserDevNameLen)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	S_UserData sUserData;
	uint16_t ui16Total, ui16Idx;
	int iRet, iFd;

	ASSERT(NULL != pchUserName);
	ASSERT(MUI_USER_NAME_LEN >= tUserNameLen);
	ASSERT(NULL != pchUserDevName);
	ASSERT(MUI_USER_DEV_NAME_LEN >= tUserDevNameLen);

	if (NULL == data_file_path_get(MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_add]Can't get path of device %04hx:%04hx.\r\n", MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START);
		return FILE_S_ADDR_ERR;
	}

	iFd = open(achPath, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_add]Call function 'open(%s)' fail, errno:%d\r\n", achPath, errno);
		return iFd;
	}

	do
	{
		ui16Total = 0;
		iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, sizeof(ui16Total));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_add]Call 'file_read_by_fd()' error.\r\n");
			break;
		}

		if (ui16Total >= MUI_USER_NUM_MAX)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_add]Can't added new user to system, user total is %u\r\n", ui16Total);
			iRet = USER_ERR_NUM_ULTRALIMIT;
			break;
		}

		iRet = s_user_rom_search_by_fd(iFd, pchUserName, tUserNameLen, 0, ui16Total);
		if (iRet >= 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_add]Have find user name that want to add system.\r\n");
			iRet = USER_ERR_ALREADY_EXIST;
			break;
		}
		ui16Idx = (uint16_t)(iRet - MI_USER_ROM_SEARCH_NEW_OFFSET);

		//初始化用户数据
		memset(&sUserData, 0, sizeof(sUserData));
		memcpy(sUserData.name, pchUserName, tUserNameLen);
		memcpy(sUserData.dev_name, pchUserDevName, tUserDevNameLen);
		time(&sUserData.create_time);
		memcpy(&sUserData.last_use_time, &sUserData.create_time, sizeof(sUserData.last_use_time));
		iRet = s_user_rom_insert_by_fd(iFd, ui16Idx, &sUserData);
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_add]Call 's_user_rom_insert_by_fd()' fail.\r\n");
			break;
		}

		iRet = s_user_rom_bring_in_ram_by_fd(iFd, ui16Idx);	//导入到RAM中,并挤走最后一个
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_add]Call 's_user_rom_bring_in_ram_by_fd()' fail.\r\n");
			break;
		}

		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_add]Have added new user to <%u>.\r\n", ui16Idx);

		iRet = (int)ui16Idx;
	} while (0);

	close(iFd);

	return iRet;
}

static int s_user_rom_del (const char *pchName, size_t tLen)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Total, ui16Idx;
	int iRet, iFd;

	ASSERT(NULL != pchName);
	ASSERT(MUI_USER_NAME_LEN >= tLen);

	if (NULL == data_file_path_get(MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_del]Can't get path of device %04hx:%04hx.\r\n", MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START);
		return FILE_S_ADDR_ERR;
	}

	iFd = open(achPath, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_del]Call function 'open(%s)' fail, errno:%d\r\n", achPath, errno);
		return iFd;
	}

	do
	{
		ui16Total = 0;
		iRet = file_read_by_fd(iFd, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, sizeof(ui16Total));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_del]Call 'file_read_by_fd()' error.\r\n");
			break;
		}

		iRet = s_user_rom_search_by_fd(iFd, pchName, tLen, 0, ui16Total);
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_del]Call function 's_user_rom_search_by_fd()' fail.\r\n");
			break;
		}
		ui16Idx = (uint16_t)iRet;

		iRet = s_user_rom_del_by_fd(iFd, ui16Idx);
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_user_rom_del]Call 's_user_rom_insert_by_fd()' fail.\r\n");
			break;
		}

		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_user_rom_del]Have deleted user from <%u>.\r\n", ui16Idx);

		iRet = (int)ui16Idx;
	} while (0);

	close(iFd);

	return iRet;
}

















int user_data_ram_read (size_t tOffset, void *pvBuf, size_t tSize)
{
	UNUSED(tOffset);
	UNUSED(pvBuf);
	UNUSED(tSize);

	return 0;
}

int user_data_ram_write (size_t tOffset, const void *pvBuf, size_t tSize)
{
	UNUSED(tOffset);
	UNUSED(pvBuf);
	UNUSED(tSize);

	return 0;
}

int user_data_rom_read (size_t tOffset, void *pvBuf, size_t tSize)
{
	UNUSED(tOffset);
	UNUSED(pvBuf);
	UNUSED(tSize);

	return 0;
}

int user_data_rom_write (size_t tOffset, const void *pvBuf, size_t tSize)
{
	UNUSED(tOffset);
	UNUSED(pvBuf);
	UNUSED(tSize);

	return 0;
}

int user_data_ram_rom_write (size_t tRamOffset, const void *pvRamBuf, size_t tRamSize, size_t tRomOffset, const void *pvRomBuf, size_t tRomSize)
{
	UNUSED(tRamOffset);
	UNUSED(pvRamBuf);
	UNUSED(tRamSize);
	UNUSED(tRomOffset);
	UNUSED(pvRomBuf);
	UNUSED(tRomSize);

	return 0;
}

int user_data_total_get (void)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Total;
	int iRet;

	if (NULL == data_file_path_get(MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_total_get]Can't get path of device %04hx:%04hx.\r\n", MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START);
		return FILE_S_ADDR_ERR;
	}

	iRet = file_exclusive_read(achPath, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, M_SIZEOF(S_UserAttrData, ui16Total), &s_tUserRomMutex, MUI_USER_COMMON_NUM_MAX);
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_total_get]Call 'file_exclusive_read()' fail.\r\n");
		return iRet;
	}

	return (int)ui16Total;
}

int user_data_get_by_idx (uint16_t ui16Idx, S_UserData *psUserData)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tUserRomMutex, MUI_USER_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_ROM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_get_by_idx]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_user_rom_get_by_idx(ui16Idx, psUserData);
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_get_by_idx]Call 's_user_rom_get_by_idx()' fail.\r\n");
	}

	sem_post(&s_tUserRomMutex);

	return iRet;
}

int user_data_verify (const char *pchUserName, size_t tLen)
{
	int iRet;

	ASSERT(NULL != pchUserName);
	ASSERT(MUI_USER_NAME_LEN >= tLen);

	if (0 > sem_wait_ms(&s_tUserRomMutex, MUI_USER_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_ROM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_verify]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tUserRamMutex, MUI_USER_RAM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_RAM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_verify]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRamMutex);
		sem_post(&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	do
	{
		iRet = s_user_ram_search(pchUserName, tLen, 0, MUI_USER_COMMON_NUM_MAX);
		if (iRet >= 0)
		{
			s_user_ram_time_updata_by_offset(iRet);//调整最后使用时间
			iRet = s_user_ram_idx_get_by_offset(iRet);
			break;
		}
		iRet = s_user_rom_checkout(pchUserName, tLen);
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_verify]Call 's_user_rom_checkout()' fail.\r\n");
			break;
		}
	} while (0);

	s_user_ram_show();

	sem_post(&s_tUserRamMutex);
	sem_post(&s_tUserRomMutex);

	return iRet;
}

int user_data_add (const char *pchUserName, size_t tUserNameLen, const char *pchUserDevName, size_t tUserDevNameLen)
{
	int iRet;

	ASSERT(NULL != pchUserName);
	ASSERT(MUI_USER_NAME_LEN >= tUserNameLen);
	ASSERT(NULL != pchUserDevName);
	ASSERT(MUI_USER_DEV_NAME_LEN >= tUserDevNameLen);

	if (0 > sem_wait_ms(&s_tUserRomMutex, MUI_USER_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_ROM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tUserRamMutex, MUI_USER_RAM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_RAM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRamMutex);
		sem_post(&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	do
	{
		iRet = s_user_ram_search(pchUserName, tUserNameLen, 0, MUI_USER_COMMON_NUM_MAX);
		if (iRet >= 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_add]Have find this user name in RAM.\r\n");
			iRet = USER_ERR_ALREADY_EXIST;
			break;
		}
		iRet = s_user_rom_add(pchUserName, tUserNameLen, pchUserDevName, tUserDevNameLen);
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_add]Call 's_user_rom_add()' fail.\r\n");
			break;
		}

		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[user_data_add]Have added user:\r\n");
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "username:");
		USER_DATA_DYN_PUTS(M_DBG_LEVEL_DEBUG, pchUserName, tUserNameLen);
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\ndevicename:");
		USER_DATA_DYN_PUTS(M_DBG_LEVEL_DEBUG, pchUserDevName, tUserDevNameLen);
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	} while (0);

	s_user_ram_show();

	sem_post(&s_tUserRamMutex);
	sem_post(&s_tUserRomMutex);

	return iRet;
}

int user_data_del (const char *pchUserName, size_t tLen)
{
	int iRet;

	ASSERT(NULL != pchUserName);
	ASSERT(MUI_USER_NAME_LEN >= tLen);

	if (0 > sem_wait_ms(&s_tUserRomMutex, MUI_USER_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_ROM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_del]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tUserRamMutex, MUI_USER_RAM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_RAM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_del]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRamMutex);
		sem_post(&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_user_rom_del(pchUserName, tLen);
	if (iRet < 0)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_del]Call 's_user_rom_del()' fail.\r\n");
	}

	s_user_ram_show();

	sem_post(&s_tUserRamMutex);
	sem_post(&s_tUserRomMutex);

	return iRet;
}

int user_data_del_all (void)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Total;
	int iRet;

	if (0 > sem_wait_ms(&s_tUserRomMutex, MUI_USER_ROM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_ROM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_del]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tUserRamMutex, MUI_USER_RAM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_RAM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[user_data_del]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRamMutex);
		sem_post(&s_tUserRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	do
	{
		if (NULL == data_file_path_get(MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_del_all]Can't get path of user data.\r\n");
			iRet = FILE_S_ADDR_ERR;
			break;
		}
		ui16Total = 0;
		iRet = file_write(achPath, M_OFFSET(S_UserAttrData, ui16Total), &ui16Total, M_SIZEOF(S_UserAttrData, ui16Total));
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_del_all]Call 'file_write()' error.\r\n");
			break;
		}

		memset(&s_sUserStateData, 0, sizeof(s_sUserStateData));

		iRet = 0;
	} while (0);

	s_user_ram_show();

	sem_post(&s_tUserRamMutex);
	sem_post(&s_tUserRomMutex);

	return iRet;
}

int user_data_init (void)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t aui16IdList[MUI_USER_DATA_NUM_MAX];
	int iRet;

	iRet = sem_init(&s_tUserRomMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = sem_init(&s_tUserRamMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return iRet;
	}

	if (0 > sem_wait_ms(&s_tUserRamMutex, MUI_USER_RAM_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_USER_RAM_WAIT_TIME_MS_MAX)
	{
		USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_init]Wait semaphore %u over time.\r\n", (uint32_t)&s_tUserRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	memset(&s_sUserStateData, 0, sizeof(s_sUserStateData));

	iRet = data_id_exclusive_scan_by_type(MUI_DATA_TYPE_USER, aui16IdList, MUI_USER_DATA_NUM_MAX, &s_tUserRomMutex, MUI_USER_ROM_WAIT_TIME_MS_MAX);
	if ((iRet<MUI_USER_DATA_NUM_MAX) || (MUI_DATABASE_ID_START!=aui16IdList[0]))
	{
		//User文件未找到,现在创建该文件,并进行初始化
		if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_USER, achPath, sizeof(achPath)))
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_init]Can't get path of user type.\r\n");
			return FILE_S_ADDR_ERR;
		}
		iRet = mk_multi_dir(achPath, MUI_DATA_CREAT_MODE);
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_init]Can't execute mkdir for user.\r\n");
			return FILE_S_ADDR_ERR;
		}
		if (NULL == data_file_path_get(MUI_DATA_TYPE_USER, MUI_DATABASE_ID_START, achPath, sizeof(achPath)))
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_init]Can't get path of user data.\r\n");
			return FILE_S_ADDR_ERR;
		}
		aui16IdList[0] = 0;
		iRet = file_exclusive_write(achPath, M_OFFSET(S_UserAttrData, ui16Total), &aui16IdList[0], sizeof(aui16IdList[0]), &s_tUserRomMutex, MUI_USER_ROM_WAIT_TIME_MS_MAX);
		if (iRet < 0)
		{
			USER_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[user_data_init]Can't creat user data-%s to ROM.\r\n", achPath);
			return iRet;
		}
	}

	s_user_ram_show();

	sem_post(&s_tUserRamMutex);

	USER_DATA_DYN_PRINT(M_DBG_LEVEL_INFO, "[user_data_init]User data initialize successfully.\r\n");

	return 0;
}

int user_data_test (void)
{
	char achUserName[MUI_USER_NAME_LEN+1];
	char achUserDevName[MUI_USER_DEV_NAME_LEN+1];
	int i;

	for (i=0; i<MUI_USER_NUM_MAX+10; i++)
	{
		memset(achUserName, 0, sizeof(achUserName));
		memset(achUserDevName, 0, sizeof(achUserDevName));
		snprintf(achUserName, sizeof(achUserName), "User-%d", i);
		snprintf(achUserDevName, sizeof(achUserDevName), "User-Dev-%d", i);
		user_data_add(achUserName, strlen(achUserName), achUserDevName, strlen(achUserDevName));
	}

	for (i=10; i<20; i++)
	{
		memset(achUserName, 0, sizeof(achUserName));
		snprintf(achUserName, sizeof(achUserName), "User-%d", i);
		user_data_del(achUserName, strlen(achUserName));
	}

	for (i=0; i<MUI_USER_NUM_MAX+10; i++)
	{
		memset(achUserName, 0, sizeof(achUserName));
		snprintf(achUserName, sizeof(achUserName), "User-%d", i);
		user_data_verify(achUserName, strlen(achUserName));
	}

	for (i=0; i<MUI_USER_NUM_MAX+10; i++)
	{
		memset(achUserName, 0, sizeof(achUserName));
		memset(achUserDevName, 0, sizeof(achUserDevName));
		snprintf(achUserName, sizeof(achUserName), "User-%d", i);
		snprintf(achUserDevName, sizeof(achUserDevName), "User-Dev-%d", i);
		user_data_add(achUserName, strlen(achUserName), achUserDevName, strlen(achUserDevName));
	}

	for (i=0; i<MUI_USER_NUM_MAX+10; i++)
	{
		memset(achUserName, 0, sizeof(achUserName));
		snprintf(achUserName, sizeof(achUserName), "User-%d", i);
		user_data_verify(achUserName, strlen(achUserName));
	}

	return 0;
}

