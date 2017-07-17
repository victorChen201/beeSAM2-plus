/*
 * group_data.h
 *
 *  Created on: April 14, 2015
 *      Author: songyongheng
 */
#ifndef	__M_FILE_NAME_group_data_H__
#define	__M_FILE_NAME_group_data_H__

#include "common.h"
#include "file_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct _GroupApiAttr
{
    S_AttrName		name;			//A unique, editable name given to the device.
    S_AttrDescr		descr;			//An editable attribute given to device. Now, it used to label the ZONE that device belongs.
} S_GroupApiAttr;

typedef struct _GroupZbAttr
{
    S_ZbAttrNwkAddr	nwkaddr;
} S_GroupZbAttr;

typedef struct _GroupMember
{
	uint16_t		devType;
	uint16_t		devId;
} S_GroupMember;

typedef struct _GroupOpAttr
{
    uint16_t		total;		//成员总数
    S_GroupMember 	member[MUI_DEV_NUM_MAX_PER_GROUP];
} S_GroupOpAttr;
//................................................
typedef struct _GroupAttrFlash
{
    S_GroupApiAttr	api_attr;
    S_GroupZbAttr	zb_attr;
    S_GroupOpAttr	op_attr;
} S_GroupAttrFlash;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int group_data_id_list_get (uint16_t *pui16Buf, uint16_t ui16Capacity);
extern int group_data_num_get (void);
extern int group_data_assign_id (void);
extern int group_data_read (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize);
extern int group_data_write (uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize);
extern int group_data_add (const S_GroupAttrFlash *pvGroupFlash);
extern int group_data_del (uint16_t ui16Id);
extern int group_data_del_all (void);
extern int group_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
extern int group_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id);
extern int group_data_add_member (uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType);
extern int group_data_del_member (uint16_t ui16Id, const uint16_t ui16DevId, const uint16_t ui16DevType);
extern int group_data_init(void);

#endif /* __M_FILE_NAME_group_data_H__ */

