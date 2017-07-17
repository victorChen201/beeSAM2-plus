/*
 * local_report.h
 *
 *  Created on: July 28, 2014
 *      Author: songyongheng
 */
//#define	__M_FILE_NAME_local_report_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_local_report_H__
#define	__M_FILE_NAME_local_report_H__

#include "common.h"
#include "sys_time.h"
#include "config.h"
#include "report.h"

extern int local_report_init (void);
extern int local_report_send (const void *pvMsg, size_t tLen);

#endif
