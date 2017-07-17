//#define	__M_FILE_NAME_at_port_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_at_port_H__
#define	__M_FILE_NAME_at_port_H__

#include "common.h"
#include "ring_buffer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_AT_PORT_CMD_LEN_MAX					134
#define MUI_AT_PORT_ANSWER_LEN_MAX				100
#define MUI_AT_PORT_ISR_BUF_SIZE				98									//!!!必须保证此数大于等于最长的AT应答行
#define MUI_AT_PORT_EVENT_HEAD_LEN				(sizeof(uint8_t)+sizeof(uint8_t))	// | Event Type(1 Byte) | Event Len(1 Byte) |

#define MUI_AT_QUERY_INTERVAL_MIN				100		//两个AT命令之间最短要间隔毫秒数
#define MUI_AT_QUERY_WAIT_MAX					15000	//AT命令最长最多等待毫秒数(仅仅JNNWK命令)
#define MUI_AT_QUERY_WAIT_NORMAL_MAX			11000	//每个普通AT命令最长最多等待毫秒数

#define AT_CMD_TLSCAN_S							"AT$TLSCAN"
#define AT_CMD_TLIDEN_S							"AT$TLIDEN"
#define AT_CMD_TLJOIN_S							"AT$TLJOIN"
#define AT_CMD_MATCH_S							"AT$MATCH"
#define AT_CMD_IDEN_S							"AT$IDEN"
#define AT_CMD_TLR_S							"AT$TLR"
#define AT_CMD_SESEC_S							"AT$SESEC"
#define AT_CMD_OFF_S							"AT$OFF"
#define AT_CMD_ON_S								"AT$ON"
#define AT_CMD_ATTRRR_S							"AT$ATTRRR"
#define AT_CMD_TOGGLE_S							"AT$TOGGLE"
#define AT_CMD_MVTOLV_S							"AT$MVTOLV"
#define AT_CMD_MVTOCT_S							"AT$MVTOCT"
#define AT_CMD_MVTOH_S							"AT$MVTOH"
#define AT_CMD_MVTOS_S							"AT$MVTOS"
#define AT_CMD_MVTOS_HS							"AT$MVTOHS"
#define AT_CMD_MVTOS_XY							"AT$MVTOC"
#define AT_CMD_ADDGR_S							"AT$ADDGR"
#define AT_CMD_GETGRM_S							"AT$GETGRM"
#define AT_CMD_VIEWGR_S							"AT$VIEWGR"
#define AT_CMD_REMGR_S							"AT$REMGR"
#define AT_CMD_REMAGR_S							"AT$REMAGR"
#define AT_CMD_STWRN_S							"AT$STWRN"
#define AT_CMD_LVREQ_S							"AT$LVREQ"
#define AT_CMD_BIND_S							"AT$BIND"
#define AT_CMD_ZERESP_S							"AT$ZERESP"
#define AT_CMD_SIMPDESC_S						"AT$SIMPDESC"
#define AT_CMD_ACEP_S							"AT$ACEP"
#define AT_CMD_JNNWK_S							"AT$JNNWK"
#define AT_CMD_JNEN_S							"AT$JNEN"
#define AT_CMD_DNS_S							"AT$DNS"
#define AT_CMD_ATI0_S							"ATI0"
#define AT_CMD_ATZ_S							"ATZ"
#define AT_CMD_START_S							"AT$START"
#define AT_CMD_RST_S							"AT$R"
#define AT_CMD_R_MAC_UZIP_S						"AT$MACLR"
#define AT_CMD_W_MAC_UZIP_S						"AT$MACLW"
#define AT_CMD_R_SN_UZIP_S						"AT$SNLR"
#define AT_CMD_W_SN_UZIP_S						"AT$SNLW"
#define AT_CMD_SCAN_S							"AT$SCAN"
#define AT_CMD_STORESC_S						"AT$STORESC"
#define AT_CMD_REMSC_S							"AT$REMSC"
#define AT_CMD_RECALLSC_S						"AT$RECALLSC"


#define MSTR_AT_FIN_FLG							"\r\n"

#define MSTR_AT_HEADER_OK						"OK"
#define MSTR_AT_HEADER_ERROR					"ERROR"

#define MSTR_AT_HEADER_ACEP						"$ACEP:"
#define MSTR_AT_HEADER_SIMPDESC					"$SIMPDESC:"
//#define MSTR_AT_HEADER_SIMPDESC					"$SIMPLEDESC:"
#define MSTR_AT_HEADER_ATTRRR					"$ATTRRR:"

#define MSTR_AT_HEADER_READY					"$READY"
#define MSTR_AT_HEADER_NEWDEVU					"$NEWDEVU:"
#define MSTR_AT_HEADER_REPORTU					"$REPORTU:"
#define MSTR_AT_HEADER_ZEREQ					"$ZENROLLREQ:"
#define MSTR_AT_HEADER_ZSCHN					"$ZSCHN:"
#define MSTR_AT_HEADER_DEVLEFT					"$DEVLEFT:"
#define MSTR_AT_HEADER_IMGREQ					"$IMGREQ:"
#define MSTR_AT_HEADER_IMGRDY					"$IMGRDY:"
#define MSTR_AT_HEADER_DBG						"$DBG:"
#define MSTR_AT_HEADER_TESTMODE					"$TESTMODE:"

#define MUI_AT_EVENT_TYPE_READY					0
#define MUI_AT_EVENT_TYPE_NEWDEVU				1
#define MUI_AT_EVENT_TYPE_REPORTU				2
#define MUI_AT_EVENT_TYPE_ZEREQ					3
#define MUI_AT_EVENT_TYPE_ZSCHN					4
#define MUI_AT_EVENT_TYPE_DEVLEFT				5
#define MUI_AT_EVENT_TYPE_IMGREQ				6
#define MUI_AT_EVENT_TYPE_IMGRDY				7
#define MUI_AT_EVENT_TYPE_DBG					8
#define MUI_AT_EVENT_TYPE_TESTMODE				9
#define MUI_AT_EVENT_TYPE_UZIPUPGRADE			10

#define MUI_AT_DEV_EP_NUM_MAX					4
#define MUI_ZIGBEE_GROUP_100_ID					100	// for all lights, be same to Philips
#define MUI_SCENE_GROUP_200_ID					200	// for all lights, be same to Philips

#define MUI_AT_ANSWER_LEN_ATZ					256
#define MUI_AT_ANSWER_LEN_ATI0					100
#define MUI_AT_ANSWER_LEN_DNS					100
#define MUI_AT_ANSWER_LEN_JNNWK					100
#define MUI_AT_ANSWER_LEN_ACEP					50
#define MUI_AT_ANSWER_LEN_SIMPLEDESC			128
#define MUI_AT_ANSWER_LEN_BIND					50
#define MUI_AT_ANSWER_LEN_ADDGR					50
#define MUI_AT_ANSWER_LEN_REMGR					50
#define MUI_AT_ANSWER_LEN_LVREQ					50
#define MUI_AT_ANSWER_LEN_MACLR					100
#define MUI_AT_ANSWER_LEN_SNLR					100

#define MUI_AT_ANSWER_LEN_DFT					50
#define MUI_AT_ANSWER_LEN_REACHABLE				50
#define MUI_AT_ANSWER_LEN_ON					50
#define MUI_AT_ANSWER_LEN_BRI					50
#define MUI_AT_ANSWER_LEN_SAT					50
#define MUI_AT_ANSWER_LEN_HUE					50
#define MUI_AT_ANSWER_LEN_CT					50
#define MUI_AT_ANSWER_LEN_XY					50
#define MUI_AT_ANSWER_LEN_COLOR_MODE			50
#define MUI_AT_ANSWER_LEN_ALERT					50
#define MUI_AT_ANSWER_LEN_EFFECT				50
#define MUI_AT_ANSWER_LEN_LEVEL					50
#define MUI_AT_ANSWER_LEN_COUNT					50
#define MUI_AT_ANSWER_LEN_TEMP					50
#define MUI_AT_ANSWER_LEN_HUM					50
#define MUI_AT_ANSWER_LEN_CONSUMPTION			50
#define MUI_AT_ANSWER_LEN_WARN_MODE				50
#define MUI_AT_ANSWER_LEN_DURATION				50
#define MUI_AT_ANSWER_LEN_WARN_CFG				50
#define MUI_AT_ANSWER_LEN_ACTION				50
#define MUI_AT_ANSWER_LEN_POWER					50

#define MUI_AT_CMD_BUF_SIZE						1024

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#include "at_port_linux.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_at_port_H__".
