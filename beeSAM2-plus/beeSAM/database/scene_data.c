#include "scene_data.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SCENE_DATA_DBG

#ifndef SCENE_DATA_PUTC
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_PUTC(Char)			PUTC(Char)
#else
#define SCENE_DATA_PUTC(Char)
#endif
#endif

#ifndef SCENE_DATA_PUTS
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SCENE_DATA_PUTS(Str, Len)
#endif
#endif

#ifndef SCENE_DATA_PUTASCC
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_PUTASCC(Char)			PUTASCC(Char)
#else
#define SCENE_DATA_PUTASCC(Char)
#endif
#endif

#ifndef SCENE_DATA_PUTASCS
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SCENE_DATA_PUTASCS(Str, Len)
#endif
#endif

#ifndef SCENE_DATA_PRINT
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SCENE_DATA_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SCENE_DATA_DYN_PUTC
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SCENE|Dbg), Char)
#else
#define SCENE_DATA_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SCENE_DATA_DYN_PUTS
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SCENE|Dbg), Str, Len)
#else
#define SCENE_DATA_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SCENE_DATA_DYN_PUTASCC
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SCENE|Dbg), Char)
#else
#define SCENE_DATA_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SCENE_DATA_DYN_PUTASCS
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SCENE|Dbg), Str, Len)
#else
#define SCENE_DATA_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SCENE_DATA_DYN_PRINT
#ifdef SCENE_DATA_DBG
#define SCENE_DATA_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SCENE|Dbg), Fmt, ##Arg)
#else
#define SCENE_DATA_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

#define MUI_SCENE_DATA_WAIT_TIME_MS_MAX		3000

static sem_t s_tSceneRomMutex;

static int s_scene_data_assign_id (void);
static int s_scene_data_add (const void *pvRomBuf, size_t tRomSize);
static int s_scene_data_del (uint16_t ui16Id);
static int s_scene_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
static int s_scene_data_scene_new_id (uint16_t ui16NewId, uint16_t ui16Id);
static int s_scene_data_add_member (uint16_t ui16Id, const S_SceneMember *psMemberList, uint16_t ui16Num);
static int s_scene_data_del_member (uint16_t ui16Id, const S_SceneMember *psMemberList, uint16_t ui16Num);

static int s_scene_data_assign_id (void)
{
	uint16_t aui16IdList[MUI_SCENE_DATA_NUM_MAX];
	int iCnt, i;

	iCnt = data_id_scan_by_type(MUI_DATA_TYPE_SCENE, aui16IdList, MUI_SCENE_DATA_NUM_MAX);
	if (iCnt < 0)
	{
		if (iCnt != FILE_S_ERR_DIR_NOT_EXIST)
		{
			SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_scene_data_assign_id]Call 'data_id_scan_by_type(%04hx)' file.\r\n", MUI_DATA_TYPE_SCENE);
			return iCnt;
		}

		return MUI_DATABASE_ID_START;
	}

	if (iCnt >= MUI_SCENE_DATA_NUM_MAX)
	{
		return SCENE_ASSIGN_ID_ERROR;
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

static int s_scene_data_add (const void *pvRomBuf, size_t tRomSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Id;
	int iRet;

	iRet = s_scene_data_assign_id();
	if (iRet < MUI_DATABASE_ID_START)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_add]Call 'scene_data_assign_id()' fail.\r\n");
		return iRet;
	}
	ui16Id = (uint16_t)iRet;

	//检测并创建目录
	if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_SCENE, achPath, sizeof(achPath)))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_add]Can't get path of scene type\r\n");
		return FILE_S_ADDR_ERR;
	}
	iRet = mk_multi_dir(achPath, MUI_DATA_CREAT_MODE);
	if (iRet < 0)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_add]Can't execute mkdir for scene type \r\n");
		return FILE_S_ADDR_ERR;
	}

	//创建ROM中的数据
	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16Id, achPath, sizeof(achPath)))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_add]Can't get path of scene:%04hx.\r\n", ui16Id);
		iRet = FILE_S_ADDR_ERR;
	}
	iRet = file_write(achPath, 0, pvRomBuf, tRomSize);
	if (iRet < 0)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_add]Can't write new scene data-%s to ROM.\r\n", achPath);
		return iRet;
	}

	SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[scene_data_add]Have added scene:%04hx.\r\n",  ui16Id);

	return (int)ui16Id;
}

static int s_scene_data_del (uint16_t ui16Id)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	//删除ROM中的数据
	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16Id, achPath, sizeof(achPath)))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_del]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_del(achPath);
	if (iRet < 0)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_del]Can't delete device data-%s from ROM.\r\n", achPath);
		return iRet;
	}

	//删除无数据的目录
	if (NULL == data_dir_path_get_by_type(MUI_DATA_TYPE_SCENE, achPath, sizeof(achPath)))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[scene_data_del]Can't get path of scene type.\r\n");
	}
	iRet = del_empty_dir(achPath);
	if (iRet < 0)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[scene_data_del]Can't delete empty dir for scene type.\r\n");
	}

	SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[scene_data_del]Have deleted scene:%04hx.\r\n",  ui16Id);

	return 0;
}

static int s_scene_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId)
{
	char achSrcPath[MUI_FILE_ABS_PATH_LEN];
	char achDestPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	ASSERT(ui16DestId != ui16SrcId);

	//先修改ROM数据(先操作ROM数据再操作RAM数据,因为RAM数据只是内存操作没有失败可能,这样的顺序是为了防止回退操作)
	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16SrcId, achSrcPath, MUI_FILE_ABS_PATH_LEN))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_swap_id]Can't get path of device :%04hx.\r\n",  ui16SrcId);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16DestId, achDestPath, MUI_FILE_ABS_PATH_LEN))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_swap_id]Can't get path of device :%04hx.\r\n",  ui16DestId);
		return FILE_S_ADDR_ERR;
	}

	iRet = file_name_swap(achDestPath, achSrcPath);
	if (iRet < 0)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_swap_id]Call 'file_name_swap(%s, %s)' fail.\r\n", achDestPath, achSrcPath);
		return iRet;
	}

	return iRet;
}

static int s_scene_data_scene_new_id (uint16_t ui16NewId, uint16_t ui16Id)
{
	char achBuf[MUI_FILE_ABS_PATH_LEN*2];
	char *pchPath, *pchNewPath;
	int iRet;

	ASSERT(ui16NewId != ui16Id);

	pchPath    = &achBuf[MUI_FILE_ABS_PATH_LEN*0];
	pchNewPath = &achBuf[MUI_FILE_ABS_PATH_LEN*1];

	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16Id, pchPath, MUI_FILE_ABS_PATH_LEN))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_scene_new_id]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16NewId, pchNewPath, MUI_FILE_ABS_PATH_LEN))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_scene_new_id]Can't get path of device :%04hx.\r\n",  ui16NewId);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_rename(pchNewPath, pchPath);
	if (iRet < 0)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_scene_new_id]Call 'file_rename(%s, %s)' fail.\r\n", pchNewPath, pchPath);
		return iRet;
	}

	return iRet;
}

static int s_scene_data_add_member (uint16_t ui16Id, const S_SceneMember *psMemberList, uint16_t ui16Num)
{
	ASSERT(NULL != psMemberList);
	ASSERT(ui16Num <= MUI_ACTION_NUM_MAX_PER_SCENE);

	int iRet;
	uint16_t actionnum;
	char achPath[MUI_FILE_ABS_PATH_LEN];

	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16Id, achPath, sizeof(achPath)))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_add_member]Can't get path of scene:%04hx.\r\n", ui16Id);
		iRet = FILE_S_ADDR_ERR;
	}

	iRet = file_read(achPath, M_OFFSET(S_SceneDataFalsh, scene_attr.action_num), &actionnum, M_SIZEOF(S_SceneDataFalsh, scene_attr.action_num));
	if (iRet < 0)
	{
		return iRet;
	}
	SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_add_member]=========actionnum= %d.\r\n", actionnum);
	iRet = file_write(achPath, (sizeof(S_SceneAttr)+actionnum*sizeof(S_SceneMember)), psMemberList, ui16Num*sizeof(psMemberList[0]));
	if (iRet < 0)
	{
		return iRet;
	}
	actionnum = actionnum + ui16Num;
	iRet = file_write(achPath, M_OFFSET(S_SceneDataFalsh, scene_attr.action_num), &actionnum, M_SIZEOF(S_SceneDataFalsh, scene_attr.action_num));
	return iRet;
}

static int s_scene_data_del_member (uint16_t ui16Id, const S_SceneMember *psMemberList, uint16_t ui16Num)
{
	UNUSED(ui16Id);
	UNUSED(psMemberList);
	UNUSED(ui16Num);
	return 0;
}


int scene_data_id_list_get (uint16_t *pui16Buf, uint16_t ui16Capacity)
{
	ASSERT(NULL != pui16Buf);
	ASSERT(ui16Capacity <= MUI_SCENE_DATA_NUM_MAX);

	return data_id_exclusive_scan_by_type(MUI_DATA_TYPE_SCENE, pui16Buf, ui16Capacity, &s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX);
}

int scene_data_num_get (void)
{
	uint16_t aui16IdList[MUI_SCENE_DATA_NUM_MAX];

	return data_id_exclusive_scan_by_type(MUI_DATA_TYPE_SCENE, aui16IdList, (sizeof(aui16IdList)/sizeof(aui16IdList[0])), &s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX);
}

int scene_data_assign_id (void)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCENE_DATA_WAIT_TIME_MS_MAX)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[scene_data_assign_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tSceneRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_scene_data_assign_id();

	sem_post(&s_tSceneRomMutex);

	return iRet;
}

int scene_data_read (uint16_t ui16Id, size_t tOffscene, void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);

	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16Id, achPath, sizeof(achPath)))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_read]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_read(achPath, tOffscene, pvBuf, tSize, &s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX);
}

int scene_data_write (uint16_t ui16Id, size_t tOffscene, const void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);

	if (NULL == data_file_path_get(MUI_DATA_TYPE_SCENE, ui16Id, achPath, sizeof(achPath)))
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_write]Can't get path of device :%04hx.\r\n",  ui16Id);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_write(achPath, tOffscene, pvBuf, tSize, &s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX);
}

int scene_data_add (const void *pvRomBuf)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCENE_DATA_WAIT_TIME_MS_MAX)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[scene_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tSceneRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_scene_data_add(pvRomBuf, sizeof(S_SceneDataFalsh));

	sem_post(&s_tSceneRomMutex);

	return iRet;
}

int scene_data_del (uint16_t ui16Id)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCENE_DATA_WAIT_TIME_MS_MAX)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[scene_data_del]Wait semaphore %u over time.\r\n", (uint32_t)&s_tSceneRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_scene_data_del(ui16Id);

	sem_post(&s_tSceneRomMutex);

	return iRet;
}

int scene_data_del_all (void)
{
	int iRet=0;
	int i;
	int dev_num = 0;
	uint16_t scene_id_list[MUI_SCENE_DATA_NUM_MAX];

	dev_num = scene_data_id_list_get(scene_id_list, MUI_SCENE_DATA_NUM_MAX);
	if (dev_num < 0)
	{
		return 0;
	}

	for (i=0; i<dev_num; i++)
	{
		iRet = scene_data_del(scene_id_list[i]);
		if(iRet < 0)
		{
			SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[scene_data_del_all]Can't delete scene id = %d from ROM.\r\n", scene_id_list[i]);
		}
	}

	return iRet;
}

int scene_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCENE_DATA_WAIT_TIME_MS_MAX)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[scene_data_swap_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tSceneRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_scene_data_swap_id(ui16DestId, ui16SrcId);

	sem_post(&s_tSceneRomMutex);

	return iRet;
}

int scene_data_scene_new_id (uint16_t ui16NewId, uint16_t ui16Id)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCENE_DATA_WAIT_TIME_MS_MAX)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[scene_data_scene_new_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tSceneRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_scene_data_scene_new_id(ui16NewId, ui16Id);

	sem_post(&s_tSceneRomMutex);

	return iRet;
}

int scene_data_add_member (uint16_t ui16Id, const S_SceneMember *psMemberList, uint16_t ui16Num)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCENE_DATA_WAIT_TIME_MS_MAX)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[scene_data_add_member]Wait semaphore %u over time.\r\n", (uint32_t)&s_tSceneRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_scene_data_add_member(ui16Id, psMemberList, ui16Num);

	sem_post(&s_tSceneRomMutex);

	return iRet;
}

int scene_data_del_member (uint16_t ui16Id, const S_SceneMember *psMemberList, uint16_t ui16Num)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tSceneRomMutex, MUI_SCENE_DATA_WAIT_TIME_MS_MAX))	//最多阻塞(MUI_SCENE_DATA_WAIT_TIME_MS_MAX)
	{
		SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[scene_data_del_member]Wait semaphore %u over time.\r\n", (uint32_t)&s_tSceneRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	iRet = s_scene_data_del_member(ui16Id, psMemberList, ui16Num);

	sem_post(&s_tSceneRomMutex);

	return iRet;
}

int scene_data_init (void)
{
	int iRet;
	SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[scene_data_init]scene_data_init start\r\n");
	iRet = sem_init(&s_tSceneRomMutex, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		return iRet;
	}
	SCENE_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[scene_data_init]scene_data_init end\r\n");
	return iRet;
}

