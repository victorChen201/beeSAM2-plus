#ifndef _SCHDU_DATA_H_

#define _SCHDU_DATA_H_
#include "common.h"
#include "sys_time.h"
#include "rule.h"

#define S_POS_ID             0
#define S_POS_NUM            1

#define S_DELETE          0
#define S_UPDATE            1
#define S_ADDTAIL           2

#define S_TIME_POS          10
#define S_START_TIME        0
#define S_END_TIME          4

#define S_YY_MM_T           0
#define S_HH_SS_T           1

#define S_TIME_LEN          20
#define SCHDU_STACK        1048
#define SCHDU_PRI           1
//#define DEBUG

/*
 uint8_t schid;
 uint8_t config_num;
 uint8_t dev_type;
 uint8_t dev_id;
 uint8_t action_type;
 uint8_t quittype
 uint16_t delay
 uint16_t duration
 uint16_t quite_value1
 uint16_t quite_value2

 uint16_t action_value1;
 uint16_t action_value2;
 uint8_t active;
 uint8_t repeat;
 char time[20];
 char name[32];
 char desc[64];

 */

/*typedef struct _RULE_Output
 {

 uint8_t  output_type;
 uint8_t  output_id;
 uint8_t  delay_flag;
 uint16_t delay_count;
 uint16_t out_delay;
 uint16_t output_dur;
 uint16_t output_counttime;
 uint16_t out_value1;
 uint16_t out_value2;
 uint16_t out_oldvalue1;
 uint16_t out_oldvalue2;
 struct _RULE_Output* output_next;

 unsigned char out_longcount;
 unsigned int out_flick;
 uint8_t out_value_type;
 uint8_t finish_flag;
 uint16_t quit_state1;
 uint16_t quit_state2;
 } Rule_Output;*/

typedef struct _SCHDU_DEV
{
	uint8_t dev_type;
	uint8_t action_type;
	uint8_t dev_id;

	uint8_t quit_type;
	uint8_t tri_flag;
	uint32_t flick;
	uint16_t s_counttime;
	uint16_t s_delaycount;
	uint16_t s_dur;
	uint32_t s_delay;
	uint16_t action_value1;
	uint16_t action_value2;
	uint16_t quit_value1;
	uint16_t quit_value2;

	struct _SCHDU_DEV *dev_next;
} schdu_dev;

typedef struct _SCHDU
{
	uint8_t schid;
	uint8_t exe_out_conut;
	uint8_t config_num;
	uint8_t active;
	uint8_t repeat;
	uint8_t tri_flag;
	char time[S_TIME_LEN];
	uint32_t m_time_count;
	struct _SCHDU_DEV* dev_head;
	struct _SCHDU* m_next;
} s_schdu;

void init_schdu_task (void);
void schdu_task (void* prm);
void skip_schdu_table (void);
int set_change_schdu (const uint8_t id, const uint8_t flag);
//int set_schdu_active (const uint8_t id, const uint8_t on_off);
#endif
