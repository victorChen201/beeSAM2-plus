#include "rule.h"
#include "api_deal.h"
#include "group.h"
#include "schedule.h"
#include "extend_data.h"
#include "rule_data.h"
#include "dev_if.h"
#include "set.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define RULES_DBG

#ifndef RULES_PUTC
#ifdef RULES_DBG
#define RULES_PUTC(Char)			PUTC(Char)
#else
#define RULES_PUTC(Char)
#endif
#endif

#ifndef RULES_PUTS
#ifdef RULES_DBG
#define RULES_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define RULES_PUTS(Str, Len)
#endif
#endif

#ifndef RULES_PUTASCC
#ifdef RULES_DBG
#define RULES_PUTASCC(Char)			PUTASCC(Char)
#else
#define RULES_PUTASCC(Char)
#endif
#endif

#ifndef RULES_PUTASCS
#ifdef RULES_DBG
#define RULES_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define RULES_PUTASCS(Str, Len)
#endif
#endif

#ifndef RULES_PRINT
#ifdef RULES_DBG
#define RULES_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define RULES_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef RULES_DYN_PUTC
#ifdef RULES_DBG
#define RULES_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_RULE|Dbg), Char)
#else
#define RULES_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef RULES_DYN_PUTS
#ifdef RULES_DBG
#define RULES_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_RULE|Dbg), Str, Len)
#else
#define RULES_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef RULES_DYN_PUTASCC
#ifdef RULES_DBG
#define RULES_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_RULE|Dbg), Char)
#else
#define RULES_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef RULES_DYN_PUTASCS
#ifdef RULES_DBG
#define RULES_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_RULE|Dbg), Str, Len)
#else
#define RULES_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef RULES_DYN_PRINT
#ifdef RULES_DBG
#define RULES_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_RULE|Dbg), Fmt, ##Arg)
#else
#define RULES_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

S_Rules_Ram 	G_Rules_RAM;
S_RuelsOutPutQuene	G_RulesOutPutQuene;
S_RulesInputStateQuene G_RulesInputStateQuene;
static pthread_t s_Rules_DealThread; /* 线程ID*/
static sem_t s_tRulesRamDataSem;


/*static void* s_rules_execute_deal_task (void *pArg);
static int rules_ram_data_add(uint16_t ui16Id, const S_ApiParseRules *pvRules);
static int rules_ram_data_del();
static int rules_ram_data_active_change(uint16_t ui16Id, uint8_t ui8Active);
static int rules_ram_data_repeat_change(uint16_t ui16Id, uint8_t ui8Repeat);
static int rules_ram_data_duration_change(uint16_t ui16Id, uint32_t ui8Dur);
static int rules_ram_data_time_change(uint16_t ui16Id, S_ModeTime *psRTime);
*/
static int rules_ram_data_add(uint16_t ui16Id, const S_ApiParseRules *pvRules);
static int rules_ram_data_del();
static int rules_ram_data_active_change(uint16_t ui16Id, uint8_t *ui8Active);
static int rules_ram_data_repeat_change(uint16_t ui16Id, uint8_t *ui8Repeat);
static int rules_ram_data_duration_change(uint16_t ui16Id, uint32_t *ui8Dur);
static int rules_ram_data_time_change(uint16_t ui16Id, S_ModeTime *psRTime);
static int rules_RamData_init();
static int rules_execute_check(S_Rules_Ram_data *psRulesRamData);
static int rules_set_rollback_quitstate(S_RuelsOutPutRamState *psOutput);
static int rules_get_input_dev_state(S_Rules_Ram_data  *psRulesData, char *psLogic);
static void rules_output_quene_add(S_RuelsOutPutRamState *psRuelsState, uint8_t ui8t);
static int ruels_output_active ();
static void* s_rules_execute_deal_task (void *pArg);


int rules_get_id_list(uint16_t *pvIdList, uint16_t ui16Capacity)
{
	return rule_data_get_id_list(pvIdList, ui16Capacity);
}

static int rules_ram_data_add(uint16_t ui16Id, const S_ApiParseRules *pvRules)
{
	uint16_t buf_id[MUI_RULE_DATA_NUM_MAX] = {0};
	int rules_num;
	int j;
	if (0 <= sem_wait_ms(&s_tRulesRamDataSem, 10000))
	{
		G_Rules_RAM.RulesNum++;

		rules_num = rules_get_id_list(buf_id, sizeof(buf_id)/sizeof(buf_id[0]));
		if (rules_num < 0)
		{
			sem_post(&s_tRulesRamDataSem);
			return rules_num;
		}
		if (rules_num != G_Rules_RAM.RulesNum)
		{
			sem_post(&s_tRulesRamDataSem);
			return -1;
		}
		memcpy(G_Rules_RAM.IdList, buf_id, sizeof(buf_id)/sizeof(buf_id[0]));

		G_Rules_RAM.G_RulesRamData[ui16Id].execute = 0;
		G_Rules_RAM.G_RulesRamData[ui16Id].duration_count = 0;
		G_Rules_RAM.G_RulesRamData[ui16Id].active = pvRules->RulesAttr.active;
		G_Rules_RAM.G_RulesRamData[ui16Id].repeat = pvRules->RulesAttr.repeat;
		G_Rules_RAM.G_RulesRamData[ui16Id].duration = pvRules->RulesAttr.duration;
		G_Rules_RAM.G_RulesRamData[ui16Id].InPutNum = pvRules->RulesConfig.InputNum;
		G_Rules_RAM.G_RulesRamData[ui16Id].ToutPutNum = pvRules->RulesConfig.ToutputNum;
		G_Rules_RAM.G_RulesRamData[ui16Id].FoutPutNum = pvRules->RulesConfig.FoutputNum;
		G_Rules_RAM.G_RulesRamData[ui16Id].Toutstate = 0;
		G_Rules_RAM.G_RulesRamData[ui16Id].Foutstate = 0;

		memcpy(&G_Rules_RAM.G_RulesRamData[ui16Id].time, &pvRules->RulesAttr.time, sizeof(S_ModeTime));
		memcpy(G_Rules_RAM.G_RulesRamData[ui16Id].expersion, pvRules->RulesConfig.expersion, MUI_RULE_LOGICAL_EXP_SIZE_MAX);

		for (j=0; j<pvRules->RulesConfig.InputNum; j++)
		{
			memcpy(&G_Rules_RAM.G_RulesRamData[ui16Id].InPut[j], &pvRules->RulesConfig.InputAction[j], sizeof(S_RulesInPutState));
		}
		for (j=0; j<pvRules->RulesConfig.ToutputNum; j++)
		{
			memcpy(&G_Rules_RAM.G_RulesRamData[ui16Id].TrueOutPut[j], &pvRules->RulesConfig.ToutputAction[j], sizeof(S_RuelsOutPutState));
		}
		for (j=0; j<pvRules->RulesConfig.FoutputNum; j++)
		{
			memcpy(&G_Rules_RAM.G_RulesRamData[ui16Id].FalseOutPut[j], &pvRules->RulesConfig.FoutputAction[j], sizeof(S_RuelsOutPutState));
		}
	}
	sem_post(&s_tRulesRamDataSem);
	return rules_num;
}

static int rules_ram_data_del()
{
	uint16_t buf_id[MUI_RULE_DATA_NUM_MAX] = {0};
	int rules_num;
	int iRet=1;
	if (0 <= sem_wait_ms(&s_tRulesRamDataSem, 10000))
	{
		RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_ram_data_del] ++++++++++++++++++++++++++++RulesNum=%d++++++++++++++++++++++\r\n", G_Rules_RAM.RulesNum);
		G_Rules_RAM.RulesNum--;
		RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_ram_data_del] ++++++++++++++++++++++++++++RulesNum=%d++++++++++++++++++++++\r\n", G_Rules_RAM.RulesNum);

		rules_num = rules_get_id_list(buf_id, sizeof(buf_id)/sizeof(buf_id[0]));
		if (rules_num < 0)
		{
			if (rules_num == FILE_S_ERR_DIR_NOT_EXIST || rules_num == FILE_S_ADDR_ERR)
			{
				RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_ram_data_del] Call 'rules_get_id_list' fail :%d,will set 0\r\n", rules_num);
				rules_num= 0;
			}else
			{
				RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_ram_data_del] Call 'rules_get_id_list' fail :%d\r\n", rules_num);
				sem_post(&s_tRulesRamDataSem);
				return rules_num;
			}
		}
		RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_ram_data_del] ++++++++++++++++++++++++++++rules_num=%d++++++++++++++++++++++\r\n", rules_num);
		if (rules_num != G_Rules_RAM.RulesNum)
		{
			sem_post(&s_tRulesRamDataSem);
			return -1;
		}
		memcpy(G_Rules_RAM.IdList, buf_id, sizeof(buf_id)/sizeof(buf_id[0]));
	}else
	{
		RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_ram_data_del] Call 'sem_wait_ms' fail!\r\n");
		iRet = -1;
	}
	sem_post(&s_tRulesRamDataSem);
	return iRet;
}

static int rules_ram_data_active_change(uint16_t ui16Id, uint8_t *ui8Active)
{
	if (0 <= sem_wait_ms(&s_tRulesRamDataSem, 10000))
	{
		G_Rules_RAM.G_RulesRamData[ui16Id].active = *ui8Active;
	}
	sem_post(&s_tRulesRamDataSem);
	return 1;
}

static int rules_ram_data_repeat_change(uint16_t ui16Id, uint8_t *ui8Repeat)
{
	if (0 <= sem_wait_ms(&s_tRulesRamDataSem, 10000))
	{
		G_Rules_RAM.G_RulesRamData[ui16Id].repeat = *ui8Repeat;
	}
	sem_post(&s_tRulesRamDataSem);
	return 1;
}

static int rules_ram_data_duration_change(uint16_t ui16Id, uint32_t *ui8Dur)
{
	if (0 <= sem_wait_ms(&s_tRulesRamDataSem, 10000))
	{
		G_Rules_RAM.G_RulesRamData[ui16Id].duration = *ui8Dur;
	}
	sem_post(&s_tRulesRamDataSem);
	return 1;
}

static int rules_ram_data_time_change(uint16_t ui16Id, S_ModeTime *psRTime)
{
	if (0 <= sem_wait_ms(&s_tRulesRamDataSem, 10000))
	{
		memcpy(&G_Rules_RAM.G_RulesRamData[ui16Id].time, psRTime, sizeof(S_ModeTime));
	}
	sem_post(&s_tRulesRamDataSem);
	return 1;
}


int rules_add(const S_ApiParseRules *pvRules)
{
	int iRet=0;
	uint16_t	ui16Id;

	ASSERT(pvRules != NULL);
	iRet = rule_data_add(pvRules, sizeof(S_ApiParseRules));;
	if (iRet < 0)
	{
		return iRet;
	}
	ui16Id = iRet;
	iRet = rules_ram_data_add(iRet, pvRules);
	if (iRet < 0)
	{
		return iRet;
	}
	return ui16Id;
}

int rules_del(uint16_t ui16Id)
{
	int iRet=0;
	iRet = rule_data_del(ui16Id);
	if (iRet < 0)
	{
		RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_del] rule_data_del fail.:%d\r\n", iRet);
		return iRet;
	}
	return rules_ram_data_del();
}

int rules_read_info(uint16_t ui16Id, S_ApiParseRules *pvRules)
{
	return rule_data_read_info(ui16Id, 0, (void*)pvRules, sizeof(S_ApiParseRules));
}

int rules_read_name(uint16_t ui16Id, S_AttrName *pvName)
{
	return rule_data_read_info(ui16Id, 0, (void*)pvName, sizeof(S_AttrName));
}

int rules_read_active(uint16_t ui16Id, uint8_t *pvActive)
{
	return rule_data_read_info(ui16Id, sizeof(S_AttrName)+sizeof(S_AttrDescr), (void*)pvActive, sizeof(uint8_t));
}

int rules_write_name(uint16_t ui16Id, S_AttrName *pvName)
{
	return rule_data_write_info(ui16Id, 0, (void*)pvName, sizeof(S_AttrName));
}

int rules_write_description(uint16_t ui16Id, S_AttrDescr *pvDesc)
{
	return rule_data_write_info(ui16Id, sizeof(S_AttrName), (void*)pvDesc, sizeof(S_AttrDescr));
}

int rules_write_active(uint16_t ui16Id, uint8_t *pvActive)
{
	int iRet = 0;
	iRet =  rule_data_write_info(ui16Id, sizeof(S_AttrName)+sizeof(S_AttrDescr), (void*)pvActive, sizeof(uint8_t));
	if (iRet < 0)
	{
		return iRet;
	}
	return rules_ram_data_active_change(ui16Id, pvActive);
}

int rules_write_repeat(uint16_t ui16Id, uint8_t *pvRepeat)
{
	int iRet = 0;
	iRet = rule_data_write_info(ui16Id, sizeof(S_AttrName)+sizeof(S_AttrDescr)+sizeof(uint8_t), (void*)pvRepeat, sizeof(uint8_t));
	if (iRet < 0)
	{
		return iRet;
	}
	return rules_ram_data_repeat_change(ui16Id, pvRepeat);
}

int rules_write_duration(uint16_t ui16Id, uint32_t *pvDuration)
{
	int iRet = 0;
	iRet = rule_data_write_info(ui16Id, sizeof(S_AttrName)+sizeof(S_AttrDescr)+sizeof(uint8_t)+sizeof(uint8_t), (void*)pvDuration, sizeof(uint32_t));
	if (iRet < 0)
	{
		return iRet;
	}
	return rules_ram_data_duration_change(ui16Id, pvDuration);
}

int rules_write_time(uint16_t ui16Id, S_ModeTime *pvTime)
{
	int iRet = 0;
	iRet = rule_data_write_info(ui16Id, sizeof(S_AttrName)+sizeof(S_AttrDescr)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint32_t), (void*)pvTime, sizeof(S_ModeTime));
	if (iRet < 0)
	{
		return iRet;
	}
	return rules_ram_data_time_change(ui16Id, pvTime);
}



static int rules_RamData_init()
{
	int iRet = 0, i, j;
	int rules_num = 0;
	uint16_t buf_id[MUI_RULE_DATA_NUM_MAX] = {0};
	S_ApiParseRules  RulesFlashData;

	rules_num = rules_get_id_list(buf_id, sizeof(buf_id)/sizeof(buf_id[0]));
	if (rules_num < 0)
	{
		RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_RamData_init] rules_get_id_list fail.:%d\r\n", rules_num);
		return rules_num;
	}
	G_Rules_RAM.RulesNum = rules_num;
	memcpy(G_Rules_RAM.IdList, buf_id, sizeof(buf_id)/sizeof(buf_id[0]));

	for (i=0; i< rules_num; i++)
	{
		iRet = rules_read_info(buf_id[i], &RulesFlashData);
		if (iRet < 0)
		{
			RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_RamData_init] rules_read_info fail.:%d\r\n", iRet);
			continue;
		}
		G_Rules_RAM.G_RulesRamData[buf_id[i]].execute = 0;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].duration_count = 0;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].active = RulesFlashData.RulesAttr.active;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].repeat = RulesFlashData.RulesAttr.repeat;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].duration = RulesFlashData.RulesAttr.duration;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].InPutNum = RulesFlashData.RulesConfig.InputNum;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].ToutPutNum = RulesFlashData.RulesConfig.ToutputNum;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].FoutPutNum = RulesFlashData.RulesConfig.FoutputNum;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].Toutstate = 0;
		G_Rules_RAM.G_RulesRamData[buf_id[i]].Foutstate = 0;

		memcpy(&G_Rules_RAM.G_RulesRamData[buf_id[i]].time, &RulesFlashData.RulesAttr.time, sizeof(S_ModeTime));
		memcpy(G_Rules_RAM.G_RulesRamData[buf_id[i]].expersion, RulesFlashData.RulesConfig.expersion, MUI_RULE_LOGICAL_EXP_SIZE_MAX);
		for (j=0; j<RulesFlashData.RulesConfig.InputNum; j++)
		{
			memcpy(&G_Rules_RAM.G_RulesRamData[buf_id[i]].InPut[j], &RulesFlashData.RulesConfig.InputAction[j], sizeof(S_RulesInPutState));
		}
		for (j=0; j<RulesFlashData.RulesConfig.ToutputNum; j++)
		{
			memcpy(&G_Rules_RAM.G_RulesRamData[buf_id[i]].TrueOutPut[j], &RulesFlashData.RulesConfig.ToutputAction[j], sizeof(S_RuelsOutPutState));
		}
		for (j=0; j<RulesFlashData.RulesConfig.FoutputNum; j++)
		{
			memcpy(&G_Rules_RAM.G_RulesRamData[buf_id[i]].FalseOutPut[j], &RulesFlashData.RulesConfig.FoutputAction[j], sizeof(S_RuelsOutPutState));
		}
	}

	return iRet;
}

static int rules_execute_check(S_Rules_Ram_data *psRulesRamData)
{
	struct tm *timep;
	time_t now;
	uint32_t	Ruels_time;
	uint32_t	Now_time;

	time(&now);

	timep = localtime(&now);

	Now_time = mktime(timep);
	Ruels_time = mktime(&psRulesRamData->time.Time);

	if (psRulesRamData->time.time_mode == 1)
	{
		if (psRulesRamData->repeat&(1<<psRulesRamData->time.Time.tm_wday))
		{
			if (psRulesRamData->repeat&(1<<7))
			{
				psRulesRamData->time.Time.tm_year = timep->tm_year;
				psRulesRamData->time.Time.tm_mon = timep->tm_mon;
				psRulesRamData->time.Time.tm_mday = timep->tm_mday;
				Ruels_time = mktime(&psRulesRamData->time.Time);
				if ((Now_time-Ruels_time)>=0 && ((Now_time-Ruels_time)<= psRulesRamData->duration))
				{
					return MUI_RULES_TIME_CHECK_TRUE;
				}
			}
			else
			{
				Ruels_time = mktime(&psRulesRamData->time.Time);
				if ((Now_time-Ruels_time) <= 7*24*3600)
				{
					psRulesRamData->time.Time.tm_year = timep->tm_year;
					psRulesRamData->time.Time.tm_mon = timep->tm_mon;
					psRulesRamData->time.Time.tm_mday = timep->tm_mday;
					Ruels_time = mktime(&psRulesRamData->time.Time);
					if ((Now_time-Ruels_time)>=0 && ((Now_time-Ruels_time)<= psRulesRamData->duration))
					{
						return MUI_RULES_TIME_CHECK_TRUE;
					}
				}
			}
		}
		return MUI_RULES_TIME_CHECK_FALSE;
	}
	else if (psRulesRamData->time.time_mode == 2)
	{
		Ruels_time = mktime(&psRulesRamData->time.Time);
		if ((Now_time-Ruels_time)>=0 && ((Now_time-Ruels_time)<= psRulesRamData->duration))
		{
			return MUI_RULES_TIME_CHECK_TRUE;
		}
		return MUI_RULES_TIME_CHECK_FALSE;
	}
	return MUI_RULES_TIME_CHECK_FALSE;
}

static int rules_set_rollback_quitstate(S_RuelsOutPutRamState *psOutput)
{
	int iRet = 0;

	psOutput->quitstate.ActionType = psOutput->action.ActionType;
	switch(psOutput->action.ActionType)
	{
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
			iRet = read_dev_state_on(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.on);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
			iRet = read_dev_state_bri(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.bri);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
			iRet = read_dev_state_sat(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.sat);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
			iRet = read_dev_state_hue(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.hue);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
			iRet = read_dev_state_ct(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.ct);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
			iRet = read_dev_state_xy(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.xy);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
			iRet = read_dev_state_level(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.level);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
			iRet = 0;
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
			iRet = 0;
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_MODE:
			iRet = read_dev_state_warn_mode(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.warnmode);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_DURATION:
			iRet = read_dev_state_duration(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.duration);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
			iRet = read_dev_state_warn_cfg(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.warncfg);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
			iRet = read_dev_state_action(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.action);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
			iRet = read_dev_state_play(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.play);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
			iRet = read_dev_state_volume(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.volume);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
			iRet = read_dev_state_url_id(psOutput->DevType, psOutput->DevId, &psOutput->quitstate.ActionValue.urlid);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
			psOutput->quitstate.ActionValue.execute = set_get_execute_state(psOutput->DevId);
			break;
		case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE:
			if (psOutput->DevType == MUI_DATA_TYPE_RULE)
			{
				iRet = rules_read_active(psOutput->DevId, &psOutput->quitstate.ActionValue.active);
			}
			else if (psOutput->DevType == MUI_DATA_TYPE_SCHED)
			{

			}
			break;
		default:
			break;
	}
	return iRet;
}

static int rules_get_input_dev_state(S_Rules_Ram_data  *psRulesData, char *psLogic)
{
	int iRet = 0;
	int i, j;
	uint8_t ui8On;
	uint16_t ui16Temp;
	uint16_t ui16Hum;

	for (i=0; i<psRulesData->InPutNum; i++)
	{
		for (j=0; j<G_RulesInputStateQuene.RulesInputNum; j++)
		{
			if (G_RulesInputStateQuene.sRulesInputDevState[j].DevType == psRulesData->InPut[i].DevType && G_RulesInputStateQuene.sRulesInputDevState[j].DevId == psRulesData->InPut[i].DevId)
			{
				switch(psRulesData->InPut[i].ActionType)
				{
					case MUI_RULES_ACTION_STATE_WORD_IDX_ON:
						if (psRulesData->InPut[i].ActionValue.on == G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.on)
						{
							psLogic[i+1] = 1;
							ui8On = psLogic[i+1];
						}else
						{
							psLogic[i+1] = 0;
						}
						psRulesData->InPut[i].ActionValueOld.on = G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.on;
						break;
					case MUI_RULES_ACTION_STATE_WORD_IDX_TEMP:
						if (psRulesData->InPut[i].ActionValue.temp.less == 1)
						{
							if (G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.temp.temp < psRulesData->InPut[i].ActionValue.temp.temp)
							{
								psLogic[i+1] = 1;
							}else
							{
								psLogic[i+1] = 0;
							}
						}else
						{
							if (G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.temp.temp >= psRulesData->InPut[i].ActionValue.temp.temp)
							{
								psLogic[i+1] = 1;
							}else
							{
								psLogic[i+1] = 0;
							}
						}
						break;
					case MUI_RULES_ACTION_STATE_WORD_IDX_HUM:
						if (psRulesData->InPut[i].ActionValue.hum.less == 1)
						{
							if (G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.hum.humidity < psRulesData->InPut[i].ActionValue.hum.humidity)
							{
								psLogic[i+1] = 1;
							}else
							{
								psLogic[i+1] = 0;
							}
						}else
						{
							if (G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.hum.humidity >= psRulesData->InPut[i].ActionValue.hum.humidity)
							{
								psLogic[i+1] = 1;
							}else
							{
								psLogic[i+1] = 0;
							}
						}
						break;
					case MUI_RULES_ACTION_STATE_WORD_IDX_EDGE:
						if (psRulesData->InPut[i].ActionValue.edge == 1)   //up
						{
							if ((psRulesData->InPut[i].ActionValueOld.on == 0) && (G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.on == 1))
							{
								psLogic[i+1] = 1;
							}else
							{
								psLogic[i+1] = 0;
							}
						}
						else if (psRulesData->InPut[i].ActionValue.edge == 2)   //down
						{
							if ((psRulesData->InPut[i].ActionValueOld.on == 1) && (G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.on == 0))
							{
								psLogic[i+1] = 1;
							}else
							{
								psLogic[i+1] = 0;
							}
						}
						else if (psRulesData->InPut[i].ActionValue.edge == 3)   //all
						{
							if (psRulesData->InPut[i].ActionValueOld.on != G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.on)
							{
								psLogic[i+1] = 1;
							}else
							{
								psLogic[i+1] = 0;
							}
						}
						psRulesData->InPut[i].ActionValueOld.on = G_RulesInputStateQuene.sRulesInputDevState[j].ActionValue.on;
						break;
					default :
						break;
				}
				break;
			}
		}

		if (j < G_RulesInputStateQuene.RulesInputNum)
		{
			continue;
		}

		switch(psRulesData->InPut[i].ActionType)
		{
			case MUI_RULES_ACTION_STATE_WORD_IDX_ON:
				iRet = read_dev_state_on(psRulesData->InPut[i].DevType, psRulesData->InPut[i].DevId, &ui8On);
				if (iRet > 0)
				{
					if (psRulesData->InPut[i].ActionValue.on == ui8On)
					{
						psLogic[i+1] = 1;
						ui8On = psLogic[i+1];
					}else
					{
						psLogic[i+1] = 0;
					}
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].DevType = psRulesData->InPut[i].DevType;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].DevId = psRulesData->InPut[i].DevId;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].ActionValue.on = ui8On;
					G_RulesInputStateQuene.RulesInputNum++;
				}else
				{
					psLogic[i+1] = 0;
				}
				break;
			case MUI_RULES_ACTION_STATE_WORD_IDX_TEMP:
				iRet = read_dev_state_temp(psRulesData->InPut[i].DevType, psRulesData->InPut[i].DevId, &ui16Temp);
				if (iRet > 0)
				{
					if (psRulesData->InPut[i].ActionValue.temp.less == 1)
					{
						if (ui16Temp < psRulesData->InPut[i].ActionValue.temp.temp)
						{
							psLogic[i+1] = 1;
						}else
						{
							psLogic[i+1] = 0;
						}
					}else
					{
						if (ui16Temp >= psRulesData->InPut[i].ActionValue.temp.temp)
						{
							psLogic[i+1] = 1;
						}else
						{
							psLogic[i+1] = 0;
						}
					}
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].DevType = psRulesData->InPut[i].DevType;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].DevId = psRulesData->InPut[i].DevId;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].ActionValue.temp.temp = ui16Temp;
					G_RulesInputStateQuene.RulesInputNum++;
				}else
				{
					psLogic[i+1] = 0;
				}
				break;
			case MUI_RULES_ACTION_STATE_WORD_IDX_HUM:
				iRet = read_dev_state_hum(psRulesData->InPut[i].DevType, psRulesData->InPut[i].DevId, &ui16Hum);
				if (iRet > 0)
				{
					if (psRulesData->InPut[i].ActionValue.hum.less == 1)
					{
						if (ui16Hum < psRulesData->InPut[i].ActionValue.hum.humidity)
						{
							psLogic[i+1] = 1;
						}else
						{
							psLogic[i+1] = 0;
						}
					}else
					{
						if (ui16Hum >= psRulesData->InPut[i].ActionValue.hum.humidity)
						{
							psLogic[i+1] = 1;
						}else
						{
							psLogic[i+1] = 0;
						}
					}
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].DevType = psRulesData->InPut[i].DevType;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].DevId = psRulesData->InPut[i].DevId;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].ActionValue.hum.humidity = ui16Hum;
					G_RulesInputStateQuene.RulesInputNum++;
				}else
				{
					psLogic[i+1] = 0;
				}
				break;
			case MUI_RULES_ACTION_STATE_WORD_IDX_EDGE:
				iRet = read_dev_state_on(psRulesData->InPut[i].DevType, psRulesData->InPut[i].DevId, &ui8On);
				if (iRet > 0)
				{
					if (psRulesData->InPut[i].ActionValue.edge == 1)   //up
					{
						if ((psRulesData->InPut[i].ActionValueOld.on == 0) && (ui8On == 1))
						{
							psLogic[i+1] = 1;
						}else
						{
							psLogic[i+1] = 0;
						}
					}
					else if (psRulesData->InPut[i].ActionValue.edge == 2)   //down
					{
						if ((psRulesData->InPut[i].ActionValueOld.on == 1) && (ui8On == 0))
						{
							psLogic[i+1] = 1;
						}else
						{
							psLogic[i+1] = 0;
						}
					}
					else if (psRulesData->InPut[i].ActionValue.edge == 3)   //all
					{
						if (psRulesData->InPut[i].ActionValueOld.on != ui8On)
						{
							psLogic[i+1] = 1;
						}else
						{
							psLogic[i+1] = 0;
						}
					}
					//RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\n\n\n--------------------------------------------\r\n", ui8On);
					//RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[[[[[[[[[[[[[[[[[ActionValue:%d]]]]]]]]]]]]]]]]\r\n", ui8On);
					//RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[[[[[[[[[[[[[[[[[ActionValueOld:%d]]]]]]]]]]]]]]]]\r\n", psRulesData->InPut[i].ActionValueOld.on);
					//RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\n\n\n--------------------------------------------\r\n", ui8On);
					psRulesData->InPut[i].ActionValueOld.on = ui8On;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].DevType = psRulesData->InPut[i].DevType;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].DevId = psRulesData->InPut[i].DevId;
					G_RulesInputStateQuene.sRulesInputDevState[G_RulesInputStateQuene.RulesInputNum].ActionValue.on = ui8On;
					G_RulesInputStateQuene.RulesInputNum++;
				}else
				{
					psLogic[i+1] = 0;
				}
				break;
			default :
				break;
		}
	}
	return iRet;
}

static void rules_output_quene_add(S_RuelsOutPutRamState *psRuelsState, uint8_t ui8t)
{
	int i;
	for(i=0; i<G_RulesOutPutQuene.RulesOutPutNum; i++)
	{
		if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == psRuelsState->DevType && G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId == psRuelsState->DevId && G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionType == psRuelsState->action.ActionType)
		{
			if (ui8t == 1)
			{
				memcpy(&G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue, &psRuelsState->action.ActionValue, sizeof(U_RulesAction));
				return ;
			}
		}
	}
	G_RulesOutPutQuene.sRuelsOutPutAttr[G_RulesOutPutQuene.RulesOutPutNum].DevType = psRuelsState->DevType;
	G_RulesOutPutQuene.sRuelsOutPutAttr[G_RulesOutPutQuene.RulesOutPutNum].DevId = psRuelsState->DevId;
	if (ui8t == 1)
	{
		G_RulesOutPutQuene.sRuelsOutPutAttr[G_RulesOutPutQuene.RulesOutPutNum].action.ActionType = psRuelsState->action.ActionType;
		memcpy(&G_RulesOutPutQuene.sRuelsOutPutAttr[G_RulesOutPutQuene.RulesOutPutNum].action.ActionValue, &psRuelsState->action.ActionValue, sizeof(U_RulesAction));
	}
	else if (ui8t == 2)
	{
		G_RulesOutPutQuene.sRuelsOutPutAttr[G_RulesOutPutQuene.RulesOutPutNum].action.ActionType = psRuelsState->quitstate.ActionType;
		memcpy(&G_RulesOutPutQuene.sRuelsOutPutAttr[G_RulesOutPutQuene.RulesOutPutNum].action.ActionValue, &psRuelsState->quitstate.ActionValue, sizeof(U_RulesAction));
	}
	G_RulesOutPutQuene.RulesOutPutNum++;
}


/*
 *
 *
 *
 */
static int ruels_output_active ()
{
	int i, iRet=0, iTmp;

	for (i = 0; i < G_RulesOutPutQuene.RulesOutPutNum; i++)
	{
		switch (G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionType)
		{
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_set_state_on(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.on);
				}
				else
				{
					iTmp = set_dev_state_on(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.on);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_light_state_bri(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.bri);
				}
				else
				{
					iTmp = set_dev_state_bri(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.bri);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					//iTmp = group_light_state_sat(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.on);
				}
				else
				{
					iTmp = set_dev_state_sat(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.sat);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					//iTmp = group_light_state_hue(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.on);
				}
				else
				{
					iTmp = set_dev_state_hue(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.hue);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_light_state_ct(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.ct);
				}
				else
				{
					iTmp = set_dev_state_ct(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.ct);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_light_state_xy(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.xy);
				}
				else
				{
					iTmp = set_dev_state_xy(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.xy);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_set_state_level(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.level);
				}
				else
				{
					iTmp = set_dev_state_level(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.level);
				}
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
				iTmp = set_dev_state_temp(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.temp.temp);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
				iTmp = set_dev_state_hum(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.hum.humidity);
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_CONSUMPTION:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_warn_dev_state_warn_cfg(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.warncfg);
				}
				else
				{
					iTmp = set_dev_state_warn_cfg(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.warncfg);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_shade_state_action(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.action);
				}
				else
				{
					iTmp = set_dev_state_action(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.action);
				}
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
				iTmp = set_dev_state_play(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.play);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
				iTmp = set_dev_state_volume(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.volume);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
				iTmp = set_dev_state_url_id(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType, G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.urlid);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
				iTmp = set_execute_active(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.execute);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE :
				if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_RULE)
				{
					iTmp = rules_write_active(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.active);
				}
				else if (G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevType == MUI_DATA_TYPE_SCHED)
				{
					iTmp = schedule_write_active(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.active);
				}
				break;
			default:
				continue;
		}

	}
	return iRet;
}


//-------------------------------------------------------------------------------------------------*
/*	rules active处理任务函数
 ;	(功能)
 ;	(参数)
 ;		pArg:	传给函数运行的参数
 ;	(返回)
 ;		NULL
 ;	(注意)
 */
static void* s_rules_execute_deal_task (void *pArg)
{
	UNUSED(pArg);

	int iRet = 0;
	int i, j = 0, k;
	char LogicBuf[MUI_RULE_LOGICAL_EXP_SIZE_MAX] = {0};
	G_RulesOutPutQuene.RulesOutPutNum = 0;
	G_RulesInputStateQuene.RulesInputNum = 0;

	iRet = sem_init(&s_tRulesRamDataSem, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_rules_execute_deal_task] sem_init fail.:%d\r\n", iRet);
		return;
	}
	rules_RamData_init();

	while(1)
	{
		if (0 > sem_wait_ms(&s_tRulesRamDataSem, 10000))
		{
			RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_rules_execute_deal_task] sem_wait_ms fail.\r\n");
			continue;
		}

		//循环所有的rules
		for (i=0; i< G_Rules_RAM.RulesNum; i++)
		{
			//如果active为true时，判断duration，如果duration的值大于86400(一整天)就跳过检查rules是否在指定的时间段内(execute设置为2)，否则就开始检查rules的时间条件是否符合(execute设置为1)
			if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].active == 1)
			{
				if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].duration > 86400)
				{
					G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].execute = 2;
				}else
				{
					iRet = rules_execute_check(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]]);
					if (iRet == MUI_RULES_TIME_CHECK_TRUE)
					{
						G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].duration_count = G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].duration;
						G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].execute = 1;
					}
				}
			}
			else
			{
				//是否要执行quitstate
				G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].execute = 0;
				G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate = 0;
				G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate = 0;
				continue;
			}

			//execute大于0时需要判断两种情况: execute = 1, execute = 2
			if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].execute > 0)
			{
				//execute为1时，最后要检查duration的状态
				if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].execute == 1)
				{
					memset(LogicBuf, 0, MUI_RULE_LOGICAL_EXP_SIZE_MAX);
					rules_get_input_dev_state(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]], LogicBuf);
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].InPutNum > 1)
					{
						iRet = Get_Logic_Val(G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].expersion, LogicBuf);
					}else
					{
						iRet = LogicBuf[0];
					}
					if (iRet == 1)
					{
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate == 1)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate = 2;
						}
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate == 0)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate = 1;
						}
					}
					else
					{
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate == 1)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate = 2;
						}
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate == 0)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate = 1;
						}
					}
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].duration_count > 0)
					{
						G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].duration_count--;
					}
					else
					{
						G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].execute = 0;
					}
				}
				else if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].execute == 2)
				{
					memset(LogicBuf, 0, MUI_RULE_LOGICAL_EXP_SIZE_MAX);
					rules_get_input_dev_state(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]], LogicBuf);
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].InPutNum > 1)
					{
						iRet = Get_Logic_Val(G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].expersion, LogicBuf);
					}else
					{
						iRet = LogicBuf[1];
					}
					if (iRet == 1)
					{
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate == 1)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate = 2;
						}
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate == 0)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate = 1;
							for (k=0; k<G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].ToutPutNum; k++)
							{
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[k].delay_count = 1;
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[k].duration_count = 1;
							}
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].ToutFinishNum = 0;
						}
					}
					else
					{
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate == 1)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate = 2;
						}
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate == 0)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate = 1;
							for (k=0; k<G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FoutPutNum; k++)
							{
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[k].delay_count = 1;
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[k].duration_count = 1;
							}
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FoutFinishNum = 0;
						}
					}
				}
			}

			//
			if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate > 0)
			{
				//Toutstate为2时需要判断delay和duration是否需要重置
				if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate == 2)
				{
					//判断是否需要重置
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration_count == 0)
					{
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delayoverlay == 1)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delay_count = 1;
						}
					}
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].durationoverlay == 1)
					{
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration_count != G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration_count = 1;
						}
					}
					//判断完成后把Toutstate设置为1 并初始化所有的TrueOutPut的delay_count和duration_count为1
					G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate = 1;

					for (k=0; k<G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].ToutPutNum; k++)
					{
						G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[k].delay_count = 1;
						G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[k].duration_count = 1;
					}
				}

				for (j=0; j<G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].ToutPutNum; j++)
				{
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate > 0)
					{
						RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++\r\n");
						RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_Rules_execute_deal_task] s*********%hu  %d**delay=%d,delaycount=%d*************\r\n",G_Rules_RAM.IdList[i], j, G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delay, G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delay_count);
						RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++\r\n");
						//因为delay_count初始化值为1，所以delay加1
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delay_count == (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delay+1))
						{
							//duration_count为1时表示时刚开始计时，这时开始执行output
							//执行该Toutput
							if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration_count == 1)
							{
								if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].quitmethod == 2)
								{
									if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].DevType >= MUI_DATA_TYPE_DEV_START &&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].DevType <= MUI_DATA_TYPE_DEV_END)
									{
										iRet = rules_set_rollback_quitstate(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j]);
									}
								}
								rules_output_quene_add(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j], 1);
							}
							//因为duration_count初始化值为1，所以 duration加1
							if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration_count == (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration+1))
							{
								//执行quitstate
								if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].quitmethod > 0)
								{
									rules_output_quene_add(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j], 2);
								}
								//恢复delay_count和duration_count为默认值0
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delay_count = 0;
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration_count = 0;
								//ToutFinishNum加1，如果和ToutPutNum相等，则表示所有output执行完成，把Toutstate设置为0，ToutFinishNum也设置为0
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].ToutFinishNum ++;
								if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].ToutFinishNum == G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].ToutPutNum)
								{
									G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Toutstate = 0;
									G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].ToutFinishNum = 0;
								}
							}
							else
							{
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].duration_count++;
							}
						}
						else
						{
							//为0则表示为开始delay
							if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delay_count != 0)
							{
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].TrueOutPut[j].delay_count++;
							}
						}
					}
				}
			}

			if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate > 0)
			{
				if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate == 2)
				{
					//判断是否需要重置
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration_count == 0)
					{
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delayoverlay == 1)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delay_count = 1;
						}
					}
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].durationoverlay == 1)
					{
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration_count != G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration)
						{
							G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration_count = 1;
						}
					}
					G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate = 1;
					for (k=0; k<G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FoutPutNum; k++)
					{
						G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[k].delay_count = 1;
						G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[k].duration_count = 1;
					}
				}

				for (j=0; j<G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FoutPutNum; j++)
				{
					if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate > 0)
					{
						RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++\r\n");
						RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_Rules_execute_deal_task] s*********%hu  %d**delay=%d,delaycount=%d*************\r\n",G_Rules_RAM.IdList[i], j, G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delay, G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delay_count);
						RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"+++++++++++++++++++++++++\r\n");
						if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delay_count == (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delay+1))
						{
							//执行该output
							if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration_count == 1)
							{
								if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].quitmethod == 2)
								{
									if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].DevType >= MUI_DATA_TYPE_DEV_START &&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].DevType <= MUI_DATA_TYPE_DEV_END)
									{
										iRet = rules_set_rollback_quitstate(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j]);
									}
								}
								rules_output_quene_add(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j], 1);
							}
							if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration_count == (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration+1))
							{
								//执行quitstate
								if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].quitmethod == 1)
								{
									rules_output_quene_add(&G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j], 2);
								}

								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delay_count = 0;
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration_count = 0;
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FoutFinishNum++;
								if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FoutFinishNum == G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FoutPutNum)
								{
									G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].Foutstate = 0;
									G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FoutFinishNum = 0;
								}
							}
							else
							{
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].duration_count++;
							}
						}
						else
						{
							if (G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delay_count != 0)
							{
								G_Rules_RAM.G_RulesRamData[G_Rules_RAM.IdList[i]].FalseOutPut[j].delay_count++;
							}
						}
					}
				}
			}

			/*if (i == (G_Rules_RAM.RulesNum-1))
			{
				//input state 设置为0
				G_RulesInputStateQuene.RulesInputNum = 0;
				if (G_RulesOutPutQuene.RulesOutPutNum > 0)
				{
					//好了，发送命令吧，缓存方式发送
					ruels_output_active();
					G_RulesOutPutQuene.RulesOutPutNum = 0;

				}
			}*/
		}
		G_RulesInputStateQuene.RulesInputNum = 0;
		if (G_RulesOutPutQuene.RulesOutPutNum > 0)
		{
			//好了，发送命令吧，缓存方式发送
			ruels_output_active();
			G_RulesOutPutQuene.RulesOutPutNum = 0;

		}
		sem_post(&s_tRulesRamDataSem);
		msleep(1000);
	}
	return NULL;
}



int rules_init()
{
	int iRet = 0;

	RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_init] s***********start pthread*************\r\n");
	iRet = pthread_create(&s_Rules_DealThread, NULL, s_rules_execute_deal_task, NULL);
	if (iRet < 0)
	{
		RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_init] s**********pthread error*************\r\n");
		return -1;
	}
	return 1;
}














