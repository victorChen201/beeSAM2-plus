
#include "auto_report.h"
#include "common.h"
#include "data_common.h"
#include "dev_data.h"
#include "api_deal.h"
#include "report.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define AUTO_REPORT_DBG

#ifndef AUTO_REPORT_PUTC
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_PUTC(Char)			PUTC(Char)
#else
#define AUTO_REPORT_PUTC(Char)
#endif
#endif

#ifndef AUTO_REPORT_PUTS
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define AUTO_REPORT_PUTS(Str, Len)
#endif
#endif

#ifndef AUTO_REPORT_PUTASCC
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_PUTASCC(Char)			PUTASCC(Char)
#else
#define AUTO_REPORT_PUTASCC(Char)
#endif
#endif

#ifndef AUTO_REPORT_PUTASCS
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define AUTO_REPORT_PUTASCS(Str, Len)
#endif
#endif

#ifndef AUTO_REPORT_PRINT
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define AUTO_REPORT_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef AUTO_REPORT_DYN_PUTC
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_AUTO_REPORT|Dbg), Char)
#else
#define AUTO_REPORT_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef AUTO_REPORT_DYN_PUTS
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_AUTO_REPORT|Dbg), Str, Len)
#else
#define AUTO_REPORT_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef AUTO_REPORT_DYN_PUTASCC
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_AUTO_REPORT|Dbg), Char)
#else
#define AUTO_REPORT_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef AUTO_REPORT_DYN_PUTASCS
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_AUTO_REPORT|Dbg), Str, Len)
#else
#define AUTO_REPORT_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef AUTO_REPORT_DYN_PRINT
#ifdef AUTO_REPORT_DBG
#define AUTO_REPORT_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_AUTO_REPORT|Dbg), Fmt, ##Arg)
#else
#define AUTO_REPORT_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif


#define	MUI_AUTO_REPORT_BUF_LEN		2048

typedef struct
{
	uint8_t type;
	uint8_t id;
	uint16_t interval;
} S_AutoReportDev;



static S_AutoReportDev s_asAutoReportDev[MUI_LOGIC_DEV_NUM_MAX];
static uint16_t g_report_dev_num = 0;
static pthread_t s_auto_reportDealThread; /* 线程ID*/

static void* auto_report_deal_task (void *pArg);

int auto_report_dev_add (const uint16_t type, const uint16_t id, const uint16_t interval)
{
	ASSERT(type > 0);
	ASSERT(id >= 0);
	ASSERT(interval > 0);
	s_asAutoReportDev[g_report_dev_num].type = type;
	s_asAutoReportDev[g_report_dev_num].id = id;
	s_asAutoReportDev[g_report_dev_num].interval = interval;
	return 1;
}

int auto_report_dev_del (const uint16_t type, const uint16_t id)
{

	int i = 0;
	for (i; i < g_report_dev_num; i++)
	{
		if (s_asAutoReportDev[g_report_dev_num].type == type && s_asAutoReportDev[g_report_dev_num].id == id)
		{
			for (i; i < g_report_dev_num; i++)
			{
				s_asAutoReportDev[i].type = s_asAutoReportDev[i + 1].type;
				s_asAutoReportDev[i].id = s_asAutoReportDev[i + 1].id;
				s_asAutoReportDev[i].interval = s_asAutoReportDev[i + 1].interval;
			}
			return 1;
		}
	}
	return -1;
}

int auto_report_dev_modify (const uint16_t type, const uint16_t id, const uint16_t interval)
{
	int i = 0;
	for (i; i < g_report_dev_num; i++)
	{
		if (s_asAutoReportDev[g_report_dev_num].type == type && s_asAutoReportDev[g_report_dev_num].id == id)
		{
			s_asAutoReportDev[i].interval = interval;
			return 1;
		}
	}
	return -1;
}

static void auto_report_dev_init (void)
{
	uint32_t aui32Idx[MUI_LOGIC_DEV_NUM_MAX];
	uint16_t ui16AutoRpt;
	int iRet, iTotal, i;

	iTotal = get_dev_idx_list(aui32Idx, sizeof(aui32Idx)/sizeof(aui32Idx[0]));
	for (i=0; i<iTotal; i++)
	{
		iRet = read_dev_attr_autorpt(M_GET_TYPE_FROM_IDX(aui32Idx[i]), M_GET_ID_FROM_IDX(aui32Idx[i]), &ui16AutoRpt);
		if ((iRet>0) && (ui16AutoRpt>0))
		{
			auto_report_dev_add(M_GET_TYPE_FROM_IDX(aui32Idx[i]), M_GET_ID_FROM_IDX(aui32Idx[i]), ui16AutoRpt);
		}
	}
}

//-------------------------------------------------------------------------------------------------*
/*	自动上报事件处理任务函数
 ;	(功能)
 ;	(参数)
 ;		pArg:	传给函数运行的参数
 ;	(返回)
 ;		NULL
 ;	(注意)
 */
static void* auto_report_deal_task (void *pArg)
{
	int i = 0;
	int iRet = 0;
	uint32_t tick = 1;
	uint8_t report_buf[MUI_AUTO_REPORT_BUF_LEN] = "";

	while (1)
	{
		for (i = 0; i < g_report_dev_num; i++)
		{
			if (tick % s_asAutoReportDev[i].interval == 0)
			{
				send_dev_report(MUI_HTTP_METHOD_EX_MODIFY, s_asAutoReportDev[i].type, s_asAutoReportDev[i].id, MUI_API_DEV_DATA_ASM_JSON_CFG_STATE|MUI_API_DEV_DATA_ASM_JSON_CFG_API_ATTR);
			}
		}
		if (0 == (tick % 300))
		{
			dev_data_state_sync();
		}
		tick++;
		msleep(1000);
	}

	return NULL;
}

int auto_rp_init ()
{
	int iRet;
	auto_report_dev_init();
	iRet = pthread_create(&s_auto_reportDealThread, NULL, auto_report_deal_task, NULL);
	if (iRet < 0)
	{
		return -1;
	}
	return 1;
}

