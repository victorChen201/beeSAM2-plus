//#define	__M_FILE_NAME_light_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_light_H__
#define	__M_FILE_NAME_light_H__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#include "common.h"
#include "data_common.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_LIGHT_COLOR_MODE_MC		0		//单色
#define MUI_LIGHT_COLOR_MODE_HS		1
#define MUI_LIGHT_COLOR_MODE_XY		2
#define MUI_LIGHT_COLOR_MODE_CT		3

#define MUI_LIGHT_STATE_CT_MIN		153
#define MUI_LIGHT_STATE_CT_DFT		300
#define MUI_LIGHT_STATE_CT_MAX		500

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int light_default_state (S_LightState *psState);
extern int init_light_ram_data (uint16_t ui16Id, S_LightStateRam *psState);
//--------------------------------------------------------------
extern int read_light_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr);
extern int read_light_attr_type (uint16_t ui16Id, S_AttrType *psType);
extern int read_light_attr_name (uint16_t ui16Id, S_AttrName *psName);
extern int read_light_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr);
extern int read_light_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId);
extern int read_light_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer);
extern int read_light_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt);
extern int read_light_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt);
//................................................
extern int read_light_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr);
extern int read_light_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr);
extern int read_light_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp);
//................................................
extern int read_light_ext_attr (uint16_t ui16Id, S_LightExtAttr *psExtAttr);
extern int read_light_ext_attr_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode);
//................................................
extern int read_light_state (uint16_t ui16Id, S_LightState *psState);
extern int read_light_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable);
extern int read_light_state_on (uint16_t ui16Id, uint8_t *pui8On);
extern int read_light_state_bri (uint16_t ui16Id, uint8_t *pui8Bri);
extern int read_light_state_sat (uint16_t ui16Id, uint8_t *pui8Sat);
extern int read_light_state_hue (uint16_t ui16Id, uint16_t *pui16Hue);
extern int read_light_state_ct (uint16_t ui16Id, uint16_t *pui16Ct);
extern int read_light_state_xy (uint16_t ui16Id, S_Color_XY *psXY);
extern int read_light_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode);
extern int read_light_state_alert (uint16_t ui16Id, uint8_t *pui8Alert);
extern int read_light_state_effect (uint16_t ui16Id, uint8_t *pui8Effect);
extern int read_light_state_level (uint16_t ui16Id, uint16_t *pui16Level);
extern int read_light_state_count (uint16_t ui16Id, uint16_t *pui16Count);
extern int read_light_state_temp (uint16_t ui16Id, uint16_t *pui16Temp);
extern int read_light_state_hum (uint16_t ui16Id, uint16_t *pui16Hum);
extern int read_light_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption);
extern int read_light_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode);
extern int read_light_state_duration (uint16_t ui16Id, uint16_t *pui16Duration);
extern int read_light_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg);
extern int read_light_state_action (uint16_t ui16Id, uint8_t *pui8Action);
extern int read_light_state_power (uint16_t ui16Id, uint8_t *pui8Power);
//................................................
extern int read_light_misc (uint16_t ui16Id, S_DevMiscState *psMisc);
extern int read_light_misc_new_dev (uint16_t ui16Id, uint8_t *pui8NewDev);
extern int read_light_misc_not_added (uint16_t ui16Id, uint8_t *pui8NotAdded);
extern int read_light_misc_at_err_tick (uint16_t ui16Id, uint8_t *pui8AtErrTick);
//--------------------------------------------------------------
extern int write_light_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr);
extern int write_light_attr_type (uint16_t ui16Id, const S_AttrType *psType);
extern int write_light_attr_name (uint16_t ui16Id, const S_AttrName *psName);
extern int write_light_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr);
extern int write_light_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId);
extern int write_light_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer);
extern int write_light_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt);
extern int write_light_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt);
//................................................
extern int write_light_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr);
extern int write_light_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr);
extern int write_light_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp);
//................................................
extern int write_light_ext_attr (uint16_t ui16Id, const S_LightExtAttr *psExtAttr);
extern int write_light_ext_attr_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode);
//................................................
extern int write_light_state (uint16_t ui16Id, const S_LightState *psState);
extern int write_light_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable);
extern int write_light_state_on (uint16_t ui16Id, const uint8_t *pui8On);
extern int write_light_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri);
extern int write_light_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat);
extern int write_light_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue);
extern int write_light_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct);
extern int write_light_state_xy (uint16_t ui16Id, const S_Color_XY *psXY);
extern int write_light_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode);
extern int write_light_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert);
extern int write_light_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect);
extern int write_light_state_level (uint16_t ui16Id, const uint16_t *pui16Level);
extern int write_light_state_count (uint16_t ui16Id, const uint16_t *pui16Count);
extern int write_light_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp);
extern int write_light_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum);
extern int write_light_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption);
extern int write_light_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode);
extern int write_light_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration);
extern int write_light_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg);
extern int write_light_state_action (uint16_t ui16Id, const uint8_t *pui8Action);
extern int write_light_state_power (uint16_t ui16Id, const uint8_t *pui8Power);
//................................................
extern int write_light_misc (uint16_t ui16Id, const S_DevMiscState *psMisc);
extern int write_light_misc_new_dev (uint16_t ui16Id, const uint8_t *pui8NewDev);
extern int write_light_misc_not_added (uint16_t ui16Id, const uint8_t *pui8NotAdded);
extern int write_light_misc_at_err_tick (uint16_t ui16Id, const uint8_t *pui8AtErrTick);
//--------------------------------------------------------------
extern int get_light_state (uint16_t ui16Id, S_LightState *psState);
extern int get_light_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable);
extern int get_light_state_on (uint16_t ui16Id, uint8_t *pui8On);
extern int get_light_state_bri (uint16_t ui16Id, uint8_t *pui8Bri);
extern int get_light_state_sat (uint16_t ui16Id, uint8_t *pui8Sat);
extern int get_light_state_hue (uint16_t ui16Id, uint16_t *pui16Hue);
extern int get_light_state_ct (uint16_t ui16Id, uint16_t *pui16Ct);
extern int get_light_state_xy (uint16_t ui16Id, S_Color_XY *psXY);
extern int get_light_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode);
extern int get_light_state_alert (uint16_t ui16Id, uint8_t *pui8Alert);
extern int get_light_state_effect (uint16_t ui16Id, uint8_t *pui8Effect);
extern int get_light_state_level (uint16_t ui16Id, uint16_t *pui16Level);
extern int get_light_state_count (uint16_t ui16Id, uint16_t *pui16Count);
extern int get_light_state_temp (uint16_t ui16Id, uint16_t *pui16Temp);
extern int get_light_state_hum (uint16_t ui16Id, uint16_t *pui16Hum);
extern int get_light_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption);
extern int get_light_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode);
extern int get_light_state_duration (uint16_t ui16Id, uint16_t *pui16Duration);
extern int get_light_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg);
extern int get_light_state_action (uint16_t ui16Id, uint8_t *pui8Action);
extern int get_light_state_power (uint16_t ui16Id, uint8_t *pui8Power);
//--------------------------------------------------------------
extern int set_light_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr);
extern int set_light_attr_type (uint16_t ui16Id, const S_AttrType *psType);
extern int set_light_attr_name (uint16_t ui16Id, const S_AttrName *psName);
extern int set_light_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr);
extern int set_light_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId);
extern int set_light_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer);
extern int set_light_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt);
extern int set_light_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt);
//................................................
extern int set_light_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr);
extern int set_light_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr);
extern int set_light_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp);
//................................................
extern int set_light_ext_attr (uint16_t ui16Id, const S_LightExtAttr *psExtAttr);
extern int set_light_ext_attr_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode);
//................................................
extern int set_light_state (uint16_t ui16Id, const S_LightState *psState);
extern int set_light_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable);
extern int set_light_state_on (uint16_t ui16Id, const uint8_t *pui8On);
extern int set_light_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri);
extern int set_light_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat);
extern int set_light_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue);
extern int set_light_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct);
extern int set_light_state_xy (uint16_t ui16Id, const S_Color_XY *psXY);
extern int set_light_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode);
extern int set_light_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert);
extern int set_light_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect);
extern int set_light_state_level (uint16_t ui16Id, const uint16_t *pui16Level);
extern int set_light_state_count (uint16_t ui16Id, const uint16_t *pui16Count);
extern int set_light_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp);
extern int set_light_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum);
extern int set_light_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption);
extern int set_light_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode);
extern int set_light_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration);
extern int set_light_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg);
extern int set_light_state_action (uint16_t ui16Id, const uint8_t *pui8Action);
extern int set_light_state_power (uint16_t ui16Id, const uint8_t *pui8Power);
//--------------------------------------------------------------
extern int update_light_state_on (uint16_t ui16Id, const uint8_t *pui8On);
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int rebind_light (uint16_t ui16Id);
extern int add_light (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, const S_LightExtInfo *psExtInfo);
extern int del_light (uint16_t ui16Id);
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

#endif	//end of "#ifndef __M_FILE_NAME_light_H__".
