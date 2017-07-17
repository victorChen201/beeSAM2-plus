#include "at_port.h"
#include "dev_data.h"
#include "dev_if.h"
#include "api_deal.h"
#include "zigbee_manage.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define DEV_IF_DBG

#ifndef DEV_IF_PUTC
#ifdef DEV_IF_DBG
#define DEV_IF_PUTC(Char)			PUTC(Char)
#else
#define DEV_IF_PUTC(Char)
#endif
#endif

#ifndef DEV_IF_PUTS
#ifdef DEV_IF_DBG
#define DEV_IF_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define DEV_IF_PUTS(Str, Len)
#endif
#endif

#ifndef DEV_IF_PUTASCC
#ifdef DEV_IF_DBG
#define DEV_IF_PUTASCC(Char)			PUTASCC(Char)
#else
#define DEV_IF_PUTASCC(Char)
#endif
#endif

#ifndef DEV_IF_PUTASCS
#ifdef DEV_IF_DBG
#define DEV_IF_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define DEV_IF_PUTASCS(Str, Len)
#endif
#endif

#ifndef DEV_IF_PRINT
#ifdef DEV_IF_DBG
#define DEV_IF_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define DEV_IF_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef DEV_IF_DYN_PUTC
#ifdef DEV_IF_DBG
#define DEV_IF_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_DEV_IF|Dbg), Char)
#else
#define DEV_IF_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef DEV_IF_DYN_PUTS
#ifdef DEV_IF_DBG
#define DEV_IF_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_DEV_IF|Dbg), Str, Len)
#else
#define DEV_IF_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef DEV_IF_DYN_PUTASCC
#ifdef DEV_IF_DBG
#define DEV_IF_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_DEV_IF|Dbg), Char)
#else
#define DEV_IF_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef DEV_IF_DYN_PUTASCS
#ifdef DEV_IF_DBG
#define DEV_IF_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_DEV_IF|Dbg), Str, Len)
#else
#define DEV_IF_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef DEV_IF_DYN_PRINT
#ifdef DEV_IF_DBG
#define DEV_IF_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_DEV_IF|Dbg), Fmt, ##Arg)
#else
#define DEV_IF_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

#define MUI_REMOTE_REPORT_DEL_DEV		512
#define MUI_REMOTE_REPORT_ATTR_LEN		1024
#define MUI_REMOTE_REPORT_STATE_LEN		1024
#define MUI_REMOTE_REPORT_ALL_LEN		2048

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int init_dev_ram_data (uint16_t ui16DevType, uint16_t ui16Id, U_DevStateSave *puState)
{
	ASSERT(NULL != puState);
	UNUSED(ui16Id);

	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return init_light_ram_data(ui16Id, &puState->light_state);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return init_occ_sensor_ram_data(ui16Id, &puState->occ_sensor_state);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return init_bin_sensor_ram_data(ui16Id, &puState->bin_sensor_state);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return init_temp_sensor_ram_data(ui16Id, &puState->temp_sensor_state);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return init_hum_sensor_ram_data(ui16Id, &puState->hum_sensor_state);
	case MUI_DATA_TYPE_BIN_CTRL:
		return init_bin_ctrl_ram_data(ui16Id, &puState->bin_ctrl_state);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return init_level_ctrl_ram_data(ui16Id, &puState->level_ctrl_state);
	case MUI_DATA_TYPE_WARN_DEV:
		return init_warn_dev_ram_data(ui16Id, &puState->warn_dev_state);
	case MUI_DATA_TYPE_PLUG:
		return init_plug_ram_data(ui16Id, &puState->plug_state);
	case MUI_DATA_TYPE_SWITCH:
		return init_switch_ram_data(ui16Id, &puState->switch_state);
	case MUI_DATA_TYPE_SHADE:
		return init_shade_ram_data(ui16Id, &puState->shade_state);
	case MUI_DATA_TYPE_SONOS:
		return init_sonos_ram_data(ui16Id, &puState->sonos_state);
	case MUI_DATA_TYPE_PANEL:
		return init_panel_ram_data(ui16Id, &puState->sonos_state);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}

//--------------------------------------------------------------------------------------------------
int read_dev_attr (uint16_t ui16DevType, uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_attr(ui16Id, psApiAttr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_attr_type (uint16_t ui16DevType, uint16_t ui16Id, S_AttrType *psType)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_attr_type(ui16Id, psType);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_attr_name (uint16_t ui16DevType, uint16_t ui16Id, S_AttrName *psName)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_attr_name(ui16Id, psName);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_attr_descr (uint16_t ui16DevType, uint16_t ui16Id, S_AttrDescr *psDescr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_attr_descr(ui16Id, psDescr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_attr_modelid (uint16_t ui16DevType, uint16_t ui16Id, S_AttrModelId *psModelId)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_attr_modelid(ui16Id, psModelId);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_attr_swver (uint16_t ui16DevType, uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_attr_swver(ui16Id, psSwVer);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_attr_changerpt (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_attr_changerpt(ui16Id, pui8ChangeRpt);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_attr_autorpt (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_attr_autorpt(ui16Id, pui16AutoRpt);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int read_dev_zb_attr (uint16_t ui16DevType, uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_zb_attr(ui16Id, psZbAttr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_zb_attr_nwkaddr (uint16_t ui16DevType, uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_zb_attr_ep (uint16_t ui16DevType, uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_zb_attr_ep(ui16Id, psEp);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int read_dev_ext_attr (uint16_t ui16DevType, uint16_t ui16Id, U_DevExtAttr *puExtAttr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_ext_attr(ui16Id, &puExtAttr->light_ext_attr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_ext_attr(ui16Id, &puExtAttr->occ_sensor_ext_attr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_ext_attr(ui16Id, &puExtAttr->bin_sensor_ext_attr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_ext_attr(ui16Id, &puExtAttr->temp_sensor_ext_attr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_ext_attr(ui16Id, &puExtAttr->hum_sensor_ext_attr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_ext_attr(ui16Id, &puExtAttr->bin_ctrl_ext_attr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_ext_attr(ui16Id, &puExtAttr->level_ctrl_ext_attr);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_ext_attr(ui16Id, &puExtAttr->warn_dev_ext_attr);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_ext_attr(ui16Id, &puExtAttr->plug_ext_attr);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_ext_attr(ui16Id, &puExtAttr->switch_ext_attr);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_ext_attr(ui16Id, &puExtAttr->shade_ext_attr);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_ext_attr(ui16Id, &puExtAttr->sonos_ext_attr);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_ext_attr(ui16Id, &puExtAttr->sonos_ext_attr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int read_dev_state (uint16_t ui16DevType, uint16_t ui16Id, U_DevState *puState)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state(ui16Id, &puState->light_state);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_state(ui16Id, &puState->occ_sensor_state);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_state(ui16Id, &puState->bin_sensor_state);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_state(ui16Id, &puState->temp_sensor_state);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_state(ui16Id, &puState->hum_sensor_state);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_state(ui16Id, &puState->bin_ctrl_state);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_state(ui16Id, &puState->level_ctrl_state);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_state(ui16Id, &puState->warn_dev_state);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_state(ui16Id, &puState->plug_state);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_state(ui16Id, &puState->switch_state);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_state(ui16Id, &puState->shade_state);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_state(ui16Id, &puState->sonos_state);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_state(ui16Id, &puState->sonos_state);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_reachable (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Reachable)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_state_reachable(ui16Id, pui8Reachable);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_on (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8On)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return read_plug_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return read_panel_state_on(ui16Id, pui8On);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_bri (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Bri)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_bri(ui16Id, pui8Bri);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_sat (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Sat)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_sat(ui16Id, pui8Sat);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_hue (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Hue)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_hue(ui16Id, pui16Hue);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_ct (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Ct)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_ct(ui16Id, pui16Ct);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_xy (uint16_t ui16DevType, uint16_t ui16Id, S_Color_XY *psXY)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_xy(ui16Id, psXY);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_colormode (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_colormode(ui16Id, pui8ColorMode);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_alert (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Alert)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_alert(ui16Id, pui8Alert);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_effect (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Effect)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_state_effect(ui16Id, pui8Effect);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_level (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Level)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return read_plug_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return read_panel_state_level(ui16Id, pui16Level);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_count (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Count)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
		return read_plug_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_temp (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Temp)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_state_temp(ui16Id, pui16Temp);
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_hum (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Hum)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_state_hum(ui16Id, pui16Hum);
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_consumption (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Consumption)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return read_plug_state_consumption(ui16Id, pui16Consumption);
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_warn_mode (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_state_warn_mode(ui16Id, pui8WarnMode);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_duration (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Duration)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_state_duration(ui16Id, pui16Duration);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_warn_cfg (uint16_t ui16DevType, uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_state_warn_cfg(ui16Id, psWarnCfg);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_action (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Action)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SHADE:
		return read_shade_state_action(ui16Id, pui8Action);
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_power (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Power)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_play (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Play)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_state_play(ui16Id, pui8Play);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_volume (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Volume)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_state_volume(ui16Id, pui16Volume);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_state_url_id (uint16_t ui16DevType, uint16_t ui16Id, uint32_t *pui32UrlId)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_state_url_id(ui16Id, pui32UrlId);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int read_dev_misc (uint16_t ui16DevType, uint16_t ui16Id, S_DevMiscState *psMisc)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_misc(ui16Id, psMisc);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int read_dev_misc_at_err_tick (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *psMisc)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return read_light_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return read_occ_sensor_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return read_bin_sensor_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return read_temp_sensor_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return read_hum_sensor_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_BIN_CTRL:
		return read_bin_ctrl_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return read_level_ctrl_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_WARN_DEV:
		return read_warn_dev_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_PLUG:
		return read_plug_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_SWITCH:
		return read_switch_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_SHADE:
		return read_shade_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_SONOS:
		return read_sonos_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_PANEL:
		return read_panel_misc_at_err_tick(ui16Id, psMisc);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}

//--------------------------------------------------------------------------------------------------
int write_dev_attr (uint16_t ui16DevType, uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_attr(ui16Id, psApiAttr);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_attr(ui16Id, psApiAttr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_attr_type (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrType *psType)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_attr_type(ui16Id, psType);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_attr_type(ui16Id, psType);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_attr_name (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrName *psName)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_attr_name(ui16Id, psName);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_attr_name(ui16Id, psName);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_attr_descr (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_attr_descr(ui16Id, psDescr);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_attr_descr(ui16Id, psDescr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_attr_modelid (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_attr_modelid(ui16Id, psModelId);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_attr_modelid(ui16Id, psModelId);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_attr_swver (uint16_t ui16DevType, uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_attr_swver(ui16Id, psSwVer);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_attr_swver(ui16Id, psSwVer);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_attr_changerpt (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_attr_changerpt(ui16Id, pui8ChangeRpt);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_attr_changerpt(ui16Id, pui8ChangeRpt);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_attr_autorpt (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_attr_autorpt(ui16Id, pui16AutoRpt);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_attr_autorpt(ui16Id, pui16AutoRpt);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int write_dev_zb_attr (uint16_t ui16DevType, uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_zb_attr(ui16Id, psZbAttr);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_zb_attr(ui16Id, psZbAttr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_zb_attr_nwkaddr (uint16_t ui16DevType, uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_zb_attr_nwkaddr(ui16Id, psNwkAddr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_zb_attr_ep (uint16_t ui16DevType, uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_zb_attr_ep(ui16Id, psEp);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_zb_attr_ep(ui16Id, psEp);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int write_dev_ext_attr (uint16_t ui16DevType, uint16_t ui16Id, const U_DevExtAttr *puExtAttr)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_ext_attr(ui16Id, &puExtAttr->light_ext_attr);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_ext_attr(ui16Id, &puExtAttr->occ_sensor_ext_attr);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_ext_attr(ui16Id, &puExtAttr->bin_sensor_ext_attr);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_ext_attr(ui16Id, &puExtAttr->temp_sensor_ext_attr);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_ext_attr(ui16Id, &puExtAttr->hum_sensor_ext_attr);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_ext_attr(ui16Id, &puExtAttr->bin_ctrl_ext_attr);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_ext_attr(ui16Id, &puExtAttr->level_ctrl_ext_attr);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_ext_attr(ui16Id, &puExtAttr->warn_dev_ext_attr);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_ext_attr(ui16Id, &puExtAttr->plug_ext_attr);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_ext_attr(ui16Id, &puExtAttr->switch_ext_attr);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_ext_attr(ui16Id, &puExtAttr->shade_ext_attr);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_ext_attr(ui16Id, &puExtAttr->sonos_ext_attr);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_ext_attr(ui16Id, &puExtAttr->sonos_ext_attr);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int write_dev_state (uint16_t ui16DevType, uint16_t ui16Id, const U_DevState *puState)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state(ui16Id, &puState->light_state);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_state(ui16Id, &puState->occ_sensor_state);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_state(ui16Id, &puState->bin_sensor_state);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_state(ui16Id, &puState->temp_sensor_state);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_state(ui16Id, &puState->hum_sensor_state);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_state(ui16Id, &puState->bin_ctrl_state);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_state(ui16Id, &puState->level_ctrl_state);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_state(ui16Id, &puState->warn_dev_state);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_state(ui16Id, &puState->plug_state);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_state(ui16Id, &puState->switch_state);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_state(ui16Id, &puState->shade_state);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_state(ui16Id, &puState->sonos_state);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_state(ui16Id, &puState->sonos_state);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_reachable (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_state_reachable(ui16Id, pui8Reachable);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_on (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8On)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return write_plug_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return write_panel_state_on(ui16Id, pui8On);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_bri (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Bri)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_bri(ui16Id, pui8Bri);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_sat (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Sat)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_sat(ui16Id, pui8Sat);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_hue (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Hue)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_hue(ui16Id, pui16Hue);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_ct (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Ct)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_ct(ui16Id, pui16Ct);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_xy (uint16_t ui16DevType, uint16_t ui16Id, const S_Color_XY *psXY)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_xy(ui16Id, psXY);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_colormode (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_colormode(ui16Id, pui8ColorMode);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_alert (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Alert)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_alert(ui16Id, pui8Alert);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_effect (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Effect)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_state_effect(ui16Id, pui8Effect);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_level (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Level)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return write_plug_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return write_panel_state_level(ui16Id, pui16Level);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_count (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Count)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
		return write_plug_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_temp (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Temp)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_state_temp(ui16Id, pui16Temp);
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_hum (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Hum)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_state_hum(ui16Id, pui16Hum);
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_consumption (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return write_plug_state_consumption(ui16Id, pui16Consumption);
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_warn_mode (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_state_warn_mode(ui16Id, pui8WarnMode);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_duration (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Duration)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_state_duration(ui16Id, pui16Duration);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_warn_cfg (uint16_t ui16DevType, uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_state_warn_cfg(ui16Id, psWarnCfg);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_action (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Action)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SHADE:
		return write_shade_state_action(ui16Id, pui8Action);
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_power (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Power)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_play (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Play)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_state_play(ui16Id, pui8Play);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_volume (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Volume)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_state_volume(ui16Id, pui16Volume);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int write_dev_state_url_id (uint16_t ui16DevType, uint16_t ui16Id, const uint32_t *pui32UrlId)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_state_url_id(ui16Id, pui32UrlId);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int write_dev_misc (uint16_t ui16DevType, uint16_t ui16Id, const S_DevMiscState *psMisc)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_misc(ui16Id, psMisc);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_misc(ui16Id, psMisc);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//.............................................................................
int write_dev_misc_at_err_tick (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *psMisc)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return write_light_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return write_occ_sensor_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return write_bin_sensor_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return write_temp_sensor_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return write_hum_sensor_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_BIN_CTRL:
		return write_bin_ctrl_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return write_level_ctrl_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_WARN_DEV:
		return write_warn_dev_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_PLUG:
		return write_plug_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_SWITCH:
		return write_switch_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_SHADE:
		return write_shade_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_SONOS:
		return write_sonos_misc_at_err_tick(ui16Id, psMisc);
	case MUI_DATA_TYPE_PANEL:
		return write_panel_misc_at_err_tick(ui16Id, psMisc);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}

//--------------------------------------------------------------------------------------------------
int get_dev_state (uint16_t ui16DevType, uint16_t ui16Id, U_DevState *puState)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state(ui16Id, &puState->light_state);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return get_occ_sensor_state(ui16Id, &puState->occ_sensor_state);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return get_bin_sensor_state(ui16Id, &puState->bin_sensor_state);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return get_temp_sensor_state(ui16Id, &puState->temp_sensor_state);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return get_hum_sensor_state(ui16Id, &puState->hum_sensor_state);
	case MUI_DATA_TYPE_BIN_CTRL:
		return get_bin_ctrl_state(ui16Id, &puState->bin_ctrl_state);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return get_level_ctrl_state(ui16Id, &puState->level_ctrl_state);
	case MUI_DATA_TYPE_WARN_DEV:
		return get_warn_dev_state(ui16Id, &puState->warn_dev_state);
	case MUI_DATA_TYPE_PLUG:
		return get_plug_state(ui16Id, &puState->plug_state);
	case MUI_DATA_TYPE_SWITCH:
		return get_switch_state(ui16Id, &puState->switch_state);
	case MUI_DATA_TYPE_SHADE:
		return get_shade_state(ui16Id, &puState->shade_state);
	case MUI_DATA_TYPE_SONOS:
		return get_sonos_state(ui16Id, &puState->sonos_state);
	case MUI_DATA_TYPE_PANEL:
		return get_panel_state(ui16Id, &puState->sonos_state);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_reachable (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Reachable)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return get_occ_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return get_bin_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return get_temp_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return get_hum_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_BIN_CTRL:
		return get_bin_ctrl_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return get_level_ctrl_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_WARN_DEV:
		return get_warn_dev_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_PLUG:
		return get_plug_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SWITCH:
		return get_switch_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SHADE:
		return get_shade_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SONOS:
		return get_sonos_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_PANEL:
		return get_panel_state_reachable(ui16Id, pui8Reachable);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_on (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8On)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return get_occ_sensor_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return get_bin_sensor_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_BIN_CTRL:
		return get_bin_ctrl_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return get_plug_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_SWITCH:
		return get_switch_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return get_panel_state_on(ui16Id, pui8On);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_bri (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Bri)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_bri(ui16Id, pui8Bri);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_sat (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Sat)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_sat(ui16Id, pui8Sat);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_hue (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Hue)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_hue(ui16Id, pui16Hue);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_ct (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Ct)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_ct(ui16Id, pui16Ct);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_xy (uint16_t ui16DevType, uint16_t ui16Id, S_Color_XY *psXY)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_xy(ui16Id, psXY);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_colormode (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_colormode(ui16Id, pui8ColorMode);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_alert (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Alert)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_alert(ui16Id, pui8Alert);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_effect (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Effect)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return get_light_state_effect(ui16Id, pui8Effect);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_level (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Level)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return get_level_ctrl_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return get_plug_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SWITCH:
		return get_switch_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SHADE:
		return get_shade_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return get_panel_state_level(ui16Id, pui16Level);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_count (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Count)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_OCC_SENSOR:
		return get_occ_sensor_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return get_bin_sensor_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_BIN_CTRL:
		return get_bin_ctrl_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
		return get_plug_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_SWITCH:
		return get_switch_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_temp (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Temp)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return get_temp_sensor_state_temp(ui16Id, pui16Temp);
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_hum (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Hum)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_HUM_SENSOR:
		return get_hum_sensor_state_hum(ui16Id, pui16Hum);
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_consumption (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Consumption)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return get_plug_state_consumption(ui16Id, pui16Consumption);
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_warn_mode (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return get_warn_dev_state_warn_mode(ui16Id, pui8WarnMode);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_duration (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Duration)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return get_warn_dev_state_duration(ui16Id, pui16Duration);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_warn_cfg (uint16_t ui16DevType, uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return get_warn_dev_state_warn_cfg(ui16Id, psWarnCfg);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_action (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Action)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SHADE:
		return get_shade_state_action(ui16Id, pui8Action);
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_power (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Power)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_OCC_SENSOR:
		return get_occ_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return get_bin_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return get_temp_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return get_hum_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_play (uint16_t ui16DevType, uint16_t ui16Id, uint8_t *pui8Play)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return get_sonos_state_play(ui16Id, pui8Play);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_volume (uint16_t ui16DevType, uint16_t ui16Id, uint16_t *pui16Volume)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return get_sonos_state_volume(ui16Id, pui16Volume);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int get_dev_state_url_id (uint16_t ui16DevType, uint16_t ui16Id, uint32_t *pui32UrlId)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return get_sonos_state_url_id(ui16Id, pui32UrlId);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}

//--------------------------------------------------------------------------------------------------
int set_dev_state (uint16_t ui16DevType, uint16_t ui16Id, const U_DevState *puState)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state(ui16Id, &puState->light_state);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return set_occ_sensor_state(ui16Id, &puState->occ_sensor_state);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return set_bin_sensor_state(ui16Id, &puState->bin_sensor_state);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return set_temp_sensor_state(ui16Id, &puState->temp_sensor_state);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return set_hum_sensor_state(ui16Id, &puState->hum_sensor_state);
	case MUI_DATA_TYPE_BIN_CTRL:
		return set_bin_ctrl_state(ui16Id, &puState->bin_ctrl_state);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return set_level_ctrl_state(ui16Id, &puState->level_ctrl_state);
	case MUI_DATA_TYPE_WARN_DEV:
		return set_warn_dev_state(ui16Id, &puState->warn_dev_state);
	case MUI_DATA_TYPE_PLUG:
		return set_plug_state(ui16Id, &puState->plug_state);
	case MUI_DATA_TYPE_SWITCH:
		return set_switch_state(ui16Id, &puState->switch_state);
	case MUI_DATA_TYPE_SHADE:
		return set_shade_state(ui16Id, &puState->shade_state);
	case MUI_DATA_TYPE_SONOS:
		return set_sonos_state(ui16Id, &puState->sonos_state);
	case MUI_DATA_TYPE_PANEL:
		return set_panel_state(ui16Id, &puState->sonos_state);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_reachable (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return set_occ_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return set_bin_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return set_temp_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return set_hum_sensor_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_BIN_CTRL:
		return set_bin_ctrl_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return set_level_ctrl_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_WARN_DEV:
		return set_warn_dev_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_PLUG:
		return set_plug_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SWITCH:
		return set_switch_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SHADE:
		return set_shade_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_SONOS:
		return set_sonos_state_reachable(ui16Id, pui8Reachable);
	case MUI_DATA_TYPE_PANEL:
		return set_panel_state_reachable(ui16Id, pui8Reachable);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_on (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8On)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_BIN_CTRL:
		return set_bin_ctrl_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return set_plug_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_SWITCH:
		return set_switch_state_on(ui16Id, pui8On);
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return set_panel_state_on(ui16Id, pui8On);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_bri (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Bri)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_bri(ui16Id, pui8Bri);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_sat (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Sat)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_sat(ui16Id, pui8Sat);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_hue (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Hue)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_hue(ui16Id, pui16Hue);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_ct (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Ct)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_ct(ui16Id, pui16Ct);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_xy (uint16_t ui16DevType, uint16_t ui16Id, const S_Color_XY *psXY)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_xy(ui16Id, psXY);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_colormode (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_colormode(ui16Id, pui8ColorMode);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_alert (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Alert)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_alert(ui16Id, pui8Alert);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_effect (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Effect)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return set_light_state_effect(ui16Id, pui8Effect);
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_level (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Level)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return set_level_ctrl_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return set_plug_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SWITCH:
		return set_switch_state_level(ui16Id, pui16Level);
	case MUI_DATA_TYPE_SHADE:
		//return set_shade_state_level(ui16Id, pui16Level);
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PANEL:
		return set_panel_state_level(ui16Id, pui16Level);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_count (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Count)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_OCC_SENSOR:
		return set_occ_sensor_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return set_bin_sensor_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_BIN_CTRL:
		return set_bin_ctrl_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
		return set_plug_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_SWITCH:
		return set_switch_state_count(ui16Id, pui16Count);
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_temp (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Temp)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return set_temp_sensor_state_temp(ui16Id, pui16Temp);
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_hum (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Hum)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_HUM_SENSOR:
		return set_hum_sensor_state_hum(ui16Id, pui16Hum);
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_consumption (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_PLUG:
		return set_plug_state_consumption(ui16Id, pui16Consumption);
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_warn_mode (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return set_warn_dev_state_warn_mode(ui16Id, pui8WarnMode);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_duration (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Duration)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return set_warn_dev_state_duration(ui16Id, pui16Duration);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_warn_cfg (uint16_t ui16DevType, uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_WARN_DEV:
		return set_warn_dev_state_warn_cfg(ui16Id, psWarnCfg);
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_action (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Action)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SHADE:
		return set_shade_state_action(ui16Id, pui8Action);
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_power (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Power)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_OCC_SENSOR:
		return set_occ_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return set_bin_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return set_temp_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return set_hum_sensor_state_power(ui16Id, pui8Power);
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
	case MUI_DATA_TYPE_SONOS:
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_play (uint16_t ui16DevType, uint16_t ui16Id, const uint8_t *pui8Play)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return set_sonos_state_play(ui16Id, pui8Play);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_volume (uint16_t ui16DevType, uint16_t ui16Id, const uint16_t *pui16Volume)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return set_sonos_state_volume(ui16Id, pui16Volume);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int set_dev_state_url_id (uint16_t ui16DevType, uint16_t ui16Id, const uint32_t *pui32UrlId)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
	case MUI_DATA_TYPE_OCC_SENSOR:
	case MUI_DATA_TYPE_BIN_SENSOR:
	case MUI_DATA_TYPE_TEMP_SENSOR:
	case MUI_DATA_TYPE_HUM_SENSOR:
	case MUI_DATA_TYPE_BIN_CTRL:
	case MUI_DATA_TYPE_LEVEL_CTRL:
	case MUI_DATA_TYPE_WARN_DEV:
	case MUI_DATA_TYPE_PLUG:
	case MUI_DATA_TYPE_SWITCH:
	case MUI_DATA_TYPE_SHADE:
		return DEV_IF_ERR_UNSUPPORT;
	case MUI_DATA_TYPE_SONOS:
		return set_sonos_state_url_id(ui16Id, pui32UrlId);
	case MUI_DATA_TYPE_PANEL:
		return DEV_IF_ERR_UNSUPPORT;
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}

//--------------------------------------------------------------------------------------------------
int get_dev_total (void)
{
	return dev_data_total_get();
}

int get_dev_num_by_type (uint16_t ui16DevType)
{
	return dev_data_num_get_by_type(ui16DevType);
}

int get_dev_idx_list (uint32_t *pui32Buf, uint16_t ui16Capacity)
{
	return dev_data_idx_list_get(pui32Buf, ui16Capacity);
}

int get_dev_id_list_by_type (uint16_t ui16DevType, uint16_t *pui16IdList, uint16_t ui16Capacity)
{
	return dev_data_id_list_get_by_type(ui16DevType, pui16IdList, ui16Capacity);
}

int assign_dev_id_by_type (uint16_t ui16DevType)
{
	return dev_data_assign_id_by_type(ui16DevType);
}

int swap_dev_id_by_type (uint16_t ui16DevType, uint16_t ui16DestId, uint16_t ui16SrcId)
{
	return dev_data_swap_id(ui16DevType, ui16DestId, ui16SrcId);
}

//--------------------------------------------------------------------------------------------------
//return number of devices that with same modelid
int find_devs_by_modelid (const S_AttrModelId *psModelId, S_DevIdx *psDevIdx, uint16_t ui16Capacity)
{
	S_AttrModelId sModelId;
	uint32_t aui32IdxList[MUI_LOGIC_DEV_NUM_MAX];
	uint16_t ui16Cnt = 0;
	int i, iNum;

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psDevIdx);
	ASSERT(0 < ui16Capacity);

	iNum = get_dev_idx_list(aui32IdxList, (sizeof(aui32IdxList)/sizeof(aui32IdxList[0])));
	if (iNum <= 0)
	{
		DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[find_devs_by_modelid]There are not any devices with modelid:%s.\r\n", psModelId->str);
		return 0;
	}
	for (i=0; i<iNum; i++)
	{
		if ((0<read_dev_attr_modelid(M_GET_TYPE_FROM_IDX(aui32IdxList[i]), M_GET_ID_FROM_IDX(aui32IdxList[i]), &sModelId)) &&
			(0==strncmp(sModelId.str, psModelId->str, sizeof(sModelId.str))))
		{
			psDevIdx[ui16Cnt].ui16Type = M_GET_TYPE_FROM_IDX(aui32IdxList[i]);
			psDevIdx[ui16Cnt].ui16Id   = M_GET_ID_FROM_IDX(aui32IdxList[i]);
			ui16Cnt++;
			if (ui16Cnt > ui16Capacity)
			{
				DEV_IF_DYN_PRINT(M_DBG_LEVEL_ERR, "[find_devs_by_modelid]Buffer capacity:%u is small than count:%u.\r\n", ui16Capacity, ui16Cnt);
				return DEV_IF_ERR_PARA_WRONG;
			}
		}
	}

	DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[find_devs_by_modelid]Have found %u devices with same modelid.\r\n", ui16Cnt);

	return (int)ui16Cnt;
}
//return number of devices that with same nwkaddr
int find_devs_by_nwkaddr (const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint16_t ui16Capacity)
{
	S_ZbAttrNwkAddr sNwkAddr;
	uint32_t aui32IdxList[MUI_LOGIC_DEV_NUM_MAX];
	uint16_t ui16Cnt = 0;
	int i, iNum;

	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psDevIdx);
	ASSERT(0 < ui16Capacity);

	iNum = get_dev_idx_list(aui32IdxList, (sizeof(aui32IdxList)/sizeof(aui32IdxList[0])));
	if (iNum <= 0)
	{
		DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[find_devs_by_nwkaddr]There are not any devices with nwkaddr:%s.\r\n", psNwkAddr->str);
		return 0;
	}
	for (i=0; i<iNum; i++)
	{
		if ((0<read_dev_zb_attr_nwkaddr(M_GET_TYPE_FROM_IDX(aui32IdxList[i]), M_GET_ID_FROM_IDX(aui32IdxList[i]), &sNwkAddr)) &&
			(0==strncmp(sNwkAddr.str, psNwkAddr->str, sizeof(sNwkAddr.str))))
		{
			psDevIdx[ui16Cnt].ui16Type = M_GET_TYPE_FROM_IDX(aui32IdxList[i]);
			psDevIdx[ui16Cnt].ui16Id   = M_GET_ID_FROM_IDX(aui32IdxList[i]);
			ui16Cnt++;
			if (ui16Cnt >= ui16Capacity)
			{
				return DEV_IF_ERR_PARA_WRONG;
			}
		}
	}

	DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[find_devs_by_nwkaddr]Have found %u devices with same modelid.\r\n", ui16Cnt);

	return (int)ui16Cnt;
}
//return exist device ID
int search_dev_by_type_nwkaddr_ep (uint16_t ui16DevType, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp)
{
	S_DevZbAttr sZbAttr;
	uint16_t aui6IdList[MUI_LOGIC_DEV_NUM_MAX];
	int i, iNum;

	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psEp);

	iNum = get_dev_id_list_by_type(ui16DevType, aui6IdList, (sizeof(aui6IdList)/sizeof(aui6IdList[0])));
	if (iNum <= 0)
	{
		DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[search_dev_by_type_nwkaddr_ep]There are not any devices with nwkaddr[%s]:ep[%s].\r\n", psNwkAddr->str, psEp->str);
		return DEV_IF_ERR_NO_FIND_NWK_ADDR;
	}
	for (i=0; i<iNum; i++)
	{
		if ((0<read_dev_zb_attr(ui16DevType, aui6IdList[i], &sZbAttr)) &&
			(0==strncmp(sZbAttr.nwkaddr.str, psNwkAddr->str, sizeof(sZbAttr.nwkaddr.str))) &&
			(0==strncmp(sZbAttr.ep.str, psEp->str, sizeof(sZbAttr.ep.str))))
		{
			return (int)aui6IdList[i];
		}
	}

	return DEV_IF_ERR_NO_FIND_NWK_ADDR;
}
//return exist device ID
int search_dev_by_nwkaddr_ep (const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, uint16_t *pui16DevType)
{
	int iRet;
	uint16_t ui16DevType;

	for (ui16DevType=MUI_DATA_TYPE_DEV_START; ui16DevType<=MUI_DATA_TYPE_DEV_END; ui16DevType++)
	{
		iRet = search_dev_by_type_nwkaddr_ep(ui16DevType, psNwkAddr, psEp);
		if (iRet > 0)
		{
			*pui16DevType = ui16DevType;
			return iRet;
		}
	}

	return DEV_IF_ERR_NO_FIND_NWK_ADDR;
}
//return number of devices that have been update.
int update_devs_nwkaddr_by_type_modelid (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint16_t ui16Capacity)
{
	S_ZbAttrNwkAddr sNwkAddr;
	uint8_t aui8Buf[MUI_REMOTE_REPORT_ALL_LEN];
	uint8_t ui8Reachable, ui8ChangeRpt, ui8ReportMode;
	int i, iRet, iNum, iCnt=0;

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psDevIdx);
	ASSERT(0 < ui16Capacity);

	iNum = find_devs_by_modelid(psModelId, psDevIdx, ui16Capacity);
	if (iNum <= 0)
	{
		DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_devs_nwkaddr_by_type_modelid]Call 'find_devs_by_modelid()' fail.\r\n");
		return iNum;
	}
	for (i=0; i<iNum; i++)
	{
		ui8ReportMode = 0;

		if ((0<read_dev_state_reachable(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &ui8Reachable)) &&
			(0==ui8Reachable))
		{
			ui8Reachable = 1;
			if (0 > write_dev_state_reachable(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &ui8Reachable))
			{
				DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_devs_nwkaddr_by_type_modelid]Call 'write_dev_state_reachable()' fail.\r\n");
			}
			else
			{
				ui8ReportMode += 2;
			}
		}
		if ((0<read_dev_zb_attr_nwkaddr(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &sNwkAddr)) &&
			(0!=strncmp(sNwkAddr.str, psNwkAddr->str, sizeof(sNwkAddr.str))))
		{
			if (0 > write_dev_zb_attr_nwkaddr(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, psNwkAddr))
			{
				DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_devs_nwkaddr_by_type_modelid]Call 'write_dev_zb_attr_nwkaddr()' fail.\r\n");
			}
			else
			{
				ui8ReportMode += 1;
				iCnt++;	//被更换nwkaddr设备的计数
			}
		}

		if (ui8ReportMode != 0)
		{
			iRet = read_dev_attr_changerpt(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &ui8ChangeRpt);
			if (iRet < 0)
			{
				DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_devs_nwkaddr_by_type_modelid]Call 'read_dev_attr_changerpt()' fail.\r\n");
				break;
			}

			if (ui8ChangeRpt != 0)
			{
				//上报
				send_dev_report(MUI_HTTP_METHOD_EX_MODIFY, psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);
			}
		}
	}

	DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_devs_nwkaddr_by_type_modelid]Have updated %d devices' short address.\r\n", iCnt);

	return iNum;
}
//return number of devices that have been deleted.
int del_devs_data_by_modelid (const S_AttrModelId *psModelId, S_DevIdx *psDevIdx, uint16_t ui16Capacity)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_DEL_DEV];
	uint16_t ui16Cnt, ui16AutoRpt;
	int iRet, iNum, i;

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psDevIdx);
	ASSERT(0 < ui16Capacity);

	iNum = find_devs_by_modelid(psModelId, psDevIdx, ui16Capacity);
	if (iNum <= 0)
	{
		DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[del_devs_data_by_modelid]Have not find any device.\r\n");
		return 0;
	}

	ui16Cnt = 0;
	ui16AutoRpt = 1;	//为防止读取该数据失败导致不会自动上报删除设备列表,默认设置为需要上报

	for (i=0; i<iNum; i++)
	{
		read_dev_attr_autorpt(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &ui16AutoRpt);
		if (0 > dev_data_del(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id))
		{
			DEV_IF_DYN_PRINT(M_DBG_LEVEL_ERR, "[del_devs_data_by_modelid] Call :dev_data_del failed.\r\n");
			continue;
		}

		//删除自动上报
		auto_report_dev_del(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id);

		//上报删除设备
		send_dev_report(MUI_HTTP_METHOD_EX_DEL, psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);

		ui16Cnt++;
	}

	return (uint16_t)ui16Cnt;
}
//--------------------------------------------------------------------------------------------------
int check_confilct_zb_nwkaddr (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint16_t ui16Capacity)
{
	S_AttrModelId sModelId;
	uint8_t aui8Buf[MUI_REMOTE_REPORT_ALL_LEN];
	uint8_t ui8Reachable, ui8ChangeRpt, ui8ReportMode;
	int i, iRet, iNum, iCnt=0;

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psDevIdx);
	ASSERT(0 < ui16Capacity);

	iNum = find_devs_by_nwkaddr(psNwkAddr, psDevIdx, ui16Capacity);
	if (iNum <= 0)
	{
		DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[check_confilct_zb_nwkaddr]Have not find any device.\r\n");
		return 0;
	}
	DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[check_confilct_zb_nwkaddr]Have found %d devices with same nwkaddr:%s.\r\n", iNum, psNwkAddr->str);
	for (i=0; i<iNum; i++)
	{
		ui8ReportMode = 0;

		if ((0<read_dev_state_reachable(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &ui8Reachable)) && (1==ui8Reachable))
		{
			if ((0<read_dev_attr_modelid(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &sModelId)) &&
				(0!=strncmp(sModelId.str, psModelId->str, sizeof(sModelId.str))))
			{
				ui8Reachable = 0;
				if (0 > write_dev_state_reachable(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &ui8Reachable))
				{
					DEV_IF_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[check_confilct_zb_nwkaddr]Call 'write_dev_state_reachable()' fail.\r\n");
				}
				//发送SCAN命令
				zigbee_print_queue_query(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, AT_CMD_SCAN_S"=%s\r\n", psModelId->str);

				iCnt++;
				ui8ReportMode += 1;
			}

			if (ui8ReportMode != 0)
			{
				iRet = read_dev_attr_changerpt(psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, &ui8ChangeRpt);
				if ((iRet>0) && (ui8ChangeRpt!=0))
				{
					//上报
					send_dev_report(MUI_HTTP_METHOD_EX_MODIFY, psDevIdx[i].ui16Type, psDevIdx[i].ui16Id, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);
				}
			}
		}
	}

	return iCnt;
}
int deal_dev_at_result (uint16_t ui16DevType, uint16_t ui16Id, int iResult)
{
	S_AttrModelId sModelId;
	uint8_t ui8Reachable, ui8AtErrTick;
	int iRet = 0;

	if (ui16DevType == MUI_DATA_TYPE_INVALID)
		return iRet;

	iRet = read_dev_misc_at_err_tick(ui16DevType, ui16Id, &ui8AtErrTick);
	if (iRet < 0)
	{
		return iRet;
	}
	if (iResult >= 0)
	{
		if (0 != ui8AtErrTick)
		{
			ui8AtErrTick = 0;
			iRet = write_dev_misc_at_err_tick(ui16DevType, ui16Id, &ui8AtErrTick);
			if (iRet < 0)
			{
				return iRet;
			}
			ui8Reachable = 1;
			iRet = write_dev_state_reachable(ui16DevType, ui16Id, &ui8Reachable);
			if (iRet < 0)
			{
				return iRet;
			}
		}
	}
	else
	{
		if (ui8AtErrTick < MUI_DEV_AT_ERR_CNT_MAX)
		{
			ui8AtErrTick++;
			iRet = write_dev_misc_at_err_tick(ui16DevType, ui16Id, &ui8AtErrTick);
			if (iRet < 0)
			{
				return iRet;
			}
		}
		if (ui8AtErrTick >= MUI_DEV_AT_ERR_SCAN_LIMIT)
		{
			ui8Reachable = 0;
			iRet = write_dev_state_reachable(ui16DevType, ui16Id, &ui8Reachable);
			if (iRet < 0)
			{
				return iRet;
			}
			iRet = read_dev_attr_modelid(ui16DevType, ui16Id, &sModelId);
			if (iRet < 0)
			{
				return iRet;
			}
			iRet = zigbee_print_queue_query(MUI_DATA_TYPE_INVALID, MUI_DEV_ID_INVALID, AT_CMD_SCAN_S"=%s\r\n", sModelId.str);
			if (iRet < 0)
			{
				return iRet;
			}
		}
	}

	return iRet;
}
//--------------------------------------------------------------------------------------------------
int rebind_dev (uint16_t ui16DevType, uint16_t ui16Id)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return rebind_light(ui16Id);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return rebind_occ_sensor(ui16Id);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return rebind_bin_sensor(ui16Id);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return rebind_temp_sensor(ui16Id);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return rebind_hum_sensor(ui16Id);
	case MUI_DATA_TYPE_BIN_CTRL:
		return rebind_bin_ctrl(ui16Id);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return rebind_level_ctrl(ui16Id);
	case MUI_DATA_TYPE_WARN_DEV:
		return rebind_warn_dev(ui16Id);
	case MUI_DATA_TYPE_PLUG:
		return rebind_plug(ui16Id);
	case MUI_DATA_TYPE_SWITCH:
		return rebind_switch(ui16Id);
	case MUI_DATA_TYPE_SHADE:
		return rebind_shade(ui16Id);
	case MUI_DATA_TYPE_SONOS:
		return rebind_sonos(ui16Id);
	case MUI_DATA_TYPE_PANEL:
		return rebind_panel(ui16Id);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int add_dev (uint16_t ui16DevType, const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, const U_DevExtInfo *puExtInfo)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return add_light(psModelId, psNwkAddr, psEp, &puExtInfo->light_ext_info);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return add_occ_sensor(psModelId, psNwkAddr, psEp, &puExtInfo->occ_sensor_ext_info);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return add_bin_sensor(psModelId, psNwkAddr, psEp, &puExtInfo->bin_sensor_ext_info);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return add_temp_sensor(psModelId, psNwkAddr, psEp, &puExtInfo->temp_sensor_ext_info);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return add_hum_sensor(psModelId, psNwkAddr, psEp, &puExtInfo->hum_sensor_ext_info);
	case MUI_DATA_TYPE_BIN_CTRL:
		return add_bin_ctrl(psModelId, psNwkAddr, psEp, &puExtInfo->bin_ctrl_ext_info);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return add_level_ctrl(psModelId, psNwkAddr, psEp, &puExtInfo->level_ctrl_ext_info);
	case MUI_DATA_TYPE_WARN_DEV:
		return add_warn_dev(psModelId, psNwkAddr, psEp, &puExtInfo->warn_dev_ext_info);
	case MUI_DATA_TYPE_PLUG:
		return add_plug(psModelId, psNwkAddr, psEp, &puExtInfo->plug_ext_info);
	case MUI_DATA_TYPE_SWITCH:
		return add_switch(psModelId, psNwkAddr, psEp, &puExtInfo->switch_ext_info);
	case MUI_DATA_TYPE_SHADE:
		return add_shade(psModelId, psNwkAddr, psEp, &puExtInfo->shade_ext_info);
	case MUI_DATA_TYPE_SONOS:
		return add_sonos(psModelId, psNwkAddr, psEp, &puExtInfo->sonos_ext_info);
	case MUI_DATA_TYPE_PANEL:
		return add_panel(psModelId, psNwkAddr, psEp, &puExtInfo->panel_ext_info);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
int del_dev (uint16_t ui16DevType, uint16_t ui16Id)
{
	switch (ui16DevType)
	{
	case MUI_DATA_TYPE_LIGHT:
		return del_light(ui16Id);
	case MUI_DATA_TYPE_OCC_SENSOR:
		return del_occ_sensor(ui16Id);
	case MUI_DATA_TYPE_BIN_SENSOR:
		return del_bin_sensor(ui16Id);
	case MUI_DATA_TYPE_TEMP_SENSOR:
		return del_temp_sensor(ui16Id);
	case MUI_DATA_TYPE_HUM_SENSOR:
		return del_hum_sensor(ui16Id);
	case MUI_DATA_TYPE_BIN_CTRL:
		return del_bin_ctrl(ui16Id);
	case MUI_DATA_TYPE_LEVEL_CTRL:
		return del_level_ctrl(ui16Id);
	case MUI_DATA_TYPE_WARN_DEV:
		return del_warn_dev(ui16Id);
	case MUI_DATA_TYPE_PLUG:
		return del_plug(ui16Id);
	case MUI_DATA_TYPE_SWITCH:
		return del_switch(ui16Id);
	case MUI_DATA_TYPE_SHADE:
		return del_shade(ui16Id);
	case MUI_DATA_TYPE_SONOS:
		return del_sonos(ui16Id);
	case MUI_DATA_TYPE_PANEL:
		return del_panel(ui16Id);
	default:
		return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}
//==================================================================================================
