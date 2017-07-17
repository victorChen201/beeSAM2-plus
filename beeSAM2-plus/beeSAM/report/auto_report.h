/*
 * auto_report.h
 *
 *  Created on: July 28, 2014
 *      Author: songyongheng
 */
//#define	__M_FILE_NAME_auto_report_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_auto_report_H__
#define	__M_FILE_NAME_auto_report_H__

#include "common.h"
#include "sys_time.h"

extern int auto_report_dev_add (const uint16_t type, const uint16_t id, const uint16_t interval);
extern int auto_report_dev_del (const uint16_t type, const uint16_t id);
extern int auto_report_dev_modify (const uint16_t type, const uint16_t id, const uint16_t interval);
extern int auto_rp_init ();

#endif

