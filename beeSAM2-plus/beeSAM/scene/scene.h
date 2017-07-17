/*
 * scene.h
 *
 *  Created on: July 28, 2014
 *      Author: songyongheng
 */
//#define	__M_FILE_NAME_scene_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_scene_H__
#define	__M_FILE_NAME_scene_H__

#include "common.h"
#include "dev_if.h"
#include "api_deal.h"
#include "scene_data.h"

typedef struct _S_SceneDevAddr
{
	uint16_t devType;
	uint16_t devId;
} S_SceneDevAddr;

typedef struct _S_SceneAtcmd
{
	uint8_t			AtcmdNum;
	S_SceneDevAddr SceneDevAddr[MAX_SCENE_MEMBER_NUM];	//scene最大的member数目，不会超过这个数目
} S_SceneAtcmd;

#define SCENE_CONSTANT_GROUP_ID				200


extern int scene_init ();
extern int scene_add(const void *pvRomBuf);
extern int scene_del(uint16_t ui16Id);
extern int scene_write_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize);
extern int scene_write_name(const uint16_t ui16Id, const S_AttrName *name);
extern int scene_write_description(const uint16_t ui16Id, const S_AttrDescr *description);
extern int scene_write_action_num(const uint16_t ui16Id, const uint16_t *ui16action_num);
extern int scene_get_id_list(uint16_t *aui16IdList, uint16_t ui16Capacity);
extern int scene_read_name(const uint16_t ui16Id, const S_AttrName *name);
extern int scene_read_info(const uint16_t ui16Id, void *pvBuf, const size_t tSize);
extern int scene_read_action(uint16_t ui16Id, uint16_t ui16actionId, S_SceneMember *pvAttr);
extern void scene_devaddr_add(uint16_t ui16Type, uint16_t ui16Id);
extern int scene_atcmd_send (uint16_t	ui16SceneId);
extern int scene_create (const S_SceneDataFalsh *pSceneData);
extern int scene_active(uint16_t ui16Id);


#endif /*__M_FILE_NAME_scene_H__*/
