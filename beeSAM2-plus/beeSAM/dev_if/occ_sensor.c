#include "dev_if.h"
#include "occ_sensor.h"
#include "zigbee_manage.h"
#include "database.h"
#include "group.h"
#include "set.h"
#include "schedule.h"
#include "api_deal.h"
#include "report.h"
#include "auto_report.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define OCC_SENSOR_DBG

#ifndef OCC_SENSOR_PUTC
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_PUTC(Char)			PUTC(Char)
#else
#define OCC_SENSOR_PUTC(Char)
#endif
#endif

#ifndef OCC_SENSOR_PUTS
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define OCC_SENSOR_PUTS(Str, Len)
#endif
#endif

#ifndef OCC_SENSOR_PUTASCC
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_PUTASCC(Char)			PUTASCC(Char)
#else
#define OCC_SENSOR_PUTASCC(Char)
#endif
#endif

#ifndef OCC_SENSOR_PUTASCS
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define OCC_SENSOR_PUTASCS(Str, Len)
#endif
#endif

#ifndef OCC_SENSOR_PRINT
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define OCC_SENSOR_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef OCC_SENSOR_DYN_PUTC
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_OCC_SENSOR|Dbg), Char)
#else
#define OCC_SENSOR_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef OCC_SENSOR_DYN_PUTS
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_OCC_SENSOR|Dbg), Str, Len)
#else
#define OCC_SENSOR_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef OCC_SENSOR_DYN_PUTASCC
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_OCC_SENSOR|Dbg), Char)
#else
#define OCC_SENSOR_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef OCC_SENSOR_DYN_PUTASCS
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_OCC_SENSOR|Dbg), Str, Len)
#else
#define OCC_SENSOR_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef OCC_SENSOR_DYN_PRINT
#ifdef OCC_SENSOR_DBG
#define OCC_SENSOR_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_OCC_SENSOR|Dbg), Fmt, ##Arg)
#else
#define OCC_SENSOR_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

#define MUI_REMOTE_REPORT_OCC_SENSOR_ATTR_LEN	1024
#define MUI_REMOTE_REPORT_OCC_SENSOR_STATE_LEN	1024
#define MUI_REMOTE_REPORT_OCC_SENSOR_ALL_LEN	2048

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//static int occ_sensor_default_state (S_OccSensorStateRam *psState);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int occ_sensor_default_state (S_OccSensorState *psState)
{
	psState->reachable = 0;	//This value will be set to 1 when received REPORTU.
	psState->power	   = 255;
	psState->on		   = 0;
	psState->count	   = 0;

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int init_occ_sensor_ram_data (uint16_t ui16Id, S_OccSensorStateRam *psState)
{
	ASSERT(NULL != psState);
	int iRet;
	iRet = database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.zb_attr), &psState->zb_attr, M_SIZEOF(S_OccSensorStateRam, zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		occ_sensor_default_state(&psState->state);
	}
	return iRet;
}
//--------------------------------------------------------------------------------------------------
int read_occ_sensor_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr), M_MEM_TYPE_FLASH);
}
int read_occ_sensor_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int read_occ_sensor_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int read_occ_sensor_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int read_occ_sensor_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int read_occ_sensor_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int read_occ_sensor_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int read_occ_sensor_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_occ_sensor_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, occ_sensor_state.zb_attr), M_MEM_TYPE_RAM);
}
int read_occ_sensor_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, occ_sensor_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
}
int read_occ_sensor_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, occ_sensor_state.zb_attr.ep), M_MEM_TYPE_RAM);
}
//.............................................................................
int read_occ_sensor_ext_attr (uint16_t ui16Id, S_OccSensorExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.ext_attr), M_MEM_TYPE_FLASH);
}
//.............................................................................
int read_occ_sensor_state (uint16_t ui16Id, S_OccSensorState *psState)
{
	ASSERT(NULL != psState);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state), psState, M_SIZEOF(U_DevStateSave, occ_sensor_state.state), M_MEM_TYPE_RAM);
}
int read_occ_sensor_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.reachable), M_MEM_TYPE_RAM);
}
int read_occ_sensor_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	ASSERT(NULL != pui8On);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.on), pui8On, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.on), M_MEM_TYPE_RAM);
}
int read_occ_sensor_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	ASSERT(NULL != pui16Count);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.count), pui16Count, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.count), M_MEM_TYPE_RAM);
}
int read_occ_sensor_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int read_occ_sensor_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	ASSERT(NULL != pui8Power);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.power), pui8Power, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.power), M_MEM_TYPE_RAM);
}
//.............................................................................
int read_occ_sensor_misc (uint16_t ui16Id, S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.misc), psMisc, M_SIZEOF(U_DevStateSave, occ_sensor_state.misc), M_MEM_TYPE_RAM);
}
int read_occ_sensor_misc_new_dev (uint16_t ui16Id, uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, occ_sensor_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int read_occ_sensor_misc_not_added (uint16_t ui16Id, uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, occ_sensor_state.misc.not_added), M_MEM_TYPE_RAM);
}
int read_occ_sensor_misc_at_err_tick (uint16_t ui16Id, uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, occ_sensor_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int write_occ_sensor_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	ASSERT(NULL != psApiAttr);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr), psApiAttr, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr), M_MEM_TYPE_FLASH);
}
int write_occ_sensor_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	ASSERT(NULL != psType);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.type), psType, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.type), M_MEM_TYPE_FLASH);
}
int write_occ_sensor_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.name), psName, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.name), M_MEM_TYPE_FLASH);
}
int write_occ_sensor_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	ASSERT(NULL != psDescr);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.descr), psDescr, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.descr), M_MEM_TYPE_FLASH);
}
int write_occ_sensor_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	ASSERT(NULL != psModelId);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.modelid), psModelId, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.modelid), M_MEM_TYPE_FLASH);
}
int write_occ_sensor_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	ASSERT(NULL != psSwVer);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.swver), psSwVer, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.swver), M_MEM_TYPE_FLASH);
}
int write_occ_sensor_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	ASSERT(NULL != pui8ChangeRpt);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.changerpt), pui8ChangeRpt, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.changerpt), M_MEM_TYPE_FLASH);
}
int write_occ_sensor_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	ASSERT(NULL != pui16AutoRpt);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.api_attr.autorpt), pui16AutoRpt, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.autorpt), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_occ_sensor_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	ASSERT(NULL != psZbAttr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.zb_attr), psZbAttr, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.zb_attr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.zb_attr), psZbAttr, M_SIZEOF(U_DevStateSave, occ_sensor_state.zb_attr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_occ_sensor_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	ASSERT(NULL != psNwkAddr);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.zb_attr.nwkaddr), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.zb_attr.nwkaddr), psNwkAddr, M_SIZEOF(U_DevStateSave, occ_sensor_state.zb_attr.nwkaddr), M_MEM_TYPE_RAM);
	}
	return iRet;
}
int write_occ_sensor_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	ASSERT(NULL != psEp);
	int iRet;
	iRet = database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.zb_attr.ep), psEp, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.zb_attr.ep), M_MEM_TYPE_FLASH);
	if (iRet > 0)
	{
		iRet = database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.zb_attr.ep), psEp, M_SIZEOF(U_DevStateSave, occ_sensor_state.zb_attr.ep), M_MEM_TYPE_RAM);
	}
	return iRet;
}
//.............................................................................
int write_occ_sensor_ext_attr (uint16_t ui16Id, const S_OccSensorExtAttr *psExtAttr)
{
	ASSERT(NULL != psExtAttr);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevAttrSave, occ_sensor_attr.ext_attr), psExtAttr, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.ext_attr), M_MEM_TYPE_FLASH);
}
//.............................................................................
int write_occ_sensor_state (uint16_t ui16Id, const S_OccSensorState *psState)
{
	ASSERT(NULL != psState);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state), psState, M_SIZEOF(U_DevStateSave, occ_sensor_state.state), M_MEM_TYPE_RAM);
}
int write_occ_sensor_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	ASSERT(NULL != pui8Reachable);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.reachable), pui8Reachable, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.reachable), M_MEM_TYPE_RAM);
}
int write_occ_sensor_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	uint8_t ui8On;
	uint16_t ui16Count;
	int iRet;
	ASSERT(NULL != pui8On);
	iRet = database_read(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.on), &ui8On, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.on), M_MEM_TYPE_RAM);
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.on), pui8On, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.on), M_MEM_TYPE_RAM);
	if (iRet < 0)
	{
		return iRet;
	}
	if ((0==ui8On) && (0!=*pui8On))
	{
		iRet = read_occ_sensor_state_count(ui16Id, &ui16Count);
		if (iRet < 0)
		{
			return iRet;
		}
		ui16Count++;
		iRet = write_occ_sensor_state_count(ui16Id, &ui16Count);
	}
	return iRet;
}
int write_occ_sensor_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	ASSERT(NULL != pui16Count);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.count), pui16Count, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.count), M_MEM_TYPE_RAM);
}
int write_occ_sensor_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int write_occ_sensor_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	ASSERT(NULL != pui8Power);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.state.power), pui8Power, M_SIZEOF(U_DevStateSave, occ_sensor_state.state.power), M_MEM_TYPE_RAM);
}
//................................................
int write_occ_sensor_misc (uint16_t ui16Id, const S_DevMiscState *psMisc)
{
	ASSERT(NULL != psMisc);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.misc), psMisc, M_SIZEOF(U_DevStateSave, occ_sensor_state.misc), M_MEM_TYPE_RAM);
}
int write_occ_sensor_misc_new_dev (uint16_t ui16Id, const uint8_t *pui8NewDev)
{
	ASSERT(NULL != pui8NewDev);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.misc.new_dev), pui8NewDev, M_SIZEOF(U_DevStateSave, occ_sensor_state.misc.new_dev), M_MEM_TYPE_RAM);
}
int write_occ_sensor_misc_not_added (uint16_t ui16Id, const uint8_t *pui8NotAdded)
{
	ASSERT(NULL != pui8NotAdded);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.misc.not_added), pui8NotAdded, M_SIZEOF(U_DevStateSave, occ_sensor_state.misc.not_added), M_MEM_TYPE_RAM);
}
int write_occ_sensor_misc_at_err_tick (uint16_t ui16Id, const uint8_t *pui8AtErrTick)
{
	ASSERT(NULL != pui8AtErrTick);
	return database_write(MUI_DATA_TYPE_OCC_SENSOR, ui16Id, M_OFFSET(U_DevStateSave, occ_sensor_state.misc.at_err_tick), pui8AtErrTick, M_SIZEOF(U_DevStateSave, occ_sensor_state.misc.at_err_tick), M_MEM_TYPE_RAM);
}
//--------------------------------------------------------------------------------------------------
int get_occ_sensor_attr (uint16_t ui16Id, S_DevApiAttr *psApiAttr)
{
	return read_occ_sensor_attr(ui16Id, psApiAttr);
}
int get_occ_sensor_attr_type (uint16_t ui16Id, S_AttrType *psType)
{
	return read_occ_sensor_attr_type(ui16Id, psType);
}
int get_occ_sensor_attr_name (uint16_t ui16Id, S_AttrName *psName)
{
	return read_occ_sensor_attr_name(ui16Id, psName);
}
int get_occ_sensor_attr_descr (uint16_t ui16Id, S_AttrDescr *psDescr)
{
	return read_occ_sensor_attr_descr(ui16Id, psDescr);
}
int get_occ_sensor_attr_modelid (uint16_t ui16Id, S_AttrModelId *psModelId)
{
	return read_occ_sensor_attr_modelid(ui16Id, psModelId);
}
int get_occ_sensor_attr_swver (uint16_t ui16Id, S_AttrSwVer *psSwVer)
{
	return read_occ_sensor_attr_swver(ui16Id, psSwVer);
}
int get_occ_sensor_attr_changerpt (uint16_t ui16Id, uint8_t *pui8ChangeRpt)
{
	return read_occ_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int get_occ_sensor_attr_autorpt (uint16_t ui16Id, uint16_t *pui16AutoRpt)
{
	return read_occ_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int get_occ_sensor_zb_attr (uint16_t ui16Id, S_DevZbAttr *psZbAttr)
{
	return read_occ_sensor_zb_attr(ui16Id, psZbAttr);
}
int get_occ_sensor_zb_attr_nwkaddr (uint16_t ui16Id, S_ZbAttrNwkAddr *psNwkAddr)
{
	return read_occ_sensor_zb_attr_nwkaddr(ui16Id, psNwkAddr);
}
int get_occ_sensor_zb_attr_ep (uint16_t ui16Id, S_ZbAttrEp *psEp)
{
	return read_occ_sensor_zb_attr_ep(ui16Id, psEp);
}
//.............................................................................
int get_occ_sensor_ext_attr (uint16_t ui16Id, S_OccSensorExtAttr *psExtAttr)
{
	return read_occ_sensor_ext_attr(ui16Id, psExtAttr);
}
//.............................................................................
int get_occ_sensor_state (uint16_t ui16Id, S_OccSensorState *psState)
{
	UNUSED(ui16Id);
	UNUSED(psState);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_reachable (uint16_t ui16Id, uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_on (uint16_t ui16Id, uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_bri (uint16_t ui16Id, uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_sat (uint16_t ui16Id, uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_hue (uint16_t ui16Id, uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_ct (uint16_t ui16Id, uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_xy (uint16_t ui16Id, S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_colormode (uint16_t ui16Id, uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_alert (uint16_t ui16Id, uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_effect (uint16_t ui16Id, uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_level (uint16_t ui16Id, uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_count (uint16_t ui16Id, uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_temp (uint16_t ui16Id, uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_hum (uint16_t ui16Id, uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_consumption (uint16_t ui16Id, uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_warn_mode (uint16_t ui16Id, uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_duration (uint16_t ui16Id, uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_warn_cfg (uint16_t ui16Id, S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_action (uint16_t ui16Id, uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int get_occ_sensor_state_power (uint16_t ui16Id, uint8_t *pui8Power)
{
	char achAnswer[MUI_AT_ANSWER_LEN_POWER];
	S_DevZbAttr sZbAttr;
	int iRet, iLen;
	char *pch;

	ASSERT(NULL != pui8Power);

	iRet = read_occ_sensor_zb_attr(ui16Id, &sZbAttr);
	if (iRet <= 0)
	{
		return iRet;
	}
	iLen = zigbee_print_query(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, AT_CMD_ATTRRR_S"=%s,%s,0001,0021\r\n", sZbAttr.nwkaddr.str, sZbAttr.ep.str);
	if (iLen <= 0)
	{
		return iLen;
	}

	iRet = at_cmd_answer_verify(achAnswer, iLen);
	if (iRet < 0)
	{
		return iRet;
	}

	pch = strnstr(achAnswer, MSTR_AT_HEADER_ATTRRR, iLen);
	if (NULL == pch)
	{
		return AT_PORT_ERR_UNEXPECT_ANSWER;
	}
	if (1 != sscanf(pch, MSTR_AT_HEADER_ATTRRR"%*4s,%*2s,%*4s,%*4s,%02hhx\r\n", pui8Power))
	{
		return AT_PORT_ERR_UNEXPECT_ANSWER;
	}
	iRet = write_occ_sensor_state_power(ui16Id, pui8Power);

	return iRet;
}
//--------------------------------------------------------------------------------------------------
int set_occ_sensor_attr (uint16_t ui16Id, const S_DevApiAttr *psApiAttr)
{
	return write_occ_sensor_attr(ui16Id, psApiAttr);
}
int set_occ_sensor_attr_type (uint16_t ui16Id, const S_AttrType *psType)
{
	UNUSED(ui16Id);
	UNUSED(psType);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_attr_name (uint16_t ui16Id, const S_AttrName *psName)
{
	return write_occ_sensor_attr_name(ui16Id, psName);
}
int set_occ_sensor_attr_descr (uint16_t ui16Id, const S_AttrDescr *psDescr)
{
	return write_occ_sensor_attr_descr(ui16Id, psDescr);
}
int set_occ_sensor_attr_modelid (uint16_t ui16Id, const S_AttrModelId *psModelId)
{
	UNUSED(ui16Id);
	UNUSED(psModelId);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_attr_swver (uint16_t ui16Id, const S_AttrSwVer *psSwVer)
{
	UNUSED(ui16Id);
	UNUSED(psSwVer);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_attr_changerpt (uint16_t ui16Id, const uint8_t *pui8ChangeRpt)
{
	return write_occ_sensor_attr_changerpt(ui16Id, pui8ChangeRpt);
}
int set_occ_sensor_attr_autorpt (uint16_t ui16Id, const uint16_t *pui16AutoRpt)
{
	return write_occ_sensor_attr_autorpt(ui16Id, pui16AutoRpt);
}
//.............................................................................
int set_occ_sensor_zb_attr (uint16_t ui16Id, const S_DevZbAttr *psZbAttr)
{
	UNUSED(ui16Id);
	UNUSED(psZbAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_zb_attr_nwkaddr (uint16_t ui16Id, const S_ZbAttrNwkAddr *psNwkAddr)
{
	UNUSED(ui16Id);
	UNUSED(psNwkAddr);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_zb_attr_ep (uint16_t ui16Id, const S_ZbAttrEp *psEp)
{
	UNUSED(ui16Id);
	UNUSED(psEp);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_occ_sensor_ext_attr (uint16_t ui16Id, const S_OccSensorExtAttr *psExtAttr)
{
	UNUSED(ui16Id);
	UNUSED(psExtAttr);
	return DEV_IF_ERR_UNSUPPORT;
}
//.............................................................................
int set_occ_sensor_state (uint16_t ui16Id, const S_OccSensorState *psState)
{
	UNUSED(ui16Id);
	UNUSED(psState);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_reachable (uint16_t ui16Id, const uint8_t *pui8Reachable)
{
	UNUSED(ui16Id);
	UNUSED(pui8Reachable);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	UNUSED(ui16Id);
	UNUSED(pui8On);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_bri (uint16_t ui16Id, const uint8_t *pui8Bri)
{
	UNUSED(ui16Id);
	UNUSED(pui8Bri);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_sat (uint16_t ui16Id, const uint8_t *pui8Sat)
{
	UNUSED(ui16Id);
	UNUSED(pui8Sat);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_hue (uint16_t ui16Id, const uint16_t *pui16Hue)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hue);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_ct (uint16_t ui16Id, const uint16_t *pui16Ct)
{
	UNUSED(ui16Id);
	UNUSED(pui16Ct);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_xy (uint16_t ui16Id, const S_Color_XY *psXY)
{
	UNUSED(ui16Id);
	UNUSED(psXY);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_colormode (uint16_t ui16Id, const uint8_t *pui8ColorMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8ColorMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_alert (uint16_t ui16Id, const uint8_t *pui8Alert)
{
	UNUSED(ui16Id);
	UNUSED(pui8Alert);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_effect (uint16_t ui16Id, const uint8_t *pui8Effect)
{
	UNUSED(ui16Id);
	UNUSED(pui8Effect);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_level (uint16_t ui16Id, const uint16_t *pui16Level)
{
	UNUSED(ui16Id);
	UNUSED(pui16Level);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	UNUSED(ui16Id);
	UNUSED(pui16Count);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_temp (uint16_t ui16Id, const uint16_t *pui16Temp)
{
	UNUSED(ui16Id);
	UNUSED(pui16Temp);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_hum (uint16_t ui16Id, const uint16_t *pui16Hum)
{
	UNUSED(ui16Id);
	UNUSED(pui16Hum);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_consumption (uint16_t ui16Id, const uint16_t *pui16Consumption)
{
	UNUSED(ui16Id);
	UNUSED(pui16Consumption);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_warn_mode (uint16_t ui16Id, const uint8_t *pui8WarnMode)
{
	UNUSED(ui16Id);
	UNUSED(pui8WarnMode);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_duration (uint16_t ui16Id, const uint16_t *pui16Duration)
{
	UNUSED(ui16Id);
	UNUSED(pui16Duration);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_warn_cfg (uint16_t ui16Id, const S_WarnCfg *psWarnCfg)
{
	UNUSED(ui16Id);
	UNUSED(psWarnCfg);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_action (uint16_t ui16Id, const uint8_t *pui8Action)
{
	UNUSED(ui16Id);
	UNUSED(pui8Action);
	return DEV_IF_ERR_UNSUPPORT;
}
int set_occ_sensor_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	UNUSED(ui16Id);
	UNUSED(pui8Power);
	return DEV_IF_ERR_UNSUPPORT;
}
//--------------------------------------------------------------------------------------------------
int update_occ_sensor_state_on (uint16_t ui16Id, const uint8_t *pui8On)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_OCC_SENSOR_STATE_LEN];
	uint8_t ui8Reachable;
	uint8_t ui8On;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Changed = 0;
	int iRet;

	ASSERT(NULL != pui8On);

	iRet = read_occ_sensor_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 == ui8Reachable)
	{
		ui8Reachable = 1;
		iRet = write_occ_sensor_state_reachable(ui16Id, &ui8Reachable);
		if (iRet < 0)
		{
			return iRet;
		}
		ui8Changed = 1;
	}

	iRet = read_occ_sensor_state_on(ui16Id, &ui8On);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui8On != *pui8On)
	{
		iRet = write_occ_sensor_state_on(ui16Id, pui8On);
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

	iRet = read_occ_sensor_attr_changerpt(ui16Id, &ui8ChangeRpt);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui8ChangeRpt != 0)
	{
		//上报
		send_dev_state_report(MUI_HTTP_METHOD_EX_MODIFY, MUI_DATA_TYPE_OCC_SENSOR, ui16Id, MUI_DEV_STATE_ON);
	}

	return iRet;
}
int update_occ_sensor_state_count (uint16_t ui16Id, const uint16_t *pui16Count)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_OCC_SENSOR_STATE_LEN];
	uint16_t ui16Count;
	uint8_t ui8Reachable;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Changed = 0;
	int iRet;

	ASSERT(NULL != pui16Count);

	iRet = read_occ_sensor_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 == ui8Reachable)
	{
		ui8Reachable = 1;
		iRet = write_occ_sensor_state_reachable(ui16Id, &ui8Reachable);
		if (iRet < 0)
		{
			return iRet;
		}
		ui8Changed = 1;
	}

	iRet = read_occ_sensor_state_count(ui16Id, &ui16Count);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui16Count != *pui16Count)
	{
		iRet = write_occ_sensor_state_count(ui16Id, pui16Count);
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

	iRet = read_occ_sensor_attr_changerpt(ui16Id, &ui8ChangeRpt);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui8ChangeRpt != 0)
	{
		//上报
		send_dev_state_report(MUI_HTTP_METHOD_EX_MODIFY, MUI_DATA_TYPE_OCC_SENSOR, ui16Id, MUI_DEV_STATE_COUNT);
	}

	return iRet;
}
int update_occ_sensor_state_power (uint16_t ui16Id, const uint8_t *pui8Power)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_OCC_SENSOR_STATE_LEN];
	uint8_t ui8Reachable;
	uint8_t ui8Power;
	uint8_t ui8ChangeRpt;
	uint8_t ui8Changed = 0;
	int iRet;

	ASSERT(NULL != pui8Power);

	iRet = read_occ_sensor_state_reachable(ui16Id, &ui8Reachable);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 == ui8Reachable)
	{
		ui8Reachable = 1;
		iRet = write_occ_sensor_state_reachable(ui16Id, &ui8Reachable);
		if (iRet < 0)
		{
			return iRet;
		}
		ui8Changed = 1;
	}

	iRet = read_occ_sensor_state_power(ui16Id, &ui8Power);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui8Power != *pui8Power)
	{
		iRet = write_occ_sensor_state_power(ui16Id, pui8Power);
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

	iRet = read_occ_sensor_attr_changerpt(ui16Id, &ui8ChangeRpt);
	if (iRet < 0)
	{
		return iRet;
	}

	if (ui8ChangeRpt != 0)
	{
		//上报
		send_dev_state_report(MUI_HTTP_METHOD_EX_MODIFY, MUI_DATA_TYPE_OCC_SENSOR, ui16Id, MUI_DEV_STATE_POWER);
	}

	return iRet;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
int rebind_occ_sensor (uint16_t ui16Id)
{
	char achAnswer[MUI_AT_ANSWER_LEN_BIND];
	S_AttrModelId sModelId;
	S_ZbAttrEp sEp;
	uint8_t ui8Reachable;
	int iRet;

	iRet = read_occ_sensor_attr_modelid(ui16Id, &sModelId);
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = read_occ_sensor_zb_attr_ep(ui16Id, &sEp);
	if (iRet < 0)
	{
		return iRet;
	}

	//Binding Occupancy Sensor
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_BIND_S"=01,01,0f,%s,%s,%04x\r\n", sModelId.str, sEp.str, OCC_SENSOR_CLUSTER_ID);
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_BIND_S"=01,01,0f,%s,%s,%04x\r\n", sModelId.str, sEp.str, POWER_CONFIG_CLUSTER_ID);
	if (iRet <= 0)
	{
		return iRet;
	}

	ui8Reachable = 1;
	iRet = write_occ_sensor_state_reachable(ui16Id, &ui8Reachable);
	return iRet;
}
int add_occ_sensor (const S_AttrModelId *psModelId, const S_ZbAttrNwkAddr *psNwkAddr, const S_ZbAttrEp *psEp, const S_OccSensorExtInfo *psExtInfo)
{
	uint8_t aui8Buf[MUI_REMOTE_REPORT_OCC_SENSOR_ALL_LEN];
	char achAnswer[MUI_AT_ANSWER_LEN_BIND];
	U_DevAttrSave uDevAttr;
	U_DevStateSave uDevState;
	int iNewId, iRet;

	UNUSED(psExtInfo);

	ASSERT(NULL != psModelId);
	ASSERT(NULL != psNwkAddr);
	ASSERT(NULL != psEp);

	//get occ_sensor version
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_ATTRRR_S"=%s,%s,0000,4000\r\n", psNwkAddr->str, psEp->str);
	if (iRet <= 0)
	{
		strncpy(uDevAttr.occ_sensor_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.swver.str));
	}
	else
	{
	    iRet = at_cmd_answer_verify(achAnswer, iRet);
	    if (iRet != 0)
	    {
			strncpy(uDevAttr.occ_sensor_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.swver.str));
	    }
		if (1!=sscanf((char*)achAnswer, "$ATTRRR:%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%s\r\n", uDevAttr.occ_sensor_attr.api_attr.swver.str))
		{
			strncpy(uDevAttr.occ_sensor_attr.api_attr.swver.str, "Unknow", M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.swver.str));
		}
	}

	//Binding Occupancy Sensor
	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_BIND_S"=01,01,0f,%s,%s,%04x\r\n", psModelId->str, psEp->str, OCC_SENSOR_CLUSTER_ID);
	if (iRet <= 0)
	{
		return iRet;
	}

	iRet = zigbee_print_query_repeat(achAnswer, sizeof(achAnswer), MUI_AT_QUERY_WAIT_NORMAL_MAX, 2, MUI_AT_QUERY_INTERVAL_MIN,
									 AT_CMD_BIND_S"=01,01,0f,%s,%s,%04x\r\n", psModelId->str, psEp->str, POWER_CONFIG_CLUSTER_ID);
	if (iRet <= 0)
	{
		return iRet;
	}

	//API attribute
	strncpy(uDevAttr.occ_sensor_attr.api_attr.type.str, "Occupancy Sensor", M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.type.str));
  	sprintf(uDevAttr.occ_sensor_attr.api_attr.name.str, "Occupancy Sensor ?");
	memset(uDevAttr.occ_sensor_attr.api_attr.descr.str, 0, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.descr.str));
	strncpy(uDevAttr.occ_sensor_attr.api_attr.modelid.str, psModelId->str, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.api_attr.modelid.str));
	uDevAttr.occ_sensor_attr.api_attr.changerpt = 1;		//默认改变上报
	uDevAttr.occ_sensor_attr.api_attr.autorpt   = 0;		//默认不定时上报

	//Zigbee attribute
	memcpy(uDevAttr.occ_sensor_attr.zb_attr.nwkaddr.str, psNwkAddr->str, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.zb_attr.nwkaddr.str));
	memcpy(uDevAttr.occ_sensor_attr.zb_attr.ep.str, psEp->str, M_SIZEOF(U_DevAttrSave, occ_sensor_attr.zb_attr.ep.str));

	//Misc state
	uDevState.occ_sensor_state.misc.new_dev   = 1;
	uDevState.occ_sensor_state.misc.not_added = 1;

	//Zigbee attribute
	memcpy(&uDevState.occ_sensor_state.zb_attr, &uDevAttr.occ_sensor_attr.zb_attr, M_SIZEOF(U_DevStateSave, occ_sensor_state.zb_attr));

	//Device state
	occ_sensor_default_state(&uDevState.occ_sensor_state.state);

	iNewId = dev_data_add(MUI_DATA_TYPE_OCC_SENSOR, &uDevAttr, sizeof(U_DevAttrSave), &uDevState, sizeof(U_DevStateSave));
	if (iNewId < 0)
	{
		return iNewId;
	}
  	sprintf(uDevAttr.occ_sensor_attr.api_attr.name.str, "Occupancy Sensor %d", iNewId);
	write_occ_sensor_attr_name(iNewId, &uDevAttr.occ_sensor_attr.api_attr.name);

	//上报新增设备
	send_dev_report(MUI_HTTP_METHOD_EX_ADD, MUI_DATA_TYPE_OCC_SENSOR, iNewId, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);

	return iNewId;
}
int del_occ_sensor (uint16_t ui16Id)
{
	S_DevIdx asDevIdx[MUI_AT_DEV_EP_NUM_MAX];	//!!!Chandler:此数组应该取决于系统一个MAC地址所关联设备的最大数,目前以switch设备为最多,可关联4个
	S_AttrModelId sModelId;
	int iRet;

	iRet = read_occ_sensor_attr_modelid(ui16Id, &sModelId);
	if (iRet < 0)
	{
		OCC_SENSOR_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[del_occ_sensor] Call : read_occ_sensor_attr_modelid failed!:%d\r\n", iRet);
		return iRet;
	}
	iRet = del_devs_data_by_modelid(&sModelId, asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));
	if (iRet <= 0)
	{
		OCC_SENSOR_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[del_occ_sensor] Call : del_devs_data_by_modelid failed!:%d\r\n", iRet);
		return iRet;
	}

	return zigbee_dev_del(&sModelId);
}

