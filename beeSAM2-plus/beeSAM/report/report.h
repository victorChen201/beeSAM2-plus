/*
 * report.h
 *
 *  Created on: Feb 02, 2015
 *      Author: songyongheng
 */
//#define	__M_FILE_NAME_remote_api_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_report_H__
#define	__M_FILE_NAME_report_H__

#include "common.h"
#include "cJSON.h"
#include "remote_report.h"
#include "local_report.h"

extern int msg_ring_buf_rd (S_RingBuffer *psRingBuf, void *pvBuf, size_t tSize, size_t *ptLen);
extern int msg_ring_buf_wr (S_RingBuffer *psRingBuf, const void *pvMsg, size_t tLen);
extern int report_msg_send (const uint8_t *pbuf, const size_t tlen, PFun_RemoteReportCallback pFunAnswer, PFun_RemoteReportCallback pFunError, uint32_t waitms);
extern int send_report_with_json (uint8_t ui8Method, uint16_t ui16Type, uint16_t ui16Id, cJSON *psBody);
extern int send_dev_report (uint8_t ui8Method, uint16_t ui16Type, uint16_t ui16Id, uint8_t ui8Mode);
extern int send_dev_state_report (uint8_t ui8Method, uint16_t ui16Type, uint16_t ui16Id, uint8_t ui8State);

#endif	//end of "#ifndef __M_FILE_NAME_remote_api_H__".

