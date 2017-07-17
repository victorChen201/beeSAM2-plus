#include "set.h"

//#include "api_deal.h"
/*
 uint16_t setid;
 uint16_t action_num;

 uint16_t dev_type;
 uint16_t dev_id;
 uint8_t action_type;
 uint16_t action_value1;
 uint16_t action_value2;
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SET_DBG

#ifndef SET_PUTC
#ifdef SET_DBG
#define SET_PUTC(Char)			PUTC(Char)
#else
#define SET_PUTC(Char)
#endif
#endif

#ifndef SET_PUTS
#ifdef SET_DBG
#define SET_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SET_PUTS(Str, Len)
#endif
#endif

#ifndef SET_PUTASCC
#ifdef SET_DBG
#define SET_PUTASCC(Char)			PUTASCC(Char)
#else
#define SET_PUTASCC(Char)
#endif
#endif

#ifndef SET_PUTASCS
#ifdef SET_DBG
#define SET_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SET_PUTASCS(Str, Len)
#endif
#endif

#ifndef SET_PRINT
#ifdef SET_DBG
#define SET_PRINT(Fmt, Arg...)			PRINT(Fmt, ##Arg)
#else
#define SET_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SET_DYN_PUTC
#ifdef SET_DBG
#define SET_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SET|Dbg), Char)
#else
#define SET_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SET_DYN_PUTS
#ifdef SET_DBG
#define SET_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SET|Dbg), Str, Len)
#else
#define SET_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SET_DYN_PUTASCC
#ifdef SET_DBG
#define SET_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SET|Dbg), Char)
#else
#define SET_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SET_DYN_PUTASCS
#ifdef SET_DBG
#define SET_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SET|Dbg), Str, Len)
#else
#define SET_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SET_DYN_PRINT
#ifdef SET_DBG
#define SET_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SET|Dbg), Fmt, ##Arg)
#else
#define SET_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif


static S_SetExecuteState Set_Execute_State[255];
static pthread_t s_set_DealThread; /* 线程ID*/
static sem_t s_tsetexecuteSem;

static void* s_set_execute_deal_task (void *pArg);

//-----------------------------------------------------------------------------------------------------
/*static int s_set_assign_id();
static int set_add(const void *pvRomBuf, size_t tRomSize);
static int set_del(uint16_t ui16Id);
static int set_swap_id(uint16_t ui16DestId, uint16_t ui16SrcId);
static int set_set_new_id(uint16_t ui16NewId, uint16_t ui16Id);
static int set_write_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize);
static int set_write_name(const uint16_t ui16Id, const S_AttrName *name);
static int set_write_description(const uint16_t ui16Id, const S_AttrDescr *description);
static int set_write_action_num(const uint16_t ui16Id, const uint16_t *ui16action_num);
static int set_write_delay(const uint16_t ui16Id, const uint16_t *ui16delay);
*/

//-----------------------------------------------------------------------------------------------------

int set_get_id_list(uint16_t *aui16IdList, uint16_t ui16Capacity)
{
	return set_data_id_list_get(aui16IdList, ui16Capacity);
}

static int s_set_assign_id()
{
	return set_data_assign_id();
}

int set_add(const void *pvRomBuf, size_t tRomSize)
{
	ASSERT(NULL != pvRomBuf);

	return set_data_add(pvRomBuf, tRomSize);
}

int set_del(uint16_t ui16Id)
{
	return set_data_del(ui16Id);
}

static int set_swap_id(uint16_t ui16DestId, uint16_t ui16SrcId)
{
	return set_data_swap_id(ui16DestId, ui16SrcId);
}

static int set_set_new_id(uint16_t ui16NewId, uint16_t ui16Id)
{
	return set_data_set_new_id(ui16NewId, ui16Id);
}

int set_read_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize)
{
	ASSERT(NULL != pvBuf);
	return set_data_read(ui16Id, 0, pvBuf, tSize);
}

int set_read_name(const uint16_t ui16Id, const S_AttrName *name)
{
	ASSERT(NULL != name);
	return set_data_read(ui16Id, M_OFFSET(S_SetAttrFlash, name), (void*)name, M_SIZEOF(S_SetAttrFlash, name));
}

int set_read_description(const uint16_t ui16Id, const S_AttrDescr *description)
{
	ASSERT(NULL != description);
	return set_data_read(ui16Id, M_OFFSET(S_SetAttrFlash, descr), (void*)description, M_SIZEOF(S_SetAttrFlash, descr));
}

int set_read_action_num(const uint16_t ui16Id, const uint16_t *ui16action_num)
{
	ASSERT(NULL != ui16action_num);
	return set_data_read(ui16Id, M_OFFSET(S_SetAttrFlash, action_num), (void*)ui16action_num, M_SIZEOF(S_SetAttrFlash, action_num));
}

int set_read_delay(const uint16_t ui16Id, const uint16_t *ui16delay)
{
	ASSERT(NULL != ui16delay);
	return set_data_read(ui16Id, M_OFFSET(S_SetAttrFlash, delay), (void*)ui16delay, M_SIZEOF(S_SetAttrFlash, delay));
}

//ui16actionId 从0开始， 0位第一个
int set_read_action(uint16_t ui16Id, uint16_t ui16actionId, S_SetMember *pvAttr)
{
	ASSERT(NULL != pvAttr);
	return set_data_read(ui16Id, (sizeof(S_SetAttrFlash)+ui16actionId*sizeof(S_SetMember)), (void*)pvAttr, sizeof(S_SetMember));
}

int set_read_action_all(uint16_t ui16Id, void *pvAllAttr, size_t tSize)
{
	uint16_t ui16ActionNum;
	int iRet;

	ASSERT(NULL != pvAllAttr);

	iRet = set_read_action_num(ui16Id, &ui16ActionNum);
	if(iRet < 0)
	{
		return iRet;
	}
	if (sizeof(S_SetMember)*ui16ActionNum > tSize)
	{
		return -1;
	}
	return set_data_read(ui16Id, sizeof(S_SetAttrFlash), pvAllAttr, sizeof(S_SetMember)*ui16ActionNum);
}

int set_write_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize)
{
	ASSERT(NULL != pvBuf);
	ASSERT(tSize <= sizeof(S_SetAttrFlash));
	return set_data_write(ui16Id, 0, pvBuf, tSize);
}

int set_write_name(const uint16_t ui16Id, const S_AttrName *name)
{
	ASSERT(NULL != name);
	return set_data_write(ui16Id, M_OFFSET(S_SetAttrFlash, name), name, M_SIZEOF(S_SetAttrFlash, name));
}

int set_write_description(const uint16_t ui16Id, const S_AttrDescr *description)
{
	ASSERT(NULL != description);
	return set_data_write(ui16Id, M_OFFSET(S_SetAttrFlash, descr), description, M_SIZEOF(S_SetAttrFlash, descr));
}

int set_write_action_num(const uint16_t ui16Id, const uint16_t *ui16action_num)
{
	ASSERT(NULL != ui16action_num);
	return set_data_write(ui16Id, M_OFFSET(S_SetAttrFlash, action_num), ui16action_num, M_SIZEOF(S_SetAttrFlash, action_num));
}

int set_write_delay(const uint16_t ui16Id, const uint16_t *ui16delay)
{
	ASSERT(NULL != ui16delay);
	return set_data_write(ui16Id, M_OFFSET(S_SetAttrFlash, delay), ui16delay, M_SIZEOF(S_SetAttrFlash, delay));
}

int s_set_add_member(uint16_t ui16Id, const S_SetMember *psActionList, uint16_t ui16ActionNum)
{
	ASSERT(NULL != psActionList);
	ASSERT(ui16ActionNum <= MUI_ACTION_NUM_MAX_PER_SET);
	return set_data_add_member(ui16Id, psActionList, ui16ActionNum);
}


//----------------------------------------------------------------------------------------------

int set_get_execute_state(uint16_t ui16Id)
{
	return Set_Execute_State[ui16Id - 1].set_execute;
}

int set_execute_active (const uint16_t id, const uint8_t execute)
{
	int iRet = 0;
	uint16_t ui16Delay;

	if (execute > 0)
	{
		if (Set_Execute_State[id - 1].set_execute == 0)
		{
			iRet = set_read_delay (id, &ui16Delay);
			if (iRet > 0)
			{
				if (0 <= sem_wait_ms(&s_tsetexecuteSem, 1000))
				{
					SET_DYN_PRINT(M_DBG_LEVEL_DEBUG,"-------------------------------------------------------\r\n\n");
					SET_DYN_PRINT(M_DBG_LEVEL_DEBUG,"----Set_Execute_State[id-1].set_execute = %d------\r\n", execute);
					SET_DYN_PRINT(M_DBG_LEVEL_DEBUG,"-------------------------------------------------------\r\n\n");
					Set_Execute_State[id - 1].set_execute = execute;
					Set_Execute_State[id - 1].set_delay = ui16Delay;
				}
				sem_post(&s_tsetexecuteSem);
			}
		}
	}
	else
	{
		if (Set_Execute_State[id - 1].set_execute != 0)
		{
			if (0 <= sem_wait_ms(&s_tsetexecuteSem, 1000))
			{
				Set_Execute_State[id - 1].set_execute = 0;
				Set_Execute_State[id - 1].set_delay = 0;
			}
			sem_post(&s_tsetexecuteSem);
		}
	}
	return iRet;
}

/*
 *
 *
 *
 */
int set_active (const uint16_t id)
{
	int i, iRet=0;
	S_SetAttrFlash set_flash;
	S_SetMember set_attr;

	if (set_read_info(id, &set_flash, sizeof(S_SetAttrFlash)) > 0)
	{
		for (i = 0; i < set_flash.action_num; i++)
		{
			memset(&set_attr, 0, sizeof(S_SetMember));
			iRet = set_read_action(id, i, &set_attr);
			if (iRet < 0)
			{
				continue;
			}
			switch (set_attr.action_type)
			{
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					iRet = group_set_state_on(set_attr.dev_id, &set_attr.uSetAtion.on);
				}
				else
				{
					iRet = set_dev_state_on(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.on);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					iRet = group_light_state_bri(set_attr.dev_id, &set_attr.uSetAtion.bri);
				}
				else
				{
					iRet = set_dev_state_bri(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.bri);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					//iRet = group_light_state_sat(set_attr.dev_id, &set_attr.uSetAtion.sat);
				}
				else
				{
					iRet = set_dev_state_sat(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.sat);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					//iRet = group_light_state_hue(set_attr.dev_id, &set_attr.uSetAtion.hue);
				}
				else
				{
					iRet = set_dev_state_hue(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.hue);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					iRet = group_light_state_ct(set_attr.dev_id, &set_attr.uSetAtion.ct);
				}
				else
				{
					iRet = set_dev_state_ct(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.ct);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					iRet = group_light_state_xy(set_attr.dev_id, &set_attr.uSetAtion.xy);
				}
				else
				{
					iRet = set_dev_state_xy(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.xy);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					iRet = group_set_state_level(set_attr.dev_id, &set_attr.uSetAtion.level);
				}
				else
				{
					iRet = set_dev_state_level(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.level);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				iRet = set_dev_state_temp(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.temp);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				iRet = set_dev_state_hum(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.hum);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
				iRet = set_dev_state_warn_mode(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.warnmode);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
				iRet = set_dev_state_duration(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.duration);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					iRet = group_warn_dev_state_warn_cfg(set_attr.dev_id, &set_attr.uSetAtion.warncfg);
				}
				else
				{
					iRet = set_dev_state_warn_cfg(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.warncfg);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
				if (set_attr.dev_type == MUI_DATA_TYPE_GROUP)
				{
					iRet = group_shade_state_action(set_attr.dev_id, &set_attr.uSetAtion.action);
				}
				else
				{
					iRet = set_dev_state_action(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.action);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
				if (set_attr.dev_type == MUI_DATA_TYPE_SET)
				{
					if (set_get_execute_state(set_attr.dev_id - 1) == 0)
					{
						set_execute_active(set_attr.dev_id, (uint16_t) set_attr.uSetAtion.execute);
					}
				}
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
				if (set_attr.dev_type == MUI_DATA_TYPE_RULE)
				{
					iRet = rules_write_active(set_attr.dev_id, &set_attr.uSetAtion.active);
				}
				if (set_attr.dev_type == MUI_DATA_TYPE_SCHED)
				{
					iRet = schedule_write_active(set_attr.dev_id, &set_attr.uSetAtion.active);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
				if (set_attr.dev_type == MUI_DATA_TYPE_SONOS)
				{
					set_dev_state_play(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.play);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
				if (set_attr.dev_type == MUI_DATA_TYPE_SONOS)
				{
					set_dev_state_volume(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.volume);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
				if (set_attr.dev_type == MUI_DATA_TYPE_SONOS)
				{
					set_dev_state_url_id(set_attr.dev_type, set_attr.dev_id, &set_attr.uSetAtion.urlid);
				}
				break;
			default:
				break;
			}

		}
	}
	return iRet;
}


int set_dev_replace (const uint8_t type, const uint8_t id, const uint8_t newid)
{
	UNUSED(type);
	UNUSED(id);
	UNUSED(newid);

	return 0;
}

//-------------------------------------------------------------------------------------------------*
/*	set active处理任务函数
 ;	(功能)
 ;	(参数)
 ;		pArg:	传给函数运行的参数
 ;	(返回)
 ;		NULL
 ;	(注意)
 */
static void* s_set_execute_deal_task (void *pArg)
{
	UNUSED(pArg);

	int iRet = 0;
	uint16_t i;

	iRet = sem_init(&s_tsetexecuteSem, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		SET_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_set_execute_deal_task] sem_init fail.:%d\r\n", iRet);
		return;
	}

	while (1)
	{
		for (i = 0; i < MUI_SET_DATA_NUM_MAX; i++)
		{
			if (Set_Execute_State[i].set_execute == 1)
			{
				if (Set_Execute_State[i].set_delay > 0)
				{
					if (0 <= sem_wait_ms(&s_tsetexecuteSem, 1000))
					{
						Set_Execute_State[i].set_delay--;
						SET_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[------------Set_Execute_State[%d].set_delay = %d------------]\r\n", i, Set_Execute_State[i].set_delay);
					}
					sem_post(&s_tsetexecuteSem);
				}
				else
				{
					SET_DYN_PRINT(M_DBG_LEVEL_DEBUG,"set_active = %d\r\n", i + 1);
					set_active(i + 1);
					if (0 <= sem_wait_ms(&s_tsetexecuteSem, 1000))
					{
						Set_Execute_State[i].set_execute = 0;
					}
					sem_post(&s_tsetexecuteSem);
				}
			}
		}
		msleep(1000);
	}

	return NULL;
}

int set_init ()
{
	int iRet = 0;

	iRet = pthread_create(&s_set_DealThread, NULL, s_set_execute_deal_task, NULL);
	if (iRet < 0)
	{
		return -1;
	}
	return 1;
}

