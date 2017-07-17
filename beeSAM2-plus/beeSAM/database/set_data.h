/*
 * set_data.h
 *
 *  Created on: April 14, 2015
 *      Author: songyongheng
 */
#ifndef	__M_FILE_NAME_set_data_H__
#define	__M_FILE_NAME_set_data_H__

#include "common.h"
#include "file_linux.h"

#define MAX_SET_ACTION_VALUE_LEN			16

typedef union
{
	uint8_t			on;
	uint8_t			bri;
	uint8_t			sat;
	uint16_t		hue;
	uint16_t		ct;
	S_Color_XY		xy;
    uint16_t 		level;
	uint16_t		temp;
	uint16_t		hum;
	uint8_t			warnmode;
	uint16_t		duration;
	S_WarnCfg		warncfg;
	uint8_t			action;
	uint8_t			play;
	uint16_t		volume;
	uint32_t		urlid;
	uint8_t			execute;
	uint8_t			active;
} U_SetAction;

typedef struct
{
	uint16_t	dev_type;
	uint16_t	dev_id;
	uint16_t	action_type;
	//uint8_t		action_value[MAX_SET_ACTION_VALUE_LEN];
	U_SetAction	uSetAtion;
} S_SetMember;

typedef struct
{
	S_AttrName	name;			//A unique, editable name given to the device.
	S_AttrDescr	descr;			//An editable attribute given to device. Now, it used to label the ZONE that device belongs.
	uint16_t	action_num;		//成员总数
	uint16_t	delay;			//set的延时时间，单位:秒
} S_SetAttrFlash;

extern int set_data_id_list_get (uint16_t *pui16Buf, uint16_t ui16Capacity);
extern int set_data_num_get (void);
extern int set_data_assign_id (void);
extern int set_data_read (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize);
extern int set_data_write (uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize);
extern int set_data_add (const void *pvRomBuf, size_t tRomSize);
extern int set_data_del (uint16_t ui16Id);
extern int set_data_del_all (void);
extern int set_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
extern int set_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id);
extern int set_data_add_member (uint16_t ui16Id, const S_SetMember *psMemberList, uint16_t ui16Num);
extern int set_data_del_member (uint16_t ui16Id, const S_SetMember *psMemberList, uint16_t ui16Num);
extern int set_data_init (void);

#endif /* __M_FILE_NAME_set_data_H__ */
