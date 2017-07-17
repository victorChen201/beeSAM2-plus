#include "dev_if.h"
#include "light.h"
#include "zigbee_manage.h"
#include "database.h"
#include "group.h"
#include "set.h"
#include "schedule.h"
#include "api_deal.h"
#include "report.h"
#include "auto_report.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define LIGHT_DBG

#ifndef LIGHT_PUTC
#ifdef LIGHT_DBG
#define LIGHT_PUTC(Char)			PUTC(Char)
#else
#define LIGHT_PUTC(Char)
#endif
#endif

#ifndef LIGHT_PUTS
#ifdef LIGHT_DBG
#define LIGHT_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define LIGHT_PUTS(Str, Len)
#endif
#endif

#ifndef LIGHT_PUTASCC
#ifdef LIGHT_DBG
#define LIGHT_PUTASCC(Char)			PUTASCC(Char)
#else
#define LIGHT_PUTASCC(Char)
#endif
#endif

#ifndef LIGHT_PUTASCS
#ifdef LIGHT_DBG
#define LIGHT_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define LIGHT_PUTASCS(Str, Len)
#endif
#endif

#ifndef LIGHT_PRINT
#ifdef LIGHT_DBG
#define LIGHT_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define LIGHT_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef LIGHT_DYN_PUTC
#ifdef LIGHT_DBG
#define LIGHT_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_LIGHT|Dbg), Char)
#else
#define LIGHT_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef LIGHT_DYN_PUTS
#ifdef LIGHT_DBG
#define LIGHT_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_LIGHT|Dbg), Str, Len)
#else
#define LIGHT_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef LIGHT_DYN_PUTASCC
#ifdef LIGHT_DBG
#define LIGHT_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_LIGHT|Dbg), Char)
#else
#define LIGHT_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef LIGHT_DYN_PUTASCS
#ifdef LIGHT_DBG
#define LIGHT_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_LIGHT|Dbg), Str, Len)
#else
#define LIGHT_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef LIGHT_DYN_PRINT
#ifdef LIGHT_DBG
#define LIGHT_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_LIGHT|Dbg), Fmt, ##Arg)
#else
#define LIGHT_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

#define MUI_REMOTE_REPORT_LIGHT_ATTR_LEN	1024
#define MUI_REMOTE_REPORT_LIGHT_STATE_LEN	1024
#define MUI_REMOTE_REPORT_LIGHT_ALL_LEN		2048

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//static int light_default_state (S_LightStateRam *psState);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int light_default_state (S_LightState *psState)
{
	psState->reachable  	= 1;
	psState->on		  		= 1;
	psState->bri		  	= 100;
	psState->sat		  	= 10;
	psState->hue		  	= 1000;
	psState->ct		  		= MUI_LIGHT_STATE_CT_DFT;
	psState->xy.x		  	= 100;
	psState->xy.y		  	= 100;
	psState->colormode  	= MUI_LIGHT_COLOR_MODE_CT;
	psState->alert	  		= 1;
	psState->effect	  		= 1;

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int init_light_ram_data (uint16_t ui16Id, S_LightStateRam *psState)
{
	ASSERT(NULL != psState);
	int iRet;
	iRet = database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.zb_attr), &psState->zb_attr, M_SIZEOF(S_LightStateRam, zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		light_default_state(&psState->state);
		iRet = database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.ext_attr.colormode), &psState->state.colormode, M_SIZEOF(U_DevAttrSave, light_attr.ext_attr.colormode), M_MEM_TYPE_FLASH);
	}
	return iRet;
}
//--------------------------------------------------------------------------------------------------
int read_light_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, light_attr.api_attr), M_MEM_TYPE_FLASH);
}
int read_light_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int read_light_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int read_light_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int read_light_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int read_light_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int read_light_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int read_light_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_light_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, light_state.zb_attr), M_MEM_TYPE_RAM);
}
int read_light_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, light_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
}
int read_light_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, light_state.zb_attr.ep), M_MEM_TYPE_RAM);
}
//.............................................................................
int read_light_ext_attr (uint16_t ui16Id, S_LightExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, light_attr.ext_attr), M_MEM_TYPE_FLASH);
}
int read_light_ext_attr_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	ASSERT(NULL != pui8ColorMode);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.ext_attr.colormode), pui8ColorMode, M_SIZEOF(U_DevAttrSave, light_attr.ext_attr.colormode), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_light_state (uint16_t ui16Id, S_LightState *psState)
{
	ASSERT(NULL != psState);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state), psState, M_SIZEOF(U_DevStateSave, light_state.state), M_MEM_TYPE_RAM);
}
int read_light_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, light_state.state.reachable), M_MEM_TYPE_RAM);
}
int read_light_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	ASSERT(NULL != pui8On);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.on), pui8On, M_SIZEOF(U_DevStateSave, light_state.state.on), M_MEM_TYPE_RAM);
}
int read_light_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	ASSERT(NULL != pui8Bri);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.bri), pui8Bri, M_SIZEOF(U_DevStateSave, light_state.state.bri), M_MEM_TYPE_RAM);
}
int read_light_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	ASSERT(NULL != pui8Sat);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.sat), pui8Sat, M_SIZEOF(U_DevStateSave, light_state.state.sat), M_MEM_TYPE_RAM);
}
int read_light_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	ASSERT(NULL != pui16Hue);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.hue), pui16Hue, M_SIZEOF(U_DevStateSave, light_state.state.hue), M_MEM_TYPE_RAM);
}
int read_light_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	ASSERT(NULL != pui16Ct);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.ct), pui16Ct, M_SIZEOF(U_DevStateSave, light_state.state.ct), M_MEM_TYPE_RAM);
}
int read_light_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	ASSERT(NULL != psXY);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.xy), psXY, M_SIZEOF(U_DevStateSave, light_state.state.xy), M_MEM_TYPE_RAM);
}
int read_light_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	ASSERT(NULL != pui8ColorMode);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.colormode), pui8ColorMode, M_SIZEOF(U_DevStateSave, light_state.state.colormode), M_MEM_TYPE_RAM);
}
int read_light_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	ASSERT(NULL != pui8Alert);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.alert), pui8Alert, M_SIZEOF(U_DevStateSave, light_state.state.alert), M_MEM_TYPE_RAM);
}
int read_light_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	ASSERT(NULL != pui8Effect);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.effect), pui8Effect, M_SIZEOF(U_DevStateSave, light_state.state.effect), M_MEM_TYPE_RAM);
}
int read_light_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_light_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int read_light_misc (uint16_t ui16Id, S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.misc), psMisc, M_SIZEOF(U_DevStateSave, light_state.misc), M_MEM_TYPE_RAM);
}
int read_light_misc_new_dev (uint16_t ui16Id, uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, light_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int read_light_misc_not_added (uint16_t ui16Id, uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, light_state.misc.not_added), M_MEM_TYPE_RAM);
}
int read_light_misc_at_err_tick (uint16_t ui16Id, uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_read(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, light_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int write_light_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, light_attr.api_attr), M_MEM_TYPE_FLASH);
}
int write_light_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int write_light_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int write_light_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int write_light_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int write_light_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int write_light_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int write_light_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_light_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.zb_attr), psZbAttr, M_SIZEOF(U_DevAttrSave, light_attr.zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, light_state.zb_attr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_light_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevAttrSave, light_attr.zb_attr.nwkaddr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, light_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_light_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.zb_attr.ep), psEp, M_SIZEOF(U_DevAttrSave, light_attr.zb_attr.ep), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, light_state.zb_attr.ep), M_MEM_TYPE_RAM);
	}
	return iRet;
}
//.............................................................................
int write_light_ext_attr (uint16_t ui16Id, const S_LightExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, light_attr.ext_attr), M_MEM_TYPE_FLASH);
}
int write_light_ext_attr_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	ASSERT(NULL != pui8ColorMode);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.ext_attr.colormode), pui8ColorMode, M_SIZEOF(U_DevAttrSave, light_attr.ext_attr.colormode), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_light_state (uint16_t ui16Id, const S_LightState *psState)
{
	ASSERT(NULL != psState);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state), psState, M_SIZEOF(U_DevStateSave, light_state.state), M_MEM_TYPE_RAM);
}
int write_light_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, light_state.state.reachable), M_MEM_TYPE_RAM);
}
int write_light_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	ASSERT(NULL != pui8On);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.on), pui8On, M_SIZEOF(U_DevStateSave, light_state.state.on), M_MEM_TYPE_RAM);
}
int write_light_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	ASSERT(NULL != pui8Bri);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.bri), pui8Bri, M_SIZEOF(U_DevStateSave, light_state.state.bri), M_MEM_TYPE_RAM);
}
int write_light_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	ASSERT(NULL != pui8Sat);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.sat), pui8Sat, M_SIZEOF(U_DevStateSave, light_state.state.sat), M_MEM_TYPE_RAM);
}
int write_light_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	ASSERT(NULL != pui16Hue);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.hue), pui16Hue, M_SIZEOF(U_DevStateSave, light_state.state.hue), M_MEM_TYPE_RAM);
}
int write_light_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	ASSERT(NULL != pui16Ct);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.ct), pui16Ct, M_SIZEOF(U_DevStateSave, light_state.state.ct), M_MEM_TYPE_RAM);
}
int write_light_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	ASSERT(NULL != psXY);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.xy), psXY, M_SIZEOF(U_DevStateSave, light_state.state.xy), M_MEM_TYPE_RAM);
}
int write_light_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	int iRet;
	ASSERT(NULL != pui8ColorMode);
	iRet = database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.colormode), pui8ColorMode, M_SIZEOF(U_DevStateSave, light_state.state.colormode), M_MEM_TYPE_RAM);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevAttrSave, light_attr.ext_attr.colormode), pui8ColorMode, M_SIZEOF(U_DevAttrSave, light_attr.ext_attr.colormode), M_MEM_TYPE_FLASH);
	}
	return iRet;
}
int write_light_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	ASSERT(NULL != pui8Alert);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.alert), pui8Alert, M_SIZEOF(U_DevStateSave, light_state.state.alert), M_MEM_TYPE_RAM);
}
int write_light_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	ASSERT(NULL != pui8Effect);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.state.effect), pui8Effect, M_SIZEOF(U_DevStateSave, light_state.state.effect), M_MEM_TYPE_RAM);
}
int write_light_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_light_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//................................................
int write_light_misc (uint16_t ui16Id, const S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.misc), psMisc, M_SIZEOF(U_DevStateSave, light_state.misc), M_MEM_TYPE_RAM);
}
int write_light_misc_new_dev (uint16_t ui16Id, const uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, light_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int write_light_misc_not_added (uint16_t ui16Id, const uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, light_state.misc.not_added), M_MEM_TYPE_RAM);
}
int write_light_misc_at_err_tick (uint16_t ui16Id, const uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_write(MUI_DATA_TYPE_LIGHT, ui16Id, M_OFFSET(U_DevStateSave, light_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, light_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int get_light_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	return read_light_attr(ui16Id, psApiAttr);
}
int get_light_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	return read_light_attr_type(ui16Id, psType);
}
int get_light_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	return read_light_attr_name(ui16Id, psName);
}
int get_light_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	return read_light_attr_descr(ui16Id, psDescr);
}
int get_light_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	return read_light_attr_modelid(ui16Id, psModelId);
}
int get_light_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	return read_light_attr_swver(ui16Id, psSwVer);
}
int get_light_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	return read_light_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int get_light_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	return read_light_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int get_light_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	return read_light_zb_attr(ui16Id, psZbAttr);
}
int get_light_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	return read_light_zb_attr_nwkaddr(ui16Id, psNwkAddr);
}
int get_light_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	return read_light_zb_attr_ep(ui16Id, psEp);
}
//.............................................................................
int get_light_ext_attr (uint16_t ui16Id, S_LightExtAttr *psExtAttr)
{
	return read_light_ext_attr(ui16Id, psExtAttr);
}
//.............................................................................
int get_light_state (uint16_t ui16Id, S_LightState *psState)
{
	UNUSED(ui16Id);
	UNUSED(psState);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_light_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//--------------------------------------------------------------------------------------------------
int set_light_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	return write_light_attr(ui16Id, psApiAttr);
}
int set_light_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	UNUSED(ui16Id);
	UNUSED(psType);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	return write_light_attr_name(ui16Id, psName);
}
int set_light_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	return write_light_attr_descr(ui16Id, psDescr);
}
int set_light_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	UNUSED(ui16Id);
	UNUSED(psModelId);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	UNUSED(ui16Id);
	UNUSED(psSwVer);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	return write_light_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int set_light_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	return write_light_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int set_light_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	UNUSED(ui16Id);
	UNUSED(psZbAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	UNUSED(ui16Id);
	UNUSED(psNwkAddr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	UNUSED(ui16Id);
	UNUSED(psEp);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_light_ext_attr (uint16_t ui16Id, const S_LightExtAttr *psExtAttr)
{
	UNUSED(ui16Id);
	UNUSED(psExtAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_light_state (uint16_t ui16Id, const S_LightState *psState)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LIGHT_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_ON];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	S_LightState sState;
	uint8_t ui8ColorMode;
	uint8_t ui8Reachable;
	uint16_t ui16Ct;
	int iRet;

	ASSERT(NULL != psState);

	iRet = read_light_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_light_state(ui16Id, &sState);
	if (iRet < 0)
	{
		LIGHT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "read_light_state failed\r\n");
		return iRet;
	}

	iRet = read_light_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		LIGHT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "read_light_zb_attr failed\r\n");
		return iRet;
	}

	LIGHT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "sState.on=[%d]psState->on=[%d]\r\n", sState.on, psState->on);
	//if (sState.on != psState->on)
	//{
		if (psState->on)
		{
		#ifdef AT_PORT_UNQUEUE_QUERY
			iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ON_S"=%s,%s\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
		#else
			LIGHT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "zigbee_print_queue_queryp sState->on true\r\n");
			iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_ON_S"=%s,%s\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
		#endif
		}
		else
		{
		#ifdef AT_PORT_UNQUEUE_QUERY
			iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_OFF_S"=%s,%s\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
		#else
			LIGHT_DYN_PRINT(M_DBG_LEVEL_DEBUG, "zigbee_print_queue_query sState->on false\r\n");
			iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_OFF_S"=%s,%s\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
		#endif
		}
	//}
	sState.on = psState->on;

	//if(sState.bri != psState->bri)
	//{
	#ifdef AT_PORT_UNQUEUE_QUERY
		iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOLV_S"=%s,%s,%02x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psState->bri);
	#else
		iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_MVTOLV_S"=%s,%s,%02x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psState->bri);
	#endif
	//}
	sState.bri = psState->bri;

	if (MUI_LIGHT_COLOR_MODE_CT == sState.colormode)
	{
		//值范围修正
		ui16Ct = psState->ct;
		if (ui16Ct < MUI_LIGHT_STATE_CT_MIN)
		{
			ui16Ct = MUI_LIGHT_STATE_CT_MIN;
		}
		else if (ui16Ct > MUI_LIGHT_STATE_CT_MAX)
		{
			ui16Ct = MUI_LIGHT_STATE_CT_MAX;
		}
		//if(sState.ct != ui16Ct)
		//{
		#ifdef AT_PORT_UNQUEUE_QUERY
			iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOCT_S"=%s,%s,%04x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, ui16Ct);
		#else
			iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_MVTOCT_S"=%s,%s,%04x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, ui16Ct);
		#endif
		//}
		sState.ct = psState->ct;
	}
	else
	{
		//if(sState.xy.x != psState->xy.x || sState.xy.y != psState->xy.y)
		//{
		#ifdef AT_PORT_UNQUEUE_QUERY
			iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOS_XY"=%s,%s,%04x,%04x,0001\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psState->xy.x, psState->xy.y);
		#else
			iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_MVTOS_XY"=%s,%s,%04x,%04x,0001\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psState->xy.x, psState->xy.y);
		#endif
		//}
		sState.xy.x = psState->xy.x;
		sState.xy.y = psState->xy.y;
	}

	sState.reachable = psState->reachable;

	iRet = write_light_state(ui16Id, &sState);
	if (iRet < 0)
	{
		return iRet;
	}

	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LIGHT_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_ON];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != pui8On);

	iRet = read_light_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_light_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}
	if (*pui8On)
	{
	#ifdef AT_PORT_UNQUEUE_QUERY
		iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ON_S"=%s,%s\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
	#else
		iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_ON_S"=%s,%s\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
	#endif
	}
	else
	{
	#ifdef AT_PORT_UNQUEUE_QUERY
		iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_OFF_S"=%s,%s\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
	#else
		iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_OFF_S"=%s,%s\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
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
	iRet = write_light_state_on(ui16Id, pui8On);

	return iRet;
}
int set_light_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LIGHT_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_BRI];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != pui8Bri);

	iRet = read_light_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_light_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}
#ifdef AT_PORT_UNQUEUE_QUERY
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOLV_S"=%s,%s,%02x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, *pui8Bri);
#else
	iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_MVTOLV_S"=%s,%s,%02x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, *pui8Bri);
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
	iRet = write_light_state_bri(ui16Id, pui8Bri);

	return iRet;
}
int set_light_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LIGHT_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_CT];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8Reachable;
	uint8_t ui8ColorMode;
	uint8_t ui8ChangeRpt;
	uint16_t ui16Ct;
	int iRet;

	ASSERT(NULL != pui16Ct);

	iRet = read_light_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_light_state_colormode(ui16Id, &ui8ColorMode);
	if (iRet <= 0)
	{
		return iRet;
	}
	if (MUI_LIGHT_COLOR_MODE_CT != ui8ColorMode)
	{
		return DEV_IF_ERR_UNSUPPORT;
	}

	iRet = read_light_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
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
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOCT_S"=%s,%s,%04x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, ui16Ct);
#else
	iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_MVTOCT_S"=%s,%s,%04x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, ui16Ct);
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
	iRet = write_light_state_ct(ui16Id, &ui16Ct);

	return iRet;
}
int set_light_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LIGHT_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_XY];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ColorMode;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != psXY);

	iRet = read_light_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_light_state_colormode(ui16Id, &ui8ColorMode);
	if (iRet <= 0)
	{
		return iRet;
	}
	if (MUI_LIGHT_COLOR_MODE_XY != ui8ColorMode)
	{
		return DEV_IF_ERR_UNSUPPORT;
	}

	iRet = read_light_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}
#ifdef AT_PORT_UNQUEUE_QUERY
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOS_XY"=%s,%s,%04x,%04x,0001\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psXY->x, psXY->y);
#else
	iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LIGHT, ui16Id, AT_CMD_MVTOS_XY"=%s,%s,%04x,%04x,0001\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psXY->x, psXY->y);
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
	iRet = write_light_state_xy(ui16Id, psXY);

	return iRet;
}
int set_light_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_light_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//--------------------------------------------------------------------------------------------------
int update_light_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LIGHT_STATE_LEN];
	uint8_t ui8Reachable;
	uint8_t ui8On;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Changed = 0;
	int iRet;

	ASSERT(NULL != pui8On);

	iRet = read_light_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 == ui8Reachable)
	{
		ui8Reachable = 1;
		iRet = write_light_state_reachable(ui16Id, &ui8Reachable);
		if (iRet < 0)
		{
			return iRet;
		}
		ui8Changed = 1;
	}

	iRet = read_light_state_on(ui16Id, &ui8On);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui8On != *pui8On)
	{
		iRet = write_light_state_on(ui16Id, pui8On);
		if (iRet < 0)
		{
			//return iRet;	//因为可能涉及到reachable的修改,因此此处要忽略本错误
		}
		ui8Changed = 1;
	}

	if (0 == ui8Changed)
	{
		return iRet;
	}

	iRet = read_light_attr_changerpt(ui16Id, &ui8ChangeRpt);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui8ChangeRpt != 0)
	{
		//上报
		send_dev_state_report(MUI_HTTP_METHOD_EX_MODIFY, MUI_DATA_TYPE_LIGHT, ui16Id, MUI_DEV_STATE_ON);
	}

	return iRet;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int rebind_light (uint16_t ui16Id)
{
	uint8_t ui8Reachable;
	ui8Reachable = 1;
	return write_light_state_reachable(ui16Id, &ui8Reachable);
}
int add_light (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, const S_LightExtInfo *psExtInfo)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LIGHT_ALL_LEN];
	char achAnswer[MUI_AT_ANSWER_LEN_ADDGR];
	U_DevAttrSave uDevAttr;
	U_DevStateSave uDevState;
	int iNewId, iRet;

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psEp);

	//get lights version
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ATTRRR_S"=%s,%s,0000,4000\r\n", psNwkAddr->str, psEp->str);
	if (iRet <= 0)
	{
		strncpy(uDevAttr.light_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, light_attr.api_attr.swver.str));
	}
	else
	{
	    iRet = at_cmd_answer_verify(achAnswer, iRet);
	    if (iRet != 0)
	    {
			strncpy(uDevAttr.light_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, light_attr.api_attr.swver.str));
	    }
		if (1!=sscanf((char*)achAnswer, "$ATTRRR:%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%s\r\n", uDevAttr.light_attr.api_attr.swver.str))
		{
			strncpy(uDevAttr.light_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, light_attr.api_attr.swver.str));
		}
	}


	//Add Light to gropu0
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ADDGR_S"=%s,%s,%04x,group%d\r\n", psNwkAddr->str, psEp->str, MUI_ZIGBEE_GROUP_100_ID, MUI_ZIGBEE_GROUP_100_ID);
	if (iRet <= 0)
	{
		//return iRet; If add light to group fail, then ignore this AT command.
	}
	//Add to gropu200
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ADDGR_S"=%s,%s,%04x,group%d\r\n", psNwkAddr->str, psEp->str, MUI_SCENE_GROUP_200_ID, MUI_SCENE_GROUP_200_ID);
	if (iRet <= 0)
	{
		//return iRet; If add to group fail, then ignore this AT command.
	}

	//API attribute
	if (MUI_LIGHT_COLOR_MODE_CT == psExtInfo->colormode)
	{
		strncpy(uDevAttr.light_attr.api_attr.type.str, "Color temperature light", M_SIZEOF(U_DevAttrSave, light_attr.api_attr.type.str));
	}
	else
	{
		strncpy(uDevAttr.light_attr.api_attr.type.str, "Extended color light", M_SIZEOF(U_DevAttrSave, light_attr.api_attr.type.str));
	}
  	sprintf(uDevAttr.light_attr.api_attr.name.str, "Light ?");
	memset(uDevAttr.light_attr.api_attr.descr.str, 0, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.descr.str));
	strncpy(uDevAttr.light_attr.api_attr.modelid.str, psModelId->str, M_SIZEOF(U_DevAttrSave, light_attr.api_attr.modelid.str));
	uDevAttr.light_attr.api_attr.changerpt = 0;		//默认改变不上报
	uDevAttr.light_attr.api_attr.autorpt   = 0;		//默认不定时上报

	//Zigbee attribute
	memcpy(uDevAttr.light_attr.zb_attr.nwkaddr.str, psNwkAddr->str, M_SIZEOF(U_DevAttrSave, light_attr.zb_attr.nwkaddr.str));
	memcpy(uDevAttr.light_attr.zb_attr.ep.str, psEp->str, M_SIZEOF(U_DevAttrSave, light_attr.zb_attr.ep.str));

	//Extend attribute
	uDevAttr.light_attr.ext_attr.colormode = psExtInfo->colormode;

	//Misc state
	uDevState.light_state.misc.new_dev   = 1;
	uDevState.light_state.misc.not_added = 1;

	//Zigbee attribute
	memcpy(&uDevState.light_state.zb_attr, &uDevAttr.light_attr.zb_attr, M_SIZEOF(U_DevStateSave, light_state.zb_attr));

	//Device state
	light_default_state(&uDevState.light_state.state);
	uDevState.light_state.state.colormode = psExtInfo->colormode;

	iNewId = dev_data_add(MUI_DATA_TYPE_LIGHT, &uDevAttr, sizeof(U_DevAttrSave), &uDevState, sizeof(U_DevStateSave));
	if (iNewId < 0)
	{
		return iNewId;
	}
  	sprintf(uDevAttr.light_attr.api_attr.name.str, "Light %d", iNewId);
	write_light_attr_name(iNewId, &uDevAttr.light_attr.api_attr.name);

	//上报新增设备
	send_dev_report(MUI_HTTP_METHOD_EX_ADD, MUI_DATA_TYPE_LIGHT, iNewId, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);

	return iNewId;
}
int del_light (uint16_t ui16Id)
{
#if 0
	//2014-7-24 11:22 According to request of Jason, don't need send AT command - "AT$REMGR" to device.
	char achAnswer[MUI_AT_ANSWER_LEN_REMGR];
#endif
	S_DevIdx asDevIdx[MUI_AT_DEV_EP_NUM_MAX];	//!!!Chandler:此数组应该取决于系统一个MAC地址所关联设备的最大数,目前以switch设备为最多,可关联4个
	S_AttrModelId sModelId;
#if 0
	//2014-7-24 11:22 According to request of Jason, don't need send AT command - "AT$REMGR" to device.
	S_DevZbAttr sZbAttr;
#endif
	int iRet;

	iRet = read_light_attr_modelid(ui16Id, &sModelId);
	if (iRet < 0)
	{
		return iRet;
	}
#if 0
	//2014-7-24 11:22 According to request of Jason, don't need send AT command - "AT$REMGR" to device.
	iRet = read_light_zb_attr(ui16Id, &sZbAttr);
	if (iRet < 0)
	{
		return iRet;
	}
	//Delete Light from gropu0
	zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX,
					   AT_CMD_REMGR_S"=%s,%s,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, MUI_ZIGBEE_GROUP_100_ID);
	if (iRet <= 0)
	{
		//return iRet; If delete light from group fail, then ignore this AT command.
	}
#endif
	iRet = del_devs_data_by_modelid(&sModelId, asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));
	if (iRet <= 0)
	{
		return iRet;
	}

	return zigbee_dev_del(&sModelId);
}

