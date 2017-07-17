#include "group_data.h"
#include "sys_time_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define GROUP_DATA_DBG

#ifndef GROUP_DATA_PUTC
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_PUTC(Char)			PUTC(Char)
#else
#define GROUP_DATA_PUTC(Char)
#endif
#endif

#ifndef GROUP_DATA_PUTS
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define GROUP_DATA_PUTS(Str, Len)
#endif
#endif

#ifndef GROUP_DATA_PUTASCC
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_PUTASCC(Char)			PUTASCC(Char)
#else
#define GROUP_DATA_PUTASCC(Char)
#endif
#endif

#ifndef GROUP_DATA_PUTASCS
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define GROUP_DATA_PUTASCS(Str, Len)
#endif
#endif

#ifndef GROUP_DATA_PRINT
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define GROUP_DATA_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef GROUP_DATA_DYN_PUTC
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_GROUP|Dbg), Char)
#else
#define GROUP_DATA_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef GROUP_DATA_DYN_PUTS
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_GROUP|Dbg), Str, Len)
#else
#define GROUP_DATA_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef GROUP_DATA_DYN_PUTASCC
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_GROUP|Dbg), Char)
#else
#define GROUP_DATA_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef GROUP_DATA_DYN_PUTASCS
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_GROUP|Dbg), Str, Len)
#else
#define GROUP_DATA_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef GROUP_DATA_DYN_PRINT
#ifdef GROUP_DATA_DBG
#define GROUP_DATA_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_GROUP|Dbg), Fmt, ##Arg)
#else
#define GROUP_DATA_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_GROUP_DATA_WAIT_TIME_MS_MAX	3000

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static sem_t s_tGroupRomMutex;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int s_group_data_assign_id (void);
static int s_group_data_add (const S_GroupAttrFlash *pvGroupFlash);
static int s_group_data_del (uint16_t ui16Id);
static int s_group_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
static int s_group_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id);
static int s_group_data_add_member (uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType);
static int s_group_data_del_member (uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType);

//-------------------------------------------------------------------------------------------------*
static int s_group_data_assign_id (void)
{
	uint16_t aui16IdList[MUI_GROUP_DATA_NUM_MAX];
	int iCnt, i;

	iCnt = data_id_scan_by_type(MUI_DATA_TYPE_GROUP, aui16IdList, MUI_GROUP_DATA_NUM_MAX);
	if (iCnt < 0)
	{
		if (iCnt != FILE_S_ERR_DIR_NOT_EXIST)
		{
			GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_group_data_assign_id]Call 'data_id_scan_by_type(%04hx)' file.\r\n", MUI_DATA_TYPE_GROUP);
			return iCnt;
		}

		return MUI_DATABASE_ID_START;
	}

	if (iCnt >= MUI_GROUP_DATA_NUM_MAX)
	{
		return GROUP_ASSIGN_ID_ERROR;
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

static int s_group_data_add (const S_GroupAttrFlash *pvGroupFlash)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Id;
	int iRet;

	iRet = s_group_data_assign_id();
	if (iRet < MUI_DATABASE_ID_START)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_add]Call 's_group_data_assign_id()' fail.\r\n");
		return iRet;
	}
	ui16Id = (uint16_t)iRet;

	//检测并创建目录
	if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_GROUP, achPath, sizeof(achPath)))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_add]Can't get path of group type\r\n");
		return FILE_S_ADDR_ERR;
	}
	iRet = mk_multi_dir(achPath, MUI_DATA_CREAT_MODE);
	if (iRet < 0)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_add]Can't execute mkdir for group type \r\n");
		return FILE_S_ADDR_ERR;
	}

	//创建ROM中的数据
	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16Id, achPath, sizeof(achPath)))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_add]Can't get path of group:%04hx.\r\n", ui16Id);
		iRet = FILE_S_ADDR_ERR;
	}
	iRet = file_write(achPath, 0, pvGroupFlash, sizeof(S_GroupAttrFlash));
	if (iRet < 0)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_add]Can't write new group data-%s to ROM.\r\n", achPath);
		return iRet;
	}

	GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_group_data_add]Have added group:%04hx.\r\n",  ui16Id);

	return (int)ui16Id;
}

static int s_group_data_del (uint16_t ui16Id)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	//删除ROM中的数据
	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16Id, achPath, sizeof(achPath)))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_del]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_del(achPath);
	if (iRet < 0)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_del]Can't delete device data-%s from ROM.\r\n", achPath);
		return iRet;
	}

	//删除无数据的目录
	if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_GROUP, achPath, sizeof(achPath)))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_group_data_del]Can't get path of group type.\r\n");
	}
	iRet = del_empty_dir(achPath);
	if (iRet < 0)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_group_data_del]Can't delete empty dir for group type.\r\n");
	}

	GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_group_data_del]Have deleted group:%04hx.\r\n",  ui16Id);

	return 0;
}

static int s_group_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId)
{
	char achSrcPath[MUI_FILE_ABS_PATH_LEN];
	char achDestPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	ASSERT(ui16DestId != ui16SrcId);

	//先修改ROM数据(先操作ROM数据再操作RAM数据,因为RAM数据只是内存操作没有失败可能,这样的顺序是为了防止回退操作)
	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16SrcId, achSrcPath, MUI_FILE_ABS_PATH_LEN))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_swap_id]Can't get path of device :%04hx.\r\n",  ui16SrcId);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16DestId, achDestPath, MUI_FILE_ABS_PATH_LEN))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_swap_id]Can't get path of device :%04hx.\r\n",  ui16DestId);
		return FILE_S_ADDR_ERR;
	}

	iRet = file_name_swap(achDestPath, achSrcPath);
	if (iRet < 0)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_swap_id]Call 'file_name_swap(%s, %s)' fail.\r\n", achDestPath, achSrcPath);
		return iRet;
	}

	return iRet;
}


static int s_group_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id)
{
	char achBuf[MUI_FILE_ABS_PATH_LEN*2];
	char *pchPath, *pchNewPath;
	int iRet;

	ASSERT(ui16NewId != ui16Id);

	pchPath    = &achBuf[MUI_FILE_ABS_PATH_LEN*0];
	pchNewPath = &achBuf[MUI_FILE_ABS_PATH_LEN*1];

	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16Id, pchPath, MUI_FILE_ABS_PATH_LEN))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_set_new_id]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16NewId, pchNewPath, MUI_FILE_ABS_PATH_LEN))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_set_new_id]Can't get path of device :%04hx.\r\n",  ui16NewId);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_rename(pchNewPath, pchPath);
	if (iRet < 0)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_set_new_id]Call 'file_rename(%s, %s)' fail.\r\n", pchNewPath, pchPath);
		return iRet;
	}

	return iRet;
}

static int s_group_data_add_member (uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType)
{
	int iRet, i;
	S_GroupOpAttr opattr;
	char achPath[MUI_FILE_ABS_PATH_LEN];

	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16Id, achPath, sizeof(achPath)))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_add]Can't get path of group:%04hx.\r\n", ui16Id);
		iRet = FILE_S_ADDR_ERR;
	}

	iRet = file_read(achPath, M_OFFSET(S_GroupAttrFlash, op_attr), &opattr, M_SIZEOF(S_GroupAttrFlash, op_attr));
	if (iRet < 0)
	{
		return iRet;
	}

	if (opattr.total >= MUI_DEV_NUM_MAX_PER_GROUP)
	{
		return GROUP_DEV_NUM_ERROR;
	}

	for (i=0; i<opattr.total; i++)
	{
		if (opattr.member[i].devId == ui16DevId && opattr.member[i].devType == ui16DevType)
		{
			GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_add] devid have in idlist.:%d\r\n", ui16DevId);
			return GROUP_DEV_ID_ERROR;
		}
	}

	opattr.member[opattr.total].devId = ui16DevId;
	opattr.member[opattr.total].devType = ui16DevType;
	opattr.total++;
	iRet = file_write(achPath, M_OFFSET(S_GroupAttrFlash, op_attr), &opattr, M_SIZEOF(S_GroupAttrFlash, op_attr));

	return iRet;
}

static int s_group_data_del_member (uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType)
{
	int iRet, i;
	S_GroupOpAttr opattr;
	char achPath[MUI_FILE_ABS_PATH_LEN];

	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16Id, achPath, sizeof(achPath)))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_group_data_add]Can't get path of group:%04hx.\r\n", ui16Id);
		iRet = FILE_S_ADDR_ERR;
	}

	iRet = file_read(achPath, M_OFFSET(S_GroupAttrFlash, op_attr), &opattr, M_SIZEOF(S_GroupAttrFlash, op_attr));
	if (iRet < 0)
	{
		return iRet;
	}

	for (i=0; i<opattr.total; i++)
	{
		if (opattr.member[i].devId == ui16DevId && opattr.member[i].devType == ui16DevType)
		{
			memcpy(&opattr.member[i], &opattr.member[i+1], sizeof(S_GroupMember)*(opattr.total-i));
			opattr.total--;
		}
	}
	iRet = file_write(achPath, M_OFFSET(S_GroupAttrFlash, op_attr), &opattr, M_SIZEOF(S_GroupAttrFlash, op_attr));
	return iRet;
}







int group_data_id_list_get (uint16_t *pui16Buf, uint16_t ui16Capacity)
{
	ASSERT(NULL != pui16Buf);
	ASSERT(ui16Capacity <= MUI_GROUP_DATA_NUM_MAX);

	return data_id_exclusive_scan_by_type(MUI_DATA_TYPE_GROUP, pui16Buf, ui16Capacity, &s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX);
}

int group_data_num_get (void)
{
	uint16_t aui16IdList[MUI_GROUP_DATA_NUM_MAX];

	return data_id_exclusive_scan_by_type(MUI_DATA_TYPE_GROUP, aui16IdList, (sizeof(aui16IdList)/sizeof(aui16IdList[0])), &s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX);
}

int group_data_assign_id (void)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_GROUP_DATA_WAIT_TIME_MS_MAX)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_assign_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tGroupRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_group_data_assign_id();

	sem_post(&s_tGroupRomMutex);

	return iRet;
}

int group_data_read (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(S_GroupAttrFlash));	//不能超出数据的最大边界

	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16Id, achPath, sizeof(achPath)))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_rom_read]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_read(achPath, tOffset, pvBuf, tSize, &s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX);
}

int group_data_write (uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(S_GroupAttrFlash));	//不能超出数据的最大边界

	if (NULL == data_file_path_get(MUI_DATA_TYPE_GROUP, ui16Id, achPath, sizeof(achPath)))
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_rom_write]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_write(achPath, tOffset, pvBuf, tSize, &s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX);
}

int group_data_add (const S_GroupAttrFlash *pvGroupFlash)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_GROUP_DATA_WAIT_TIME_MS_MAX)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tGroupRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_group_data_add(pvGroupFlash);

	sem_post(&s_tGroupRomMutex);

	return iRet;
}

int group_data_del (const uint16_t ui16Id)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_GROUP_DATA_WAIT_TIME_MS_MAX)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_del]Wait semaphore %u over time.\r\n", (uint32_t)&s_tGroupRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet =  s_group_data_del(ui16Id);

	sem_post(&s_tGroupRomMutex);
	return iRet;
}

int group_data_del_all (void)
{
	uint16_t group_id_list[MUI_GROUP_DATA_NUM_MAX];
	int dev_num = 0;
	int iRet = 0, i;

	dev_num = group_data_id_list_get(group_id_list, sizeof(group_id_list)/sizeof(group_id_list[0]));
	if (dev_num < 0)
	{
		return 0;
	}

	for (i=0; i<dev_num; i++)
	{
		iRet = group_data_del(group_id_list[i]);
		if(iRet < 0)
		{
			GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[group_data_del_all]Can't delete group id = %d from ROM.\r\n", group_id_list[i]);
		}
	}

	return iRet;
}

int group_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_GROUP_DATA_WAIT_TIME_MS_MAX)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_swap_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tGroupRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_group_data_swap_id(ui16DestId, ui16SrcId);

	sem_post(&s_tGroupRomMutex);

	return iRet;
}

int group_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_GROUP_DATA_WAIT_TIME_MS_MAX)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_set_new_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tGroupRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_group_data_set_new_id(ui16NewId, ui16Id);

	sem_post(&s_tGroupRomMutex);

	return iRet;
}

int group_data_add_member (uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_GROUP_DATA_WAIT_TIME_MS_MAX)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_add_member]Wait semaphore %u over time.\r\n", (uint32_t)&s_tGroupRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_group_data_add_member(ui16Id, ui16DevId, ui16DevType);

	sem_post(&s_tGroupRomMutex);

	return iRet;
}

int group_data_del_member(uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tGroupRomMutex, MUI_GROUP_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_GROUP_DATA_WAIT_TIME_MS_MAX)
	{
		GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[group_data_del_member]Wait semaphore %u over time.\r\n", (uint32_t)&s_tGroupRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_group_data_del_member(ui16Id, ui16DevId, ui16DevType);

	sem_post(&s_tGroupRomMutex);

	return iRet;
}




int group_data_init(void)
{
	int iRet;
	GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[group_data_init]group_data_init start\r\n");
	iRet = sem_init(&s_tGroupRomMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return iRet;
	}
	GROUP_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[group_data_init]group_data_init end\r\n");
	return iRet;
}


