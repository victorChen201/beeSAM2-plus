/*
 * scene_data.h
 *
 *  Created on: April 14, 2015
 *      Author: songyongheng
 */
#ifndef	__M_FILE_NAME_scene_data_H__
#define	__M_FILE_NAME_scene_data_H__

#include "common.h"
#include "file_linux.h"

#define MAX_SCENE_ACTION_VALUE_LEN			16
#define MAX_SCENE_MEMBER_NUM				50

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
} U_SceneAction;

typedef struct
{
	uint16_t	dev_type;
	uint16_t	dev_id;
	uint16_t	action_type;
	U_SceneAction	uSceneAtion;
} S_SceneMember;

typedef struct
{
	S_AttrName	name;			//A unique, editable name given to the device.
	S_AttrDescr	descr;			//An editable attribute given to device. Now, it used to label the ZONE that device belongs.
	uint16_t	action_num;		//成员总数
} S_SceneAttr;

typedef struct
{
	S_SceneAttr		scene_attr;
	S_SceneMember	scene_member[MAX_SCENE_MEMBER_NUM];
} S_SceneDataFalsh;

extern int scene_data_id_list_get (uint16_t *pui16Buf, uint16_t ui16Capacity);
extern int scene_data_num_get (void);
extern int scene_data_assign_id (void);
extern int scene_data_read (uint16_t ui16Id, size_t tOffscene, void *pvBuf, size_t tSize);
extern int scene_data_write (uint16_t ui16Id, size_t tOffscene, const void *pvBuf, size_t tSize);
extern int scene_data_add (const void *pvRomBuf);
extern int scene_data_del (uint16_t ui16Id);
extern int scene_data_del_all (void);
extern int scene_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
extern int scene_data_scene_new_id (uint16_t ui16NewId, uint16_t ui16Id);
extern int scene_data_add_member (uint16_t ui16Id, const S_SceneMember *psMemberList, uint16_t ui16Num);
extern int scene_data_del_member (uint16_t ui16Id, const S_SceneMember *psMemberList, uint16_t ui16Num);
extern int scene_data_init (void);

#endif /* __M_FILE_NAME_scene_data_H__ */
