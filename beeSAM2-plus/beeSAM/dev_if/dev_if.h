//#define	__M_FILE_NAME_dev_if_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_dev_if_H__
#define	__M_FILE_NAME_dev_if_H__

#include "common.h"
#include "sys_time.h"
#include "data_common.h"
#include "database.h"
#include "light.h"
#include "occ_sensor.h"
#include "bin_sensor.h"
#include "temp_sensor.h"
#include "hum_sensor.h"
#include "bin_ctrl.h"
#include "level_ctrl.h"
#include "plug.h"
#include "warn_dev.h"
#include "switch.h"
#include "shade.h"
#include "sonos.h"
#include "report.h"
#include "auto_report.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int init_dev_ram_data (uint16_t ui16DevType, uint16_t ui16Id, U_DevStateSave *puState);
//--------------------------------------------------------------
extern int read_dev_attr (uint16_t ui16DevType, uint16_t ui16Id, S_DevApiAttr *psApiAttr);
//................................................
extern int read_dev_attr_type (uint16_t ui16DevType, uint16_t ui16Id, S_AttrType *psType);
extern int read_dev_attr_name (uint16_t ui16DevType, uint16_t ui16Id, S_AttrName *psName);
extern int read_dev_attr_descr (uint16_t ui16DevType, uint16_t ui16Id, S_AttrDescr *psDescr);
extern int read_dev_attr_modelid (uint16_t ui16DevType, uint16_t ui16Id, S_AttrModelId *psModelId);
extern int read_dev_attr_swversion (uint16_t ui16DevType, uint16_t ui16Id, S_AttrSwVer *psSwVer);
extern int read_dev_attr_changerpt (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8ChangeRpt);
extern int read_dev_attr_autorpt (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16AutoRpt);
//--------------------------------------------------------------
extern int read_dev_zb_attr (uint16_t ui16DevType, uint16_t ui16Id, S_DevZbAttr *psZbAttr);
//................................................
extern int read_dev_zb_attr_nwkaddr (uint16_t ui16DevType, uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr);
extern int read_dev_zb_attr_ep (uint16_t ui16DevType, uint16_t ui16Id, S_ZbAttrEp *psEp);
//--------------------------------------------------------------
extern int read_dev_ext_attr (uint16_t ui16DevType, uint16_t ui16Id, U_DevExtAttr *puExtAttr);
//--------------------------------------------------------------
extern int read_dev_state (uint16_t ui16DevType, uint16_t ui16Id, U_DevState *puState);
//................................................
extern int read_dev_state_reachable (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Reachable);
extern int read_dev_state_on (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8On);
extern int read_dev_state_bri (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Bri);
extern int read_dev_state_sat (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Sat);
extern int read_dev_state_hue (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Hue);
extern int read_dev_state_ct (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Ct);
extern int read_dev_state_xy (uint16_t ui16DevType, uint16_t ui16Id, S_Color_XY *psXY);
extern int rea_dev_state_colormode (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8ColorMode);
extern int read_dev_state_alert (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Alert);
extern int read_dev_state_effect (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Effect);
extern int read_dev_state_level (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Level);
extern int read_dev_state_count (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Count);
extern int read_dev_state_temp (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Temp);
extern int read_dev_state_hum (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Hum);
extern int read_dev_state_consumption (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Consumption);
extern int rea_dev_state_warn_mode (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8WarnMode);
extern int read_dev_state_duration (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Duration);
extern int read_dev_state_warn_cfg (uint16_t ui16DevType, uint16_t ui16Id, S_WarnCfg *psWarnCfg);
extern int read_dev_state_action (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Action);
extern int rea_dev_state_power (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Power);
extern int read_dev_state_play (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Play);
extern int read_dev_state_volume (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Volume);
extern int read_dev_state_url_id (uint16_t ui16DevType, uint16_t ui16Id, uint32_t *pui32UrlId);
//--------------------------------------------------------------
extern int read_dev_misc (uint16_t ui16DevType, uint16_t ui16Id, S_DevMiscState *psMisc);
//................................................
extern int read_dev_misc_at_err_tick (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *psMisc);


//--------------------------------------------------------------
extern int write_dev_attr (uint16_t ui16DevType, uint16_t ui16Id, const S_DevApiAttr *psApiAttr);
//................................................
extern int write_dev_attr_type (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrType *psType);
extern int write_dev_attr_name (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrName *psName);
extern int write_dev_attr_descr (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrDescr *psDescr);
extern int write_dev_attr_modelid (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrModelId *psModelId);
extern int write_dev_attr_swversion (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrSwVer *psSwVer);
extern int write_dev_attr_changerpt (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8ChangeRpt);
extern int write_dev_attr_autorpt (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16AutoRpt);
//--------------------------------------------------------------
extern int write_dev_zb_attr (uint16_t ui16DevType, uint16_t ui16Id, const S_DevZbAttr *psZbAttr);
//................................................
extern int write_dev_zb_attr_nwkaddr (uint16_t ui16DevType, uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr);
extern int write_dev_zb_attr_ep (uint16_t ui16DevType, uint16_t ui16Id, const S_ZbAttrEp *psEp);
//--------------------------------------------------------------
extern int write_dev_ext_attr (uint16_t ui16DevType, uint16_t ui16Id, const U_DevExtAttr *puExtAttr);
//--------------------------------------------------------------
extern int write_dev_state (uint16_t ui16DevType, uint16_t ui16Id, const U_DevState *puState);
//................................................
extern int write_dev_state_reachable (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Reachable);
extern int write_dev_state_on (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8On);
extern int write_dev_state_bri (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Bri);
extern int write_dev_state_sat (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Sat);
extern int write_dev_state_hue (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Hue);
extern int write_dev_state_ct (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Ct);
extern int write_dev_state_xy (uint16_t ui16DevType, uint16_t ui16Id, const S_Color_XY *psXY);
extern int write_dev_state_colormode (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8ColorMode);
extern int write_dev_state_alert (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Alert);
extern int write_dev_state_effect (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Effect);
extern int write_dev_state_level (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Level);
extern int write_dev_state_count (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Count);
extern int write_dev_state_temp (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Temp);
extern int write_dev_state_hum (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Hum);
extern int write_dev_state_consumption (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Consumption);
extern int write_dev_state_warn_mode (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8WarnMode);
extern int write_dev_state_duration (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Duration);
extern int write_dev_state_warn_cfg (uint16_t ui16DevType, uint16_t ui16Id, const S_WarnCfg *psWarnCfg);
extern int write_dev_state_action (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Action);
extern int write_dev_state_power (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Power);
extern int write_dev_state_play (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Play);
extern int write_dev_state_volume (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Volume);
extern int write_dev_state_url_id (uint16_t ui16DevType, uint16_t ui16Id, const uint32_t *pui32UrlId);
//--------------------------------------------------------------
extern int write_dev_misc (uint16_t ui16DevType, uint16_t ui16Id, const S_DevMiscState *psMisc);
//................................................
extern int write_dev_misc_at_err_tick (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *psMisc);


//--------------------------------------------------------------
extern int get_dev_state (uint16_t ui16DevType, uint16_t ui16Id, U_DevState *puState);
extern int get_dev_state_reachable (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Reachable);
extern int get_dev_state_on (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8On);
extern int get_dev_state_bri (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Bri);
extern int get_dev_state_sat (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Sat);
extern int get_dev_state_hue (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Hue);
extern int get_dev_state_ct (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Ct);
extern int get_dev_state_xy (uint16_t ui16DevType, uint16_t ui16Id, S_Color_XY *psXY);
extern int get_dev_state_colormode (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8ColorMode);
extern int get_dev_state_alert (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Alert);
extern int get_dev_state_effect (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Effect);
extern int get_dev_state_level (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Level);
extern int get_dev_state_count (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Count);
extern int get_dev_state_temp (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Temp);
extern int get_dev_state_hum (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Hum);
extern int get_dev_state_consumption (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Consumption);
extern int get_dev_state_warn_mode (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8WarnMode);
extern int get_dev_state_duration (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Duration);
extern int get_dev_state_warn_cfg (uint16_t ui16DevType, uint16_t ui16Id, S_WarnCfg *psWarnCfg);
extern int get_dev_state_action (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Action);
extern int get_dev_state_power (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Power);
extern int get_dev_state_play (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Play);
extern int get_dev_state_volume (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Volume);
extern int get_dev_state_url_id (uint16_t ui16DevType, uint16_t ui16Id, uint32_t *pui32UrlId);
//--------------------------------------------------------------
extern int set_dev_state (uint16_t ui16DevType, uint16_t ui16Id, const U_DevState *puState);
extern int set_dev_state_reachable (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Reachable);
extern int set_dev_state_on (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8On);
extern int set_dev_state_bri (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Bri);
extern int set_dev_state_sat (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Sat);
extern int set_dev_state_hue (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Hue);
extern int set_dev_state_ct (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Ct);
extern int set_dev_state_xy (uint16_t ui16DevType, uint16_t ui16Id, const S_Color_XY *psXY);
extern int set_dev_state_colormode (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8ColorMode);
extern int set_dev_state_alert (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Alert);
extern int set_dev_state_effect (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Effect);
extern int set_dev_state_level (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Level);
extern int set_dev_state_count (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Count);
extern int set_dev_state_temp (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Temp);
extern int set_dev_state_hum (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Hum);
extern int set_dev_state_consumption (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Consumption);
extern int set_dev_state_warn_mode (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8WarnMode);
extern int set_dev_state_duration (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Duration);
extern int set_dev_state_warn_cfg (uint16_t ui16DevType, uint16_t ui16Id, const S_WarnCfg *psWarnCfg);
extern int set_dev_state_action (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Action);
extern int set_dev_state_power (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Power);
extern int set_dev_state_play (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Play);
extern int set_dev_state_volume (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Volume);
extern int set_dev_state_url_id (uint16_t ui16DevType, uint16_t ui16Id, const uint32_t *pui32UrlId);

//--------------------------------------------------------------
extern int get_dev_total (void);
extern int get_dev_num_by_type (uint16_t ui16DevType);
extern int get_dev_idx_list (uint32_t *pui32Buf, uint16_t ui16Capacity);
extern int get_dev_id_list_by_type (uint16_t ui16DevType, uint16_t *pui16IdList, uint16_t ui16Capacity);
extern int assign_dev_id_by_type (uint16_t ui16DevType);
extern int swap_dev_id_by_type (uint16_t ui16DevType, uint16_t ui16DestId, uint16_t ui16SrcId);

extern int find_devs_by_modelid (const S_AttrModelId *psModelId, S_DevIdx *psDevIdx, uint16_t ui16Capacity);
extern int find_devs_by_nwkaddr (const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint16_t ui16Capacity);
extern int search_dev_by_type_nwkaddr_ep (uint16_t ui16DevType, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp);
extern int search_dev_by_nwkaddr_ep (const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, uint16_t *pui16DevType);
extern int rebind_dev (uint16_t ui16DevType, uint16_t ui16Id);
extern int check_confilct_zb_nwkaddr (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint16_t ui16Capacity);
extern int deal_dev_at_result (uint16_t ui16DevType, uint16_t ui16Id, int iResult);
extern int add_dev (uint16_t ui16DevType, const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, const U_DevExtInfo *puExtInfo);
extern int del_dev (uint16_t ui16DevType, uint16_t ui16Id);
//--------------------------------------------------------------
extern int search_dev_by_nwkaddr_ep (const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, uint16_t *pui16DevType);
extern int update_devs_nwkaddr_by_type_modelid (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint16_t ui16Capacity);
extern int del_devs_data_by_modelid (const S_AttrModelId *psModelId, S_DevIdx *psDevIdx, uint16_t ui16Capacity);
//--------------------------------------------------------------



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_dev_if_H__".
