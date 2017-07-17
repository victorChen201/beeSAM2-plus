#include "scene.h"

//#include "api_deal.h"
/*
 uint16_t sceneid;
 uint16_t action_num;

 uint16_t dev_type;
 uint16_t dev_id;
 uint8_t action_type;
 uint16_t action_value1;
 uint16_t action_value2;
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SCENE_DBG

#ifndef SCENE_PUTC
#ifdef SCENE_DBG
#define SCENE_PUTC(Char)			PUTC(Char)
#else
#define SCENE_PUTC(Char)
#endif
#endif

#ifndef SCENE_PUTS
#ifdef SCENE_DBG
#define SCENE_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SCENE_PUTS(Str, Len)
#endif
#endif

#ifndef SCENE_PUTASCC
#ifdef SCENE_DBG
#define SCENE_PUTASCC(Char)			PUTASCC(Char)
#else
#define SCENE_PUTASCC(Char)
#endif
#endif

#ifndef SCENE_PUTASCS
#ifdef SCENE_DBG
#define SCENE_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SCENE_PUTASCS(Str, Len)
#endif
#endif

#ifndef SCENE_PRINT
#ifdef SCENE_DBG
#define SCENE_PRINT(Fmt, Arg...)			PRINT(Fmt, ##Arg)
#else
#define SCENE_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SCENE_DYN_PUTC
#ifdef SCENE_DBG
#define SCENE_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SCENE|Dbg), Char)
#else
#define SCENE_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SCENE_DYN_PUTS
#ifdef SCENE_DBG
#define SCENE_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SCENE|Dbg), Str, Len)
#else
#define SCENE_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SCENE_DYN_PUTASCC
#ifdef SCENE_DBG
#define SCENE_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SCENE|Dbg), Char)
#else
#define SCENE_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SCENE_DYN_PUTASCS
#ifdef SCENE_DBG
#define SCENE_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SCENE|Dbg), Str, Len)
#else
#define SCENE_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SCENE_DYN_PRINT
#ifdef SCENE_DBG
#define SCENE_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SCENE|Dbg), Fmt, ##Arg)
#else
#define SCENE_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif


static S_SceneAtcmd		SceneAtcmd;


//-----------------------------------------------------------------------------------------------------
static int s_scene_dev_join_atcmd(uint16_t uI16id, const char *nwk, const char *ep, char *pvBuf);
static int s_scene_dev_del_atcmd(uint16_t uI16id, const char *nwk, const char *ep, char *pvBuf);

//-----------------------------------------------------------------------------------------------------

int scene_get_id_list(uint16_t *aui16IdList, uint16_t ui16Capacity)
{
	return scene_data_id_list_get(aui16IdList, ui16Capacity);
}

static int s_scene_assign_id()
{
	return scene_data_assign_id();
}

int scene_add(const void *pvRomBuf)
{
	ASSERT(NULL != pvRomBuf);

	return scene_data_add(pvRomBuf);
}

static int scene_swap_id(uint16_t ui16DestId, uint16_t ui16SrcId)
{
	return scene_data_swap_id(ui16DestId, ui16SrcId);
}

static int set_scene_new_id(uint16_t ui16NewId, uint16_t ui16Id)
{
	return scene_data_scene_new_id(ui16NewId, ui16Id);
}

int scene_read_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize)
{
	ASSERT(NULL != pvBuf);
	return scene_data_read(ui16Id, 0, pvBuf, tSize);
}

int scene_read_name(const uint16_t ui16Id, const S_AttrName *name)
{
	ASSERT(NULL != name);
	return scene_data_read(ui16Id, M_OFFSET(S_SceneDataFalsh, scene_attr.name), (void*)name, M_SIZEOF(S_SceneDataFalsh, scene_attr.name));
}

int scene_read_description(const uint16_t ui16Id, const S_AttrDescr *description)
{
	ASSERT(NULL != description);
	return scene_data_read(ui16Id, M_OFFSET(S_SceneDataFalsh, scene_attr.descr), (void*)description, M_SIZEOF(S_SceneDataFalsh, scene_attr.descr));
}

int scene_read_action_num(const uint16_t ui16Id, const uint16_t *ui16action_num)
{
	ASSERT(NULL != ui16action_num);
	return scene_data_read(ui16Id, M_OFFSET(S_SceneDataFalsh, scene_attr.action_num), (void*)ui16action_num, M_SIZEOF(S_SceneDataFalsh, scene_attr.action_num));
}

//ui16actionId 从0开始， 0位第一个
int scene_read_action(uint16_t ui16Id, uint16_t ui16actionId, S_SceneMember *pvAttr)
{
	ASSERT(NULL != pvAttr);
	return scene_data_read(ui16Id, (sizeof(S_SceneDataFalsh)+ui16actionId*sizeof(S_SceneMember)), (void*)pvAttr, sizeof(S_SceneMember));
}

int scene_read_action_all(uint16_t ui16Id, void *pvAllAttr, size_t tSize)
{
	uint16_t ui16ActionNum;
	int iRet;

	ASSERT(NULL != pvAllAttr);

	iRet = scene_read_action_num(ui16Id, &ui16ActionNum);
	if(iRet < 0)
	{
		return iRet;
	}
	if (sizeof(S_SceneMember)*ui16ActionNum > tSize)
	{
		return -1;
	}
	return scene_data_read(ui16Id, sizeof(S_SceneDataFalsh), pvAllAttr, sizeof(S_SceneMember)*ui16ActionNum);
}

int scene_write_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize)
{
	ASSERT(NULL != pvBuf);
	ASSERT(tSize <= sizeof(S_SceneDataFalsh));
	return scene_data_write(ui16Id, 0, pvBuf, tSize);
}

int scene_write_name(const uint16_t ui16Id, const S_AttrName *name)
{
	ASSERT(NULL != name);
	return scene_data_write(ui16Id, M_OFFSET(S_SceneDataFalsh, scene_attr.name), name, M_SIZEOF(S_SceneDataFalsh, scene_attr.name));
}

int scene_write_description(const uint16_t ui16Id, const S_AttrDescr *description)
{
	ASSERT(NULL != description);
	return scene_data_write(ui16Id, M_OFFSET(S_SceneDataFalsh, scene_attr.descr), description, M_SIZEOF(S_SceneDataFalsh, scene_attr.descr));
}

int scene_write_action_num(const uint16_t ui16Id, const uint16_t *ui16action_num)
{
	ASSERT(NULL != ui16action_num);
	return scene_data_write(ui16Id, M_OFFSET(S_SceneDataFalsh, scene_attr.action_num), ui16action_num, M_SIZEOF(S_SceneDataFalsh, scene_attr.action_num));
}

static int s_scene_add_member(uint16_t ui16Id, const S_SceneMember *psActionList, uint16_t ui16ActionNum)
{
	ASSERT(NULL != psActionList);
	ASSERT(ui16ActionNum <= MUI_ACTION_NUM_MAX_PER_SCENE);
	return scene_data_add_member(ui16Id, psActionList, ui16ActionNum);
}

static int s_scene_del_member(uint16_t ui16Id, const S_SceneMember *psActionList, uint16_t ui16ActionNum)
{
	ASSERT(NULL != psActionList);
	ASSERT(ui16ActionNum <= MUI_ACTION_NUM_MAX_PER_SCENE);
	return scene_data_add_member(ui16Id, psActionList, ui16ActionNum);
}

//----------------------------------------------------------------------------------------------

static int s_scene_dev_join_atcmd(uint16_t uI16id, const char *nwk, const char *ep, char *pvBuf)
{
    ASSERT(NULL != nwk);
    ASSERT(NULL != ep);
    ASSERT(NULL != pvBuf);
    char atcmd[MUI_AT_PORT_CMD_LEN_MAX] = "";
    sprintf(atcmd, AT_CMD_STORESC_S"=%s,%s,%04x,%02x\r\n", nwk, ep, SCENE_CONSTANT_GROUP_ID, uI16id);
    return zigbee_query_repeat(atcmd, strlen(atcmd), pvBuf, 100, MUI_AT_QUERY_WAIT_NORMAL_MAX, 1, MUI_AT_QUERY_INTERVAL_MIN);
}

static int s_scene_dev_del_atcmd(uint16_t uI16id, const char *nwk, const char *ep, char *pvBuf)
{
    ASSERT(NULL != nwk);
    ASSERT(NULL != ep);
    ASSERT(NULL != pvBuf);
    char atcmd[100] = "";
    sprintf(atcmd, AT_CMD_REMSC_S"=%s,%s,%04x,%02x\r\n", nwk, ep, SCENE_CONSTANT_GROUP_ID, uI16id);
    return zigbee_query_repeat(atcmd, strlen(atcmd), pvBuf, 100, MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN);
}

int scene_active(uint16_t ui16Id)
{
 	char response[MUI_AT_PORT_ANSWER_LEN_MAX] = "";
    char atcmd[100] = "";
    sprintf(atcmd, AT_CMD_RECALLSC_S"=%04x,%04x,%02x\r\n", SCENE_CONSTANT_GROUP_ID, SCENE_CONSTANT_GROUP_ID, ui16Id);
    return zigbee_query_repeat(atcmd, strlen(atcmd), response, 100, MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN);
}

int scene_del(uint16_t ui16Id)
{
	int iRet = 0;
	int i;
	char response[MUI_AT_PORT_ANSWER_LEN_MAX] = "";
	S_SceneDataFalsh	scene_data;
	S_DevZbAttr zb_attr;

	iRet = scene_read_info(ui16Id, &scene_data, sizeof(S_SceneDataFalsh));
	if (iRet < 0)
	{
		return iRet;
	}
	for (i=0; i<scene_data.scene_attr.action_num; i++)
	{
		iRet = read_dev_zb_attr(scene_data.scene_member[i].dev_type, scene_data.scene_member[i].dev_id, &zb_attr);
		if (iRet < 0)
		{
			continue;
		}
		//send
		iRet = s_scene_dev_del_atcmd(ui16Id, zb_attr.nwkaddr.str, zb_attr.ep.str, response);
		{

		}
	}
	return scene_data_del(ui16Id);
}



void scene_devaddr_add(uint16_t ui16Type, uint16_t ui16Id)
{
	int i = 0;
	for (i=0; i<SceneAtcmd.AtcmdNum; i++)
	{
		if (SceneAtcmd.SceneDevAddr[i].devType == ui16Type && SceneAtcmd.SceneDevAddr[i].devId == ui16Id)
		{
			break;
		}
	}
	if (i == SceneAtcmd.AtcmdNum)
	{
		SceneAtcmd.SceneDevAddr[i].devType = ui16Type;
		SceneAtcmd.SceneDevAddr[i].devId = ui16Id;
		SceneAtcmd.AtcmdNum ++;
	}
}

int scene_atcmd_send (uint16_t	ui16SceneId)
{
	int i = 0;
	int iRet = 0;
	S_DevZbAttr zb_attr;
	char response[MUI_AT_PORT_ANSWER_LEN_MAX] = "";

	for (i=0; i<SceneAtcmd.AtcmdNum; i++)
	{
		iRet = read_dev_zb_attr(SceneAtcmd.SceneDevAddr[i].devType, SceneAtcmd.SceneDevAddr[i].devId, &zb_attr);
		if (iRet < 0)
		{
			continue;
		}
		//send
		iRet = s_scene_dev_join_atcmd(ui16SceneId, zb_attr.nwkaddr.str, zb_attr.ep.str, response);
		{

		}
	}
	SceneAtcmd.AtcmdNum = 0;
	return iRet;
}

/*
 *
 *
 *
 */
int scene_create (const S_SceneDataFalsh *pSceneData)
{
	int i, iRet=0;
	uint16_t	ui16Id;
	S_SceneDataFalsh	scene_data;

	ASSERT(pSceneData!=NULL);

	memcpy(&scene_data.scene_attr, &pSceneData->scene_attr, sizeof(S_SceneAttr));
	scene_data.scene_attr.action_num = 0;
	SceneAtcmd.AtcmdNum = 0;
	iRet = scene_add(&scene_data);
	if(iRet < 0)
	{
		PRINT("scene_add error! :%d\r\n", iRet);
		return iRet;
	}
	ui16Id = (uint16_t)iRet;

	for (i = 0; i < pSceneData->scene_attr.action_num; i++)
	{
		switch (pSceneData->scene_member[i].action_type)
		{
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ON:
			iRet = set_dev_state_on(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.on);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_BRI:
			iRet = set_dev_state_bri(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.bri);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_SAT:
			iRet = set_dev_state_sat(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.sat);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUE:
			iRet = set_dev_state_hue(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.hue);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_CT:
			iRet = set_dev_state_ct(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.ct);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_XY:
			iRet = set_dev_state_xy(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.xy);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_LEVEL:
			iRet = set_dev_state_level(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.level);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_TEMP:
			iRet = set_dev_state_temp(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.temp);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUM:
			iRet = set_dev_state_hum(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.hum);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
			iRet = set_dev_state_warn_mode(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.warnmode);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_DURATION:
			iRet = set_dev_state_duration(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.duration);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
			iRet = set_dev_state_warn_cfg(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.warncfg);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ACTION:
			iRet = set_dev_state_action(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id, &pSceneData->scene_member[i].uSceneAtion.action);
			if (iRet > 0)
			{
				scene_devaddr_add(pSceneData->scene_member[i].dev_type, pSceneData->scene_member[i].dev_id);
			}
			break;
		default:
			break;
		}
		iRet = s_scene_add_member(ui16Id, &pSceneData->scene_member[i], 1);
		if (iRet < 0)
		{
			return iRet;
		}

	}
	iRet = scene_atcmd_send(ui16Id);
	if (iRet < 0)
	{
		return iRet;
	}
	return ui16Id;
}


int scene_dev_replace (const uint8_t type, const uint8_t id, const uint8_t newid)
{
	UNUSED(type);
	UNUSED(id);
	UNUSED(newid);

	return 0;
}

//-------------------------------------------------------------------------------------------------*

