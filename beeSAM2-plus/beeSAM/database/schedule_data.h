/*
 * schedule_data.h
 *
 *  Created on: April 22, 2015
 *      Author: songyongheng
 */
#ifndef	__M_FILE_NAME_schedule_data_H__
#define	__M_FILE_NAME_schedule_data_H__

#include "common.h"
#include "file_linux.h"

extern int schedule_data_init (void);
extern int schedule_data_get_id_list (uint16_t *pui16Buf, uint16_t ui16Capacity);
extern int schedule_data_assign_id (void);
extern int schedule_data_add (const void *pvRomBuf, size_t tRomSize);
extern int schedule_data_del_all (void);
extern int schedule_data_del (uint16_t ui16Id);
extern int schedule_data_read_info (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize);
extern int schedule_data_write_info (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize);
extern int schedule_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
extern int schedule_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id);
extern int schedule_data_add_member (uint16_t ui16Id, void *pvActionList, const size_t tSize, uint16_t ui16ActionNum);
extern int schedule_data_del_member (uint16_t ui16Id, void *pvOutPutList, const size_t tSize);

#endif /* __M_FILE_NAME_schedule_data_H__ */





































