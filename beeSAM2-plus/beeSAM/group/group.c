#include "common.h"
#include "group.h"
#include "at_port.h"
#include "json_deal.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define GROUP_DBG

#ifndef GROUP_PUTC
#ifdef GROUP_DBG
#define GROUP_PUTC(Char)			PUTC(Char)
#else
#define GROUP_PUTC(Char)
#endif
#endif

#ifndef GROUP_PUTS
#ifdef GROUP_DBG
#define GROUP_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define GROUP_PUTS(Str, Len)
#endif
#endif

#ifndef GROUP_PUTASCC
#ifdef GROUP_DBG
#define GROUP_PUTASCC(Char)			PUTASCC(Char)
#else
#define GROUP_PUTASCC(Char)
#endif
#endif

#ifndef GROUP_PUTASCS
#ifdef GROUP_DBG
#define GROUP_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define GROUP_PUTASCS(Str, Len)
#endif
#endif

#ifndef GROUP_PRINT
#ifdef GROUP_DBG
#define GROUP_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define GROUP_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef GROUP_DYN_PUTC
#ifdef GROUP_DBG
#define GROUP_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_GROUP|Dbg), Char)
#else
#define GROUP_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef GROUP_DYN_PUTS
#ifdef GROUP_DBG
#define GROUP_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_GROUP|Dbg), Str, Len)
#else
#define GROUP_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef GROUP_DYN_PUTASCC
#ifdef GROUP_DBG
#define GROUP_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_GROUP|Dbg), Char)
#else
#define GROUP_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef GROUP_DYN_PUTASCS
#ifdef GROUP_DBG
#define GROUP_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_GROUP|Dbg), Str, Len)
#else
#define GROUP_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef GROUP_DYN_PRINT
#ifdef GROUP_DBG
#define GROUP_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_GROUP|Dbg), Fmt, ##Arg)
#else
#define GROUP_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

static int s_group_dev_join_atcmd(uint16_t groupid, const char *nwk, const char *ep, char ret[]);
static int s_group_dev_del_atcmd(uint16_t groupid, const char *nwk, const char *ep, char ret[]);
static int s_group_assign_id (void);

//------------------------------------------------------------------------------------------------------------------
int group_get_id_list(uint16_t *aui16IdList, uint16_t ui16Capacity)
{
	return group_data_id_list_get(aui16IdList, ui16Capacity);
}

static int s_group_assign_id()
{
	return group_data_assign_id();
}


static int s_group_dev_join_atcmd(uint16_t groupid, const char *nwk, const char *ep, char ret[])
{
    ASSERT(NULL != nwk);
    ASSERT(NULL != ep);
    ASSERT(NULL != ret);
    char atcmd[MUI_AT_PORT_CMD_LEN_MAX] = "";
    sprintf(atcmd, AT_CMD_ADDGR_S"=%s,%s,%04x,group%d\r\n", nwk, ep, groupid+1, groupid);
    return zigbee_query_repeat(atcmd, strlen(atcmd), ret, 100, MUI_AT_QUERY_WAIT_NORMAL_MAX, 1, MUI_AT_QUERY_INTERVAL_MIN);
}

static int s_group_dev_del_atcmd(uint16_t groupid, const char *nwk, const char *ep, char ret[])
{
    ASSERT(NULL != nwk);
    ASSERT(NULL != ep);
    ASSERT(NULL != ret);
    char atcmd[100] = "";
    sprintf(atcmd, AT_CMD_REMGR_S"=%s,%s,%04x\r\n", nwk, ep, groupid+1);
    return zigbee_query_repeat(atcmd, strlen(atcmd), ret, 100, MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN);
}

int group_dev_join(uint16_t groupid, const uint16_t ui16DevType, const uint16_t ui16Id)
{
    ASSERT(ui16DevType >= MUI_DATA_TYPE_DEV_START && ui16DevType <= MUI_DATA_TYPE_DEV_END);

    int iRet;
    char data[100]="";
	S_DevZbAttr zb_attr;
	S_GroupOpAttr opattr;

    iRet = group_read_op_attr(groupid, &opattr);
    if(iRet < 0)
    {
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_dev_join read group info error:%d\r\n", iRet);
        return GROUP_READ_ERROR;
    }

    switch(ui16DevType)
    {
    case MUI_DATA_TYPE_LIGHT:
        iRet = read_light_zb_attr(ui16Id, &zb_attr);
        break;
    case MUI_DATA_TYPE_OCC_SENSOR:
		iRet = -1;
        break;
    case MUI_DATA_TYPE_BIN_SENSOR:
		iRet = -1;
        break;
    case MUI_DATA_TYPE_TEMP_SENSOR:
		iRet = -1;
        break;
    case MUI_DATA_TYPE_HUM_SENSOR:
		iRet = -1;
        break;
    case MUI_DATA_TYPE_TH_SENSOR:
        iRet = -1;
        break;
    case MUI_DATA_TYPE_BIN_CTRL:
        iRet = read_bin_ctrl_zb_attr(ui16Id, &zb_attr);
        break;
    case MUI_DATA_TYPE_LEVEL_CTRL:
        iRet = read_level_ctrl_zb_attr(ui16Id, &zb_attr);
        break;
    case MUI_DATA_TYPE_WARN_DEV:
        iRet = read_warn_dev_zb_attr(ui16Id, &zb_attr);
        break;
    case MUI_DATA_TYPE_PLUG:
        iRet = read_plug_zb_attr(ui16Id, &zb_attr);
        break;
    case MUI_DATA_TYPE_SWITCH:
        iRet = read_switch_zb_attr(ui16Id, &zb_attr);
        break;
    case MUI_DATA_TYPE_SHADE:
        iRet = read_shade_zb_attr(ui16Id, &zb_attr);
        break;
    default:
		iRet = -1;
        break;
    }
	if(iRet < 0)
	{
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "[group_dev_join] Call: read_dev_zb_attr error:%d\r\n", iRet);
		return GROUP_READ_ZB_ATTR_ERROR;
	}
	//判断group目前设备数目，超过MUI_DEV_NUM_MAX_PER_GROUP或达到就不再添加
	if (opattr.total >= MUI_DEV_NUM_MAX_PER_GROUP)
	{
		return GROUP_DEV_NUM_ERROR;
	}

    iRet = s_group_dev_join_atcmd(groupid, zb_attr.nwkaddr.str, zb_attr.ep.str, data);
	if(iRet <= 0)
	{
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "[group_dev_join] Call: s_group_dev_join_atcmd error:%d\r\n", iRet);
        return GROUP_ZIGBEE_ERROR;
	}

	iRet = at_cmd_answer_verify(data, iRet);
    if (iRet < 0)
    {
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "[group_dev_join] Call: at_cmd_answer_verify error:%d\r\n", iRet);
        return GROUP_AT_ANSWER_VERIFY_ERROR;
    }

	iRet = group_add_member(groupid, ui16Id, ui16DevType);
    if ( iRet < 0)
    {
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_dev_join wirte group flash info error:%d\r\n", iRet);
		return GROUP_WRITE_ERROR;
	}
	return iRet;
}

int group_dev_del(uint16_t groupid, const uint16_t ui16DevType, const uint16_t ui16Id)
{
    ASSERT(ui16DevType >= MUI_DATA_TYPE_DEV_START && ui16DevType <= MUI_DATA_TYPE_DEV_END);
	int iRet;
    S_DevZbAttr zb_attr;
	S_GroupOpAttr opattr;

    iRet = group_read_op_attr(groupid, &opattr);
    if(iRet < 0)
    {
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_dev_del read group info error:%d\r\n", iRet);
        return GROUP_READ_ERROR;
    }

	if (0 == opattr.total)
	{
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_dev_del delete group error\r\n");
		return GROUP_DELETE_ERROR;
	}

    switch(ui16DevType)
    {
	    case MUI_DATA_TYPE_LIGHT:
	        iRet = read_light_zb_attr(ui16Id, &zb_attr);
	        break;
	    case MUI_DATA_TYPE_OCC_SENSOR:
	        iRet = -1;
	        break;
	    case MUI_DATA_TYPE_BIN_SENSOR:
	        iRet = -1;
	        break;
	    case MUI_DATA_TYPE_TEMP_SENSOR:
	        iRet = -1;
	        break;
	    case MUI_DATA_TYPE_HUM_SENSOR:
	        iRet = -1;
	        break;
	    case MUI_DATA_TYPE_TH_SENSOR:
	        iRet = -1;
	        break;
	    case MUI_DATA_TYPE_BIN_CTRL:
	        iRet = read_bin_ctrl_zb_attr(ui16Id, &zb_attr);
	        break;
	    case MUI_DATA_TYPE_LEVEL_CTRL:
	        iRet = read_level_ctrl_zb_attr(ui16Id, &zb_attr);
	        break;
	    case MUI_DATA_TYPE_WARN_DEV:
	        iRet = read_warn_dev_zb_attr(ui16Id, &zb_attr);
	        break;
	    case MUI_DATA_TYPE_PLUG:
	        iRet = read_plug_zb_attr(ui16Id, &zb_attr);
	        break;
	    case MUI_DATA_TYPE_SWITCH:
	        iRet = read_switch_zb_attr(ui16Id, &zb_attr);
	        break;
	    case MUI_DATA_TYPE_SHADE:
	        iRet = read_shade_zb_attr(ui16Id, &zb_attr);
	        break;
	    default:
	        iRet = -1;
	        break;
    }
	if (iRet < 0)
	{
		return iRet;
	}
    char data[100]="";
    iRet = s_group_dev_del_atcmd(groupid, zb_attr.nwkaddr.str, zb_attr.ep.str, data);
	if(iRet < 0)
	{
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_dev_del group zigbee error\r\n");
		return GROUP_ZIGBEE_ERROR;
	}
	iRet = at_cmd_answer_verify(data, iRet);
    if (iRet < 0)
    {
        return iRet;
    }
	iRet = group_del_member(groupid, ui16Id, ui16DevType);
    if ( iRet < 0)
    {
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_dev_del wirte group flash info error:%d\r\n", iRet);
		return GROUP_WRITE_ERROR;
	}

	return iRet;
}

int group_create(S_GroupAttrFlash *psGroup)
{
	int i,iRet, iTmp=0;
	uint16_t groupid;
    S_GroupAttrFlash attrgroup_Flash;

    iRet = group_add(&attrgroup_Flash);
	if (iRet < 0)
    {
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_create group add error\r\n");
        return GROUP_ADD_ERROR;
    }
	groupid = (uint16_t)iRet;

	if( strcmp(psGroup->api_attr.name.str, "") == 0)
	{
    	sprintf(attrgroup_Flash.api_attr.name.str, "group %d", groupid);
	}
	else
	{
		strcpy(attrgroup_Flash.api_attr.name.str, psGroup->api_attr.name.str);
	}

	if( strcmp(psGroup->api_attr.descr.str, "") == 0)
	{
    	sprintf(attrgroup_Flash.api_attr.descr.str, "%s", "");
	}
	else
	{
		strcpy(attrgroup_Flash.api_attr.descr.str, psGroup->api_attr.descr.str);
	}
    sprintf(attrgroup_Flash.zb_attr.nwkaddr.str, "%04x", groupid+1);
	attrgroup_Flash.op_attr.total = 0;
	//memcpy(&attrgroup_Flash.op_attr, &psGroup->op_attr, sizeof(S_GroupOpAttr));

	iRet = group_write_info(groupid, &attrgroup_Flash);
	if (iRet < 0)
	{
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "[group_create] group_write_info error devid:%d\r\n", iRet);
		return GROUP_WRITE_ERROR;
	}

    for(i=0; i<psGroup->op_attr.total; i++)
    {
        iRet = group_dev_join(groupid, psGroup->op_attr.member[i].devType, psGroup->op_attr.member[i].devId);
		if(iRet <= 0)
		{
			iTmp++;
			GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "[group_create] group_dev_join error devid:%d\r\n", psGroup->op_attr.member[i].devId);
        }
    }
	if (iTmp == psGroup->op_attr.total)
	{
		group_del(groupid);
		return GROUP_ZIGBEE_ERROR;
	}

    return groupid;
}

static int group_delete(uint16_t groupid)
{
	S_GroupOpAttr opattr;
	int i;
	int iRet;
	int tmp=0;

	iRet = group_read_op_attr(groupid, &opattr);
	if (iRet < 0)
	{
		GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_delete group delete read_op_attr error\r\n");
		return iRet;
	}

	for(i = 0; i < opattr.total; i++)
	{
		if(group_dev_del(groupid, opattr.member[i].devType, opattr.member[i].devId) < 0)
		{
			GROUP_DYN_PRINT(M_DBG_LEVEL_ERR, "group_delete group del dev error devid:%d\r\n", opattr.member[i].devId);
			tmp++;
		}
	}
	/*if(0 == tmp)
	{
		return group_data_del(groupid);
	}*/
    return group_data_del(groupid);
}

int groups_dev_check(const uint16_t type, const uint16_t id)
{
	int i = 0;
	int j = 0;
	int iRet = 0;
	uint16_t num = 0;
	int group_num = 0;
    uint16_t id_buf[MUI_GROUP_DATA_NUM_MAX];
	S_GroupOpAttr group_op_attr;
    group_num = group_get_id_list(id_buf, MUI_GROUP_DATA_NUM_MAX);

	for(i=0; i<group_num; i++)
	{
		iRet = group_read_op_attr(id_buf[i], &group_op_attr);
		if(iRet > 0)
		{
			for(j=0; j<group_op_attr.total; j++)
			{
				if(group_op_attr.member[j].devId == id && group_op_attr.member[j].devType == type)
				{
					num++;
				}
			}
		}
	}
	return num;
}

//关联删除
int groups_dev_relation_del(const uint16_t type, const uint16_t id)
{
	int i = 0;
	int j = 0;
	int iRet = 0;
	uint16_t num = 0;
	int group_num = 0;
    uint16_t id_buf[MUI_GROUP_DATA_NUM_MAX];
	S_GroupOpAttr group_op_attr;
    group_num = group_get_id_list(id_buf, MUI_GROUP_DATA_NUM_MAX);

	for(i=0; i<group_num; i++)
	{
		iRet = group_read_op_attr(id_buf[i], &group_op_attr);
		if(iRet > 0)
		{
			for(j=0; j<group_op_attr.total; j++)
			{
				if(group_op_attr.member[j].devId == id && group_op_attr.member[j].devType == type)
				{
					iRet = group_dev_del(id_buf[i], type, id);
					if( iRet > 0 )
					{
						num++;
					}
				}
			}
		}
	}
	return num;
}


int group_add(const S_GroupAttrFlash *pvGroupFlash)
{
	return group_data_add(pvGroupFlash);
}

int group_del(uint16_t ui16Id)
{
	return group_delete(ui16Id);
}

int group_swap_id(uint16_t ui16DestId, uint16_t ui16SrcId)
{
	return group_data_swap_id(ui16DestId, ui16SrcId);
}

int group_set_new_id(uint16_t ui16NewId, uint16_t ui16Id)
{
	return group_data_set_new_id(ui16NewId, ui16Id);
}

int group_read_info(uint16_t ui16Id, const S_GroupAttrFlash *pvGroupFlash)
{
	ASSERT(NULL != pvGroupFlash);
	return group_data_read(ui16Id, 0, (void*)pvGroupFlash, sizeof(S_GroupAttrFlash));
}

int group_read_name(uint16_t ui16Id, S_AttrName *name)
{
	ASSERT(NULL != name);
	return group_data_read(ui16Id, M_OFFSET(S_GroupAttrFlash, api_attr.name), name, M_SIZEOF(S_GroupAttrFlash, api_attr.name));
}

int group_read_description(uint16_t ui16Id, S_AttrDescr *description)
{
	ASSERT(NULL != description);
	return group_data_read(ui16Id, M_OFFSET(S_GroupAttrFlash, api_attr.descr), description, M_SIZEOF(S_GroupAttrFlash, api_attr.descr));
}

int group_read_nwkaddr(uint16_t ui16Id, S_ZbAttrNwkAddr *nwkaddr)
{
	ASSERT(NULL != nwkaddr);
	return group_data_read(ui16Id, M_OFFSET(S_GroupAttrFlash, zb_attr.nwkaddr), nwkaddr, M_SIZEOF(S_GroupAttrFlash, zb_attr.nwkaddr));
}

int group_read_total(uint16_t ui16Id, uint16_t *total)
{
	ASSERT(NULL != total);
	return group_data_read(ui16Id, M_OFFSET(S_GroupAttrFlash, op_attr.total), total, M_SIZEOF(S_GroupAttrFlash, op_attr.total));
}

int group_read_apiattr(uint16_t ui16Id, S_GroupApiAttr *api_attr)
{
	ASSERT(NULL != api_attr);
	return group_data_read(ui16Id, M_OFFSET(S_GroupAttrFlash, api_attr), api_attr, M_SIZEOF(S_GroupAttrFlash, api_attr));
}

int group_read_zb_attr(uint16_t ui16Id, S_GroupZbAttr *zb_attr)
{
	ASSERT(NULL != zb_attr);
	return group_data_read(ui16Id, M_OFFSET(S_GroupAttrFlash, zb_attr), zb_attr, M_SIZEOF(S_GroupAttrFlash, zb_attr));
}

int group_read_op_attr(uint16_t ui16Id, S_GroupOpAttr *op_attr)
{
	ASSERT(NULL != op_attr);
	return group_data_read(ui16Id, M_OFFSET(S_GroupAttrFlash, op_attr), op_attr, M_SIZEOF(S_GroupAttrFlash, op_attr));
}

//-------------------------------------------------------------------------------------------------------------
int group_write_info(uint16_t ui16Id, const S_GroupAttrFlash *pvGroupFlash)
{
	return group_data_write(ui16Id, 0, pvGroupFlash, sizeof(S_GroupAttrFlash));
}

int group_write_name(uint16_t ui16Id, const S_AttrName *name)
{
	ASSERT(NULL != name);
	return group_data_write(ui16Id, M_OFFSET(S_GroupAttrFlash, api_attr.name), name, M_SIZEOF(S_GroupAttrFlash, api_attr.name));
}

int group_write_description(uint16_t ui16Id, const S_AttrDescr *description)
{
	ASSERT(NULL != description);
	return group_data_write(ui16Id, M_OFFSET(S_GroupAttrFlash, api_attr.descr), description, M_SIZEOF(S_GroupAttrFlash, api_attr.descr));
}

int group_write_nwkaddr(uint16_t ui16Id, const S_ZbAttrNwkAddr *nwkaddr)
{
	ASSERT(NULL != nwkaddr);
	return group_data_write(ui16Id, M_OFFSET(S_GroupAttrFlash, zb_attr.nwkaddr), nwkaddr, M_SIZEOF(S_GroupAttrFlash, zb_attr.nwkaddr));
}

int group_write_total(uint16_t ui16Id, const uint16_t *total)
{
	ASSERT(NULL != total);
	return group_data_write(ui16Id, M_OFFSET(S_GroupAttrFlash, op_attr.total), total, M_SIZEOF(S_GroupAttrFlash, op_attr.total));
}

int group_write_apiattr(uint16_t ui16Id, const S_GroupApiAttr *api_attr)
{
	ASSERT(NULL != api_attr);
	return group_data_write(ui16Id, M_OFFSET(S_GroupAttrFlash, api_attr), api_attr, M_SIZEOF(S_GroupAttrFlash, api_attr));
}

int group_write_zb_attr(uint16_t ui16Id, const S_GroupZbAttr *zb_attr)
{
	ASSERT(NULL != zb_attr);
	return group_data_write(ui16Id, M_OFFSET(S_GroupAttrFlash, zb_attr), zb_attr, M_SIZEOF(S_GroupAttrFlash, zb_attr));
}

int group_write_op_attr(uint16_t ui16Id, const S_GroupOpAttr *op_attr)
{
	ASSERT(NULL != op_attr);
	return group_data_write(ui16Id, M_OFFSET(S_GroupAttrFlash, op_attr), op_attr, M_SIZEOF(S_GroupAttrFlash, op_attr));
}


//-----------------------------------------------------------------------------------------------------------
int group_add_member(uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType)
{
	return group_data_add_member(ui16Id, ui16DevId, ui16DevType);
}

int group_del_member(uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType)
{
	return group_data_del_member(ui16Id, ui16DevId, ui16DevType);
}


int group_light_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
#ifdef AT_PORT_UNQUEUE_QUERY
    char achAnswer[MUI_AT_ANSWER_LEN_ON];
#endif
    int iRet;
	S_GroupAttrFlash group;

    ASSERT(NULL != pui8On);

	if (ui16Id == 0)
	{
		strcpy(group.zb_attr.nwkaddr.str, "0001");
	}
	else
	{
		iRet = group_read_info(ui16Id, &group);
		if (0 > iRet)
		{
			return GROUP_READ_ERROR;
		}
	}

    if (*pui8On)
    {
#ifdef AT_PORT_UNQUEUE_QUERY
        iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
#else
        iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
#endif
    }
    else
    {
#ifdef AT_PORT_UNQUEUE_QUERY
        iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
#else
        iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
#endif
    }
    if (iRet <= 0)
    {
        return iRet;
    }
#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = at_cmd_answer_verify(achAnswer, iRet);
    if (iRet < 0)
    {
        return iRet;
    }
#endif
    return iRet;
}

int group_light_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
#ifdef AT_PORT_UNQUEUE_QUERY
    char achAnswer[MUI_AT_ANSWER_LEN_BRI];
#endif
    int iRet;
	S_GroupAttrFlash group;

    ASSERT(NULL != pui8Bri);

	if (ui16Id == 0)
	{
		strcpy(group.zb_attr.nwkaddr.str, "0001");
	}
	else
	{
		iRet = group_read_info(ui16Id, &group);
		if (0 > iRet)
		{
			return GROUP_READ_ERROR;
		}
	}

#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOLV_S"=%s,%02x,000A\r\n", group.zb_attr.nwkaddr.str, *pui8Bri);
#else
    iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_MVTOLV_S"=%s,%02x,000A\r\n", group.zb_attr.nwkaddr.str, *pui8Bri);
#endif
    if (iRet <= 0)
    {
        return iRet;
    }
#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = at_cmd_answer_verify(achAnswer, iRet);
    if (iRet < 0)
    {
        return iRet;
    }
#endif
    return iRet;
}

int group_light_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
#ifdef AT_PORT_UNQUEUE_QUERY
    char achAnswer[MUI_AT_ANSWER_LEN_XY];
#endif
    int iRet;
	S_GroupAttrFlash group;

    ASSERT(NULL != psXY);

	if (ui16Id == 0)
	{
		strcpy(group.zb_attr.nwkaddr.str, "0001");
	}
	else
	{
		iRet = group_read_info(ui16Id, &group);
		if (0 > iRet)
		{
			PRINT("[group_light_state_xy] group_read_info fail : %d id = %hu\r\n",iRet, ui16Id);
			return GROUP_READ_ERROR;
		}
	}

#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOS_XY"=%s,%04x,%04x,0001\r\n", group.zb_attr.nwkaddr.str, psXY->x, psXY->y);
#else
    iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_MVTOS_XY"=%s,%04x,%04x,0001\r\n", group.zb_attr.nwkaddr.str, psXY->x, psXY->y);
#endif
    if (iRet <= 0)
    {
        return iRet;
    }
#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = at_cmd_answer_verify(achAnswer, iRet);
    if (iRet < 0)
    {
        return iRet;
    }
#endif
    return iRet;
}

int group_light_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_CT];
#endif
	//S_DevZbAttr sZbAttr;
	//uint8_t ui8ColorMode;
	uint16_t ui16Ct;
	int iRet;

	ASSERT(NULL != pui16Ct);

	/*iRet = read_light_state_colormode(ui8Id, &ui8ColorMode);
	if (iRet <= 0)
	{
		return iRet;
	}
	if (MUI_LIGHT_COLOR_MODE_CT != ui8ColorMode)
	{
		return DEV_IF_ERR_UNSUPPORT;
	}

	iRet = read_light_zb_attr(ui8Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}*/

	S_GroupAttrFlash group;

	iRet = group_read_info(ui16Id, &group);
	if (0 > iRet)
	{
		return GROUP_READ_ERROR;
	}

	//值范围修正
	ui16Ct = *pui16Ct;
	if (ui16Ct < MUI_LIGHT_STATE_CT_MIN)
	{
		ui16Ct = MUI_LIGHT_STATE_CT_MIN;
	}
	else if (ui16Ct > MUI_LIGHT_STATE_CT_MAX)
	{
		ui16Ct = MUI_LIGHT_STATE_CT_MAX;
	}

#ifdef AT_PORT_UNQUEUE_QUERY
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOCT_S"=%s,%04x,000A\r\n", group.zb_attr.nwkaddr.str, ui16Ct);
#else
	iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_MVTOCT_S"=%s,%04x,000A\r\n", group.zb_attr.nwkaddr.str, ui16Ct);
#endif
	if (iRet <= 0)
	{
		return iRet;
	}
#ifdef AT_PORT_UNQUEUE_QUERY
	iRet = at_cmd_answer_verify(achAnswer, iRet);
	if (iRet < 0)
	{
		return iRet;
	}
#endif
	//iRet = write_light_state_ct(ui8Id, &ui16Ct);

	return iRet;
}

int group_set_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
#ifdef AT_PORT_UNQUEUE_QUERY
    char achAnswer[MUI_AT_ANSWER_LEN_ON];
#endif
    int iRet;

    ASSERT(NULL != pui8On);

	S_GroupAttrFlash group;

	iRet = group_read_info(ui16Id, &group);
	if (0 > iRet)
	{
		GROUP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "group_read_info failed!\r\n");
		return GROUP_READ_ERROR;
	}
    if (*pui8On)
    {
	#ifdef AT_PORT_UNQUEUE_QUERY
        iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
	#else
        iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
	#endif
    }
    else
    {
	#ifdef AT_PORT_UNQUEUE_QUERY
        iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
	#else
        iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
	#endif
    }
    if (iRet <= 0)
    {
        return iRet;
    }
#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = at_cmd_answer_verify(achAnswer, iRet);
    if (iRet < 0)
    {
        return iRet;
    }
#endif
    return iRet;
}

int group_set_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
#ifdef AT_PORT_UNQUEUE_QUERY
    char achAnswer[MUI_AT_ANSWER_LEN_LEVEL];
#endif
    int iRet;

    ASSERT(NULL != pui16Level);

	S_GroupAttrFlash group;

	iRet = group_read_info(ui16Id, &group);
	if (0 > iRet)
	{
		return GROUP_READ_ERROR;
	}

#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOLV_S"=%s,%02x,000A\r\n", group.zb_attr.nwkaddr.str, *pui16Level);
#else
    iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_MVTOLV_S"=%s,%02x,000A\r\n", group.zb_attr.nwkaddr.str, *pui16Level);
#endif
    if (iRet <= 0)
    {
        return iRet;
    }
#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = at_cmd_answer_verify(achAnswer, iRet);
    if (iRet < 0)
    {
        return iRet;
    }
#endif
    return iRet;
}

int group_warn_dev_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
#ifdef AT_PORT_UNQUEUE_QUERY
    char achAnswer[MUI_AT_ANSWER_LEN_WARN_CFG];
#endif
    int iRet;
    ASSERT(NULL != psWarnCfg);

	S_GroupAttrFlash group;

	iRet = group_read_info(ui16Id, &group);
	if (0 > iRet)
	{
		return GROUP_READ_ERROR;
	}

#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_STWRN_S"=%s,%02x,%04x\r\n", group.zb_attr.nwkaddr.str, psWarnCfg->warnmode, psWarnCfg->duration);
#else
    iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", group.zb_attr.nwkaddr.str, psWarnCfg->warnmode, psWarnCfg->duration);
#endif
    if (iRet <= 0)
    {
        return iRet;
    }
#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = at_cmd_answer_verify(achAnswer, iRet);
    if (iRet < 0)
    {
        return iRet;
    }
#endif
    return iRet;
}

int group_shade_state_action (uint16_t ui16Id,const uint8_t *pui8Action)
{
    uint8_t ui8CurAction;
#ifdef AT_PORT_UNQUEUE_QUERY
    char achAnswer[MUI_AT_ANSWER_LEN_ACTION];
#endif
    int iRet;

    ASSERT(NULL != pui8Action);

	S_GroupAttrFlash group;

	iRet = group_read_info(ui16Id, &group);
	if (0 > iRet)
	{
		return GROUP_READ_ERROR;
	}

    ui8CurAction = *pui8Action;
    //数据归一化: 0-OFF; 100-STOP; 255-ON
    if ((ui8CurAction!=0) && (ui8CurAction!=255))
    {
        ui8CurAction = 100;	//默认值为100,即为STOP
    }

    /*iRet = group_read_info(ui8Id, &ui8Action, 8, sizeof(uint8_t), M_MEM_TYPE_RAM);
    if (iRet <= 0)
    {
        return iRet;
    }
    //数据归一化: 0-OFF; 100-STOP; 255-ON
    if ((ui8Action!=0) && (ui8Action!=255))
    {
        ui8Action = 100;	//默认值为100,即为STOP
    }*/

    /*if (ui8Action == *pui8Action)
    {
        return 0;	//动作没有改变,直接返回!!!!返回值待优化
    }*/

    if (100 == ui8CurAction)
    {
        if (ui8CurAction != 0)
        {
		#ifdef AT_PORT_UNQUEUE_QUERY
            iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
		#else
            iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
		#endif
        }
        else
        {
		#ifdef AT_PORT_UNQUEUE_QUERY
            iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
		#else
            iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
		#endif
        }
        if (iRet <= 0)
        {
            return iRet;
        }
    }
    else
    {
        if (ui8CurAction != 0)
        {
		#ifdef AT_PORT_UNQUEUE_QUERY
            iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
		#else
            iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
		#endif
        }
        else
        {
		#ifdef AT_PORT_UNQUEUE_QUERY
            iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
		#else
            iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
		#endif
        }
        if (iRet <= 0)
        {
            return iRet;
        }
        if (ui8CurAction != 100)
        {
            msleep(500);
            if (ui8CurAction != 0)
            {
			#ifdef AT_PORT_UNQUEUE_QUERY
                iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
			#else
                iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_ON_S"=%s\r\n", group.zb_attr.nwkaddr.str);
			#endif
            }
            else
            {
			#ifdef AT_PORT_UNQUEUE_QUERY
                iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
			#else
                iRet = zigbee_print_queue_query(MUI_DATA_TYPE_GROUP, ui16Id, AT_CMD_OFF_S"=%s\r\n", group.zb_attr.nwkaddr.str);
			#endif
            }
            if (iRet <= 0)
            {
                return iRet;
            }
        }
    }
#ifdef AT_PORT_UNQUEUE_QUERY
    iRet = at_cmd_answer_verify(achAnswer, iRet);
    if (iRet < 0)
    {
        return iRet;
    }
#endif
    return iRet;
}

