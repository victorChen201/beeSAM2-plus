#include "dev_if.h"
#include "level_ctrl.h"
#include "zigbee_manage.h"
#include "database.h"
#include "group.h"
#include "set.h"
#include "schedule.h"
#include "api_deal.h"
#include "report.h"
#include "auto_report.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define LEVEL_CTRL_DBG

#ifndef LEVEL_CTRL_PUTC
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_PUTC(Char)			PUTC(Char)
#else
#define LEVEL_CTRL_PUTC(Char)
#endif
#endif

#ifndef LEVEL_CTRL_PUTS
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define LEVEL_CTRL_PUTS(Str, Len)
#endif
#endif

#ifndef LEVEL_CTRL_PUTASCC
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_PUTASCC(Char)			PUTASCC(Char)
#else
#define LEVEL_CTRL_PUTASCC(Char)
#endif
#endif

#ifndef LEVEL_CTRL_PUTASCS
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define LEVEL_CTRL_PUTASCS(Str, Len)
#endif
#endif

#ifndef LEVEL_CTRL_PRINT
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define LEVEL_CTRL_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef LEVEL_CTRL_DYN_PUTC
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_LEVEL_CTRL|Dbg), Char)
#else
#define LEVEL_CTRL_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef LEVEL_CTRL_DYN_PUTS
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_LEVEL_CTRL|Dbg), Str, Len)
#else
#define LEVEL_CTRL_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef LEVEL_CTRL_DYN_PUTASCC
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_LEVEL_CTRL|Dbg), Char)
#else
#define LEVEL_CTRL_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef LEVEL_CTRL_DYN_PUTASCS
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_LEVEL_CTRL|Dbg), Str, Len)
#else
#define LEVEL_CTRL_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef LEVEL_CTRL_DYN_PRINT
#ifdef LEVEL_CTRL_DBG
#define LEVEL_CTRL_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_LEVEL_CTRL|Dbg), Fmt, ##Arg)
#else
#define LEVEL_CTRL_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif



#define MUI_REMOTE_REPORT_LEVEL_CTRL_ATTR_LEN	1024
#define MUI_REMOTE_REPORT_LEVEL_CTRL_STATE_LEN	1024
#define MUI_REMOTE_REPORT_LEVEL_CTRL_ALL_LEN		2048

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//static int level_ctrl_default_state (S_LevelCtrlStateRam *psState);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int level_ctrl_default_state (S_LevelCtrlState *psState)
{
	psState->reachable = 1;
	psState->level	 = 0;

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int init_level_ctrl_ram_data (uint16_t ui16Id, S_LevelCtrlStateRam *psState)
{
	ASSERT(NULL != psState);
	int iRet;
	iRet = database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.zb_attr), &psState->zb_attr, M_SIZEOF(S_LevelCtrlStateRam, zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		level_ctrl_default_state(&psState->state);
	}
	return iRet;
}
//--------------------------------------------------------------------------------------------------
int read_level_ctrl_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr), M_MEM_TYPE_FLASH);
}
int read_level_ctrl_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int read_level_ctrl_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int read_level_ctrl_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int read_level_ctrl_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int read_level_ctrl_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int read_level_ctrl_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int read_level_ctrl_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_level_ctrl_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, level_ctrl_state.zb_attr), M_MEM_TYPE_RAM);
}
int read_level_ctrl_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, level_ctrl_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
}
int read_level_ctrl_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, level_ctrl_state.zb_attr.ep), M_MEM_TYPE_RAM);
}
//.............................................................................
int read_level_ctrl_ext_attr (uint16_t ui16Id, S_LevelCtrlExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.ext_attr), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_level_ctrl_state (uint16_t ui16Id, S_LevelCtrlState *psState)
{
	ASSERT(NULL != psState);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.state), psState, M_SIZEOF(U_DevStateSave, level_ctrl_state.state), M_MEM_TYPE_RAM);
}
int read_level_ctrl_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, level_ctrl_state.state.reachable), M_MEM_TYPE_RAM);
}
int read_level_ctrl_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	ASSERT(NULL != pui16Level);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.state.level), pui16Level, M_SIZEOF(U_DevStateSave, level_ctrl_state.state.level), M_MEM_TYPE_RAM);
}
int read_level_ctrl_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_level_ctrl_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int read_level_ctrl_misc (uint16_t ui16Id, S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.misc), psMisc, M_SIZEOF(U_DevStateSave, level_ctrl_state.misc), M_MEM_TYPE_RAM);
}
int read_level_ctrl_misc_new_dev (uint16_t ui16Id, uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, level_ctrl_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int read_level_ctrl_misc_not_added (uint16_t ui16Id, uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, level_ctrl_state.misc.not_added), M_MEM_TYPE_RAM);
}
int read_level_ctrl_misc_at_err_tick (uint16_t ui16Id, uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_read(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, level_ctrl_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int write_level_ctrl_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr), M_MEM_TYPE_FLASH);
}
int write_level_ctrl_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int write_level_ctrl_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int write_level_ctrl_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int write_level_ctrl_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int write_level_ctrl_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int write_level_ctrl_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int write_level_ctrl_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_level_ctrl_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.zb_attr), psZbAttr, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, level_ctrl_state.zb_attr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_level_ctrl_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.zb_attr.nwkaddr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, level_ctrl_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_level_ctrl_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.zb_attr.ep), psEp, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.zb_attr.ep), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, level_ctrl_state.zb_attr.ep), M_MEM_TYPE_RAM);
	}
	return iRet;
}
//.............................................................................
int write_level_ctrl_ext_attr (uint16_t ui16Id, const S_LevelCtrlExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevAttrSave, level_ctrl_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.ext_attr), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_level_ctrl_state (uint16_t ui16Id, const S_LevelCtrlState *psState)
{
	ASSERT(NULL != psState);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.state), psState, M_SIZEOF(U_DevStateSave, level_ctrl_state.state), M_MEM_TYPE_RAM);
}
int write_level_ctrl_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, level_ctrl_state.state.reachable), M_MEM_TYPE_RAM);
}
int write_level_ctrl_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	ASSERT(NULL != pui16Level);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.state.level), pui16Level, M_SIZEOF(U_DevStateSave, level_ctrl_state.state.level), M_MEM_TYPE_RAM);
}
int write_level_ctrl_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_level_ctrl_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//................................................
int write_level_ctrl_misc (uint16_t ui16Id, const S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.misc), psMisc, M_SIZEOF(U_DevStateSave, level_ctrl_state.misc), M_MEM_TYPE_RAM);
}
int write_level_ctrl_misc_new_dev (uint16_t ui16Id, const uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, level_ctrl_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int write_level_ctrl_misc_not_added (uint16_t ui16Id, const uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, level_ctrl_state.misc.not_added), M_MEM_TYPE_RAM);
}
int write_level_ctrl_misc_at_err_tick (uint16_t ui16Id, const uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_write(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, M_OFFSET(U_DevStateSave, level_ctrl_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, level_ctrl_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int get_level_ctrl_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	return read_level_ctrl_attr(ui16Id, psApiAttr);
}
int get_level_ctrl_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	return read_level_ctrl_attr_type(ui16Id, psType);
}
int get_level_ctrl_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	return read_level_ctrl_attr_name(ui16Id, psName);
}
int get_level_ctrl_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	return read_level_ctrl_attr_descr(ui16Id, psDescr);
}
int get_level_ctrl_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	return read_level_ctrl_attr_modelid(ui16Id, psModelId);
}
int get_level_ctrl_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	return read_level_ctrl_attr_swver(ui16Id, psSwVer);
}
int get_level_ctrl_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	return read_level_ctrl_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int get_level_ctrl_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	return read_level_ctrl_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int get_level_ctrl_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	return read_level_ctrl_zb_attr(ui16Id, psZbAttr);
}
int get_level_ctrl_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	return read_level_ctrl_zb_attr_nwkaddr(ui16Id, psNwkAddr);
}
int get_level_ctrl_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	return read_level_ctrl_zb_attr_ep(ui16Id, psEp);
}
//.............................................................................
int get_level_ctrl_ext_attr (uint16_t ui16Id, S_LevelCtrlExtAttr *psExtAttr)
{
	return read_level_ctrl_ext_attr(ui16Id, psExtAttr);
}
//.............................................................................
int get_level_ctrl_state (uint16_t ui16Id, S_LevelCtrlState *psState)
{
	UNUSED(ui16Id);
	UNUSED(psState);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_level_ctrl_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//--------------------------------------------------------------------------------------------------
int set_level_ctrl_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	return write_level_ctrl_attr(ui16Id, psApiAttr);
}
int set_level_ctrl_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	UNUSED(ui16Id);
	UNUSED(psType);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	return write_level_ctrl_attr_name(ui16Id, psName);
}
int set_level_ctrl_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	return write_level_ctrl_attr_descr(ui16Id, psDescr);
}
int set_level_ctrl_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	UNUSED(ui16Id);
	UNUSED(psModelId);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	UNUSED(ui16Id);
	UNUSED(psSwVer);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	return write_level_ctrl_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int set_level_ctrl_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	return write_level_ctrl_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int set_level_ctrl_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	UNUSED(ui16Id);
	UNUSED(psZbAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	UNUSED(ui16Id);
	UNUSED(psNwkAddr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	UNUSED(ui16Id);
	UNUSED(psEp);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_level_ctrl_ext_attr (uint16_t ui16Id, const S_LevelCtrlExtAttr *psExtAttr)
{
	UNUSED(ui16Id);
	UNUSED(psExtAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_level_ctrl_state (uint16_t ui16Id, const S_LevelCtrlState *psState)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LEVEL_CTRL_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_ON];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	S_LevelCtrlState sState;
	int iRet;

	ASSERT(NULL != psState);

	iRet = read_level_ctrl_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_level_ctrl_state(ui16Id, &sState);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = read_level_ctrl_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}

	if (sState.level != psState->level)
	{
	#ifdef AT_PORT_UNQUEUE_QUERY
		iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOLV_S"=%s,%s,%02x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psState->level);
	#else
		iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, AT_CMD_MVTOLV_S"=%s,%s,%02x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, psState->level);
	#endif
	}

	sState.reachable = psState->reachable;
	sState.level	 = psState->level;

	iRet = write_level_ctrl_state(ui16Id, &sState);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = read_level_ctrl_attr_changerpt(ui16Id, &ui8ChangeRpt);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui8ChangeRpt != 0)
	{
		//上报
		send_dev_report(MUI_HTTP_METHOD_EX_MODIFY, MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);
	}

	return iRet;
}
int set_level_ctrl_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LEVEL_CTRL_STATE_LEN];
#ifdef AT_PORT_UNQUEUE_QUERY
	char achAnswer[MUI_AT_ANSWER_LEN_LEVEL];
#endif
	S_DevZbAttr sZbAttr;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Reachable;
	int iRet;

	ASSERT(NULL != pui16Level);

	iRet = read_level_ctrl_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (ui8Reachable == 0)
	{
		return DEV_IF_ERR_UNREACHABLE;
	}

	iRet = read_level_ctrl_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}
#ifdef AT_PORT_UNQUEUE_QUERY
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_MVTOLV_S"=%s,%s,%02x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, *pui16Level);
#else
	iRet = zigbee_print_queue_query(MUI_DATA_TYPE_LEVEL_CTRL, ui16Id, AT_CMD_MVTOLV_S"=%s,%s,%02x,000A\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str, *pui16Level);
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
	iRet = write_level_ctrl_state_level(ui16Id, pui16Level);
	if (iRet < 0)
	{
		return iRet;
	}

	return iRet;
}
int set_level_ctrl_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_level_ctrl_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int rebind_level_ctrl (uint16_t ui16Id)
{
	uint8_t ui8Reachable;
	ui8Reachable = 1;
	return write_level_ctrl_state_reachable(ui16Id, &ui8Reachable);
}
int add_level_ctrl (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, const S_LevelCtrlExtInfo *psExtInfo)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_LEVEL_CTRL_ALL_LEN];
	char achAnswer[MUI_AT_ANSWER_LEN_ADDGR];
	U_DevAttrSave uDevAttr;
	U_DevStateSave uDevState;
	int iNewId, iRet;

	UNUSED(psExtInfo);

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psEp);

	//get level_ctrl version
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ATTRRR_S"=%s,%s,0000,4000\r\n", psNwkAddr->str, psEp->str);
	if (iRet <= 0)
	{
		strncpy(uDevAttr.level_ctrl_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.swver.str));
	}
	else
	{
	    iRet = at_cmd_answer_verify(achAnswer, iRet);
	    if (iRet != 0)
	    {
			strncpy(uDevAttr.level_ctrl_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.swver.str));
	    }
		if (1!=sscanf((char*)achAnswer, "$ATTRRR:%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%s\r\n", uDevAttr.level_ctrl_attr.api_attr.swver.str))
		{
			strncpy(uDevAttr.level_ctrl_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.swver.str));
		}
	}

	//API attribute
	strncpy(uDevAttr.level_ctrl_attr.api_attr.type.str, "Level Controller", M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.type.str));
  	sprintf(uDevAttr.level_ctrl_attr.api_attr.name.str, "Level Controller ?");
	memset(uDevAttr.level_ctrl_attr.api_attr.descr.str, 0, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.descr.str));
	strncpy(uDevAttr.level_ctrl_attr.api_attr.modelid.str, psModelId->str, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.api_attr.modelid.str));
	uDevAttr.level_ctrl_attr.api_attr.changerpt = 1;		//默认改变上报
	uDevAttr.level_ctrl_attr.api_attr.autorpt   = 0;		//默认不定时上报

	//Zigbee attribute
	memcpy(uDevAttr.level_ctrl_attr.zb_attr.nwkaddr.str, psNwkAddr->str, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.zb_attr.nwkaddr.str));
	memcpy(uDevAttr.level_ctrl_attr.zb_attr.ep.str, psEp->str, M_SIZEOF(U_DevAttrSave, level_ctrl_attr.zb_attr.ep.str));

	//Misc state
	uDevState.level_ctrl_state.misc.new_dev   = 1;
	uDevState.level_ctrl_state.misc.not_added = 1;

	//Zigbee attribute
	memcpy(&uDevState.level_ctrl_state.zb_attr, &uDevAttr.level_ctrl_attr.zb_attr, M_SIZEOF(U_DevStateSave, level_ctrl_state.zb_attr));

	//Device state
	level_ctrl_default_state(&uDevState.level_ctrl_state.state);

	iNewId = dev_data_add(MUI_DATA_TYPE_LEVEL_CTRL, &uDevAttr, sizeof(U_DevAttrSave), &uDevState, sizeof(U_DevStateSave));
	if (iNewId < 0)
	{
		return iNewId;
	}
  	sprintf(uDevAttr.level_ctrl_attr.api_attr.name.str, "LevelCtrl %d", iNewId);
	write_level_ctrl_attr_name(iNewId, &uDevAttr.level_ctrl_attr.api_attr.name);

	//上报新增设备
	send_dev_report(MUI_HTTP_METHOD_EX_ADD, MUI_DATA_TYPE_LEVEL_CTRL, iNewId, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);

	return iNewId;
}
int del_level_ctrl (uint16_t ui16Id)
{
	S_DevIdx asDevIdx[MUI_AT_DEV_EP_NUM_MAX];	//!!!Chandler:此数组应该取决于系统一个MAC地址所关联设备的最大数,目前以switch设备为最多,可关联4个
	S_AttrModelId sModelId;
	int iRet;

	iRet = read_level_ctrl_attr_modelid(ui16Id, &sModelId);
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

