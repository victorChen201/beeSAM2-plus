//#define	__M_FILE_NAME_schedule_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of schedule module for beeSAM project.
File name:schedule.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_schedule_H__
#define	__M_FILE_NAME_schedule_H__

#include "common.h"
#include "data_common.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_SCHEDULE_TIME_CHECK_FALSE						0
#define MUI_SCHEDULE_TIME_CHECK_TRUE						1
#define MUI_SCHEDULE_REPEAT_CHECK_FALSE						0
#define MUI_SCHEDULE_REPEAT_CHECK_TRUE						1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	uint8_t			onsec;
	uint8_t			offsec;
} S_ScheduleBlinkOutput;

typedef union
{
	uint8_t					on;
	uint8_t					bri;
	uint8_t					sat;
	uint16_t				hue;
	uint16_t				ct;
	S_Color_XY				xy;
    uint16_t 				level;
	uint8_t					warnmode;
	uint16_t				duration;
	S_WarnCfg				warncfg;
	uint8_t					action;
	uint8_t					play;
	uint16_t				volume;
	uint32_t				urlid;
	uint8_t					execute;
	uint8_t					active;
	S_ScheduleBlinkOutput	blink;
} U_ScheduleAction;

typedef struct
{
	S_AttrName			name;
	S_AttrDescr			descr;
	uint8_t				active;
	uint8_t				repeat;
	S_ModeTime 			time;
} S_ScheduleAttr;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	uint16_t			delay;
	//uint16_t			delay_count;
	uint16_t			duration;
	//uint16_t			duration_count;
	uint8_t				ActionType;
	U_ScheduleAction	ActionValue;
	uint8_t				quitmethod;
	uint8_t				quitType;
	U_ScheduleAction	quitValue;
} S_ScheduleOutPutAction;

typedef struct
{
	uint8_t						OutputNum;
	S_ScheduleOutPutAction 		OutputAction[MUI_OUTPUT_NUM_MAX_PER_SCHEDULE];
} S_ScheduleConfig;

typedef struct
{
	S_ScheduleAttr			ScheduleAttr;
	S_ScheduleConfig		ScheduleConfig;
} S_ApiParseSchedule;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	uint16_t			delay;
	int					delay_count;
	uint16_t			duration;
	int					duration_count;
	uint8_t				ActionType;
	U_ScheduleAction	ActionValue;
	uint8_t				quitmethod;
	uint8_t				quitType;
	U_ScheduleAction	quitValue;
	//暂未实现
	uint8_t				oldType;
	U_ScheduleAction	oldValue;
} S_ScheduleOutPutState;

typedef struct
{
	uint8_t						execute;
	uint8_t						active;
	uint8_t						repeat;
	S_ModeTime					time;
	uint8_t						outputNum;
	uint8_t						finishOutPut;
	S_ScheduleOutPutState		OutPut[MUI_OUTPUT_NUM_MAX_PER_SCHEDULE];
} S_ScheduleRamData;

typedef struct
{
	uint8_t			ScheduleNum;
	uint16_t		IdList[MUI_SCHEDULE_DATA_NUM_MAX];
	S_ScheduleRamData ScheduleRamData[MUI_SCHEDULE_DATA_NUM_MAX];
} S_Schedule_Ram;

typedef struct
{
	uint16_t			DevType;
	uint16_t			DevId;
	uint8_t				ActionType;
	U_ScheduleAction	ActionValue;
} S_ScheduleOutPutAttr;

typedef struct
{
	uint8_t					ScheduleOutPutNum;
	S_ScheduleOutPutAttr  	sScheduleOutPutAttr[MUI_OUTPUT_NUM_MAX_PER_SCHEDULE*2];
} S_ScheduleOutPutQuene;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int schedule_get_id_list(uint16_t *pui16Buf, uint16_t ui16Capacity);
extern int schedule_add(S_ApiParseSchedule *psSchedule);
extern int schedule_del(uint16_t ui16Id);
extern int schedule_read_info(uint16_t ui16Id, S_ApiParseSchedule *psSchedule);
extern int schedule_read_name(uint16_t ui16Id, S_AttrName *psName);
extern int schedule_read_desc(uint16_t ui16Id, S_AttrDescr *psDesc);
extern int schedule_read_repeat(uint16_t ui16Id, uint16_t *ui16Repeat);
extern int schedule_read_time(uint16_t ui16Id, S_ModeTime *psTime);
extern int schedule_read_attr(uint16_t ui16Id, S_ScheduleAttr *psAttr);
extern int schedule_read_outputnum(uint16_t ui16Id, const uint8_t *ui8OutputNum);
extern int schedule_read_output(uint16_t ui16Id, uint8_t ui8Idx, S_ScheduleOutPutState *psOutPut);
extern int schedule_read_config(uint16_t ui16Id, char *psConfig);
extern int schedule_write_info(uint16_t ui16Id, const S_ApiParseSchedule *psSchedule);
extern int schedule_write_name(uint16_t ui16Id, S_AttrName *psName);
extern int schedule_write_desc(uint16_t ui16Id, S_AttrDescr *psDesc);
extern int schedule_write_active(uint16_t ui16Id, uint8_t *ui8Active);
extern int schedule_write_repeat(uint16_t ui16Id, uint8_t *ui8Repeat);
extern int schedule_write_time(uint16_t ui16Id, S_ModeTime *psTime);
extern int schedule_write_attr(uint16_t ui16Id, S_ScheduleAttr *psAttr);
extern int schedule_write_outputnum(uint16_t ui16Id, const uint8_t *ui8OutputNum);
extern int schedule_write_output(uint16_t ui16Id, uint8_t ui8Idx, S_ScheduleOutPutState *psOutPut);
extern int schedule_write_config(uint16_t ui16Id, char *psConfig);
extern int schedule_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_schedule_H__".

