/*
 * group.h
 *
 *  Created on: Oct 11, 2013
 *      Author: yangyuantao
 */
//#define	__M_FILE_NAME_group_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_group_H__
#define	__M_FILE_NAME_group_H__
#include "common.h"
#include "data_common.h"
#include "group_data.h"
#include "net_manage.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
/*typedef struct _GroupApiAttr
{
    S_AttrName		name;			//A unique, editable name given to the device.
    S_AttrDescr		descr;			//An editable attribute given to device. Now, it used to label the ZONE that device belongs.
} S_GroupApiAttr;

typedef struct _GroupZbAttr
{
    S_ZbAttrNwkAddr	nwkaddr;
} S_GroupZbAttr;

typedef struct _GroupOpAttr
{
    uint8_t			type;		//组所包含的设备的类型
    uint16_t			total;		//成员总数
} S_GroupOpAttr;
//................................................
typedef struct _GroupAttrFlash
{
    S_GroupApiAttr	api_attr;
    S_GroupZbAttr	zb_attr;
    S_GroupOpAttr	op_attr;
	uint16_t id_list[MUI_DEV_NUM_MAX_PER_GROUP];
} S_GroupAttrFlash;*/
/*
实际在Flash中存储应采用如下的结构
S_GroupAttrFlash
{
	........
	........
	........
	组成员ID列表
	........
	........
	........
}
*/

typedef struct _GroupStateRam
{
    S_GroupZbAttr	zb_attr;
    //S_GroupOpAttr	op_attr;
    U_DevState		state;
} S_GroupStateRam;

typedef struct
{
    uint8_t on:1;
    uint8_t bri:1;
    uint8_t sat:1;
    uint8_t hue:1;
    uint8_t ct:1;
    uint8_t xy:1;
    uint8_t color_mode:1;
    uint8_t alert:1;
    uint8_t effect:1;
    uint8_t level:1;
    uint8_t warn_mode:1;
    uint8_t duration:1;
    uint8_t warn_cfg:1;
    uint8_t action:1;
} GroupStateSetOption;

typedef struct _GroupsInfo
{
    S_GroupApiAttr		api_attr;
    U_DevState			state;
    GroupStateSetOption option;
    S_GroupOpAttr		op_attr;
} GroupsInfo;


extern int group_read_info(uint16_t ui16Id, const S_GroupAttrFlash *pvGroupFlash);
extern int group_read_name(uint16_t ui16Id, S_AttrName *name);
extern int group_get_id_list(uint16_t *aui16IdList, uint16_t ui16Capacity);
extern int group_add (const S_GroupAttrFlash *pvGroupFlash);
extern int group_del (uint16_t ui16Id);
extern int group_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
extern int group_set_new_id (uint16_t ui16NewId, uint16_t ui16Id);
extern int group_read_description (uint16_t ui16Id, S_AttrDescr *description);
extern int group_read_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *nwkaddr);
extern int group_read_total (uint16_t ui16Id, uint16_t *total);
extern int group_read_apiattr (uint16_t ui16Id, S_GroupApiAttr *api_attr);
extern int group_read_zb_attr (uint16_t ui16Id, S_GroupZbAttr	*zb_attr);
extern int group_read_op_attr (uint16_t ui16Id, S_GroupOpAttr	*op_attr);
extern int group_write_info (uint16_t ui16Id, const S_GroupAttrFlash *pvGroupFlash);
extern int group_write_name (uint16_t ui16Id, const S_AttrName *name);
extern int group_write_description (uint16_t ui16Id, const S_AttrDescr *description);
extern int group_write_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *nwkaddr);
extern int group_write_total (uint16_t ui16Id, const uint16_t *total);
extern int group_write_apiattr (uint16_t ui16Id, const S_GroupApiAttr *api_attr);
extern int group_write_zb_attr (uint16_t ui16Id, const S_GroupZbAttr	*zb_attr);
extern int group_write_op_attr (uint16_t ui16Id, const S_GroupOpAttr	*op_attr);
extern int group_add_member(uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType);
extern int group_del_member (uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType);
extern int group_create(S_GroupAttrFlash *psGroup);
extern int group_dev_join(uint16_t groupid, const uint16_t ui16DevType, const uint16_t ui16Id);
extern int group_dev_del(uint16_t groupid, const uint16_t ui16DevType, const uint16_t ui16Id);

extern int group_set_state_on (uint16_t ui16Id, const uint8_t *pui8On);
extern int group_set_state_level (uint16_t ui16Id, const uint16_t *pui16Level);
extern int group_light_state_on (uint16_t ui16Id, const uint8_t *pui8On);
extern int group_light_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri);
extern int group_light_state_xy (uint16_t ui16Id, const S_Color_XY *psXY);
extern int group_light_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct);
extern int group_warn_dev_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg);
extern int group_shade_state_action (uint16_t ui16Id, const uint8_t *pui8Action);


#endif /* __M_FILE_NAME_group_H__ */
