/*
 * rule_data.h
 *
 *  Created on: April 22, 2015
 *      Author: songyongheng
 */
#ifndef	__M_FILE_NAME_rule_data_H__
#define	__M_FILE_NAME_rule_data_H__

#include "common.h"
#include "file_linux.h"

extern int rule_data_init (void);
extern int rule_data_read_info (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize);
extern int rule_data_write_info (uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize);
extern int rule_data_get_id_list (uint16_t *pui16Buf, uint16_t ui16Capacity);
extern int rule_data_assign_id (void);
extern int rule_data_add (const void *pvRomBuf, size_t tRomSize);
extern int rule_data_del (uint16_t ui16Id);
extern int rule_data_del_all (void);
extern int rule_data_swap_id (uint16_t ui16DestId, uint16_t ui16SrcId);
extern int rule_data_set_new_id (uint16_t ui16NewId, uint16_t ui16Id);

#endif /* __M_FILE_NAME_rule_data_H__ */

