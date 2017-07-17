#include "dev_if.h"
#include "sonos.h"
#include "zigbee_manage.h"
#include "database.h"
#include "group.h"
#include "set.h"
#include "schedule.h"
#include "api_deal.h"
#include "report.h"
#include "auto_report.h"
#include "sonos_drv.h"
#include "extend_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SONOS_DBG

#ifndef SONOS_PUTC
#ifdef SONOS_DBG
#define SONOS_PUTC(Char)			PUTC(Char)
#else
#define SONOS_PUTC(Char)
#endif
#endif

#ifndef SONOS_PUTS
#ifdef SONOS_DBG
#define SONOS_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SONOS_PUTS(Str, Len)
#endif
#endif

#ifndef SONOS_PUTASCC
#ifdef SONOS_DBG
#define SONOS_PUTASCC(Char)			PUTASCC(Char)
#else
#define SONOS_PUTASCC(Char)
#endif
#endif

#ifndef SONOS_PUTASCS
#ifdef SONOS_DBG
#define SONOS_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SONOS_PUTASCS(Str, Len)
#endif
#endif

#ifndef SONOS_PRINT
#ifdef SONOS_DBG
#define SONOS_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SONOS_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SONOS_DYN_PUTC
#ifdef SONOS_DBG
#define SONOS_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SONOS|Dbg), Char)
#else
#define SONOS_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SONOS_DYN_PUTS
#ifdef SONOS_DBG
#define SONOS_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SONOS|Dbg), Str, Len)
#else
#define SONOS_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SONOS_DYN_PUTASCC
#ifdef SONOS_DBG
#define SONOS_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SONOS|Dbg), Char)
#else
#define SONOS_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SONOS_DYN_PUTASCS
#ifdef SONOS_DBG
#define SONOS_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SONOS|Dbg), Str, Len)
#else
#define SONOS_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SONOS_DYN_PRINT
#ifdef SONOS_DBG
#define SONOS_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SONOS|Dbg), Fmt, ##Arg)
#else
#define SONOS_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif


#define MUI_REMOTE_REPORT_SONOS_ATTR_LEN	1024
#define MUI_REMOTE_REPORT_SONOS_STATE_LEN	1024
#define MUI_REMOTE_REPORT_SONOS_ALL_LEN		2048
#define MUI_SONOS_URL_MAX	1024
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//static int sonos_default_state (S_SonosStateRam *psState);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int sonos_default_state (S_SonosState *psState)
{
	psState->reachable = 1;
	psState->play	   = 0;
	psState->volume	   = 30;
	psState->urlid	   = 0;

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int init_sonos_ram_data (uint16_t ui16Id, S_SonosStateRam *psState)
{
	ASSERT(NULL != psState);
	int iRet;
	iRet = database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.zb_attr), &psState->zb_attr, M_SIZEOF(S_SonosStateRam, zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		sonos_default_state(&psState->state);
	}
	return iRet;
}
//--------------------------------------------------------------------------------------------------
int read_sonos_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr), M_MEM_TYPE_FLASH);
}
int read_sonos_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int read_sonos_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int read_sonos_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int read_sonos_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int read_sonos_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int read_sonos_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int read_sonos_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_sonos_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, sonos_state.zb_attr), M_MEM_TYPE_RAM);
}
int read_sonos_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, sonos_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
}
int read_sonos_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, sonos_state.zb_attr.ep), M_MEM_TYPE_RAM);
}
//.............................................................................
int read_sonos_ext_attr (uint16_t ui16Id, S_SonosExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, sonos_attr.ext_attr), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_sonos_state (uint16_t ui16Id, S_SonosState *psState)
{
	ASSERT(NULL != psState);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state), psState, M_SIZEOF(U_DevStateSave, sonos_state.state), M_MEM_TYPE_RAM);
}
int read_sonos_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, sonos_state.state.reachable), M_MEM_TYPE_RAM);
}
int read_sonos_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_sonos_state_play (uint16_t ui16Id, uint8_t *pui8Play)
{
	ASSERT(NULL != pui8Play);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state.play), pui8Play, M_SIZEOF(U_DevStateSave, sonos_state.state.play), M_MEM_TYPE_RAM);
}
int read_sonos_state_volume (uint16_t ui16Id, uint16_t *pui16Volume)
{
	ASSERT(NULL != pui16Volume);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state.volume), pui16Volume, M_SIZEOF(U_DevStateSave, sonos_state.state.volume), M_MEM_TYPE_RAM);
}
int read_sonos_state_url_id (uint16_t ui16Id, uint32_t *pui32UrlId)
{
	ASSERT(NULL != pui32UrlId);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state.urlid), pui32UrlId, M_SIZEOF(U_DevStateSave, sonos_state.state.urlid), M_MEM_TYPE_RAM);
}
//.............................................................................
int read_sonos_misc (uint16_t ui16Id, S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.misc), psMisc, M_SIZEOF(U_DevStateSave, sonos_state.misc), M_MEM_TYPE_RAM);
}
int read_sonos_misc_new_dev (uint16_t ui16Id, uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, sonos_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int read_sonos_misc_not_added (uint16_t ui16Id, uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, sonos_state.misc.not_added), M_MEM_TYPE_RAM);
}
int read_sonos_misc_at_err_tick (uint16_t ui16Id, uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_read(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, sonos_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int write_sonos_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr), M_MEM_TYPE_FLASH);
}
int write_sonos_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int write_sonos_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int write_sonos_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int write_sonos_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int write_sonos_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int write_sonos_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int write_sonos_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_sonos_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.zb_attr), psZbAttr, M_SIZEOF(U_DevAttrSave, sonos_attr.zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, sonos_state.zb_attr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_sonos_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevAttrSave, sonos_attr.zb_attr.nwkaddr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, sonos_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_sonos_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.zb_attr.ep), psEp, M_SIZEOF(U_DevAttrSave, sonos_attr.zb_attr.ep), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, sonos_state.zb_attr.ep), M_MEM_TYPE_RAM);
	}
	return iRet;
}
//.............................................................................
int write_sonos_ext_attr (uint16_t ui16Id, const S_SonosExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevAttrSave, sonos_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, sonos_attr.ext_attr), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_sonos_state (uint16_t ui16Id, const S_SonosState *psState)
{
	ASSERT(NULL != psState);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state), psState, M_SIZEOF(U_DevStateSave, sonos_state.state), M_MEM_TYPE_RAM);
}
int write_sonos_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, sonos_state.state.reachable), M_MEM_TYPE_RAM);
}
int write_sonos_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_sonos_state_play (uint16_t ui16Id, const uint8_t *pui8Play)
{
	ASSERT(NULL != pui8Play);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state.play), pui8Play, M_SIZEOF(U_DevStateSave, sonos_state.state.play), M_MEM_TYPE_RAM);
}
int write_sonos_state_volume (uint16_t ui16Id, const uint16_t *pui16Volume)
{
	ASSERT(NULL != pui16Volume);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state.volume), pui16Volume, M_SIZEOF(U_DevStateSave, sonos_state.state.volume), M_MEM_TYPE_RAM);
}
int write_sonos_state_url_id (uint16_t ui16Id, const uint32_t *pui32UrlId)
{
	ASSERT(NULL != pui32UrlId);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.state.urlid), pui32UrlId, M_SIZEOF(U_DevStateSave, sonos_state.state.urlid), M_MEM_TYPE_RAM);
}
//................................................
int write_sonos_misc (uint16_t ui16Id, const S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.misc), psMisc, M_SIZEOF(U_DevStateSave, sonos_state.misc), M_MEM_TYPE_RAM);
}
int write_sonos_misc_new_dev (uint16_t ui16Id, const uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, sonos_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int write_sonos_misc_not_added (uint16_t ui16Id, const uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, sonos_state.misc.not_added), M_MEM_TYPE_RAM);
}
int write_sonos_misc_at_err_tick (uint16_t ui16Id, const uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_write(MUI_DATA_TYPE_SONOS, ui16Id, M_OFFSET(U_DevStateSave, sonos_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, sonos_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int get_sonos_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	return read_sonos_attr(ui16Id, psApiAttr);
}
int get_sonos_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	return read_sonos_attr_type(ui16Id, psType);
}
int get_sonos_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	return read_sonos_attr_name(ui16Id, psName);
}
int get_sonos_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	return read_sonos_attr_descr(ui16Id, psDescr);
}
int get_sonos_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	return read_sonos_attr_modelid(ui16Id, psModelId);
}
int get_sonos_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	return read_sonos_attr_swver(ui16Id, psSwVer);
}
int get_sonos_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	return read_sonos_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int get_sonos_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	return read_sonos_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int get_sonos_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	return read_sonos_zb_attr(ui16Id, psZbAttr);
}
int get_sonos_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	return read_sonos_zb_attr_nwkaddr(ui16Id, psNwkAddr);
}
int get_sonos_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	return read_sonos_zb_attr_ep(ui16Id, psEp);
}
//.............................................................................
int get_sonos_ext_attr (uint16_t ui16Id, S_SonosExtAttr *psExtAttr)
{
	return read_sonos_ext_attr(ui16Id, psExtAttr);
}
//.............................................................................
int get_sonos_state (uint16_t ui16Id, S_SonosState *psState)
{
	UNUSED(ui16Id);
	UNUSED(psState);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_sonos_state_play (uint16_t ui16Id, uint8_t *pui8Play)
{
	return read_sonos_state_play(ui16Id, pui8Play);
}
int get_sonos_state_volume (uint16_t ui16Id, uint16_t *pui16Volume)
{
	return read_sonos_state_volume(ui16Id, pui16Volume);
}
int get_sonos_state_url_id (uint16_t ui16Id, uint32_t *pui32UrlId)
{
	return read_sonos_state_url_id(ui16Id, pui32UrlId);
}
//--------------------------------------------------------------------------------------------------
int set_sonos_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	return write_sonos_attr(ui16Id, psApiAttr);
}
int set_sonos_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	UNUSED(ui16Id);
	UNUSED(psType);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_sonos_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	return write_sonos_attr_name(ui16Id, psName);
}
int set_sonos_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	return write_sonos_attr_descr(ui16Id, psDescr);
}
int set_sonos_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	UNUSED(ui16Id);
	UNUSED(psModelId);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_sonos_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	UNUSED(ui16Id);
	UNUSED(psSwVer);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_sonos_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	return write_sonos_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int set_sonos_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	return write_sonos_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int set_sonos_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	UNUSED(ui16Id);
	UNUSED(psZbAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_sonos_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	UNUSED(ui16Id);
	UNUSED(psNwkAddr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_sonos_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	UNUSED(ui16Id);
	UNUSED(psEp);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_sonos_ext_attr (uint16_t ui16Id, const S_SonosExtAttr *psExtAttr)
{
	UNUSED(ui16Id);
	UNUSED(psExtAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_sonos_state (uint16_t ui16Id, const S_SonosState *psState)
{
	UNUSED(ui16Id);
	UNUSED(psState);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_sonos_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_sonos_state_play (uint16_t ui16Id, const uint8_t *pui8Play)
{
	char achUrl[MUI_SONOS_URL_MAX];
	S_DevZbAttr sNetAttr;
	uint32_t ui32UrlId;
	//uint16_t ui16Volume;
	uint8_t ui8Play;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != pui8Play);

	iRet = read_sonos_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_sonos_zb_attr(ui16Id, &sNetAttr);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = read_sonos_state_url_id(ui16Id, &ui32UrlId);
	if (iRet < 0)
	{
		return iRet;
	}
	/*
	iRet = read_sonos_state_volume(ui16Id, &ui16Volume);
	if (iRet < 0)
	{
		return iRet;
	}*/

	ui8Play = (*pui8Play!=0) ? 1 : 0;
	iRet = write_sonos_state_play(ui16Id, &ui8Play);
	if (iRet < 0)
	{
		return iRet;
	}
/*
	iRet = get_extra_data(ui32UrlId, achUrl, MUI_SONOS_URL_MAX);
	if (iRet < 0)
	{
		return -1;
	}
*/
	if (ui8Play != 0)
	{
		//SONOS_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[set_sonos_state_play] $$$$$$$$$$$$$$$$$$$[ play = %hhu  ]$$$$$$$$$$$$$$$\r\n",ui8Play);
		//sonos_drv_set_volume(&sNetAttr, ui16Volume);
		sonos_drv_play(&sNetAttr, ui8Play);
	}
	else
	{
		//SONOS_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[set_sonos_state_play] $$$$$$$$$$$$$$$$$$$[ play = %hhu  ]$$$$$$$$$$$$$$$\r\n",ui8Play);
		sonos_drv_play(&sNetAttr, ui8Play);
	}

	return iRet;
}
int set_sonos_state_volume (uint16_t ui16Id, const uint16_t *pui16Volume)
{
	S_DevZbAttr sNetAttr;
	uint16_t ui16Volume;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != pui16Volume);

	iRet = read_sonos_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_sonos_zb_attr(ui16Id, &sNetAttr);
	if (iRet < 0)
	{
		return iRet;
	}

	ui16Volume = (*pui16Volume>MUI_SONOS_STATE_VOLUME_MAX) ? MUI_SONOS_STATE_VOLUME_MAX : (*pui16Volume);

	iRet = write_sonos_state_volume(ui16Id, &ui16Volume);
	if (iRet < 0)
	{
		return iRet;
	}

	sonos_drv_set_volume(&sNetAttr, ui16Volume);

	return iRet;
}
int set_sonos_state_url_id (uint16_t ui16Id, const uint32_t *pui32UrlId)
{
	char achUrl[MUI_SONOS_URL_MAX];
	S_DevZbAttr sNetAttr;
	//uint16_t ui16Volume;
	uint8_t ui8Play = 1;
	uint8_t ui8Reachable;
	int urllen = 0;
	int iRet;

	ASSERT(NULL != pui32UrlId);

	iRet = read_sonos_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_sonos_zb_attr(ui16Id, &sNetAttr);
	if (iRet < 0)
	{
		return iRet;
	}
	urllen = extend_data_ext_id_read(MUI_DATA_TYPE_SONOS,ui16Id,*pui32UrlId,achUrl,sizeof(achUrl));
	if (urllen < 0)
	{
 		return -1;
	}
	//SONOS_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[set_sonos_state_url_id] %%%%%%%%%%%%%%%%%[ id = %hu urlid =%u [ url=%s ] ]%%%%%%%%%%%%%%%%%%\r\n",ui16Id,*pui32UrlId,achUrl);
	/*
	iRet = read_sonos_state_volume(ui16Id, &ui16Volume);
	if (iRet < 0)
	{
		return iRet;
	}*/

	iRet = write_sonos_state_url_id(ui16Id, pui32UrlId);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = write_sonos_state_play(ui16Id, &ui8Play);
	if (iRet < 0)
	{
		return iRet;
	}

	//sonos_drv_set_volume(&sNetAttr, ui16Volume);
	sonos_drv_set_url(&sNetAttr, achUrl,urllen);

	return iRet;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int get_sonos_total (void)
{
	return dev_data_num_get_by_type(MUI_DATA_TYPE_SONOS);
}
int get_sonos_all_id (uint16_t *pui16IdList, uint16_t ui16Capacity)
{
	return dev_data_id_list_get_by_type(MUI_DATA_TYPE_SONOS, pui16IdList, ui16Capacity);
}
//return number of devices that with same modelid
int find_sonoses_modelid (const S_AttrModelId *psModelId, S_DevIdx *psDevIdx, uint8_t ui8MaxNum)
{
	S_AttrModelId sModelId;
	uint16_t aui16IdList[MUI_LOGIC_DEV_NUM_MAX];
	int i, iNum;
	uint8_t ui8Cnt = 0;

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psDevIdx);
	ASSERT(ui8MaxNum > 0);

	iNum = get_sonos_all_id(aui16IdList, sizeof(aui16IdList));
	if (iNum <= 0)
	{
		SONOS_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[find_sonoses_modelid]Have not find any device.\r\n");
		return ui8Cnt;
	}
	for (i=0; i<iNum; i++)
	{
		if ((0<read_sonos_attr_modelid(aui16IdList[i], &sModelId)) &&
			(0==strncmp(sModelId.str, psModelId->str, sizeof(sModelId.str))))
		{
			psDevIdx[ui8Cnt].ui16Type = (uint16_t)MUI_DATA_TYPE_SONOS;
			psDevIdx[ui8Cnt].ui16Id   = aui16IdList[i];
			ui8Cnt++;
			if (ui8Cnt >= ui8MaxNum)
			{
				break;
			}
		}
	}

	return ui8Cnt;
}
//return exist device ID
int chk_sonos_zb_nwkaddr_ep (const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp)
{
	int i, iNum;
	S_DevZbAttr sZbAttr;
	uint16_t aui16IdList[MUI_LOGIC_DEV_NUM_MAX];

	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psEp);

	iNum = get_sonos_all_id(aui16IdList, sizeof(aui16IdList));
	if (iNum <= 0)
	{
		return DEV_IF_ERR_NO_FIND_NWK_ADDR;
	}
	for (i=0; i<iNum; i++)
	{
		if ((0<read_sonos_zb_attr(aui16IdList[i], &sZbAttr)) &&
			(0==strncmp(sZbAttr.nwkaddr.ip, psNwkAddr->ip, sizeof(sZbAttr.nwkaddr.ip))) &&
			(sZbAttr.ep.port==psEp->port))
		{
			return aui16IdList[i];
		}
	}

	return DEV_IF_ERR_NO_FIND_NWK_ADDR;
}
//return number of devices that have been update.
int update_sonoses_zb_nwkaddr (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, S_DevIdx *psDevIdx, uint8_t ui8MaxNum)
{
	S_ZbAttrNwkAddr sNwkAddr;
	uint8_t aui8Buf[MUI_REMOTE_REPORT_SONOS_ALL_LEN];
	uint8_t ui8Reachable, ui8ChangeRpt, ui8ReportMode;
	int i, iRet, iNum, iCnt=0;

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psDevIdx);
	ASSERT(ui8MaxNum > 0);

	iNum = find_sonoses_modelid(psModelId, psDevIdx, ui8MaxNum);
	if (iNum <= 0)
	{
		SONOS_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_sonoses_zb_nwkaddr]Have not find any device.\r\n");
		return iNum;
	}
	//SONOS_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_sonoses_zb_nwkaddr]Have found %d devices with same model id.\r\n", iNum);
	for (i=0; i<iNum; i++)
	{
		ui8ReportMode = 0;

		if ((0<read_sonos_state_reachable(psDevIdx[i].ui16Id, &ui8Reachable)) &&
			(0==ui8Reachable))
		{
			ui8Reachable = 1;
			if (0 > write_sonos_state_reachable(psDevIdx[i].ui16Id, &ui8Reachable))
			{
				SONOS_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_sonoses_zb_nwkaddr]Call write_sonos_state_reachable fail.\r\n");
			}
			else
			{
				ui8ReportMode += 2;
			}
		}

		if ((0<read_sonos_zb_attr_nwkaddr(psDevIdx[i].ui16Id, &sNwkAddr)) &&
			(0!=strncmp(sNwkAddr.ip, psNwkAddr->ip, sizeof(sNwkAddr.ip))))
		{
			if (0 > write_sonos_zb_attr_nwkaddr(psDevIdx[i].ui16Id, psNwkAddr))
			{
				SONOS_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_sonoses_zb_nwkaddr]Call write_sonos_zb_attr_nwkaddr fail.\r\n");
			}
			else
			{
				iCnt++;
				ui8ReportMode += 1;
			}
		}

		if (ui8ReportMode != 0)
		{
			iRet = read_sonos_attr_changerpt(psDevIdx[i].ui16Id, &ui8ChangeRpt);
			if (iRet < 0)
			{
				return iRet;
			}

			if (ui8ChangeRpt != 0)
			{
				//上报
				send_dev_report(MUI_HTTP_METHOD_EX_MODIFY, MUI_DATA_TYPE_SONOS, psDevIdx[i].ui16Id, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);
			}
		}
	}

	return iNum;
}
int rebind_sonos (uint16_t ui16Id)
{
	UNUSED(ui16Id);
	return DEV_IF_ERR_UNSUPPORT;
}
int add_sonos (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, const S_SonosExtInfo *psExtInfo)
{
	//char achAnswer[MUI_AT_ANSWER_LEN_ADDGR];
	U_DevAttrSave uDevAttr;
	U_DevStateSave uDevState;
	int iNewId, iRet;

	UNUSED(psExtInfo);

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psEp);

	//API attribute
	strncpy(uDevAttr.sonos_attr.api_attr.type.str, "Sonos", M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.type.str));
  	sprintf(uDevAttr.sonos_attr.api_attr.name.str, "Sonos ?");
	memset(uDevAttr.sonos_attr.api_attr.descr.str, 0, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.descr.str));
	strncpy(uDevAttr.sonos_attr.api_attr.modelid.str, psModelId->str, M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.modelid.str));
	strncpy(uDevAttr.sonos_attr.api_attr.swver.str, "1.0.0", M_SIZEOF(U_DevAttrSave, sonos_attr.api_attr.swver.str));
	uDevAttr.sonos_attr.api_attr.changerpt = 1;		//默认改变上报
	uDevAttr.sonos_attr.api_attr.autorpt   = 0;		//默认不定时上报

	//Zigbee attribute
	memcpy(uDevAttr.sonos_attr.zb_attr.nwkaddr.ip, psNwkAddr->ip, M_SIZEOF(U_DevAttrSave, sonos_attr.zb_attr.nwkaddr.ip));
	uDevAttr.sonos_attr.zb_attr.ep.port = psEp->port;

	//Misc state
	uDevState.sonos_state.misc.new_dev   = 1;
	uDevState.sonos_state.misc.not_added = 1;

	//Zigbee attribute
	memcpy(&uDevState.sonos_state.zb_attr, &uDevAttr.sonos_attr.zb_attr, M_SIZEOF(U_DevStateSave, sonos_state.zb_attr));

	//Device state
	sonos_default_state(&uDevState.sonos_state.state);

	iNewId = dev_data_add(MUI_DATA_TYPE_SONOS, &uDevAttr, sizeof(U_DevAttrSave), &uDevState, sizeof(U_DevStateSave));
	if (iNewId < 0)
	{
		return iNewId;
	}
  	sprintf(uDevAttr.sonos_attr.api_attr.name.str, "Sonos %d", iNewId);
	write_sonos_attr_name(iNewId, &uDevAttr.sonos_attr.api_attr.name);

	return iNewId;
}
int del_sonos (uint16_t ui16Id)
{
	S_DevIdx asDevIdx[MUI_AT_DEV_EP_NUM_MAX];	//!!!Chandler:此数组应该取决于系统一个MAC地址所关联设备的最大数,目前以switch设备为最多,可关联4个
	S_AttrModelId sModelId;
	int iRet;

	iRet = read_sonos_attr_modelid(ui16Id, &sModelId);
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = del_devs_data_by_modelid(&sModelId, asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));
	if (iRet <= 0)
	{
		return iRet;
	}

	return iRet;
}

