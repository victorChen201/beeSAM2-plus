/*
 * set.h
 *
 *  Created on: July 28, 2014
 *      Author: songyongheng
 */
//#define	__M_FILE_NAME_set_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_set_H__
#define	__M_FILE_NAME_set_H__

#include "common.h"
#include "dev_if.h"
#include "api_deal.h"
#include "set_data.h"
#include "schedule.h"
#include "group.h"

/*typedef struct _SetAttr
 {
 uint8_t dev_type;
 uint8_t dev_id;
 uint8_t action_type;
 uint16_t action_value1;
 uint16_t action_value2;
 }S_SetMember;*/


//用两个量来标志set执行的状态是为了防止在改动过程中的误操作
typedef struct _SetExecuteState
{
	uint8_t set_execute;		//只是作为是否正在执行的标志 1 为执行状态，0为未执行状态
	uint16_t set_delay;		//存储set延迟执行的时间
} S_SetExecuteState;

extern void trans_set_part (uint8_t method, char *url, char *jsonstr, S_ConntCtrl *psConntCtrl, char send[], int sendsize);
extern char *create_reply_get_attri_set (uint8_t id);
extern int set_init ();
extern int set_add(const void *pvRomBuf, size_t tRomSize);
extern int set_del(uint16_t ui16Id);
extern int set_write_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize);
extern int set_write_name(const uint16_t ui16Id, const S_AttrName *name);
extern int set_write_description(const uint16_t ui16Id, const S_AttrDescr *description);
extern int set_write_action_num(const uint16_t ui16Id, const uint16_t *ui16action_num);
extern int set_write_delay(const uint16_t ui16Id, const uint16_t *ui16delay);
extern int set_get_id_list(uint16_t *aui16IdList, uint16_t ui16Capacity);
extern int set_read_name(const uint16_t ui16Id, const S_AttrName *name);
extern int set_read_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize);
extern int set_read_action(uint16_t ui16Id, uint16_t ui16actionId, S_SetMember *pvAttr);
extern int set_get_execute_state(uint16_t ui16Id);
extern int set_execute_active (const uint16_t id, const uint8_t execute);

#endif /*__M_FILE_NAME_set_H__*/
