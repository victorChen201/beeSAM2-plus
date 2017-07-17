//#define	__M_FILE_NAME_multicast_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_multicast_H__
#define	__M_FILE_NAME_multicast_H__

#include "common.h"
#include "net_manage.h"
#include "sys_time.h"
//#include "api_deal.h"

#define MSTR_MULTICAST_DEST_IP			"239.200.255.254"
#define MUI_MULTICAST_DEST_PORT			2900
#define MUI_MULTICAST_SRC_PORT			12900

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern	int multicast_init (void);			//MULTICAST初始化函数.
extern	int multicast_send (const void *pvBuf, size_t tLen);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_multicast_H__".
