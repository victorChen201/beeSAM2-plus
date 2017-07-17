/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of at_port module for beeSAM project.
File name:at_port.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "file_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define FILE_LINUX_DBG

#ifndef FILE_LINUX_PUTC
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_PUTC(Char)				PUTC(Char)
#else
#define FILE_LINUX_PUTC(Char)
#endif
#endif

#ifndef FILE_LINUX_PUTS
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_PUTS(Str, Len)			PUTS(Str, Len)
#else
#define FILE_LINUX_PUTS(Str, Len)
#endif
#endif

#ifndef FILE_LINUX_PUTASCC
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_PUTASCC(Char)			PUTASCC(Char)
#else
#define FILE_LINUX_PUTASCC(Char)
#endif
#endif

#ifndef FILE_LINUX_PUTASCS
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define FILE_LINUX_PUTASCS(Str, Len)
#endif
#endif

#ifndef FILE_LINUX_PRINT
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define FILE_LINUX_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef FILE_LINUX_DYN_PUTC
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_FILE_LINUX|Dbg), Char)
#else
#define FILE_LINUX_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef FILE_LINUX_DYN_PUTS
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_FILE_LINUX|Dbg), Str, Len)
#else
#define FILE_LINUX_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef FILE_LINUX_DYN_PUTASCC
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_FILE_LINUX|Dbg), Char)
#else
#define FILE_LINUX_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef FILE_LINUX_DYN_PUTASCS
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_FILE_LINUX|Dbg), Str, Len)
#else
#define FILE_LINUX_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef FILE_LINUX_DYN_PRINT
#ifdef FILE_LINUX_DBG
#define FILE_LINUX_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_FILE_LINUX|Dbg), Fmt, ##Arg)
#else
#define FILE_LINUX_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MSTR_FACTORY_FILE_NAME				"factory"
#define MSTR_DEV_STATE_BACKUP_FILE_NAME		"dev_state"

#define MSTR_DATA_FILE_SUFFIX				"dat"
#define MSTR_DATA_EXTEND_FILE_SUFFIX		"ext"
#define MSTR_DATA_BACKUP_FILE_SUFFIX		"bak"
#define MSTR_DATA_TMP_FILE_SUFFIX			"~"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static S_DirAbsPath s_sDataDirAbsPath;

static const uint16_t s_aui16DataNumMax[MUI_DATA_TYPE_NUM] =
{
	MUI_CONFIG_DATA_NUM_MAX,
	MUI_USER_DATA_NUM_MAX,
	MUI_GROUP_DATA_NUM_MAX,
	MUI_SCENE_DATA_NUM_MAX,
	MUI_SET_DATA_NUM_MAX,
	MUI_SCHEDULE_DATA_NUM_MAX,
	MUI_RULE_DATA_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_LOGIC_DEV_NUM_MAX,
	MUI_EXTEND_DATA_NUM_MAX
};

static const char s_aachDataDirName[MUI_DATA_TYPE_NUM][MUI_DATA_DIR_NAME_LEN_MAX] =
{
	"config",
	"user",
	"group",
	"scene",
	"set",
	"schedule",
	"rule",
	"light",
	"occ_sensor",
	"bin_sensor",
	"temp_sensor",
	"hum_sensor",
	"th_sensor",
	"bin_ctrl",
	"level_ctrl",
	"warn_dev",
	"plug",
	"switch",
	"shade",
	"sonos",
	"panel",
	"extend"
};



//-------------------------------------------------------------------------------------------------*
/*	创建多层目录函数
;	(功能)
;	(参数)
;		pchPath:	路径(可以是相对路径也可以是绝对路径)
;		tMode:		模式
;	(返回)
;		int:		成功返回0
;	(注意)
*/
int mk_multi_dir (const char *pchPath, mode_t tMode)
{
	char achPath[MUI_FILE_ABS_PATH_LEN+1] = {0};	//要求路径总长度不超过MUI_FILE_ABS_PATH_LEN个字符,最后+1是用来存储可能会增加的/
	int i = 0;

	ASSERT(NULL != pchPath);

	while ('\0' != pchPath[i])
	{
		if (i > (int)sizeof(achPath)-1-1)
		{
			FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[mk_multi_dir]Path too long, length can't large than %u.\r\n", MUI_FILE_ABS_PATH_LEN);
			return -1;
		}
		achPath[i] = pchPath[i];
		i++;
	}
	if (0 == i)
	{
		return 0;
	}
	if (achPath[i-1] != '/')
	{
		achPath[i] = '/';
	}

	i = 1;
	while ('\0' != achPath[i])
	{
		if ('/' == achPath[i])
		{
			achPath[i] = '\0';
			if (access(achPath, F_OK) != 0)
			{
				FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[mk_multi_dir]'%s' not exist, will creat it.\r\n", achPath);
				if (mkdir(achPath, tMode) < 0)
				{
					FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[mk_multi_dir]Can not mkdir '%s', errno:%d.\r\n", achPath, errno);
					return -1;
				}
			}
			achPath[i] = '/';
		}
		i++;
	}

	return 0;
}
//..............................................................................
/*	删除空目录函数
;	(功能)
;	(参数)
;		pchPath:	路径(可以是相对路径也可以是绝对路径)
;	(返回)
;		int:			成功返回0
;						失败返回负数
;	(注意)
*/
int del_empty_dir (const char *pchPath)
{
	ASSERT(NULL != pchPath);

	return remove(pchPath);
}
//..............................................................................
/*	删除录函数
;	(功能)
;	(参数)
;		pchPath:	路径(可以是相对路径也可以是绝对路径)
;	(返回)
;		int:			成功返回0
;						失败返回负数
;	(注意)
*/
int del_dir (const char *pchPath)
{
	ASSERT(NULL != pchPath);

	return remove(pchPath);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	产生绝对路径函数
;	(功能)
;	(参数)
;		pchPath:	路径(可以是相对路径也可以是绝对路径,如果pchPath==NULL,则表示是./)
;		tMode:		模式
;		pchAbsPath:	绝对路径回写缓存
;		tSize:		缓存大小
;	(返回)
;		int:		成功返回路径的长度
;					失败返回负数
;	(注意)
*/
int file_gen_abs_path (const char *pchPath, mode_t tMode, char *pchAbsPath, size_t tSize)
{
	char *pchTmp;
	int iRet;

	ASSERT(NULL != pchAbsPath);
	ASSERT(tSize >= MUI_FILE_ABS_PATH_LEN);

	//Get data file absolute path
	if ((NULL==pchPath) || ('\0'==pchPath[0]))
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_INFO, "[file_gen_abs_path]Origin path:'%s'.\r\n", ".");
		pchTmp = realpath(".", pchAbsPath);	//有内存溢出风险
	}
	else
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_INFO, "[file_gen_abs_path]Origin path:'%s'.\r\n", pchPath);
		if (access(pchPath, F_OK) != 0)
		{
			FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[file_gen_abs_path]Call function 'chdir(%s)' fail, errno:%d\r\n", pchPath, errno);
			FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[file_gen_abs_path]Will creat path:'%s'.\r\n", pchPath);
			iRet = mk_multi_dir(pchPath, tMode);
			if (iRet < 0)
			{
				FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_gen_abs_path]Call function 'mkdir(%s, %u)' fail, errno:%d\r\n", pchPath, (uint32_t)tMode, errno);
				return iRet;
			}
		}

		pchTmp = realpath(pchPath, pchAbsPath);	//有内存溢出风险
	}

	if (NULL == pchTmp)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_gen_abs_path]Call function 'realpath()' fail, errno:%d\r\n", errno);
		return -1;
	}
	FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_INFO, "[file_gen_abs_path]Absolute Path:'%s'.\r\n", pchAbsPath);

	return (strlen(pchAbsPath));
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	文件大小获取函数
;	(功能)
;	(参数)
;		pchFileName:	文件路径
;	(返回)
;		int:			正常返回文件大小
;	(注意)
*/
int chandler_file_size_get (const char *pchFileName)
{
#if 1
	int iRet;
	struct stat sStat;

	ASSERT(NULL != pchFileName);

	iRet = stat(pchFileName, &sStat);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[chandler_file_size_get]Call 'stat(%s, &sStat)' fail, errno:%d.\r\n", pchFileName, errno);
		return iRet;
	}

	return (int)sStat.st_size;
#else
	unsigned long ulSize;
	FILE *pf;

	ASSERT(NULL != pchFileName);

	pf = fopen(pchFileName, "rb");
	if (NULL == pf)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[chandler_file_size_get]Call function 'fopen(%s, \"rb\")' fail, errno:%d\r\n", pchFileName, errno);
		return -1;
	}
	fseek(pf, SEEK_SET, SEEK_END);
	ulSize = ftell(pf);
	fclose(pf);

	return (int)ulSize;
#endif
}
//..............................................................................
/*	文件读函数
;	(功能)
;	(参数)
;		iFd:		文件句柄
;		tOffset:	偏移地址
;		pvBuf:		数据Buffer
;		tSize:		缓存大小
;	(返回)
;		int:		正常返回实际读取数据的大小
;	(注意)
*/
int file_read_by_fd (int iFd, size_t tOffset, void *pvBuf, size_t tSize)
{
	int iRet;

	ASSERT(0 <= iFd);
	ASSERT(NULL != pvBuf);
	ASSERT(tSize <= 0x7FFFFFFF);	//因为int返回类型最大能表征的正数是0x7FFFFFFF

	iRet = (int)lseek(iFd, tOffset, SEEK_SET);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_read_by_fd]Call function 'lseek()' fail, errno:%d\r\n", errno);
		return iRet;
	}

	iRet = read(iFd, pvBuf, tSize);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_read_by_fd]Call function 'read()' fail, errno:%d\r\n", errno);
	}

	return iRet;
}
//..............................................................................
/*	文件经句柄写函数
;	(功能)
;	(参数)
;		iFd:		文件句柄
;		tOffset:	偏移地址
;		pvBuf:		数据Buffer
;		tLen:		数据大小
;	(返回)
;		int:		正常返回实际读取数据的大小
;	(注意)
*/
int file_write_by_fd (int iFd, size_t tOffset, const void *pvBuf, size_t tLen)
{
	int iRet;

	ASSERT(0 <= iFd);
	ASSERT(NULL != pvBuf);
	ASSERT(tLen <= 0x7FFFFFFF);	//因为int返回类型最大能表征的正数是0x7FFFFFFF

	iRet = (int)lseek(iFd, tOffset, SEEK_SET);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_write_by_fd]Call function 'lseek()' fail, errno:%d\r\n", errno);
		return iRet;
	}

	iRet = write(iFd, pvBuf, tLen);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_write_by_fd]Call function 'write()' fail, errno:%d\r\n", errno);
	}

	return iRet;
}
//..............................................................................
/*	文件读函数
;	(功能)
;	(参数)
;		pchFileName:	文件路径
;		tOffset:		偏移地址
;		pvBuf:			数据Buffer
;		tSize:			缓存大小
;	(返回)
;		int:			正常返回实际读取数据的大小
;	(注意)
*/
int file_read (const char *pchFileName, size_t tOffset, void *pvBuf, size_t tSize)
{
	int iRet, iFd;

	ASSERT(NULL != pchFileName);
	ASSERT(NULL != pvBuf);
	ASSERT(tSize <= 0x7FFFFFFF);	//因为int返回类型最大能表征的正数是0x7FFFFFFF

	iFd = open(pchFileName, O_RDONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_read]Call function 'open(%s)' fail, errno:%d\r\n", pchFileName, errno);
		iRet = iFd;
	}
	else
	{
		iRet = file_read_by_fd(iFd, tOffset, pvBuf, tSize);
		close(iFd);
	}

	return iRet;
}
//..............................................................................
/*	文件写函数
;	(功能)
;	(参数)
;		pchFileName:	文件路径
;		tOffset:		偏移地址
;		pvBuf:			数据Buffer
;		tLen:			数据大小
;	(返回)
;		int:			正常返回实际写入数据的大小
;	(注意)
*/
int file_write (const char *pchFileName, size_t tOffset, const void *pvBuf, size_t tLen)
{
	int iRet, iFd;

	ASSERT(NULL != pchFileName);
	ASSERT(NULL != pvBuf);
	ASSERT(tLen <= 0x7FFFFFFF);	//因为int返回类型最大能表征的正数是0x7FFFFFFF

	iFd = open(pchFileName, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_write]Call function 'open(%s)' fail, errno:%d\r\n", pchFileName, errno);
		iRet = iFd;
	}
	else
	{
		iRet = file_write_by_fd(iFd, tOffset, pvBuf, tLen);
		close(iFd);
	}

	return iRet;
}
//..............................................................................
/*	文件删除函数
;	(功能)
;	(参数)
;		pchFileName:	文件路径
;	(返回)
;		int:			成功返回0
;						失败返回负数
;	(注意)
*/
int file_del (const char *pchFileName)
{
	int iRet;

	ASSERT(NULL != pchFileName);

	iRet = remove(pchFileName);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_del]Call 'remove(%s)' fail, errno:%d.\r\n", pchFileName, errno);
	}

	return iRet;
}
//..............................................................................
/*	文件拷贝函数
;	(功能)
;	(参数)
;		pchDestFile:	目标文件
;		pchSrcFile:		源头文件
;	(返回)
;		int:			成功返回0
;						失败返回负数
;	(注意)
*/
int file_cp (const char *pchDestFile, const char *pchSrcFile)
{
	uint8_t aui8Buf[1024];
	FILE *pfR, *pfW;
	int iRLen, iWLen, iRet;

	ASSERT(NULL != pchDestFile);
	ASSERT(NULL != pchSrcFile);

	if (NULL == (pfR=fopen(pchSrcFile, "r")))
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_cp]Can't open file '%s', errno:%d\r\n", pchSrcFile, errno);
		return FILE_S_NOFIND_ERR;
	}
	if (NULL == (pfW=fopen(pchDestFile, "w")))
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_cp]Can't open file '%s', errno:%d\r\n", pchDestFile, errno);
		fclose(pfR);
		return FILE_S_NOFIND_ERR;
	}

	iRet = 0;
	do
	{
		iRLen = fread(aui8Buf, 1, sizeof(aui8Buf), pfR);
		if (iRLen < 0)
		{
			FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_cp]Call 'fread()' fail, error:%d\r\n", errno);
			iRet = iRLen;
			break;
		}
		iWLen = fwrite(aui8Buf, 1, iRLen, pfW);
		if (iWLen != iRLen)
		{
			FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_cp]Call 'fwrite()' fail, error:%d\r\n", errno);
			iRet = -1;
			break;
		}
	} while (sizeof(aui8Buf) == iRLen);

	fclose(pfW);
	fclose(pfR);

	return iRet;
}
//..............................................................................
/*	文件拷贝函数
;	(功能)
;	(参数)
;		pchDestFile:	目标文件
;		pchSrcFile:		源头文件
;	(返回)
;		int:			成功返回0
;						失败返回负数
;	(注意)
*/
int file_rename (const char *pchDestFile, const char *pchSrcFile)
{
	int iRet;

	iRet = rename(pchSrcFile, pchDestFile);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[file_rename]Call 'rename(%s, %s) fail, errno:%d.\r\n", pchSrcFile, pchDestFile, errno);
	}

	return iRet;
}
//..............................................................................
/*	文件交换函数
;	(功能)
;	(参数)
;		pchFile1:	文件1
;		pchFile2:	文件2
;	(返回)
;		int:			成功返回0
;						失败返回负数
;	(注意)
*/
int file_name_swap (const char *pchFile1, const char *pchFile2)
{
	char achTmpPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	ASSERT(NULL != pchFile1);
	ASSERT(NULL != pchFile2);

	strncpy(achTmpPath, pchFile1, sizeof(achTmpPath));
	strncat(achTmpPath, MSTR_DATA_TMP_FILE_SUFFIX, sizeof(MSTR_DATA_TMP_FILE_SUFFIX)-1);
	iRet = file_rename(achTmpPath, pchFile1);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_swap_id]Call 'file_rename(%s, %s)' fail.\r\n", achTmpPath, pchFile1);
		return iRet;
	}
	iRet = file_rename(pchFile1, pchFile2);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_swap_id]Call 'file_rename(%s, %s)' fail.\r\n", pchFile1, pchFile2);
		return iRet;
	}
	iRet = file_rename(pchFile2, achTmpPath);
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_swap_id]Call 'file_rename(%s, %s)' fail.\r\n", pchFile2, achTmpPath);
		return iRet;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	文件独占读函数
;	(功能)
;	(参数)
;		pchFileName:	文件路径
;		tOffset:		偏移地址
;		pvBuf:			数据Buffer
;		tSize:			缓存大小
;		ptMutx:			信号量指针
;		ui32WaitMs:		等待信号量超时ms数
;	(返回)
;		int:			正常返回实际读取数据的大小
;	(注意)
*/
int file_exclusive_read (const char *pchFileName, size_t tOffset, void *pvBuf, size_t tSize, sem_t *ptMutx, uint32_t ui32WaitMs)
{
	int iRet;

	ASSERT(NULL != pchFileName);
	ASSERT(NULL != pvBuf);
	ASSERT(tSize <= 0x7FFFFFFF);	//因为int返回类型最大能表征的正数是0x7FFFFFFF

	if ((NULL!=ptMutx) && (0>sem_wait_ms(ptMutx, ui32WaitMs)))
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_WARNING, "[file_exclusive_read]Wait semaphore %u over time.\r\n", (uint32_t)ptMutx);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = file_read(pchFileName, tOffset, pvBuf, tSize);

	if (NULL != ptMutx)
	{
		sem_post(ptMutx);
	}

	return iRet;
}
//..............................................................................
/*	文件独占写函数
;	(功能)
;	(参数)
;		pchFileName:	文件路径
;		tOffset:		偏移地址
;		pvBuf:			数据Buffer
;		tLen:			数据大小
;		ptMutx:			信号量指针
;		ui32WaitMs:		等待信号量超时ms数
;	(返回)
;		int:			正常返回实际写入数据的大小
;	(注意)
*/
int file_exclusive_write (const char *pchFileName, size_t tOffset, const void *pvBuf, size_t tLen, sem_t *ptMutx, uint32_t ui32WaitMs)
{
	int iRet;

	ASSERT(NULL != pchFileName);
	ASSERT(NULL != pvBuf);
	ASSERT(tLen <= 0x7FFFFFFF);	//因为int返回类型最大能表征的正数是0x7FFFFFFF

	if ((NULL!=ptMutx) && (0>sem_wait_ms(ptMutx, ui32WaitMs)))
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_WARNING, "[file_exclusive_write]Wait semaphore %u over time.\r\n", (uint32_t)ptMutx);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = file_write(pchFileName, tOffset, pvBuf, tLen);

	if (NULL != ptMutx)
	{
		sem_post(ptMutx);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*

int data_idx_cmp (const void *pv1, const void *pv2)
{
	//此项表达式建立的前提是tDevIdx不会超过0x7FFFFFFF,也就是Type不能超过0x7FFF
	return ((int)(*((uint32_t*)pv1)) - (int)(*((uint32_t*)pv2)));
}

int data_id_cmp (const void *pv1, const void *pv2)
{
	return ((int)(*((uint16_t*)pv1)) - (int)(*((uint16_t*)pv2)));
}

int data_num_max_get_by_type (uint16_t ui16Type)
{
	return s_aui16DataNumMax[ui16Type-MUI_DATA_TYPE_START];
}

int data_root_path_cfg (const char *pchPath)
{
	int iRet;

	ASSERT(NULL != pchPath);

	iRet = file_gen_abs_path(pchPath, MUI_DATA_CREAT_MODE, s_sDataDirAbsPath.achPath, sizeof(s_sDataDirAbsPath.achPath));
	if (iRet < 0)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_ERR, "[data_root_path_cfg]Can't generate abstract path of '%s'\r\n", pchPath);
		return FILE_S_ADDR_ERR;
	}
	s_sDataDirAbsPath.ui8Len = (uint8_t)iRet;

	FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[data_root_path_cfg]ROM root path[%dbytes]:%s\r\n", iRet, s_sDataDirAbsPath.achPath);

	return iRet;
}

const char* data_root_path_get (void)
{
	return s_sDataDirAbsPath.achPath;
}

int data_root_path_len_get (void)
{
	return (int)s_sDataDirAbsPath.ui8Len;
}

const char* data_factory_file_path_get (char *pchPath, size_t tSize)
{
	ASSERT(tSize >= ((size_t)data_root_path_len_get()+sizeof(MSTR_FACTORY_FILE_NAME)-1+1+sizeof(MSTR_DATA_FILE_SUFFIX)-1));

	memcpy(pchPath, data_root_path_get(), data_root_path_len_get());
	snprintf(pchPath+data_root_path_len_get(), tSize-data_root_path_len_get(), "/"MSTR_FACTORY_FILE_NAME"."MSTR_DATA_FILE_SUFFIX);

	//FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[data_factory_file_path_get]Path:%s\r\n", pchPath);

	return pchPath;
}

const char* data_dev_state_backup_file_path_get (char *pchPath, size_t tSize)
{
	ASSERT(tSize >= ((size_t)data_root_path_len_get()+sizeof(MSTR_DEV_STATE_BACKUP_FILE_NAME)-1+1+sizeof(MSTR_DATA_BACKUP_FILE_SUFFIX)-1));

	memcpy(pchPath, data_root_path_get(), data_root_path_len_get());
	snprintf(pchPath+data_root_path_len_get(), tSize-data_root_path_len_get(), "/"MSTR_DEV_STATE_BACKUP_FILE_NAME"."MSTR_DATA_BACKUP_FILE_SUFFIX);

	//FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[data_dev_state_backup_file_path_get]Path:%s\r\n", pchPath);

	return pchPath;
}

const char* data_dir_path_get_by_type (uint16_t ui16Type, char *pchPath, size_t tSize)
{
	ASSERT((MUI_DATA_TYPE_START<=ui16Type)&&(ui16Type<=MUI_DATA_TYPE_END));
	ASSERT(NULL != pchPath);
	ASSERT(NULL != pchPath);
	ASSERT(tSize >= (data_root_path_len_get()+(size_t)MUI_DATA_DIR_NAME_LEN_MAX));

	memcpy(pchPath, data_root_path_get(), data_root_path_len_get());
	pchPath[data_root_path_len_get()] = '/';
	//此处出于安全考虑本应使用strncpy,但由于它的效率较低,且由于s_sDataDirAbsPath是常量定义,
	//其中的字符串不可能出现问题,所以还是决定使用strcpy函数.
	strcpy(pchPath+data_root_path_len_get()+1, s_aachDataDirName[ui16Type-MUI_DATA_TYPE_START]);

	//FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[data_dir_path_get_by_type]Path:%s\r\n", pchPath);

	return pchPath;
}

const char* data_file_path_get (uint16_t ui16Type, uint16_t ui16Id, char *pchPath, size_t tSize)
{
	ASSERT((MUI_DATA_TYPE_START<=ui16Type)&&(ui16Type<=MUI_DATA_TYPE_END));
	ASSERT(MUI_DATA_TYPE_EXTEND != ui16Type);
	ASSERT(NULL != pchPath);
	ASSERT(tSize >= ((size_t)data_root_path_len_get()+(size_t)MUI_DATA_DIR_NAME_LEN_MAX+(size_t)MUI_DATA_FILE_NAME_LEN_MAX));

	memcpy(pchPath, data_root_path_get(), data_root_path_len_get());
	snprintf(pchPath+data_root_path_len_get(), tSize-data_root_path_len_get(), "/%s/%08x."MSTR_DATA_FILE_SUFFIX,
			 s_aachDataDirName[ui16Type-MUI_DATA_TYPE_START], M_GEN_IDX_BY_TYPE_ID(ui16Type, ui16Id));

	//FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[data_file_path_get]Path:%s\r\n", pchPath);

	return pchPath;
}

const char* data_extend_file_path_get (uint16_t ui16Type, uint16_t ui16Id, char *pchPath, size_t tSize)
{
	ASSERT((MUI_DATA_TYPE_START<=ui16Type)&&(ui16Type<=MUI_DATA_TYPE_END));
	ASSERT(MUI_DATA_TYPE_EXTEND != ui16Type);
	ASSERT(NULL != pchPath);
	ASSERT(tSize >= ((size_t)data_root_path_len_get()+(size_t)MUI_DATA_DIR_NAME_LEN_MAX+(size_t)MUI_DATA_FILE_NAME_LEN_MAX));

	memcpy(pchPath, data_root_path_get(), data_root_path_len_get());
	snprintf(pchPath+data_root_path_len_get(), tSize-data_root_path_len_get(), "/%s/%08x."MSTR_DATA_EXTEND_FILE_SUFFIX,
			 s_aachDataDirName[MUI_DATA_TYPE_EXTEND-MUI_DATA_TYPE_START], M_GEN_IDX_BY_TYPE_ID(ui16Type, ui16Id));

	//FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[data_extend_file_path_get]Path:%s\r\n", pchPath);

	return pchPath;
}

int data_id_scan_by_type (uint16_t ui16Type, uint16_t *pui16Buf, uint16_t ui16Capacity)
{
	char achDirPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t aui16IdList[MUI_LOGIC_DEV_NUM_MAX];
	uint32_t ui32Idx;
	int iCnt;
	DIR *ptDir = NULL;
	struct dirent *psDirEntry;

	ASSERT((MUI_DATA_TYPE_START<=ui16Type)&&(ui16Type<=MUI_DATA_TYPE_END));
	ASSERT(MUI_DATA_TYPE_EXTEND != ui16Type);
	ASSERT(NULL != pui16Buf);

	if (NULL == data_dir_path_get_by_type(ui16Type, achDirPath, sizeof(achDirPath)))
	{
		return FILE_S_ADDR_ERR;
	}

	ptDir = opendir(achDirPath);
	if (NULL == ptDir)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_INFO, "[data_id_scan_by_type]Can't open directory %s, errno:%d\r\n", achDirPath, errno);
		return FILE_S_ERR_DIR_NOT_EXIST;
	}

	iCnt = 0;
	while (NULL != (psDirEntry=readdir(ptDir)))
	{
		if (1 != sscanf(psDirEntry->d_name, "%08x."MSTR_DATA_FILE_SUFFIX, &ui32Idx))
		{
			continue;
		}
		if (M_GET_TYPE_FROM_IDX(ui32Idx) != ui16Type)
		{
			continue;
		}
		if (iCnt > ui16Capacity)
		{
			iCnt = -1;
			break;
		}
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[data_id_scan_by_type]Have find type:%04hx device:%s\r\n", ui16Type, psDirEntry->d_name);
		aui16IdList[iCnt] = M_GET_ID_FROM_IDX(ui32Idx);
		iCnt++;
	}

	closedir(ptDir);

	if (iCnt < 0)
	{
		return FILE_S_BUF_ERR;
	}

	FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[data_id_scan_by_type]Have find %d devices of type:%04hx\r\n", iCnt, ui16Type);

	//对Buffer中ID排序
	qsort(aui16IdList, iCnt, sizeof(aui16IdList[0]), data_id_cmp);
	memcpy(pui16Buf, aui16IdList, iCnt*sizeof(aui16IdList[0]));

#if 1
	int i;
	for (i=0; i<iCnt; i++)
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%04hx ", aui16IdList[i]);
	}
	FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
#endif

	return iCnt;
}

int data_id_exclusive_scan_by_type (uint16_t ui16Type, uint16_t *pui16Buf, uint16_t ui16Capacity, sem_t *ptMutx, uint32_t ui32WaitMs)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_START<=ui16Type)&&(ui16Type<=MUI_DATA_TYPE_END));
	ASSERT(MUI_DATA_TYPE_EXTEND != ui16Type);
	ASSERT(NULL != pui16Buf);

	if ((NULL!=ptMutx) && (0>sem_wait_ms(ptMutx, ui32WaitMs)))
	{
		FILE_LINUX_DYN_PRINT(M_DBG_LEVEL_WARNING, "[data_id_exclusive_scan_by_type]Wait semaphore %u over time.\r\n", (uint32_t)ptMutx);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = data_id_scan_by_type(ui16Type, pui16Buf, ui16Capacity);

	if (NULL != ptMutx)
	{
		sem_post(ptMutx);
	}

	return iRet;
}

