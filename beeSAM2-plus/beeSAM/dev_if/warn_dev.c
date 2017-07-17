#include "dev_if.h"
#include "warn_dev.h"
#include "zigbee_manage.h"
#include "database.h"
#include "group.h"
#include "set.h"
#include "schedule.h"
#include "api_deal.h"
#include "report.h"
#include "auto_report.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define WARN_DEV_DBG

#ifndef WARN_DEV_PUTC
#ifdef WARN_DEV_DBG
#define WARN_DEV_PUTC(Char)			PUTC(Char)
#else
#define WARN_DEV_PUTC(Char)
#endif
#endif

#ifndef WARN_DEV_PUTS
#ifdef WARN_DEV_DBG
#define WARN_DEV_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define WARN_DEV_PUTS(Str, Len)
#endif
#endif

#ifndef WARN_DEV_PUTASCC
#ifdef WARN_DEV_DBG
#define WARN_DEV_PUTASCC(Char)			PUTASCC(Char)
#else
#define WARN_DEV_PUTASCC(Char)
#endif
#endif

#ifndef WARN_DEV_PUTASCS
#ifdef WARN_DEV_DBG
#define WARN_DEV_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define WARN_DEV_PUTASCS(Str, Len)
#endif
#endif

#ifndef WARN_DEV_PRINT
#ifdef WARN_DEV_DBG
#define WARN_DEV_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define WARN_DEV_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef WARN_DEV_DYN_PUTC
#ifdef WARN_DEV_DBG
#define WARN_DEV_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_WARN_DEV|Dbg), Char)
#else
#define WARN_DEV_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef WARN_DEV_DYN_PUTS
#ifdef WARN_DEV_DBG
#define WARN_DEV_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_WARN_DEV|Dbg), Str, Len)
#else
#define WARN_DEV_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef WARN_DEV_DYN_PUTASCC
#ifdef WARN_DEV_DBG
#define WARN_DEV_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_WARN_DEV|Dbg), Char)
#else
#define WARN_DEV_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef WARN_DEV_DYN_PUTASCS
#ifdef WARN_DEV_DBG
#define WARN_DEV_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_WARN_DEV|Dbg), Str, Len)
#else
#define WARN_DEV_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef WARN_DEV_DYN_PRINT
#ifdef WARN_DEV_DBG
#define WARN_DEV_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_WARN_DEV|Dbg), Fmt, ##Arg)
#else
#define WARN_DEV_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif


#define MUI_REMOTE_REPORT_WARN_DEV_ATTR_LEN	1024
#define MUI_REMOTE_REPORT_WARN_DEV_STATE_LEN	1024
#define MUI_REMOTE_REPORT_WARN_DEV_ALL_LEN		2048

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//static int warn_dev_default_state (S_WarnDevStateRam *psState);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int warn_dev_default_state (S_WarnDevState *psState)
{
	psState->reachable = 1;
	psState->warncfg.warnmode	 = 0;
	psState->warncfg.duration  = 0;

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int init_warn_dev_ram_data (uint16_t ui16Id, S_WarnDevStateRam *psState)
{
	ASSERT(NULL != psState);
	int iRet;
	iRet = database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.zb_attr), &psState->zb_attr, M_SIZEOF(S_WarnDevStateRam, zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		warn_dev_default_state(&psState->state);
	}
	return iRet;
}
//--------------------------------------------------------------------------------------------------
int read_warn_dev_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr), M_MEM_TYPE_FLASH);
}
int read_warn_dev_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int read_warn_dev_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int read_warn_dev_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int read_warn_dev_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int read_warn_dev_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int read_warn_dev_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int read_warn_dev_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_warn_dev_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, warn_dev_state.zb_attr), M_MEM_TYPE_RAM);
}
int read_warn_dev_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, warn_dev_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
}
int read_warn_dev_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, warn_dev_state.zb_attr.ep), M_MEM_TYPE_RAM);
}
//.............................................................................
int read_warn_dev_ext_attr (uint16_t ui16Id, S_WarnDevExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, warn_dev_attr.ext_attr), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_warn_dev_state (uint16_t ui16Id, S_WarnDevState *psState)
{
	ASSERT(NULL != psState);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state), psState, M_SIZEOF(U_DevStateSave, warn_dev_state.state), M_MEM_TYPE_RAM);
}
int read_warn_dev_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, warn_dev_state.state.reachable), M_MEM_TYPE_RAM);
}
int read_warn_dev_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	ASSERT(NULL != pui8WarnMode);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state.warncfg.warnmode), pui8WarnMode, M_SIZEOF(U_DevStateSave, warn_dev_state.state.warncfg.warnmode), M_MEM_TYPE_RAM);
}
int read_warn_dev_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	ASSERT(NULL != pui16Duration);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state.warncfg.duration), pui16Duration, M_SIZEOF(U_DevStateSave, warn_dev_state.state.warncfg.duration), M_MEM_TYPE_RAM);
}
int read_warn_dev_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	ASSERT(NULL != psWarnCfg);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state.warncfg), psWarnCfg, M_SIZEOF(U_DevStateSave, warn_dev_state.state.warncfg), M_MEM_TYPE_RAM);
}
int read_warn_dev_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_warn_dev_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int read_warn_dev_misc (uint16_t ui16Id, S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.misc), psMisc, M_SIZEOF(U_DevStateSave, warn_dev_state.misc), M_MEM_TYPE_RAM);
}
int read_warn_dev_misc_new_dev (uint16_t ui16Id, uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, warn_dev_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int read_warn_dev_misc_not_added (uint16_t ui16Id, uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, warn_dev_state.misc.not_added), M_MEM_TYPE_RAM);
}
int read_warn_dev_misc_at_err_tick (uint16_t ui16Id, uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_read(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, warn_dev_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int write_warn_dev_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr), M_MEM_TYPE_FLASH);
}
int write_warn_dev_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int write_warn_dev_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int write_warn_dev_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int write_warn_dev_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int write_warn_dev_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int write_warn_dev_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int write_warn_dev_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_warn_dev_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.zb_attr), psZbAttr, M_SIZEOF(U_DevAttrSave, warn_dev_attr.zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, warn_dev_state.zb_attr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_warn_dev_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevAttrSave, warn_dev_attr.zb_attr.nwkaddr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, warn_dev_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_warn_dev_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.zb_attr.ep), psEp, M_SIZEOF(U_DevAttrSave, warn_dev_attr.zb_attr.ep), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, warn_dev_state.zb_attr.ep), M_MEM_TYPE_RAM);
	}
	return iRet;
}
//.............................................................................
int write_warn_dev_ext_attr (uint16_t ui16Id, const S_WarnDevExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevAttrSave, warn_dev_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, warn_dev_attr.ext_attr), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_warn_dev_state (uint16_t ui16Id, const S_WarnDevState *psState)
{
	ASSERT(NULL != psState);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state), psState, M_SIZEOF(U_DevStateSave, warn_dev_state.state), M_MEM_TYPE_RAM);
}
int write_warn_dev_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, warn_dev_state.state.reachable), M_MEM_TYPE_RAM);
}
int write_warn_dev_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	ASSERT(NULL != pui8WarnMode);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state.warncfg.warnmode), pui8WarnMode, M_SIZEOF(U_DevStateSave, warn_dev_state.state.warncfg.warnmode), M_MEM_TYPE_RAM);
}
int write_warn_dev_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	ASSERT(NULL != pui16Duration);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state.warncfg.duration), pui16Duration, M_SIZEOF(U_DevStateSave, warn_dev_state.state.warncfg.duration), M_MEM_TYPE_RAM);
}
int write_warn_dev_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	ASSERT(NULL != psWarnCfg);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.state.warncfg), psWarnCfg, M_SIZEOF(U_DevStateSave, warn_dev_state.state.warncfg), M_MEM_TYPE_RAM);
}
int write_warn_dev_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_warn_dev_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//................................................
int write_warn_dev_misc (uint16_t ui16Id, const S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.misc), psMisc, M_SIZEOF(U_DevStateSave, warn_dev_state.misc), M_MEM_TYPE_RAM);
}
int write_warn_dev_misc_new_dev (uint16_t ui16Id, const uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, warn_dev_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int write_warn_dev_misc_not_added (uint16_t ui16Id, const uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, warn_dev_state.misc.not_added), M_MEM_TYPE_RAM);
}
int write_warn_dev_misc_at_err_tick (uint16_t ui16Id, const uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_write(MUI_DATA_TYPE_WARN_DEV, ui16Id, M_OFFSET(U_DevStateSave, warn_dev_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, warn_dev_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int get_warn_dev_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	return read_warn_dev_attr(ui16Id, psApiAttr);
}
int get_warn_dev_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	return read_warn_dev_attr_type(ui16Id, psType);
}
int get_warn_dev_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	return read_warn_dev_attr_name(ui16Id, psName);
}
int get_warn_dev_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	return read_warn_dev_attr_descr(ui16Id, psDescr);
}
int get_warn_dev_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	return read_warn_dev_attr_modelid(ui16Id, psModelId);
}
int get_warn_dev_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	return read_warn_dev_attr_swver(ui16Id, psSwVer);
}
int get_warn_dev_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	return read_warn_dev_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int get_warn_dev_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	return read_warn_dev_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int get_warn_dev_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	return read_warn_dev_zb_attr(ui16Id, psZbAttr);
}
int get_warn_dev_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	return read_warn_dev_zb_attr_nwkaddr(ui16Id, psNwkAddr);
}
int get_warn_dev_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	return read_warn_dev_zb_attr_ep(ui16Id, psEp);
}
//.............................................................................
int get_warn_dev_ext_attr (uint16_t ui16Id, S_WarnDevExtAttr *psExtAttr)
{
	return read_warn_dev_ext_attr(ui16Id, psExtAttr);
}
//.............................................................................
int get_warn_dev_state (uint16_t ui16Id, S_WarnDevState *psState)
{
	UNUSED(ui16Id);
	UNUSED(psState);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_warn_dev_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//--------------------------------------------------------------------------------------------------
int set_warn_dev_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	return write_warn_dev_attr(ui16Id, psApiAttr);
}
int set_warn_dev_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	UNUSED(ui16Id);
	UNUSED(psType);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	return write_warn_dev_attr_name(ui16Id, psName);
}
int set_warn_dev_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	return write_warn_dev_attr_descr(ui16Id, psDescr);
}
int set_warn_dev_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	UNUSED(ui16Id);
	UNUSED(psModelId);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	UNUSED(ui16Id);
	UNUSED(psSwVer);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	return write_warn_dev_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int set_warn_dev_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	return write_warn_dev_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int set_warn_dev_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	UNUSED(ui16Id);
	UNUSED(psZbAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	UNUSED(ui16Id);
	UNUSED(psNwkAddr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	UNUSED(ui16Id);
	UNUSED(psEp);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_warn_dev_ext_attr (uint16_t ui16Id, const S_WarnDevExtAttr *psExtAttr)
{
	UNUSED(ui16Id);
	UNUSED(psExtAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_warn_dev_state (uint16_t ui16Id, const S_WarnDevState *psState)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_WARN_DEV_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_ON];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	S_WarnDevState sState;
	int iRet;

	ASSERT(NULL != psState);

	iRet = read_warn_dev_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_warn_dev_state(ui16Id, &sState);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = read_warn_dev_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}

	if (sState.warncfg.warnmode != psState->warncfg.warnmode || sState.warncfg.duration != psState->warncfg.duration)
	{
	#ifdef AT_PORT_UNQUEUE_QUERY
		iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psState->warncfg.warnmode, psState->warncfg.duration);
	#else
		iRet = zigbee_print_queue_query(MUI_DATA_TYPE_WARN_DEV, ui16Id, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psState->warncfg.warnmode, psState->warncfg.duration);
	#endif
	}

	sState.reachable 		= psState->reachable;
	sState.warncfg.warnmode = psState->warncfg.warnmode;
	sState.warncfg.duration = psState->warncfg.duration;

	iRet = write_warn_dev_state(ui16Id, &sState);
	if (iRet < 0)
	{
		return iRet;
	}

	return iRet;
}
int set_warn_dev_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_WARN_DEV_STATE_LEN];
	S_WarnCfg warncfg;
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_WARN_CFG];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != pui8WarnMode);

	iRet = read_warn_dev_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_warn_dev_state_duration(ui16Id, &warncfg.duration);
	if (iRet <= 0)
	{
		return iRet;
	}
	warncfg.warnmode = *pui8WarnMode;

	iRet = read_warn_dev_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}
#ifdef AT_PORT_UNQUEUE_QUERY
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, warncfg.warnmode, warncfg.duration);
#else
	iRet = zigbee_print_queue_query(MUI_DATA_TYPE_WARN_DEV, ui16Id, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, warncfg.warnmode, warncfg.duration);
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
	iRet = write_warn_dev_state_warn_mode(ui16Id, pui8WarnMode);
	if (iRet < 0)
	{
		return iRet;
	}

	return iRet;
}
int set_warn_dev_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_WARN_DEV_STATE_LEN];
	S_WarnCfg warncfg;
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_WARN_CFG];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != pui16Duration);

	iRet = read_warn_dev_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_warn_dev_state_warn_mode(ui16Id, &warncfg.warnmode);
	if (iRet <= 0)
	{
		return iRet;
	}
	warncfg.duration= *pui16Duration;

	iRet = read_warn_dev_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}
#ifdef AT_PORT_UNQUEUE_QUERY
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, warncfg.warnmode, warncfg.duration);
#else
	iRet = zigbee_print_queue_query(MUI_DATA_TYPE_WARN_DEV, ui16Id, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, warncfg.warnmode, warncfg.duration);
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
	iRet = write_warn_dev_state_duration(ui16Id, pui16Duration);
	if (iRet < 0)
	{
		return iRet;
	}

	return iRet;
}
int set_warn_dev_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_WARN_DEV_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_WARN_CFG];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != psWarnCfg);

	iRet = read_warn_dev_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_warn_dev_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}
#ifdef AT_PORT_UNQUEUE_QUERY
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psWarnCfg->warnmode, psWarnCfg->duration);
#else
	iRet = zigbee_print_queue_query(MUI_DATA_TYPE_WARN_DEV, ui16Id, AT_CMD_STWRN_S"=%s,%s,%02x,%04x\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psWarnCfg->warnmode, psWarnCfg->duration);
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
	iRet = write_warn_dev_state_warn_cfg(ui16Id, psWarnCfg);
	if (iRet < 0)
	{
		return iRet;
	}

	return iRet;
}
int set_warn_dev_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_warn_dev_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int rebind_warn_dev (uint16_t ui16Id)
{
	uint8_t ui8Reachable;
	ui8Reachable = 1;
	return write_warn_dev_state_reachable(ui16Id, &ui8Reachable);
}
int add_warn_dev (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, const S_WarnDevExtInfo *psExtInfo)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_WARN_DEV_ALL_LEN];
	char achAnswer[MUI_AT_ANSWER_LEN_ADDGR];
	U_DevAttrSave uDevAttr;
	U_DevStateSave uDevState;
	int iNewId, iRet;

	UNUSED(psExtInfo);

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psEp);

	//get warn_dev version
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ATTRRR_S"=%s,%s,0000,4000\r\n", psNwkAddr->str, psEp->str);
	if (iRet <= 0)
	{
		strncpy(uDevAttr.warn_dev_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.swver.str));
	}
	else
	{
	    iRet = at_cmd_answer_verify(achAnswer, iRet);
	    if (iRet != 0)
	    {
			strncpy(uDevAttr.warn_dev_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.swver.str));
	    }
		if (1!=sscanf((char*)achAnswer, "$ATTRRR:%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%s\r\n", uDevAttr.warn_dev_attr.api_attr.swver.str))
		{
			strncpy(uDevAttr.warn_dev_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.swver.str));
		}
	}

	//API attribute
	strncpy(uDevAttr.warn_dev_attr.api_attr.type.str, "Warning Device", M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.type.str));
  	sprintf(uDevAttr.warn_dev_attr.api_attr.name.str, "Warning Device ?");
	memset(uDevAttr.warn_dev_attr.api_attr.descr.str, 0, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.descr.str));
	strncpy(uDevAttr.warn_dev_attr.api_attr.modelid.str, psModelId->str, M_SIZEOF(U_DevAttrSave, warn_dev_attr.api_attr.modelid.str));
	uDevAttr.warn_dev_attr.api_attr.changerpt = 1;		//默认改变上报
	uDevAttr.warn_dev_attr.api_attr.autorpt   = 0;		//默认不定时上报

	//Zigbee attribute
	memcpy(uDevAttr.warn_dev_attr.zb_attr.nwkaddr.str, psNwkAddr->str, M_SIZEOF(U_DevAttrSave, warn_dev_attr.zb_attr.nwkaddr.str));
	memcpy(uDevAttr.warn_dev_attr.zb_attr.ep.str, psEp->str, M_SIZEOF(U_DevAttrSave, warn_dev_attr.zb_attr.ep.str));

	//Misc state
	uDevState.warn_dev_state.misc.new_dev   = 1;
	uDevState.warn_dev_state.misc.not_added = 1;

	//Zigbee attribute
	memcpy(&uDevState.warn_dev_state.zb_attr, &uDevAttr.warn_dev_attr.zb_attr, M_SIZEOF(U_DevStateSave, warn_dev_state.zb_attr));

	//Device state
	warn_dev_default_state(&uDevState.warn_dev_state.state);

	iNewId = dev_data_add(MUI_DATA_TYPE_WARN_DEV, &uDevAttr, sizeof(U_DevAttrSave), &uDevState, sizeof(U_DevStateSave));
	if (iNewId < 0)
	{
		return iNewId;
	}
  	sprintf(uDevAttr.warn_dev_attr.api_attr.name.str, "Warning Device %d", iNewId);
	write_warn_dev_attr_name(iNewId, &uDevAttr.warn_dev_attr.api_attr.name);

	//上报新增设备
	send_dev_report(MUI_HTTP_METHOD_EX_ADD, MUI_DATA_TYPE_WARN_DEV, iNewId, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);

	return iNewId;
}
int del_warn_dev (uint16_t ui16Id)
{
	S_DevIdx asDevIdx[MUI_AT_DEV_EP_NUM_MAX];	//!!!Chandler:此数组应该取决于系统一个MAC地址所关联设备的最大数,目前以switch设备为最多,可关联4个
	S_AttrModelId sModelId;
	int iRet;

	iRet = read_warn_dev_attr_modelid(ui16Id, &sModelId);
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = del_devs_data_by_modelid(&sModelId, asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));
	if (iRet <= 0)
	{
		return iRet;
	}

	return zigbee_dev_del(&sModelId);
}

