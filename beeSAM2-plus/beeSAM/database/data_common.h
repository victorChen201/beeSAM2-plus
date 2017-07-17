//#define	__M_FILE_NAME_dev_common_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_dev_common_H__
#define	__M_FILE_NAME_dev_common_H__

#include "common.h"

//-------------------------------------------------------------------------------------------------*
//General
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_DATA_TYPE_START					0
//-------------------------------------------
#define MUI_DATA_TYPE_CFG_START				MUI_DATA_TYPE_START
#define MUI_DATA_TYPE_CONFIG				(MUI_DATA_TYPE_CFG_START+0)
#define MUI_DATA_TYPE_USER					(MUI_DATA_TYPE_CFG_START+1)
#define MUI_DATA_TYPE_GROUP					(MUI_DATA_TYPE_CFG_START+2)
#define MUI_DATA_TYPE_SCENE					(MUI_DATA_TYPE_CFG_START+3)
#define MUI_DATA_TYPE_SET					(MUI_DATA_TYPE_CFG_START+4)
#define MUI_DATA_TYPE_SCHED					(MUI_DATA_TYPE_CFG_START+5)
#define MUI_DATA_TYPE_RULE					(MUI_DATA_TYPE_CFG_START+6)
#define MUI_DATA_TYPE_CFG_END				MUI_DATA_TYPE_RULE
//...........................................
#define MUI_DATA_TYPE_DEV_START				(MUI_DATA_TYPE_CFG_END+1)
#define MUI_DATA_TYPE_LIGHT					(MUI_DATA_TYPE_DEV_START+0)
#define MUI_DATA_TYPE_OCC_SENSOR			(MUI_DATA_TYPE_DEV_START+1)
#define MUI_DATA_TYPE_BIN_SENSOR			(MUI_DATA_TYPE_DEV_START+2)
#define MUI_DATA_TYPE_TEMP_SENSOR			(MUI_DATA_TYPE_DEV_START+3)
#define MUI_DATA_TYPE_HUM_SENSOR			(MUI_DATA_TYPE_DEV_START+4)
#define MUI_DATA_TYPE_TH_SENSOR				(MUI_DATA_TYPE_DEV_START+5)
#define MUI_DATA_TYPE_BIN_CTRL				(MUI_DATA_TYPE_DEV_START+6)
#define MUI_DATA_TYPE_LEVEL_CTRL			(MUI_DATA_TYPE_DEV_START+7)
#define MUI_DATA_TYPE_WARN_DEV				(MUI_DATA_TYPE_DEV_START+8)
#define MUI_DATA_TYPE_PLUG					(MUI_DATA_TYPE_DEV_START+9)
#define MUI_DATA_TYPE_SWITCH				(MUI_DATA_TYPE_DEV_START+10)
#define MUI_DATA_TYPE_SHADE					(MUI_DATA_TYPE_DEV_START+11)
#define MUI_DATA_TYPE_SONOS                	(MUI_DATA_TYPE_DEV_START+12)
#define MUI_DATA_TYPE_PANEL             	(MUI_DATA_TYPE_DEV_START+13)
#define MUI_DATA_TYPE_RESERVE1             	(MUI_DATA_TYPE_DEV_START+14)
#define MUI_DATA_TYPE_RESERVE2             	(MUI_DATA_TYPE_DEV_START+15)
#define MUI_DATA_TYPE_RESERVE3             	(MUI_DATA_TYPE_DEV_START+16)
#define MUI_DATA_TYPE_RESERVE4             	(MUI_DATA_TYPE_DEV_START+17)
#define MUI_DATA_TYPE_DEV_END           	MUI_DATA_TYPE_RESERVE4
//...........................................
#define MUI_DATA_TYPE_AUX_START				(MUI_DATA_TYPE_DEV_END+1)
#define MUI_DATA_TYPE_EXTEND				(MUI_DATA_TYPE_AUX_START+0)
#define MUI_DATA_TYPE_AUX_END				MUI_DATA_TYPE_EXTEND
//-------------------------------------------
#define MUI_DATA_TYPE_END					MUI_DATA_TYPE_AUX_END

#define MUI_DATA_TYPE_INVALID				(MUI_DATA_TYPE_DEV_END+1)

#define MUI_CFG_TYPE_NUM					(MUI_DATA_TYPE_CFG_END-MUI_DATA_TYPE_CFG_START+1)
#define MUI_DEV_TYPE_NUM					(MUI_DATA_TYPE_DEV_END-MUI_DATA_TYPE_DEV_START+1)
#define MUI_DATA_TYPE_NUM					(MUI_DEV_TYPE_NUM+MUI_DEV_TYPE_NUM)

#define MUI_CONFIG_DATA_NUM_MAX				1
#define MUI_USER_DATA_NUM_MAX				1
#define MUI_GROUP_DATA_NUM_MAX				100
#define MUI_SET_DATA_NUM_MAX				100
#define MUI_SCENE_DATA_NUM_MAX				100
#define MUI_SCHEDULE_DATA_NUM_MAX			100
#define MUI_RULE_DATA_NUM_MAX				100

#define MUI_PHY_DEV_NUM_MAX					50
//#define MUI_LOGIC_DEV_NUM_MAX				255		//本设备目前最多支持的设备数
#define MUI_LOGIC_DEV_NUM_MAX				100		//本设备目前最多支持的设备数(因Cortex-M4下的RAM太少,此处暂时只支持100个设备)
#if (MUI_LOGIC_DEV_NUM_MAX >= 0x8000)
	#error "Device number must less then 0x8000."
#endif

#define MUI_EXTEND_DATA_NUM_MAX				(MUI_SET_DATA_NUM_MAX+MUI_SCHEDULE_DATA_NUM_MAX+MUI_RULE_DATA_NUM_MAX+MUI_LOGIC_DEV_NUM_MAX)

#define MUI_DATABASE_ID_START				1

#define MUI_DEV_ID_MIN						MUI_DATABASE_ID_START
#define MUI_DEV_ID_MAX						(MUI_DATABASE_ID_START+MUI_LOGIC_DEV_NUM_MAX-1)
#define MUI_DEV_ID_INVALID					(MUI_DEV_ID_MAX+1)

#define MUI_DATA_IDX_TYPE_MASK				0xFFFF0000
#define MUI_DATA_IDX_ID_MASK				0x0000FFFF

#define M_GEN_IDX_BY_TYPE_ID(_Type, _Id)	((((uint32_t)(_Type))<<16) | ((uint32_t)(_Id)))
#define M_GET_TYPE_FROM_IDX(_Idx)			((uint16_t)((((uint32_t)(_Idx))&MUI_DATA_IDX_TYPE_MASK)>>16))
#define M_GET_ID_FROM_IDX(_Idx)				((uint16_t)(((uint32_t)(_Idx))&MUI_DATA_IDX_ID_MASK))


//-------------------------------------------------------------------------------------------------*
//factory information
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define	MUI_FACTORY_SN_LEN					20

//................................................
typedef struct
{
	uint8_t 			aui8Mac[6];
} S_MacAddr;

typedef	struct
{
	char				str[MUI_FACTORY_SN_LEN+1];
} S_FactorySn;

typedef	struct
{
	char				str[MUI_STR_MAC_ADDR_LEN+1];
} S_StrMacAddr;

typedef	struct
{
	S_FactorySn			sn;
	S_StrMacAddr		mac;
	S_StrMacAddr		wifimac;
} S_FactoryInfo;

//-------------------------------------------------------------------------------------------------*
//config
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define	MUI_FACTORY_SN_LEN					20

#define	MUI_CFG_UTC_TIME_LEN				19
#define	MUI_CFG_NAME_LEN_MAX				20
#define	MUI_CFG_SW_UPDATE_URL_LEN			100
#define	MUI_CFG_SW_UPDATE_TEXT_LEN			100
#define	MUI_CFG_SW_VER_LEN					16
#define MUI_CFG_UZIP_VER_LEN				16
#define MUI_CFG_UZIP_MAC					16
#define MUI_CFG_UZIP_SN						40
#define	MUI_CFG_PROXY_ADDR_LEN				40
#define MUI_CFG_DEV_ID_LEN					20
#define MUI_CFG_PASSWD_LEN					20

//-	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -*
//Base config data type

//................................................
typedef	struct
{
	char				str[MUI_STR_IP_ADDR_LEN+1];
} S_StrIpAddr;
typedef	struct
{
	T_SysTime			time;
} S_CfgUtc;
typedef	struct
{
	char				str[MUI_CFG_NAME_LEN_MAX+1];		//A	unique,	editable name.
} S_CfgName;
typedef	struct
{
	uint8_t				updatestate;
	char				url[MUI_CFG_SW_UPDATE_URL_LEN+1];
	char				text[MUI_CFG_SW_UPDATE_TEXT_LEN+1];
	uint8_t				notify;
} S_CfgSwUpdate;
typedef	struct
{
	char				str[MUI_CFG_SW_VER_LEN+1];
} S_CfgSwVer;
typedef	struct
{
	char				str[MUI_CFG_UZIP_VER_LEN+1];
} S_CfgUzipVer;
typedef struct
{
	char				str[MUI_CFG_UZIP_MAC+1];
} S_CfgUzipMac;
typedef struct
{
	char				str[MUI_CFG_UZIP_SN+1];
} S_CfgUzipSn;
typedef	struct
{
	char				str[MUI_CFG_PROXY_ADDR_LEN+1];
} S_CfgProxyAddr;
typedef	struct
{
	char				str[MUI_CFG_DEV_ID_LEN+1];
} S_StrDevId;
typedef	struct
{
	char				str[MUI_CFG_PASSWD_LEN+1];
} S_StrPasswd;

//................................................
typedef	struct
{
	S_FactorySn			sn;
	uint32_t			databasever;

	S_CfgName			name;
	S_CfgSwVer			swver;
	S_StrMacAddr		wifimac;
	S_StrMacAddr		mac;
	S_StrIpAddr			ipaddr;
	S_StrIpAddr			netmask;
	S_StrIpAddr			gwipaddr;
	uint8_t				dhcp;

	S_CfgUzipVer		uzipver;
	S_CfgUzipMac		uzipmac;
	S_CfgUzipSn			uzipsn;

	S_CfgUtc			utc;

	S_CfgSwUpdate		swupdate;

	S_CfgProxyAddr		proxyaddr;
	uint16_t			proxyport;
	uint8_t				portalservices;

	S_StrDevId			devid;
	S_StrPasswd			passwd;

	uint8_t				linkbutton;

	//uint32_t			whitelist;	//计数
} S_ConfigInfo;
typedef	struct
{
	S_ConfigInfo		sConfigInfo;
	uint32_t			ui32Crc;
} S_ConfigAttrData;

//................................................

//-------------------------------------------------------------------------------------------------*
//user
#define MUI_USER_NUM_MAX					500
#define	MUI_USER_NAME_LEN					40
#define	MUI_USER_DEV_NAME_LEN				40


typedef	struct
{
	char		name[MUI_USER_NAME_LEN+1];			//加1是为了存储可能多出的'\0'
	char		dev_name[MUI_USER_DEV_NAME_LEN+1];	//加1是为了存储可能多出的'\0'
	T_SysTime	create_time;
	T_SysTime	last_use_time;
} S_UserData;

//-------------------------------------------------------------------------------------------------*
#define MUI_DEV_AT_ERR_SCAN_LIMIT	3
#define MUI_DEV_AT_ERR_CNT_MAX		255

//................................................
#define MUI_DEV_ATTR_BASE			0

#define MUI_DEV_ATTR_TYPE			(MUI_DEV_ATTR_BASE+0)
#define MUI_DEV_ATTR_NAME			(MUI_DEV_ATTR_BASE+1)
#define MUI_DEV_ATTR_DESCR			(MUI_DEV_ATTR_BASE+2)
#define MUI_DEV_ATTR_MODEL_ID		(MUI_DEV_ATTR_BASE+3)
#define MUI_DEV_ATTR_SW_VER			(MUI_DEV_ATTR_BASE+4)
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define MUI_DEV_ZB_ATTR_BASE		(MUI_DEV_ATTR_SW_VER+1)

#define MUI_DEV_ZB_ATTR_NWK_ADDR	(MUI_DEV_ATTR_BASE+0)
#define MUI_DEV_ZB_ATTR_EP			(MUI_DEV_ATTR_BASE+1)
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define MUI_DEV_STATE_BASE			(MUI_DEV_ZB_ATTR_EP+1)

#define MUI_DEV_STATE_REACHABLE		(MUI_DEV_STATE_BASE+0)
#define MUI_DEV_STATE_ON			(MUI_DEV_STATE_BASE+1)
#define MUI_DEV_STATE_BRI			(MUI_DEV_STATE_BASE+2)
#define MUI_DEV_STATE_SAT			(MUI_DEV_STATE_BASE+3)
#define MUI_DEV_STATE_HUE			(MUI_DEV_STATE_BASE+4)
#define MUI_DEV_STATE_CT			(MUI_DEV_STATE_BASE+5)
#define MUI_DEV_STATE_XY			(MUI_DEV_STATE_BASE+6)
#define MUI_DEV_STATE_COLOR_MODE	(MUI_DEV_STATE_BASE+7)
#define MUI_DEV_STATE_ALERT			(MUI_DEV_STATE_BASE+8)
#define MUI_DEV_STATE_EFFECT		(MUI_DEV_STATE_BASE+9)
#define MUI_DEV_STATE_LEVEL			(MUI_DEV_STATE_BASE+10)
#define MUI_DEV_STATE_COUNT			(MUI_DEV_STATE_BASE+11)
#define MUI_DEV_STATE_TEMP			(MUI_DEV_STATE_BASE+12)
#define MUI_DEV_STATE_HUM			(MUI_DEV_STATE_BASE+13)
#define MUI_DEV_STATE_CONSUMPTION	(MUI_DEV_STATE_BASE+14)
#define MUI_DEV_STATE_WARN_MODE		(MUI_DEV_STATE_BASE+15)
#define MUI_DEV_STATE_DURATION		(MUI_DEV_STATE_BASE+16)
#define MUI_DEV_STATE_WARN_CFG		(MUI_DEV_STATE_BASE+17)
#define MUI_DEV_STATE_ACTION		(MUI_DEV_STATE_BASE+18)
#define MUI_DEV_STATE_POWER			(MUI_DEV_STATE_BASE+19)
#define MUI_DEV_STATE_PLAY			(MUI_DEV_STATE_BASE+20)
#define MUI_DEV_STATE_VOLUME		(MUI_DEV_STATE_BASE+21)
#define MUI_DEV_STATE_URLID			(MUI_DEV_STATE_BASE+22)
#define MUI_DEV_STATE_EXECUTE		(MUI_DEV_STATE_BASE+23)

//................................................
#define M_MEM_TYPE_FLASH				1
#define M_MEM_TYPE_RAM					2
//................................................
#define MUI_DEV_MAC_ADDR_LEN			16
#define MUI_DEV_ETH_MAC_ADDR_LEN		MUI_DEV_MAC_ADDR_LEN
#define MUI_DEV_ZB_MAC_ADDR_LEN			MUI_DEV_MAC_ADDR_LEN
#define MUI_DEV_ZB_NWK_ADDR_LEN			4
#define MUI_DEV_ZB_EP_LEN				2
#define MUI_DEV_IP_ADDR_LEN				MUI_STR_IP_ADDR_LEN

#define MUI_DEV_ATTR_TYPE_LEN_MAX		50
#define MUI_DEV_ATTR_NAME_LEN_MAX		32
#define MUI_DEV_ATTR_DESCR_LEN_MAX		40
#define MUI_DEV_ATTR_MODEL_ID_LEN_MAX	MUI_DEV_MAC_ADDR_LEN
#define MUI_DEV_ATTR_SW_VER_LEN_MAX		16

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//Base device data type
typedef struct
{
	char			str[MUI_DEV_ATTR_TYPE_LEN_MAX+1];		//A fixed name describing the type of device. e.g. "Extended humidity temperature sensor".
} S_AttrType;
typedef struct
{
	char			str[MUI_DEV_ATTR_NAME_LEN_MAX+1];		//A unique, editable name given to the device.
} S_AttrName;
typedef struct
{
	char			str[MUI_DEV_ATTR_DESCR_LEN_MAX+1];		//An editable attribute given to device. Now, it used to label the ZONE that device belongs.
} S_AttrDescr;
typedef struct
{
	char			str[MUI_DEV_ATTR_MODEL_ID_LEN_MAX+1];	//The hardware model of the temperature sensor.(MAC address)
} S_AttrModelId;
typedef struct
{
	char			str[MUI_DEV_ATTR_SW_VER_LEN_MAX+1];		//An identifier for the software version running on the temperature sensor.
} S_AttrSwVer;
typedef union
{
	uint8_t			aui8Resv[64];
	uint16_t		aui16Resv[32];
	uint32_t		aui32Resv[16];
} U_AttrReserve;

typedef union
{
	char			str[MUI_DEV_ZB_NWK_ADDR_LEN+1];			//Short network address.
	char			ip[MUI_DEV_IP_ADDR_LEN+1];				//IP address.
} S_ZbAttrNwkAddr;
typedef union
{
	char			str[MUI_DEV_ZB_EP_LEN+1];				//End points.
	uint16_t		port;									//Port
} S_ZbAttrEp;
typedef union
{
	uint8_t			aui8Resv[16];
	uint16_t		aui16Resv[8];
	uint32_t		aui32Resv[4];
} U_ZbAttrReserve;

typedef struct
{
	uint16_t		x;				//虽然API文档要求此值为浮点数,可在实际设置中,该值为16位整形
	uint16_t		y;				//虽然API文档要求此值为浮点数,可在实际设置中,该值为16位整形
} S_Color_XY;

typedef struct
{
	uint8_t			warnmode;
	uint16_t		duration;
} S_WarnCfg;

//................................................
// Device API Attribute
typedef struct
{
	S_AttrType		type;			//A fixed name describing the type of device. e.g. "Extended humidity temperature sensor".
	S_AttrName		name;			//A unique, editable name given to the device.
	S_AttrDescr		descr;			//An editable attribute given to device. Now, it used to label the ZONE that device belongs.
	S_AttrModelId	modelid;		//The hardware model of the temperature sensor.(MAC address)
	S_AttrSwVer		swver;			//An identifier for the software version running on the temperature sensor.
	uint8_t			changerpt;		//Report when changed
	uint16_t		autorpt;		//Auto report per interval
	U_AttrReserve	reserve;		//For reserve
} S_DevApiAttr;
//................................................
// Device Zigbee Attribute
typedef struct
{
	S_ZbAttrNwkAddr	nwkaddr;
	S_ZbAttrEp		ep;
	U_ZbAttrReserve	reserve;		//For reserve
} S_DevZbAttr;

//................................................
// Device State
typedef struct
{
	uint8_t			reachable;
	uint8_t			on;
	uint8_t			bri;
	uint8_t			sat;
	uint16_t		hue;
	uint16_t		ct;
	S_Color_XY		xy;
	uint8_t			colormode;
	uint8_t			alert;
	uint8_t			effect;
} S_LightState;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
typedef struct _S_LightStateOption
{
    uint8_t id:1;
    uint8_t type:1;
    uint8_t name:1;
    uint8_t descr:1;
    uint8_t modelid:1;
    uint8_t swver:1;
    uint8_t changerpt:1;
    uint8_t autorpt:1;
    uint8_t reachable:1;
    uint8_t on:1;
    uint8_t bri:1;
    uint8_t sat:1;
    uint8_t hue:1;
    uint8_t ct:1;
    uint8_t xy:1;
    uint8_t colormode:1;
    uint8_t alert:1;
    uint8_t effect:1;
} S_LightStateOption;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
typedef struct _S_SensorStateOption
{
    uint8_t id:1;
    uint8_t type:1;
    uint8_t name:1;
    uint8_t descr:1;
    uint8_t modelid:1;
    uint8_t swver:1;
    uint8_t changerpt:1;
    uint8_t autorpt:1;
    uint8_t reachable:1;
    uint8_t power:1;
    uint8_t on:1;
    uint8_t count:1;
    uint8_t temp:1;
    uint8_t hum:1;
    uint8_t level:1;
    uint8_t consumption:1;
    uint8_t warnmode:1;
    uint8_t duration:1;
    uint8_t action:1;
    uint8_t play:1;
    uint8_t volume:1;
    uint8_t urlid:1;
} S_SensorStateOption;

typedef union
{
    S_LightStateOption light_option;
    S_SensorStateOption sensor_option;
} U_DevStateOption;

typedef struct
{
	uint8_t			reachable;
	uint8_t			power;
	uint8_t			on;
	uint16_t		count;
} S_OccSensorState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			power;
	uint8_t			on;
	uint16_t		count;
} S_BinSensorState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			power;
	uint16_t		temp;
} S_TempSensorState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			power;
	uint16_t		hum;
} S_HumSensorState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			on;
} S_BinCtrlState;

typedef struct
{
	uint8_t			reachable;
	uint16_t		level;
} S_LevelCtrlState;

typedef struct
{
	uint8_t			reachable;
	S_WarnCfg		warncfg;
} S_WarnDevState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			on;
	uint16_t		consumption;
} S_PlugState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			ctrlmode;
	uint8_t			on;
	uint16_t		level;
} S_SwitchState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			action;
	uint16_t		level;
} S_ShadeState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			on;
	uint16_t		level;
} S_PanelState;

typedef struct
{
	uint8_t			reachable;
	uint8_t			play;
	uint16_t		volume;
	uint32_t		urlid;
} S_SonosState;
//................................................
// Device Extend Attribute
typedef struct
{
	uint8_t			colormode;
} S_LightExtAttr;

typedef struct
{
	uint8_t			unused;
} S_OccSensorExtAttr;

typedef struct
{
	uint8_t			unused;
} S_BinSensorExtAttr;

typedef struct
{
	uint8_t			unused;
} S_TempSensorExtAttr;

typedef struct
{
	uint8_t			unused;
} S_HumSensorExtAttr;

typedef struct
{
	uint8_t			unused;
} S_BinCtrlExtAttr;

typedef struct
{
	uint8_t			unused;
} S_LevelCtrlExtAttr;

typedef struct
{
	uint8_t			unused;
} S_WarnDevExtAttr;

typedef struct
{
	uint8_t			unused;
} S_PlugExtAttr;

typedef struct
{
	uint8_t			ctrlmode;
} S_SwitchExtAttr;

typedef struct
{
	uint8_t			unused;
} S_ShadeExtAttr;

typedef struct
{
	uint8_t			unused;
} S_PanelExtAttr;

typedef struct
{
	uint8_t			unused;
} S_SonosExtAttr;
//................................................
// Device attrib for Flash saving
typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_LightExtAttr		ext_attr;
} S_LightAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_OccSensorExtAttr	ext_attr;
} S_OccSensorAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_BinSensorExtAttr	ext_attr;
} S_BinSensorAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_TempSensorExtAttr	ext_attr;
} S_TempSensorAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_HumSensorExtAttr	ext_attr;
} S_HumSensorAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_BinCtrlExtAttr	ext_attr;
} S_BinCtrlAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_LevelCtrlExtAttr	ext_attr;
} S_LevelCtrlAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_WarnDevExtAttr	ext_attr;
} S_WarnDevAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_PlugExtAttr		ext_attr;
} S_PlugAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_SwitchExtAttr		ext_attr;
} S_SwitchAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_ShadeExtAttr		ext_attr;
} S_ShadeAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_PanelExtAttr		ext_attr;
} S_PanelAttrFlash;

typedef struct
{
	S_DevApiAttr		api_attr;
	S_DevZbAttr			zb_attr;
	S_SonosExtAttr		ext_attr;
} S_SonosAttrFlash;
//................................................
typedef struct
{
	uint8_t				new_dev;	//New dev,
	uint8_t				not_added;	//Haven't been added to device database
	uint8_t				at_err_tick;//at_cmd error tick
} S_DevMiscState;
//................................................
// Device state for RAM saving
typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_LightState		state;
} S_LightStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_OccSensorState	state;
} S_OccSensorStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_BinSensorState	state;
} S_BinSensorStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_TempSensorState	state;
} S_TempSensorStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_HumSensorState	state;
} S_HumSensorStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_BinCtrlState		state;
} S_BinCtrlStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_LevelCtrlState	state;
} S_LevelCtrlStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_WarnDevState		state;
} S_WarnDevStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_PlugState			state;
} S_PlugStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_SwitchState		state;
} S_SwitchStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_ShadeState		state;
} S_ShadeStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_PanelState		state;
} S_PanelStateRam;

typedef struct
{
	S_DevMiscState		misc;
	S_DevZbAttr			zb_attr;
	S_SonosState		state;
} S_SonosStateRam;
//................................................
typedef union
{
	S_LightExtAttr		light_ext_attr;
	S_OccSensorExtAttr	occ_sensor_ext_attr;
	S_BinSensorExtAttr	bin_sensor_ext_attr;
	S_TempSensorExtAttr	temp_sensor_ext_attr;
	S_HumSensorExtAttr	hum_sensor_ext_attr;
	S_BinCtrlExtAttr	bin_ctrl_ext_attr;
	S_LevelCtrlExtAttr	level_ctrl_ext_attr;
	S_WarnDevExtAttr	warn_dev_ext_attr;
	S_PlugExtAttr		plug_ext_attr;
	S_SwitchExtAttr		switch_ext_attr;
	S_ShadeExtAttr		shade_ext_attr;
	S_PanelExtAttr		panel_ext_attr;
	S_SonosExtAttr		sonos_ext_attr;
} U_DevExtAttr;
//................................................
typedef union
{
	S_LightState		light_state;
	S_OccSensorState	occ_sensor_state;
	S_BinSensorState	bin_sensor_state;
	S_TempSensorState	temp_sensor_state;
	S_HumSensorState	hum_sensor_state;
	S_BinCtrlState		bin_ctrl_state;
	S_LevelCtrlState	level_ctrl_state;
	S_WarnDevState		warn_dev_state;
	S_PlugState			plug_state;
	S_SwitchState		switch_state;
	S_ShadeState		shade_state;
	S_PanelState		panel_state;
	S_SonosState		sonos_state;
} U_DevState;
//................................................
//For Flash database saving
typedef union
{
	S_LightAttrFlash		light_attr;
	S_OccSensorAttrFlash	occ_sensor_attr;
	S_BinSensorAttrFlash	bin_sensor_attr;
	S_TempSensorAttrFlash	temp_sensor_attr;
	S_HumSensorAttrFlash	hum_sensor_attr;
	S_BinCtrlAttrFlash		bin_ctrl_attr;
	S_LevelCtrlAttrFlash	level_ctrl_attr;
	S_WarnDevAttrFlash		warn_dev_attr;
	S_PlugAttrFlash			plug_attr;
	S_SwitchAttrFlash		switch_attr;
	S_ShadeAttrFlash		shade_attr;
	S_PanelAttrFlash		panel_attr;
	S_SonosAttrFlash		sonos_attr;
} U_DevAttrSave;

//For RAM database saving
typedef union
{
	S_LightStateRam			light_state;
	S_OccSensorStateRam		occ_sensor_state;
	S_BinSensorStateRam		bin_sensor_state;
	S_TempSensorStateRam	temp_sensor_state;
	S_HumSensorStateRam		hum_sensor_state;
	S_BinCtrlStateRam		bin_ctrl_state;
	S_LevelCtrlStateRam		level_ctrl_state;
	S_WarnDevStateRam		warn_dev_state;
	S_PlugStateRam			plug_state;
	S_SwitchStateRam		switch_state;
	S_ShadeStateRam			shade_state;
	S_PanelStateRam			panel_state;
	S_SonosStateRam			sonos_state;
} U_DevStateSave;

//................................................
//Device index
typedef struct
{
	uint16_t				ui16Type;
	uint16_t				ui16Id;
} S_DevIdx;
#if 0
//Device Index
typedef struct
{
	uint16_t			ui16DevType;		//设备类型
	uint16_t			ui16DevID;			//设备ID
} S_DevIdx;
#endif
//................................................
typedef struct
{
	uint8_t			colormode;
} S_LightExtInfo;

typedef struct
{
	uint8_t			unused;
} S_OccSensorExtInfo;

typedef struct
{
	uint8_t			unused;
} S_BinSensorExtInfo;

typedef struct
{
	uint8_t			unused;
} S_TempSensorExtInfo;

typedef struct
{
	uint8_t			unused;
} S_HumSensorExtInfo;

typedef struct
{
	uint8_t			unused;
} S_BinCtrlExtInfo;

typedef struct
{
	uint8_t			unused;
} S_LevelCtrlExtInfo;

typedef struct
{
	uint8_t			unused;
} S_WarnDevExtInfo;

typedef struct
{
	uint8_t			unused;
} S_PlugExtInfo;

typedef struct
{
	uint8_t			ctrlmode;
} S_SwitchExtInfo;

typedef struct
{
	uint8_t			unused;
} S_ShadeExtInfo;

typedef struct
{
	uint8_t			unused;
} S_PanelExtInfo;

typedef struct
{
	uint8_t			unused;
} S_SonosExtInfo;

typedef union
{
	S_LightExtInfo			light_ext_info;
	S_OccSensorExtInfo		occ_sensor_ext_info;
	S_BinSensorExtInfo		bin_sensor_ext_info;
	S_TempSensorExtInfo		temp_sensor_ext_info;
	S_HumSensorExtInfo		hum_sensor_ext_info;
	S_BinCtrlExtInfo		bin_ctrl_ext_info;
	S_LevelCtrlExtInfo		level_ctrl_ext_info;
	S_WarnDevExtInfo		warn_dev_ext_info;
	S_PlugExtInfo			plug_ext_info;
	S_SwitchExtInfo			switch_ext_info;
	S_ShadeExtInfo			shade_ext_info;
	S_PanelExtInfo			panel_ext_info;
	S_SonosExtInfo			sonos_ext_info;
} U_DevExtInfo;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//extend
//................................................


typedef struct
{
	uint8_t 		time_mode;
	struct tm 		Time;
} S_ModeTime;

//group
//................................................
#define MUI_DEV_NUM_MAX_PER_GROUP			50

//set
//................................................
#define MUI_ACTION_NUM_MAX_PER_SET			30

//scene
//................................................
#define MUI_ACTION_NUM_MAX_PER_SCENE		30

//schedule
//................................................
#define MUI_OUTPUT_NUM_MAX_PER_SCHEDULE		20

//rule
//................................................
#define MUI_RULE_LOGICAL_EXP_SIZE_MAX		30

#define MUI_INPUT_NUM_MAX_PER_RULE			10
#define MUI_T_OUTPUT_NUM_MAX_PER_RULE		20
#define MUI_F_OUTPUT_NUM_MAX_PER_RULE		20


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_API_DEBUG_MASK_MAX			0xFFFFFFF
#define MUI_API_DEBUG_LEVEL_MAX			15

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

#endif	//end of "#ifndef __M_FILE_NAME_dev_common_H__".
