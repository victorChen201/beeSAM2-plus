//#define	__M_FILE_NAME_api_deal_H__	//屏蔽本头文件
#ifndef __M_FILE_NAME_api_deal_H__
#define __M_FILE_NAME_api_deal_H__

#include "common.h"
#include "data_common.h"
#include "at_port.h"
#include "dev_if.h"
#include "net_manage.h"
#include "cJSON.h"
#include "database.h"
#include "dev_data.h"
#include "group_data.h"
#include "set_data.h"
#include "schedule.h"
#include "rule.h"
#include "scene_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_API_URL_GRADE_MAX								6
#define MUI_API_URL_PREFIX_LEN_MAX							8	//"/ha", "/api"
#define MUI_API_URL_USERNAME_LEN_MAX						(1+MUI_USER_NAME_LEN+1)	//"/<username>"
#define MUI_API_URL_KEY_WORD_LEN_MAX						20
#if (MUI_API_URL_USERNAME_LEN_MAX < MUI_API_URL_KEY_WORD_LEN_MAX)
	#error "Username lenght of API URL is wrong."
#endif

#define MSTR_API_URL_PREFIX_1								"/ha"
#define MSTR_API_URL_PREFIX_2								"/api"

#define MSTR_API_ANSWER_SUCCESS								"success"
#define MSTR_API_ANSWER_FAILURE								"failure"

#define MUI_API_KEYWORD_LEN_MAX								19

#define MSTR_API_KEY_WORD_CONFIG							"config"
#define MSTR_API_KEY_WORD_GROUPS							"groups"
#define MSTR_API_KEY_WORD_SCENE								"scenes"
#define MSTR_API_KEY_WORD_SET								"sets"
#define MSTR_API_KEY_WORD_SCHEDULES							"schedules"
#define MSTR_API_KEY_WORD_RULES								"rules"
#define MSTR_API_KEY_WORD_LIGHTS							"lights"
#define MSTR_API_KEY_WORD_OCC_SENSORS						"occupancysensors"
#define MSTR_API_KEY_WORD_BIN_SENSORS						"binarysensors"
#define MSTR_API_KEY_WORD_TEMP_SENSORS						"temperaturesensors"
#define MSTR_API_KEY_WORD_HUM_SENSORS						"humiditysensors"
#define MSTR_API_KEY_WORD_TH_SENSORS						"temphumsensors"
#define MSTR_API_KEY_WORD_BIN_CTRLS							"binarycontrollers"
#define MSTR_API_KEY_WORD_LEVEL_CTRLS						"levelcontrollers"
#define MSTR_API_KEY_WORD_WARN_DEVS							"warningdevices"
#define MSTR_API_KEY_WORD_PLUGS								"plugs"
#define MSTR_API_KEY_WORD_SWITCHS							"switchs"
#define MSTR_API_KEY_WORD_SHADES							"shades"
#define MSTR_API_KEY_WORD_SONOSES							"sonoses"
#define MSTR_API_KEY_WORD_PANELS							"panels"
#define MSTR_API_KEY_WORD_RTC								"rtc"
#define MSTR_API_KEY_WORD_SYS								"sys"
#define MSTR_API_KEY_WORD_DEBUG								"debug"
#define MSTR_API_KEY_WORD_AT_DEBUG							"atdebug"
#define MSTR_API_KEY_WORD_UPDATE							"update"
#define MSTR_API_KEY_WORD_UPGRADE							"upgrade"
#define MSTR_API_KEY_WORD_REMOTE_DL							"remotedownload"
#define MSTR_API_KEY_WORD_TEST_MODE							"testmode"

#define MSTR_API_KEY_WORD_NEW								"new"
#define MSTR_API_KEY_WORD_STATE								"state"
#define MSTR_API_KEY_WORD_ACTION							"action"


#define MSTR_API_URL_KEY_WORD_CONFIG						"/"MSTR_API_KEY_WORD_CONFIG
#define MSTR_API_URL_KEY_WORD_GROUPS						"/"MSTR_API_KEY_WORD_GROUPS
#define MSTR_API_URL_KEY_WORD_SCENE							"/"MSTR_API_KEY_WORD_SCENE
#define MSTR_API_URL_KEY_WORD_SET							"/"MSTR_API_KEY_WORD_SET
#define MSTR_API_URL_KEY_WORD_SCHEDULES						"/"MSTR_API_KEY_WORD_SCHEDULES
#define MSTR_API_URL_KEY_WORD_RULES							"/"MSTR_API_KEY_WORD_RULES
#define MSTR_API_URL_KEY_WORD_LIGHTS						"/"MSTR_API_KEY_WORD_LIGHTS
#define MSTR_API_URL_KEY_WORD_OCC_SENSORS					"/"MSTR_API_KEY_WORD_OCC_SENSORS
#define MSTR_API_URL_KEY_WORD_BIN_SENSORS					"/"MSTR_API_KEY_WORD_BIN_SENSORS
#define MSTR_API_URL_KEY_WORD_TEMP_SENSORS					"/"MSTR_API_KEY_WORD_TEMP_SENSORS
#define MSTR_API_URL_KEY_WORD_HUM_SENSORS					"/"MSTR_API_KEY_WORD_HUM_SENSORS
#define MSTR_API_URL_KEY_WORD_TH_SENSORS					"/"MSTR_API_KEY_WORD_TH_SENSORS
#define MSTR_API_URL_KEY_WORD_BIN_CTRLS						"/"MSTR_API_KEY_WORD_BIN_CTRLS
#define MSTR_API_URL_KEY_WORD_LEVEL_CTRLS					"/"MSTR_API_KEY_WORD_LEVEL_CTRLS
#define MSTR_API_URL_KEY_WORD_WARN_DEVS						"/"MSTR_API_KEY_WORD_WARN_DEVS
#define MSTR_API_URL_KEY_WORD_PLUGS							"/"MSTR_API_KEY_WORD_PLUGS
#define MSTR_API_URL_KEY_WORD_SWITCHS						"/"MSTR_API_KEY_WORD_SWITCHS
#define MSTR_API_URL_KEY_WORD_SHADES						"/"MSTR_API_KEY_WORD_SHADES
#define MSTR_API_URL_KEY_WORD_SONOSES						"/"MSTR_API_KEY_WORD_SONOSES
#define MSTR_API_URL_KEY_WORD_PANELS						"/"MSTR_API_KEY_WORD_PANELS
#define MSTR_API_URL_KEY_WORD_RTC							"/"MSTR_API_KEY_WORD_RTC
#define MSTR_API_URL_KEY_WORD_SYS							"/"MSTR_API_KEY_WORD_SYS
#define MSTR_API_URL_KEY_WORD_DEBUG							"/"MSTR_API_KEY_WORD_DEBUG
#define MSTR_API_URL_KEY_WORD_AT_DEBUG						"/"MSTR_API_KEY_WORD_AT_DEBUG
#define MSTR_API_URL_KEY_WORD_UPDATE						"/"MSTR_API_KEY_WORD_UPDATE
#define MSTR_API_URL_KEY_WORD_UPGRADE						"/"MSTR_API_KEY_WORD_UPGRADE
#define MSTR_API_URL_KEY_WORD_REMOTE_DL						"/"MSTR_API_KEY_WORD_REMOTE_DL
#define MSTR_API_URL_KEY_WORD_TEST_MODE						"/"MSTR_API_KEY_WORD_TEST_MODE

#define MSTR_API_URL_KEY_WORD_NEW							"/"MSTR_API_KEY_WORD_NEW
#define MSTR_API_URL_KEY_WORD_STATE							"/"MSTR_API_KEY_WORD_STATE
#define MSTR_API_URL_KEY_WORD_ACTION						"/"MSTR_API_KEY_WORD_ACTION


#define MSTR_CONFIG_PASSWD_SHOW_MASK						"******"
#define MSTR_ANSWER_UNSUPPORT_DEV							"\"Unsupport device.\""
#define MSTR_ANSWER_GET_DATA_EMPTY							"{}"
#define MSTR_ANSWER_GET_DATA_ERROR							"\"Get data error.\""
#define MSTR_ANSWER_CAN_NOT_CREATE_JSON_OBJ					"\"Can't create JSON object.\""
#define MSTR_ANSWER_JSON_PRINT_ERROR						"\"JSON print error.\""


//一下宏是s_achSetConfigKeyWord中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_CONFIG_KEY_WORD_IDX_NAME					0
#define MUI_SET_CONFIG_KEY_WORD_IDX_IP_ADDR					1
#define MUI_SET_CONFIG_KEY_WORD_IDX_NETMASK					2
#define MUI_SET_CONFIG_KEY_WORD_IDX_GW_IP_ADDR				3
#define MUI_SET_CONFIG_KEY_WORD_IDX_DHCP					4
#define MUI_SET_CONFIG_KEY_WORD_IDX_PROXY_ADDR				5
#define MUI_SET_CONFIG_KEY_WORD_IDX_PROXY_PORT				6
#define MUI_SET_CONFIG_KEY_WORD_IDX_PORTAL_SERVICES			7
#define MUI_SET_CONFIG_KEY_WORD_IDX_DEV_ID					8
#define MUI_SET_CONFIG_KEY_WORD_IDX_PASSWD					9
#define MUI_SET_CONDIG_KEY_WORD_IDX_END						MUI_SET_CONFIG_KEY_WORD_IDX_PASSWD

#define MUI_SET_CONFIG_KEY_WORD_LEN_MAX						15

//一次可以修改的Config关键量的最大个数
#define MUI_SET_CONFIG_ONCE_MODIFY_NUM_MAX					10


//以下宏是s_achSetConfigKeyWord中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_DEV_ATTR_KEY_WORD_IDX_ID					0
#define MUI_SET_DEV_ATTR_KEY_WORD_IDX_NAME					1
#define MUI_SET_DEV_ATTR_KEY_WORD_IDX_DESCRIPTION			2
#define MUI_SET_DEV_ATTR_KEY_WORD_IDX_CHANGE_RPT			3
#define MUI_SET_DEV_ATTR_KEY_WORD_IDX_AUTO_RPT				4
#define MUI_SET_DEV_ATTR_KEY_WORD_IDX_END					MUI_SET_DEV_ATTR_KEY_WORD_IDX_AUTO_RPT

#define MUI_SET_DEV_ATTR_KEY_WORD_LEN_MAX					12
#define MUI_DEV_DESCRIPTIONLEN_MAX							19

//一次可以修改的关键字的最大个数
#define MUI_SET_DEV_ATTR_ONCE_MODIFY_NUM_MAX				4


//以下宏是s_achSetConfigKeyWord中各个关键字在数组中的一维索引,必须确保正确
//#define MUI_SET_DEV_STATE_KEY_WORD_IDX_REACHABLE
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_ON					0
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI					1
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT					2
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE					3
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_CT					4
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_XY					5
//#define MUI_SET_DEV_STATE_KEY_WORD_IDX_COLOR_MODE
//#define MUI_SET_DEV_STATE_KEY_WORD_IDX_ALERT
//#define MUI_SET_DEV_STATE_KEY_WORD_IDX_EFFECT
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL				6
//#define MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP					7
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM					8
//#define MUI_SET_DEV_STATE_KEY_WORD_IDX_CONSUMPTION
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE			9
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION				10
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG				11
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION				12
//#define MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY					13
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME				14
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID				15
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE				16
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE				17
//-------------------------------------------
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_ONSEC				18
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_OFFSEC				19
#define MUI_SET_DEV_STATE_KEY_WORD_IDX_END					MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE

#define MUI_SET_DEV_STATE_KEY_WORD_LEN_MAX					9

//一次可以修改的关键字的最大个数
#define MUI_SET_DEV_STATE_ONCE_MODIFY_NUM_MAX				3	//这个值是根据各种sensor可被设定的状态数量而定的


//以下宏是s_aachSetGroupKeyWordList中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_GROUP_ATTR_KEY_WORD_IDX_NAME				0
#define MUI_SET_GROUP_ATTR_KEY_WORD_IDX_DESCRIPTION			1
#define MUI_SET_GROUP_ATTR_KEY_WORD_IDX_CONFGI				2
#define MUI_SET_GROUP_ATTR_KEY_WORD_IDX_END					MUI_SET_GROUP_ATTR_KEY_WORD_IDX_DESCRIPTION +1

#define MUI_SET_GROUP_KEY_WORD_TYPE_MAX						2
#define MUI_SET_GROUP_KEY_WORD_LEN_MAX						11

//一次可以修改的关键字的最大个数
#define MUI_SET_GROUP_ATTR_ONCE_MODIFY_NUM_MAX				3

#define MUI_SET_GROUP_ATTR_KEY_WORD_IDX_ADDRESS				0


//以下宏是s_aachSetTypeMapList中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_SET_ATTR_KEY_WORD_IDX_NAME					0
#define MUI_SET_SET_ATTR_KEY_WORD_IDX_DESCRIPTION			1
#define MUI_SET_SET_ATTR_KEY_WORD_IDX_DELAY					2
#define MUI_SET_SET_ATTR_KEY_WORD_IDX_CONFIG				3
#define MUI_SET_SET_ATTR_KEY_WORD_IDX_ADDRESS				4
#define MUI_SET_SET_ATTR_KEY_WORD_IDX_ACTION				5
#define MUI_SET_SET_ATTR_KEY_WORD_IDX_EXECUTE				6
#define MUI_SET_SET_STATE_KEY_WORD_IDX_END				MUI_SET_SET_ATTR_KEY_WORD_IDX_DESCRIPTION

//一次可以修改的关键字的最大个数
#define MUI_SET_SET_ATTR_ONCE_MODIFY_NUM_MAX				4

#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_GROUP			0
#define MUI_SET_SCENE_ATTR_ONCE_MODIFY_NUM_MAX				4
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_SET			1
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_SCHED			2
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_RULE			3
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_LIGHT			4
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_BIN_CTRL		5
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_LEVEL_CTRL		6
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_WARN_DEV		7
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_PLUG			8
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_SWITCH			9
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_SHADE			10
#define MUI_SET_SET_ACTION_TYPE_KEY_WORD_IDX_SONOS			11


//以下宏是s_aachScheduleTypeMapList中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_NAME				0
#define MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_DESCRIPTION		1
#define MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_TIME				2
#define MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_REPEAT			3
#define MUI_SET_SCHEDULE_ATTR_KEY_WORD_IDX_CONFIG			4

//以下宏是s_aachScheduleOutputStateMapList中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_ADDRESS		0
#define MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_DELAY			1
#define MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_DURATION		2
#define MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_ACTION			3
#define MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_QUITMETHOD		4
#define MUI_SET_SCHEDULE_OUTPUT_KEY_WORD_IDX_QUITSTATE		5

//一次可以修改的schedule关键量的最大个数
#define MUI_SET_SCHEDULE_ONCE_MODIFY_NUM_MAX				6


#define MUI_CHANGE_REPORT_DEV_DATA_MODE_ATTR				0x01
#define MUI_CHANGE_REPORT_DEV_DATA_MODE_STATE				0x02


//以下宏是s_aachRulesTypeMapList中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_NAME				0
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_DESCRIPTION			1
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_ACTIVE				2
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_TIME				3
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_REPEAT				4
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_DURATION			5
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_CONFIG				6
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_INPUT				7
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_EXPRESSION			8
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_TRUE_OUTPUT			9
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_FALSE_OUTPUT		10
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_ADDRESS				11
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_STATE				12
#define MUI_SET_RULES_ATTR_KEY_WORD_IDX_DELAY				13

//以下宏是s_aachSceneAttrTypeMapList中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_SCENE_ATTR_KEY_WORD_IDX_NAME				0
#define MUI_SET_SCENE_ATTR_KEY_WORD_IDX_DESCRIPTION			1
#define MUI_SET_SCENE_ATTR_KEY_WORD_IDX_CONFIG				2
#define MUI_SET_SCENE_ATTR_KEY_WORD_IDX_EXECUTE				3
#define MUI_SET_SCENE_STATE_KEY_WORD_IDX_END				MUI_SET_SCENE_ATTR_KEY_WORD_IDX_EXECUTE

//以下宏是s_aachSceneConfigTypeMapList中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_SCENE_CONFIG_KEY_WORD_IDX_ADDRESS			0
#define MUI_SET_SCENE_CONFIG_KEY_WORD_IDX_ACTION			1
#define MUI_SET_SCENE_CONFIG_KEY_WORD_IDX_END				MUI_SET_SCENE_CONFIG_KEY_WORD_IDX_ACTION

//以下宏是s_achSceneActionKeyWord中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ON					0
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_BRI				1
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_SAT				2
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUE				3
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_CT					4
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_XY					5
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_LEVEL				6
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_TEMP				7
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_HUM				8
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_MODE			9
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_DURATION			10
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_WARN_CFG			11
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ACTION				12
#define MUI_SCENE_DEV_STATE_KEY_WORD_IDX_END				MUI_SCENE_DEV_STATE_KEY_WORD_IDX_ACTION

//一次可以修改的关键字的最大个数





//以下宏是s_aachRulesInputStateMapList中各个关键字在数组中的一维索引,必须确保正确
#define	MUI_RULES_ACTION_STATE_WORD_IDX_ON					0
#define	MUI_RULES_ACTION_STATE_WORD_IDX_TEMP				1
#define	MUI_RULES_ACTION_STATE_WORD_IDX_HUM					2
#define	MUI_RULES_ACTION_STATE_WORD_IDX_LESS				3
#define	MUI_RULES_ACTION_STATE_WORD_IDX_EDGE				4

//以下宏是s_aachRulesOutputStateMapList中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_ADDRESS			0
#define MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DELAY				1
#define MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DELAYOVERLAY		2
#define MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DURATION			3
#define MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_DURATIONOVERLAY	4
#define MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_ACTION			5
#define MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_QUITMETHOD		6
#define MUI_SET_RULES_OUTPUT_KEY_WORD_IDX_QUITSTATE			7

#define MUI_RULES_OUTPUT_TYPE_TRUE							0
#define MUI_RULES_OUTPUT_TYPE_FALSE							1

//一次可以修改的Rules关键量的最大个数
#define MUI_SET_RULES_ONCE_MODIFY_NUM_MAX					6


//#define MUI_SET_SYS_KEY_WORD_LEN_MAX						15
//以下宏是s_achSetSysKeyWord中各个关键字在数组中的一维索引,必须确保正确
#define MUI_SET_SYS_KEY_WORD_IDX_SN							0
#define MUI_SET_SYS_KEY_WORD_IDX_MAC						1
#define MUI_SET_SYS_KEY_WORD_IDX_WIFI_MAC					2
#define MUI_SET_SYS_KEY_WORD_IDX_UZIPSN						3
#define MUI_SET_SYS_KEY_WORD_IDX_UZIPMAC					4
#define MUI_SET_SYS_KEY_WORD_IDX_END						MUI_SET_SYS_KEY_WORD_IDX_UZIPMAC

//一次可以修改的关键字的最大个数
#define MUI_SET_SYS_ONCE_MODIFY_NUM_MAX						5


#define MUI_API_SYS_TIME_RTC_BUF_LEN						20
#define MUI_API_REMOTE_DOWNLOAD_LINK						512
#define MUI_API_REMOTE_DOWNLOAD_USERNAME					32
#define MUI_API_REMOTE_DOWNLOAD_PASSWORD					32
#define MUI_API_UPGRADE_DEST								50
#define MUI_API_UPGRADE_FILE								50
#define MUI_API_UPGRADE_ADDRESS								50
//#define MUI_API_AT_DEBUG_COMMAND_MAX						62      //uzip 发送最大命令 64, 但调试命令还要附带 '\r\n' 2byte, 所以最大支持调试邋AT命令在62
//#define MUI_API_AT_DEBUG_ANSWER_MAX						256
//#define MUI_API_AT_DEBUG_ANSWER_TIMEOUT					10000


//在获取设备数据JSON包时,
#define MUI_API_DEV_DATA_ASM_JSON_CFG_STATE					0x01
#define MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR				0x02
#define MUI_API_DEV_DATA_ASM_JSON_CFG_ZB_ATTR				0x04
#define MUI_API_DEV_DATA_ASM_JSON_CFG_EXT_ATTR				0x08

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	char				name[MUI_USER_NAME_LEN+1];			//加1是为了存储可能多出的'\0'
	char				dev_name[MUI_USER_DEV_NAME_LEN+1];	//加1是为了存储可能多出的'\0'
} S_ApiParseCreateUser;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseGetFullState;

typedef struct
{
	uint16_t			ui16Duration;
} S_ApiParseSearchNewDev;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseFactoryNew;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseGetConfig;

typedef struct
{
	S_CfgName			name;
	S_StrIpAddr			ipaddr;
	S_StrIpAddr			netmask;
	S_StrIpAddr			gwipaddr;
	uint8_t				dhcp;
	S_CfgProxyAddr		proxyaddr;
	uint16_t			proxyport;
	uint8_t				portalservices;
	S_StrDevId			devid;
	S_StrPasswd			passwd;
} S_SetConfigInfo;
typedef struct
{
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_CONFIG_ONCE_MODIFY_NUM_MAX];
	S_SetConfigInfo		sSetConfigInfo;
} S_ApiParseSetConfig;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseGetAllDevsByType;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseGetDevAttrState;

typedef struct
{
	uint16_t			ui16Id;
	S_AttrName			name;
	S_AttrDescr			descr;
	uint8_t				changerpt;
	uint16_t			autorpt;
} S_SetDevAttr;
typedef struct
{
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_DEV_ATTR_ONCE_MODIFY_NUM_MAX];
	S_SetDevAttr		sSetDevAttr;
} S_ApiParseSetDevAttr;

typedef struct
{
	//uint8_t			reachable;
	uint8_t				on;
	uint8_t				bri;
	uint8_t				sat;
	uint16_t			hue;
	uint16_t			ct;
	S_Color_XY			xy;
	//uint8_t			colormode;
	//uint8_t			alert;
	//uint8_t			effect;
	uint16_t			level;
	//uint16_t			count;
	uint16_t			temp;
	uint16_t			hum;
	//uint16_t			consumption;
	uint8_t				warnmode;
	uint16_t			duration;
	S_WarnCfg			warncfg;
	uint8_t				action;
	//uint8_t			power;
	uint8_t				play;
	uint16_t			volume;
	uint32_t			urlid;
} S_SetDevState;
typedef struct
{
	uint16_t			ui16DevType;
	uint16_t			ui16DevId;
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_DEV_STATE_ONCE_MODIFY_NUM_MAX];
	S_SetDevState		sSetDevState;
} S_ApiParseSetDevState;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseDelDev;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseGetAllGroups;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseGetGroupAttr;







typedef struct
{
	S_AttrName			name;
	S_AttrDescr			descr;
	S_GroupOpAttr		opAttr;
} S_SetGroupAttr;
typedef struct
{
	uint8_t 			tCnt;
	uint8_t 			atKeywordMapIdxList[MUI_SET_GROUP_ATTR_ONCE_MODIFY_NUM_MAX];
	S_SetGroupAttr		sSetGroupAttr;
} S_ApiParseSetGroup;

typedef struct
{
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_DEV_STATE_ONCE_MODIFY_NUM_MAX];
	S_SetDevState		sSetDevState;
} S_SetGroupState;

typedef struct
{
	S_SetAttrFlash		setattr;
	char 				setmember[1024];
} S_SetAttr;

typedef struct
{
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_SET_ATTR_ONCE_MODIFY_NUM_MAX];
	uint8_t				execute;
	S_SetAttrFlash		setattr;
} S_ApiParseSetSet;

typedef struct
{
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_SCENE_ATTR_ONCE_MODIFY_NUM_MAX];
	uint8_t				execute;
	S_SceneDataFalsh    scene_data;
} S_ApiParseSetScene;


typedef struct
{
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_SCHEDULE_ONCE_MODIFY_NUM_MAX];
	S_ScheduleAttr		ScheduleAttr;
} S_ApiParseSetschedule;

typedef struct
{
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_RULES_ONCE_MODIFY_NUM_MAX];
	S_RulesAttr			RulesAttr;
} S_ApiParseSetRules;

typedef struct
{
	char achTimeBuf[MUI_API_SYS_TIME_RTC_BUF_LEN+1];
} S_ApiParseTime;

typedef struct
{
	uint8_t				ui8Tmp;
} S_ApiParseGetSys;

typedef struct
{
	S_FactorySn			sSn;
	S_StrMacAddr		sMac;
	S_StrMacAddr		sWifiMac;
	S_CfgUzipSn			sUzipSn;
	S_CfgUzipMac		sUzipMac;
} S_SysInfo;
typedef struct
{
	uint8_t				tCnt;
	uint8_t				atKeywordMapIdxList[MUI_SET_SYS_ONCE_MODIFY_NUM_MAX];
	S_SysInfo			sSysInfo;
} S_ApiParseSetSys;

typedef struct
{
	uint32_t			ui32mask;
	uint8_t				ui8level;
} S_ApiParsedebug;

typedef struct
{
	char				achAnswer[MUI_AT_PORT_ANSWER_LEN_MAX];
	char  				achCmd[MUI_AT_PORT_CMD_LEN_MAX-2];	//网络传下的AT命令尾部不带\r\n,因此必须要减2字节
} S_ApiParseAtdebug;

typedef struct
{
	char				achDest[MUI_API_UPGRADE_DEST];
	char 				achFile[MUI_API_UPGRADE_FILE];
	char 				achAddress[MUI_API_UPGRADE_ADDRESS];
} S_ApiParseUpgrade;

typedef struct
{
	char				achLink[MUI_API_REMOTE_DOWNLOAD_LINK];
	char 				achUserName[MUI_API_REMOTE_DOWNLOAD_USERNAME];
	char				achPassWord[MUI_API_REMOTE_DOWNLOAD_PASSWORD];
} S_ApiParseRemoteDownload;

typedef struct
{
	uint8_t				ui8Mode;
} S_ApiParseSetTestMode;

typedef struct
{
	sem_t				tMutex;
	size_t				tLen;
	uint8_t				aui8Buf[MUI_API_ANSWER_BUF_SIZE];	/* 64k的发送缓冲区*/
} S_ApiAnswerBufCtrl;

//该结构体必须配合S_ConntCtrl指针使用
typedef struct
{
	uint8_t				ui8GradeNum;	//URL关键字等级数
	uint16_t			aui16Offset[MUI_API_URL_GRADE_MAX];
} S_ApiUrlParse;

typedef struct
{
	S_ConntCtrl			*psConntCtrl;
	const cJSON			*psBody;
	S_ApiUrlParse		sUrlParse;
	uint32_t			ui32Authority;
} S_ApiDealCtrl;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern const char* api_http_method_string_get (uint8_t ui8Idx);
extern int api_answer_send (S_ConntCtrl *psConntCtrl, const void *pvBuf, int tLen);
extern int api_answer_segment_send (S_ConntCtrl *psConntCtrl, const void *pvBuf, size_t tLen, uint8_t ui8Flg);
extern int api_parse (S_ConntCtrl *psConntCtrl, const cJSON *psBody);
extern int api_deal_answer_head_send (S_ConntCtrl *psConntCtrl);
extern int remote_api_deal_answer_head_send (S_ConntCtrl *psConntCtrl, uint32_t ui32DstId, uint32_t ui32Sqn);
extern int api_deal (S_ConntCtrl *psConntCtrl);
extern int remote_api_deal (const void *pvRemoteMsg, size_t tLen);
extern int api_deal_init (void);

#endif /* __M_FILE_NAME_api_deal_H__ */
