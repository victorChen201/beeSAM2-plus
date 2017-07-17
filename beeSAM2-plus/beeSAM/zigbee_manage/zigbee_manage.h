//#define	__M_FILE_NAME_zigbee_manage_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_zigbee_manage_H__
#define	__M_FILE_NAME_zigbee_manage_H__


#include "common.h"
#include "sys_time.h"
#include "at_port.h"
#include "data_common.h"
#include "uzipupgrade_xmoedm.h"
/*
	return: -1 : error
		0  : no network
		1  : have network already

*/

#define SUPPORT_PROFILE_ID_1            0x0104
#define ON_OFF_SWITCH_DEVICE_ID        	0x0000
#define LEVEL_CTRL_SWITCH_DEVICE_ID     0x0001
#define LEVEL_CTRL_OUTPUT_DEVICE_ID     0x0003
#define REMOTE_CTRL_DEVICE_ID        	0x0006
#define MA_POWER_DEVICE_ID        		0x0009
#define SIMPLE_SENSOR_DEVICE_ID        	0x000C
#define SMART_PLUG_DEVICE_ID        	0x0051
#define ON_OFF_LIGHT_DEVICE_ID        	0x0100
#define DIMM_LIGHT_DEVICE_ID        	0x0101
#define COLOR_DIMM_LIGHT_DEVICE_ID      0x0102
#define ON_OFF_LIGHT_SWITCH_DEVICE_ID	0x0103
#define DIMM_SWITCH_DEVICE_ID    		0x0104
#define COLOR_DIMM_SWITCH_DEVICE_ID    	0x0105
#define OCCUP_SENSOR_DEVICE_ID    		0x0107
#define SHADE_DEVICE_ID    				0x0200
#define THERMOSTAT_DEVICE_ID    		0x0301
#define TEMPER_DEVICE_ID    			0x0302
#define RULE_CTRL_INDI_DEVICE_ID    	0x0400
#define RULE_ANCILL_CTRL_DEVICE_ID    	0x0401
#define RULE_ZONE_DEVICE_ID    			0x0402
#define RULE_WARN_DEVICE_ID    			0x0403

#define SUPPORT_PROFILE_ID_2            0xC05E
#define ON_OFF_PLUG_DEVICE_ID           0x0010
#define DIMM_PLUG_DEVICE_ID             0x0110
#define COLOR_LIGHT_DEVICE_ID           0x0200
#define EXCOLOR_LIGHT_DEVICE_ID        	0x0210
#define COLOR_TEMP_DEVICE_ID			0x0220

#define POWER_CONFIG_CLUSTER_ID			0x0001
#define ON_OFF_CLUSTER_ID    			0x0006
#define LEVEL_CLUSTER_ID    			0x0008
#define OCC_SENSOR_CLUSTER_ID    		0x0406
#define BIN_SENSOR_CLUSTER_ID    		0x000F
#define TEMP_SENSOR_CLUSTER_ID    		0x0402
#define HUM_SENSOR_CLUSTER_ID    		0x0405

#define OCC_SENSOR_EP          			"0D"
#define BIN_SENSOR_EP            		"10"
#define TH_SENSOR_EP              		"11"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int zigbee_manager_init (const char *pchDev);
extern void zigbee_module_ready (void);
extern int zigbee_event_checkout (uint8_t *pui8Buf, size_t tLen, uint8_t ui8EventType);
extern int zigbee_query (const void *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs);
extern int zigbee_query_repeat (const char *pvCmd, size_t tCmdLen, void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs,
								uint32_t ui32Repeat, uint32_t ui32DlyMs);
extern int zigbee_print_query (void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs, const char *pchFmt, ...);
extern int zigbee_print_query_repeat (void *pvBuf, size_t tBufSize, uint32_t ui32WaitMs, uint32_t ui32Repeat, uint32_t ui32DlyMs,
									  const char *pchFmt, ...);
extern int zigbee_queue_query(uint16_t ui16Type, uint16_t ui16Id, const void *pvCmd, size_t tCmdLen);
extern int zigbee_print_queue_query (uint16_t ui16Type, uint16_t ui16Id, const char *pchFmt, ...);
extern int zigbee_join_continue(void);
extern int zigbee_join_time(uint8_t ui8Mode, uint16_t ui16Duration);
extern int zigbee_dev_del(const S_AttrModelId *psModelId);
extern int zigbee_new_dev_deal (const void *pvEvent, size_t tLen);
extern int zigbee_report_deal (const void *pvEvent, size_t tLen);
extern int zigbee_zereq_deal (const void *pvEvent, size_t tLen);
extern int zigbee_zschn_deal (const void *pvEvent, size_t tLen);
extern int zigbee_dev_left_deal (const void *pvEvent, size_t tLen);
extern int zigbee_debug_info_deal (const void *pvEvent, size_t tLen);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_zigbee_manage_H__".
