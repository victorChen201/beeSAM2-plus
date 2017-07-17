#include "api_deal.h"
#include "json_deal.h"
#include "zigbee_manage.h"
#include "user.h"
#include "rule.h"
#include "ftp_update.h"
#include "misc.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define API_DEAL_DBG

#ifndef API_DEAL_PUTC
#ifdef API_DEAL_DBG
#define API_DEAL_PUTC(Char)			PUTC(Char)
#else
#define API_DEAL_PUTC(Char)
#endif
#endif

#ifndef API_DEAL_PUTS
#ifdef API_DEAL_DBG
#define API_DEAL_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define API_DEAL_PUTS(Str, Len)
#endif
#endif

#ifndef API_DEAL_PUTASCC
#ifdef API_DEAL_DBG
#define API_DEAL_PUTASCC(Char)			PUTASCC(Char)
#else
#define API_DEAL_PUTASCC(Char)
#endif
#endif

#ifndef API_DEAL_PUTASCS
#ifdef API_DEAL_DBG
#define API_DEAL_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define API_DEAL_PUTASCS(Str, Len)
#endif
#endif

#ifndef API_DEAL_PRINT
#ifdef API_DEAL_DBG
#define API_DEAL_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define API_DEAL_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef API_DEAL_DYN_PUTS
#ifdef API_DEAL_DBG
#define API_DEAL_DYN_PUTS(Dbg, Str, Len)	DYN_PUTS((M_DBG_MODULE_API_DEAL|Dbg), Str, Len)
#else
#define API_DEAL_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef API_DEAL_DYN_PUTASCC
#ifdef API_DEAL_DBG
#define API_DEAL_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_API_DEAL|Dbg), Char)
#else
#define API_DEAL_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef API_DEAL_DYN_PUTASCS
#ifdef API_DEAL_DBG
#define API_DEAL_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_API_DEAL|Dbg), Str, Len)
#else
#define API_DEAL_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef API_DEAL_DYN_PRINT
#ifdef API_DEAL_DBG
#define API_DEAL_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_API_DEAL|Dbg), Fmt, ##Arg)
#else
#define API_DEAL_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static const char s_aachHttpMethod[][7] =
{
	"VOID",		//MUI_HTTP_METHOD_VOID
	"POST",		//MUI_HTTP_METHOD_POST
	"GET",		//MUI_HTTP_METHOD_GET
	"PUT",		//MUI_HTTP_METHOD_PUT
	"DELETE",	//MUI_HTTP_METHOD_DELETE
	"ADD",		//MUI_HTTP_METHOD_EX_ADD
	"DEL",		//MUI_HTTP_METHOD_EX_DEL
	"MODIFY",	//MUI_HTTP_METHOD_EX_MODIFY
	"CALL",		//MUI_HTTP_METHOD_EX_CALL
	"ANSWER"	//MUI_HTTP_METHOD_EX_ANSWER
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static S_ApiAnswerBufCtrl s_sApiAnswerBufCtrl;
static uint32_t s_ui32RemoteDstId, s_ui32RemoteSqn;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int s_api_answer_buf_clean (void);
static int s_api_url_fst_grade_get (const char *pchUrl, size_t tLen, char *pchGradeWord, size_t tSize);
static int s_api_url_parse (S_ApiDealCtrl *psApiDealCtrl);
static int s_api_url_prefix_verify (const char *pchPrefix, size_t tLen);
static int s_api_url_user_verify (const char *pchUsername, size_t tLen, uint32_t *pui32Authority);
static int s_api_parse_user_creat (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_full_state (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_search_new_dev (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_factory_new (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_config (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_config (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_del_user (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_config_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_post_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_new_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_all_devs_by_type (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_dev_attr_state (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_set_dev_attr (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_set_dev_state (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_del_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_devices_deal (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_all_groups (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_group_attr (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_group (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_set_group_attr (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_set_group_state(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_group(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_create_group(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_post_group(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_del_group(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_groups_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_set_set_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_create_set(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_set(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_set(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_post_set(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_del_set(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_set_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_set_schedule_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_create_schedule(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_schedule(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_schedule(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_post_schedule(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_del_schedule(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_schedules_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_set_rules_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_create_rules(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_rules(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_rules(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_post_rules(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_del_rules(const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_rules_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_rtc_sys_time (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_rtc_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_sys (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_sys (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_sys_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_get_debug (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_debug (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_debug_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_put_at_debug (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_at_debug_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_update_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_post_upgrade (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_upgrade_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_remote_get_download (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_remote_post_download (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_remote_download_deal (const S_ApiDealCtrl *psApiDealCtrl);
static int s_api_parse_test_mode_deal (const S_ApiDealCtrl *psApiDealCtrl);

//-------------------------------------------------------------------------------------------------*
/*	Net API 应答缓存清空函数
;	(功能)清空Net API应答缓存.
;	(参数)
;		NULL
;	(返回)
;		int:		 0:成功
;					<0:失败
;	(注意)
*/
static int s_api_answer_buf_clean (void)
{
	//memset(s_sApiAnswerBufCtrl, 0, sizeof(s_sApiAnswerBufCtrl));
	s_sApiAnswerBufCtrl.tLen = 0;

	return 0;
}
//-------------------------------------------------------------------------------------------------*

static int s_api_url_fst_grade_get (const char *pchUrl, size_t tLen, char *pchGradeWord, size_t tSize)
{
	int i = 0;

	//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_url_fst_grade_get]URL(%u) is:", tLen);
	//API_DEAL_DYN_PUTS(M_DBG_LEVEL_DEBUG, pchUrl, tLen);
	//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	if (pchUrl[i] != '/')
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_api_url_fst_grade_get]Can't find '/' in this grade.\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}
	pchGradeWord[i] = pchUrl[i];
	i++;
	if (tLen < tSize)
	{
		tSize = tLen;
	}
	while (i < (int)tSize)
	{
		if (pchUrl[i] == '/')
		{
			return i;
		}
		pchGradeWord[i] = pchUrl[i];
		i++;
	}

	return i;
}
static int s_api_url_parse (S_ApiDealCtrl *psApiDealCtrl)
{
	char achWord[MUI_API_URL_USERNAME_LEN_MAX];
	char *pchUrl;
	uint16_t ui16Len;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl);
	ASSERT(0 < psApiDealCtrl->psConntCtrl->sHttpInfo.ui16UrlLen);

	psApiDealCtrl->sUrlParse.ui8GradeNum    = 0;
	psApiDealCtrl->sUrlParse.aui16Offset[0] = 0;
	pchUrl  = psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl;
	ui16Len = psApiDealCtrl->psConntCtrl->sHttpInfo.ui16UrlLen;

	//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_url_parse]URL(%u) is:", ui16Len);
	//API_DEAL_DYN_PUTS(M_DBG_LEVEL_DEBUG, pchUrl, ui16Len);
	//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	while (ui16Len > 0)
	{
		iRet = s_api_url_fst_grade_get(pchUrl, ui16Len, achWord, sizeof(achWord));
		if (iRet < 0)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_api_url_parse]Call 's_api_url_fst_grade_get()' fail.\r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}

		//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_url_parse]Sub-string(%d) is:", iRet);
		//API_DEAL_DYN_PUTS(M_DBG_LEVEL_DEBUG, achWord, iRet);
		//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

		psApiDealCtrl->sUrlParse.ui8GradeNum++;
		if (MUI_API_URL_GRADE_MAX < psApiDealCtrl->sUrlParse.ui8GradeNum)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_api_url_parse]API grade overflow!\r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}
		psApiDealCtrl->sUrlParse.aui16Offset[psApiDealCtrl->sUrlParse.ui8GradeNum] = psApiDealCtrl->sUrlParse.aui16Offset[psApiDealCtrl->sUrlParse.ui8GradeNum-1] + (uint16_t)iRet;

		//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_url_parse]Grade %u offset is %u\r\n", psApiDealCtrl->sUrlParse.ui8GradeNum, psApiDealCtrl->sUrlParse.aui16Offset[psApiDealCtrl->sUrlParse.ui8GradeNum]);
		//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_url_parse]Grade %u(%u) is:", psApiDealCtrl->sUrlParse.ui8GradeNum-1, psApiDealCtrl->sUrlParse.aui16Offset[psApiDealCtrl->sUrlParse.ui8GradeNum]-psApiDealCtrl->sUrlParse.aui16Offset[psApiDealCtrl->sUrlParse.ui8GradeNum-1]);
		//API_DEAL_DYN_PUTS(M_DBG_LEVEL_DEBUG, &psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[psApiDealCtrl->sUrlParse.ui8GradeNum-1]], psApiDealCtrl->sUrlParse.aui16Offset[psApiDealCtrl->sUrlParse.ui8GradeNum]-psApiDealCtrl->sUrlParse.aui16Offset[psApiDealCtrl->sUrlParse.ui8GradeNum-1]);
		//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

		pchUrl  += iRet;
		ui16Len -= (uint16_t)iRet;
	}

	return psApiDealCtrl->sUrlParse.ui8GradeNum;
}
static int s_api_url_prefix_verify (const char *pchPrefix, size_t tLen)
{
	if (((sizeof(MSTR_API_URL_PREFIX_1)-1)==tLen &&  0==memcmp(pchPrefix, MSTR_API_URL_PREFIX_1, tLen)) ||
		((sizeof(MSTR_API_URL_PREFIX_2)-1)==tLen &&  0==memcmp(pchPrefix, MSTR_API_URL_PREFIX_2, tLen)))
	{
		return (int)tLen;	//返回字段长度
	}

	return API_DEAL_ERR_INVALID_MSG;
}
static int s_api_url_user_verify (const char *pchUsername, size_t tLen, uint32_t *pui32Authority)
{
	int iRet;

	iRet = user_verify(&pchUsername[1], tLen-1);	//忽略开头的'/'
	if (iRet < 0)
	{
		return iRet;
	}

	*pui32Authority = 0;

	return tLen;	//返回字段长度
}
static int s_api_parse_user_creat (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseCreateUser sApiParseCreateUser;
	S_UserData sUserData;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (0 == read_config_info_link_button(NULL))
	{
		return api_answer_link_button_not_pressed(psApiDealCtrl);
	}

	iRet = json_parse_create_user(psApiDealCtrl->psBody, &sApiParseCreateUser);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sUserData.name, sApiParseCreateUser.name, sizeof(sUserData.name));
	memcpy(&sUserData.dev_name, sApiParseCreateUser.dev_name, sizeof(sUserData.dev_name));
	iRet = user_add(&sUserData);
	if (iRet < 0)
	{
		return api_answer_create_user_failed(psApiDealCtrl, &sApiParseCreateUser);
	}

	return api_answer_create_user_succeed(psApiDealCtrl, &sApiParseCreateUser);
}

static int s_api_parse_get_full_state (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseGetFullState sApiParseGetFullState;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_get_full_state(psApiDealCtrl->psBody, &sApiParseGetFullState);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_get_full_state(psApiDealCtrl, &sApiParseGetFullState);
}

static int s_api_parse_search_new_dev (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSearchNewDev sApiParseSearchNewDev;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_search_new_dev(psApiDealCtrl->psBody, &sApiParseSearchNewDev);
	if (iRet < 0)
	{
		return iRet;
	}
	if ((0>zigbee_join_time(0, sApiParseSearchNewDev.ui16Duration)) || (0>sonos_drv_join(sApiParseSearchNewDev.ui16Duration)))
	{
		return api_answer_search_new_dev_failed(psApiDealCtrl, &sApiParseSearchNewDev);
	}

	return api_answer_search_new_dev_succeed(psApiDealCtrl, &sApiParseSearchNewDev);
}

static int s_api_parse_factory_new (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseFactoryNew sApiParseFactoryNew;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_DELETE != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_factory_new(psApiDealCtrl->psBody, &sApiParseFactoryNew);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 > config_factory_new())	//config_factory_new(); sys_reboot();
	{
		return api_answer_factory_new_failed(psApiDealCtrl, &sApiParseFactoryNew);
	}

	return api_answer_factory_new_succeed(psApiDealCtrl, &sApiParseFactoryNew);
}

static int s_api_parse_get_config (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseGetConfig sApiParseGetConfig;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_get_config(psApiDealCtrl->psBody, &sApiParseGetConfig);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_get_config(psApiDealCtrl, &sApiParseGetConfig);
}

static int s_api_parse_put_config (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetConfig sApiParseSetConfig;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_set_config(psApiDealCtrl->psBody, &sApiParseSetConfig);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_set_config(psApiDealCtrl, &sApiParseSetConfig);
}

static int s_api_parse_del_user (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseFactoryNew sApiParseFactoryNew;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_DELETE != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_factory_new(psApiDealCtrl->psBody, &sApiParseFactoryNew);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 > database_del_all())	//config_factory_new(); sys_reboot();
	{
		return api_answer_factory_new_failed(psApiDealCtrl, &sApiParseFactoryNew);
	}

	return api_answer_factory_new_succeed(psApiDealCtrl, &sApiParseFactoryNew);
}

static int s_api_parse_config_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_config(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_config(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_DELETE == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_del_user(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_post_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSearchNewDev sApiParseSearchNewDev;
	int iRet;

	UNUSED(ui16DevType);

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 != psApiDealCtrl->sUrlParse.ui8GradeNum)	//	/ha/<username>/<dev-type>
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_search_new_dev(psApiDealCtrl->psBody, &sApiParseSearchNewDev);
	if (iRet < 0)
	{
		return iRet;
	}
	if ((0>zigbee_join_time(0, sApiParseSearchNewDev.ui16Duration)) || (0>sonos_drv_join(sApiParseSearchNewDev.ui16Duration)))
	{
		return api_answer_search_new_dev_failed(psApiDealCtrl, &sApiParseSearchNewDev);
	}

	return api_answer_search_new_dev_succeed(psApiDealCtrl, &sApiParseSearchNewDev);
}

static int s_api_parse_get_new_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl)
{
	UNUSED(ui16DevType);
	UNUSED(psApiDealCtrl);

	return API_DEAL_ERR_INVALID_MSG;
}
static int s_api_parse_get_all_devs_by_type (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseGetAllDevsByType sApiParseGetAllDevsByType;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_get_all_devs_by_type(psApiDealCtrl->psBody, &sApiParseGetAllDevsByType);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_get_all_devs_by_type(ui16DevType, psApiDealCtrl, &sApiParseGetAllDevsByType);
}
static int s_api_parse_get_dev_attr_state (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseGetDevAttrState sApiParseGetDevAttrState;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_get_dev_attr_state(psApiDealCtrl->psBody, &sApiParseGetDevAttrState);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_get_dev_attr_state(ui16DevType, ui16Id, psApiDealCtrl, &sApiParseGetDevAttrState);
}

static int s_api_parse_set_dev_attr (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetDevAttr sApiParseSetDevAttr;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_set_dev_attr(psApiDealCtrl->psBody, &sApiParseSetDevAttr);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_set_dev_attr(ui16DevType, ui16Id, psApiDealCtrl, &sApiParseSetDevAttr);
}

static int s_api_parse_set_dev_state (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetDevState sApiParseSetDevState;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	sApiParseSetDevState.ui16DevType = ui16DevType;
	sApiParseSetDevState.ui16DevId   = ui16Id;
	iRet = json_parse_set_dev_state(psApiDealCtrl->psBody, &sApiParseSetDevState);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_set_dev_state(ui16DevType, ui16Id, psApiDealCtrl, &sApiParseSetDevState);
}

static int s_api_parse_get_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl)
{
	uint16_t ui16Tmp;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if ((psApiDealCtrl->sUrlParse.ui8GradeNum<3) || (4<psApiDealCtrl->sUrlParse.ui8GradeNum))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		return s_api_parse_get_all_devs_by_type(ui16DevType, psApiDealCtrl);
	}

	ui16Tmp	= psApiDealCtrl->sUrlParse.aui16Offset[4]-psApiDealCtrl->sUrlParse.aui16Offset[3];
	if ((1==sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Tmp)) &&
		((MUI_DEV_ID_MIN<=ui16Tmp)&&(ui16Tmp<=MUI_DEV_ID_MAX)))
	{
		return s_api_parse_get_dev_attr_state(ui16DevType, ui16Tmp, psApiDealCtrl);
	}

	if ((sizeof(MSTR_API_URL_KEY_WORD_NEW)-1)==ui16Tmp &&  0==memcmp(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], MSTR_API_URL_KEY_WORD_NEW, ui16Tmp))
	{
		return s_api_parse_get_new_dev(ui16DevType, psApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_put_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl)
{
	uint16_t ui16Id, ui16Tmp;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if ((psApiDealCtrl->sUrlParse.ui8GradeNum<4) || (5<psApiDealCtrl->sUrlParse.ui8GradeNum))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if ((1!=sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id)) ||
		((ui16Id<MUI_DEV_ID_MIN)||(MUI_DEV_ID_MAX<ui16Id)))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (4 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		return s_api_parse_set_dev_attr(ui16DevType, ui16Id, psApiDealCtrl);
	}

	ui16Tmp = psApiDealCtrl->sUrlParse.aui16Offset[5]-psApiDealCtrl->sUrlParse.aui16Offset[4];
	if ((sizeof(MSTR_API_URL_KEY_WORD_STATE)-1)==ui16Tmp &&  0==memcmp(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[4]], MSTR_API_URL_KEY_WORD_STATE, ui16Tmp))
	{
		return s_api_parse_set_dev_state(ui16DevType, ui16Id, psApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_del_dev (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseDelDev sApiParseDelDev;
	uint16_t ui16Id;
	int iRet;

	UNUSED(ui16DevType);

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_DELETE != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (4 != psApiDealCtrl->sUrlParse.ui8GradeNum)	//	/ha/<username>/<dev-type>/<id>
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if ((1!=sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id)) ||
		((ui16Id<MUI_DEV_ID_MIN)||(MUI_DEV_ID_MAX<ui16Id)))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_del_dev(psApiDealCtrl->psBody, &sApiParseDelDev);
	if (iRet < 0)
	{
		return iRet;
	}

	if (0 > del_dev(ui16DevType, ui16Id))
	{
		return api_answer_del_dev_fail(ui16DevType, ui16Id, psApiDealCtrl, &sApiParseDelDev);
	}

	return api_answer_del_dev_succeed(ui16DevType, ui16Id, psApiDealCtrl, &sApiParseDelDev);
}

static int s_api_parse_devices_deal (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl)
{
	if (MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_post_dev(ui16DevType, psApiDealCtrl);
	}
	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_dev(ui16DevType, psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_dev(ui16DevType, psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_DELETE == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_del_dev(ui16DevType, psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
//	trans_lights_part(psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method, psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl, ((char*)psApiDealCtrl->psConntCtrl->sHttpInfo.pui8Body), psConntCtrl, pvBuf, tSize);
//	trans_sensor_part(psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method, psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl, ((char*)psApiDealCtrl->psConntCtrl->sHttpInfo.pui8Body), psConntCtrl, pvBuf, tSize);
}

static int s_api_parse_get_all_groups (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseGetAllGroups sApiParseGetAllGroups;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_get_all_groups(psApiDealCtrl->psBody, &sApiParseGetAllGroups);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_get_all_groups(psApiDealCtrl, &sApiParseGetAllGroups);
}

static int s_api_parse_get_group_attr (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseGetGroupAttr sApiParseGetGroupAttr;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_get_group_attr(psApiDealCtrl->psBody, &sApiParseGetGroupAttr);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_get_group_attr(ui16Id, psApiDealCtrl, &sApiParseGetGroupAttr);
}

static int s_api_parse_get_group (const S_ApiDealCtrl *psApiDealCtrl)
{
	uint16_t ui16Tmp;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if ((psApiDealCtrl->sUrlParse.ui8GradeNum<3) || (4<psApiDealCtrl->sUrlParse.ui8GradeNum))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		return s_api_parse_get_all_groups(psApiDealCtrl);
	}

	ui16Tmp	= psApiDealCtrl->sUrlParse.aui16Offset[4]-psApiDealCtrl->sUrlParse.aui16Offset[3];
	if ((1==sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Tmp)) &&
		((MUI_DATABASE_ID_START<=ui16Tmp)&&(ui16Tmp<=(MUI_DATABASE_ID_START+MUI_GROUP_DATA_NUM_MAX))))
	{
		return s_api_parse_get_group_attr(ui16Tmp, psApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_set_group_attr (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetGroup sApiParseSetGroupAttr;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_set_group_attr(psApiDealCtrl->psBody, &sApiParseSetGroupAttr);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_set_group_attr(ui16Id, psApiDealCtrl, &sApiParseSetGroupAttr);
}

static int s_api_parse_set_group_state(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_SetGroupState sSetGroupState;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_set_group_state(psApiDealCtrl->psBody, &sSetGroupState);
	if (iRet < 0)
	{
		return iRet;
	}

	return api_answer_set_group_state(ui16Id, psApiDealCtrl, &sSetGroupState);
}

static int s_api_parse_put_group(const S_ApiDealCtrl *psApiDealCtrl)
{
	uint16_t ui16Id, ui16Tmp;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if ((psApiDealCtrl->sUrlParse.ui8GradeNum<4) || (5<psApiDealCtrl->sUrlParse.ui8GradeNum))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (4 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		return s_api_parse_set_group_attr(ui16Id, psApiDealCtrl);
	}

	ui16Tmp = psApiDealCtrl->sUrlParse.aui16Offset[5]-psApiDealCtrl->sUrlParse.aui16Offset[4];
	if ((sizeof(MSTR_API_URL_KEY_WORD_ACTION)-1)==ui16Tmp &&  0==memcmp(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[4]], MSTR_API_URL_KEY_WORD_ACTION, ui16Tmp))
	{
		return s_api_parse_set_group_state(ui16Id, psApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_create_group(const S_ApiDealCtrl *psApiDealCtrl)
{
	S_SetGroupAttr sSetGroupAttr;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_create_group] API_DEAL_ERR_INVALID_MSG!\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_create_group(psApiDealCtrl->psBody, &sSetGroupAttr);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_create_group] Call :json_parse_create_group filed!\r\n");
		return iRet;
	}

	return api_answer_create_group(psApiDealCtrl, &sSetGroupAttr);
}

static int s_api_parse_post_group(const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 3)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_post_group] Call :psApiDealCtrl->sUrlParse.ui8GradeNum != 3!\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}

	return s_api_parse_create_group(psApiDealCtrl);
}

static int s_api_parse_del_group(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);
	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_del_group(ui16Id, &pchBody);
		if (iRet < 0)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_api_parse_del_group] Call:json_assemble_answer_del_group filed! :%d\r\n",iRet);
			return iRet;
		}
		iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
		json_free(pchBody);
		return iRet;
	}else
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	return iRet;

}

static int s_api_parse_groups_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
//	trans_groups_part(psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method, psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl, ((char*)psApiDealCtrl->psConntCtrl->sHttpInfo.pui8Body), psConntCtrl, pvBuf, tSize);
	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_group(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_group(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_post_group(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_DELETE == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_del_group(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_set_set_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetSet sApiParseSetSetAttr;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_set_set_attr(psApiDealCtrl->psBody, &sApiParseSetSetAttr);
	if (iRet < 0)
	{
		return iRet;
	}
	return api_answer_set_set_attr(ui16Id, psApiDealCtrl, &sApiParseSetSetAttr);
}

static int s_api_parse_create_set(const S_ApiDealCtrl *psApiDealCtrl)
{
	S_SetAttr sSetAttr;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_create_group] API_DEAL_ERR_INVALID_MSG!\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}
	iRet = json_parse_create_set(psApiDealCtrl->psBody, &sSetAttr);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_create_group] Call :json_parse_create_group filed!\r\n");
		return iRet;
	}

	return api_answer_create_set(psApiDealCtrl, &sSetAttr);
}

static int s_api_parse_get_set(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 3)   //获取所有set
	{
		iRet = json_assemble_answer_get_all_set(&pchBody);
		if (iRet < 0)
		{
			return iRet;
		}
	}
	else if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)   //获取单个set
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_get_set_attr(ui16Id, &pchBody);
		if (iRet < 0)
		{
			return iRet;
		}
	}
	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);
	return iRet;
}

static int s_api_parse_put_set(const S_ApiDealCtrl *psApiDealCtrl)
{
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 4)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (4 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		return s_api_parse_set_set_attr(ui16Id, psApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_post_set(const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 3)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_post_set] Call :psApiDealCtrl->sUrlParse.ui8GradeNum != 3!\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}

	return s_api_parse_create_set(psApiDealCtrl);
}

static int s_api_parse_del_set(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_del_set] sscanf filed!\r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_del_set(ui16Id, &pchBody);
		if (iRet < 0)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_del_set] Call :json_assemble_answer_del_set filed :%d!\r\n", iRet);
			return iRet;
		}
		iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
		json_free(pchBody);
	}
	return iRet;
}

static int s_api_parse_set_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
//	trans_set_part(psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method, psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl, ((char*)psApiDealCtrl->psConntCtrl->sHttpInfo.pui8Body), psConntCtrl, pvBuf, tSize);
	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_set(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_set(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_post_set(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_DELETE == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_del_set(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_set_scene_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetScene sApiParseSetSceneAttr;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_set_scene_attr(psApiDealCtrl->psBody, &sApiParseSetSceneAttr);
	if (iRet < 0)
	{
		return iRet;
	}
	return api_answer_set_scene_attr(ui16Id, psApiDealCtrl, &sApiParseSetSceneAttr);
}

static int s_api_parse_create_scene(const S_ApiDealCtrl *psApiDealCtrl)
{
	S_SceneDataFalsh sSceneData;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_create_scene] API_DEAL_ERR_INVALID_MSG!\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}
	iRet = json_parse_create_scene(psApiDealCtrl->psBody, &sSceneData);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_create_scene] Call :json_parse_create_scene filed!\r\n");
		return iRet;
	}

	return api_answer_create_scene(psApiDealCtrl, &sSceneData);
}



static int s_api_parse_get_scene(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 3)   //获取所有scene
	{
		iRet = json_assemble_answer_get_all_scene(&pchBody);
		if (iRet < 0)
		{
			return iRet;
		}
	}
	else if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)   //获取单个scene
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_get_scene_attr(ui16Id, &pchBody);
		if (iRet < 0)
		{
			return iRet;
		}
	}
	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);
	return iRet;
}

static int s_api_parse_put_scene(const S_ApiDealCtrl *psApiDealCtrl)
{
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 4)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (4 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		return s_api_parse_set_scene_attr(ui16Id, psApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_post_scene(const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 3)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_post_scene] Call :psApiDealCtrl->sUrlParse.ui8GradeNum != 3!\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}

	return s_api_parse_create_scene(psApiDealCtrl);
}

static int s_api_parse_del_scene(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_del_scene] sscanf filed!\r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_del_scene(ui16Id, &pchBody);
		if (iRet < 0)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_del_scene] Call :json_assemble_answer_del_scene filed :%d!\r\n", iRet);
			return iRet;
		}
		iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
		json_free(pchBody);
	}
	return iRet;
}

static int s_api_parse_scene_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
//	trans_set_part(psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method, psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl, ((char*)psApiDealCtrl->psConntCtrl->sHttpInfo.pui8Body), psConntCtrl, pvBuf, tSize);
	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_scene(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_scene(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_post_scene(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_DELETE == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_del_scene(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}


static int s_api_parse_set_schedule_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetschedule sScheduleApiParse;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_set_schedule_attr(psApiDealCtrl->psBody, &sScheduleApiParse);
	if (iRet < 0)
	{
		return iRet;
	}
	return api_answer_set_schedule_attr(ui16Id, psApiDealCtrl, &sScheduleApiParse);
}

static int s_api_parse_create_schedule(const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSchedule sScheduleInfo;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	iRet = json_parse_create_schedule(psApiDealCtrl->psBody, &sScheduleInfo);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_create_schedule] Call :json_parse_create_rules filed!\r\n");
		return iRet;
	}

	return api_answer_create_schedule(psApiDealCtrl, &sScheduleInfo);
}

static int s_api_parse_get_schedule(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 3)   //获取所有rules
	{
		iRet = json_assemble_answer_get_all_schedule(&pchBody);
		if (iRet < 0)
		{
			return iRet;
		}
	}
	else if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)   //获取单个rules
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_api_parse_get_schedule] get schedule id filed!\r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_get_schedule_attr(ui16Id, &pchBody);
		if (iRet < 0)
		{
			return iRet;
		}
	}
	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);
	return iRet;
}

static int s_api_parse_put_schedule(const S_ApiDealCtrl *psApiDealCtrl)
{
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 4)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (4 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		return s_api_parse_set_schedule_attr(ui16Id, psApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_post_schedule(const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 3)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_post_schedule] Call :psApiDealCtrl->sUrlParse.ui8GradeNum != 3!\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}

	return s_api_parse_create_schedule(psApiDealCtrl);
}

static int s_api_parse_del_schedule(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_del_schedule] sscanf filed!\r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_del_schedule(ui16Id, &pchBody);
		if (iRet < 0)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_del_schedule] Call :json_assemble_answer_del_rules filed :%d!\r\n", iRet);
			return iRet;
		}
		iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
		json_free(pchBody);
	}
	return iRet;

}

static int s_api_parse_schedules_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
//	trans_schedules_part(psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method, psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl, ((char*)psApiDealCtrl->psConntCtrl->sHttpInfo.pui8Body), psConntCtrl, pvBuf, tSize);
	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_schedule(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_schedule(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_post_schedule(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_DELETE == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_del_schedule(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_set_rules_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetRules sRulesAttr;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	iRet = json_parse_set_rules_attr(psApiDealCtrl->psBody, &sRulesAttr);
	if (iRet < 0)
	{
		return iRet;
	}
	return api_answer_set_rules_attr(ui16Id, psApiDealCtrl, &sRulesAttr);
}

static int s_api_parse_create_rules(const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseRules sRulesInfo;
	int iRet=0;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	iRet = json_parse_create_rules(psApiDealCtrl->psBody, &sRulesInfo);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_create_rules] Call :json_parse_create_rules filed!\r\n");
		return iRet;
	}
	return api_answer_create_rules(psApiDealCtrl, &sRulesInfo);
}

static int s_api_parse_get_rules(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 3)   //获取所有rules
	{
		iRet = json_assemble_answer_get_all_rules(&pchBody);
		if (iRet < 0)
		{
			return iRet;
		}
	}
	else if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)   //获取单个rules
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_get_rules_attr(ui16Id, &pchBody);
		if (iRet < 0)
		{
			return iRet;
		}
	}
	iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
	json_free(pchBody);
	return iRet;
}

static int s_api_parse_put_rules(const S_ApiDealCtrl *psApiDealCtrl)
{
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 4)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	if (4 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		return s_api_parse_set_rules_attr(ui16Id, psApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_post_rules(const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (psApiDealCtrl->sUrlParse.ui8GradeNum != 3)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_post_rules] Call :psApiDealCtrl->sUrlParse.ui8GradeNum != 3!\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}

	return s_api_parse_create_rules(psApiDealCtrl);
}

static int s_api_parse_del_rules(const S_ApiDealCtrl *psApiDealCtrl)
{
	char *pchBody;
	int iRet=0;
	uint16_t ui16Id;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (psApiDealCtrl->sUrlParse.ui8GradeNum == 4)
	{
		if (1 != sscanf(&psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl[psApiDealCtrl->sUrlParse.aui16Offset[3]], "/%hu", &ui16Id))
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_del_rules] sscanf filed!\r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = json_assemble_answer_del_rules(ui16Id, &pchBody);
		if (iRet < 0)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_del_rules] Call :json_assemble_answer_del_rules filed :%d!\r\n", iRet);
			return iRet;
		}
		iRet = api_answer_segment_send(psApiDealCtrl->psConntCtrl, pchBody, iRet, MUI_NET_SEND_IMMEDIATELY);
		json_free(pchBody);
	}
	return iRet;

}

static int s_api_parse_rules_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
//	trans_rules_part(psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method, psApiDealCtrl->psConntCtrl->sHttpInfo.pchUrl, ((char*)psApiDealCtrl->psConntCtrl->sHttpInfo.pui8Body), psConntCtrl, pvBuf, tSize);
	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_rules(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_rules(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_post_rules(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_DELETE == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_del_rules(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_put_rtc_sys_time (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseTime sApiParseTime;
	S_SysTime sSysTime;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_put_rtc_sys_time(psApiDealCtrl->psBody, &sApiParseTime);
		if(iRet < 0)
		{
			return iRet;
		}
		if(6!=sscanf(sApiParseTime.achTimeBuf, "%hu-%hhu-%hhuT%hhu:%hhu:%hhu",&sSysTime.m_year,&sSysTime.m_mon,&sSysTime.m_mday,
					&sSysTime.m_hur,&sSysTime.m_min,&sSysTime.m_sec))
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_api_parse_put_rtc_sys_time] Time type [year]-[mon]-[day]T[hur]:[min]:[sec] \r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}
		sSysTime.m_wday= cacl_day_by_date(&sSysTime);
		/*
		sscanf(sApiParseTime.achTimeBuf, "%c-%c-%cT%c:%c:%c", (char *) (&sSysTime.m_year), (char *) (&sSysTime.m_mon), (char *) (&sSysTime.m_mday),
				(char *) (&sSysTime.m_hur), (char *) (&sSysTime.m_min), (char *) (&sSysTime.m_sec));
		sSysTime.m_wday= cacl_day_by_date(&sSysTime);
		*/
		return api_answer_put_rtc_sys_time(psApiDealCtrl, &sSysTime);
	}
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_api_parse_put_rtc_sys_time] GradeNum[%hhu] EP: PUT /api/xxxx/rtc\r\n",psApiDealCtrl->sUrlParse.ui8GradeNum);
	return API_DEAL_ERR_INVALID_MSG;
}
static int s_api_parse_rtc_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);
	if(MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_rtc_sys_time(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_get_sys (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_SysInfo sSysInfo;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);


	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_get_sys(psApiDealCtrl->psBody, &sSysInfo);
		if (iRet < 0)
		{
			return iRet;
		}

		return api_answer_get_sys(psApiDealCtrl, &sSysInfo);
	}

	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_api_parse_get_sys] GradeNum[%hhu] EP: GET /api/xxxx/sys\r\n",psApiDealCtrl->sUrlParse.ui8GradeNum);

	return API_DEAL_ERR_INVALID_MSG;
}
static int s_api_parse_put_sys (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetSys sApiParseSetSys;
	int iRet;


	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);


	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_set_sys(psApiDealCtrl->psBody, &sApiParseSetSys);
		if (iRet < 0)
		{
			return iRet;
		}

		return api_answer_set_sys(psApiDealCtrl, &sApiParseSetSys);
	}
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_api_parse_get_sys] GradeNum[%hhu] EP: PUT /api/xxxx/sys\r\n",psApiDealCtrl->sUrlParse.ui8GradeNum);

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_sys_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_sys(psApiDealCtrl);
	}
	else if(MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_sys(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_get_debug (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParsedebug sApiParsedebug;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_get_debug(psApiDealCtrl->psBody, &sApiParsedebug);
		if (iRet < 0)
		{
			return iRet;
		}
		return api_answer_get_debug(psApiDealCtrl,&sApiParsedebug);
	}
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_api_parse_get_debug] GradeNum[%hhu] EP: GET /api/xxxx/debug\r\n",psApiDealCtrl->sUrlParse.ui8GradeNum);

	return API_DEAL_ERR_INVALID_MSG;
}
static int s_api_parse_put_debug (const S_ApiDealCtrl *psApiDealCtrl)
{
	int iRet;
	S_ApiParsedebug sApiParsedebug;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_set_debug (psApiDealCtrl->psBody, &sApiParsedebug);
		if(iRet < 0)
		{
			return iRet;
		}
		return api_answer_set_debug(psApiDealCtrl, &sApiParsedebug);
	}
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_api_parse_put_debug] GradeNum[%hhu] EP: PUT /api/xxxx/debug\r\n",psApiDealCtrl->sUrlParse.ui8GradeNum);

	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_debug_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_debug(psApiDealCtrl);
	}
	else if(MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_debug(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_put_at_debug (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseAtdebug sApiParseatdebug;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_put_at_debug(psApiDealCtrl->psBody, &sApiParseatdebug);
		if(iRet < 0)
		{
			return iRet;
		}
		iRet = zigbee_print_query(sApiParseatdebug.achAnswer, sizeof(sApiParseatdebug.achAnswer), MUI_AT_QUERY_WAIT_MAX, "%s\r\n", sApiParseatdebug.achCmd);
		if(iRet <0)
		{
			return api_answer_put_at_debug_failed(psApiDealCtrl, &sApiParseatdebug);
		}
		return api_answer_put_at_debug_succeed(psApiDealCtrl, &sApiParseatdebug);
	}
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_api_parse_put_debug] GradeNum[%hhu] EP: PUT /api/xxxx/atdebug\r\n",psApiDealCtrl->sUrlParse.ui8GradeNum);
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_at_debug_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_PUT == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_put_at_debug(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_update_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_get_upgrade (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseUpgrade sApiParseUpgrade;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_get_upgrade_state(psApiDealCtrl->psBody, &sApiParseUpgrade);
		if (iRet < 0)
		{
			return iRet;
		}
		return api_answer_get_upgrade_state(psApiDealCtrl,&sApiParseUpgrade);
	}
	return API_DEAL_ERR_INVALID_MSG;
}
static int s_api_parse_post_upgrade (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseUpgrade sApiParseupgrade;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_post_upgrade] upgrade command method POST api/xxxxx/upgrade\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_post_upgrade (psApiDealCtrl->psBody, &sApiParseupgrade);
		if(iRet < 0)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_api_parse_post_upgrade]upgrade json parse fail\r\n");
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = upgrade_destination_device(sApiParseupgrade.achDest, sApiParseupgrade.achFile,sApiParseupgrade.achAddress);
		if(iRet < 0)
		{
			return api_answer_post_upgrade_failed(psApiDealCtrl, &sApiParseupgrade);
		}
		return api_answer_post_upgrade_succeed(psApiDealCtrl, &sApiParseupgrade);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_upgrade_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_get_upgrade(psApiDealCtrl);
	}
	else if (MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_post_upgrade(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_remote_get_download (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseRemoteDownload sApiParseRemoteDownload;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_get_remote_download_state(psApiDealCtrl->psBody, &sApiParseRemoteDownload);
		if (iRet < 0)
		{
			return iRet;
		}
		return api_answer_get_remote_download_state(psApiDealCtrl,&sApiParseRemoteDownload);
	}
	return API_DEAL_ERR_INVALID_MSG;
}
static int s_api_parse_remote_post_download (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseRemoteDownload sApiParseRemoteDownload;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_remote_download(psApiDealCtrl->psBody,&sApiParseRemoteDownload);
		if(iRet < 0)
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = ftp_upgrade(sApiParseRemoteDownload.achUserName,sApiParseRemoteDownload.achPassWord, sApiParseRemoteDownload.achLink, "/root/", 900);
		if (iRet < 0)
		{
			return api_answer_remote_post_download_failed(psApiDealCtrl, &sApiParseRemoteDownload);
		}

		return api_answer_remote_post_download_succeed(psApiDealCtrl, &sApiParseRemoteDownload);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_remote_download_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_GET == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_remote_get_download(psApiDealCtrl);
	}
	else if(MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_remote_post_download(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}

static int s_api_parse_set_test_mode (const S_ApiDealCtrl *psApiDealCtrl)
{
	S_ApiParseSetTestMode sApiParseSetTestMode;
	int iRet;

	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST != psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}
	if (3 == psApiDealCtrl->sUrlParse.ui8GradeNum)
	{
		iRet = json_parse_set_test_mode(psApiDealCtrl->psBody, &sApiParseSetTestMode);
		if(iRet < 0)
		{
			return API_DEAL_ERR_INVALID_MSG;
		}
		iRet = set_led_light_mode(sApiParseSetTestMode.ui8Mode);
		if (iRet < 0)
		{
			return api_answer_set_test_mode_failed(psApiDealCtrl, &sApiParseSetTestMode);
		}

		return api_answer_set_test_mode_succeed(psApiDealCtrl, &sApiParseSetTestMode);
	}
	return API_DEAL_ERR_INVALID_MSG;
}
static int s_api_parse_test_mode_deal (const S_ApiDealCtrl *psApiDealCtrl)
{
	ASSERT(NULL != psApiDealCtrl);
	ASSERT(NULL != psApiDealCtrl->psConntCtrl);

	if (MUI_HTTP_METHOD_POST == psApiDealCtrl->psConntCtrl->sHttpInfo.ui8Method)
	{
		return s_api_parse_set_test_mode(psApiDealCtrl);
	}
	return API_DEAL_ERR_INVALID_MSG;
}























const char* api_http_method_string_get (uint8_t ui8Idx)
{
	ASSERT(ui8Idx < (sizeof(s_aachHttpMethod)/sizeof(s_aachHttpMethod[0])));
	return s_aachHttpMethod[ui8Idx];
}

//-------------------------------------------------------------------------------------------------*
/*	Net API 发送函数
;	(功能)根据FD发送报文.
;	(参数)
;		NULL
;	(返回)
;		int:		>=0:发送的字节数
;					 <0:发送失败
;	(注意)
*/
int api_answer_send (S_ConntCtrl *psConntCtrl, const void *pvBuf, int tLen)
{
	int iRet;

	ASSERT(pvBuf != NULL);

	if ((NULL==psConntCtrl) || (0>=tLen))
	{
		return -1;
	}

	iRet = send(psConntCtrl->iConntFd, (const char*)pvBuf, tLen, 0);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_send]Send answer fail, errno:%d\r\n", iRet);
		return -1;
	}
	else
	{
		/*
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[api_answer_send]Have sent %d bytes message as bellow:\r\n", tLen);
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[api_answer_send]==================================================\r\n");
		API_DEAL_PUTS((const char*)pvBuf, tLen);
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
		*/
	}

	return tLen;
}
//-------------------------------------------------------------------------------------------------*
/*	Net API 分段发送函数
;	(功能)根据FD分段发送报文.
;	(参数)
;		psConntCtrl:	连接控制结构体指针
;		pvBuf:			要发送的报文缓冲区指针
;		tLen:			要发送的报文长度
;		ui8Flg:			发送标志
;						bit1: 1(立即发送); 0(仅仅放入缓存,但如果缓存已满的话则立即发送)
;						bit0: 1(数据还没有结束); 0(数据结束,应立即发送)
;	(返回)
;		int:			>=0:发送的字节数
;						 <0:发送失败或缓存益处
;	(注意)
*/
int api_answer_segment_send (S_ConntCtrl *psConntCtrl, const void *pvBuf, size_t tLen, uint8_t ui8Flg)
{
	size_t tRemain;
	int iRet;

	ASSERT(pvBuf != NULL);
	ASSERT(tLen > 0);

	/*
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[api_answer_segment_send]--------------------------Start---------------------------\r\n");
	API_DEAL_DYN_PUTS(M_DBG_LEVEL_DEBUG, pvBuf, tLen);
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n[api_answer_segment_send]---------------------------End----------------------------\r\n");
	*/

	if (NULL == psConntCtrl)
	{
		return -1;
	}

	while (tLen > (tRemain=(sizeof(s_sApiAnswerBufCtrl.aui8Buf)-s_sApiAnswerBufCtrl.tLen)))
	{
	#if 0
		memcpy(&s_sApiAnswerBufCtrl.aui8Buf[s_sApiAnswerBufCtrl.tLen], pvBuf, tRemain);
		//缓冲区已满,先将部分数据发送出去;
		iRet = send(psConntCtrl->iConntFd, (const char*)s_sApiAnswerBufCtrl.aui8Buf, sizeof(s_sApiAnswerBufCtrl.aui8Buf), 0);
		if (iRet < 0)
		{
			API_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_segment_send]Call 'send()' fail, errno:%d\r\n", errno);
		}
		tLen -= tRemain;
		s_sApiAnswerBufCtrl.tLen = 0;
	#else
		s_sApiAnswerBufCtrl.tLen = 0;
		return -1;
	#endif
	}

	memcpy(&s_sApiAnswerBufCtrl.aui8Buf[s_sApiAnswerBufCtrl.tLen], pvBuf, tLen);
	s_sApiAnswerBufCtrl.tLen += tLen;

#if 0
	if ((0==(ui8Flg&(uint8_t)(MUI_NET_SEND_IMMEDIATELY))) &&
		(0!=(ui8Flg&(uint8_t)(MUI_NET_SEND_NOT_FINISH))))
#else
	if (0 != (ui8Flg&(uint8_t)(MUI_NET_SEND_NOT_FINISH)))
#endif
	{
		return (int)s_sApiAnswerBufCtrl.tLen;
	}

	/*
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[api_answer_segment_send]##########################Start###########################\r\n");
	API_DEAL_DYN_PUTS(M_DBG_LEVEL_DEBUG, (char*)s_sApiAnswerBufCtrl.aui8Buf, (int)s_sApiAnswerBufCtrl.tLen);
	API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n[api_answer_segment_send]###########################End############################\r\n");
	*/
	//将数据包发送出去;
	if (psConntCtrl->iConntFd > 0)
	{
		iRet = send(psConntCtrl->iConntFd, (const char*)s_sApiAnswerBufCtrl.aui8Buf, s_sApiAnswerBufCtrl.tLen, 0);
	}
	else
	{
		s_sApiAnswerBufCtrl.aui8Buf[s_sApiAnswerBufCtrl.tLen] = '}';
		s_sApiAnswerBufCtrl.tLen++;
		iRet = remote_api_answer((const char*)s_sApiAnswerBufCtrl.aui8Buf, s_sApiAnswerBufCtrl.tLen, s_ui32RemoteDstId, s_ui32RemoteSqn);
	}
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[api_answer_segment_send]Send answer fail, errno:%d\r\n", errno);
	}
	else
	{
		/*
        API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[api_answer_segment_send]Have sent %d bytes message as bellow:\r\n", s_sApiAnswerBufCtrl.Len+(s_sApiAnswerBufCtrl.Buf[s_sApiAnswerBufCtrl.Len]='\0'));
        API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[api_answer_segment_send]==================================================\r\n"
											   "{\r\n"
											   "%s"
						                       "\r\n}\r\n", (const char*)s_sApiAnswerBufCtrl.Buf);
		*/
	}

	s_sApiAnswerBufCtrl.tLen = 0;

	return iRet;
}
//-------------------------------------------------------------------------------------------------*

int api_parse (S_ConntCtrl *psConntCtrl, const cJSON *psBody)
{
	S_ApiDealCtrl sApiDealCtrl;
	uint16_t ui16GradeNum, ui16Len;
	uint16_t ui16DevType;
	char *pchTmp;
	int iRet;

	ASSERT(NULL != psConntCtrl);
	ASSERT(NULL != psConntCtrl->sHttpInfo.pchUrl);
	ASSERT(0 < psConntCtrl->sHttpInfo.ui16UrlLen);

	sApiDealCtrl.psConntCtrl = psConntCtrl;
	sApiDealCtrl.psBody		 = psBody;

	//开始分析URL
	iRet = s_api_url_parse(&sApiDealCtrl);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_parse]Call 's_api_url_parse()' fail.\r\n");
		return iRet;
	}

	ui16GradeNum = (uint16_t)iRet;
	if (0 == ui16GradeNum)
	{
		return API_DEAL_ERR_INVALID_MSG;
	}

	//URL第1层关键字处理:开始校验URL的前缀
	iRet = s_api_url_prefix_verify(&sApiDealCtrl.psConntCtrl->sHttpInfo.pchUrl[sApiDealCtrl.sUrlParse.aui16Offset[0]],
								   sApiDealCtrl.sUrlParse.aui16Offset[1]-sApiDealCtrl.sUrlParse.aui16Offset[0]);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_parse]Call 's_api_url_prefix_verify()' fail.\r\n");
		return iRet;
	}
	if (1 == ui16GradeNum)
	{
		if (MUI_HTTP_METHOD_POST == sApiDealCtrl.psConntCtrl->sHttpInfo.ui8Method)
		{
			//用户创建处理
			return s_api_parse_user_creat(&sApiDealCtrl);
		}
		return API_DEAL_ERR_INVALID_MSG;
	}

	//URL第2层关键字处理:开始校验URL的Username
	iRet = s_api_url_user_verify(&sApiDealCtrl.psConntCtrl->sHttpInfo.pchUrl[sApiDealCtrl.sUrlParse.aui16Offset[1]],
								 sApiDealCtrl.sUrlParse.aui16Offset[2]-sApiDealCtrl.sUrlParse.aui16Offset[1],
								 &sApiDealCtrl.ui32Authority);
	if (iRet < 0)
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[api_parse]Call 's_api_url_user_verify()' fail.\r\n");
		return iRet;
	}
	if (2 == ui16GradeNum)
	{
		if (MUI_HTTP_METHOD_GET == sApiDealCtrl.psConntCtrl->sHttpInfo.ui8Method)
		{
			return s_api_parse_get_full_state(&sApiDealCtrl);
		}
		else if (MUI_HTTP_METHOD_POST == sApiDealCtrl.psConntCtrl->sHttpInfo.ui8Method)
		{
			return s_api_parse_search_new_dev(&sApiDealCtrl);
		}
		else if (MUI_HTTP_METHOD_DELETE == sApiDealCtrl.psConntCtrl->sHttpInfo.ui8Method)
		{
			return s_api_parse_factory_new(&sApiDealCtrl);
		}
		return API_DEAL_ERR_INVALID_MSG;
	}
	//URL第3层关键字处理:开始比较Keyword
	ui16DevType = MUI_DATA_TYPE_INVALID;
	pchTmp  = &sApiDealCtrl.psConntCtrl->sHttpInfo.pchUrl[sApiDealCtrl.sUrlParse.aui16Offset[2]];
	ui16Len = sApiDealCtrl.sUrlParse.aui16Offset[3]-sApiDealCtrl.sUrlParse.aui16Offset[2];
	if ((sizeof(MSTR_API_URL_KEY_WORD_CONFIG)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_CONFIG, ui16Len))
	{
		return s_api_parse_config_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_GROUPS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_GROUPS, ui16Len))
	{
		return s_api_parse_groups_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_SET)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_SET, ui16Len))
	{
		return s_api_parse_set_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_SCENE)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_SCENE, ui16Len))
	{
		return s_api_parse_scene_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_SCHEDULES)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_SCHEDULES, ui16Len))
	{
		return s_api_parse_schedules_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_RULES)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_RULES, ui16Len))
	{
		return s_api_parse_rules_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_LIGHTS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_LIGHTS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_LIGHT;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_OCC_SENSORS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_OCC_SENSORS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_OCC_SENSOR;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_BIN_SENSORS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_BIN_SENSORS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_BIN_SENSOR;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_TEMP_SENSORS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_TEMP_SENSORS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_TEMP_SENSOR;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_HUM_SENSORS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_HUM_SENSORS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_HUM_SENSOR;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_BIN_CTRLS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_BIN_CTRLS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_BIN_CTRL;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_LEVEL_CTRLS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_LEVEL_CTRLS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_LEVEL_CTRL;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_WARN_DEVS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_WARN_DEVS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_WARN_DEV;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_PLUGS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_PLUGS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_PLUG;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_SWITCHS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_SWITCHS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_SWITCH;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_SHADES)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_SHADES, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_SHADE;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_SONOSES)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_SONOSES, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_SONOS;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_PANELS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_PANELS, ui16Len))
	{
		ui16DevType = MUI_DATA_TYPE_PANEL;
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_RTC)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_RTC, ui16Len))
	{
		return s_api_parse_rtc_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_SYS)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_SYS, ui16Len))
	{
		return s_api_parse_sys_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_DEBUG)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_DEBUG, ui16Len))
	{
		return s_api_parse_debug_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_AT_DEBUG)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_AT_DEBUG, ui16Len))
	{
		return s_api_parse_at_debug_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_UPDATE)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_UPDATE, ui16Len))
	{
		return s_api_parse_update_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_UPGRADE)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_UPGRADE, ui16Len))
	{
		return s_api_parse_upgrade_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_REMOTE_DL)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_REMOTE_DL, ui16Len))
	{
		return s_api_parse_remote_download_deal(&sApiDealCtrl);
	}
	else if ((sizeof(MSTR_API_URL_KEY_WORD_TEST_MODE)-1)==ui16Len &&  0==memcmp(pchTmp, MSTR_API_URL_KEY_WORD_TEST_MODE, ui16Len))
	{
		return s_api_parse_test_mode_deal(&sApiDealCtrl);
	}

	if ((MUI_DATA_TYPE_DEV_START<=ui16DevType) && (ui16DevType<=MUI_DATA_TYPE_DEV_END))
	{
		return s_api_parse_devices_deal(ui16DevType, &sApiDealCtrl);
	}

	return API_DEAL_ERR_INVALID_MSG;
}

int api_deal_answer_head_send (S_ConntCtrl *psConntCtrl)
{
	char achBuf[500];	//此大小根据报文头而定.
	int iLen;

	ASSERT(NULL != psConntCtrl);

	iLen = snprintf(achBuf, sizeof(achBuf), "HTTP/1.1 200 OK\r\n"
											"Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0\r\n"
											"Pragma: no-cache\r\n"
											"Expires: Mon, 1 Aug 2011 09:00:00 GMT\r\n"
											"Connection: close\r\n"
											"Access-Control-Max-Age: 0\r\n"
											"Access-Control-Allow-Origin: *\r\n"
											"Access-Control-Allow-Credentials: true\r\n"
											"Access-Control-Allow-Methods: POST, GET, OPTIONS, PUT, DELETE\r\n"
											"Access-Control-Allow-Headers: Content-Type\r\n"
											"Content-type: application/json\r\n"
											"\r\n");
	ASSERT((size_t)iLen < sizeof(achBuf));	//由于无法确认缓存是否不够用,因此我们限定如果填满了Buffer,则认为缓存已经不够用

	if (api_answer_segment_send(psConntCtrl, achBuf, iLen, MUI_NET_SEND_IMMEDIATELY|MUI_NET_SEND_NOT_FINISH) < 0)	//先发出包头，以便Philip app程序快速响应
	{
		return -1;
	}

	return iLen;
}

int remote_api_deal_answer_head_send (S_ConntCtrl *psConntCtrl, uint32_t ui32DstId, uint32_t ui32Sqn)
{
	char achBuf[100];	//此大小根据报文头而定.
	int iLen;

	ASSERT(NULL != psConntCtrl);

	s_ui32RemoteDstId = ui32DstId;
	s_ui32RemoteSqn   = ui32Sqn;

	iLen = snprintf(achBuf, sizeof(achBuf), "{\"method\":\"%s\",\"url\":\"%s\",\"body\":", s_aachHttpMethod[psConntCtrl->sHttpInfo.ui8Method], psConntCtrl->sHttpInfo.pchUrl);
	ASSERT((size_t)iLen < sizeof(achBuf));	//由于无法确认缓存是否不够用,因此我们限定如果填满了Buffer,则认为缓存已经不够用

	if (api_answer_segment_send(psConntCtrl, achBuf, iLen, MUI_NET_SEND_IMMEDIATELY|MUI_NET_SEND_NOT_FINISH) < 0)	//先发出包头，以便Philip app程序快速响应
	{
		return -1;
	}

	return iLen;
}

int api_deal (S_ConntCtrl *psConntCtrl)
{
	cJSON *psBody;
	int iRet;

	ASSERT(NULL != psConntCtrl);

	if (0 > sem_wait_ms(&s_sApiAnswerBufCtrl.tMutex, 5000))
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[api_deal]Wait for s_tApiDealMutex timeout.\r\n");
		return -1;
	}

	s_api_answer_buf_clean();

	do
	{
		iRet = api_deal_answer_head_send(psConntCtrl);
		if (iRet < 0)
		{
			break;
		}

		psBody = NULL;
		if ((psConntCtrl->sHttpInfo.ui32BodyLen>0) && (NULL==(psBody=cJSON_Parse((const char*)psConntCtrl->sHttpInfo.pui8Body))))
		{
			iRet = JSON_DEAL_ERR_PARSE_FAIL;
		}
		else
		{
			iRet = api_parse(psConntCtrl, psBody);
		}

		cJSON_Delete(psBody);

		if (iRet < 0)
		{
			if (API_DEAL_ERR_INVALID_MSG == iRet)
			{
				api_answer_invalid_api(psConntCtrl, &iRet);
			}
			else
			{
				api_answer_low_layer_error(psConntCtrl, &iRet);
			}
		}
	} while (0);

	sem_post(&s_sApiAnswerBufCtrl.tMutex);

	return iRet;
}

int remote_api_deal (const void *pvRemoteMsg, size_t tLen)
{
	S_ConntCtrl sConnCtrl;
	cJSON *psJson, *psNode, *psMessage, *psBody;
	uint32_t ui32DstId, ui32SrcId, ui32Sqn;
	uint8_t ui8KeyFlag;
	int iRet;

	ASSERT(NULL != pvRemoteMsg);

	if ((0==tLen) || ('\0'!=((const char*)pvRemoteMsg)[tLen-1]))
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_ERR, "[remote_api_deal]Received invalid remote API message.\r\n");
		return API_DEAL_ERR_INVALID_MSG;
	}

	//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "***********************************************\r\n");
	//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%s", (const char*)pvRemoteMsg);
	//API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n***********************************************\r\n");

	psJson = cJSON_Parse(pvRemoteMsg);
	if (NULL == psJson)
	{
		return JSON_DEAL_ERR_PARSE_FAIL;
	}

	sConnCtrl.iConntFd = -1;		//用-1表示远程连接
	sConnCtrl.tTicks   = 0;			//记录的滴答数
	sConnCtrl.pui8Buf  = NULL;		//报文缓冲区
	sConnCtrl.tLen	   = 0;			//总报文长度(多个segment总和)

	ui8KeyFlag = 0;
	for (psNode=psJson->child; NULL!=psNode; psNode=psNode->next)
	{
		if ((0==strncmp(psNode->string, "dstid", sizeof("dstid")-1)) && (cJSON_Number==psNode->type))
		{
			ui32DstId = (uint32_t)psNode->valueint;	//!!!!!!!!!!!!!!!
			ui8KeyFlag |= 0x01;
		}
		else if ((0==strncmp(psNode->string, "srcid", sizeof("srcid")-1)) && (cJSON_Number==psNode->type))
		{
			ui32SrcId = (uint32_t)psNode->valueint;	//!!!!!!!!!!!!!!!
			ui8KeyFlag |= 0x02;
		}
		else if ((0==strncmp(psNode->string, "sqn", sizeof("sqn")-1)) && (cJSON_Number==psNode->type))
		{
			ui32Sqn = (uint32_t)psNode->valueint;	//!!!!!!!!!!!!!!!
			ui8KeyFlag |= 0x04;
		}
		else if ((0==strncmp(psNode->string, "message", sizeof("message")-1)) && (cJSON_Object==psNode->type))
		{
			psMessage = psNode;
			ui8KeyFlag |= 0x08;
		}
	}
	if (ui8KeyFlag != 0x0F)
	{
		cJSON_Delete(psJson);
		return -1;
	}

	sConnCtrl.sHttpInfo.ui8Schedule = MUI_HTTP_DEAL_RECVED_BODY;
	sConnCtrl.sHttpInfo.pui8Line	= NULL;
	sConnCtrl.sHttpInfo.pui8Head	= NULL;
	sConnCtrl.sHttpInfo.pui8Body	= NULL;
	sConnCtrl.sHttpInfo.ui32BodyLen = 0;

	psBody = NULL;
	ui8KeyFlag = 0x04;
	for (psNode=psMessage->child; NULL!=psNode; psNode=psNode->next)
	{
		if ((0==strncmp(psNode->string, "method", sizeof("method")-1)) && (cJSON_String==psNode->type))
		{
			if (0 == strncmp(psNode->valuestring, "POST", sizeof("POST")-1))
			{
				sConnCtrl.sHttpInfo.ui8Method = MUI_HTTP_METHOD_POST;
			}
			else if (0 == strncmp(psNode->valuestring, "GET", sizeof("GET")-1))
			{
				sConnCtrl.sHttpInfo.ui8Method = MUI_HTTP_METHOD_GET;
			}
			else if (0 == strncmp(psNode->valuestring, "PUT", sizeof("PUT")-1))
			{
				sConnCtrl.sHttpInfo.ui8Method = MUI_HTTP_METHOD_PUT;
			}
			else if (0 == strncmp(psNode->valuestring, "DELETE", sizeof("DELETE")-1))
			{
				sConnCtrl.sHttpInfo.ui8Method = MUI_HTTP_METHOD_DELETE;
			}
			else
			{
				continue;
			}
			ui8KeyFlag |= 0x01;
		}
		else if ((0==strncmp(psNode->string, "url", sizeof("url")-1)) && (cJSON_String==psNode->type))
		{
			sConnCtrl.sHttpInfo.pchUrl	   = psNode->valuestring;
			sConnCtrl.sHttpInfo.ui16UrlLen = strlen(sConnCtrl.sHttpInfo.pchUrl);
			ui8KeyFlag |= 0x02;
		}
		else if ((0==strncmp(psNode->string, "body", sizeof("body")-1)) && (cJSON_Object==psNode->type))
		{
			psBody = psNode;
			ui8KeyFlag |= 0x04;
		}
	}
	if (ui8KeyFlag != 0x07)
	{
		cJSON_Delete(psJson);
		return -1;
	}

	if (0 > sem_wait_ms(&s_sApiAnswerBufCtrl.tMutex, 5000))
	{
		API_DEAL_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[remote_api_deal]Wait for s_tApiDealMutex timeout.\r\n");
		cJSON_Delete(psJson);
		return -1;
	}

	s_api_answer_buf_clean();

	do
	{
		iRet = remote_api_deal_answer_head_send(&sConnCtrl, ui32SrcId, ui32Sqn);
		if (iRet < 0)
		{
			break;
		}

		iRet = api_parse(&sConnCtrl, psBody);
		if (iRet < 0)
		{
			if (API_DEAL_ERR_INVALID_MSG == iRet)
			{
				api_answer_invalid_api(&sConnCtrl, &iRet);
			}
			else
			{
				api_answer_low_layer_error(&sConnCtrl, &iRet);
			}
		}
	} while (0);

	sem_post(&s_sApiAnswerBufCtrl.tMutex);
	cJSON_Delete(psJson);

	return iRet;
}

int api_deal_init (void)
{
	/* 清除API answer缓冲区*/
	s_api_answer_buf_clean();
	return sem_init(&s_sApiAnswerBufCtrl.tMutex, 0, 1);
}




