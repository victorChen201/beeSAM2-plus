//#define	__M_FILE_NAME_common_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_common_H__
#define	__M_FILE_NAME_common_H__

#include "pf_linux.h"

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ring_buffer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define M_DBG_INFO
//********************************
//#define M_FOR_beeSAM2_plus
//#define M_FOR_beeSAM3
//#define M_FOR_NEWLAND
//#define M_FOR_EGGPLANT
//********************************
#ifndef M_DBG_INFO
//#define M_DBG_BUF_SEND
#endif
#if defined(M_FOR_beeSAM2_plus) || defined(M_FOR_beeSAM3)
#define M_SYS_IO_CTRL
#endif
#ifndef M_FOR_NEWLAND
#define M_SSDP_FEATUE
#endif
#ifndef M_FOR_NEWLAND
#define M_REMOTE_COMMUNICATION
#endif
#if !defined(M_FOR_beeSAM2_plus) && !defined(M_FOR_beeSAM3)
#define M_UZIP_USB_MODULE
#endif
//#define M_LOCAL_REPORT_BUF_SEND

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#include "dbg_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define M_OFFSET(_Struct, _Memb)    	((size_t)(&(((_Struct*)0)->_Memb)))
#define M_SIZEOF(_Struct, _Memb)    	(sizeof((((_Struct*)0)->_Memb)))

#define MUI_STR_MAC_ADDR_LEN			17		//00:0C:29:9E:AB:F5
#define MUI_STR_IP_ADDR_LEN				15		//192.168.100.255

//#define AT_PORT_UNQUEUE_QUERY		//不使用缓存发送方式查询AT命令

#define MUI_HTTP_METHOD_VOID			0		//无效
#define MUI_HTTP_METHOD_POST			1		//POST
#define MUI_HTTP_METHOD_GET				2		//GET
#define MUI_HTTP_METHOD_PUT				3		//PUT
#define MUI_HTTP_METHOD_DELETE			4		//DELETE
#define MUI_HTTP_METHOD_EX_ADD			5		//ADD
#define MUI_HTTP_METHOD_EX_DEL			6		//DEL
#define MUI_HTTP_METHOD_EX_MODIFY		7		//MODIFY
#define MUI_HTTP_METHOD_EX_CALL			8		//CALL
#define MUI_HTTP_METHOD_EX_ANSWER		9		//ANSWER

#define AT_PORT_ERR_NUM_BASE			-300
#define AT_PORT_ERR_INIT_FAIL			(AT_PORT_ERR_NUM_BASE-1)		//初始化失败
#define AT_PORT_ERR_AT_QUERY_OT			(AT_PORT_ERR_NUM_BASE-2)		//AT 查询超时
#define AT_PORT_ERR_AT_NO_ANSWER		(AT_PORT_ERR_NUM_BASE-3)		//AT 查询无应答
#define AT_PORT_ERR_UNKNOW_EVENT		(AT_PORT_ERR_NUM_BASE-4)		//未知事件
#define AT_PORT_ERR_SEM_FAIL			(AT_PORT_ERR_NUM_BASE-5)		//信号量获取失败
#define AT_PORT_ERR_UNEXPECT_ANSWER		(AT_PORT_ERR_NUM_BASE-6)		//非预期的应答
#define AT_PORT_ERR_ATPORTFD_FAIL		(AT_PORT_ERR_NUM_BASE-7)
#define AT_PORT_ERR_FORMAT_ERR			(AT_PORT_ERR_NUM_BASE-8)		//格式错误
#define AT_PORT_ERR_UNKNOW_ANSWER		(AT_PORT_ERR_NUM_BASE-9)

#define AT_PORT_ERR_UZIP_BUSY				(AT_PORT_ERR_NUM_BASE-10)		//uzip复位中
#define AT_PORT_UZIP_ERR				(AT_PORT_ERR_NUM_BASE-10)		//uZip内部错误号起始编号
//后面要预留256个错误码给uZip错误码来映射

#define DEV_IF_ERR_NUM_BASE				-600
#define DEV_IF_ERR_DEV_TYPE_WRONG		(DEV_IF_ERR_NUM_BASE-1)
#define DEV_IF_ERR_NOT_WRITABLE			(DEV_IF_ERR_NUM_BASE-2)
#define DEV_IF_ERR_BUF_LESS				(DEV_IF_ERR_NUM_BASE-3)
#define DEV_IF_ERR_PARA_WRONG			(DEV_IF_ERR_NUM_BASE-4)
#define DEV_IF_ERR_LOW_LEVL				(DEV_IF_ERR_NUM_BASE-5)
#define DEV_IF_ERR_NO_ID				(DEV_IF_ERR_NUM_BASE-6)
#define DEV_IF_ERR_NO_FIND_MAC			(DEV_IF_ERR_NUM_BASE-7)
#define DEV_IF_ERR_NO_FIND_NWK_ADDR		(DEV_IF_ERR_NUM_BASE-8)
#define DEV_IF_ERR_UNSUPPORT			(DEV_IF_ERR_NUM_BASE-9)
#define DEV_IF_ERR_UNREACHABLE			(DEV_IF_ERR_NUM_BASE-10)

#define FILE_S_ERR_NUM_BASE				-1000
#define FILE_S_UNKONW_ERR				( FILE_S_ERR_NUM_BASE - 1 )
#define FILE_S_PRM_ERR					( FILE_S_ERR_NUM_BASE - 2 )
#define FILE_S_LOEVEL_ERR				( FILE_S_ERR_NUM_BASE - 3 )
#define FILE_S_ADDR_ERR					( FILE_S_ERR_NUM_BASE - 4 )
#define FILE_S_MEM_ERR					( FILE_S_ERR_NUM_BASE - 5 )
#define FILE_S_NOFIND_ERR				( FILE_S_ERR_NUM_BASE - 6 )
#define FILE_S_OVER_MAX_ID				( FILE_S_ERR_NUM_BASE - 7 )
#define FILE_S_NO_DEV                   ( FILE_S_ERR_NUM_BASE - 8 )
#define FILE_S_BUF_ERR                  ( FILE_S_ERR_NUM_BASE - 9 )
#define FILE_S_ALREADY_EXSIT            ( FILE_S_ERR_NUM_BASE - 15 )
#define FILE_S_INIT_ERR                 ( FILE_S_ERR_NUM_BASE - 16 )
#define FILE_S_ERR_SEM_FAIL             ( FILE_S_ERR_NUM_BASE - 17 )
#define FILE_S_ERR_SEEK_FAIL            ( FILE_S_ERR_NUM_BASE - 18 )
#define FILE_S_ERR_DEV_NUM_ULTRALIMIT	( FILE_S_ERR_NUM_BASE - 19 )
#define FILE_S_ERR_DIR_NOT_EMPTY		( FILE_S_ERR_NUM_BASE - 20 )
#define FILE_S_ERR_DATA_TYPE_ERR		( FILE_S_ERR_NUM_BASE - 21 )
#define FILE_S_ERR_DATABASE_MODE_ERR	( FILE_S_ERR_NUM_BASE - 22 )
#define FILE_S_ERR_DIR_NOT_EXIST		( FILE_S_ERR_NUM_BASE - 23 )
#define FILE_S_ERR_FACTORY_DATA_WRONG	( FILE_S_ERR_NUM_BASE - 24 )
#define FILE_S_ERR_UNSUPPORT			( FILE_S_ERR_NUM_BASE - 25 )

#define USR_NO_EXSIT                    ( FILE_S_ERR_NUM_BASE - 10 )
#define UDR_ALREAFY_EXSIT               ( FILE_S_ERR_NUM_BASE - 11 )
#define USR_FULL                        ( FILE_S_ERR_NUM_BASE - 12 )

#define USER_ERR_NO_FIND				( FILE_S_ERR_NUM_BASE - 10 )
#define USER_ERR_ALREADY_EXIST			( FILE_S_ERR_NUM_BASE - 11 )
#define USER_ERR_RAM_ITEM_INVALID		( FILE_S_ERR_NUM_BASE - 12 )
#define USER_ERR_ROM_ACCESS_FAIL		( FILE_S_ERR_NUM_BASE - 13 )
#define USER_ERR_NUM_ULTRALIMIT			( FILE_S_ERR_NUM_BASE - 14 )
#define USER_ERR_UNKONW_ERR				( FILE_S_ERR_NUM_BASE - 15 )

#define LOG_EXPR_ERR                    ( FILE_S_ERR_NUM_BASE - 13 )
#define LOG_BUF_ERR                     ( FILE_S_ERR_NUM_BASE - 14 )

#define ZIGBEE_MANAGE_ERR_NUM_BASE		-1100
#define ZIGBEE_MANAGE_ERR_INIT_FAIL		(ZIGBEE_MANAGE_ERR_NUM_BASE-1)		//初始化失败
#define ZIGBEE_MANAGE_RESET_ERROR		( ZIGBEE_MANAGE_ERR_NUM_BASE - 2 )
#define ZIGBEE_MANAGE_PRINT_BUF_ERROR	( ZIGBEE_MANAGE_ERR_NUM_BASE - 3 )
#define ZIGBEE_MANAGE_EP_NUM_OVERFLOW	( ZIGBEE_MANAGE_ERR_NUM_BASE - 4 )
#define ZIGBEE_MANAGE_UNRECGN_DEC		( ZIGBEE_MANAGE_ERR_NUM_BASE - 5 )		//无法识别的设备
#define ZIGBEE_MANAGE_EVENT_ERROR		( ZIGBEE_MANAGE_ERR_NUM_BASE - 6 )		//事件错误
#define ZIGBEE_MANAGE_BUF_ERROR			( ZIGBEE_MANAGE_ERR_NUM_BASE - 7 )		//缓存错误

#define GROUP_ERROR_NUM_BASE			-1200
#define GROUP_READ_ERROR				( GROUP_ERROR_NUM_BASE - 1 )
#define GROUP_WRITE_ERROR				( GROUP_ERROR_NUM_BASE - 2 )
#define GROUP_DEV_TYPE_ERROR			( GROUP_ERROR_NUM_BASE - 3 )
#define GROUP_ZIGBEE_ERROR				( GROUP_ERROR_NUM_BASE - 4 )
#define GROUP_DELETE_ERROR				( GROUP_ERROR_NUM_BASE - 5 )
#define GROUP_ADD_ERROR					( GROUP_ERROR_NUM_BASE - 6 )
#define GROUP_READ_ZB_ATTR_ERROR		( GROUP_ERROR_NUM_BASE - 7 )
#define GROUP_AT_ANSWER_VERIFY_ERROR	( GROUP_ERROR_NUM_BASE - 8 )
#define GROUP_GET_ID_LIST_ERROR			( GROUP_ERROR_NUM_BASE - 9 )
#define GROUP_ASSIGN_ID_ERROR			( GROUP_ERROR_NUM_BASE - 10 )
#define GROUP_DEV_ID_ERROR				( GROUP_ERROR_NUM_BASE - 11 )
#define GROUP_DEV_NUM_ERROR				( GROUP_ERROR_NUM_BASE - 12 )

#define SET_ERROR_NUM_BASE			-1300
#define SET_READ_ARGUMENT_ERROR		( SET_ERROR_NUM_BASE - 1 )
#define SET_ASSIGN_ID_ERROR			( SET_ERROR_NUM_BASE - 2 )

#define SCHEDULE_ERROR_NUM_BASE			-1400
#define SCHEDULE_ASSIGN_ID_ERROR		( SCHEDULE_ERROR_NUM_BASE - 1)

#define RULES_ERROR_BASE				-1500
#define RULES_ASSIGN_ID_ERROR			( RULES_ERROR_BASE -1 )

#define API_DEAL_ERR_BASE				-1600
#define API_DEAL_ERR_INVALID_MSG		(API_DEAL_ERR_BASE-1)		//无效报文
#define API_DEAL_ERR_INVALID_USER		(API_DEAL_ERR_BASE-2)		//无效用户

#define JSON_DEAL_ERR_BASE				-1700
#define JSON_DEAL_ERR_MEM_FAULT			(JSON_DEAL_ERR_BASE-1)		//内存失败
#define JSON_DEAL_ERR_PRINT_FAULT		(JSON_DEAL_ERR_BASE-2)		//内存失败
#define JSON_DEAL_ERR_PARSE_FAIL		(JSON_DEAL_ERR_BASE-3)		//内存失败
#define JSON_DEAL_ERR_DATA_WRONG		(JSON_DEAL_ERR_BASE-4)		//内存失败

#define SCENE_ERROR_NUM_BASE			-1800
#define SCENE_ASSIGN_ID_ERROR			(SCENE_ERROR_NUM_BASE-1)

#define AT_PORT_UZIP_ERR_BASE			-2147418112		//uZip内部错误号起始编号
//后面要预留65535个错误码给uZip错误码来映射
#define AT_PORT_UZIP_ERR_E100			( AT_PORT_UZIP_ERR_BASE - 57600 )
#define AT_PORT_UZIP_ERR_D000			( AT_PORT_UZIP_ERR_BASE - 53248 )
//////////////////////////////////////////////////////////////////////////////////

#define MSTR_IF_NAME					"eth0"
//#define MSTR_IF_NAME					"br-lan"

#define MSTR_AT_PORT_DEV				"/dev/ttyS1"
//#define MSTR_AT_PORT_DEV				"/dev/ttyUSB0"

#define MSTR_FILE_DEFT_PATH				"./"

#define MSTR_SOCKET_LISTEN_IP			"127.0.0.1"
//#define MUI_SOCKET_LISTEN_PORT		7890
#define MUI_SOCKET_LISTEN_PORT			80

#define MSTR_SOCKET_REPORT_IP			"127.0.0.1"
#define MUI_SOCKET_REPORT_PORT			7891

#define  MSTR_WEB_SOCKET_SERVER_URI 	"ws://192.168.1.103:56788/websocket"
//#define  MSTR_WEB_SOCKET_SERVER_URI 	"ws://113.108.118.106:56788/websocket"

//#define MUI_CLOUD_ID_DB_SERVER		0x0000
#define MUI_CLOUD_ID_DB_SERVER			111111111
#define MUI_ADD_DEV_DURATION_SEC		30


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	char		achNetPortDev[64];
	char		achAtPortDev[64];
	char		achFilePath[128];
	char		achListenIp[16];
	uint16_t	ui16ListenPort;
	char		achReportIp[16];
	uint16_t	ui16ReportPort;
	char		achDbgIp[16];

	char netmask[16];
	char gateway[16];
	char mac[32];
} S_RunPara;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern uint16_t crc16_cal_continue (const void *pvBuf, size_t tLen, uint16_t ui16Tmp);
extern uint16_t crc16_cal (const void *pvBuf, size_t tLen);
extern uint32_t crc32_cal_continue (const void *pvBuf, size_t tLen, uint32_t ui32Tmp);
extern uint32_t crc32_cal (const void *pvBuf, size_t tLen);
extern char* strnstr (const char *pchStr, const char *pchFind, size_t tSLen);
extern void* mem_search (const void *pvBuf, size_t tBufLen, const void *pvSub, size_t tSubLen);
extern void* mem_header_chk (const void *pvBuf, size_t tBufLen, const void *pvHeader, size_t tHeaderLen, size_t *ptLen);
extern void* mem_tail_chk (const void *pvBuf, size_t tBufLen, const void *pvTail, size_t tTailLen, size_t *ptLen);
extern int sub_str_get_util_rn (const char *pchStr, char *pchSubStr, size_t tStrLen);

extern const char* get_sw_version (char *pchVer, size_t tLen);
extern uint32_t get_database_version (void);

extern int sem_wait_ms (sem_t *ptSem, uint32_t ui32WaitMs);

extern int get_peer_mac (int iSocketFd, char* pchBuf, size_t tSize);


extern int sys_reboot (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_common_H__".
