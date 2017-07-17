#ifndef _RULE_DATA_H_
#define _RULE_DATA_H_

#include "rule_comm.h"
#include "dev_if.h"


//--------------------------------------------------------------------
#define MUI_RULES_TIME_CHECK_FALSE							0
#define MUI_RULES_TIME_CHECK_TRUE							1

typedef struct
{
	uint16_t		temp;
	uint8_t			less;
} S_RuleTempInput;

typedef struct
{
	uint16_t		humidity;
	uint8_t			less;
} S_RuleHumidityInput;

typedef struct
{
	uint8_t			onsec;
	uint8_t			offsec;
} S_RuleBlinkOutput;


typedef union
{
	uint8_t			on;
	uint8_t			bri;
	uint8_t			sat;
	uint16_t		hue;
	uint16_t		ct;
	S_Color_XY		xy;
    uint16_t 		level;
	uint8_t			warnmode;
	uint16_t		duration;
	S_WarnCfg		warncfg;
	uint8_t			action;
	uint8_t			play;
	uint16_t		volume;
	uint32_t		urlid;
	uint8_t			execute;
	uint8_t			active;
//----------------------------------------------
	uint8_t			edge;		//1为up，2为down, 3为all
	S_RuleTempInput			temp;
	S_RuleHumidityInput		hum;
	S_RuleBlinkOutput		blink;
} U_RulesAction;



typedef struct
{
	S_AttrName			name;
	S_AttrDescr			descr;
	uint8_t 			active;
	uint8_t				repeat;
	uint32_t			duration;
	S_ModeTime 		time;
} S_RulesAttr;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	uint8_t				ActionType;
	U_RulesAction		ActionValue;
} S_RulesInPutState;

typedef struct
{
	uint8_t				ActionType;
	U_RulesAction		ActionValue;
} S_RuelsOutPutAction;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	uint16_t			delay;
	uint8_t				delayoverlay;
	uint16_t			duration;
	uint8_t				durationoverlay;
	S_RuelsOutPutAction	action;
	uint8_t				quitmethod;
	S_RuelsOutPutAction	quitstate;
} S_RuelsOutPutState;

typedef struct
{
	uint8_t				InputNum;
	S_RulesInPutState	InputAction[MUI_INPUT_NUM_MAX_PER_RULE];
	char				expersion[MUI_RULE_LOGICAL_EXP_SIZE_MAX];
	uint8_t				ToutputNum;
	S_RuelsOutPutState	ToutputAction[MUI_T_OUTPUT_NUM_MAX_PER_RULE];
	uint8_t				FoutputNum;
	S_RuelsOutPutState	FoutputAction[MUI_F_OUTPUT_NUM_MAX_PER_RULE];
} S_RulesConfig;

typedef struct
{
	S_RulesAttr			RulesAttr;
	S_RulesConfig		RulesConfig;
} S_ApiParseRules;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	uint8_t				ActionType;
	U_RulesAction		ActionValue;
	U_RulesAction		ActionValueOld;
} S_RulesInPutRamState;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	uint16_t			delay;
	uint8_t				delayoverlay;
	uint16_t			duration;
	uint8_t				durationoverlay;
	S_RuelsOutPutAction	action;
	uint8_t				quitmethod;
	S_RuelsOutPutAction	quitstate;
	uint16_t			delay_count;
	uint16_t			duration_count;
} S_RuelsOutPutRamState;

typedef struct _S_Rules_Ram_data
{
	uint8_t					execute;
	uint8_t					Toutstate;
	uint8_t					ToutFinishNum;
	uint8_t					Foutstate;       //为1时，表示input条件判断完成，为2时，表示执行函数已经接收1的指令，0为不执行
	uint8_t					FoutFinishNum;
	uint8_t 				active;
	uint8_t					repeat;
	uint32_t				duration;
	uint32_t				duration_count;
	S_ModeTime 			time;
	uint8_t					InPutNum;
	uint8_t					ToutPutNum;
	uint8_t					FoutPutNum;
	S_RulesInPutRamState	InPut[MUI_INPUT_NUM_MAX_PER_RULE];
	char					expersion[MUI_RULE_LOGICAL_EXP_SIZE_MAX];
	S_RuelsOutPutRamState	TrueOutPut[MUI_T_OUTPUT_NUM_MAX_PER_RULE];
	S_RuelsOutPutRamState	FalseOutPut[MUI_F_OUTPUT_NUM_MAX_PER_RULE];
} S_Rules_Ram_data;


typedef struct
{
	uint8_t			RulesNum;
	uint16_t		IdList[MUI_RULE_DATA_NUM_MAX];
	S_Rules_Ram_data G_RulesRamData[MUI_RULE_DATA_NUM_MAX];
} S_Rules_Ram;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	S_RuelsOutPutAction	action;
} S_RuelsOutPutAttr;

typedef struct
{
	uint8_t			RulesOutPutNum;
	S_RuelsOutPutAttr  sRuelsOutPutAttr[(MUI_T_OUTPUT_NUM_MAX_PER_RULE+MUI_F_OUTPUT_NUM_MAX_PER_RULE)*MUI_RULE_DATA_NUM_MAX];
} S_RuelsOutPutQuene;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	U_RulesAction		ActionValue;
} S_RulesInputDevState;

typedef struct
{
	uint8_t			RulesInputNum;
	S_RulesInputDevState  sRulesInputDevState[MUI_RULE_DATA_NUM_MAX*MUI_INPUT_NUM_MAX_PER_RULE];
} S_RulesInputStateQuene;

#define MUI_RULES_TIME_LEN_MAX								20

extern int rules_get_id_list(uint16_t *pvIdList, uint16_t ui16Capacity);
extern int rules_add(const S_ApiParseRules *pvRules);
extern int rules_del(uint16_t ui16Id);
extern int rules_read_info(uint16_t ui16Id, S_ApiParseRules *pvRules);
extern int rules_read_name(uint16_t ui16Id, S_AttrName *pvName);
extern int rules_write_name(uint16_t ui16Id, S_AttrName *pvName);
extern int rules_write_description(uint16_t ui16Id, S_AttrDescr *pvDesc);
extern int rules_write_active(uint16_t ui16Id, uint8_t *pvActive);
extern int rules_write_repeat(uint16_t ui16Id, uint8_t *pvRepeat);
extern int rules_write_duration(uint16_t ui16Id, uint32_t *pvDuration);
extern int rules_write_time(uint16_t ui16Id, S_ModeTime *pvTime);
extern int rules_init();

#endif
