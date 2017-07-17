//#define	__M_FILE_NAME_json_deal_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_json_deal_H__
#define	__M_FILE_NAME_json_deal_H__

#include "common.h"
#include "data_common.h"
#include "cJSON.h"
#include "at_port.h"
#include "api_deal.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int json_obj_assemble_dev_data (uint16_t ui16DevType, uint16_t ui16Id, uint8_t ui8Mode, cJSON **ppsJson);
extern int json_obj_assemble_dev_state (uint16_t ui16DevType, uint16_t ui16Id, uint8_t ui8State, cJSON **ppsJson);

extern int json_assemble_answer_invalid_api (char **ppchJson, int *piErr);
extern int json_assemble_answer_low_layer_error (char **ppchJson, int *piErr);
extern int json_assemble_answer_link_button_not_pressed (char **ppchJson);
extern int json_assemble_answer_create_user_fail (char **ppchJson, S_ApiParseCreateUser *psParse);
extern int json_assemble_answer_create_user_succeed (char **ppchJson, S_ApiParseCreateUser *psParse);
extern int json_assemble_answer_search_new_dev_fail (char **ppchJson, S_ApiParseSearchNewDev *psParse);
extern int json_assemble_answer_search_new_dev_succeed (char **ppchJson, S_ApiParseSearchNewDev *psParse);
extern int json_assemble_answer_factory_new_fail (char **ppchJson, S_ApiParseFactoryNew *psParse);
extern int json_assemble_answer_factory_new_succeed (char **ppchJson, S_ApiParseFactoryNew *psParse);
extern int json_assemble_get_config (char **ppchJson);
extern int json_assemble_get_dev_attr_state (uint16_t ui16DevType, uint16_t ui16Id, char **ppchJson);
extern int json_assemble_answer_del_dev_fail (uint16_t ui16DevType, uint16_t ui16Id, char **ppchJson, S_ApiParseDelDev *psParse);
extern int json_assemble_answer_del_dev_succeed (uint16_t ui16DevType, uint16_t ui16Id, char **ppchJson, S_ApiParseDelDev *psParse);
extern int json_assemble_answer_get_all_group (const S_ApiDealCtrl *psApiDealCtrl, char **ppchJson);
extern int json_assemble_get_group_attr(uint16_t ui16Id, char **ppchJson);
extern int json_assemble_answer_del_group(uint16_t ui16Id, char **ppchJson);
extern int json_assemble_answer_get_all_set(char **ppchJson);
extern int json_assemble_answer_get_set_attr(uint16_t ui16Id, char **ppchJson);
extern int json_assemble_answer_del_set(uint16_t ui16Id, char **ppchJson);
extern int json_assemble_answer_get_all_schedule(char **ppchJson);
extern int json_assemble_answer_get_schedule_attr(uint16_t ui16Id, char **ppchJson);
extern int json_assemble_answer_del_schedule(uint16_t ui16Id, char **ppchJson);
extern int json_assemble_answer_get_all_rules(char **ppchJson);
extern int json_assemble_answer_get_rules_attr(uint16_t ui16Id, char **ppchJson);
extern int json_assemble_answer_del_rules(uint16_t ui16Id, char **ppchJson);

extern int json_assemble_report_with_body (uint8_t ui8Method, uint16_t ui16Type, uint16_t ui16Id, char **ppchJson, cJSON *psBody);

extern int json_parse_create_user (const cJSON *psBody, S_ApiParseCreateUser *psParse);
extern int json_parse_get_full_state (const cJSON *psBody, S_ApiParseGetFullState *psParse);
extern int json_parse_search_new_dev (const cJSON *psBody, S_ApiParseSearchNewDev *psParse);
extern int json_parse_factory_new (const cJSON *psBody, S_ApiParseFactoryNew *psParse);
extern int json_parse_get_config (const cJSON *psBody, S_ApiParseGetConfig *psParse);
extern int json_parse_set_config (const cJSON *psBody, S_ApiParseSetConfig *psParse);
extern int json_parse_get_all_devs_by_type (const cJSON *psBody, S_ApiParseGetAllDevsByType *psParse);
extern int json_parse_get_dev_attr_state (const cJSON *psBody, S_ApiParseGetDevAttrState *psParse);
extern int json_parse_set_dev_attr (const cJSON *psBody, S_ApiParseSetDevAttr *psParse);
extern int json_parse_set_dev_state (const cJSON *psBody, S_ApiParseSetDevState *psParse);
extern int json_parse_del_dev (const cJSON *psBody, S_ApiParseDelDev *psParse);
extern int json_parse_get_all_groups (const cJSON *psBody, S_ApiParseGetAllGroups *psParse);
extern int json_parse_get_group_attr (const cJSON *psBody, S_ApiParseGetGroupAttr *psParse);



extern int json_parse_config_group (const cJSON *psBody, S_GroupOpAttr *psParse);
extern int json_parse_set_group_attr (const cJSON *psBody, S_ApiParseSetGroup *psParse);
extern int json_parse_set_group_state (const cJSON *psBody, S_SetGroupState *psParse);
extern int json_parse_create_group (const cJSON *psBody, S_SetGroupAttr *psParse);
extern int json_parse_set_set_attr(const cJSON *psBody, S_ApiParseSetSet *psParse);
extern int json_parse_action_create_set(const cJSON *psJson, S_SetMember *psParse);
extern int json_parse_config_create_set(const cJSON *psJson, S_SetAttr *psParse);
extern int json_parse_create_set(const cJSON *psBody, S_SetAttr *psParse);
extern int json_parse_quitstate_create_schedule(const cJSON *psJson, S_ScheduleOutPutAction *psParse);
extern int json_parse_action_create_schedule(const cJSON *psJson, S_ScheduleOutPutAction *psParse);
extern int json_parse_config_create_schedule(const cJSON *psJson, S_ScheduleConfig *psParse);
extern int json_parse_set_schedule_attr(const cJSON *psBody, S_ApiParseSetschedule *psParse);
extern int json_parse_create_schedule(const cJSON *psBody, S_ApiParseSchedule *psParse);
extern int json_parse_state_create_rules(const cJSON *psJson, S_RulesInPutState *psParse);
extern int json_parse_input_create_rules(const cJSON *psJson, S_RulesConfig *psParse);
extern int json_parse_action_create_rules(const cJSON *psJson, S_RuelsOutPutState *psParse);
extern int json_parse_quitstate_create_rules(const cJSON *psJson, S_RuelsOutPutState *psParse);
extern int json_parse_output_create_rules(const cJSON *psJson, uint8_t OutPutType, S_RulesConfig *psParse);
extern int json_parse_config_create_rules(const cJSON *psJson, S_RulesConfig *psParse);
extern int json_parse_create_rules(const cJSON *psBody, S_ApiParseRules *psParse);
extern int json_parse_set_rules_attr(const cJSON *psBody, S_ApiParseSetRules *psParse);
extern int json_parse_put_rtc_sys_time (const cJSON *psBody, S_ApiParseTime *psParse);
extern int json_parse_get_sys (const cJSON *psBody, S_SysInfo *psParse);
extern int json_parse_set_sys (const cJSON *psBody, S_ApiParseSetSys *psParse);
extern int json_parse_get_debug (const cJSON *psBody, S_ApiParsedebug *psParse);
extern int json_parse_set_debug (const cJSON *psBody, S_ApiParsedebug *psParse);
extern int json_parse_put_at_debug (const cJSON *psBody, S_ApiParseAtdebug *psParse);
extern int json_parse_post_upgrade (const cJSON *psBody, S_ApiParseUpgrade *psParse);
extern int json_parse_get_remote_download_state (const cJSON *psBody, S_ApiParseRemoteDownload *psParse);
extern int json_parse_remote_download (const cJSON *psBody, S_ApiParseRemoteDownload *psParse);
extern int json_parse_set_test_mode (const cJSON *psBody, S_ApiParseSetTestMode *psParse);

extern int api_answer_invalid_api (S_ConntCtrl *psConntCtrl, int *piErr);
extern int api_answer_low_layer_error (S_ConntCtrl *psConntCtrl, int *piErr);
extern int api_answer_link_button_not_pressed (const S_ApiDealCtrl *psApiDealCtrl);
extern int api_answer_create_user_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseCreateUser *psParse);
extern int api_answer_create_user_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseCreateUser *psParse);
extern int api_answer_get_full_state (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetFullState *psParse);
extern int api_answer_search_new_dev_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSearchNewDev *psParse);
extern int api_answer_search_new_dev_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSearchNewDev *psParse);
extern int api_answer_factory_new_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseFactoryNew *psParse);
extern int api_answer_factory_new_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseFactoryNew *psParse);
extern int api_answer_get_config (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetConfig *psParse);
extern int api_answer_set_config (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetConfig *psParse);
extern int api_answer_get_all_devs_by_type (uint16_t ui16DevType, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetAllDevsByType *psParse);
extern int api_answer_get_dev_attr_state (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetDevAttrState *psParse);
extern int api_answer_set_dev_attr (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetDevAttr *psParse);
extern int api_answer_set_dev_state (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetDevState *psParse);
extern int api_answer_del_dev_fail (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseDelDev *psParse);
extern int api_answer_del_dev_succeed (uint16_t ui16DevType, uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseDelDev *psParse);
extern int api_answer_get_all_groups (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetAllGroups *psParse);
extern int api_answer_get_group_attr (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseGetGroupAttr *psParse);
extern int api_answer_set_group_attr (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetGroup *psParse);
extern int api_answer_set_group_state (uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_SetGroupState *psParse);
extern int api_answer_create_group(const S_ApiDealCtrl *psApiDealCtrl, S_SetGroupAttr *psParse);
extern int api_answer_set_set_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetSet *psParse);
extern int api_answer_create_set(const S_ApiDealCtrl *psApiDealCtrl, S_SetAttr *psParse);
extern int api_answer_create_schedule(const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSchedule *psParse);
extern int api_answer_set_schedule_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetschedule *psParse);
extern int api_answer_create_rules(const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseRules *psParse);
extern int api_answer_set_rules_attr(uint16_t ui16Id, const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetRules *psParse);
extern int api_answer_put_rtc_sys_time (const S_ApiDealCtrl *psApiDealCtrl, S_SysTime *psParse);
extern int api_answer_get_sys (const S_ApiDealCtrl *psApiDealCtrl, S_SysInfo *psParse);
extern int api_answer_set_sys (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetSys *psParse);
extern int api_answer_get_debug (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParsedebug *psParse);
extern int api_answer_set_debug (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParsedebug *psParse);
extern int api_answer_put_at_debug_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseAtdebug *psParse);
extern int api_answer_put_at_debug_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseAtdebug *psParse);
extern int api_answer_post_upgrade_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseUpgrade *psParse);
extern int api_answer_post_upgrade_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseUpgrade *psParse);
extern int api_answer_get_remote_download_state (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseRemoteDownload *psParse);
extern int api_answer_remote_post_download_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseRemoteDownload *psParse);
extern int api_answer_remote_post_download_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseRemoteDownload *psParse);
extern int api_answer_set_test_mode_failed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetTestMode *psParse);
extern int api_answer_set_test_mode_succeed (const S_ApiDealCtrl *psApiDealCtrl, S_ApiParseSetTestMode *psParse);

#endif	//end of "#ifndef __M_FILE_NAME_json_deal_H__".
