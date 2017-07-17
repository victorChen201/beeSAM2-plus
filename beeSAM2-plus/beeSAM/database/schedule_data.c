#include "schedule_data.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SCHEDULE_DATA_DBG

#ifndef SCHEDULE_DATA_PUTC
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_PUTC(Char)			PUTC(Char)
#else
#define SCHEDULE_DATA_PUTC(Char)
#endif
#endif

#ifndef SCHEDULE_DATA_PUTS
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SCHEDULE_DATA_PUTS(Str, Len)
#endif
#endif

#ifndef SCHEDULE_DATA_PUTASCC
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_PUTASCC(Char)			PUTASCC(Char)
#else
#define SCHEDULE_DATA_PUTASCC(Char)
#endif
#endif

#ifndef SCHEDULE_DATA_PUTASCS
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SCHEDULE_DATA_PUTASCS(Str, Len)
#endif
#endif

#ifndef SCHEDULE_DATA_PRINT
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SCHEDULE_DATA_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SCHEDULE_DATA_DYN_PUTC
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SHADE|Dbg), Char)
#else
#define SCHEDULE_DATA_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SCHEDULE_DATA_DYN_PUTS
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SHADE|Dbg), Str, Len)
#else
#define SCHEDULE_DATA_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SCHEDULE_DATA_DYN_PUTASCC
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SHADE|Dbg), Char)
#else
#define SCHEDULE_DATA_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SCHEDULE_DATA_DYN_PUTASCS
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SHADE|Dbg), Str, Len)
#else
#define SCHEDULE_DATA_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SCHEDULE_DATA_DYN_PRINT
#ifdef SCHEDULE_DATA_DBG
#define SCHEDULE_DATA_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SHADE|Dbg), Fmt, ##Arg)
#else
#define SCHEDULE_DATA_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

#define MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX		3000

static sem_t s_tScheduleRomMutex;

static int s_schedule_data_add (const void *pvRomBuf, size_t tRomSize);
static int s_schedule_data_del (uint16_t ui16Id);
static int s_schedule_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
static int s_schedule_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id);


int schedule_data_init (void)
{
	int iRet;
	SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_init]schedule_data_init start\r\n");
	iRet = sem_init(&s_tScheduleRomMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_init] sem_init failed:%d\r\n", iRet);
		return iRet;
	}
	SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_init]schedule_data_init end\r\n");
	return iRet;
}


int schedule_data_get_id_list (uint16_t *pui16Buf, uint16_t ui16Capacity)
{
	ASSERT(NULL != pui16Buf);
	ASSERT(ui16Capacity <= MUI_SCHEDULE_DATA_NUM_MAX);

	return data_id_exclusive_scan_by_type(MUI_DATA_TYPE_SCHED, pui16Buf, ui16Capacity, &s_tScheduleRomMutex, MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX);
}

int schedule_data_assign_id (void)
{
	uint16_t aui16IdList[MUI_SCHEDULE_DATA_NUM_MAX];
	int iCnt, i;

	iCnt = data_id_scan_by_type(MUI_DATA_TYPE_SCHED, aui16IdList, MUI_SCHEDULE_DATA_NUM_MAX);
	if (iCnt < 0)
	{
		if (iCnt != FILE_S_ERR_DIR_NOT_EXIST)
		{
			SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[schedule_data_assign_id]Call 'data_id_scan_by_type(%04hx)' file.\r\n", MUI_DATA_TYPE_SCHED);
			return iCnt;
		}

		return MUI_DATABASE_ID_START;
	}

	if (iCnt >= MUI_SCHEDULE_DATA_NUM_MAX)
	{
		return SCHEDULE_ASSIGN_ID_ERROR;
	}

	for (i=0; i<iCnt; i++)
	{
		if (aui16IdList[i] != (i+MUI_DATABASE_ID_START))
		{
			break;
		}
	}

	return (i+MUI_DATABASE_ID_START);
}

static int s_schedule_data_add (const void *pvRomBuf, size_t tRomSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Id;
	int iRet;

	iRet = schedule_data_assign_id();
	if (iRet < MUI_DATABASE_ID_START)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_add]Call 'schedule_data_assign_id()' fail.\r\n");
		return iRet;
	}
	ui16Id = (uint16_t)iRet;

	//检测并创建目录
	if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_SCHED, achPath, sizeof(achPath)))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_add]Can't get path of schedule type\r\n");
		return FILE_S_ADDR_ERR;
	}
	iRet = mk_multi_dir(achPath, MUI_DATA_CREAT_MODE);
	if (iRet < 0)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_add]Can't execute mkdir for schedule type \r\n");
		return FILE_S_ADDR_ERR;
	}

	//创建ROM中的数据
	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCHED, ui16Id, achPath, sizeof(achPath)))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_add]Can't get path of schedule:%04hx.\r\n", ui16Id);
		iRet = FILE_S_ADDR_ERR;
	}
	iRet = file_write(achPath, 0, pvRomBuf, tRomSize);
	if (iRet < 0)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_add]Can't write new schedule data-%s to ROM.\r\n", achPath);
		return iRet;
	}

	SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[schedule_data_add]Have added schedule:%04hx.\r\n",  ui16Id);

	return (int)ui16Id;
}

static int s_schedule_data_del (uint16_t ui16Id)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	//删除ROM中的数据
	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCHED, ui16Id, achPath, sizeof(achPath)))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_del]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_del(achPath);
	if (iRet < 0)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_del]Can't delete device data-%s from ROM.\r\n", achPath);
		return iRet;
	}

	//删除无数据的目录
	if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_SCHED, achPath, sizeof(achPath)))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[schedule_data_del]Can't get path of schedule type.\r\n");
	}
	iRet = del_empty_dir(achPath);
	if (iRet < 0)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[schedule_data_del]Can't delete empty dir for schedule type.\r\n");
	}

	SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[schedule_data_del]Have deleted schedule:%04hx.\r\n",  ui16Id);

	return 0;
}

int schedule_data_read_info (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);

	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCHED, ui16Id, achPath, sizeof(achPath)))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_read_info]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_read(achPath, tOffset, pvBuf, tSize, &s_tScheduleRomMutex, MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX);
}

int schedule_data_write_info (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);

	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCHED, ui16Id, achPath, sizeof(achPath)))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_write_info]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_write(achPath, tOffset, pvBuf, tSize, &s_tScheduleRomMutex, MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX);
}

static int s_schedule_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId)
{
	char achSrcPath[MUI_FILE_ABS_PATH_LEN];
	char achDestPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	ASSERT(ui16DestId != ui16SrcId);

	//先修改ROM数据(先操作ROM数据再操作RAM数据,因为RAM数据只是内存操作没有失败可能,这样的顺序是为了防止回退操作)
	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16SrcId, achSrcPath, MUI_FILE_ABS_PATH_LEN))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_swap_id]Can't get path of device :%04hx.\r\n",  ui16SrcId);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16DestId, achDestPath, MUI_FILE_ABS_PATH_LEN))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_swap_id]Can't get path of device :%04hx.\r\n",  ui16DestId);
		return FILE_S_ADDR_ERR;
	}

	iRet = file_name_swap(achDestPath, achSrcPath);
	if (iRet < 0)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_schedule_data_swap_id]Call 'file_name_swap(%s, %s)' fail.\r\n", achDestPath, achSrcPath);
		return iRet;
	}

	return iRet;
}

static int s_schedule_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id)
{
	char achBuf[MUI_FILE_ABS_PATH_LEN*2];
	char *pchPath, *pchNewPath;
	int iRet;

	ASSERT(ui16NewId != ui16Id);

	pchPath    = &achBuf[MUI_FILE_ABS_PATH_LEN*0];
	pchNewPath = &achBuf[MUI_FILE_ABS_PATH_LEN*1];

	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCHED, ui16Id, pchPath, MUI_FILE_ABS_PATH_LEN))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_set_new_id]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCHED, ui16NewId, pchNewPath, MUI_FILE_ABS_PATH_LEN))
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_set_new_id]Can't get path of device :%04hx.\r\n",  ui16NewId);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_rename(pchNewPath, pchPath);
	if (iRet < 0)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_set_new_id]Call 'file_rename(%s, %s)' fail.\r\n", pchNewPath, pchPath);
		return iRet;
	}

	return iRet;
}

int schedule_data_add (const void *pvRomBuf, size_t tRomSize)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tScheduleRomMutex, MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[schedule_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tScheduleRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_schedule_data_add(pvRomBuf, tRomSize);

	sem_post(&s_tScheduleRomMutex);

	return iRet;
}


int schedule_data_del (uint16_t ui16Id)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tScheduleRomMutex, MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tScheduleRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_schedule_data_del(ui16Id);

	sem_post(&s_tScheduleRomMutex);

	return iRet;
}

int schedule_data_del_all (void)
{
	int iRet;
	int i;
	int dev_num = 0;
	uint16_t schedule_id_list[MUI_SCHEDULE_DATA_NUM_MAX];

	dev_num = schedule_data_get_id_list(schedule_id_list, MUI_SCHEDULE_DATA_NUM_MAX);
	if (dev_num < 0)
	{
		return 0;
	}

	for (i=0; i<dev_num; i++)
	{
		iRet = schedule_data_del(schedule_id_list[i]);
		if(iRet < 0)
		{
			SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[schedule_data_del]Can't delete schedule id = %d from ROM.\r\n", schedule_id_list[i]);
		}
	}

	return iRet;
}


int schedule_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tScheduleRomMutex, MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tScheduleRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_schedule_data_swap_id(ui16DestId, ui16SrcId);

	sem_post(&s_tScheduleRomMutex);

	return iRet;
}

int schedule_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tScheduleRomMutex, MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tScheduleRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_schedule_data_set_new_id(ui16NewId, ui16Id);

	sem_post(&s_tScheduleRomMutex);

	return iRet;
}

/*int schedule_data_add_member (uint16_t ui16Id, void *pvActionList, const size_t tSize, uint16_t ui16ActionNum)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tScheduleRomMutex, MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCHEDULE_DATA_WAIT_TIME_MS_MAX)
	{
		SCHEDULE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tScheduleRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_schedule_data_add_member(ui16Id, pvActionList, tSize, ui16ActionNum);

	sem_post(&s_tScheduleRomMutex);

	return iRet;
}*/











































































































































