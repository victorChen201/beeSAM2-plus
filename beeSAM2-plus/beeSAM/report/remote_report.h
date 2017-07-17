/*
 * remote_report.h
 *
 *  Created on: July 28, 2014
 *      Author: songyongheng
 */
//#define	__M_FILE_NAME_remote_api_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_remote_api_H__
#define	__M_FILE_NAME_remote_api_H__

#include "common.h"

typedef void (*PFun_RemoteReportCallback) (void *pBuf, size_t tlen);

#ifdef M_REMOTE_COMMUNICATION
extern int remote_api_init (void);
extern int remote_msg_receive (const void *pvMsg, size_t tLen);
extern int remote_api_answer (const void *pvMsg, size_t tLen, uint32_t ui32DestId, uint32_t ui32Sqn);
extern int remote_report_send (const void *pvMsg, size_t tLen, PFun_RemoteReportCallback pfunAnswer, PFun_RemoteReportCallback pfunError, uint32_t ui32WaitMs);
#else
#define remote_api_init()															(0)
#define remote_msg_receive(_pvMsg, _tLen)											(0)
#define remote_api_answer(_pvMsg, _tLen, _ui32DestId, _ui32Sqn)						(0)
#define remote_report_send(_pvMsg, _tLen, _pfunAnswer, _pfunError, _ui32WaitMs)		(0)
#endif

#endif
