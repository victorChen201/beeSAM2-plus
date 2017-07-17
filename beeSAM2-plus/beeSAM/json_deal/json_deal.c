#include "common.h"
#include "data_common.h"
#include "extend_data.h"
#include "json_deal.h"
#include "config.h"
#include "group.h"
#include "set.h"
#include "schedule.h"
#include "rule.h"
#include "sys_time.h"
#include "sonos_drv.h"
#include "api_deal.h"
#include "scene.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define JSON_DEAL_DBG

#ifndef JSON_DEAL_PUTC
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_PUTC(Char)			PUTC(Char)
#else
#define JSON_DEAL_PUTC(Char)
#endif
#endif

#ifndef JSON_DEAL_PUTS
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define JSON_DEAL_PUTS(Str, Len)
#endif
#endif

#ifndef JSON_DEAL_PUTASCC
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_PUTASCC(Char)			PUTASCC(Char)
#else
#define JSON_DEAL_PUTASCC(Char)
#endif
#endif

#ifndef JSON_DEAL_PUTASCS
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define JSON_DEAL_PUTASCS(Str, Len)
#endif
#endif

#ifndef JSON_DEAL_PRINT
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define JSON_DEAL_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef JSON_DEAL_DYN_PUTS
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_DYN_PUTS(Dbg, Str, Len)	DYN_PUTS((M_DBG_MODULE_JSON_DEAL|Dbg), Str, Len)
#else
#define JSON_DEAL_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef JSON_DEAL_DYN_PUTASCC
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_JSON_DEAL|Dbg), Char)
#else
#define JSON_DEAL_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef JSON_DEAL_DYN_PUTASCS
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_JSON_DEAL|Dbg), Str, Len)
#else
#define JSON_DEAL_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef JSON_DEAL_DYN_PRINT
#ifdef JSON_DEAL_DBG
#define JSON_DEAL_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_JSON_DEAL|Dbg), Fmt, ##Arg)
#else
#define JSON_DEAL_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MSTR_LIGHT_ALERT_NONE							"none"
#define MSTR_LIGHT_ALERT_SELECT							"select"
#define MSTR_LIGHT_ALERT_LSELECT						"lselect"
#define MSTR_LIGHT_EFFECT_NONE							"none"
#define MSTR_LIGHT_EFFECT_COLOR_LOOP					"colorloop"
#define MSTR_LIGHT_COLOR_MODE_MC						"mc"
#define MSTR_LIGHT_COLOR_MODE_HS						"hs"
#define MSTR_LIGHT_COLOR_MODE_XY						"xy"
#define MSTR_LIGHT_COLOR_MODE_CT						"ct"

#define MSTR_SWITCH_CTRL_MODE_ON_OFF					"on/off"
#define MSTR_SWITCH_CTRL_MODE_LEVEL						"level"
#define MSTR_SWITCH_CTRL_MODE_MULTI						"multi"

//---------------------------------------------------------------------rules
typedef struct
{
	uint16_t			ui16Key;
	char				achKeyword[MUI_API_KEYWORD_LEN_MAX];
} S_SetDataKeywordMap;

typedef struct
{
	const S_SetDataKeywordMap *psKeywordMap;
	uint8_t				ui8MembNum;
} S_KeywordMapDesc;

typedef struct
{
	uint16_t			ui16Key;
	char				achKeyword[MUI_API_KEYWORD_LEN_MAX];
} S_RulesKeywordMap;

typedef struct
{
	uint16_t			ui16Key;
	char				achKeyword[MUI_API_KEYWORD_LEN_MAX];
} S_RulesInPutStateKeywordMap;

typedef struct
{
	uint16_t			ui16Key;
	char				achKeyword[MUI_API_KEYWORD_LEN_MAX];
} S_RulesOutPutStateKeywordMap;

//----------------------------------------------------------------------
static const char s_aachCfgUrlKeyWordList[MUI_CFG_TYPE_NUM][MUI_API_URL_KEY_WORD_LEN_MAX] =
{
	MSTR_API_URL_KEY_WORD_CONFIG,
	MSTR_API_URL_KEY_WORD_GROUPS,
	MSTR_API_URL_KEY_WORD_SET,
	MSTR_API_URL_KEY_WORD_SCHEDULES,
	MSTR_API_URL_KEY_WORD_RULES
};

static const char s_aachDevUrlKeyWordList[MUI_DEV_TYPE_NUM][MUI_API_URL_KEY_WORD_LEN_MAX] =
{
	MSTR_API_URL_KEY_WORD_LIGHTS,
	MSTR_API_URL_KEY_WORD_OCC_SENSORS,
	MSTR_API_URL_KEY_WORD_BIN_SENSORS,
	MSTR_API_URL_KEY_WORD_TEMP_SENSORS,
	MSTR_API_URL_KEY_WORD_HUM_SENSORS,
	MSTR_API_URL_KEY_WORD_TH_SENSORS,
	MSTR_API_URL_KEY_WORD_BIN_CTRLS,
	MSTR_API_URL_KEY_WORD_LEVEL_CTRLS,
	MSTR_API_URL_KEY_WORD_WARN_DEVS,
	MSTR_API_URL_KEY_WORD_PLUGS,
	MSTR_API_URL_KEY_WORD_SWITCHS,
	MSTR_API_URL_KEY_WORD_SHADES,
	MSTR_API_URL_KEY_WORD_SONOSES
};

static const char s_aachDevDescriptionList[MUI_DEV_TYPE_NUM][MUI_DEV_DESCRIPTIONLEN_MAX] =
{
	"light",
	"occupancy sensor",
	"binary sensor",
	"temperature sensor",
	"humidity sensor",
	"TH sensor",
	"binary controller",
	"level contriller",
	"warning device",
	"plug",
	"switch",
	"shade",
	"sonos"
};

static const S_SetDataKeywordMap s_asGetFullStateKeywordMapList[] =
{
	{MUI_DATA_TYPE_CONFIG,								MSTR_API_KEY_WORD_CONFIG},
	{MUI_DATA_TYPE_GROUP,								MSTR_API_KEY_WORD_GROUPS},
	{MUI_DATA_TYPE_SET,									MSTR_API_KEY_WORD_SET},
	{MUI_DATA_TYPE_SCHED,								MSTR_API_KEY_WORD_SCHEDULES},
	{MUI_DATA_TYPE_RULE,								MSTR_API_KEY_WORD_RULES},
	{MUI_DATA_TYPE_SCENE,								MSTR_API_KEY_WORD_SCENE},

	{MUI_DATA_TYPE_LIGHT,								MSTR_API_KEY_WORD_LIGHTS},
	{MUI_DATA_TYPE_OCC_SENSOR,							MSTR_API_KEY_WORD_OCC_SENSORS},
	{MUI_DATA_TYPE_BIN_SENSOR,							MSTR_API_KEY_WORD_BIN_SENSORS},
	{MUI_DATA_TYPE_TEMP_SENSOR,							MSTR_API_KEY_WORD_TEMP_SENSORS},
	{MUI_DATA_TYPE_HUM_SENSOR,							MSTR_API_KEY_WORD_HUM_SENSORS},
	{MUI_DATA_TYPE_TH_SENSOR,							MSTR_API_KEY_WORD_TH_SENSORS},
	{MUI_DATA_TYPE_BIN_CTRL,							MSTR_API_KEY_WORD_BIN_CTRLS},
	{MUI_DATA_TYPE_LEVEL_CTRL,							MSTR_API_KEY_WORD_LEVEL_CTRLS},
	{MUI_DATA_TYPE_WARN_DEV,							MSTR_API_KEY_WORD_WARN_DEVS},
	{MUI_DATA_TYPE_PLUG,								MSTR_API_KEY_WORD_PLUGS},
	{MUI_DATA_TYPE_SWITCH,								MSTR_API_KEY_WORD_SWITCHS},
	{MUI_DATA_TYPE_SHADE,								MSTR_API_KEY_WORD_SHADES},
	{MUI_DATA_TYPE_SONOS,								MSTR_API_KEY_WORD_SONOSES}
};

static const S_SetDataKeywordMap s_asSetConfigKeywordMapList[] =
{
	{MUI_SET_CONFIG_KEY_WORD_IDX_NAME,					"name"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_IP_ADDR,				"ipaddr"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_NETMASK,				"netmask"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_GW_IP_ADDR,			"gwipaddr"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_DHCP,					"dhcp"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_PROXY_ADDR,			"proxyaddr"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_PROXY_PORT,			"proxyport"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_PORTAL_SERVICES,		"portalservices"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_DEV_ID,				"devid"},
	{MUI_SET_CONFIG_KEY_WORD_IDX_PASSWD,				"passwd"}
};

static const S_SetDataKeywordMap s_aachGroupTypeMapList[]=
{
	{MUI_SET_GROUP_ATTR_KEY_WORD_IDX_NAME, 				"name"},
	{MUI_SET_GROUP_ATTR_KEY_WORD_IDX_DESCRIPTION, 		"description"},
	{MUI_SET_GROUP_ATTR_KEY_WORD_IDX_CONFGI,			"config"}
};

static const S_SetDataKeywordMap s_aachGroupConfigTypeMapList[]=
{
	{MUI_SET_GROUP_ATTR_KEY_WORD_IDX_ADDRESS, 			"address"},
};

static const S_SetDataKeywordMap s_aachGroupConfigActionTypeMapList[]=
{
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI,				"bri"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT,				"sat"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE,				"hue"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_CT,					"ct"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_XY,					"xy"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL,				"level"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP,				"temp"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM,				"hum"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE,			"warnmode"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION,			"duration"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG,			"warncfg"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION,				"action"},
};

static const S_SetDataKeywordMap s_aachSetTypeMapList[]=
{
	{MUI_SET_SET_ATTR_KEY_WORD_IDX_NAME, 				"name"},
	{MUI_SET_SET_ATTR_KEY_WORD_IDX_DESCRIPTION, 		"description"},
	{MUI_SET_SET_ATTR_KEY_WORD_IDX_DELAY,				"delay"},
	{MUI_SET_SET_ATTR_KEY_WORD_IDX_CONFIG, 				"config"},
	{MUI_SET_SET_ATTR_KEY_WORD_IDX_ADDRESS, 			"address"},
	{MUI_SET_SET_ATTR_KEY_WORD_IDX_ACTION, 				"action"},
	{MUI_SET_SET_ATTR_KEY_WORD_IDX_EXECUTE, 			"execute"},
};

static const S_SetDataKeywordMap s_aachSetActionMapList[]=
{
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI,				"bri"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT,				"sat"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE,				"hue"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_CT,					"ct"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_XY,					"xy"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL,				"level"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP,				"temp"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM,				"hum"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE,			"warnmode"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION,			"duration"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG,			"warncfg"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION,				"action"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY,				"play"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME,				"volume"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID,				"urlid"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE,			"execute"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE,				"active"},
};
//---------------------------------------------------------------------------
//scene

static const S_SetDataKeywordMap s_aachSceneAttrTypeMapList[]=
{
	{MUI_SET_SCENE_ATTR_KEY_WORD_IDX_NAME, 				"name"},
	{MUI_SET_SCENE_ATTR_KEY_WORD_IDX_DESCRIPTION, 		"description"},
	{MUI_SET_SCENE_ATTR_KEY_WORD_IDX_CONFIG, 			"config"},
	{MUI_SET_SCENE_ATTR_KEY_WORD_IDX_EXECUTE,			"execute"}
};

static const S_SetDataKeywordMap s_aachSceneConfigTypeMapList[]=
{
	{MUI_SET_SCENE_CONFIG_KEY_WORD_IDX_ADDRESS, 		"address"},
	{MUI_SET_SCENE_CONFIG_KEY_WORD_IDX_ACTION, 			"action"},
};

static const S_SetDataKeywordMap s_aachSceneActionMapList[]=
{
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ON,				"on"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_BRI,				"bri"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_SAT,				"sat"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUE,				"hue"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_CT,				"ct"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_XY,				"xy"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_LEVEL,			"level"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_TEMP,				"temp"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUM,				"hum"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_MODE,		"warnmode"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_DURATION,			"duration"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_CFG,			"warncfg"},
	{MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ACTION,			"action"},
};


static const S_SetDataKeywordMap s_aachRulesTypeMapList[]=
{
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_NAME, 				"name"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_DESCRIPTION, 		"description"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_ACTIVE,			"active"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_TIME, 				"time"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_REPEAT, 			"repeat"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_DURATION, 			"duration"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_CONFIG, 			"config"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_INPUT, 			"input"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_EXPRESSION, 		"expression"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_TRUE_OUTPUT, 		"true-output"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_FALSE_OUTPUT, 		"false-output"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_ADDRESS, 			"address"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_STATE, 			"state"},
	{MUI_SET_RULES_ATTR_KEY_WORD_IDX_DELAY, 			"delay"}
};

static const S_SetDataKeywordMap s_aachRulesInputStateMapList[]=
{
	{MUI_RULES_ACTION_STATE_WORD_IDX_ON,				"on"},
	{MUI_RULES_ACTION_STATE_WORD_IDX_TEMP,				"temp"},
	{MUI_RULES_ACTION_STATE_WORD_IDX_HUM,				"hum"},
	{MUI_RULES_ACTION_STATE_WORD_IDX_LESS, 				"less"},
	{MUI_RULES_ACTION_STATE_WORD_IDX_EDGE, 				"edge"}
};

static const S_SetDataKeywordMap s_aachRulesOutputStateMapList[]=
{
	{MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_ADDRESS, 		"address"},
	{MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DELAY, 			"delay"},
	{MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DELAYOVERLAY, 	"delayoverlay"},
	{MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DURATION, 		"duration"},
	{MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DURATIONOVERLAY, "durationoverlay"},
	{MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_ACTION, 			"action"},
	{MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_QUITMETHOD, 		"quitmethod"},
	{MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_QUITSTATE, 		"quitstate"},
};

static const S_SetDataKeywordMap s_aachRulesActionMapList[]=
{
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI,				"bri"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT,				"sat"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE,				"hue"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_CT,					"ct"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_XY,					"xy"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL,				"level"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP,				"temp"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM,				"hum"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE,			"warnmode"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION,			"duration"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG,			"warncfg"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION,				"action"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY,				"play"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME,				"volume"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID,				"urlid"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE,			"execute"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE,				"active"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC,				"onsec"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC, 			"offsec"}
};

static const S_SetDataKeywordMap s_aachScheduleTypeMapList[]=
{
	{MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_NAME, 			"name"},
	{MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_DESCRIPTION, 	"description"},
	{MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_TIME, 			"time"},
	{MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_REPEAT, 		"repeat"},
	{MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_CONFIG, 		"config"},
};

static const S_SetDataKeywordMap s_aachScheduleOutputStateMapList[]=
{
	{MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_ADDRESS, 		"address"},
	{MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_DELAY, 		"delay"},
	{MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_DURATION, 	"duration"},
	{MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_ACTION, 		"action"},
	{MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_QUITMETHOD, 	"quitmethod"},
	{MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_QUITSTATE, 	"quitstate"},
};

static const S_SetDataKeywordMap s_asSetDevAttrKeywordMapList[] =
{
	{MUI_SET_DEV_ATTR_KEY_WORD_IDX_ID,					"id"},
	{MUI_SET_DEV_ATTR_KEY_WORD_IDX_NAME,				"name"},
	{MUI_SET_DEV_ATTR_KEY_WORD_IDX_DESCRIPTION,			"description"},
	{MUI_SET_DEV_ATTR_KEY_WORD_IDX_CHANGE_RPT,			"changerpt"},
	{MUI_SET_DEV_ATTR_KEY_WORD_IDX_AUTO_RPT,			"autorpt"}
};

static const S_SetDataKeywordMap s_asSetSysKeywordMapList[] =
{
	{MUI_SET_SYS_KEY_WORD_IDX_SN,						"sn"},
	{MUI_SET_SYS_KEY_WORD_IDX_MAC,						"mac"},
	{MUI_SET_SYS_KEY_WORD_IDX_WIFI_MAC,					"wifimac"},
	{MUI_SET_SYS_KEY_WORD_IDX_UZIPSN,					"uzipsn"},
	{MUI_SET_SYS_KEY_WORD_IDX_UZIPMAC,					"uzipmac"}
};

static const S_SetDataKeywordMap s_asSetLightStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI,				"bri"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT,				"sat"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE,				"hue"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_CT,					"ct"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_XY,					"xy"}
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_COLOR_MODE,		"colormode"},
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_ALERT,			"alert"},
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_EFFECT,			"effect"}
};

static const S_SetDataKeywordMap s_asSetOccSensorStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER,			"power"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"}
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT,			"count"}
};

static const S_SetDataKeywordMap s_asSetBinSensorStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER,			"power"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"}
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT,			"count"}
};

static const S_SetDataKeywordMap s_asSetTempSensorStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER,			"power"},
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP,				"temp"}
};

static const S_SetDataKeywordMap s_asSetHumSensorStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER,			"power"},
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM,				"hum"}
};

static const S_SetDataKeywordMap s_asSetBinCtrlStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"}
};

static const S_SetDataKeywordMap s_asSetLevelCtrlStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL,				"level"}
};

static const S_SetDataKeywordMap s_asSetWarnDevStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE,			"warnmode"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION,			"duration"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG,			"warncfg"}
};

static const S_SetDataKeywordMap s_asSetPlugStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"}
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_CONSUMPTION,		"consumption"}
};

static const S_SetDataKeywordMap s_asSetSwitchStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL,				"level"}
};

static const S_SetDataKeywordMap s_asSetShadeStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION,				"action"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL,				"level"}
};

static const S_SetDataKeywordMap s_asSetSonosStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY,				"play"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME,				"volume"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID,				"url"}
};

static const S_SetDataKeywordMap s_asSetPanelStateKeywordMapList[] =
{
	//{MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE,		"reachable"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ON,					"on"},
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL,				"level"}
};


static const S_SetDataKeywordMap s_asSetSetStateKeywordMapList[] =
{
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE,			"execute"}
};

static const S_SetDataKeywordMap s_asSetRulesStateKeywordMapList[] =
{
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE,				"active"}
};

static const S_SetDataKeywordMap s_asSetScheduleStateKeywordMapList[] =
{
	{MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE,				"active"}
};


static const S_KeywordMapDesc s_asDataKeywordMapDescList[] =
{
	{NULL,												0},	// config
	{NULL,												0},	// user
	{NULL,												0},	// group
	{s_asSetSetStateKeywordMapList,					sizeof(s_asSetSetStateKeywordMapList)/sizeof(s_asSetSetStateKeywordMapList[0])},
	{s_asSetScheduleStateKeywordMapList,				sizeof(s_asSetScheduleStateKeywordMapList)/sizeof(s_asSetScheduleStateKeywordMapList[0])},
	{s_asSetRulesStateKeywordMapList,					sizeof(s_asSetRulesStateKeywordMapList)/sizeof(s_asSetRulesStateKeywordMapList[0])},
	{s_asSetLightStateKeywordMapList,					sizeof(s_asSetLightStateKeywordMapList)/sizeof(s_asSetLightStateKeywordMapList[0])},
	{s_asSetOccSensorStateKeywordMapList,				sizeof(s_asSetOccSensorStateKeywordMapList)/sizeof(s_asSetOccSensorStateKeywordMapList[0])},
	{s_asSetBinSensorStateKeywordMapList,				sizeof(s_asSetBinSensorStateKeywordMapList)/sizeof(s_asSetBinSensorStateKeywordMapList[0])},
	{s_asSetTempSensorStateKeywordMapList,				sizeof(s_asSetTempSensorStateKeywordMapList)/sizeof(s_asSetTempSensorStateKeywordMapList[0])},
	{s_asSetHumSensorStateKeywordMapList,				sizeof(s_asSetHumSensorStateKeywordMapList)/sizeof(s_asSetHumSensorStateKeywordMapList[0])},
	{NULL,												0},	// TH_sensor
	{s_asSetBinCtrlStateKeywordMapList,					sizeof(s_asSetBinCtrlStateKeywordMapList)/sizeof(s_asSetBinCtrlStateKeywordMapList[0])},
	{s_asSetLevelCtrlStateKeywordMapList,				sizeof(s_asSetLevelCtrlStateKeywordMapList)/sizeof(s_asSetLevelCtrlStateKeywordMapList[0])},
	{s_asSetWarnDevStateKeywordMapList,					sizeof(s_asSetWarnDevStateKeywordMapList)/sizeof(s_asSetWarnDevStateKeywordMapList[0])},
	{s_asSetPlugStateKeywordMapList,					sizeof(s_asSetPlugStateKeywordMapList)/sizeof(s_asSetPlugStateKeywordMapList[0])},
	{s_asSetSwitchStateKeywordMapList,					sizeof(s_asSetSwitchStateKeywordMapList)/sizeof(s_asSetSwitchStateKeywordMapList[0])},
	{s_asSetShadeStateKeywordMapList,					sizeof(s_asSetShadeStateKeywordMapList)/sizeof(s_asSetShadeStateKeywordMapList[0])},
	{s_asSetSonosStateKeywordMapList,					sizeof(s_asSetSonosStateKeywordMapList)/sizeof(s_asSetSonosStateKeywordMapList[0])},
	{s_asSetPanelStateKeywordMapList, 					sizeof(s_asSetPanelStateKeywordMapList)/sizeof(s_asSetPanelStateKeywordMapList[0])}
};

//----------------------------------------------------------------------
static int s_json_print_unformat (cJSON *psJson, char **ppchOut);
static int s_keyword_map_search (const S_SetDataKeywordMap *psKeywordMap, uint16_t ui16MembNum, const char *pchKeyword, size_t tLen);

//-------------------------------------------------------------------------------------------------*
static int s_json_print_unformat (cJSON *psJson, char **ppchOut)
{
	*ppchOut = cJSON_PrintUnformatted(psJson);

	if (NULL == (*ppchOut))
	{
		return JSON_DEAL_ERR_PRINT_FAULT;
	}

	return (int)strlen(*ppchOut);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
static int s_keyword_map_search (const S_SetDataKeywordMap *psKeywordMap, uint16_t ui16MembNum, const char *pchKeyword, size_t tLen)
{
	uint16_t i;

	ASSERT(NULL != psKeywordMap);
	ASSERT(0 < ui16MembNum);
	ASSERT(NULL != pchKeyword);
	ASSERT(tLen < sizeof(psKeywordMap->achKeyword));

	for (i=0; i<ui16MembNum; i++, psKeywordMap++)
	{
		if ((0==memcmp(psKeywordMap->achKeyword, pchKeyword, tLen)) && ('\0'==psKeywordMap->achKeyword[tLen]))
		{
			return (int)i;
		}
	}

	return -1;
}
//-------------------------------------------------------------------------------------------------*
/*	组装设备数据JSON包函数
;	(功能)
;	(参数)
;		ui16DevType:	设备类型
;		ui16Id:			设备ID
;		ui8Mode:		组装模式(bit0:State; bit1:ApiAttr; bit2:ZbAttr; bit3:ExtAttr)
;		ppsJson:		回传组装好的JSON结构体包指针的指针
;	(返回)
;		int:			0表示成功,负数表示出错
;	(注意)
*/
int json_obj_assemble_dev_data (uint16_t ui16DevType, uint16_t ui16Id, uint8_t ui8Mode, cJSON **ppsJson)
{
	S_DevApiAttr sApiAttr;
	S_DevZbAttr sZbAttr;
	//U_DevExtAttr uExtAttr;
	U_DevState uState;
	cJSON *psR, *psC1, *psC2;
	double adXY[2];
	int iRet;

	ASSERT(0 != (ui8Mode&(MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR|MUI_API_DEV_DATA_ASM_JSON_CFG_ZB_ATTR|MUI_API_DEV_DATA_ASM_JSON_CFG_EXT_ATTR)));

	if (NULL == (psR=cJSON_CreateObject()))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_dev_attr_state]Call 'cJSON_CreateObject()' fail.\r\n");
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	if (ui8Mode & MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR)
	{
		iRet = read_dev_attr(ui16DevType, ui16Id, &sApiAttr);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_dev_attr_state]Call 'read_dev_attr()' fail.\r\n");
			cJSON_Delete(psR);
			return iRet;
		}

		//API attribute
		cJSON_AddStringToObject(psR, "type", sApiAttr.type.str);
		cJSON_AddStringToObject(psR, "name", sApiAttr.name.str);
		cJSON_AddStringToObject(psR, "description", sApiAttr.descr.str);
		cJSON_AddStringToObject(psR, "modelid", sApiAttr.modelid.str);
		cJSON_AddStringToObject(psR, "swver", sApiAttr.swver.str);
		cJSON_AddBoolToObject(psR, "changerpt", sApiAttr.changerpt);
		cJSON_AddNumberToObject(psR, "autorpt", sApiAttr.autorpt);
	}

	if (ui8Mode & MUI_API_DEV_DATA_ASM_JSON_CFG_ZB_ATTR)
	{
		iRet = read_dev_zb_attr(ui16DevType, ui16Id, &sZbAttr);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_dev_attr_state]Call 'read_dev_zb_attr()' fail.\r\n");
			cJSON_Delete(psR);
			return iRet;
		}

		//Zigbee attribute
		if (MUI_DATA_TYPE_SONOS == ui16DevType)
		{
			cJSON_AddStringToObject(psR, "ip", sZbAttr.nwkaddr.ip);
			cJSON_AddNumberToObject(psR, "port", sZbAttr.ep.port);
		}
		else
		{
			cJSON_AddStringToObject(psR, "nwkaddr", sZbAttr.nwkaddr.str);
			cJSON_AddStringToObject(psR, "ep", sZbAttr.ep.str);
		}
	}
#if 0
	if (ui8Mode & MUI_API_DEV_DATA_ASM_JSON_CFG_EXT_ATTR)
	{
		iRet = read_dev_ext_attr(ui16DevType, ui16Id, &uExtAttr);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_assemble_get_dev_attr_state]Call 'read_dev_ext_attr()' fail.\r\n");
			cJSON_Delete(psR);
			return iRet;
		}

		//Extend attribute
		if (MUI_DATA_TYPE_LIGHT == ui16DevType)
		{
			cJSON_AddNumberToObject(psR, "colormode", uExtAttr.light_ext_attr.colormode);
		}
	}
#endif
	if (ui8Mode & MUI_API_DEV_DATA_ASM_JSON_CFG_STATE)
	{
		iRet = read_dev_state(ui16DevType, ui16Id, &uState);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_dev_attr_state]Call 'read_dev_state()' fail.\r\n");
			cJSON_Delete(psR);
			return iRet;
		}

		//State
		if (NULL != (psC1=cJSON_CreateObject()))
		{
			cJSON_AddItemToObject(psR, "state", psC1);
			switch (ui16DevType)
			{
				case MUI_DATA_TYPE_LIGHT:
					cJSON_AddBoolToObject(psC1, "reachable", uState.light_state.reachable);
					cJSON_AddBoolToObject(psC1, "on", uState.light_state.on);
					cJSON_AddNumberToObject(psC1, "bri", uState.light_state.bri);
					cJSON_AddNumberToObject(psC1, "sat", uState.light_state.sat);
					cJSON_AddNumberToObject(psC1, "hue", uState.light_state.hue);
					cJSON_AddNumberToObject(psC1, "ct", uState.light_state.ct);
					adXY[0] = (double)uState.light_state.xy.x / 65536.0;
					adXY[1] = (double)uState.light_state.xy.y / 65536.0;
					cJSON_AddItemToObject(psC1, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					switch (uState.light_state.colormode)
					{
						case MUI_LIGHT_COLOR_MODE_CT:
							cJSON_AddStringToObject(psC1, "colormode", MSTR_LIGHT_COLOR_MODE_CT);
							break;
						case MUI_LIGHT_COLOR_MODE_XY:
							cJSON_AddStringToObject(psC1, "colormode", MSTR_LIGHT_COLOR_MODE_XY);
							break;
						case MUI_LIGHT_COLOR_MODE_HS:
							cJSON_AddStringToObject(psC1, "colormode", MSTR_LIGHT_COLOR_MODE_HS);
							break;
						default: // MUI_LIGHT_COLOR_MODE_MC
							cJSON_AddStringToObject(psC1, "colormode", MSTR_LIGHT_COLOR_MODE_MC);
							break;
					}
					switch (uState.light_state.alert)
					{
						case 3:
							cJSON_AddStringToObject(psC1, "alert", MSTR_LIGHT_ALERT_LSELECT);
							break;
						case 2:
							cJSON_AddStringToObject(psC1, "alert", MSTR_LIGHT_ALERT_SELECT);
							break;
						default: // 1
							cJSON_AddStringToObject(psC1, "alert", MSTR_LIGHT_ALERT_NONE);
							break;
					}
					if (2 == uState.light_state.effect)
					{
						cJSON_AddStringToObject(psC1, "effect", MSTR_LIGHT_EFFECT_COLOR_LOOP);
					}
					else
					{
						cJSON_AddStringToObject(psC1, "effect", MSTR_LIGHT_ALERT_NONE);
					}
					break;
				case MUI_DATA_TYPE_OCC_SENSOR:
					cJSON_AddBoolToObject(psC1, "reachable", uState.occ_sensor_state.reachable);
					cJSON_AddNumberToObject(psC1, "power", uState.occ_sensor_state.power);
					cJSON_AddBoolToObject(psC1, "on", uState.occ_sensor_state.on);
					cJSON_AddNumberToObject(psC1, "count", uState.occ_sensor_state.count);
					break;
				case MUI_DATA_TYPE_BIN_SENSOR:
					cJSON_AddBoolToObject(psC1, "reachable", uState.bin_sensor_state.reachable);
					cJSON_AddNumberToObject(psC1, "power", uState.bin_sensor_state.power);
					cJSON_AddBoolToObject(psC1, "on", uState.bin_sensor_state.on);
					cJSON_AddNumberToObject(psC1, "count", uState.bin_sensor_state.count);
					break;
				case MUI_DATA_TYPE_TEMP_SENSOR:
					cJSON_AddBoolToObject(psC1, "reachable", uState.temp_sensor_state.reachable);
					cJSON_AddNumberToObject(psC1, "power", uState.temp_sensor_state.power);
					cJSON_AddNumberToObject(psC1, "temp", (double)(uState.temp_sensor_state.temp)/100.0);
					break;
				case MUI_DATA_TYPE_HUM_SENSOR:
					cJSON_AddBoolToObject(psC1, "reachable", uState.hum_sensor_state.reachable);
					cJSON_AddNumberToObject(psC1, "power", uState.hum_sensor_state.power);
					cJSON_AddNumberToObject(psC1, "hum", (double)(uState.hum_sensor_state.hum)/100.0);
					break;
				case MUI_DATA_TYPE_TH_SENSOR:
					break;
				case MUI_DATA_TYPE_BIN_CTRL:
					cJSON_AddBoolToObject(psC1, "reachable", uState.bin_ctrl_state.reachable);
					cJSON_AddBoolToObject(psC1, "on", uState.bin_ctrl_state.on);
					break;
				case MUI_DATA_TYPE_LEVEL_CTRL:
					cJSON_AddBoolToObject(psC1, "reachable", uState.level_ctrl_state.reachable);
					cJSON_AddNumberToObject(psC1, "level", uState.level_ctrl_state.level);
					break;
				case MUI_DATA_TYPE_WARN_DEV:
					cJSON_AddBoolToObject(psC1, "reachable", uState.warn_dev_state.reachable);
					if (NULL != (psC2=cJSON_CreateObject()))
					{
						cJSON_AddItemToObject(psC1, "warncfg", psC2);
						cJSON_AddNumberToObject(psC2, "warnmode", uState.warn_dev_state.warncfg.warnmode);
						cJSON_AddNumberToObject(psC2, "duration", uState.warn_dev_state.warncfg.duration);
					}
					break;
				case MUI_DATA_TYPE_PLUG:
					cJSON_AddBoolToObject(psC1, "reachable", uState.plug_state.reachable);
					cJSON_AddBoolToObject(psC1, "on", uState.plug_state.on);
					cJSON_AddNumberToObject(psC1, "consumption", uState.plug_state.consumption);
					break;
				case MUI_DATA_TYPE_SWITCH:
					cJSON_AddBoolToObject(psC1, "reachable", uState.switch_state.reachable);
					switch (uState.switch_state.ctrlmode)
					{
						case MUI_SWITCH_CTRL_MODE_MUTI:
							cJSON_AddStringToObject(psC1, "ctrlmode", MSTR_SWITCH_CTRL_MODE_MULTI);
							break;
						case MUI_SWITCH_CTRL_MODE_LEVEL:
							cJSON_AddStringToObject(psC1, "ctrlmode", MSTR_SWITCH_CTRL_MODE_LEVEL);
							break;
						default: // MUI_SWITCH_CTRL_MODE_ON_OFF
							cJSON_AddStringToObject(psC1, "ctrlmode", MSTR_SWITCH_CTRL_MODE_ON_OFF);
							break;
					}
					cJSON_AddBoolToObject(psC1, "on", uState.switch_state.on);
					cJSON_AddNumberToObject(psC1, "level", uState.switch_state.level);
					break;
				case MUI_DATA_TYPE_SHADE:
					cJSON_AddBoolToObject(psC1, "reachable", uState.shade_state.reachable);
					cJSON_AddNumberToObject(psC1, "action", uState.shade_state.action);
					cJSON_AddNumberToObject(psC1, "level", uState.shade_state.level);
					break;
				case MUI_DATA_TYPE_SONOS:
					cJSON_AddBoolToObject(psC1, "reachable", uState.sonos_state.reachable);
					cJSON_AddNumberToObject(psC1, "play", uState.sonos_state.play);
					cJSON_AddNumberToObject(psC1, "volume", uState.sonos_state.volume);
					cJSON_AddNumberToObject(psC1, "urlid", uState.sonos_state.urlid);
					break;
				case MUI_DATA_TYPE_PANEL:
					cJSON_AddBoolToObject(psC1, "reachable", uState.panel_state.reachable);
					cJSON_AddBoolToObject(psC1, "on", uState.panel_state.on);
					cJSON_AddNumberToObject(psC1, "level", uState.panel_state.level);
					break;
				default:
					cJSON_AddStringToObject(psC1, "error", "Device type wrong.");
			}
		}
	}

	*ppsJson = psR;

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	组装设备状态数据JSON包函数
;	(功能)
;	(参数)
;		ui16DevType:	设备类型
;		ui16Id:			设备ID
;		ui8State:		状态值
;		ppsJson:		回传组装好的JSON结构体包指针的指针
;	(返回)
;		int:			0表示成功,负数表示出错
;	(注意)
*/
int json_obj_assemble_dev_state (uint16_t ui16DevType, uint16_t ui16Id, uint8_t ui8State, cJSON **ppsJson)
{
	U_DevState uState;
	cJSON *psR, *psC1, *psC2;
	double adXY[2];
	int iRet;

	ASSERT((MUI_DEV_STATE_REACHABLE<=ui8State) && (ui8State<=MUI_DEV_STATE_EXECUTE));

	if (NULL == (psR=cJSON_CreateObject()))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_dev_attr_state]Call 'cJSON_CreateObject()' fail.\r\n");
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	iRet = read_dev_state(ui16DevType, ui16Id, &uState);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_dev_attr_state]Call 'read_dev_state()' fail.\r\n");
		cJSON_Delete(psR);
		return iRet;
	}

	//State
	if (NULL != (psC1=cJSON_CreateObject()))
	{
		cJSON_AddItemToObject(psR, "state", psC1);
		switch (ui16DevType)
		{
			case MUI_DATA_TYPE_LIGHT:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.light_state.reachable);
						break;
					case MUI_DEV_STATE_ON:
						cJSON_AddBoolToObject(psC1, "on", uState.light_state.on);
						break;
					case MUI_DEV_STATE_BRI:
						cJSON_AddNumberToObject(psC1, "bri", uState.light_state.bri);
						break;
					case MUI_DEV_STATE_SAT:
						cJSON_AddNumberToObject(psC1, "sat", uState.light_state.sat);
						break;
					case MUI_DEV_STATE_HUE:
						cJSON_AddNumberToObject(psC1, "hue", uState.light_state.hue);
						break;
					case MUI_DEV_STATE_CT:
						cJSON_AddNumberToObject(psC1, "ct", uState.light_state.ct);
						break;
					case MUI_DEV_STATE_XY:
						adXY[0] = (double)uState.light_state.xy.x / 65536.0;
						adXY[1] = (double)uState.light_state.xy.y / 65536.0;
						cJSON_AddItemToObject(psC1, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
						break;
					case MUI_DEV_STATE_COLOR_MODE:
						switch (uState.light_state.colormode)
						{
							case MUI_LIGHT_COLOR_MODE_CT:
								cJSON_AddStringToObject(psC1, "colormode", MSTR_LIGHT_COLOR_MODE_CT);
								break;
							case MUI_LIGHT_COLOR_MODE_XY:
								cJSON_AddStringToObject(psC1, "colormode", MSTR_LIGHT_COLOR_MODE_XY);
								break;
							case MUI_LIGHT_COLOR_MODE_HS:
								cJSON_AddStringToObject(psC1, "colormode", MSTR_LIGHT_COLOR_MODE_HS);
								break;
							default: // MUI_LIGHT_COLOR_MODE_MC
								cJSON_AddStringToObject(psC1, "colormode", MSTR_LIGHT_COLOR_MODE_MC);
								break;
						}
						break;
					case MUI_DEV_STATE_ALERT:
						switch (uState.light_state.alert)
						{
							case 3:
								cJSON_AddStringToObject(psC1, "alert", MSTR_LIGHT_ALERT_LSELECT);
								break;
							case 2:
								cJSON_AddStringToObject(psC1, "alert", MSTR_LIGHT_ALERT_SELECT);
								break;
							default: // 1
								cJSON_AddStringToObject(psC1, "alert", MSTR_LIGHT_ALERT_NONE);
								break;
						}
						break;
					case MUI_DEV_STATE_EFFECT:
						if (2 == uState.light_state.effect)
						{
							cJSON_AddStringToObject(psC1, "effect", MSTR_LIGHT_EFFECT_COLOR_LOOP);
						}
						else
						{
							cJSON_AddStringToObject(psC1, "effect", MSTR_LIGHT_ALERT_NONE);
						}
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_OCC_SENSOR:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.occ_sensor_state.reachable);
						break;
					case MUI_DEV_STATE_POWER:
						cJSON_AddNumberToObject(psC1, "power", uState.occ_sensor_state.power);
						break;
					case MUI_DEV_STATE_ON:
						cJSON_AddBoolToObject(psC1, "on", uState.occ_sensor_state.on);
						break;
					case MUI_DEV_STATE_COUNT:
						cJSON_AddNumberToObject(psC1, "count", uState.occ_sensor_state.count);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_BIN_SENSOR:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.bin_sensor_state.reachable);
						break;
					case MUI_DEV_STATE_POWER:
						cJSON_AddNumberToObject(psC1, "power", uState.bin_sensor_state.power);
						break;
					case MUI_DEV_STATE_ON:
						cJSON_AddBoolToObject(psC1, "on", uState.bin_sensor_state.on);
						break;
					case MUI_DEV_STATE_COUNT:
						cJSON_AddNumberToObject(psC1, "count", uState.bin_sensor_state.count);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_TEMP_SENSOR:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.temp_sensor_state.reachable);
						break;
					case MUI_DEV_STATE_POWER:
						cJSON_AddNumberToObject(psC1, "power", uState.temp_sensor_state.power);
						break;
					case MUI_DEV_STATE_TEMP:
						cJSON_AddNumberToObject(psC1, "temp", (double)(uState.temp_sensor_state.temp)/100.0);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_HUM_SENSOR:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.hum_sensor_state.reachable);
						break;
					case MUI_DEV_STATE_POWER:
						cJSON_AddNumberToObject(psC1, "power", uState.hum_sensor_state.power);
						break;
					case MUI_DEV_STATE_HUM:
						cJSON_AddNumberToObject(psC1, "hum", (double)(uState.hum_sensor_state.hum)/100.0);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_TH_SENSOR:
				break;
			case MUI_DATA_TYPE_BIN_CTRL:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.bin_ctrl_state.reachable);
						break;
					case MUI_DEV_STATE_ON:
						cJSON_AddBoolToObject(psC1, "on", uState.bin_ctrl_state.on);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_LEVEL_CTRL:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.level_ctrl_state.reachable);
						break;
					case MUI_DEV_STATE_LEVEL:
						cJSON_AddNumberToObject(psC1, "level", uState.level_ctrl_state.level);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_WARN_DEV:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.warn_dev_state.reachable);
						break;
					case MUI_DEV_STATE_CONSUMPTION:
						if (NULL != (psC2=cJSON_CreateObject()))
						{
							cJSON_AddItemToObject(psC1, "warncfg", psC2);
							cJSON_AddNumberToObject(psC2, "warnmode", uState.warn_dev_state.warncfg.warnmode);
							cJSON_AddNumberToObject(psC2, "duration", uState.warn_dev_state.warncfg.duration);
						}
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_PLUG:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.plug_state.reachable);
						break;
					case MUI_DEV_STATE_ON:
						cJSON_AddBoolToObject(psC1, "on", uState.plug_state.on);
						break;
					case MUI_DEV_STATE_CONSUMPTION:
						cJSON_AddNumberToObject(psC1, "consumption", uState.plug_state.consumption);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_SWITCH:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.switch_state.reachable);
						break;
					case MUI_DEV_STATE_COLOR_MODE:
						switch (uState.switch_state.ctrlmode)
						{
							case MUI_SWITCH_CTRL_MODE_MUTI:
								cJSON_AddStringToObject(psC1, "ctrlmode", MSTR_SWITCH_CTRL_MODE_MULTI);
								break;
							case MUI_SWITCH_CTRL_MODE_LEVEL:
								cJSON_AddStringToObject(psC1, "ctrlmode", MSTR_SWITCH_CTRL_MODE_LEVEL);
								break;
							default: // MUI_SWITCH_CTRL_MODE_ON_OFF
								cJSON_AddStringToObject(psC1, "ctrlmode", MSTR_SWITCH_CTRL_MODE_ON_OFF);
								break;
						}
						break;
					case MUI_DEV_STATE_ON:
						cJSON_AddBoolToObject(psC1, "on", uState.switch_state.on);
						break;
					case MUI_DEV_STATE_LEVEL:
						cJSON_AddNumberToObject(psC1, "level", uState.switch_state.level);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_SHADE:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.shade_state.reachable);
						break;
					case MUI_DEV_STATE_ACTION:
						cJSON_AddNumberToObject(psC1, "action", uState.shade_state.action);
						break;
					case MUI_DEV_STATE_LEVEL:
						cJSON_AddNumberToObject(psC1, "level", uState.shade_state.level);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_SONOS:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.sonos_state.reachable);
						break;
					case MUI_DEV_STATE_PLAY:
						cJSON_AddNumberToObject(psC1, "play", uState.sonos_state.play);
						break;
					case MUI_DEV_STATE_VOLUME:
						cJSON_AddNumberToObject(psC1, "volume", uState.sonos_state.volume);
						break;
					case MUI_DEV_STATE_URLID:
						cJSON_AddNumberToObject(psC1, "urlid", uState.sonos_state.urlid);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			case MUI_DATA_TYPE_PANEL:
				switch (ui8State)
				{
					case MUI_DEV_STATE_REACHABLE:
						cJSON_AddBoolToObject(psC1, "reachable", uState.panel_state.reachable);
						break;
					case MUI_DEV_STATE_ON:
						cJSON_AddBoolToObject(psC1, "on", uState.panel_state.on);
						break;
					case MUI_DEV_STATE_LEVEL:
						cJSON_AddNumberToObject(psC1, "level", uState.panel_state.level);
						break;
					default:
						cJSON_AddStringToObject(psC1, "error", "State type wrong.");
				}
				break;
			default:
				cJSON_AddStringToObject(psC1, "error", "Device type wrong.");
		}
	}

	*ppsJson = psR;

	return 0;
}
//-------------------------------------------------------------------------------------------------*


void rules_get_time (const char *psTimeStr, S_ModeTime *psRulesTime)
{

	char* num = NULL;
	char Tmp_Str[MUI_RULES_TIME_LEN_MAX] = "";
	int count = 0;
	struct tm *timep;
	time_t now;

	while (count < MUI_RULES_TIME_LEN_MAX)
	{
		if (':' == psTimeStr[count] || '-' == psTimeStr[count])
		{
			Tmp_Str[count] = 0;
		}
		else
		{
			Tmp_Str[count] = psTimeStr[count];
		}
		count++;
	}

	if ('T' == psTimeStr[0])
	{
		time(&now);
		timep = localtime(&now);

		psRulesTime->Time.tm_year = timep->tm_year;
		psRulesTime->Time.tm_mon = timep->tm_mon;
		psRulesTime->Time.tm_mday = timep->tm_mday;
		psRulesTime->Time.tm_wday = timep->tm_wday;
		num = &Tmp_Str[1];
		psRulesTime->Time.tm_hour = atoi(num);
		num = &Tmp_Str[4];
		psRulesTime->Time.tm_min = atoi(num);
		num = &Tmp_Str[7];
		psRulesTime->Time.tm_sec = atoi(num);

		psRulesTime->time_mode = 1;
	}
	else
	{
		Tmp_Str[10] = 0;
		num = &Tmp_Str[0];
		psRulesTime->Time.tm_year = atoi(num) - 1900;
		num = &Tmp_Str[5];
		psRulesTime->Time.tm_mon = atoi(num) - 1;
		num = &Tmp_Str[8];
		psRulesTime->Time.tm_mday = atoi(num);
		num = &Tmp_Str[11];
		psRulesTime->Time.tm_hour = atoi(num);
		num = &Tmp_Str[14];
		psRulesTime->Time.tm_min = atoi(num);
		num = &Tmp_Str[17];
		psRulesTime->Time.tm_sec = atoi(num);

		psRulesTime->time_mode = 2;
	}
}














int json_assemble_answer_invalid_api (char **ppchJson, int *piErr)
{
	cJSON *psR, *psC1;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		cJSON_AddNumberToObject(psC1, "Invalid API", *piErr);

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}
int json_assemble_answer_low_layer_error (char **ppchJson, int *piErr)
{
	cJSON *psR, *psC1;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		cJSON_AddNumberToObject(psC1, "Low layer error", *piErr);

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_answer_link_button_not_pressed (char **ppchJson)
{
	cJSON *psR, *psC1, *psC2;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, "error", psC2);
		cJSON_AddNumberToObject(psC2, "type", 101);
		cJSON_AddStringToObject(psC2, "address", "");
		cJSON_AddStringToObject(psC2, "description", "link button not pressed");

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}
int json_assemble_answer_create_user_fail (char **ppchJson, S_ApiParseCreateUser *psParse)
{
	cJSON *psR, *psC1, *psC2;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, MSTR_API_ANSWER_FAILURE, psC2);
		cJSON_AddStringToObject(psC2, "username", psParse->name);

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}
int json_assemble_answer_create_user_succeed (char **ppchJson, S_ApiParseCreateUser *psParse)
{
	cJSON *psR, *psC1, *psC2;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, MSTR_API_ANSWER_SUCCESS, psC2);
		cJSON_AddStringToObject(psC2, "username", psParse->name);

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_answer_search_new_dev_fail (char **ppchJson, S_ApiParseSearchNewDev *psParse)
{
	cJSON *psR, *psC1, *psC2;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	UNUSED(psParse);

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, MSTR_API_ANSWER_FAILURE, psC2);
		cJSON_AddStringToObject(psC2, "/", "Search for new devices");

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}
int json_assemble_answer_search_new_dev_succeed (char **ppchJson, S_ApiParseSearchNewDev *psParse)
{
	cJSON *psR, *psC1, *psC2;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	UNUSED(psParse);

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, MSTR_API_ANSWER_SUCCESS, psC2);
		cJSON_AddStringToObject(psC2, "/", "Search for new devices");

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_answer_factory_new_fail (char **ppchJson, S_ApiParseFactoryNew *psParse)
{
	cJSON *psR, *psC1, *psC2;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	UNUSED(psParse);

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, MSTR_API_ANSWER_FAILURE, psC2);
		cJSON_AddStringToObject(psC2, "/", "Can't delete all devicess");

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}
int json_assemble_answer_factory_new_succeed (char **ppchJson, S_ApiParseFactoryNew *psParse)
{
	cJSON *psR, *psC1, *psC2;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;

	UNUSED(psParse);

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, MSTR_API_ANSWER_SUCCESS, psC2);
		cJSON_AddStringToObject(psC2, "/", "Have deleted all devices");

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_get_config (char **ppchJson)
{
	S_ConfigInfo sConfigInfo;
	cJSON *psR, *psC1;
	int iRet;

	iRet = read_config_info(&sConfigInfo);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_config]Call 'read_config_info()' fail.\r\n");
		return iRet;
	}

	iRet = JSON_DEAL_ERR_MEM_FAULT;
	if (NULL == (psR=cJSON_CreateObject()))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_config]Call 'cJSON_CreateObject()' fail.\r\n");
	}

	cJSON_AddStringToObject(psR, "sn", sConfigInfo.sn.str);
	cJSON_AddNumberToObject(psR, "databasever", sConfigInfo.databasever);

	cJSON_AddStringToObject(psR, "name", sConfigInfo.name.str);
	cJSON_AddStringToObject(psR, "swver", sConfigInfo.swver.str);
	cJSON_AddStringToObject(psR, "wifimac", sConfigInfo.wifimac.str);
	cJSON_AddStringToObject(psR, "mac", sConfigInfo.mac.str);
	cJSON_AddStringToObject(psR, "ipaddr", sConfigInfo.ipaddr.str);
	cJSON_AddStringToObject(psR, "netmask", sConfigInfo.netmask.str);
	cJSON_AddStringToObject(psR, "gwipaddr", sConfigInfo.gwipaddr.str);
	cJSON_AddBoolToObject(psR, "dhcp", sConfigInfo.dhcp);

	cJSON_AddStringToObject(psR, "uzipver", sConfigInfo.uzipver.str);
	cJSON_AddStringToObject(psR, "uzipmac", sConfigInfo.uzipmac.str);
	cJSON_AddStringToObject(psR, "uzipsn", sConfigInfo.uzipsn.str);

	cJSON_AddTimeToObject(psR, "utc", &sConfigInfo.utc.time);

	if (NULL != (psC1=cJSON_CreateObject()))
	{
		cJSON_AddItemToObject(psR, "swupdate", psC1);
		cJSON_AddBoolToObject(psC1, "updatestate", sConfigInfo.swupdate.updatestate);
		cJSON_AddStringToObject(psC1, "url", sConfigInfo.swupdate.url);
		cJSON_AddStringToObject(psC1, "text", sConfigInfo.swupdate.text);
		cJSON_AddNumberToObject(psC1, "notify", sConfigInfo.swupdate.notify);
	}

	cJSON_AddStringToObject(psR, "proxyaddr", sConfigInfo.proxyaddr.str);
	cJSON_AddNumberToObject(psR, "proxyport", sConfigInfo.proxyport);
	cJSON_AddBoolToObject(psR, "portalservices", sConfigInfo.portalservices);

	cJSON_AddStringToObject(psR, "devid", sConfigInfo.devid.str);
	cJSON_AddStringToObject(psR, "passwd", MSTR_CONFIG_PASSWD_SHOW_MASK);

	cJSON_AddBoolToObject(psR, "linkbutton", sConfigInfo.linkbutton);

	//uint32_t			whitelist;	//计数

	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_config]JSON print error.\r\n");
	}

	return iRet;
}

int json_assemble_get_dev_attr_state (uint16_t ui16DevType, uint16_t ui16Id, char **ppchJson)
{
	cJSON *psR;
	int iRet;

	iRet = json_obj_assemble_dev_data(ui16DevType, ui16Id, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR|MUI_API_DEV_DATA_ASM_JSON_CFG_ZB_ATTR|MUI_API_DEV_DATA_ASM_JSON_CFG_EXT_ATTR, &psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_dev_attr_state]Call 'json_obj_assemble_dev_data()' fail.\r\n");
		return iRet;
	}

	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_dev_attr_state]JSON print error.\r\n");
	}

	return iRet;
}

int json_assemble_answer_del_dev_fail (uint16_t ui16DevType, uint16_t ui16Id, char **ppchJson, S_ApiParseDelDev *psParse)
{
	char achName[MUI_API_URL_KEY_WORD_LEN_MAX+1+5+1];
	char achDescription[MUI_DEV_DESCRIPTIONLEN_MAX+64];
	cJSON *psR, *psC1, *psC2;
	int iLen, iRet = JSON_DEAL_ERR_MEM_FAULT;

	UNUSED(psParse);

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, MSTR_API_ANSWER_FAILURE, psC2);
		iLen = snprintf(achName, sizeof(achName), "%s/%u", s_aachDevUrlKeyWordList[ui16DevType-MUI_DATA_TYPE_DEV_START], ui16Id);
		ASSERT((size_t)iLen < sizeof(achName));
		iLen = snprintf(achDescription, sizeof(achDescription), "Can't delete %s %u.", s_aachDevDescriptionList[ui16DevType-MUI_DATA_TYPE_DEV_START], ui16Id);
		ASSERT((size_t)iLen < sizeof(achDescription));
		cJSON_AddStringToObject(psC2, achName, achDescription);

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}
int json_assemble_answer_del_dev_succeed (uint16_t ui16DevType, uint16_t ui16Id, char **ppchJson, S_ApiParseDelDev *psParse)
{
	char achName[MUI_API_URL_KEY_WORD_LEN_MAX+1+5+1];
	char achDescription[MUI_DEV_DESCRIPTIONLEN_MAX+64];
	cJSON *psR, *psC1, *psC2;
	int iLen, iRet = JSON_DEAL_ERR_MEM_FAULT;

	UNUSED(psParse);

	if (NULL == (psR=cJSON_CreateArray()))	return iRet;
	do
	{
		if (NULL == (psC1=cJSON_CreateObject()))	break;
		cJSON_AddItemToArray(psR, psC1);
		if (NULL == (psC2=cJSON_CreateObject()))	break;
		cJSON_AddItemToObject(psC1, MSTR_API_ANSWER_SUCCESS, psC2);
		iLen = snprintf(achName, sizeof(achName), "%s/%u", s_aachDevUrlKeyWordList[ui16DevType-MUI_DATA_TYPE_DEV_START], ui16Id);
		ASSERT((size_t)iLen < sizeof(achName));
		iLen = snprintf(achDescription, sizeof(achDescription), "Have deleted %s %u.", s_aachDevDescriptionList[ui16DevType-MUI_DATA_TYPE_DEV_START], ui16Id);
		ASSERT((size_t)iLen < sizeof(achDescription));
		cJSON_AddStringToObject(psC2, achName, achDescription);

		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_get_group_attr(uint16_t ui16Id, char **ppchJson)
{
	S_GroupAttrFlash sGroupAttr;
	cJSON *psR, *psC1, *psC2;
	char achBuf[1+MUI_API_KEYWORD_LEN_MAX+1+5+1];	// /dev-type/ID
	int iRet, i;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	iRet = group_read_info(ui16Id, &sGroupAttr);
	if ((iRet<0) || (sGroupAttr.op_attr.total>MUI_DEV_NUM_MAX_PER_GROUP))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_get_group_attr]Call 'group_read_info() fail.\r\n");
		cJSON_Delete(psR);
		return iRet;
	}

	cJSON_AddStringToObject(psR, "name", sGroupAttr.api_attr.name.str);
	cJSON_AddStringToObject(psR, "description", sGroupAttr.api_attr.descr.str);

	if (NULL == (psC1=cJSON_CreateObject()))
	{
		cJSON_Delete(psR);
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	cJSON_AddItemToObject(psR, "config", psC1);
	for (i = 0; i < (int)sGroupAttr.op_attr.total; i++)
	{
		if (NULL == (psC2=cJSON_CreateObject()))
		{
			break;
		}

		snprintf(achBuf, sizeof(achBuf), "%u", (uint16_t)(i+MUI_DATABASE_ID_START));
		cJSON_AddItemToObject(psC1, achBuf, psC2);

		snprintf(achBuf, sizeof(achBuf), "/%s/%u", s_asGetFullStateKeywordMapList[sGroupAttr.op_attr.member[i].devType-1].achKeyword, sGroupAttr.op_attr.member[i].devId);
		cJSON_AddStringToObject(psC2, "address", achBuf);
	}

	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_answer_get_all_set(char **ppchJson)
{
	int iRet, i = 0;
	int num;
	S_AttrName setname;
	char idstr[5]="";
	uint16_t idbuf[MUI_SET_DATA_NUM_MAX];
	cJSON *psR, *psC1;

	num = set_get_id_list(idbuf, MUI_SET_DATA_NUM_MAX);

	if (NULL == (psR=cJSON_CreateObject()))	return JSON_DEAL_ERR_MEM_FAULT;
	for (i=0; i<num; i++)
	{
		iRet = set_read_name(idbuf[i], &setname);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_answer_get_all_group]Call 'set_read_name' fail.:%d\r\n", iRet);
			continue;
		}
		if (NULL == (psC1=cJSON_CreateObject())) break;
		sprintf(idstr, "%d", idbuf[i]);
		cJSON_AddItemToObject(psR, idstr, psC1);
		cJSON_AddStringToObject(psC1, "name", setname.str);

	}
	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_answer_get_set_attr(uint16_t ui16Id, char **ppchJson)
{
	cJSON *psR, *psC1, *psC2, *psC3;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;
	int i;
	char idstr[2]="";
	char address[30]="";
	S_SetAttrFlash setflash;
	S_SetMember setmember;
	double adXY[2];

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = set_read_info(ui16Id, &setflash, sizeof(S_SetAttrFlash));
		if (iRet < 0)
		{
			break;
		}
		cJSON_AddStringToObject(psR, "name", setflash.name.str);
		cJSON_AddStringToObject(psR, "description", setflash.descr.str);
		cJSON_AddNumberToObject(psR, "delay", setflash.delay);
		cJSON_AddBoolToObject(psR, "execute", set_get_execute_state(ui16Id));

		if (NULL == (psC1=cJSON_CreateObject())) break;
		cJSON_AddItemToObject(psR, "config", psC1);

		for (i=0; i<setflash.action_num; i++)
		{
			if (NULL == (psC2=cJSON_CreateObject())) break;

			iRet = set_read_action(ui16Id, i, &setmember);
			if (iRet < 0)
			{
				JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_answer_get_set_attr]Call 'set_read_action' fail:%d.\r\n",iRet);
				return iRet;
			}
			sprintf(idstr, "%d", i+1);
			cJSON_AddItemToObject(psC1, idstr, psC2);

			//setmember.dev_type-1 是因为数组是从0开始的
			sprintf(address, "/%s/%d", s_asGetFullStateKeywordMapList[setmember.dev_type-1].achKeyword, setmember.dev_id);
			cJSON_AddStringToObject(psC2, "address", address);

			if (NULL == (psC3=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC2, "action", psC3);

			switch (setmember.action_type)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC3, "on", setmember.uSetAtion.on);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					cJSON_AddNumberToObject(psC3, "bri", setmember.uSetAtion.bri);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					cJSON_AddNumberToObject(psC3, "sat", setmember.uSetAtion.sat);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					cJSON_AddNumberToObject(psC3, "hue", setmember.uSetAtion.hue);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					cJSON_AddNumberToObject(psC3, "ct", setmember.uSetAtion.ct);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					adXY[0] = (double)setmember.uSetAtion.xy.x / 65536.0;
					adXY[1] = (double)setmember.uSetAtion.xy.y / 65536.0;
					cJSON_AddItemToObject(psC3, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					cJSON_AddNumberToObject(psC3, "level", setmember.uSetAtion.level);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
					cJSON_AddNumberToObject(psC3, "temp", (double)(setmember.uSetAtion.temp)/100.0);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					cJSON_AddNumberToObject(psC3, "warnmode", setmember.uSetAtion.warnmode);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					cJSON_AddNumberToObject(psC3, "duration", setmember.uSetAtion.duration);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					//cJSON_AddItemToObject(psC3, "warncfg", psC4);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					cJSON_AddNumberToObject(psC3, "action", setmember.uSetAtion.action);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					cJSON_AddNumberToObject(psC3, "play", setmember.uSetAtion.play);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					cJSON_AddNumberToObject(psC3, "volume", setmember.uSetAtion.volume);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					cJSON_AddNumberToObject(psC3, "urlid", setmember.uSetAtion.urlid);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					cJSON_AddBoolToObject(psC3, "execute", setmember.uSetAtion.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					cJSON_AddBoolToObject(psC3, "active", setmember.uSetAtion.active);
					break;
				default:
					break;
			}
		}

	} while (0);
	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);
	return iRet;
}

int json_assemble_answer_del_set(uint16_t ui16Id, char **ppchJson)
{
	cJSON *psR;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;
	char buf[20]="";

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = set_del(ui16Id);
		sprintf(buf, MSTR_API_URL_KEY_WORD_SET"/%d deleted.", ui16Id);
		if (iRet < 0)
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE, buf);
		}else
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS, buf);
		}
		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;

}

int json_assemble_answer_get_all_scene(char **ppchJson)
{
	int iRet, i = 0;
	int num;
	S_AttrName setname;
	char idstr[5]="";
	uint16_t idbuf[MUI_SCENE_DATA_NUM_MAX];
	cJSON *psR, *psC1;

	num = scene_get_id_list(idbuf, MUI_SCENE_DATA_NUM_MAX);

	if (NULL == (psR=cJSON_CreateObject()))	return JSON_DEAL_ERR_MEM_FAULT;
	for (i=0; i<num; i++)
	{
		iRet = scene_read_name(idbuf[i], &setname);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_answer_get_all_scene]Call 'scene_read_name' fail.:%d\r\n", iRet);
			continue;
		}
		if (NULL == (psC1=cJSON_CreateObject())) break;
		sprintf(idstr, "%d", idbuf[i]);
		cJSON_AddItemToObject(psR, idstr, psC1);
		cJSON_AddStringToObject(psC1, "name", setname.str);

	}
	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_answer_get_scene_attr(uint16_t ui16Id, char **ppchJson)
{
	cJSON *psR, *psC1, *psC2, *psC3;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;
	int i;
	char idstr[2]="";
	char address[30]="";
	S_SceneDataFalsh sceneFlash;
	double adXY[2];

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = scene_read_info(ui16Id, &sceneFlash, sizeof(S_SceneDataFalsh));
		if (iRet < 0)
		{
			break;
		}
		cJSON_AddStringToObject(psR, "name", sceneFlash.scene_attr.name.str);
		cJSON_AddStringToObject(psR, "description", sceneFlash.scene_attr.descr.str);
		//cJSON_AddBoolToObject(psR, "execute", scene_get_execute_state(ui16Id));

		if (NULL == (psC1=cJSON_CreateObject())) break;
		cJSON_AddItemToObject(psR, "config", psC1);

		for (i=0; i<sceneFlash.scene_attr.action_num; i++)
		{
			if (NULL == (psC2=cJSON_CreateObject())) break;

			sprintf(idstr, "%d", i+1);
			cJSON_AddItemToObject(psC1, idstr, psC2);

			//setmember.dev_type-1 是因为数组是从0开始的
			sprintf(address, "/%s/%d", s_asGetFullStateKeywordMapList[sceneFlash.scene_member[i].dev_type-1].achKeyword, sceneFlash.scene_member[i].dev_id);
			cJSON_AddStringToObject(psC2, "address", address);

			if (NULL == (psC3=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC2, "action", psC3);

			switch (sceneFlash.scene_member[i].action_type)
			{
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC3, "on", sceneFlash.scene_member[i].uSceneAtion.on);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_BRI:
					cJSON_AddNumberToObject(psC3, "bri", sceneFlash.scene_member[i].uSceneAtion.bri);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_SAT:
					cJSON_AddNumberToObject(psC3, "sat", sceneFlash.scene_member[i].uSceneAtion.sat);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUE:
					cJSON_AddNumberToObject(psC3, "hue", sceneFlash.scene_member[i].uSceneAtion.hue);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_CT:
					cJSON_AddNumberToObject(psC3, "ct", sceneFlash.scene_member[i].uSceneAtion.ct);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_XY:
					adXY[0] = (double)sceneFlash.scene_member[i].uSceneAtion.xy.x / 65536.0;
					adXY[1] = (double)sceneFlash.scene_member[i].uSceneAtion.xy.y / 65536.0;
					cJSON_AddItemToObject(psC3, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_LEVEL:
					cJSON_AddNumberToObject(psC3, "level", sceneFlash.scene_member[i].uSceneAtion.level);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_TEMP:
					cJSON_AddNumberToObject(psC3, "temp", (double)(sceneFlash.scene_member[i].uSceneAtion.temp)/100.0);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUM:
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					cJSON_AddNumberToObject(psC3, "warnmode", sceneFlash.scene_member[i].uSceneAtion.warnmode);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_DURATION:
					cJSON_AddNumberToObject(psC3, "duration", sceneFlash.scene_member[i].uSceneAtion.duration);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					//cJSON_AddItemToObject(psC3, "warncfg", psC4);
					break;
				case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ACTION:
					cJSON_AddNumberToObject(psC3, "action", sceneFlash.scene_member[i].uSceneAtion.action);
					break;
				default:
					break;
			}
		}

	} while (0);
	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);
	return iRet;
}

int json_assemble_answer_del_scene(uint16_t ui16Id, char **ppchJson)
{
	cJSON *psR;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;
	char buf[20]="";

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = scene_del(ui16Id);
		sprintf(buf, MSTR_API_URL_KEY_WORD_SCENE"/%d deleted.", ui16Id);
		if (iRet < 0)
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE, buf);
		}else
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS, buf);
		}
		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;

}



int json_assemble_answer_get_all_schedule(char **ppchJson)
{
	int iRet, i = 0;
	int num;
	S_AttrName ScheduleName;
	char idstr[5]="";
	uint16_t idbuf[MUI_SCHEDULE_DATA_NUM_MAX];
	cJSON *psR, *psC1;

	num = schedule_get_id_list(idbuf, MUI_SCHEDULE_DATA_NUM_MAX);

	if (NULL == (psR=cJSON_CreateObject()))	return JSON_DEAL_ERR_MEM_FAULT;
	for (i=0; i<num; i++)
	{
		iRet = schedule_read_name(idbuf[i], &ScheduleName);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_answer_get_all_schedule]Call 'schedule_read_name' fail.:%d\r\n", iRet);
			continue;
		}
		if (NULL == (psC1=cJSON_CreateObject())) break;
		sprintf(idstr, "%d", idbuf[i]);
		cJSON_AddItemToObject(psR, idstr, psC1);
		cJSON_AddStringToObject(psC1, "name", ScheduleName.str);

	}
	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_answer_get_schedule_attr(uint16_t ui16Id, char **ppchJson)
{
	cJSON 				*psR, *psOutput, *psC1, *psC2, *psC3;
	int					iRet = JSON_DEAL_ERR_MEM_FAULT;
	int 				i;
	char 				idstr[2]="";
	char 				address[30]="";
	char 				time[20]="";
	S_ApiParseSchedule 	schedulefalsh;

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = schedule_read_info(ui16Id, &schedulefalsh);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_answer_get_schedule_attr] Call :schedule_read_info failed :%d\r\n", iRet);
			break;
		}
		cJSON_AddStringToObject(psR, "name", schedulefalsh.ScheduleAttr.name.str);
		cJSON_AddStringToObject(psR, "description", schedulefalsh.ScheduleAttr.descr.str);
		cJSON_AddNumberToObject(psR, "repeat", schedulefalsh.ScheduleAttr.repeat);
		if (schedulefalsh.ScheduleAttr.time.time_mode == 2)
		{
			sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02d", schedulefalsh.ScheduleAttr.time.Time.tm_year, schedulefalsh.ScheduleAttr.time.Time.tm_mon, schedulefalsh.ScheduleAttr.time.Time.tm_mday, schedulefalsh.ScheduleAttr.time.Time.tm_hour, schedulefalsh.ScheduleAttr.time.Time.tm_min, schedulefalsh.ScheduleAttr.time.Time.tm_sec);
		}
		else if (schedulefalsh.ScheduleAttr.time.time_mode == 1)
		{
			sprintf(time, "T%02d:%02d:%02d", schedulefalsh.ScheduleAttr.time.Time.tm_hour, schedulefalsh.ScheduleAttr.time.Time.tm_min, schedulefalsh.ScheduleAttr.time.Time.tm_sec);
		}
		cJSON_AddStringToObject(psR, "time", time);
		//JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "schedule time = %s\r\n", time);

		//if (NULL == (psC1=cJSON_CreateObject())) break;
		if (NULL == (psOutput=cJSON_CreateObject()))
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_answer_get_schedule_attr] Call 'cJSON_CreateObject'failed\r\n");
			break;
		}
		//cJSON_AddItemToObject(psC1, "output", psOutput);

		for (i=0; i<schedulefalsh.ScheduleConfig.OutputNum; i++)
		{
			if (NULL == (psC1=cJSON_CreateObject())) break;

			sprintf(idstr, "%d", i+1);
			cJSON_AddItemToObject(psOutput, idstr, psC1);

			//schedulefalsh.ScheduleConfig.OutputAction[i].DevId-1 是因为数组是从0开始的
			sprintf(address, "/%s/%d", s_asGetFullStateKeywordMapList[schedulefalsh.ScheduleConfig.OutputAction[i].DevType-1].achKeyword, schedulefalsh.ScheduleConfig.OutputAction[i].DevId);
			cJSON_AddStringToObject(psC1, "address", address);

			cJSON_AddNumberToObject(psC1, "delay", schedulefalsh.ScheduleConfig.OutputAction[i].delay);
			cJSON_AddNumberToObject(psC1, "duration", schedulefalsh.ScheduleConfig.OutputAction[i].duration);

			if (NULL == (psC2=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC1, "action", psC2);

			switch (schedulefalsh.ScheduleConfig.OutputAction[i].ActionType)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC2, "on", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.on);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					cJSON_AddNumberToObject(psC2, "bri", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.bri);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					cJSON_AddNumberToObject(psC2, "sat", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.sat);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					cJSON_AddNumberToObject(psC2, "hue", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.hue);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					cJSON_AddNumberToObject(psC2, "ct", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.ct);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					//adXY[0] = (double)schedulefalsh.ScheduleConfig.OutputAction[i].action.ActionValue.xy.x / 65536.0;
					//adXY[1] = (double)schedulefalsh.ScheduleConfig.OutputAction[i].action.ActionValue.xy.y / 65536.0;
					//cJSON_AddItemToObject(psC2, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					cJSON_AddNumberToObject(psC2, "level", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.level);
					break;
				//case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				//	cJSON_AddNumberToObject(psC2, "temp", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.temp.temp);
				//	break;
				//case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				//	cJSON_AddNumberToObject(psC2, "hum", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.hum.humidity);
				//	break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					cJSON_AddNumberToObject(psC2, "warnmode", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.warnmode);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					cJSON_AddNumberToObject(psC2, "duration", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.duration);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					//**************************************************
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					cJSON_AddNumberToObject(psC2, "action", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.action);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					cJSON_AddNumberToObject(psC2, "play", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.play);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					cJSON_AddNumberToObject(psC2, "volume", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.volume);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					cJSON_AddNumberToObject(psC2, "urlid", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.urlid);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					cJSON_AddBoolToObject(psC2, "execute", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					cJSON_AddNumberToObject(psC2, "active", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.active);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC:
					cJSON_AddNumberToObject(psC2, "onsec", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.blink.onsec);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC:
					cJSON_AddNumberToObject(psC2, "offsec", schedulefalsh.ScheduleConfig.OutputAction[i].ActionValue.blink.offsec);
					break;
				default :
					break;
			}
			cJSON_AddNumberToObject(psC1, "quitmethod", schedulefalsh.ScheduleConfig.OutputAction[i].quitmethod);
			if (NULL == (psC3=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC1, "quitstate", psC3);

			switch (schedulefalsh.ScheduleConfig.OutputAction[i].quitType)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC3, "on", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.on);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					cJSON_AddNumberToObject(psC3, "bri", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.bri);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					cJSON_AddNumberToObject(psC3, "sat", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.sat);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					cJSON_AddNumberToObject(psC3, "hue", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.hue);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					cJSON_AddNumberToObject(psC3, "ct", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.ct);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					//adXY[0] = (double)schedulefalsh.ScheduleConfig.OutputAction[i].quitstate.ActionValue.xy.x / 65536.0;
					//adXY[1] = (double)schedulefalsh.ScheduleConfig.OutputAction[i].quitstate.ActionValue.xy.y / 65536.0;
					//cJSON_AddItemToObject(psC3, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					cJSON_AddNumberToObject(psC3, "level", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.level);
					break;
				//case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				//	cJSON_AddNumberToObject(psC3, "temp", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.temp.temp);
				//	break;
				//case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				//	cJSON_AddNumberToObject(psC3, "hum", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.hum.humidity);
				//	break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					cJSON_AddNumberToObject(psC3, "warnmode", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.warnmode);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					cJSON_AddNumberToObject(psC3, "duration", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.duration);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					//**************************************************
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					cJSON_AddNumberToObject(psC3, "action", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.action);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					cJSON_AddNumberToObject(psC3, "play", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.play);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					cJSON_AddNumberToObject(psC3, "volume", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.volume);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					cJSON_AddNumberToObject(psC3, "urlid", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.urlid);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					cJSON_AddBoolToObject(psC3, "execute", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					cJSON_AddNumberToObject(psC3, "active", schedulefalsh.ScheduleConfig.OutputAction[i].quitValue.active);
					break;
				default :
					break;
			}
		}
		cJSON_AddItemToObject(psR, "config", psOutput);
	} while (0);
	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);
	return iRet;
}

int json_assemble_answer_del_schedule(uint16_t ui16Id, char **ppchJson)
{
	cJSON *psR;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;
	char buf[20]="";

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = schedule_del(ui16Id);
		sprintf(buf, MSTR_API_URL_KEY_WORD_SCHEDULES"/%d deleted.", ui16Id);
		if (iRet < 0)
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE, buf);
		}else
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS, buf);
		}
		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;

}

int json_assemble_answer_get_all_rules(char **ppchJson)
{
	int iRet, i = 0;
	int num;
	S_AttrName RulesName;
	char idstr[5]="";
	uint16_t idbuf[MUI_RULE_DATA_NUM_MAX];
	cJSON *psR, *psC1;

	num = rules_get_id_list(idbuf, MUI_RULE_DATA_NUM_MAX);

	if (NULL == (psR=cJSON_CreateObject()))	return JSON_DEAL_ERR_MEM_FAULT;
	for (i=0; i<num; i++)
	{
		iRet = rules_read_name(idbuf[i], &RulesName);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_answer_get_all_rules]Call 'rules_read_name' fail.:%d\r\n", iRet);
			continue;
		}
		if (NULL == (psC1=cJSON_CreateObject())) break;
		sprintf(idstr, "%d", idbuf[i]);
		cJSON_AddItemToObject(psR, idstr, psC1);
		cJSON_AddStringToObject(psC1, "name", RulesName.str);

	}
	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);

	return iRet;
}

int json_assemble_answer_get_rules_attr(uint16_t ui16Id, char **ppchJson)
{
	cJSON 				*psR, *psInput, *psToutput, *psFoutput, *psC1, *psC2, *psC3, *psC4;
	int					iRet = JSON_DEAL_ERR_MEM_FAULT;
	int 				i;
	char 				idstr[2]="";
	char 				address[30]="";
	char 				time[20]="";
	double 				adXY[2];
	S_ApiParseRules 	rulesflash;

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = rules_read_info(ui16Id, &rulesflash);
		if (iRet < 0)
		{
			break;
		}
		cJSON_AddStringToObject(psR, "name", rulesflash.RulesAttr.name.str);
		cJSON_AddStringToObject(psR, "description", rulesflash.RulesAttr.descr.str);
		cJSON_AddBoolToObject(psR, "active", rulesflash.RulesAttr.active);
		cJSON_AddNumberToObject(psR, "repeat", rulesflash.RulesAttr.repeat);
		if (rulesflash.RulesAttr.time.time_mode == 2)
		{
			sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02d", rulesflash.RulesAttr.time.Time.tm_year, rulesflash.RulesAttr.time.Time.tm_mon, rulesflash.RulesAttr.time.Time.tm_mday, rulesflash.RulesAttr.time.Time.tm_hour, rulesflash.RulesAttr.time.Time.tm_min, rulesflash.RulesAttr.time.Time.tm_sec);
		}
		else if (rulesflash.RulesAttr.time.time_mode == 1)
		{
			sprintf(time, "T%02d:%02d:%02d", rulesflash.RulesAttr.time.Time.tm_hour, rulesflash.RulesAttr.time.Time.tm_min, rulesflash.RulesAttr.time.Time.tm_sec);
		}
		cJSON_AddStringToObject(psR, "time", time);
		cJSON_AddNumberToObject(psR, "duration", rulesflash.RulesAttr.duration);

		if (NULL == (psC1=cJSON_CreateObject())) break;
		cJSON_AddItemToObject(psR, "config", psC1);
		if (NULL == (psInput=cJSON_CreateObject())) break;
		cJSON_AddItemToObject(psC1, "input", psInput);


		for (i=0; i<rulesflash.RulesConfig.InputNum; i++)
		{
			if (NULL == (psC2=cJSON_CreateObject())) break;

			sprintf(idstr, "%d", i+1);
			cJSON_AddItemToObject(psInput, idstr, psC2);

			//RulesInput.DevId-1 是因为数组是从0开始的
			sprintf(address, "/%s/%d", s_asGetFullStateKeywordMapList[rulesflash.RulesConfig.InputAction[i].DevType-1].achKeyword, rulesflash.RulesConfig.InputAction[i].DevId);
			cJSON_AddStringToObject(psC2, "address", address);

			if (NULL == (psC3=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC2, "state", psC3);

			switch (rulesflash.RulesConfig.InputAction[i].ActionType)
			{
				case MUI_RULES_ACTION_STATE_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC3, "on", rulesflash.RulesConfig.InputAction[i].ActionValue.on);
					break;
				case MUI_RULES_ACTION_STATE_WORD_IDX_TEMP:
					cJSON_AddNumberToObject(psC3, "temp", rulesflash.RulesConfig.InputAction[i].ActionValue.temp.temp);
					cJSON_AddBoolToObject(psC3, "less", rulesflash.RulesConfig.InputAction[i].ActionValue.temp.less);
					break;
				case MUI_RULES_ACTION_STATE_WORD_IDX_HUM:
					cJSON_AddNumberToObject(psC3, "hum", rulesflash.RulesConfig.InputAction[i].ActionValue.hum.humidity);
					cJSON_AddBoolToObject(psC3, "less", rulesflash.RulesConfig.InputAction[i].ActionValue.hum.less);
					break;
				case MUI_RULES_ACTION_STATE_WORD_IDX_EDGE:
					if (rulesflash.RulesConfig.InputAction[i].ActionValue.edge == 1)
					{
						cJSON_AddStringToObject(psC3, "edge", "up");
					}
					else if (rulesflash.RulesConfig.InputAction[i].ActionValue.edge == 2)
					{
						cJSON_AddStringToObject(psC3, "edge", "down");
					}
					else if (rulesflash.RulesConfig.InputAction[i].ActionValue.edge == 3)
					{
						cJSON_AddStringToObject(psC3, "edge", "all");
					}
				default:
					break;
			}
		}
		cJSON_AddStringToObject(psInput, "expression", rulesflash.RulesConfig.expersion);

		if (NULL == (psToutput=cJSON_CreateObject())) break;
		cJSON_AddItemToObject(psC1, "true-output", psToutput);

		for (i=0; i<rulesflash.RulesConfig.ToutputNum; i++)
		{
			if (NULL == (psC2=cJSON_CreateObject())) break;

			sprintf(idstr, "%d", i+1);
			cJSON_AddItemToObject(psToutput, idstr, psC2);

			//RulesInput.DevId-1 是因为数组是从0开始的
			sprintf(address, "/%s/%d", s_asGetFullStateKeywordMapList[rulesflash.RulesConfig.ToutputAction[i].DevType-1].achKeyword, rulesflash.RulesConfig.ToutputAction[i].DevId);
			cJSON_AddStringToObject(psC2, "address", address);

			cJSON_AddNumberToObject(psC2, "delay", rulesflash.RulesConfig.ToutputAction[i].delay);
			cJSON_AddBoolToObject(psC2, "delayoverlay", rulesflash.RulesConfig.ToutputAction[i].delayoverlay);
			cJSON_AddNumberToObject(psC2, "duration", rulesflash.RulesConfig.ToutputAction[i].duration);
			cJSON_AddBoolToObject(psC2, "durationoverlay", rulesflash.RulesConfig.ToutputAction[i].durationoverlay);

			if (NULL == (psC3=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC2, "action", psC3);

			switch (rulesflash.RulesConfig.ToutputAction[i].action.ActionType)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC3, "on", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.on);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					cJSON_AddNumberToObject(psC3, "bri", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.bri);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					cJSON_AddNumberToObject(psC3, "sat", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.sat);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					cJSON_AddNumberToObject(psC3, "hue", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.hue);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					cJSON_AddNumberToObject(psC3, "ct", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.ct);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					adXY[0] = (double)rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.xy.x / 65536.0;
					adXY[1] = (double)rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.xy.y / 65536.0;
					//cJSON_AddItemToObject(psC3, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					cJSON_AddItemToObject(psC3, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					cJSON_AddNumberToObject(psC3, "level", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.level);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
					cJSON_AddNumberToObject(psC3, "temp", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.temp.temp);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
					cJSON_AddNumberToObject(psC3, "hum", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.hum.humidity);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					cJSON_AddNumberToObject(psC3, "warnmode", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.warnmode);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					cJSON_AddNumberToObject(psC3, "duration", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.duration);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					//**************************************************
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					cJSON_AddNumberToObject(psC3, "action", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.action);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					cJSON_AddNumberToObject(psC3, "play", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.play);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					cJSON_AddNumberToObject(psC3, "volume", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.volume);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					cJSON_AddNumberToObject(psC3, "urlid", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.urlid);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					cJSON_AddBoolToObject(psC3, "execute", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					cJSON_AddNumberToObject(psC3, "active", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.active);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC:
					cJSON_AddNumberToObject(psC3, "onsec", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.blink.onsec);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC:
					cJSON_AddNumberToObject(psC3, "offsec", rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.blink.offsec);
					break;
				default :
					break;
			}
			cJSON_AddNumberToObject(psC2, "quitmethod", rulesflash.RulesConfig.ToutputAction[i].quitmethod);
			if (NULL == (psC4=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC2, "quitstate", psC4);

			switch (rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionType)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC4, "on", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.on);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					cJSON_AddNumberToObject(psC4, "bri", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.bri);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					cJSON_AddNumberToObject(psC4, "sat", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.sat);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					cJSON_AddNumberToObject(psC4, "hue", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.hue);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					cJSON_AddNumberToObject(psC4, "ct", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.ct);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					adXY[0] = (double)rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.xy.x / 65536.0;
					adXY[1] = (double)rulesflash.RulesConfig.ToutputAction[i].action.ActionValue.xy.y / 65536.0;
					//cJSON_AddItemToObject(psC3, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					cJSON_AddItemToObject(psC4, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					cJSON_AddNumberToObject(psC4, "level", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.level);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
					cJSON_AddNumberToObject(psC4, "temp", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.temp.temp);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
					cJSON_AddNumberToObject(psC4, "hum", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.hum.humidity);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					cJSON_AddNumberToObject(psC4, "warnmode", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.warnmode);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					cJSON_AddNumberToObject(psC4, "duration", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.duration);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					//**************************************************
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					cJSON_AddNumberToObject(psC4, "action", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.action);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					cJSON_AddNumberToObject(psC4, "play", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.play);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					cJSON_AddNumberToObject(psC4, "volume", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.volume);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					cJSON_AddNumberToObject(psC4, "urlid", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.urlid);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					cJSON_AddBoolToObject(psC4, "execute", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					cJSON_AddNumberToObject(psC4, "active", rulesflash.RulesConfig.ToutputAction[i].quitstate.ActionValue.active);
					break;
				default :
					break;
			}
		}

		if (NULL == (psFoutput=cJSON_CreateObject())) break;
		cJSON_AddItemToObject(psC1, "false-output", psFoutput);

		for (i=0; i<rulesflash.RulesConfig.FoutputNum; i++)
		{
			if (NULL == (psC2=cJSON_CreateObject())) break;

			sprintf(idstr, "%d", i+1);
			cJSON_AddItemToObject(psFoutput, idstr, psC2);

			//RulesInput.DevId-1 是因为数组是从0开始的
			sprintf(address, "/%s/%d", s_asGetFullStateKeywordMapList[rulesflash.RulesConfig.FoutputAction[i].DevType-1].achKeyword, rulesflash.RulesConfig.FoutputAction[i].DevId);
			cJSON_AddStringToObject(psC2, "address", address);

			cJSON_AddNumberToObject(psC2, "delay", rulesflash.RulesConfig.FoutputAction[i].delay);
			cJSON_AddBoolToObject(psC2, "delayoverlay", rulesflash.RulesConfig.FoutputAction[i].delayoverlay);
			cJSON_AddNumberToObject(psC2, "duration", rulesflash.RulesConfig.FoutputAction[i].duration);
			cJSON_AddBoolToObject(psC2, "durationoverlay", rulesflash.RulesConfig.FoutputAction[i].durationoverlay);

			if (NULL == (psC3=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC2, "action", psC3);

			switch (rulesflash.RulesConfig.FoutputAction[i].action.ActionType)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC3, "on", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.on);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					cJSON_AddNumberToObject(psC3, "bri", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.bri);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					cJSON_AddNumberToObject(psC3, "sat", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.sat);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					cJSON_AddNumberToObject(psC3, "hue", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.hue);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					cJSON_AddNumberToObject(psC3, "ct", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.ct);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					//adXY[0] = (double)RulesOutput.action.ActionValue.xy.x / 65536.0;
					//adXY[1] = (double)RulesOutput.action.ActionValue.xy.y / 65536.0;
					//cJSON_AddItemToObject(psC3, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					cJSON_AddNumberToObject(psC3, "level", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.level);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
					cJSON_AddNumberToObject(psC3, "temp", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.temp.temp);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
					cJSON_AddNumberToObject(psC3, "hum", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.hum.humidity);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					cJSON_AddNumberToObject(psC3, "warnmode", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.warnmode);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					cJSON_AddNumberToObject(psC3, "duration", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.duration);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					//**************************************************
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					cJSON_AddNumberToObject(psC3, "action", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.action);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					cJSON_AddNumberToObject(psC3, "play", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.play);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					cJSON_AddNumberToObject(psC3, "volume", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.volume);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					cJSON_AddNumberToObject(psC3, "urlid", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.urlid);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					cJSON_AddBoolToObject(psC3, "execute", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					cJSON_AddNumberToObject(psC3, "active", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.active);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC:
					cJSON_AddNumberToObject(psC3, "onsec", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.blink.onsec);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC:
					cJSON_AddNumberToObject(psC3, "offsec", rulesflash.RulesConfig.FoutputAction[i].action.ActionValue.blink.offsec);
					break;
				default :
					break;
			}
			cJSON_AddNumberToObject(psC2, "quitmethod", rulesflash.RulesConfig.FoutputAction[i].quitmethod);

			if (NULL == (psC4=cJSON_CreateObject())) break;
			cJSON_AddItemToObject(psC2, "quitstate", psC4);

			switch (rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionType)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					cJSON_AddBoolToObject(psC4, "on", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.on);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					cJSON_AddNumberToObject(psC4, "bri", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.bri);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					cJSON_AddNumberToObject(psC4, "sat", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.sat);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					cJSON_AddNumberToObject(psC4, "hue", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.hue);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					cJSON_AddNumberToObject(psC4, "ct", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.ct);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					//adXY[0] = (double)RulesOutput.quitstate.ActionValue.xy.x / 65536.0;
					//adXY[1] = (double)RulesOutput.quitstate.ActionValue.xy.y / 65536.0;
					//cJSON_AddItemToObject(psC4, "xy", cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0])));
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					cJSON_AddNumberToObject(psC4, "level", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.level);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
					cJSON_AddNumberToObject(psC4, "temp", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.temp.temp);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
					cJSON_AddNumberToObject(psC4, "hum", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.hum.humidity);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					cJSON_AddNumberToObject(psC4, "warnmode", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.warnmode);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					cJSON_AddNumberToObject(psC4, "duration", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.duration);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					//**************************************************
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					cJSON_AddNumberToObject(psC4, "action", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.action);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					cJSON_AddNumberToObject(psC4, "play", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.play);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					cJSON_AddNumberToObject(psC4, "volume", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.volume);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					cJSON_AddNumberToObject(psC4, "urlid", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.urlid);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					cJSON_AddBoolToObject(psC4, "execute", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					cJSON_AddNumberToObject(psC4, "active", rulesflash.RulesConfig.FoutputAction[i].quitstate.ActionValue.active);
					break;
				default :
					break;
			}
		}
	} while (0);
	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);
	return iRet;
}


int json_assemble_answer_del_rules(uint16_t ui16Id, char **ppchJson)
{
	cJSON *psR;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;
	char buf[20]="";

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = rules_del(ui16Id);
		sprintf(buf, MSTR_API_URL_KEY_WORD_RULES"/%d deleted.", ui16Id);
		if (iRet < 0)
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE, buf);
		}else
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS, buf);
		}
		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;

}

int json_assemble_answer_put_debug_state (char **ppchJson,S_ApiParsedebug *psParse)
{
	int iRet = JSON_DEAL_ERR_MEM_FAULT;
	cJSON *psR;

	JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_assemble_answer_put_debug_state]############ mask = [ %x ]  level = [ %d ]################\r\n",psParse->ui32mask,psParse->ui8level);

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	dbg_info_mask_set (psParse->ui32mask<<4);
	cJSON_AddStringToObject(psR, "mask"," mask set succed");
	dbg_info_level_set (psParse->ui8level);
	cJSON_AddStringToObject(psR, "level"," level set succed");

	iRet = s_json_print_unformat(psR, ppchJson);

	cJSON_Delete(psR);

	return iRet;
}

//-------------------------------------------------------------------------------------------------*
/*	用BODY组装JSON(格式)的Report函数
;	(功能)
;	(参数)
;		ui8Method:	方法(PUT/GET/DELETE/POST; ADD/DEL/MODIFY/CALL/ANSWER)
;		ui16Type:	设备类型
;		ui16Id:		设备ID
;		ppchJson:	组装好的JSON字符串回写指针
;		psBody:		BODY的JSON结构体包
;	(返回)
;		int:		组装后的长度,负数表示出错
;	(注意)
*/
int json_assemble_report_with_body (uint8_t ui8Method, uint16_t ui16Type, uint16_t ui16Id, char **ppchJson, cJSON *psBody)
{
	cJSON *psR;
	char achUrl[MUI_API_URL_PREFIX_LEN_MAX+MUI_API_URL_USERNAME_LEN_MAX+MUI_API_URL_KEY_WORD_LEN_MAX+1+5+1];
	int iRet;

	ASSERT(NULL != ppchJson);
	ASSERT(NULL != psBody);

	if (NULL == (psR=cJSON_CreateObject()))
	{
		cJSON_Delete(psBody);
		return -1;
	}

	cJSON_AddStringToObject(psR, "method", api_http_method_string_get(ui8Method));
	iRet = snprintf(achUrl, sizeof(achUrl), "/ha/autoreport%s", (ui16Type<=MUI_DATA_TYPE_CFG_END)?s_aachCfgUrlKeyWordList[ui16Type]:s_aachDevUrlKeyWordList[ui16Type-MUI_DATA_TYPE_DEV_START]);
	if (ui16Type != MUI_DATA_TYPE_CONFIG)
	{
		iRet += snprintf(&achUrl[iRet], sizeof(achUrl)-(size_t)iRet, "/%u", ui16Id);
	}
	cJSON_AddStringToObject(psR, "url", achUrl);
	cJSON_AddItemToObject(psR, "body", psBody);

	iRet = s_json_print_unformat(psR, ppchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_assemble_report_with_body]JSON print error.\r\n");
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*











int json_parse_create_user (const cJSON *psBody, S_ApiParseCreateUser *psParse)
{
	cJSON *psNode;
	int iCnt;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	iCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			return JSON_DEAL_ERR_PARSE_FAIL;
		}

		if (0 == strcmp(psNode->string, "devicetype"))
		{
			strncpy(psParse->dev_name, psNode->valuestring, sizeof(psParse->dev_name));
			iCnt++;
		}
		else if (0 == strcmp(psNode->string, "username"))
		{
			strncpy(psParse->name, psNode->valuestring, sizeof(psParse->name));
			iCnt++;
		}
	}

	return iCnt;
}

int json_parse_get_full_state (const cJSON *psBody, S_ApiParseGetFullState *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}

int json_parse_search_new_dev (const cJSON *psBody, S_ApiParseSearchNewDev *psParse)
{
	cJSON *psNode;
	int iCnt;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	iCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			return JSON_DEAL_ERR_PARSE_FAIL;
		}

		if (0 == strcmp(psNode->string, "duration"))
		{
			psParse->ui16Duration = psNode->valueint;
			iCnt++;
		}
	}

	return iCnt;
}

int json_parse_factory_new (const cJSON *psBody, S_ApiParseFactoryNew *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}

int json_parse_get_config (const cJSON *psBody, S_ApiParseGetConfig *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}

int json_parse_set_config (const cJSON *psBody, S_ApiParseSetConfig *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		iRet = s_keyword_map_search(s_asSetConfigKeywordMapList, sizeof(s_asSetConfigKeywordMapList)/sizeof(s_asSetConfigKeywordMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			continue;
		}
		switch (s_asSetConfigKeywordMapList[iRet].ui16Key)
		{
			case MUI_SET_CONFIG_KEY_WORD_IDX_NAME:
				strncpy(psParse->sSetConfigInfo.name.str, psNode->valuestring, sizeof(psParse->sSetConfigInfo.name.str));
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_IP_ADDR:
				strncpy(psParse->sSetConfigInfo.ipaddr.str, psNode->valuestring, sizeof(psParse->sSetConfigInfo.ipaddr.str));
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_NETMASK:
				strncpy(psParse->sSetConfigInfo.netmask.str, psNode->valuestring, sizeof(psParse->sSetConfigInfo.netmask.str));
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_GW_IP_ADDR:
				strncpy(psParse->sSetConfigInfo.gwipaddr.str, psNode->valuestring, sizeof(psParse->sSetConfigInfo.gwipaddr.str));
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_DHCP:
				psParse->sSetConfigInfo.dhcp = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_PROXY_ADDR:
				strncpy(psParse->sSetConfigInfo.proxyaddr.str, psNode->valuestring, sizeof(psParse->sSetConfigInfo.proxyaddr.str));
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_PROXY_PORT:
				psParse->sSetConfigInfo.proxyport = (uint16_t)psNode->valueint;
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_PORTAL_SERVICES:
				psParse->sSetConfigInfo.portalservices = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_DEV_ID:
				strncpy(psParse->sSetConfigInfo.devid.str, psNode->valuestring, sizeof(psParse->sSetConfigInfo.devid.str));
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_PASSWD:
				strncpy(psParse->sSetConfigInfo.passwd.str, psNode->valuestring, sizeof(psParse->sSetConfigInfo.passwd.str));
				break;
			default:
				break;
		}

		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt >= MUI_SET_CONFIG_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	return (int)psParse->tCnt;
}

int json_parse_get_all_devs_by_type (const cJSON *psBody, S_ApiParseGetAllDevsByType *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}

int json_parse_get_dev_attr_state (const cJSON *psBody, S_ApiParseGetDevAttrState *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}

int json_parse_set_dev_attr (const cJSON *psBody, S_ApiParseSetDevAttr *psParse)
{
	cJSON *psNode;
	uint8_t ui8ChangeId = 0;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		iRet = s_keyword_map_search(s_asSetDevAttrKeywordMapList, sizeof(s_asSetDevAttrKeywordMapList)/sizeof(s_asSetDevAttrKeywordMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			continue;
		}
		switch (s_asSetDevAttrKeywordMapList[iRet].ui16Key)
		{
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_ID:
				psParse->sSetDevAttr.ui16Id = (uint16_t)psNode->valueint;
				ui8ChangeId = 1;
				break;
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_NAME:
				strncpy(psParse->sSetDevAttr.name.str, psNode->valuestring, sizeof(psParse->sSetDevAttr.name.str));
				break;
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_DESCRIPTION:
				strncpy(psParse->sSetDevAttr.descr.str, psNode->valuestring, sizeof(psParse->sSetDevAttr.descr.str));
				break;
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_CHANGE_RPT:
				psParse->sSetDevAttr.changerpt = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_AUTO_RPT:
				psParse->sSetDevAttr.autorpt = (uint16_t)psNode->valueint;
				break;
			default:
				break;
		}

		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt >= MUI_SET_DEV_ATTR_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	if ((1==ui8ChangeId) && (psParse->tCnt>1))
	{
		//ID 不能和其它属性一起修改
		return API_DEAL_ERR_INVALID_MSG;
	}

	return (int)psParse->tCnt;
}

int json_parse_set_dev_state (const cJSON *psBody, S_ApiParseSetDevState *psParse)
{
	cJSON *psNode;
	uint8_t ui8ChangeId = 0;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);
	ASSERT((MUI_DATA_TYPE_DEV_START<=psParse->ui16DevType) && (psParse->ui16DevType<=MUI_DATA_TYPE_DEV_END));

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		//JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_set_dev_state]Keyword:%s\r\n", psNode->string);

		iRet = s_keyword_map_search(s_asDataKeywordMapDescList[psParse->ui16DevType-MUI_DATA_TYPE_START].psKeywordMap, s_asDataKeywordMapDescList[psParse->ui16DevType-MUI_DATA_TYPE_START].ui8MembNum, psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			continue;
		}
		switch (s_asDataKeywordMapDescList[psParse->ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[iRet].ui16Key)
		{
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
				psParse->sSetDevState.on = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
				psParse->sSetDevState.bri = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
				psParse->sSetDevState.sat = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
				psParse->sSetDevState.hue = (uint16_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
				psParse->sSetDevState.ct = (uint16_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
				if ((NULL!=psNode->child) && (NULL!=psNode->child->next))
				{
					psParse->sSetDevState.xy.x = (uint16_t)(psNode->child->valuedouble * 65536.0);
					psParse->sSetDevState.xy.y = (uint16_t)(psNode->child->next->valuedouble * 65536.0);
				}
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COLOR_MODE:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_ALERT:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_EFFECT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
				psParse->sSetDevState.level = (uint16_t)psNode->valueint;
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				psParse->sSetDevState.temp = (uint16_t)(psNode->valuedouble * 100.0);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				psParse->sSetDevState.hum = (uint16_t)(psNode->valuedouble * 100.0);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_CONSUMPTION:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
				psParse->sSetDevState.warnmode = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
				psParse->sSetDevState.duration = (uint16_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				if ((NULL!=psNode->child) && (NULL!=psNode->child->next))
				{
					psParse->sSetDevState.warncfg.warnmode = (uint8_t)psNode->child->valueint;
					psParse->sSetDevState.warncfg.duration = (uint16_t)psNode->child->valueint;
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
				psParse->sSetDevState.action = (uint8_t)psNode->valueint;
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
				psParse->sSetDevState.play = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
				psParse->sSetDevState.volume = (uint16_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
				//JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_api_parse_set_dev_state]@@@@@@@@@@@@@@@@@@@@    [   url =%s  ]  [devType = %hu]  @@@@@@@@@@@@@@@@@@@@@\r\n",psNode->valuestring,psParse->ui16DevType);
				read_sonos_state_url_id(psParse->ui16DevId,&psParse->sSetDevState.urlid);
				//JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_api_parse_set_dev_state]@@@@@@@@@@@@@@@@@@@@    [ delete  urlid =%u  ] @@@@@@@@@@@@@@@@@@@@@\r\n",psParse->sSetDevState.urlid);
				extend_data_ext_id_del(psParse->ui16DevType,psParse->ui16DevId,psParse->sSetDevState.urlid);//每个设备只存储一个URL
				extend_data_ext_id_add(psParse->ui16DevType,psParse->ui16DevId,psNode->valuestring,strlen(psNode->valuestring),&psParse->sSetDevState.urlid);
				//JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_api_parse_set_dev_state]@@@@@@@@@@@@@@@@@@@@    [ new urlid =%u  ] @@@@@@@@@@@@@@@@@@@@@\r\n",psParse->sSetDevState.urlid);
				break;
			default:
				break;
		}

		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt > MUI_SET_DEV_STATE_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	if ((1==ui8ChangeId) && (psParse->tCnt>1))
	{
		//ID 不能和其它属性一起修改
		return API_DEAL_ERR_INVALID_MSG;
	}

	return (int)psParse->tCnt;
}

int json_parse_del_dev (const cJSON *psBody, S_ApiParseDelDev *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}

int json_parse_get_all_groups (const cJSON *psBody, S_ApiParseGetAllGroups *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}

int json_parse_get_group_attr (const cJSON *psBody, S_ApiParseGetGroupAttr *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}



int json_parse_config_group (const cJSON *psBody, S_GroupOpAttr *psParse)
{
	cJSON *psNode, *psConfigNode, *psAddress;
	int iRet = 0;
	char buf[20] = "";
	uint16_t ui16Id;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);
	psParse->total = 0;

	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			for (psConfigNode=psNode->child; NULL!=psConfigNode; psConfigNode=psConfigNode->next)
			{
				if (cJSON_Object == psNode->type)
				{
					for (psAddress=psConfigNode->child; NULL!=psAddress; psAddress=psAddress->next)
					{
						iRet = s_keyword_map_search(s_aachGroupConfigTypeMapList, sizeof(s_aachGroupConfigTypeMapList)/sizeof(s_aachGroupConfigTypeMapList[0]), psAddress->string, strlen(psAddress->string));
						if (iRet < 0)
						{
							JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_group] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
							continue;
						}
						else
						{
							switch (s_aachGroupConfigTypeMapList[iRet].ui16Key)
							{
								case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_ADDRESS:
									PRINT("group address valuestring=%s!\r\n", psAddress->valuestring);
									if (2 != sscanf(psAddress->valuestring, "/%[a-z]/%hu", buf, &ui16Id))
									{
										JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_group] sscanf filed\r\n\n");
										return API_DEAL_ERR_INVALID_MSG;
									}
									iRet = s_keyword_map_search(s_asGetFullStateKeywordMapList, sizeof(s_asGetFullStateKeywordMapList)/sizeof(s_asGetFullStateKeywordMapList[0]), buf, strlen(buf));
									if (iRet < 0)
									{
										JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_group] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
										continue;
									}

									psParse->member[psParse->total].devId = ui16Id;
									psParse->member[psParse->total].devType = s_asGetFullStateKeywordMapList[iRet].ui16Key;
									psParse->total++;
									break;
								default:
									break;
							}
						}
					}
				}
			}
		}
	}

	return iRet;
}

int json_parse_set_group_attr (const cJSON *psBody, S_ApiParseSetGroup *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		/*if (cJSON_Object == psNode->type)
		{
			continue;
		}*/
		iRet = s_keyword_map_search(s_aachGroupTypeMapList, sizeof(s_aachGroupTypeMapList)/sizeof(s_aachGroupTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			continue;
		}
		else
		{
			switch (s_aachGroupTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->sSetGroupAttr.name.str, psNode->valuestring, sizeof(psParse->sSetGroupAttr.name.str));
					break;
				case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->sSetGroupAttr.descr.str, psNode->valuestring, sizeof(psParse->sSetGroupAttr.descr.str));
					break;
				case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_CONFGI:
					json_parse_config_group(psBody, &psParse->sSetGroupAttr.opAttr);
					PRINT("psParse->sSetGroupAttr.opAttr.total = %hu \r\n", psParse->sSetGroupAttr.opAttr.total);
					break;
				default:
					break;
			}
		}

		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt > MUI_SET_GROUP_ATTR_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	return (int)psParse->tCnt;
}

int json_parse_set_group_state (const cJSON *psBody, S_SetGroupState *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachGroupConfigActionTypeMapList, sizeof(s_aachGroupConfigActionTypeMapList)/sizeof(s_aachGroupConfigActionTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			continue;
		}
		switch (s_aachGroupConfigActionTypeMapList[iRet].ui16Key)
		{
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
				psParse->sSetDevState.on = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
				psParse->sSetDevState.bri = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
				psParse->sSetDevState.sat = (uint8_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
				psParse->sSetDevState.hue = (uint16_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
				psParse->sSetDevState.ct = (uint16_t)psNode->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
				if ((NULL!=psNode->child) && (NULL!=psNode->child->next))
				{
					psParse->sSetDevState.xy.x = (uint16_t)(psNode->child->valuedouble * 65536.0);
					psParse->sSetDevState.xy.y = (uint16_t)(psNode->child->next->valuedouble * 65536.0);
				}
				PRINT("+++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
				PRINT("psParse->sSetDevState.xy.x = %hu\r\n", psParse->sSetDevState.xy.x);
				PRINT("psParse->sSetDevState.xy.y = %hu\r\n", psParse->sSetDevState.xy.y);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COLOR_MODE:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_ALERT:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_EFFECT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
				psParse->sSetDevState.level = (uint16_t)psNode->valueint;
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				psParse->sSetDevState.temp = (uint16_t)(psNode->valuedouble * 100.0);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				psParse->sSetDevState.hum = (uint16_t)(psNode->valuedouble * 100.0);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_CONSUMPTION:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
				psParse->sSetDevState.warncfg.warnmode = (uint8_t)psNode->child->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
				psParse->sSetDevState.warncfg.duration = (uint16_t)psNode->child->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				if ((NULL!=psNode->child) && (NULL!=psNode->child->next))
				{
					psParse->sSetDevState.warncfg.warnmode = (uint8_t)psNode->child->valueint;
					psParse->sSetDevState.warncfg.duration = (uint16_t)psNode->child->valueint;
				}
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
				psParse->sSetDevState.action = (uint8_t)psNode->child->valueint;
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
				psParse->sSetDevState.play = (uint8_t)psNode->child->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
				psParse->sSetDevState.volume = (uint16_t)psNode->child->valueint;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
				psParse->sSetDevState.urlid = 0;
				break;
			default:
				break;
		}

		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt >= MUI_SET_DEV_STATE_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	return (int)psParse->tCnt;
}

int json_parse_create_group (const cJSON *psBody, S_SetGroupAttr *psParse)
{
	cJSON *psNode;
	int iRet = 0;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		/*if (cJSON_Object == psNode->type)
		{
			continue;
		}*/
		iRet = s_keyword_map_search(s_aachGroupTypeMapList, sizeof(s_aachGroupTypeMapList)/sizeof(s_aachGroupTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_group] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachGroupTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->name.str, psNode->valuestring, sizeof(psParse->name.str));
					break;
				case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->descr.str, psNode->valuestring, sizeof(psParse->descr.str));
					break;
				case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_CONFGI:
					json_parse_config_group(psBody, &psParse->opAttr);
				default:
					break;
			}
		}
	}

	return iRet;
}

int json_parse_set_set_attr(const cJSON *psBody, S_ApiParseSetSet *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		iRet = s_keyword_map_search(s_aachSetTypeMapList, sizeof(s_aachSetTypeMapList)/sizeof(s_aachSetTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			continue;
		}
		else
		{
			switch (s_aachSetTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_SET_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->setattr.name.str, psNode->valuestring, sizeof(psParse->setattr.name.str));
					break;
				case MUI_SET_SET_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->setattr.descr.str, psNode->valuestring, sizeof(psParse->setattr.descr.str));
					break;
				case MUI_SET_SET_ATTR_KEY_WORD_IDX_DELAY:
					psParse->setattr.delay = psNode->valueint;
					break;
				case MUI_SET_SET_ATTR_KEY_WORD_IDX_EXECUTE:
					psParse->execute = psNode->valueint;
					break;
				default:
					break;
			}
		}

		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt > MUI_SET_SET_ATTR_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	return (int)psParse->tCnt;
}

int json_parse_action_create_set(const cJSON *psJson, S_SetMember *psParse)
{
	cJSON *psNode;
	int iRet;
	uint8_t ui8Tmp=0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachSetActionMapList, sizeof(s_aachSetActionMapList)/sizeof(s_aachSetActionMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		switch (s_aachSetActionMapList[iRet].ui16Key)
		{
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_ON;
				psParse->uSetAtion.on = (uint8_t)psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI;
				psParse->uSetAtion.bri = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT;
				psParse->uSetAtion.sat = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE;
				psParse->uSetAtion.hue = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_CT;
				psParse->uSetAtion.ct = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_XY;
				psParse->uSetAtion.xy.x = (uint16_t)(psNode->child->valuedouble * 65535);
				psParse->uSetAtion.xy.y = (uint16_t)(psNode->child->next->valuedouble * 65535);
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL;
				psParse->uSetAtion.level = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP;
				psParse->uSetAtion.temp = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM;
				psParse->uSetAtion.hum = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE;
				psParse->uSetAtion.warnmode = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION;
				psParse->uSetAtion.duration = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG;
				psParse->uSetAtion.warncfg.warnmode = psNode->valueint;
				psParse->uSetAtion.warncfg.duration = psNode->child->next->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION;
				psParse->uSetAtion.action = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY;
				psParse->uSetAtion.play = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME;
				psParse->uSetAtion.volume = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID;
				//要先把string放到拓展数据库中，再把ID复制
				psParse->uSetAtion.urlid = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE;
				psParse->uSetAtion.execute = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
				psParse->action_type = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE;
				psParse->uSetAtion.active = psNode->valueint;
				ui8Tmp++;
				break;
			default:
				break;
		}
	}
	return ui8Tmp;
}

int json_parse_config_create_set(const cJSON *psJson, S_SetAttr *psParse)
{
	cJSON *psNode, *psConfigNode;
	uint8_t ui8Tmp=0;
	int iRet;
	S_SetMember sSetMember;
	char buf[20] = "";
	uint16_t ui16Id;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			for (psConfigNode=psNode->child; NULL!=psConfigNode; psConfigNode=psConfigNode->next)
			{
				iRet = s_keyword_map_search(s_aachSetTypeMapList, sizeof(s_aachSetTypeMapList)/sizeof(s_aachSetTypeMapList[0]), psConfigNode->string, strlen(psConfigNode->string));
				if (iRet < 0)
				{
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
					continue;
				}
				else
				{
					switch (s_aachSetTypeMapList[iRet].ui16Key)
					{
						case MUI_SET_SET_ATTR_KEY_WORD_IDX_ADDRESS:
							if (2 != sscanf(psConfigNode->valuestring, "/%[a-z]/%hu", buf, &ui16Id))
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_create_set] sscanf filed\r\n\n");
								return API_DEAL_ERR_INVALID_MSG;
							}
							iRet = s_keyword_map_search(s_asGetFullStateKeywordMapList, sizeof(s_asGetFullStateKeywordMapList)/sizeof(s_asGetFullStateKeywordMapList[0]), buf, strlen(buf));
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
								continue;
							}
							sSetMember.dev_id = ui16Id;
							sSetMember.dev_type = s_asGetFullStateKeywordMapList[iRet].ui16Key;
							break;
						case MUI_SET_SET_ATTR_KEY_WORD_IDX_ACTION:
							json_parse_action_create_set(psConfigNode, &sSetMember);
							ui8Tmp++;
							break;
						default:
							break;
					}
				}

			}
			memcpy(&psParse->setmember[sizeof(S_SetMember)*(ui8Tmp-1)], &sSetMember, sizeof(S_SetMember));
		}
	}

	return ui8Tmp;

}

int json_parse_create_set(const cJSON *psBody, S_SetAttr *psParse)
{
	cJSON *psNode;
	int iRet,i;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	i = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachSetTypeMapList, sizeof(s_aachSetTypeMapList)/sizeof(s_aachSetTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachSetTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->setattr.name.str, psNode->valuestring, sizeof(psParse->setattr.name));
					break;
				case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->setattr.descr.str, psNode->valuestring, sizeof(psParse->setattr.descr));
					break;
				case MUI_SET_SET_ATTR_KEY_WORD_IDX_DELAY:
					psParse->setattr.delay = psNode->valueint;
					break;
				case MUI_SET_SET_ATTR_KEY_WORD_IDX_CONFIG:
					iRet = json_parse_config_create_set(psNode, psParse);
					psParse->setattr.action_num = iRet;
					break;
				default:
					break;
			}
			i++;
		}
	}

	return i;

}

int json_parse_set_scene_attr(const cJSON *psBody, S_ApiParseSetScene *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		iRet = s_keyword_map_search(s_aachSceneAttrTypeMapList, sizeof(s_aachSceneAttrTypeMapList)/sizeof(s_aachSceneAttrTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			continue;
		}
		else
		{
			switch (s_aachSceneAttrTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->scene_data.scene_attr.name.str, psNode->valuestring, sizeof(psParse->scene_data.scene_attr.name.str));
					break;
				case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->scene_data.scene_attr.descr.str, psNode->valuestring, sizeof(psParse->scene_data.scene_attr.descr.str));
					break;
				case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_EXECUTE:
					PRINT("******************************************************************\r\n");
					PRINT("execute = %d\r\n", psNode->valueint);
					psParse->execute = psNode->valueint;
					break;
				default:
					break;
			}
		}

		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt > MUI_SET_SCENE_ATTR_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	return (int)psParse->tCnt;
}


int json_parse_action_create_scene(const cJSON *psJson, S_SceneDataFalsh *psParse)
{
	cJSON *psNode;
	int iRet;
	uint8_t ui8Tmp=0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		PRINT("the string is %s\r\n", psNode->string);
		iRet = s_keyword_map_search(s_aachSceneActionMapList, sizeof(s_aachSceneActionMapList)/sizeof(s_aachSceneActionMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_sceme] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		switch (s_aachSceneActionMapList[iRet].ui16Key)
		{
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ON:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ON;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.on = (uint8_t)psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_BRI:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_BRI;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.bri = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_SAT:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_SAT;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.sat = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUE:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUE;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.hue = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_CT:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_CT;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.ct = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_XY:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_XY;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.xy.x = (uint16_t)(psNode->child->valuedouble * 65535);
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.xy.y = (uint16_t)(psNode->child->next->valuedouble * 65535);
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_LEVEL:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_LEVEL;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.level = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_TEMP:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_TEMP;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.temp = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUM:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUM;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.hum = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_MODE;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.warnmode = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_DURATION:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_DURATION;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.duration = psNode->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_CFG;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.warncfg.warnmode = psNode->valueint;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.warncfg.duration = psNode->child->next->valueint;
				ui8Tmp++;
				break;
			case MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ACTION:
				psParse->scene_member[psParse->scene_attr.action_num].action_type = MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ACTION;
				psParse->scene_member[psParse->scene_attr.action_num].uSceneAtion.action = psNode->valueint;
				ui8Tmp++;
				break;
			default:
				break;
		}
	}
	return ui8Tmp;
}


int json_parse_config_create_scene(const cJSON *psJson, S_SceneDataFalsh *psParse)
{
	cJSON *psNode, *psConfigNode;
	uint8_t ui8Tmp=0;
	int iRet;
	char buf[20] = "";
	uint16_t ui16Id;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);
	psParse->scene_attr.action_num = 0;

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			for (psConfigNode=psNode->child; NULL!=psConfigNode; psConfigNode=psConfigNode->next)
			{
				iRet = s_keyword_map_search(s_aachSceneConfigTypeMapList, sizeof(s_aachSceneConfigTypeMapList)/sizeof(s_aachSceneConfigTypeMapList[0]), psConfigNode->string, strlen(psConfigNode->string));
				if (iRet < 0)
				{
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
					continue;
				}
				else
				{
					switch (s_aachSceneConfigTypeMapList[iRet].ui16Key)
					{
						case MUI_SET_SCENE_CONFIG_KEY_WORD_IDX_ADDRESS:
							if (2 != sscanf(psConfigNode->valuestring, "/%[a-z]/%hu", buf, &ui16Id))
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_create_set] sscanf filed\r\n\n");
								return API_DEAL_ERR_INVALID_MSG;
							}
							iRet = s_keyword_map_search(s_asGetFullStateKeywordMapList, sizeof(s_asGetFullStateKeywordMapList)/sizeof(s_asGetFullStateKeywordMapList[0]), buf, strlen(buf));
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
								continue;
							}
							psParse->scene_member[psParse->scene_attr.action_num].dev_id = ui16Id;
							psParse->scene_member[psParse->scene_attr.action_num].dev_type = s_asGetFullStateKeywordMapList[iRet].ui16Key;
							break;
						case MUI_SET_SCENE_CONFIG_KEY_WORD_IDX_ACTION:
							json_parse_action_create_scene(psConfigNode, psParse);
							ui8Tmp++;
							break;
						default:
							break;
					}
				}

			}
			psParse->scene_attr.action_num++;
		}
	}

	return ui8Tmp;

}


int json_parse_create_scene(const cJSON *psBody, S_SceneDataFalsh *psParse)
{
	cJSON *psNode;
	int iRet,i;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	i = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachSceneAttrTypeMapList, sizeof(s_aachSceneAttrTypeMapList)/sizeof(s_aachSceneAttrTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_scene] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachSceneAttrTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->scene_attr.name.str, psNode->valuestring, sizeof(psParse->scene_attr.name));
					break;
				case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->scene_attr.descr.str, psNode->valuestring, sizeof(psParse->scene_attr.descr));
					break;
				case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_CONFIG:
					iRet = json_parse_config_create_scene(psNode, psParse);
					break;
				default:
					break;
			}
			i++;
		}
	}

	return i;

}


int json_parse_quitstate_create_schedule(const cJSON *psJson, S_ScheduleOutPutAction *psParse)
{
	cJSON *psNode;
	int iRet;
	int i=0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachRulesActionMapList, sizeof(s_aachRulesActionMapList)/sizeof(s_aachRulesActionMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_quitstate_create_schedule] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachRulesActionMapList[iRet].ui16Key)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ON;
					psParse->quitValue.on = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI;
					psParse->quitValue.bri = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT;
					psParse->quitValue.sat = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE;
					psParse->quitValue.hue = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_CT;
					psParse->quitValue.ct = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_XY;
					psParse->quitValue.xy.x = (uint16_t)(psNode->child->valuedouble * 65535);
					psParse->quitValue.xy.y = (uint16_t)(psNode->child->next->valuedouble * 65535);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL;
					psParse->quitValue.level = psNode->valueint;
					break;
				//case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				//	psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP;
				//	psParse->quitValue.temp.temp = psNode->valueint;
				//	break;
				//case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				//	psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM;
				//	psParse->quitValue.hum.humidity = psNode->valueint;
				//	break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE;
					psParse->quitValue.warnmode = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION;
					psParse->quitValue.duration = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG;
					psParse->quitValue.warncfg.warnmode = psNode->valueint;
					psParse->quitValue.warncfg.duration = psNode->child->next->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION;
					psParse->quitValue.action = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY;
					psParse->quitValue.play = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME;
					psParse->quitValue.volume = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID;
					//**************************************************************************
					psParse->quitValue.urlid = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE;
					psParse->quitValue.execute = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					psParse->quitType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE;
					psParse->quitValue.active = psNode->valueint;
					break;
				default:
					iRet = -1;
					break;
			}
			i++;
		}
	}
	return i;
}

int json_parse_action_create_schedule(const cJSON *psJson, S_ScheduleOutPutAction *psParse)
{
	cJSON *psNode;
	int iRet = 0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachRulesActionMapList, sizeof(s_aachRulesActionMapList)/sizeof(s_aachRulesActionMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_action_create_schedule] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachRulesActionMapList[iRet].ui16Key)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ON;
					psParse->ActionValue.on = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI;
					psParse->ActionValue.bri = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT;
					psParse->ActionValue.sat = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE;
					psParse->ActionValue.hue = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_CT;
					psParse->ActionValue.ct = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_XY;
					psParse->ActionValue.xy.x = (uint16_t)(psNode->child->valuedouble * 65535);
					psParse->ActionValue.xy.y = (uint16_t)(psNode->child->next->valuedouble * 65535);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL;
					psParse->ActionValue.level = psNode->valueint;
					break;
				//case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				//	psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP;
				//	psParse->ActionValue.temp.temp = psNode->valueint;
				//	break;
				//case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				//	psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM;
				//	psParse->ActionValue.hum.humidity = psNode->valueint;
				//	break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE;
					psParse->ActionValue.warnmode = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION;
					psParse->ActionValue.duration = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG;
					psParse->ActionValue.warncfg.warnmode = psNode->valueint;
					psParse->ActionValue.warncfg.duration = psNode->child->next->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION;
					psParse->ActionValue.action = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY;
					psParse->ActionValue.play = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME;
					psParse->ActionValue.volume = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID;
					//**************************************************************************
					psParse->ActionValue.urlid = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE;
					psParse->ActionValue.execute = psNode->valueint;
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "************execute=%d************\r\n", psParse->ActionValue.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE;
					psParse->ActionValue.active = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC;
					psParse->ActionValue.blink.onsec = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC:
					psParse->ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC;
					psParse->ActionValue.blink.offsec = psNode->valueint;
					break;
				default:
					iRet = -1;
					break;
			}

		}
	}
	return iRet;
}

int json_parse_config_create_schedule(const cJSON *psJson, S_ScheduleConfig *psParse)
{
	cJSON *psNode, *psOutPutNode;
	int iRet = 0;
	char buf[20] = "";
	uint16_t ui16Id;
	int actionNum = 0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		psParse->OutputAction[actionNum].DevType = 0xffff;
		psParse->OutputAction[actionNum].ActionType = 255;
		psParse->OutputNum = 0;
		psParse->OutputAction[actionNum].quitType = 255;
		if (cJSON_Object == psNode->type)
		{
			for (psOutPutNode=psNode->child; NULL!=psOutPutNode; psOutPutNode=psOutPutNode->next)
			{
				iRet = s_keyword_map_search(s_aachScheduleOutputStateMapList, sizeof(s_aachScheduleOutputStateMapList)/sizeof(s_aachScheduleOutputStateMapList[0]), psOutPutNode->string, strlen(psOutPutNode->string));
				if (iRet < 0)
				{
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_create_schedule] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
					continue;
				}
				else
				{
					switch (s_aachScheduleOutputStateMapList[iRet].ui16Key)
					{
						case MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_ADDRESS:
							if (2 != sscanf(psOutPutNode->valuestring, "/%[a-z]/%hu", buf, &ui16Id))
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_config_create_schedule] sscanf filed\r\n\n");
								return API_DEAL_ERR_INVALID_MSG;
							}
							iRet = s_keyword_map_search(s_asGetFullStateKeywordMapList, sizeof(s_asGetFullStateKeywordMapList)/sizeof(s_asGetFullStateKeywordMapList[0]), buf, strlen(buf));
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_create_schedule] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
								continue;
							}
							psParse->OutputAction[actionNum].DevId = ui16Id;
							psParse->OutputAction[actionNum].DevType = s_asGetFullStateKeywordMapList[iRet].ui16Key;
							psParse->OutputNum++;
							break;
						case MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_DELAY:
							psParse->OutputAction[actionNum].delay = psOutPutNode->valueint;
							break;
						case MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_DURATION:
							psParse->OutputAction[actionNum].duration = psOutPutNode->valueint;
							break;
						case MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_ACTION:
							json_parse_action_create_schedule(psOutPutNode, &psParse->OutputAction[actionNum]);
							break;
						case MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_QUITMETHOD:
							psParse->OutputAction[actionNum].quitmethod = psOutPutNode->valueint;
							break;
						case MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_QUITSTATE:
							iRet = json_parse_quitstate_create_schedule(psOutPutNode, &psParse->OutputAction[actionNum]);
							break;
						default:
							break;
					}

				}
			}
			actionNum++;
		}
	}
	return iRet;
}

int json_parse_set_schedule_attr(const cJSON *psBody, S_ApiParseSetschedule *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachScheduleTypeMapList, sizeof(s_aachScheduleTypeMapList)/sizeof(s_aachScheduleTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_rules] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachScheduleTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->ScheduleAttr.name.str, psNode->valuestring, sizeof(psParse->ScheduleAttr.name.str));
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "schedule change name =%s---------------------------------.\r\n",psParse->ScheduleAttr.name.str);
					break;
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->ScheduleAttr.descr.str, psNode->valuestring, sizeof(psParse->ScheduleAttr.descr.str));
					break;
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_TIME:
					rules_get_time(psNode->valuestring, &psParse->ScheduleAttr.time);
					break;
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_REPEAT:
					psParse->ScheduleAttr.repeat = psNode->valueint;
					break;
				default:
					break;
			}
		}
		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt > MUI_SET_SCHEDULE_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	return (int)psParse->tCnt;
}


int json_parse_create_schedule(const cJSON *psBody, S_ApiParseSchedule *psParse)
{
	cJSON *psNode;
	int iRet = 0;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachScheduleTypeMapList, sizeof(s_aachScheduleTypeMapList)/sizeof(s_aachScheduleTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_schedule] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachScheduleTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->ScheduleAttr.name.str, psNode->valuestring, sizeof(psParse->ScheduleAttr.name));
					break;
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->ScheduleAttr.descr.str, psNode->valuestring, sizeof(psParse->ScheduleAttr.descr));
					break;
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_TIME:
					rules_get_time(psNode->valuestring, &psParse->ScheduleAttr.time);
					break;
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_REPEAT:
					psParse->ScheduleAttr.repeat = psNode->valueint;
					break;
				case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_CONFIG:
					json_parse_config_create_schedule(psNode, &psParse->ScheduleConfig);
					break;
				default:
					break;
			}
		}
	}

	return iRet;
}

int json_parse_state_create_rules(const cJSON *psJson, S_RulesInPutState *psParse)
{
	cJSON *psNode;
	int iRet = 0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachRulesInputStateMapList, sizeof(s_aachRulesInputStateMapList)/sizeof(s_aachRulesInputStateMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		switch (s_aachRulesInputStateMapList[iRet].ui16Key)
		{
			case MUI_RULES_ACTION_STATE_WORD_IDX_ON:
				psParse->ActionType = MUI_RULES_ACTION_STATE_WORD_IDX_ON;
				psParse->ActionValue.on = psNode->valueint;
				break;
			case MUI_RULES_ACTION_STATE_WORD_IDX_TEMP:
				psParse->ActionType = MUI_RULES_ACTION_STATE_WORD_IDX_TEMP;
				psParse->ActionValue.temp.temp = (uint16_t)psNode->valuedouble * 100;
				break;
			case MUI_RULES_ACTION_STATE_WORD_IDX_HUM:
				psParse->ActionType = MUI_RULES_ACTION_STATE_WORD_IDX_HUM;
				psParse->ActionValue.hum.humidity = (uint16_t)psNode->valuedouble * 100;
				break;
			case MUI_RULES_ACTION_STATE_WORD_IDX_LESS:
				if (psParse->ActionType == MUI_RULES_ACTION_STATE_WORD_IDX_TEMP)
				{
					psParse->ActionValue.temp.less = psNode->valueint;
				}
				else if (psParse->ActionType == MUI_RULES_ACTION_STATE_WORD_IDX_HUM)
				{
					psParse->ActionValue.hum.less = psNode->valueint;
				}
				break;
			case MUI_RULES_ACTION_STATE_WORD_IDX_EDGE:
				psParse->ActionType = MUI_RULES_ACTION_STATE_WORD_IDX_EDGE;
				if (0==strcmp("up", psNode->valuestring))
				{
					psParse->ActionValue.edge = 1;
				}
				if (0==strcmp("down", psNode->valuestring))
				{
					psParse->ActionValue.edge = 2;
				}
				if (0==strcmp("all", psNode->valuestring))
				{
					psParse->ActionValue.edge = 3;
				}
				break;
			default:
				break;
		}
	}
	return iRet;
}

int json_parse_input_create_rules(const cJSON *psJson, S_RulesConfig *psParse)
{
	cJSON *psNode, *psInputNode;
	int iRet;
	char buf[20] = "";
	uint16_t ui16Id;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	psParse->InputNum = 0;
	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			for (psInputNode=psNode->child; NULL!=psInputNode; psInputNode=psInputNode->next)
			{
				iRet = s_keyword_map_search(s_aachRulesTypeMapList, sizeof(s_aachRulesTypeMapList)/sizeof(s_aachRulesTypeMapList[0]), psInputNode->string, strlen(psInputNode->string));
				if (iRet < 0)
				{
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_input_create_rules] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
					continue;
				}
				else
				{
					switch (s_aachRulesTypeMapList[iRet].ui16Key)
					{
						case MUI_SET_RULES_ATTR_KEY_WORD_IDX_ADDRESS:
							if (2 != sscanf(psInputNode->valuestring, "/%[a-z]/%hu", buf, &ui16Id))
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_create_set] sscanf filed\r\n\n");
								return API_DEAL_ERR_INVALID_MSG;
							}
							iRet = s_keyword_map_search(s_asGetFullStateKeywordMapList, sizeof(s_asGetFullStateKeywordMapList)/sizeof(s_asGetFullStateKeywordMapList[0]), buf, strlen(buf));
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
								continue;
							}
							psParse->InputAction[psParse->InputNum].DevId = ui16Id;
							psParse->InputAction[psParse->InputNum].DevType = s_asGetFullStateKeywordMapList[iRet].ui16Key;
							break;
						case MUI_SET_RULES_ATTR_KEY_WORD_IDX_STATE:
							json_parse_state_create_rules(psInputNode, &psParse->InputAction[psParse->InputNum]);
							break;
						default:
							break;
					}

				}
			}
			psParse->InputNum ++;
		}
		else
		{
			iRet = s_keyword_map_search(s_aachRulesTypeMapList, sizeof(s_aachRulesTypeMapList)/sizeof(s_aachRulesTypeMapList[0]), psNode->string, strlen(psNode->string));
			if (iRet < 0)
			{
				JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_input_create_rules] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
				continue;
			}
			else
			{
				switch (s_aachRulesTypeMapList[iRet].ui16Key)
				{
					case MUI_SET_RULES_ATTR_KEY_WORD_IDX_EXPRESSION:
						memcpy(psParse->expersion, psNode->valuestring, sizeof(psParse->expersion));
						break;
					default :
						break;
				}
			}
		}
	}

	return psParse->InputNum;

}

int json_parse_action_create_rules(const cJSON *psJson, S_RuelsOutPutState *psParse)
{
	cJSON *psNode;
	int iRet = 0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachRulesActionMapList, sizeof(s_aachRulesActionMapList)/sizeof(s_aachRulesActionMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachRulesActionMapList[iRet].ui16Key)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ON;
					psParse->action.ActionValue.on = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI;
					psParse->action.ActionValue.bri = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT;
					psParse->action.ActionValue.sat = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE;
					psParse->action.ActionValue.hue = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_CT;
					psParse->action.ActionValue.ct = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_XY;
					psParse->action.ActionValue.xy.x = (uint16_t)(psNode->child->valuedouble * 65535);
					psParse->action.ActionValue.xy.y = (uint16_t)(psNode->child->next->valuedouble * 65535);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL;
					psParse->action.ActionValue.level = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP;
					psParse->action.ActionValue.temp.temp = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM;
					psParse->action.ActionValue.hum.humidity = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE;
					psParse->action.ActionValue.warnmode = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION;
					psParse->action.ActionValue.duration = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG;
					psParse->action.ActionValue.warncfg.warnmode = psNode->valueint;
					psParse->action.ActionValue.warncfg.duration = psNode->child->next->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION;
					psParse->action.ActionValue.action = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY;
					psParse->action.ActionValue.play = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME;
					psParse->action.ActionValue.volume = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID;
					//**************************************************************************
					psParse->action.ActionValue.urlid = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE;
					psParse->action.ActionValue.execute = psNode->valueint;
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "************execute=%d************\r\n", psParse->action.ActionValue.execute);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE;
					psParse->action.ActionValue.active = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC;
					psParse->action.ActionValue.blink.onsec = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC:
					psParse->action.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC;
					psParse->action.ActionValue.blink.offsec = psNode->valueint;
					break;
				default:
					iRet = -1;
					break;
			}

		}
	}
	return iRet;
}

int json_parse_quitstate_create_rules(const cJSON *psJson, S_RuelsOutPutState *psParse)
{
	cJSON *psNode;
	int iRet;
	int i=0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachRulesActionMapList, sizeof(s_aachRulesActionMapList)/sizeof(s_aachRulesActionMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachRulesActionMapList[iRet].ui16Key)
			{
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ON;
					psParse->quitstate.ActionValue.on = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI;
					psParse->quitstate.ActionValue.bri = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT;
					psParse->quitstate.ActionValue.sat = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE;
					psParse->quitstate.ActionValue.hue = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_CT;
					psParse->quitstate.ActionValue.ct = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_XY;
					psParse->quitstate.ActionValue.xy.x = (uint16_t)(psNode->child->valuedouble * 65535);
					psParse->quitstate.ActionValue.xy.y = (uint16_t)(psNode->child->next->valuedouble * 65535);
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL;
					psParse->quitstate.ActionValue.level = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP;
					psParse->quitstate.ActionValue.temp.temp = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM;
					psParse->quitstate.ActionValue.hum.humidity = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE;
					psParse->quitstate.ActionValue.warnmode = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION;
					psParse->quitstate.ActionValue.duration = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG;
					psParse->quitstate.ActionValue.warncfg.warnmode = psNode->valueint;
					psParse->quitstate.ActionValue.warncfg.duration = psNode->child->next->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION;
					psParse->quitstate.ActionValue.action = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY;
					psParse->quitstate.ActionValue.play = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME;
					psParse->quitstate.ActionValue.volume = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID;
					//**************************************************************************
					psParse->quitstate.ActionValue.urlid = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE;
					psParse->quitstate.ActionValue.execute = psNode->valueint;
					break;
				case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
					psParse->quitstate.ActionType = MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE;
					psParse->quitstate.ActionValue.active = psNode->valueint;
					break;
				default:
					iRet = -1;
					break;
			}
			i++;
		}
	}
	return i;
}


int json_parse_output_create_rules(const cJSON *psJson, uint8_t OutPutType, S_RulesConfig *psParse)
{
	cJSON *psNode, *psOutPutNode;
	int iRet = 0;
	char buf[20] = "";
	uint16_t ui16Id;
	S_RuelsOutPutState	RulesAction;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	memset(&RulesAction, 0, sizeof(S_RuelsOutPutState));
	RulesAction.DevType = 0xffff;
	RulesAction.action.ActionType = 255;
	RulesAction.quitstate.ActionType = 255;

	if (OutPutType == MUI_RULES_OUTPUT_TYPE_TRUE)
	{
		psParse->ToutputNum = 0;
	}
	else if (OutPutType == MUI_RULES_OUTPUT_TYPE_FALSE)
	{
		psParse->FoutputNum = 0;
	}
	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			for (psOutPutNode=psNode->child; NULL!=psOutPutNode; psOutPutNode=psOutPutNode->next)
			{
				iRet = s_keyword_map_search(s_aachRulesOutputStateMapList, sizeof(s_aachRulesOutputStateMapList)/sizeof(s_aachRulesOutputStateMapList[0]), psOutPutNode->string, strlen(psOutPutNode->string));
				if (iRet < 0)
				{
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
					continue;
				}
				else
				{
					switch (s_aachRulesOutputStateMapList[iRet].ui16Key)
					{
						case MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_ADDRESS:
							if (2 != sscanf(psOutPutNode->valuestring, "/%[a-z]/%hu", buf, &ui16Id))
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_config_create_set] sscanf filed\r\n\n");
								return API_DEAL_ERR_INVALID_MSG;
							}
							iRet = s_keyword_map_search(s_asGetFullStateKeywordMapList, sizeof(s_asGetFullStateKeywordMapList)/sizeof(s_asGetFullStateKeywordMapList[0]), buf, strlen(buf));
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
								continue;
							}
							RulesAction.DevId = ui16Id;
							RulesAction.DevType = s_asGetFullStateKeywordMapList[iRet].ui16Key;
							break;
						case MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DELAY:
							RulesAction.delay = psOutPutNode->valueint;
							break;
						case MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DELAYOVERLAY:
							RulesAction.delayoverlay = psOutPutNode->valueint;
							break;
						case MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DURATION:
							RulesAction.duration = psOutPutNode->valueint;
							break;
						case MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DURATIONOVERLAY:
							RulesAction.durationoverlay = psOutPutNode->valueint;
							break;
						case MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_ACTION:
							json_parse_action_create_rules(psOutPutNode, &RulesAction);
							break;
						case MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_QUITMETHOD:
							RulesAction.quitmethod = psOutPutNode->valueint;
							break;
						case MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_QUITSTATE:
							iRet = json_parse_quitstate_create_rules(psOutPutNode, &RulesAction);
							break;
						default:
							break;
					}

				}
			}
			if (OutPutType == MUI_RULES_OUTPUT_TYPE_TRUE)
			{
				memcpy(&psParse->ToutputAction[psParse->ToutputNum], &RulesAction, sizeof(S_RuelsOutPutState));
				psParse->ToutputNum ++;
			}
			else if (OutPutType == MUI_RULES_OUTPUT_TYPE_FALSE)
			{
				memcpy(&psParse->FoutputAction[psParse->FoutputNum], &RulesAction, sizeof(S_RuelsOutPutState));
				psParse->FoutputNum ++;
			}
		}
	}
	return iRet;
}

int json_parse_config_create_rules(const cJSON *psJson, S_RulesConfig *psParse)
{
	cJSON *psNode;
	int iRet = 0;

	ASSERT(NULL != psJson);
	ASSERT(NULL != psParse);

	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachRulesTypeMapList, sizeof(s_aachRulesTypeMapList)/sizeof(s_aachRulesTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_set] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachRulesTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_INPUT:
					json_parse_input_create_rules(psNode, psParse);
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_TRUE_OUTPUT:
					json_parse_output_create_rules(psNode, MUI_RULES_OUTPUT_TYPE_TRUE, psParse);
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_FALSE_OUTPUT:
					json_parse_output_create_rules(psNode, MUI_RULES_OUTPUT_TYPE_FALSE, psParse);
					break;
				default:
					break;
			}
		}
	}

	return iRet;

}




int json_parse_create_rules(const cJSON *psBody, S_ApiParseRules *psParse)
{
	cJSON *psNode;
	int iRet,i;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	i = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachRulesTypeMapList, sizeof(s_aachRulesTypeMapList)/sizeof(s_aachRulesTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_rules] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachRulesTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->RulesAttr.name.str, psNode->valuestring, sizeof(psParse->RulesAttr.name));
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->RulesAttr.descr.str, psNode->valuestring, sizeof(psParse->RulesAttr.descr));
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_ACTIVE:
					psParse->RulesAttr.active = psNode->valueint;
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_TIME:
					rules_get_time(psNode->valuestring, &psParse->RulesAttr.time);
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_REPEAT:
					psParse->RulesAttr.repeat = psNode->valueint;
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_DURATION:
					psParse->RulesAttr.duration = psNode->valueint;
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_CONFIG:
					json_parse_config_create_rules(psNode, &psParse->RulesConfig);
					break;
				default:
					break;
			}
			i++;
		}
	}

	return i;
}

int json_parse_set_rules_attr(const cJSON *psBody, S_ApiParseSetRules *psParse)
{
	cJSON *psNode;
	int iRet,i;
	i = 0;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		iRet = s_keyword_map_search(s_aachRulesTypeMapList, sizeof(s_aachRulesTypeMapList)/sizeof(s_aachRulesTypeMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_parse_create_rules] Call :s_keyword_map_search error: keyword = %d\r\n", iRet);
			continue;
		}
		else
		{
			switch (s_aachRulesTypeMapList[iRet].ui16Key)
			{
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_NAME:
					strncpy(psParse->RulesAttr.name.str, psNode->valuestring, sizeof(psParse->RulesAttr.name));
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_DESCRIPTION:
					strncpy(psParse->RulesAttr.descr.str, psNode->valuestring, sizeof(psParse->RulesAttr.descr));
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_ACTIVE:
					psParse->RulesAttr.active = psNode->valueint;
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_TIME:
					rules_get_time(psNode->valuestring, &psParse->RulesAttr.time);
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_REPEAT:
					psParse->RulesAttr.repeat = psNode->valueint;
					break;
				case MUI_SET_RULES_ATTR_KEY_WORD_IDX_DURATION:
					psParse->RulesAttr.duration = psNode->valueint;
					break;
				default:
					break;
			}
			i++;
		}
		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt > MUI_SET_RULES_ONCE_MODIFY_NUM_MAX)
		{
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	return (int)psParse->tCnt;
}













int json_parse_put_rtc_sys_time (const cJSON *psBody, S_ApiParseTime *psParse)
{
	cJSON *psNode;
	int iCnt;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	iCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			return JSON_DEAL_ERR_PARSE_FAIL;
		}
		if (0 == strcmp(psNode->string, "time"))
        {
			strcpy(psParse->achTimeBuf, psNode->valuestring);
			iCnt++;
			if(iCnt > 1)//最大参数 1
			{
				return API_DEAL_ERR_INVALID_MSG;
			}
        }
		else
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
	}

	return iCnt;
}

int json_parse_get_sys (const cJSON *psBody, S_SysInfo *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}
int json_parse_set_sys (const cJSON *psBody, S_ApiParseSetSys *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	psParse->tCnt = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		iRet = s_keyword_map_search(s_asSetSysKeywordMapList, sizeof(s_asSetSysKeywordMapList)/sizeof(s_asSetSysKeywordMapList[0]), psNode->string, strlen(psNode->string));
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[[json_parse_set_sys][s_keyword_map_search]]JSON  error.\r\n");
			continue;
		}
		switch (s_asSetSysKeywordMapList[iRet].ui16Key)
		{
			case MUI_SET_SYS_KEY_WORD_IDX_SN:
				strncpy(psParse->sSysInfo.sSn.str, psNode->valuestring, sizeof(psParse->sSysInfo.sSn.str));
				break;
			case MUI_SET_SYS_KEY_WORD_IDX_MAC:
				strncpy(psParse->sSysInfo.sMac.str, psNode->valuestring, sizeof(psParse->sSysInfo.sMac.str));
				break;
			case MUI_SET_SYS_KEY_WORD_IDX_WIFI_MAC:
				strncpy(psParse->sSysInfo.sWifiMac.str, psNode->valuestring, sizeof(psParse->sSysInfo.sWifiMac.str));
				break;
			case MUI_SET_SYS_KEY_WORD_IDX_UZIPSN:
				strncpy(psParse->sSysInfo.sUzipSn.str, psNode->valuestring, sizeof(psParse->sSysInfo.sUzipSn.str));
				break;
			case MUI_SET_SYS_KEY_WORD_IDX_UZIPMAC:
				strncpy(psParse->sSysInfo.sUzipMac.str, psNode->valuestring, sizeof(psParse->sSysInfo.sUzipMac.str));
				break;
			default:
				break;//配置选项不匹配
		}

		psParse->atKeywordMapIdxList[psParse->tCnt] = iRet;
		psParse->tCnt++;
		if (psParse->tCnt > MUI_SET_SYS_ONCE_MODIFY_NUM_MAX)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_set_sys] Modified too much [ %d ].\r\n",psParse->tCnt);
			return API_DEAL_ERR_INVALID_MSG;	//超出可配置个数上限
		}
	}

	return (int)psParse->tCnt;
}

int json_parse_get_debug (const cJSON *psBody, S_ApiParsedebug *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}
int json_parse_set_debug (const cJSON *psBody, S_ApiParsedebug *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	iRet = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		if (0 == strcmp(psNode->string, "mask"))
        {
			if(psNode->valueint > (int)MUI_API_DEBUG_MASK_MAX)
			{
				psNode->valueint = (int)MUI_API_DEBUG_MASK_MAX;
			}
			psParse->ui32mask = (uint32_t)psNode->valueint;
			iRet++;
        }
		else if (0 == strcmp(psNode->string, "level"))
        {
          	if(psNode->valueint > MUI_API_DEBUG_LEVEL_MAX)
          	{
				psNode->valueint = MUI_API_DEBUG_LEVEL_MAX;
			}
			psParse->ui8level = (uint8_t)psNode->valueint;
			iRet++;
        }
		else
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
	}
	if((iRet < 1)||(iRet > 2))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_WARNING, "[json_parse_set_debug] Modified too much[ %d ].\r\n",iRet);
		return API_DEAL_ERR_INVALID_MSG;
	}

	return iRet;
}


int json_parse_put_at_debug (const cJSON *psBody, S_ApiParseAtdebug *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	iRet = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		if (0 == strcmp(psNode->string, "cmd"))
		{
			strncpy(psParse->achCmd, psNode->valuestring, sizeof(psParse->achCmd));
			memset(psParse->achAnswer, 0, sizeof(psParse->achAnswer));
			iRet++;
		}
		else
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
	}
	if(iRet != 1)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_put_at_debug]Json packet size error [ iRet = %d ]\r\n", iRet);
		return API_DEAL_ERR_INVALID_MSG;
	}
	return iRet;
}


int json_parse_get_upgrade_state (const cJSON *psBody, S_ApiParseUpgrade *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}
int json_parse_post_upgrade (const cJSON *psBody, S_ApiParseUpgrade *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	iRet = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		if (0 == strcmp(psNode->string,"dest"))
		{
			strncpy(psParse->achDest, psNode->valuestring,sizeof(psParse->achDest));
			iRet++;
		}
		else if (0 == strcmp(psNode->string,"file"))
		{
			strncpy(psParse->achFile, psNode->valuestring,sizeof(psParse->achFile));
			iRet++;
		}
		else if (0 == strcmp(psNode->string,"address"))
		{
			strncpy(psParse->achAddress, psNode->valuestring,sizeof(psParse->achAddress));
			iRet++;
		}
		else
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_post_upgrade]Json packet error[ %s ]\r\n", psNode->string);
			return API_DEAL_ERR_INVALID_MSG;
		}
	}
	if(iRet != 3)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_post_upgrade]Json packet size error [ iRet = %d ]\r\n", iRet);
		return API_DEAL_ERR_INVALID_MSG;
	}
	return iRet;
}



int json_parse_get_remote_download_state (const cJSON *psBody, S_ApiParseRemoteDownload *psParse)
{
	UNUSED(psBody);
	UNUSED(psParse);

	return 0;
}
int json_parse_remote_download (const cJSON *psBody, S_ApiParseRemoteDownload *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	iRet = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		if (0 == strcmp(psNode->string, "link"))
		{
			strncpy(psParse->achLink, psNode->valuestring,sizeof(psParse->achLink));
			iRet++;
		}
		else if (0 == strcmp(psNode->string, "username"))
		{
			strncpy(psParse->achUserName, psNode->valuestring,sizeof(psParse->achUserName));
			iRet++;
		}
		else if (0 == strcmp(psNode->string, "password"))
		{
			strncpy(psParse->achPassWord, psNode->valuestring,sizeof(psParse->achPassWord));
			iRet++;
		}
		else
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_remote_download]Json packet error[ %s ]\r\n", psNode->string);
			return API_DEAL_ERR_INVALID_MSG;
		}
	}
	if(iRet != 3)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_remote_download]Json packet size error [ iRet = %d ]\r\n", iRet);
		return API_DEAL_ERR_INVALID_MSG;
	}
	return iRet;
}

int json_parse_set_test_mode (const cJSON *psBody, S_ApiParseSetTestMode *psParse)
{
	cJSON *psNode;
	int iRet;

	ASSERT(NULL != psBody);
	ASSERT(NULL != psParse);

	iRet = 0;
	for (psNode=psBody->child; NULL!=psNode; psNode=psNode->next)
	{
		if (cJSON_Object == psNode->type)
		{
			continue;
		}
		if (0 == strcmp(psNode->string, "mode"))
		{
			psParse->ui8Mode = (uint8_t)psNode->valueint;
			iRet++;
		}
		else
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_set_test_mode]Json packet error[ %s ]\r\n", psNode->string);
			return API_DEAL_ERR_INVALID_MSG;
		}
	}
	if(iRet != 1)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[json_parse_set_test_mode]Json packet size error [ iRet = %d ]\r\n", iRet);
		return API_DEAL_ERR_INVALID_MSG;
	}
	return iRet;
}









int api_answer_invalid_api (S_ConntCtrl *psConntCtrl, int *piErr)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_invalid_api(&pchBody, piErr);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = api_answer_segment_send(psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}
int api_answer_low_layer_error (S_ConntCtrl *psConntCtrl, int *piErr)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_low_layer_error(&pchBody, piErr);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = api_answer_segment_send(psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}

int api_answer_link_button_not_pressed (const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_link_button_not_pressed(&pchBody);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}
int api_answer_create_user_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseCreateUser *psParse)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_create_user_fail(&pchBody, psParse);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_answer_create_user_failed]Call 'json_assemble_answer_create_user_fail' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}
int api_answer_create_user_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseCreateUser *psParse)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_create_user_succeed(&pchBody, psParse);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_answer_create_user_succeed]Call 'json_assemble_answer_create_user_succeed' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}

int api_answer_get_full_state (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetFullState *psParse)
{
	uint16_t aui6IdList[MUI_LOGIC_DEV_NUM_MAX];
	char achBuf[1+1+MUI_API_KEYWORD_LEN_MAX+1+1];
	char *pchJson;
	int iRet, iNum, i, j;

	UNUSED(psParse);

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "{", sizeof("{")-1, MUI_NET_SEND_NOT_FINISH);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send({)' fail.\r\n");
		return iRet;
	}

	for (i=0; i<(int)(sizeof(s_asGetFullStateKeywordMapList)/sizeof(s_asGetFullStateKeywordMapList[0])); i++)
	{
		if (0 == i)
		{
			iRet = snprintf(achBuf, sizeof(achBuf), "\"%s\":", s_asGetFullStateKeywordMapList[i].achKeyword);
		}
		else
		{
			iRet = snprintf(achBuf, sizeof(achBuf), ",\"%s\":", s_asGetFullStateKeywordMapList[i].achKeyword);
		}
		iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, achBuf, iRet, MUI_NET_SEND_NOT_FINISH);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(...)' fail.\r\n");
			return iRet;
		}

		switch (s_asGetFullStateKeywordMapList[i].ui16Key)
		{
			case MUI_DATA_TYPE_CONFIG:
				iRet = json_assemble_get_config(&pchJson);
				if (iRet <= 0)
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_ERROR, sizeof(MSTR_ANSWER_GET_DATA_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Config Data)' fail.\r\n");
						return iRet;
					}
				}
				else
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_NOT_FINISH);
					json_free(pchJson);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Config Data)' fail.\r\n");
						return iRet;
					}
				}
				break;
			case MUI_DATA_TYPE_GROUP:
				iNum = group_get_id_list(aui6IdList, (sizeof(aui6IdList)/sizeof(aui6IdList[0])));
				if (iNum <= 0)
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_EMPTY, sizeof(MSTR_ANSWER_GET_DATA_EMPTY)-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Group Data)' fail.\r\n");
						return iRet;
					}
				}
				else
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "{", sizeof("{")-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send({)' fail.\r\n");
						return iRet;
					}

					for (j=0; j<iNum; j++)
					{
						if (0 == j)
						{
							iRet = snprintf(achBuf, sizeof(achBuf), "\"%u\":", aui6IdList[j]);
						}
						else
						{
							iRet = snprintf(achBuf, sizeof(achBuf), ",\"%u\":", aui6IdList[j]);
						}

						iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, achBuf, iRet, MUI_NET_SEND_NOT_FINISH);
						if (iRet < 0)
						{
							JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(ui16DevId)' fail.\r\n");
							return iRet;
						}

						iRet = json_assemble_get_group_attr(aui6IdList[j], &pchJson);
						if (iRet < 0)
						{
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_ERROR, sizeof(MSTR_ANSWER_GET_DATA_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Group Data)' fail.\r\n");
								return iRet;
							}
						}
						else
						{
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_NOT_FINISH);
							json_free(pchJson);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Group Data)' fail.\r\n");
								return iRet;
							}
						}
					}

					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "}", sizeof("}")-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(})' fail.\r\n");
						return iRet;
					}
				}
				break;
			case MUI_DATA_TYPE_SET:
				iNum = set_get_id_list(aui6IdList, (sizeof(aui6IdList)/sizeof(aui6IdList[0])));
				if (iNum <= 0)
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_EMPTY, sizeof(MSTR_ANSWER_GET_DATA_EMPTY)-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Set Data)' fail.\r\n");
						return iRet;
					}
				}
				else
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "{", sizeof("{")-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send({)' fail.\r\n");
						return iRet;
					}

					for (j=0; j<iNum; j++)
					{
						if (0 == j)
						{
							iRet = snprintf(achBuf, sizeof(achBuf), "\"%u\":", aui6IdList[j]);
						}
						else
						{
							iRet = snprintf(achBuf, sizeof(achBuf), ",\"%u\":", aui6IdList[j]);
						}

						iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, achBuf, iRet, MUI_NET_SEND_NOT_FINISH);
						if (iRet < 0)
						{
							JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(ui16DevId)' fail.\r\n");
							return iRet;
						}

						iRet = json_assemble_answer_get_set_attr(aui6IdList[j], &pchJson);
						if (iRet < 0)
						{
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_ERROR, sizeof(MSTR_ANSWER_GET_DATA_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Set Data)' fail.\r\n");
								return iRet;
							}
						}
						else
						{
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_NOT_FINISH);
							json_free(pchJson);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Set Data)' fail.\r\n");
								return iRet;
							}
						}
					}

					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "}", sizeof("}")-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(})' fail.\r\n");
						return iRet;
					}
				}
				break;
			case MUI_DATA_TYPE_SCHED:
				iNum = schedule_get_id_list(aui6IdList, (sizeof(aui6IdList)/sizeof(aui6IdList[0])));
				if (iNum <= 0)
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_EMPTY, sizeof(MSTR_ANSWER_GET_DATA_EMPTY)-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Schedule Data)' fail.\r\n");
						return iRet;
					}
				}
				else
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "{", sizeof("{")-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send({)' fail.\r\n");
						return iRet;
					}

					for (j=0; j<iNum; j++)
					{
						if (0 == j)
						{
							iRet = snprintf(achBuf, sizeof(achBuf), "\"%u\":", aui6IdList[j]);
						}
						else
						{
							iRet = snprintf(achBuf, sizeof(achBuf), ",\"%u\":", aui6IdList[j]);
						}

						iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, achBuf, iRet, MUI_NET_SEND_NOT_FINISH);
						if (iRet < 0)
						{
							JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(ui16DevId)' fail.\r\n");
							return iRet;
						}

						iRet = json_assemble_answer_get_schedule_attr(aui6IdList[j], &pchJson);
						if (iRet < 0)
						{
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_ERROR, sizeof(MSTR_ANSWER_GET_DATA_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Schedule Data)' fail.\r\n");
								return iRet;
							}
						}
						else
						{
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_NOT_FINISH);
							json_free(pchJson);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Schedule Data)' fail.\r\n");
								return iRet;
							}
						}
					}

					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "}", sizeof("}")-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(})' fail.\r\n");
						return iRet;
					}
				}
				break;
			case MUI_DATA_TYPE_RULE:
				iNum = rules_get_id_list(aui6IdList, (sizeof(aui6IdList)/sizeof(aui6IdList[0])));
				if (iNum <= 0)
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_EMPTY, sizeof(MSTR_ANSWER_GET_DATA_EMPTY)-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Rule Data)' fail.\r\n");
						return iRet;
					}
				}
				else
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "{", sizeof("{")-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send({)' fail.\r\n");
						return iRet;
					}

					for (j=0; j<iNum; j++)
					{
						if (0 == j)
						{
							iRet = snprintf(achBuf, sizeof(achBuf), "\"%u\":", aui6IdList[j]);
						}
						else
						{
							iRet = snprintf(achBuf, sizeof(achBuf), ",\"%u\":", aui6IdList[j]);
						}

						iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, achBuf, iRet, MUI_NET_SEND_NOT_FINISH);
						if (iRet < 0)
						{
							JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(ui16DevId)' fail.\r\n");
							return iRet;
						}

						iRet = json_assemble_answer_get_rules_attr(aui6IdList[j], &pchJson);
						if (iRet < 0)
						{
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_ERROR, sizeof(MSTR_ANSWER_GET_DATA_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Rule Data)' fail.\r\n");
								return iRet;
							}
						}
						else
						{
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_NOT_FINISH);
							json_free(pchJson);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Rule Data)' fail.\r\n");
								return iRet;
							}
						}
					}

					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "}", sizeof("}")-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(})' fail.\r\n");
						return iRet;
					}
				}
				break;
			default:
				if ((s_asGetFullStateKeywordMapList[i].ui16Key<MUI_DATA_TYPE_DEV_START) || (MUI_DATA_TYPE_DEV_END<s_asGetFullStateKeywordMapList[i].ui16Key))
				{
					iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_UNSUPPORT_DEV, sizeof(MSTR_ANSWER_UNSUPPORT_DEV)-1, MUI_NET_SEND_NOT_FINISH);
					if (iRet < 0)
					{
						JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Device Data)' fail.\r\n");
						return iRet;
					}
				}
				else
				{
					iNum = get_dev_id_list_by_type(s_asGetFullStateKeywordMapList[i].ui16Key, aui6IdList, (sizeof(aui6IdList)/sizeof(aui6IdList[0])));
					if (iNum < 0)
					{
						iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_ERROR, sizeof(MSTR_ANSWER_GET_DATA_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
						if (iRet < 0)
						{
							JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Device Data)' fail.\r\n");
							return iRet;
						}
					}
					else
					{
						iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "{", sizeof("{")-1, MUI_NET_SEND_NOT_FINISH);
						if (iRet < 0)
						{
							JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send({)' fail.\r\n");
							return iRet;
						}

						for (j=0; j<iNum; j++)
						{
							if (0 == j)
							{
								iRet = snprintf(achBuf, sizeof(achBuf), "\"%u\":", aui6IdList[j]);
							}
							else
							{
								iRet = snprintf(achBuf, sizeof(achBuf), ",\"%u\":", aui6IdList[j]);
							}
							iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, achBuf, iRet, MUI_NET_SEND_NOT_FINISH);
							if (iRet < 0)
							{
								JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(ui16DevId)' fail.\r\n");
								return iRet;
							}
							iRet = json_assemble_get_dev_attr_state(s_asGetFullStateKeywordMapList[i].ui16Key, aui6IdList[j], &pchJson);
							if (iRet < 0)
							{
								iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_ERROR, sizeof(MSTR_ANSWER_GET_DATA_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
								if (iRet < 0)
								{
									JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Device Data)' fail.\r\n");
									return iRet;
								}
							}
							else
							{
								iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_NOT_FINISH);
								json_free(pchJson);
								if (iRet < 0)
								{
									JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(Device Data)' fail.\r\n");
									return iRet;
								}
							}
						}

						iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "}", sizeof("}")-1, MUI_NET_SEND_NOT_FINISH);
						if (iRet < 0)
						{
							JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(})' fail.\r\n");
							return iRet;
						}
					}
				}
				break;
		}
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "}", sizeof("}")-1, MUI_NET_SEND_IMMEDIATELY);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_full_state]Call 'api_answer_segment_send(})' fail.\r\n");
	}

	return iRet;
}

int api_answer_search_new_dev_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSearchNewDev *psParse)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_search_new_dev_fail(&pchBody, psParse);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_answer_search_new_dev_failed]Call 'json_assemble_answer_search_new_dev_fail' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}

int api_answer_search_new_dev_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSearchNewDev *psParse)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_search_new_dev_succeed(&pchBody, psParse);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_answer_search_new_dev_succeed]Call 'json_assemble_answer_search_new_dev_succeed' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}

int api_answer_factory_new_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseFactoryNew *psParse)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_factory_new_fail(&pchBody, psParse);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_answer_factory_new_failed]Call 'json_assemble_answer_factory_new_fail' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}
int api_answer_factory_new_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseFactoryNew *psParse)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_factory_new_succeed(&pchBody, psParse);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_answer_factory_new_succeed]Call 'json_assemble_answer_factory_new_succeed' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}

int api_answer_get_config (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetConfig *psParse)
{
	char *pchJson;
	int iRet;

	UNUSED(psParse);

	iRet = json_assemble_get_config(&pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_config]Call 'api_answer_get_config()' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_config]Call 'api_answer_segment_send(Config Data)' fail.\r\n");
	}

	return iRet;
}

int api_answer_set_config (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetConfig *psParse)
{
	cJSON *psR, *psRRemote, *psC1;
	char *pchJson;
	int iRet, i, iTmp;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	psRRemote = NULL;
	//远程上报并不是必须要求成功,因此此处即使未获取到指针,并不需要退出函数
	psRRemote = cJSON_CreateObject();

	iRet = 0;
	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			break;
		}
		switch (s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			case MUI_SET_CONFIG_KEY_WORD_IDX_NAME:
				cJSON_AddStringToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.name.str);
				iTmp = write_config_info_name(&psParse->sSetConfigInfo.name);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_IP_ADDR:
				cJSON_AddStringToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.ipaddr.str);
				iTmp = write_config_info_ip_addr(&psParse->sSetConfigInfo.ipaddr);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_NETMASK:
				cJSON_AddStringToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.netmask.str);
				iTmp = write_config_info_netmask(&psParse->sSetConfigInfo.netmask);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_GW_IP_ADDR:
				cJSON_AddStringToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.gwipaddr.str);
				iTmp = write_config_info_gw_ip_addr(&psParse->sSetConfigInfo.gwipaddr);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_DHCP:
				cJSON_AddBoolToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.dhcp);
				iTmp = write_config_info_dhcp(&psParse->sSetConfigInfo.dhcp);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_PROXY_ADDR:
				cJSON_AddStringToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.proxyaddr.str);
				iTmp = write_config_info_proxy_addr(&psParse->sSetConfigInfo.proxyaddr);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_PROXY_PORT:
				cJSON_AddNumberToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.proxyport);
				iTmp = write_config_info_proxy_port(&psParse->sSetConfigInfo.proxyport);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_PORTAL_SERVICES:
				cJSON_AddBoolToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.portalservices);
				iTmp = write_config_info_portal_services(&psParse->sSetConfigInfo.portalservices);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_DEV_ID:
				cJSON_AddStringToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetConfigInfo.devid.str);
				iTmp = write_config_info_dev_id(&psParse->sSetConfigInfo.devid);
				break;
			case MUI_SET_CONFIG_KEY_WORD_IDX_PASSWD:
				cJSON_AddStringToObject(psC1, s_asSetConfigKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, MSTR_CONFIG_PASSWD_SHOW_MASK);
				iTmp = write_config_info_passwd(&psParse->sSetConfigInfo.passwd);
				break;
			default:
				cJSON_Delete(psC1);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
			if (NULL != psRRemote)
			{
				JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[api_answer_set_config]@@@Added a JSON object to psRRemote.\r\n");
				cJSON_AddObjectDataToObject(psRRemote, psC1);
			}
		}
	}
	if (iRet < 0)
	{
		cJSON_Delete(psR);
		cJSON_Delete(psRRemote);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_config]JSON print error.\r\n");
		cJSON_Delete(psRRemote);
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_config]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	//发送远程上报
	if ((NULL!=psRRemote) && (NULL!=(psR=cJSON_CreateObject())))
	{
		cJSON_AddItemToObject(psR, "config", psRRemote);
		send_report_with_json(MUI_HTTP_METHOD_EX_MODIFY, MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, psR);
	}

	return iRet;
}

int api_answer_get_all_devs_by_type (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetAllDevsByType *psParse)
{
	S_DevApiAttr sApiAttr;
	S_DevZbAttr sZbAttr;
	//U_DevExtAttr uExtAttr;
	U_DevState uState;
	cJSON *psR;
	uint16_t aui6IdList[MUI_LOGIC_DEV_NUM_MAX];
	char achBuf[1+1+5+1+1];	//ID的十进制数据不可能超过5个字符(99999)
	char *pchJson;
	int iRet, iNum, i;

	UNUSED(psParse);

	iNum = get_dev_id_list_by_type(ui16DevType, aui6IdList, (sizeof(aui6IdList)/sizeof(aui6IdList[0])));
	if (iNum < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_devs_by_type]Call 'get_dev_id_list_by_type()' fail.\r\n");
		return iNum;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "{", sizeof("{")-1, MUI_NET_SEND_NOT_FINISH);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_devs_by_type]Call 'api_answer_segment_send({)' fail.\r\n");
		return iRet;
	}

	for (i=0; i<iNum; i++)
	{
		if (0 == i)
		{
			iRet = snprintf(achBuf, sizeof(achBuf), "\"%u\":", aui6IdList[i]);
		}
		else
		{
			iRet = snprintf(achBuf, sizeof(achBuf), ",\"%u\":", aui6IdList[i]);
		}
		iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, achBuf, iRet, MUI_NET_SEND_NOT_FINISH);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_devs_by_type]Call 'api_answer_segment_send(ui16DevId)' fail.\r\n");
			return iRet;
		}

		iRet = 0;
		do
		{
			iRet = read_dev_attr(ui16DevType, aui6IdList[i], &sApiAttr);
			if (iRet < 0)
			{
				break;
			}
			iRet = read_dev_zb_attr(ui16DevType, aui6IdList[i], &sZbAttr);
			if (iRet < 0)
			{
				break;
			}
			//iRet = read_dev_ext_attr(ui16DevType, aui6IdList[i], &uExtAttr);
			//if (iRet < 0)
			//{
			//	break;
			//}
			iRet = read_dev_state(ui16DevType, aui6IdList[i], &uState);
			if (iRet < 0)
			{
				break;
			}
		} while (0);
		if (iRet < 0)
		{
			iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_GET_DATA_ERROR, sizeof(MSTR_ANSWER_GET_DATA_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
			if (iRet < 0)
			{
				JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_devs_by_type]Call 'api_answer_segment_send(MSTR_ANSWER_GET_DATA_ERROR)' fail.\r\n");
				return iRet;
			}
			continue;
		}

		if (NULL == (psR=cJSON_CreateObject()))
		{
			iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_CAN_NOT_CREATE_JSON_OBJ, sizeof(MSTR_ANSWER_CAN_NOT_CREATE_JSON_OBJ)-1, MUI_NET_SEND_NOT_FINISH);
			if (iRet < 0)
			{
				JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_devs_by_type]Call 'api_answer_segment_send(MSTR_ANSWER_CAN_NOT_CREATE_JSON_OBJ)' fail.\r\n");
				return iRet;
			}
			continue;
		}

		//API attribute
		//cJSON_AddStringToObject(psR, "type", sApiAttr.type.str);
		cJSON_AddStringToObject(psR, "name", sApiAttr.name.str);
		cJSON_AddStringToObject(psR, "description", sApiAttr.descr.str);
		cJSON_AddStringToObject(psR, "modelid", sApiAttr.modelid.str);
		//cJSON_AddStringToObject(psR, "swver", sApiAttr.swver.str);
		//cJSON_AddBoolToObject(psR, "changerpt", sApiAttr.changerpt);
		//cJSON_AddNumberToObject(psR, "autorpt", sApiAttr.autorpt);
		//Network attribute
		if (MUI_DATA_TYPE_SONOS == ui16DevType)
		{
			cJSON_AddStringToObject(psR, "ip", sZbAttr.nwkaddr.ip);
			cJSON_AddNumberToObject(psR, "port", sZbAttr.ep.port);
		}
		else
		{
			cJSON_AddStringToObject(psR, "nwkaddr", sZbAttr.nwkaddr.str);
			cJSON_AddStringToObject(psR, "ep", sZbAttr.ep.str);
		}
		//Extend attribute
		//if (MUI_DATA_TYPE_LIGHT == ui16DevType)
		//{
		//	cJSON_AddNumberToObject(psR, "colormode", uExtAttr.light_ext_attr.colormode);
		//}

		//State
		cJSON_AddBoolToObject(psR, "reachable", uState.light_state.reachable);

		iRet = s_json_print_unformat(psR, &pchJson);
		cJSON_Delete(psR);
		if (iRet < 0)
		{
			iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, MSTR_ANSWER_JSON_PRINT_ERROR, sizeof(MSTR_ANSWER_JSON_PRINT_ERROR)-1, MUI_NET_SEND_NOT_FINISH);
			if (iRet < 0)
			{
				JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_devs_by_type]Call 'api_answer_segment_send(MSTR_ANSWER_JSON_PRINT_ERROR)' fail.\r\n");
				return iRet;
			}
			continue;
		}

		iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_NOT_FINISH);
		json_free(pchJson);
		if (iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_devs_by_type]Call 'api_answer_segment_send(Device Data)' fail.\r\n");
			return iRet;
		}
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, "}", sizeof("}")-1, MUI_NET_SEND_IMMEDIATELY);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_devs_by_type]Call 'api_answer_segment_send(})' fail.\r\n");
	}

	return iRet;
}

int api_answer_get_dev_attr_state (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetDevAttrState *psParse)
{
	char *pchJson;
	int iRet;

	UNUSED(psParse);

	iRet = json_assemble_get_dev_attr_state(ui16DevType, ui16Id, &pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_dev_attr_state]Call 'json_assemble_get_dev_attr_state()' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_dev_attr_state]Call 'api_answer_segment_send(Device Data)' fail.\r\n");
	}

	return iRet;
}

int api_answer_set_dev_attr (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetDevAttr *psParse)
{
	cJSON *psR, *psRRemote, *psC1;
	char *pchJson;
	uint8_t ui8ChangeRpt;
	int iRet, i, iTmp;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	psRRemote = NULL;
	if ((0<read_dev_attr_changerpt(ui16DevType, ui16Id, &ui8ChangeRpt)) && (0!=ui8ChangeRpt))
	{
		//远程上报并不是必须要求成功,因此此处即使未获取到指针,并不需要退出函数
		psRRemote = cJSON_CreateObject();
	}

	iRet = 0;
	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			break;
		}
		switch (s_asSetDevAttrKeywordMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_ID:
				cJSON_AddNumberToObject(psC1, s_asSetDevAttrKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevAttr.ui16Id);
				iTmp = swap_dev_id_by_type(ui16DevType, psParse->sSetDevAttr.ui16Id, ui16Id);
				break;
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_NAME:
				cJSON_AddStringToObject(psC1, s_asSetDevAttrKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevAttr.name.str);
				iTmp = write_dev_attr_name(ui16DevType, ui16Id, &psParse->sSetDevAttr.name);
				break;
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_DESCRIPTION:
				cJSON_AddStringToObject(psC1, s_asSetDevAttrKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevAttr.descr.str);
			/*
				char *pch_;
				pch_ = psParse->sSetDevAttr.descr.str;
				JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "********************************\r\n");
				while (*pch_ != '\0')
				{
					JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%02x", *pch_++);
				}
				JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n********************************\r\n");
			*/
				iTmp = write_dev_attr_descr(ui16DevType, ui16Id, &psParse->sSetDevAttr.descr);
				break;
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_CHANGE_RPT:
				cJSON_AddBoolToObject(psC1, s_asSetDevAttrKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevAttr.changerpt);
				iTmp = write_dev_attr_changerpt(ui16DevType, ui16Id, &psParse->sSetDevAttr.changerpt);
				break;
			case MUI_SET_DEV_ATTR_KEY_WORD_IDX_AUTO_RPT:
				cJSON_AddNumberToObject(psC1, s_asSetDevAttrKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevAttr.autorpt);
				iTmp = write_dev_attr_autorpt(ui16DevType, ui16Id, &psParse->sSetDevAttr.autorpt);
				break;
			default:
				cJSON_Delete(psC1);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
			if (NULL != psRRemote)
			{
				cJSON_AddObjectDataToObject(psRRemote, psC1);
			}
		}
	}
	if (iRet < 0)
	{
		cJSON_Delete(psR);
		cJSON_Delete(psRRemote);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_dev_attr]JSON print error.\r\n");
		cJSON_Delete(psRRemote);
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_dev_attr]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	//发送远程上报
	if ((NULL!=psRRemote) && (NULL!=(psR=cJSON_CreateObject())))
	{
		cJSON_AddItemToObject(psR, "attr", psRRemote);
		send_report_with_json(MUI_HTTP_METHOD_EX_MODIFY, ui16DevType, ui16Id, psR);
	}

	return iRet;
}

int api_answer_set_dev_state (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetDevState *psParse)
{
	cJSON *psR, *psRRemote, *psC1, *psC2;
	char *pchJson;
	uint8_t ui8ChangeRpt;
	double adXY[2];
	int iRet, iTmp, i;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	psRRemote = NULL;
	if ((0<read_dev_attr_changerpt(ui16DevType, ui16Id, &ui8ChangeRpt)) && (0!=ui8ChangeRpt))
	{
		//远程上报并不是必须要求成功,因此此处即使未获取到指针,并不需要退出函数
		psRRemote = cJSON_CreateObject();
	}

	iRet = 0;
	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			break;
		}
		switch (s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
				cJSON_AddBoolToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.on);
				iTmp = set_dev_state_on(ui16DevType, ui16Id, &psParse->sSetDevState.on);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.bri);
				iTmp = set_dev_state_bri(ui16DevType, ui16Id, &psParse->sSetDevState.bri);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.sat);
				iTmp = set_dev_state_sat(ui16DevType, ui16Id, &psParse->sSetDevState.sat);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.hue);
				iTmp = set_dev_state_hue(ui16DevType, ui16Id, &psParse->sSetDevState.hue);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.ct);
				iTmp = set_dev_state_ct(ui16DevType, ui16Id, &psParse->sSetDevState.ct);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
				adXY[0] = (double)psParse->sSetDevState.xy.x / 65536.0;
				adXY[1] = (double)psParse->sSetDevState.xy.y / 65536.0;
				psC2 = cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0]));
				if (NULL != psC2)
				{
					cJSON_AddItemToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psC2);
				}
				iTmp = set_dev_state_xy(ui16DevType, ui16Id, &psParse->sSetDevState.xy);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COLOR_MODE:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_ALERT:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_EFFECT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.level);
				iTmp = set_dev_state_level(ui16DevType, ui16Id, &psParse->sSetDevState.level);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, (double)(psParse->sSetDevState.temp)/100.0);
				iTmp = set_dev_state_temp(ui16DevType, ui16Id, &psParse->sSetDevState.temp);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, (double)(psParse->sSetDevState.hum)/100.0);
				iTmp = set_dev_state_hum(ui16DevType, ui16Id, &psParse->sSetDevState.hum);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_CONSUMPTION:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.warnmode);
				iTmp = set_dev_state_warn_mode(ui16DevType, ui16Id, &psParse->sSetDevState.warnmode);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.duration);
				iTmp = set_dev_state_duration(ui16DevType, ui16Id, &psParse->sSetDevState.duration);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				psC2 = cJSON_CreateObject();
				if (NULL != psC2)
				{
					cJSON_AddNumberToObject(psC2, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]-2].achKeyword, psParse->sSetDevState.warnmode);
					cJSON_AddNumberToObject(psC2, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]-1].achKeyword, psParse->sSetDevState.duration);
					cJSON_AddItemToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psC2);
				}
				iTmp = set_dev_state_warn_cfg(ui16DevType, ui16Id, &psParse->sSetDevState.warncfg);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.action);
				iTmp = set_dev_state_action(ui16DevType, ui16Id, &psParse->sSetDevState.action);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.play);
				iTmp = set_dev_state_play(ui16DevType, ui16Id, &psParse->sSetDevState.play);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.volume);
				iTmp = set_dev_state_volume(ui16DevType, ui16Id, &psParse->sSetDevState.volume);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
				cJSON_AddNumberToObject(psC1, s_asDataKeywordMapDescList[ui16DevType-MUI_DATA_TYPE_START].psKeywordMap[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.urlid);
				iTmp = set_dev_state_url_id(ui16DevType, ui16Id, &psParse->sSetDevState.urlid);
				break;
			default:
				cJSON_Delete(psC1);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
			if (NULL != psRRemote)
			{
				cJSON_AddObjectDataToObject(psRRemote, psC1);
			}
		}
	}
	if (iRet < 0)
	{
		cJSON_Delete(psR);
		cJSON_Delete(psRRemote);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_dev_state]JSON print error.\r\n");
		cJSON_Delete(psRRemote);
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_dev_state]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	//发送远程上报
	if ((NULL!=psRRemote) && (NULL!=(psR=cJSON_CreateObject())))
	{
		cJSON_AddItemToObject(psR, "state", psRRemote);
		send_report_with_json(MUI_HTTP_METHOD_EX_MODIFY, ui16DevType, ui16Id, psR);
	}

	return iRet;
}

int api_answer_del_dev_fail (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseDelDev *psParse)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_del_dev_fail(ui16DevType, ui16Id, &pchBody, psParse);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_answer_del_dev_fail]Call 'json_assemble_answer_del_dev_fail' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}
int api_answer_del_dev_succeed (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseDelDev *psParse)
{
	char *pchBody;
	int iRet;

	iRet = json_assemble_answer_del_dev_succeed(ui16DevType, ui16Id, &pchBody, psParse);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_answer_del_dev_succeed]Call 'json_assemble_answer_del_dev_succeed' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);

	return iRet;
}

int api_answer_get_all_groups (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetAllGroups *psParse)
{
	S_AttrName sAttrName;
	cJSON *psR, *psC1;
	uint16_t aui6IdList[MUI_LOGIC_DEV_NUM_MAX];
	char achBuf[5+1];	//ID的十进制数据不可能超过5个字符(99999)
	char *pchJson;
	int iRet, iNum, i;

	UNUSED(psParse);

	iNum = group_get_id_list(aui6IdList, sizeof(aui6IdList)/sizeof(aui6IdList[0]));
	if (iNum < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_groups]Call 'group_get_id_list()' fail.\r\n");
		//return iNum;
	}

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	for (i=0; i<iNum; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			continue;
		}
		snprintf(achBuf, sizeof(achBuf), "%u", aui6IdList[i]);
		cJSON_AddItemToObject(psR, achBuf, psC1);

		iRet = group_read_name(aui6IdList[i], &sAttrName);
		if (iRet < 0)
		{
			cJSON_AddStringToObject(psC1, "name", MSTR_ANSWER_GET_DATA_ERROR);
		}
		else
		{
			cJSON_AddStringToObject(psC1, "name", sAttrName.str);
		}
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_groups]Call 's_json_print_unformat()' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_all_groups]Call 'api_answer_segment_send(Device Data)' fail.\r\n");
		return iRet;
	}

	return iRet;
}

int api_answer_get_group_attr (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetGroupAttr *psParse)
{
	char *pchJson;
	int iRet;

	UNUSED(psParse);

	iRet = json_assemble_get_group_attr(ui16Id, &pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_group_attr]Call 'json_assemble_get_group_attr()' fail.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_group_attr]Call 'api_answer_segment_send(Group Data)' fail.\r\n");
	}

	return iRet;
}

int api_answer_set_group_attr (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetGroup *psParse)
{
	cJSON *psR, *psC1, *psC2, *psC3;
	char *pchJson;
	char idstr[2]="";
	char address[30]="";
	int iRet=0, i, j, k, iTmp=0;
	S_GroupOpAttr	opAttr;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_group_attr]Call 'cJSON_CreateObject fail.\r\n");
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	iRet = group_read_op_attr(ui16Id, &opAttr);
	if(iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_group_attr]Call 'group_read_op_attr fail :%d.\r\n",iRet);
		return iRet;
	}

	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			break;
		}
		switch (s_aachGroupTypeMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_NAME:
				cJSON_AddStringToObject(psC1, s_aachGroupTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetGroupAttr.name.str);
				iTmp = group_write_name(ui16Id, &psParse->sSetGroupAttr.name);
				break;
			case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_DESCRIPTION:
				cJSON_AddStringToObject(psC1, s_aachGroupTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetGroupAttr.descr.str);
				iTmp = group_write_description(ui16Id, &psParse->sSetGroupAttr.descr);
				break;
			case MUI_SET_GROUP_ATTR_KEY_WORD_IDX_CONFGI:
				if (NULL == (psC2=cJSON_CreateObject())) break;
				cJSON_AddItemToObject(psC1, "config", psC2);

				for (j=0; j<psParse->sSetGroupAttr.opAttr.total; j++)
				{
					for (k=0; k<opAttr.total; k++)
					{
						if (psParse->sSetGroupAttr.opAttr.member[j].devType == opAttr.member[k].devType && psParse->sSetGroupAttr.opAttr.member[j].devId == opAttr.member[k].devId)
						{
							break;
						}
					}
					if (k == opAttr.total)
					{
						if (NULL == (psC3=cJSON_CreateObject())) break;
						sprintf(idstr, "%d", j+1);
						cJSON_AddItemToObject(psC2, idstr, psC3);
						iTmp = group_dev_join(ui16Id, psParse->sSetGroupAttr.opAttr.member[j].devType, psParse->sSetGroupAttr.opAttr.member[j].devId);
						sprintf(address, "/%s/%d", s_asGetFullStateKeywordMapList[psParse->sSetGroupAttr.opAttr.member[j].devType-1].achKeyword, psParse->sSetGroupAttr.opAttr.member[j].devId);
						if(iTmp > 0)
						{
							cJSON_AddStringToObject(psC3, "success add", address);
							//iTmp = group_add_member(ui16Id, psParse->sSetGroupAttr.opAttr.member[j].devId, psParse->sSetGroupAttr.opAttr.member[j].devType);
						}
						else
						{
							cJSON_AddStringToObject(psC3, "fail add", address);
						}
					}
				}

				for (j=0; j<opAttr.total; j++)
				{
					for (k=0; k<psParse->sSetGroupAttr.opAttr.total; k++)
					{
						if (psParse->sSetGroupAttr.opAttr.member[k].devType == opAttr.member[j].devType && psParse->sSetGroupAttr.opAttr.member[k].devId == opAttr.member[j].devId)
						{
							break;
						}
					}
					if (k == psParse->sSetGroupAttr.opAttr.total)
					{
						if (NULL == (psC3=cJSON_CreateObject())) break;
						sprintf(idstr, "%d", j+1);
						cJSON_AddItemToObject(psC2, idstr, psC3);
						iTmp = group_dev_del(ui16Id, opAttr.member[j].devType, opAttr.member[j].devId);
						sprintf(address, "/%s/%d", s_asGetFullStateKeywordMapList[opAttr.member[j].devType-1].achKeyword, opAttr.member[j].devId);
						if(iTmp > 0)
						{
							cJSON_AddStringToObject(psC3, "success delete", address);
							//iTmp = group_del_member(ui16Id, opAttr.member[j].devId, opAttr.member[j].devType);
						}
						else
						{
							cJSON_AddStringToObject(psC3, "fail delete", address);
						}
					}
				}
				break;
			default:
				break;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		}
	}
	if (iRet < 0)
	{
		cJSON_Delete(psR);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_group_attr]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_group_attr]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_set_group_state (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_SetGroupState *psParse)
{
	cJSON *psR, *psC1, *psC2;
	char *pchJson;
	double adXY[2];
	int iRet, iTmp, i;
	S_GroupOpAttr opattr;

	iRet = group_read_op_attr(ui16Id, &opattr);
	if (iRet < 0)
	{
		return iRet;
	}

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			break;
		}
		switch (s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
				cJSON_AddBoolToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.on);
				iTmp = group_set_state_on(ui16Id, &psParse->sSetDevState.on);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
				cJSON_AddNumberToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.bri);
				iTmp = group_light_state_bri( ui16Id, &psParse->sSetDevState.bri);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
				cJSON_AddNumberToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.sat);
				//iTmp = group_set_state_sat(ui16Id, &psParse->sSetDevState.sat);
				iTmp = -1;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
				cJSON_AddNumberToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.hue);
				//iTmp = group_set_state_hue(ui16Id, &psParse->sSetDevState.hue);
				iTmp = -1;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
				cJSON_AddNumberToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.ct);
				iTmp = group_light_state_ct(ui16Id, &psParse->sSetDevState.ct);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
				adXY[0] = (double)psParse->sSetDevState.xy.x / 65536.0;
				adXY[1] = (double)psParse->sSetDevState.xy.y / 65536.0;
				psC2 = cJSON_CreateDoubleArray(adXY, sizeof(adXY)/sizeof(adXY[0]));
				if (NULL != psC2)
				{
					cJSON_AddItemToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psC2);
				}
				iTmp = group_light_state_xy(ui16Id, &psParse->sSetDevState.xy);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COLOR_MODE:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_ALERT:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_EFFECT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
				cJSON_AddNumberToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.level);
				iTmp = group_set_state_level(ui16Id, &psParse->sSetDevState.level);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				cJSON_AddNumberToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, (double)(psParse->sSetDevState.temp)/100.0);
				//iTmp = group_set_state_temp(ui16Id, &psParse->sSetDevState.temp);
				iTmp = -1;
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				cJSON_AddNumberToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, (double)(psParse->sSetDevState.hum)/100.0);
				//iTmp = group_set_state_hum(ui16Id, &psParse->sSetDevState.hum);
				iTmp = -1;
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_CONSUMPTION:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				psC2 = cJSON_CreateObject();
				if (NULL != psC2)
				{
					cJSON_AddNumberToObject(psC2, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.warnmode);
					cJSON_AddNumberToObject(psC2, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.duration);
					cJSON_AddItemToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psC2);
				}
				iTmp = group_warn_dev_state_warn_cfg(ui16Id, &psParse->sSetDevState.warncfg);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
				cJSON_AddNumberToObject(psC1, s_aachGroupConfigActionTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSetDevState.action);
				iTmp = group_shade_state_action(ui16Id, &psParse->sSetDevState.action);
				break;
			default:
				cJSON_Delete(psC1);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		}
	}
	if (iRet < 0)
	{
		cJSON_Delete(psR);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_group_state]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_group_state]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}

int api_answer_create_group(const S_ApiDealCtrl *psApiDealCtrl, S_SetGroupAttr *psParse)
{
	cJSON *psR, *psC1, *psC2;
	char *pchJson;
	int iRet=0;
	S_GroupAttrFlash sGroupAttr;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC1=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC2=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	strncpy(sGroupAttr.api_attr.name.str, psParse->name.str, sizeof(sGroupAttr.api_attr.name.str));
	strncpy(sGroupAttr.api_attr.descr.str, psParse->descr.str, sizeof(sGroupAttr.api_attr.descr.str));
	memcpy(&sGroupAttr.op_attr, &psParse->opAttr, sizeof(S_GroupOpAttr));
	PRINT("==========================================================\r\n\n");
	PRINT("total = %hu\r\n", sGroupAttr.op_attr.total);
	PRINT("type = %hu,   %hu\r\n", psParse->opAttr.member[0].devType, sGroupAttr.op_attr.member[0].devType);
	PRINT("type = %hu,   %hu\r\n", psParse->opAttr.member[1].devType, sGroupAttr.op_attr.member[1].devType);
	PRINT("type = %hu,   %hu\r\n", psParse->opAttr.member[2].devType, sGroupAttr.op_attr.member[2].devType);

	iRet = group_create(&sGroupAttr);
	if (iRet < 0)
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		cJSON_AddNumberToObject(psC1, "error", iRet);
	}
	else
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		cJSON_AddNumberToObject(psC1, "id", iRet);
	}


	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_group]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_group]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int json_assemble_answer_del_group(uint16_t ui16Id, char **ppchJson)
{
	cJSON *psR;
	int iRet = JSON_DEAL_ERR_MEM_FAULT;
	char buf[20]="";

	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		iRet = group_del(ui16Id);
		sprintf(buf, MSTR_API_URL_KEY_WORD_GROUPS"/%d deleted.", ui16Id);
		if (iRet < 0)
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE, buf);
		}else
		{
			cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS, buf);
		}


		iRet = s_json_print_unformat(psR, ppchJson);
	} while (0);

	cJSON_Delete(psR);

	return iRet;

}

int api_answer_set_set_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetSet *psParse)
{
	cJSON *psR, *psC1=NULL;
	char *pchJson;
	int iRet=0, i, iTmp=0;
	int execute = 0;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_set_attr]Call 'cJSON_CreateObject fail.\r\n");
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_set_attr]Call 'cJSON_CreateObject fail.\r\n");
			break;
		}
		switch (s_aachSetTypeMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			case MUI_SET_SET_ATTR_KEY_WORD_IDX_NAME:
				cJSON_AddStringToObject(psC1, s_aachSetTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->setattr.name.str);
				iTmp = set_write_name(ui16Id, &psParse->setattr.name);
				break;
			case MUI_SET_SET_ATTR_KEY_WORD_IDX_DESCRIPTION:
				cJSON_AddStringToObject(psC1, s_aachSetTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->setattr.descr.str);
				iTmp = set_write_description(ui16Id, &psParse->setattr.descr);
				break;
			case MUI_SET_SET_ATTR_KEY_WORD_IDX_DELAY:
				cJSON_AddNumberToObject(psC1, s_aachSetTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->setattr.delay);
				iTmp = set_write_delay(ui16Id, &psParse->setattr.delay);
				break;
			case MUI_SET_SET_ATTR_KEY_WORD_IDX_EXECUTE:
				execute = 1;
				break;
			default:
				cJSON_Delete(psC1);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		}
	}
	if (execute > 0)
	{
		iTmp = set_execute_active(ui16Id, psParse->execute);
		cJSON_AddBoolToObject(psC1, "execute", iTmp);
	}

	if (iRet < 0)
	{
		cJSON_Delete(psR);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_set_attr]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_set_attr]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_create_set(const S_ApiDealCtrl *psApiDealCtrl, S_SetAttr *psParse)
{
	cJSON *psR, *psC1, *psC2;
	char *pchJson;
	int iRet=0;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC1=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC2=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	iRet = set_add(psParse, sizeof(S_SetAttr));
	if (iRet < 0)
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		cJSON_AddNumberToObject(psC1, "error", iRet);
	}
	else
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		cJSON_AddNumberToObject(psC1, "id", iRet);
	}


	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_group]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_group]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_set_scene_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetScene *psParse)
{
	cJSON *psR, *psC1=NULL;
	char *pchJson;
	int iRet=0, i, iTmp=0;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_scene_attr]Call 'cJSON_CreateObject fail.\r\n");
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_scene_attr]Call 'cJSON_CreateObject fail.\r\n");
			break;
		}
		switch (s_aachSceneAttrTypeMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_NAME:
				cJSON_AddStringToObject(psC1, s_aachSceneAttrTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->scene_data.scene_attr.name.str);
				iTmp = scene_write_name(ui16Id, &psParse->scene_data.scene_attr.name);
				break;
			case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_DESCRIPTION:
				cJSON_AddStringToObject(psC1, s_aachSceneAttrTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->scene_data.scene_attr.descr.str);
				iTmp = scene_write_description(ui16Id, &psParse->scene_data.scene_attr.descr);
				break;
			case MUI_SET_SCENE_ATTR_KEY_WORD_IDX_EXECUTE:
				PRINT("-=-=-=-=-=-=-=-=-=-=-=-=-execute=%d-=-=-=-=-=-=-=-=-=-\r\n", psParse->execute);
				if (psParse->execute > 0)
				{
					iTmp = scene_active(ui16Id);
					cJSON_AddBoolToObject(psC1, "execute", iTmp);
				}
				break;
			default:
				cJSON_Delete(psC1);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		}
	}

	if (iRet < 0)
	{
		cJSON_Delete(psR);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_set_attr]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_set_attr]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}


int api_answer_create_scene(const S_ApiDealCtrl *psApiDealCtrl, S_SceneDataFalsh *psParse)
{
	cJSON *psR, *psC1, *psC2;
	char *pchJson;
	int iRet=0;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC1=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC2=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	iRet = scene_create(psParse);
	if (iRet < 0)
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		cJSON_AddNumberToObject(psC1, "error", iRet);
	}
	else
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		cJSON_AddNumberToObject(psC1, "id", iRet);
	}


	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_group]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_group]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_create_schedule(const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSchedule *psParse)
{
	cJSON *psR, *psC1, *psC2;
	char *pchJson;
	int iRet=0;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC1=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC2=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	iRet = schedule_add(psParse);
	if (iRet < 0)
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		cJSON_AddNumberToObject(psC1, "error", iRet);
	}
	else
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		cJSON_AddNumberToObject(psC1, "id", iRet);
	}


	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_schedule]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_schedule]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_set_schedule_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetschedule *psParse)
{
	cJSON *psR, *psC1;
	char *pchJson;
	int iRet=0, i, iTmp=0;
	char time[20]="";

	if (NULL == (psR=cJSON_CreateObject()))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_schedule_attr]Call 'cJSON_CreateObject fail.\r\n");
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_schedule_attr]Call 'cJSON_CreateObject fail.\r\n");
			break;
		}
		switch (s_aachScheduleTypeMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_NAME:
				cJSON_AddStringToObject(psC1, s_aachScheduleTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->ScheduleAttr.name.str);
				iTmp = schedule_write_name(ui16Id, &psParse->ScheduleAttr.name);
				break;
			case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_DESCRIPTION:
				cJSON_AddStringToObject(psC1, s_aachScheduleTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->ScheduleAttr.descr.str);
				iTmp = schedule_write_desc(ui16Id, &psParse->ScheduleAttr.descr);
				break;
			case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_TIME:
				if (psParse->ScheduleAttr.time.time_mode == 2)
				{
					sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02d", psParse->ScheduleAttr.time.Time.tm_year, psParse->ScheduleAttr.time.Time.tm_mon, psParse->ScheduleAttr.time.Time.tm_mday, psParse->ScheduleAttr.time.Time.tm_hour, psParse->ScheduleAttr.time.Time.tm_min, psParse->ScheduleAttr.time.Time.tm_sec);
				}
				else if (psParse->ScheduleAttr.time.time_mode == 1)
				{
					sprintf(time, "T%02d:%02d:%02d", psParse->ScheduleAttr.time.Time.tm_hour, psParse->ScheduleAttr.time.Time.tm_min, psParse->ScheduleAttr.time.Time.tm_sec);
				}

				cJSON_AddStringToObject(psC1, s_aachRulesTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, time);
				iTmp = schedule_write_time(ui16Id, &psParse->ScheduleAttr.time);
				break;
			case MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_REPEAT:
				cJSON_AddNumberToObject(psC1, s_aachScheduleTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->ScheduleAttr.repeat);
				iTmp = schedule_write_repeat(ui16Id, &psParse->ScheduleAttr.repeat);
				break;
			default:
				//cJSON_Delete(psR);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		}
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_schedule_attr]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_schedule_attr]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_create_rules(const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseRules *psParse)
{
	cJSON *psR, *psC1, *psC2;
	char *pchJson;
	int iRet=0;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC1=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}
	if (NULL == (psC2=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	iRet = rules_add(psParse);
	if (iRet < 0)
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		cJSON_AddNumberToObject(psC1, "error", iRet);
	}
	else
	{
		cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		cJSON_AddNumberToObject(psC1, "id", iRet);
	}


	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_group]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_create_group]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_set_rules_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetRules *psParse)
{
	cJSON *psR, *psC1;
	char *pchJson;
	int iRet=0, i, iTmp=0;
	char time[20]="";

	if (NULL == (psR=cJSON_CreateObject()))
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_rules_attr]Call 'cJSON_CreateObject fail.\r\n");
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_rules_attr]Call 'cJSON_CreateObject fail.\r\n");
			break;
		}
		switch (s_aachRulesTypeMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			case MUI_SET_RULES_ATTR_KEY_WORD_IDX_NAME:
				cJSON_AddStringToObject(psC1, s_aachRulesTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->RulesAttr.name.str);
				iTmp = rules_write_name(ui16Id, &psParse->RulesAttr.name);
				break;
			case MUI_SET_RULES_ATTR_KEY_WORD_IDX_DESCRIPTION:
				cJSON_AddStringToObject(psC1, s_aachRulesTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->RulesAttr.descr.str);
				iTmp = rules_write_description(ui16Id, &psParse->RulesAttr.descr);
				break;
			case MUI_SET_RULES_ATTR_KEY_WORD_IDX_ACTIVE:
				cJSON_AddBoolToObject(psC1, s_aachRulesTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->RulesAttr.active);
				iTmp = rules_write_active(ui16Id, &psParse->RulesAttr.active);
				break;
			case MUI_SET_RULES_ATTR_KEY_WORD_IDX_TIME:
				if (psParse->RulesAttr.time.time_mode == 2)
				{
					sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02d", psParse->RulesAttr.time.Time.tm_year, psParse->RulesAttr.time.Time.tm_mon, psParse->RulesAttr.time.Time.tm_mday, psParse->RulesAttr.time.Time.tm_hour, psParse->RulesAttr.time.Time.tm_min, psParse->RulesAttr.time.Time.tm_sec);
				}
				else if (psParse->RulesAttr.time.time_mode == 1)
				{
					sprintf(time, "T%02d:%02d:%02d", psParse->RulesAttr.time.Time.tm_hour, psParse->RulesAttr.time.Time.tm_min, psParse->RulesAttr.time.Time.tm_sec);
				}
				cJSON_AddStringToObject(psC1, s_aachRulesTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, time);
				iTmp = rules_write_time(ui16Id, &psParse->RulesAttr.time);
				break;
			case MUI_SET_RULES_ATTR_KEY_WORD_IDX_REPEAT:
				cJSON_AddNumberToObject(psC1, s_aachRulesTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->RulesAttr.repeat);
				iTmp = rules_write_repeat(ui16Id, &psParse->RulesAttr.repeat);
			break;
			case MUI_SET_RULES_ATTR_KEY_WORD_IDX_DURATION:
				cJSON_AddNumberToObject(psC1, s_aachRulesTypeMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->RulesAttr.duration);
				iTmp = rules_write_duration(ui16Id, &psParse->RulesAttr.duration);
				break;
			default:
				cJSON_Delete(psC1);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		}
	}
	if (iRet < 0)
	{
		cJSON_Delete(psR);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_rules_attr]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_rules_attr]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_put_rtc_sys_time (const S_ApiDealCtrl *psApiDealCtrl, S_SysTime *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;
	/*
	JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[api_answer_put_rtc_sys_time] RTC[%d-%d-%dT%d:%d:%d] Wday:[%d]\r\n",
	psParse->m_year,psParse->m_mon,psParse->m_mday,psParse->m_hur,psParse->m_min,psParse->m_sec,psParse->m_wday);
	*/
	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;

	update_sys_time(psParse);
	cJSON_AddStringToObject(psR, "time"," time set succed");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_rtc_sys_time]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_rtc_sys_time]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}
	return iRet;
}

int api_answer_get_sys (const S_ApiDealCtrl *psApiDealCtrl, S_SysInfo *psParse)
{
	cJSON *psR;
	char *pchJson;
	int iRet;

	UNUSED(psParse);

	iRet = read_factory_info_sn (&psParse->sSn);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys] read_factory_info_sn error.\r\n");
		return iRet;
	}
	iRet = read_factory_info_mac (&psParse->sMac);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys] read_factory_info_mac error.\r\n");
		return iRet;
	}
	iRet = read_factory_info_wifi_mac (&psParse->sWifiMac);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys] read_factory_info_wifi_mac error.\r\n");
		return iRet;
	}
	iRet = read_config_info_uzip_sn(&psParse->sUzipSn);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys] read_config_info_uzip_sn error.\r\n");
		return iRet;
	}
	iRet = read_config_info_uzip_mac(&psParse->sUzipMac);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys] read_config_info_uzip_mac error.\r\n");
		return iRet;
	}

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	cJSON_AddStringToObject(psR, "sn", psParse->sSn.str);
	cJSON_AddStringToObject(psR, "mac", psParse->sMac.str);
	cJSON_AddStringToObject(psR, "wifimac", psParse->sWifiMac.str);
	cJSON_AddStringToObject(psR, "uzipsn", psParse->sUzipSn.str);
	cJSON_AddStringToObject(psR, "uzipmac", psParse->sUzipMac.str);

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}

int api_answer_set_sys (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetSys *psParse)
{
	cJSON *psR, *psC1;
	char *pchJson;
	int iRet, i, iTmp;

	if (NULL == (psR=cJSON_CreateObject()))
	{
		return JSON_DEAL_ERR_MEM_FAULT;
	}

	iRet = 0;
	for (i=0; i<(int)psParse->tCnt; i++)
	{
		if (NULL == (psC1=cJSON_CreateObject()))
		{
			iRet = JSON_DEAL_ERR_MEM_FAULT;
			break;
		}
		switch (s_asSetSysKeywordMapList[psParse->atKeywordMapIdxList[i]].ui16Key)
		{
			case MUI_SET_SYS_KEY_WORD_IDX_SN:
				cJSON_AddStringToObject(psC1, s_asSetSysKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSysInfo.sSn.str);
				iTmp = write_factory_info_sn(&psParse->sSysInfo.sSn);
				break;
			case MUI_SET_SYS_KEY_WORD_IDX_MAC:
				cJSON_AddStringToObject(psC1, s_asSetSysKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSysInfo.sMac.str);
				iTmp = write_factory_info_mac(&psParse->sSysInfo.sMac);
				break;
			case MUI_SET_SYS_KEY_WORD_IDX_WIFI_MAC:
				cJSON_AddStringToObject(psC1, s_asSetSysKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSysInfo.sWifiMac.str);
				iTmp = write_factory_info_wifimac(&psParse->sSysInfo.sWifiMac);
				break;
			case MUI_SET_SYS_KEY_WORD_IDX_UZIPSN:
				cJSON_AddStringToObject(psC1, s_asSetSysKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSysInfo.sUzipSn.str);
				iTmp = write_config_info_uzip_sn(&psParse->sSysInfo.sUzipSn);
				break;
			case MUI_SET_SYS_KEY_WORD_IDX_UZIPMAC:
				cJSON_AddStringToObject(psC1, s_asSetSysKeywordMapList[psParse->atKeywordMapIdxList[i]].achKeyword, psParse->sSysInfo.sUzipMac.str);
				iTmp = write_config_info_uzip_mac(&psParse->sSysInfo.sUzipMac);
				break;
			default:
				cJSON_Delete(psC1);
				continue;
		}
		if (iTmp < 0)
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_FAILURE, psC1);
		}
		else
		{
			cJSON_AddItemToObject(psR, MSTR_API_ANSWER_SUCCESS, psC1);
		}
	}
	if (iRet < 0)
	{
		cJSON_Delete(psR);
		return iRet;
	}

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_config]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_config]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}

int api_answer_get_debug (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParsedebug *psParse)
{
	cJSON *psR;
	char *pchJson;
	int iRet;

	psParse->ui32mask = dbg_info_mask_get () >> 4;
	psParse->ui8level = (uint8_t)dbg_info_level_get ();
	if(psParse->ui8level >= MUI_API_DEBUG_LEVEL_MAX)
	{
		psParse->ui8level = MUI_API_DEBUG_LEVEL_MAX;
	}
	//JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_get_debug]############ mask = [ %x ]  level = [ %d ]################\r\n",psParse->ui32mask,psParse->ui8level);

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddNumberToObject(psR, "mask",psParse->ui32mask);
	cJSON_AddNumberToObject(psR, "level",psParse->ui8level);

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_rtc_sys_time]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_debug]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}
	return iRet;
}
int api_answer_set_debug (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParsedebug *psParse)
{
	cJSON *psR;
	char *pchJson;
	int iRet;

	//JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[json_assemble_answer_put_debug_state]############ mask = [ %x ]  level = [ %d ]################\r\n",psParse->ui32mask,psParse->ui8level);
	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	dbg_info_mask_set (psParse->ui32mask<<4);
	cJSON_AddStringToObject(psR, "mask"," mask set succed");
	dbg_info_level_set (psParse->ui8level);
	cJSON_AddStringToObject(psR, "level"," level set succed");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_rtc_sys_time]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_debug]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}

int api_answer_put_at_debug_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseAtdebug *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	UNUSED(psParse);

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE,"Can't  AT command send fail");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_rtc_sys_time]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_at_debug_failed]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}
int api_answer_put_at_debug_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseAtdebug *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS,psParse->achAnswer);

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_rtc_sys_time]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_at_debug_succeed]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}






int api_answer_get_upgrade_state(const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseUpgrade *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	char address[36]="";
	char state[36]="";
	char achAnswer[100];
	uint8_t deviceupgradestate;
	char device[17]="";
	S_AttrModelId psModelId;
	S_DevIdx asDevIdx[MUI_AT_DEV_EP_NUM_MAX];
	uint16_t ui16Capacity;

	UNUSED(psParse);

	iRet = 0;
	//设备升级状态有三种: 00 没有升级或者升级完成，01 下载程序 02 升级设备中
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), 10000,"AT$IMGSTATUS\r\n");
	if (iRet <= 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[api_answer_get_upgrade_state]AT$IMFSTATUS send fail.\r\n");
		return iRet;
	}
	JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[api_answer_get_upgrade_state]device upgrade state %s\r\n",achAnswer);
	iRet = sscanf(achAnswer, "$IMGSTATUS:%02hhx,%s",&deviceupgradestate,psModelId.str);
	JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[api_answer_get_upgrade_state] [iRet = %d][%hhu][%s]\r\n",iRet,deviceupgradestate,psModelId.str);
	if(iRet <= 0)
	{
		memcpy(address,"none",36);
		memcpy(state,"none",36);
	}
	else if(iRet == 1)
	{
		if(0 == deviceupgradestate)
		{
			memcpy(address,"none",24);
			memcpy(state,"uzip no upgrade or finish upgrade",36);

		}
	}
	else
	{
		iRet = find_devs_by_modelid (&psModelId,&asDevIdx, sizeof(asDevIdx)/sizeof(S_DevIdx));
		if(iRet < 0)
		{
			JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[api_answer_get_upgrade_state] this modelid <--> device \r\n");
			return iRet;
		}
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[api_answer_get_upgrade_state] [iRet = %d][%hu][%hu]\r\n",iRet,asDevIdx[0].ui16Type,asDevIdx[0].ui16Id);
		find_device_by_device_type(asDevIdx[0].ui16Type,device);
		sprintf(address,"%s/%hu",device,asDevIdx[0].ui16Id);
		if(1 == deviceupgradestate)
		{
			//memcpy(address,address,36);
			memcpy(state,"uzip download upgradefile",36);
		}
		else if(2 == deviceupgradestate)
		{
			//memcpy(address,address,36);
			memcpy(state,"device upgrading",36);
		}
	}
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		cJSON_AddStringToObject(psR, "address",address);
		cJSON_AddStringToObject(psR, "state",state);
	}while(0);

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_remote_download_state]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_remote_download_state]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}
int api_answer_post_upgrade_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseUpgrade *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	UNUSED(psParse);

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE,"Can't upgrade AT command send fail");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_post_upgrade_failed]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_post_upgrade_failed]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}
int api_answer_post_upgrade_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseUpgrade *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	UNUSED(psParse);

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS,"Have upgrade AT command send succeed");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_post_upgrade_succeed]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_post_upgrade_succeed]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}

int api_answer_get_remote_download_state (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseRemoteDownload *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	UNUSED(psParse);

	uint8_t downloadstate = ftp_update_listen_state();

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	do
	{
		if(0 == downloadstate)
			cJSON_AddStringToObject(psR, "upgrade","none");
		else if(downloadstate == 1)
			cJSON_AddStringToObject(psR, "upgrade","upgrading");
		else if(downloadstate == 2)
			cJSON_AddStringToObject(psR, "upgrade","ready");
	}while(0);

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_remote_download_state]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_remote_download_state]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;
}
int api_answer_remote_post_download_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseRemoteDownload *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	UNUSED(psParse);

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE,"Can't create ftp pthread");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_rtc_sys_time]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}
int api_answer_remote_post_download_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseRemoteDownload *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	UNUSED(psParse);

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS,"Have create ftp pthread");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_put_rtc_sys_time]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_get_sys]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}

int api_answer_set_test_mode_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetTestMode *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	UNUSED(psParse);

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddStringToObject(psR, MSTR_API_ANSWER_FAILURE, "Set test mode");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_test_mode_failed]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_test_mode_failed]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}
int api_answer_set_test_mode_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetTestMode *psParse)
{
	char *pchJson;
	cJSON *psR;
	int iRet;

	UNUSED(psParse);

	iRet = 0;
	if (NULL == (psR=cJSON_CreateObject()))	return iRet;
	cJSON_AddStringToObject(psR, MSTR_API_ANSWER_SUCCESS, "Set test mode");

	iRet = s_json_print_unformat(psR, &pchJson);
	cJSON_Delete(psR);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_test_mode_succeed]JSON print error.\r\n");
		return iRet;
	}

	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchJson, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchJson);
	if (iRet < 0)
	{
		JSON_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_set_test_mode_succeed]Call 'api_answer_segment_send(Answer Data)' fail.\r\n");
	}

	return iRet;

}


