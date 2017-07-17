#include "schedule.h"
#include "group.h"
#include "schdu_data.h"
#include "set.h"
#include "schedule_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define SCHEDULE_DBG

#ifndef SCHEDULE_PUTC
#ifdef SCHEDULE_DBG
#define SCHEDULE_PUTC(Char)			PUTC(Char)
#else
#define SCHEDULE_PUTC(Char)
#endif
#endif

#ifndef SCHEDULE_PUTS
#ifdef SCHEDULE_DBG
#define SCHEDULE_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define SCHEDULE_PUTS(Str, Len)
#endif
#endif

#ifndef SCHEDULE_PUTASCC
#ifdef SCHEDULE_DBG
#define SCHEDULE_PUTASCC(Char)			PUTASCC(Char)
#else
#define SCHEDULE_PUTASCC(Char)
#endif
#endif

#ifndef SCHEDULE_PUTASCS
#ifdef SCHEDULE_DBG
#define SCHEDULE_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define SCHEDULE_PUTASCS(Str, Len)
#endif
#endif

#ifndef SCHEDULE_PRINT
#ifdef SCHEDULE_DBG
#define SCHEDULE_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define SCHEDULE_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef SCHEDULE_DYN_PUTC
#ifdef SCHEDULE_DBG
#define SCHEDULE_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_SCHEDULE|Dbg), Char)
#else
#define SCHEDULE_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef SCHEDULE_DYN_PUTS
#ifdef SCHEDULE_DBG
#define SCHEDULE_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_SCHEDULE|Dbg), Str, Len)
#else
#define SCHEDULE_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef SCHEDULE_DYN_PUTASCC
#ifdef SCHEDULE_DBG
#define SCHEDULE_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_SCHEDULE|Dbg), Char)
#else
#define SCHEDULE_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef SCHEDULE_DYN_PUTASCS
#ifdef SCHEDULE_DBG
#define SCHEDULE_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_SCHEDULE|Dbg), Str, Len)
#else
#define SCHEDULE_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef SCHEDULE_DYN_PRINT
#ifdef SCHEDULE_DBG
#define SCHEDULE_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_SCHEDULE|Dbg), Fmt, ##Arg)
#else
#define SCHEDULE_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

#define MUI_SCHEDULE_WAIT_TIME_MS_MAX		3000

#if 0
/*读取schedule信息
 *
 *作用:	读取schedule信息
 *
 *参数:	schedid				schedule id
 *		buf					读取信息缓存区
 *		buf_size			读取schedule信息大小
 *
 *返回:	成功>0，失败<0
 */
int read_sched_info (uint8_t schedid, char buf[], int buf_size)
{
	if (buf == NULL || buf_size < 0)
	{
		return -1;
	}

	//return File_Read_Schdu(schedid, (uint8_t *) buf, 0, buf_size);
	return schedule_data_read_info(schedid, 0, buf, buf_size);
}

/*读取schedule id
 *
 *作用:	读取schedule id
 *
 *参数:	schedid				schedule id
 *
 *返回:	成功>0，失败<0
 */
int read_sched_id (uint8_t schedid)
{
	uint8_t id;
	//return File_Read_Schdu(schedid, &id, 0, sizeof(uint8_t));
	return schedule_data_read_info(schedid, 0, &id, sizeof(uint8_t));
}

/*读取schedule config数目
 *
 *作用:	读取schedule config数目
 *
 *参数:	schedid				schedule id
 *		config_num			schedule config数目缓存区
 *
 *返回:	成功>0，失败<0
 */
int read_sched_config_num (uint8_t schedid, uint8_t *config_num)
{
	if (config_num == NULL)
	{
		return -1;
	}

	//return File_Read_Schdu(schedid, config_num, sizeof(uint8_t), sizeof(uint8_t));
	return schedule_data_read_info(schedid, sizeof(uint8_t), config_num, sizeof(uint8_t));
}

/*读取schedule是否能被触发
 *
 *作用:	读取schedule是否能被触发
 *
 *参数:	schedid				schedule id
 *		active				active返回缓存区
 *
 *返回:	成功返回>0，失败返回<0
 */
int read_sched_active (uint8_t schedid, uint8_t *active)
{
	if (active == NULL)
	{
		return -1;
	}

	uint8_t config_num = 0;
	if (read_sched_config_num(schedid, &config_num) > 0)
	{
		int idx = 2 * sizeof(uint8_t) + config_num * sizeof(ScheduleConfig);
		//return File_Read_Schdu(schedid, active, idx + sizeof(uint8_t), sizeof(uint8_t));
		return schedule_data_read_info(schedid, idx + sizeof(uint8_t), active, sizeof(uint8_t));
	}
	else
	{
		return -1;
	}
}

/*读取schedule重复触发数据
 *
 *作用:	读取schedule重复触发数据
 *
 *参数:	schedid				schedule id
 *		repeat				重复触发数据缓存区
 *
 *返回:	成功>0，失败<0
 */
int read_sched_repeat (uint8_t schedid, uint8_t *repeat)
{
	if (repeat == NULL)
	{
		return -1;
	}

	uint8_t config_num = 0;
	if (read_sched_config_num(schedid, &config_num) > 0)
	{
		int idx = 2 * sizeof(uint8_t) + config_num * sizeof(ScheduleConfig) + sizeof(uint8_t);
		//return File_Read_Schdu(schedid, repeat, idx + sizeof(uint8_t), sizeof(uint8_t));
		return schedule_data_read_info(schedid, idx + sizeof(uint8_t), repeat, sizeof(uint8_t));
	}
	else
	{
		return -1;
	}
}

/*读取schedule触发时间
 *
 *作用:	读取schedule触发时间
 *
 *参数:	schedid					schedule id
 *		time					触发时间返回缓存区
 *
 *返回:	成功返回>0，失败返回<0
 */
int read_sched_time (uint8_t schedid, char time[])
{
	if (time == NULL)
	{
		return -1;
	}

	uint8_t config_num = 0;
	if (read_sched_config_num(schedid, &config_num) > 0)
	{
		int idx = 2 * sizeof(uint8_t) + config_num * sizeof(ScheduleConfig) + 2 * sizeof(uint8_t);
		//return File_Read_Schdu(schedid, (uint8_t *) time, idx + sizeof(uint8_t), MAX_TIME_SIZE);
		return schedule_data_read_info(schedid, idx + sizeof(uint8_t), time, MAX_TIME_SIZE);
	}
	else
	{
		return -1;
	}
}

/*读取schedule name
 *
 *作用:	读取schedule name
 *
 *参数:	schedid					schedule id
 *		name					name返回缓存区
 *
 *返回:	成功返回>0，失败返回<0
 */
int read_sched_name (uint8_t schedid, char name[])
{
	if (name == NULL)
	{
		return -1;
	}

	uint8_t config_num = 0;
	if (read_sched_config_num(schedid, &config_num) > 0)
	{
		int idx = 2 * sizeof(uint8_t) + config_num * sizeof(ScheduleConfig) + 2 * sizeof(uint8_t) + MAX_TIME_SIZE;
		//return File_Read_Schdu(schedid, (uint8_t *) name, idx, MAX_NAME_SIZE);
		return schedule_data_read_info(schedid, idx, name, MAX_NAME_SIZE);
	}
	else
	{
		return -1;
	}
}

/*读取schedule description
 *
 *作用:	读取schedule description
 *
 *参数:	schedid					schedule id
 *		desc					description返回缓存区
 *
 *返回:	成功返回>0，失败返回<0
 */
int read_sched_desc (uint8_t schedid, char desc[])
{
	if (desc == NULL)
	{
		return -1;
	}

	uint8_t config_num = 0;
	if (read_sched_config_num(schedid, &config_num) > 0)
	{
		int idx = 2 * sizeof(uint8_t) + config_num * sizeof(ScheduleConfig) + 2 * sizeof(uint8_t) + MAX_NAME_SIZE;
		//return File_Read_Schdu(schedid, (uint8_t *) desc, idx + sizeof(uint8_t), MAX_DESCR_SIZE);
		return schedule_data_read_info(schedid, idx + sizeof(uint8_t), desc, MAX_DESCR_SIZE);
	}
	else
	{
		return -1;
	}
}

/*写入schedule信息
 *
 *作用:	写入schedule信息
 *
 *参数:	schedid				schedule id
 *		buf					读取信息缓存区
 *		buf_size			读取schedule信息大小
 *
 *返回:	成功>0，失败<0
 */
int write_sched_info (uint8_t schedid, char buf[], int buf_size)
{
	if (buf == NULL || buf_size < 0)
	{
		return -1;
	}

	//return File_Write_Schdu(schedid, (uint8_t *) buf, 0, buf_size);
	return schedule_data_write_info(schedid, 0, buf, buf_size);
}

/*获取最后一个schedule的id
 *
 *作用:	获取最后一个schedule的id
 *
 *参数:
 *
 *返回:	如果schedule为空则返回0，否则返回最后一个schedule的id
 */
int get_new_sched_id ()
{
	uint8_t i, id = 0;
	uint8_t ids[MUI_SCHEDULE_DATA_NUM_MAX] = "";
	int n;
	/*int n = Get_Schdu_Num();
	Copy_Schdu_Id(ids, n);*/
	n = schedule_data_get_id_list(ids, MUI_SCHEDULE_DATA_NUM_MAX);
	bubble_sort(ids, n);

	if (n == 0 || ids[0] > 1)
	{
		return 1;
	}

	for (i = 0; i < n; i++)
	{
		if (i == n - 1)
		{
			id = ids[i] + 1;
		}
		else
		{
			if (ids[i + 1] - ids[i] > 1)
			{
				id = ids[i] + 1;
				return id;
			}
		}
	}

	return id;
}

/*删除schedule
 *
 *作用:	删除schedule
 *
 *参数:	schedid					schedule的id
 *
 *返回:	成功返回>0，失败返回<0
 */
int del_sched (uint8_t schedid)
{
	//return File_Del_Schdu(schedid);
	return schedule_data_del(schedid);
}

/*获取所有的schedule
 *
 *作用:	获取所有的schedule
 *
 *参数:	psConntCtrl			控制结构体指针
 *		send			发送缓存区
 *		sendsize		发送缓存区的大小
 *
 *返回:
 */
void get_all_schedules (S_ConntCtrl *psConntCtrl, char send[], int sendsize)
{
	char *json = create_reply_get_all_schedules();
	create_packet_reply(json, psConntCtrl, send, sendsize);
	free(json);
}

/*创建schedule
 *
 *作用:	创建schedule并回包
 *
 *参数:	psConntCtrl			控制结构体指针
 *		send			发送缓存区
 *		sendsize		发送缓存区的大小
 *		buf				schedule数据字符串
 *		size			schedule数据大小
 *
 *返回: 成功返回创建的schedule的id，失败返回0
 */
int create_schedule (S_ConntCtrl *psConntCtrl, char send[], int sendsize, char buf[], int size)
{
	int n = 0;
	CJSONData jsondata[1];
	int ret = 0;
	/*uint8_t id = get_new_sched_id();
	buf[0] = id;
	if (size > 0)
	{
		ret = write_sched_info(id, buf, size - sizeof(ScheduleSetOption));
	}*/
	ret = schedule_data_add(buf, size - sizeof(ScheduleSetOption));

	jsondata[n].result = ret > 0 ? 1 : 0;
	jsondata[n].type = 3;
	sprintf(jsondata[n].string, "%d", ret);
	strcpy(jsondata[n].name, "id");
	n++;
	create_packet(jsondata, n, psConntCtrl, send, sendsize);

	if (jsondata[n - 1].result == 1)
	{
		return ret;
	}
	else
	{
		return 0;
	}
}

/*获取schedule属性
 *
 *作用:	获取schedule属性并回包
 *
 *参数:	id_num			schedule的id
 *		psConntCtrl			控制结构体指针
 *		send			发送缓存区
 *		sendsize		发送缓存区的大小
 *
 *返回:
 */
void get_sched_attributes (uint8_t id_num, S_ConntCtrl *psConntCtrl, char send[], int sendsize)
{
	char *json = create_reply_get_sched_attributes(id_num);
	create_packet_reply(json, psConntCtrl, send, sendsize);
	free(json);
}

/*删除schedule
 *
 *作用:	删除schedule并回包
 *
 *参数:	id_num			schedule的id
 *		psConntCtrl			控制结构体指针
 *		send			发送缓存区
 *		sendsize		发送缓存区的大小
 *
 *返回:	成功返回1，失败返回0
 */
int delete_sched (uint8_t id_num, S_ConntCtrl *psConntCtrl, char send[], int sendsize)
{
	int n = 0;
	CJSONData jsondata[1];
	jsondata[n].result = del_sched(id_num) > 0 ? 1 : 0;
	jsondata[n].type = 4;
	sprintf(jsondata[n].name, "/schedules/%d deleted.", id_num);
	n++;
	create_packet(jsondata, n, psConntCtrl, send, sendsize);
	return jsondata[n - 1].result;
}

/*设置schedule属性
 *
 *作用:	设置schedule属性并回包
 *
 *参数:	id			schedule的id
 *		buf				要设置的属性的数据
 *		size			要设置的属性的数据的大小
 *		psConntCtrl			控制结构体指针
 *		send			发送缓存区
 *		sendsize		发送缓存区的大小
 *
 *返回:
 */
void set_schedules_attributes (uint8_t id, char buf[], int size, S_ConntCtrl *psConntCtrl, char send[], int sendsize)
{
	ScheduleAttr attr;
	ScheduleSetOption option;
	memset(&attr, 0, sizeof(ScheduleAttr));
	memset(&option, 0, sizeof(ScheduleSetOption));
	int n = 0;
	CJSONData jsondata[MAX_NUM_JSON_DATA];
	int idx = 0;
	uint8_t config_num = buf[sizeof(uint8_t)];
	int s = 2 * sizeof(uint8_t) + config_num * sizeof(ScheduleConfig);
	memcpy(&attr, buf + s, sizeof(ScheduleAttr));
	memcpy(&option, buf + s + sizeof(ScheduleAttr), sizeof(ScheduleSetOption));
	memset(buf, 0, size);

	int len = read_sched_info(id, buf, size);
	config_num = buf[sizeof(uint8_t)];
	s = 2 * sizeof(uint8_t) + config_num * sizeof(ScheduleConfig);

	if (len <= 0)
	{
		return;
	}

	if (option.active == 1)
	{
		idx = s;
		buf[idx] = attr.active;
		jsondata[n].type = 1;
		jsondata[n].intdata = attr.active;
		sprintf(jsondata[n].name, "/schedules/%d/active", id);
		jsondata[n].result = 1;
		n++;
	}

	if (option.repeat == 1)
	{
		idx = s + sizeof(uint8_t);
		buf[idx] = attr.repeat;
		jsondata[n].type = 2;
		jsondata[n].intdata = attr.repeat;
		sprintf(jsondata[n].name, "/schedules/%d/repeat", id);
		jsondata[n].result = 1;
		n++;
	}

	if (option.time == 1)
	{
		idx = s + 2 * sizeof(uint8_t);
		memcpy(buf + idx, attr.time, MAX_TIME_SIZE);
		jsondata[n].type = 3;
		strcpy(jsondata[n].string, attr.time);
		sprintf(jsondata[n].name, "/schedules/%d/time", id);
		jsondata[n].result = 1;
		n++;
	}

	if (option.name == 1)
	{
		idx = s + 2 * sizeof(uint8_t) + MAX_TIME_SIZE;
		memcpy(buf + idx, attr.name, MAX_NAME_SIZE);
		idx += MAX_NAME_SIZE;
		jsondata[n].type = 3;
		strcpy(jsondata[n].string, attr.name);
		sprintf(jsondata[n].name, "/schedules/%d/name", id);
		jsondata[n].result = 1;
		n++;
	}

	if (option.desc == 1)
	{
		idx = s + 2 * sizeof(uint8_t) + MAX_TIME_SIZE + MAX_NAME_SIZE;
		memcpy(buf + idx, attr.desc, MAX_DESCR_SIZE);
		jsondata[n].type = 3;
		strcpy(jsondata[n].string, attr.desc);
		sprintf(jsondata[n].name, "/schedules/%d/description", id);
		jsondata[n].result = 1;
		n++;
	}

	write_sched_info(id, buf, len);

	create_packet(jsondata, n, psConntCtrl, send, sendsize);
}

/*
 * 处理schedule API函数
 * 作用:	处理schedule
 *
 * 参数:	method					方法
 *		url						url字符串指针
 *		jsonstr					json包
 *		psConntCtrl			控制结构体指针
 *		send			发送缓存区
 *		sendsize		发送缓存区的大小
 *
 * 返回:
 */
void trans_schedules_part (uint8_t method, char *url, char *jsonstr, S_ConntCtrl *psConntCtrl, char send[], int sendsize)
{
	uint8_t id_num;
	char *p1 = strrchr(url, '/');
	if (p1 == NULL)
	{
		return;
	}

	if (method == MUI_HTTP_METHOD_GET)
	{
		if (strcmp(p1, "/schedules") == 0)
		{
			get_all_schedules(psConntCtrl, send, sendsize);
		}
		else
		{
			id_num = atoi(p1 + 1);
			if (id_num > 0)
			{
				if (read_sched_id(id_num) > 0)
				{
					get_sched_attributes(id_num, psConntCtrl, send, sendsize);
				}
			}
		}
	}
	else if (method == MUI_HTTP_METHOD_POST)
	{
		if (strcmp(p1, "/schedules") == 0)
		{
			char buf[1024] = "";
			int len = parse_json_string_schedule(jsonstr, buf, 1024);
			id_num = create_schedule(psConntCtrl, send, sendsize, buf, len);
			if (id_num > 0)
			{
				if (set_change_schdu(id_num, 2) < 0)
				{
					return;
				}
			}
		}
	}
	else if (method == MUI_HTTP_METHOD_PUT)
	{
		id_num = atoi(p1 + 1);
		if (id_num > 0)
		{
			if (read_sched_id(id_num) > 0)
			{
				char buf[1024] = "";
				if (parse_json_string_schedule(jsonstr, buf, 1024) > 0)
				{
					set_schedules_attributes(id_num, buf, 1024, psConntCtrl, send, sendsize);
				}
			}
		}
	}
	else if (method == MUI_HTTP_METHOD_DELETE)
	{
		id_num = atoi(p1 + 1);
		if (id_num > 0)
		{
			if (read_sched_id(id_num) > 0)
			{
				if (delete_sched(id_num, psConntCtrl, send, sendsize) > 0)
				{
					if (set_change_schdu(id_num, 0) < 0)
					{
						return;
					}
				}
			}
		}
	}
}

/*
 *schedule执行函数
 *作用: 执行schedule
 *
 *参数:	type		dev_type
 *		id		dev_id
 *		action_type		action_type
 *		value1			action_value1
 *		value2			action_value2
 *
 *返回:
 */
void process_schedules (uint8_t type, uint8_t id, uint8_t action_type, uint16_t value1, uint16_t value2)
{
	if (type == MUI_DATA_TYPE_BIN_CTRL)
	{
		uint8_t on = value1;
		set_bin_ctrl_state_on(id, &on);
	}
	else if (type == MUI_DATA_TYPE_LEVEL_CTRL)
	{
		set_level_ctrl_state_level(id, &value1);
	}
	else if (type == MUI_DATA_TYPE_WARN_DEV)
	{
		S_WarnCfg warn_cfg;
		warn_cfg.warnmode = value1;
		warn_cfg.duration = value2;
		set_warn_dev_state_warn_cfg(id, &warn_cfg);
	}
	else if (type == MUI_DATA_TYPE_PLUG)
	{
		uint8_t on = value1;
		set_plug_state_on(id, &on);
	}
	else if (type == MUI_DATA_TYPE_SWITCH)
	{
		uint8_t on = value1;
		set_switch_state_on(id, &on);
	}
	else if (type == MUI_DATA_TYPE_SHADE)
	{
		uint8_t action = value1;
		set_shade_state_action(id, &action);
	}
	else if (type == MUI_DATA_TYPE_LIGHT)
	{
		if (action_type == 1)
		{
			uint8_t on = value1;
			set_light_state_on(id, &on);
		}
		else if (action_type == 2)
		{
			uint8_t bri = value1;
			set_light_state_bri(id, &bri);
		}
		else if (action_type == 3)
		{
			S_Color_XY xy;
			xy.x = value1;
			xy.y = value2;
			set_light_state_xy(id, &xy);
		}
	}
	else if (type == MUI_DATA_TYPE_GROUP)
	{
		//set_groups_state(id);
	}
	else if (type == MUI_DATA_TYPE_SET)
	{
		set_active(id);
	}
}

int schedules_device_id_check (const uint8_t type, const uint8_t id)
{
	int n = 0;
	uint8_t i, j, count, config_num;
	count = 0;
	ScheduleConfig config;
	char buf[1024] = "";
	int idx = 0;
	uint16_t buf_id[MUI_SCHEDULE_DATA_NUM_MAX];
	/*n = Get_Schdu_Num();
	Copy_Schdu_Id(buf_id, n);*/
	n = schedule_data_get_id_list(buf_id, MUI_SCHEDULE_DATA_NUM_MAX);

	for (i = 0; i < n; i++)
	{
		if (read_sched_info(buf_id[i], buf, 1024) > 0)
		{
			idx = 0;
			idx += sizeof(uint8_t);
			config_num = buf[idx];
			idx += sizeof(uint8_t);

			for (j = 0; j < config_num; j++)
			{
				memcpy(&config, buf + idx, sizeof(ScheduleConfig));
				idx += sizeof(ScheduleConfig);
				if (type == config.type && id == config.id)
				{
					count++;
				}
			}
		}
	}

	return count;
}

int schedules_device_id_delete (const uint8_t type, const uint8_t id)
{
	int count = 0;
	uint8_t n = 0;
	uint8_t i, j, find_flag, config_num;
	find_flag = 0;
	ScheduleConfig config;
	char buf[1024] = "";
	char write_buf[1024] = "";
	int idx = 0;
	uint8_t buf_id[MUI_SCHEDULE_DATA_NUM_MAX] = "";
	/*n = Get_Schdu_Num();
	Copy_Schdu_Id(buf_id, n);*/
	n = schedule_data_get_id_list(buf_id, MUI_SCHEDULE_DATA_NUM_MAX);

	for (i = 0; i < n; i++)
	{
		if (read_sched_info(buf_id[i], buf, 1024) > 0)
		{
			idx = 0;
			idx += sizeof(uint8_t);
			config_num = buf[idx];
			idx += sizeof(uint8_t);

			for (j = 0; j < config_num; j++)
			{
				memcpy(&config, buf + idx, sizeof(ScheduleConfig));
				idx += sizeof(ScheduleConfig);
				if (type == config.type && id == config.id)
				{
					find_flag = 1;
					count++;
				}
			}
		}

		if (find_flag == 1)
		{
			find_flag = 0;
			idx = 0;
			write_buf[idx] = buf[0];
			idx += sizeof(uint8_t);
			write_buf[idx] = buf[1];
			idx += sizeof(uint8_t);
			write_buf[idx] = 0;  //config_num = 0;
			idx += sizeof(uint8_t);
			memcpy(write_buf + idx, buf + idx + config_num * sizeof(ScheduleConfig), sizeof(ScheduleAttr));
			idx += sizeof(ScheduleAttr);
		}

		if (write_sched_info(buf_id[i], write_buf, idx) < 0)
		{
			return -1;
		}
	}

	return count;
}

int schedules_device_id_replace (const uint8_t type, const uint8_t dst_id, const uint8_t src_id)
{
	UNUSED(type);
	UNUSED(dst_id);
	UNUSED(src_id);

	return 0;
}

int schedules_device_id_change (const uint8_t type, const uint8_t dst_id, const uint8_t src_id)
{
	int count = 0;
	uint8_t n = 0;
	uint8_t i, j, config_num;
	ScheduleConfig config;
	char buf[1024] = "";
	int idx = 0;
	uint8_t buf_id[MUI_SCHEDULE_DATA_NUM_MAX] = "";
	/*n = Get_Schdu_Num();
	Copy_Schdu_Id(buf_id, n);*/
	n = schedule_data_get_id_list(buf_id, MUI_SCHEDULE_DATA_NUM_MAX);

	for (i = 0; i < n; i++)
	{
		if (read_sched_info(buf_id[i], buf, 1024) > 0)
		{
			idx = 0;
			idx += sizeof(uint8_t);
			config_num = buf[idx];
			idx += sizeof(uint8_t);

			for (j = 0; j < config_num; j++)
			{
				memcpy(&config, buf + idx, sizeof(ScheduleConfig));
				if (type == config.type && src_id == config.id)
				{
					config.id = dst_id;
					memcpy(buf + idx, &config, sizeof(ScheduleConfig));
					count++;
				}
				else if (type == config.type && dst_id == config.id)
				{
					config.id = src_id;
					memcpy(buf + idx, &config, sizeof(ScheduleConfig));
					count++;
				}

				idx += sizeof(ScheduleConfig);
			}

			idx += sizeof(ScheduleAttr);
		}

		if (write_sched_info(buf_id[i], buf, idx) < 0)
		{
			return -1;
		}
	}

	return count;
}

/*int schedules_device_id_set_to (const uint8_t type, const uint8_t dst_id, const uint8_t src_id)
{
	int count = 0;
	uint8_t n = 0;
	uint8_t i, j, config_num;
	ScheduleConfig config;
	char buf[1024] = "";
	int idx = 0;
	uint8_t buf_id[MUI_SCHEDULE_DATA_NUM_MAX] = "";

	n = schedule_data_get_id_list(buf_id, MUI_SCHEDULE_DATA_NUM_MAX);

	for (i = 0; i < n; i++)
	{
		if (read_sched_info(buf_id[i], buf, 1024) > 0)
		{
			idx = 0;
			idx += sizeof(uint8_t);
			config_num = buf[idx];
			idx += sizeof(uint8_t);

			for (j = 0; j < config_num; j++)
			{
				memcpy(&config, buf + idx, sizeof(S_ScheduleConfig));
				if (type == config.type && src_id == config.id)
				{
					config.id = dst_id;
					memcpy(buf + idx, &config, sizeof(S_ScheduleConfig));
					count++;
				}
				idx += sizeof(S_ScheduleConfig);
			}

			idx += sizeof(S_ScheduleAttr);
		}

		if (write_sched_info(buf_id[i], buf, idx) < 0)
		{
			return -1;
		}
	}

	return count;
}
*/
#endif
//-----------------------------------------------------------------------------------------------------------------------------------------------
S_Schedule_Ram G_Schedule_Ram;
S_ScheduleOutPutQuene 	G_Schedule_OutPut_Quene;
static pthread_t s_Schedule_DealThread; /* 线程ID*/
static sem_t s_tScheduleRamDataSem;


static int schedule_ram_data_repeat_change(uint16_t ui16Id, uint8_t *ui8Repeat);
static int schedule_ram_data_time_change(uint16_t ui16Id, S_ModeTime *psRTime);
static int schedule_ram_data_add(uint16_t ui16Id, S_ApiParseSchedule *psSchedule);
static int schedule_ram_data_del();
static int schedule_execute_check(uint8_t ui8Repeat, S_ModeTime *psScheduleTime);
static int schedule_RamData_init();
static void schedule_output_quene_add(S_ScheduleOutPutState *psScheduleState, uint8_t ui8t);
static int schedule_output_active ();


int schedule_get_id_list(uint16_t *pui16Buf, uint16_t ui16Capacity)
{
	ASSERT(NULL != pui16Buf);
	ASSERT(ui16Capacity <= MUI_SCHEDULE_DATA_NUM_MAX);

	return schedule_data_get_id_list(pui16Buf, ui16Capacity);
}

static int schedule_ram_data_active_change(uint16_t ui16Id, uint8_t *ui8Active)
{
	if (0 <= sem_wait_ms(&s_tScheduleRamDataSem, 10000))
	{
		G_Schedule_Ram.ScheduleRamData[ui16Id].active = *ui8Active;
	}
	sem_post(&s_tScheduleRamDataSem);
	return 1;
}

static int schedule_ram_data_repeat_change(uint16_t ui16Id, uint8_t *ui8Repeat)
{
	if (0 <= sem_wait_ms(&s_tScheduleRamDataSem, 10000))
	{
		G_Schedule_Ram.ScheduleRamData[ui16Id].repeat = *ui8Repeat;
	}
	sem_post(&s_tScheduleRamDataSem);
	return 1;
}

static int schedule_ram_data_time_change(uint16_t ui16Id, S_ModeTime *psRTime)
{
	if (0 <= sem_wait_ms(&s_tScheduleRamDataSem, 10000))
	{
		memcpy(&G_Schedule_Ram.ScheduleRamData[ui16Id].time, psRTime, sizeof(S_ModeTime));
	}
	sem_post(&s_tScheduleRamDataSem);
	return 1;
}

static int schedule_ram_data_add(uint16_t ui16Id, S_ApiParseSchedule *psSchedule)
{
	uint16_t buf_id[MUI_SCHEDULE_DATA_NUM_MAX] = {0};
	int schedule_num;
	int j;
	if (0 <= sem_wait_ms(&s_tScheduleRamDataSem, 10000))
	{
		G_Schedule_Ram.ScheduleNum++;

		schedule_num = schedule_get_id_list(buf_id, sizeof(buf_id)/sizeof(buf_id[0]));
		if (schedule_num < 0)
		{
			sem_post(&s_tScheduleRamDataSem);
			return schedule_num;
		}
		if (schedule_num != G_Schedule_Ram.ScheduleNum)
		{
			sem_post(&s_tScheduleRamDataSem);
			return -1;
		}
		memcpy(G_Schedule_Ram.IdList, buf_id, sizeof(buf_id)/sizeof(buf_id[0]));

		G_Schedule_Ram.ScheduleRamData[ui16Id].repeat = psSchedule->ScheduleAttr.repeat;
		memcpy(&G_Schedule_Ram.ScheduleRamData[ui16Id].time, &psSchedule->ScheduleAttr.time, sizeof(S_ModeTime));
		G_Schedule_Ram.ScheduleRamData[ui16Id].outputNum = psSchedule->ScheduleConfig.OutputNum;
		for (j=0; j<G_Schedule_Ram.ScheduleRamData[ui16Id].outputNum; j++)
		{
			G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].DevType = psSchedule->ScheduleConfig.OutputAction[j].DevType;
			G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].DevId = psSchedule->ScheduleConfig.OutputAction[j].DevId;
			G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].delay = psSchedule->ScheduleConfig.OutputAction[j].delay;
			G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].duration = psSchedule->ScheduleConfig.OutputAction[j].duration;
			G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].ActionType = psSchedule->ScheduleConfig.OutputAction[j].ActionType;
			memcpy(&G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].ActionValue, &psSchedule->ScheduleConfig.OutputAction[j].ActionValue, sizeof(U_ScheduleAction));
			G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].quitmethod = psSchedule->ScheduleConfig.OutputAction[j].quitmethod;
			G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].quitType = psSchedule->ScheduleConfig.OutputAction[j].quitType;
			memcpy(&G_Schedule_Ram.ScheduleRamData[ui16Id].OutPut[j].quitValue, &psSchedule->ScheduleConfig.OutputAction[j].quitValue, sizeof(U_ScheduleAction));
		}
	}
	else
	{
		SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[schedule_ram_data_add] Call 'sem_wait_ms' fail!\r\n");
		return -1;
	}
	sem_post(&s_tScheduleRamDataSem);
	SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"schdeule add num = %d\r\n", G_Schedule_Ram.ScheduleNum);
	return ui16Id;
}

static int schedule_ram_data_del()
{
	uint16_t buf_id[MUI_SCHEDULE_DATA_NUM_MAX] = {0};
	int schedule_num;
	int iRet=0;
	if (0 <= sem_wait_ms(&s_tScheduleRamDataSem, 10000))
	{
		G_Schedule_Ram.ScheduleNum--;

		schedule_num = schedule_get_id_list(buf_id, sizeof(buf_id)/sizeof(buf_id[0]));
		if (schedule_num < 0)
		{
			if (schedule_num == FILE_S_ERR_DIR_NOT_EXIST || schedule_num == FILE_S_ADDR_ERR)
			{
				SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[schedule_ram_data_del] Call 'schedule_get_id_list' fail :%d,will set 0\r\n", schedule_num);
				schedule_num= 0;
			}else
			{
				SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[schedule_ram_data_del] Call 'schedule_get_id_list' fail :%d\r\n", schedule_num);
				sem_post(&s_tScheduleRamDataSem);
				return schedule_num;
			}
		}
		if (schedule_num != G_Schedule_Ram.ScheduleNum)
		{
			sem_post(&s_tScheduleRamDataSem);
			return -1;
		}
		memcpy(G_Schedule_Ram.IdList, buf_id, sizeof(buf_id)/sizeof(buf_id[0]));
	}
	else
	{
		SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[schedule_ram_data_del] Call 'sem_wait_ms' fail!\r\n");
		iRet = -1;
		return iRet;
	}
	sem_post(&s_tScheduleRamDataSem);
	return iRet;
}

int schedule_add(S_ApiParseSchedule *psSchedule)
{
	int iRet = 0;
	ASSERT(NULL != psSchedule);
	iRet = schedule_data_add(psSchedule, sizeof(S_ApiParseSchedule));
	if (iRet < 0)
	{
		return iRet;
	}
	return schedule_ram_data_add(iRet, psSchedule);
}

int schedule_del(uint16_t ui16Id)
{
	int iRet = 0;

	iRet = schedule_data_del(ui16Id);
	if (iRet < 0)
	{
		return iRet;
	}
	return schedule_ram_data_del();
}

int schedule_read_info(uint16_t ui16Id, S_ApiParseSchedule *psSchedule)
{
	ASSERT(NULL != psSchedule);
	return schedule_data_read_info(ui16Id, 0, psSchedule, sizeof(S_ApiParseSchedule));
}

int schedule_read_name(uint16_t ui16Id, S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.name), psName, sizeof(S_AttrName));
}

int schedule_read_desc(uint16_t ui16Id, S_AttrDescr *psDesc)
{
	ASSERT(NULL != psDesc);
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.descr), psDesc, sizeof(S_AttrDescr));
}

int schedule_read_active(uint16_t ui16Id, uint8_t *ui8Active)
{
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.active), ui8Active, sizeof(uint8_t));
}

int schedule_read_repeat(uint16_t ui16Id, uint16_t *ui16Repeat)
{
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.repeat), ui16Repeat, sizeof(uint16_t));
}

int schedule_read_time(uint16_t ui16Id, S_ModeTime *psTime)
{
	ASSERT(NULL != psTime);
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.time), psTime, sizeof(S_ModeTime));
}

int schedule_read_attr(uint16_t ui16Id, S_ScheduleAttr *psAttr)
{
	ASSERT(NULL != psAttr);
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr), psAttr, sizeof(S_ScheduleAttr));
}

int schedule_read_outputnum(uint16_t ui16Id, const uint8_t *ui8OutputNum)
{
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleConfig.OutputNum), (void*)ui8OutputNum, sizeof(uint8_t));
}

//ui8Idx从0开始，0表示第一个output
int schedule_read_output(uint16_t ui16Id, uint8_t ui8Idx, S_ScheduleOutPutState *psOutPut)
{
	ASSERT(NULL != psOutPut);
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleConfig)+sizeof(S_ScheduleOutPutState)*ui8Idx, psOutPut, sizeof(S_ScheduleOutPutState));
}

int schedule_read_config(uint16_t ui16Id, char *psConfig)
{
	ASSERT(NULL != psConfig);
	return schedule_data_read_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleConfig), psConfig, sizeof(S_ModeTime));
}


int schedule_write_info(uint16_t ui16Id, const S_ApiParseSchedule *psSchedule)
{
	ASSERT(NULL != psSchedule);
	return schedule_data_write_info(ui16Id, 0, (void*)psSchedule, sizeof(S_ApiParseSchedule));
}

int schedule_write_name(uint16_t ui16Id, S_AttrName *psName)
{
	ASSERT(NULL != psName);
	return schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.name), psName, sizeof(S_AttrName));
}

int schedule_write_desc(uint16_t ui16Id, S_AttrDescr *psDesc)
{
	ASSERT(NULL != psDesc);
	return schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.descr), psDesc, sizeof(S_AttrDescr));
}

int schedule_write_active(uint16_t ui16Id, uint8_t *ui8Active)
{
	int iRet;
	iRet = schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.active), ui8Active, sizeof(uint8_t));
	if (iRet < 0)
	{
		return iRet;
	}

	return schedule_ram_data_active_change(ui16Id, ui8Active);
}

int schedule_write_repeat(uint16_t ui16Id, uint8_t *ui8Repeat)
{
	int iRet;
	iRet = schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.repeat), ui8Repeat, sizeof(uint8_t));
	if (iRet < 0)
	{
		return iRet;
	}

	return schedule_ram_data_repeat_change(ui16Id, (void*)ui8Repeat);
}

int schedule_write_time(uint16_t ui16Id, S_ModeTime *psTime)
{
	int iRet;
	ASSERT(NULL != psTime);
	iRet = schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr.time), psTime, sizeof(S_ModeTime));
	if (iRet < 0)
	{
		return iRet;
	}
	return schedule_ram_data_time_change(ui16Id, psTime);
}

int schedule_write_attr(uint16_t ui16Id, S_ScheduleAttr *psAttr)
{
	ASSERT(NULL != psAttr);
	return schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleAttr), psAttr, sizeof(S_ScheduleAttr));
}

int schedule_write_outputnum(uint16_t ui16Id, const uint8_t *ui8OutputNum)
{
	return schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleConfig.OutputNum), (void*)ui8OutputNum, sizeof(uint8_t));
}

//ui8Idx从0开始，0表示第一个output
int schedule_write_output(uint16_t ui16Id, uint8_t ui8Idx, S_ScheduleOutPutState *psOutPut)
{
	ASSERT(NULL != psOutPut);
	return schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleConfig)+sizeof(S_ScheduleOutPutState)*ui8Idx, psOutPut, sizeof(S_ScheduleOutPutState));
}

int schedule_write_config(uint16_t ui16Id, char *psConfig)
{
	ASSERT(NULL != psConfig);
	return schedule_data_write_info(ui16Id, M_OFFSET(S_ApiParseSchedule, ScheduleConfig), psConfig, sizeof(S_ModeTime));
}


static int schedule_RamData_init()
{
	int iRet = 0, i, j;
	int schedule_num = 0;
	uint16_t buf_id[MUI_SCHEDULE_DATA_NUM_MAX] = {0};
	S_ApiParseSchedule  ScheduleFlashData;

	schedule_num = schedule_get_id_list(buf_id, sizeof(buf_id)/sizeof(buf_id[0]));
	if (schedule_num < 0)
	{
		//SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_RamData_init] rules_get_id_list fail.:%d\r\n", schedule_num);
		return schedule_num;
	}
	G_Schedule_Ram.ScheduleNum = schedule_num;
	memcpy(G_Schedule_Ram.IdList, buf_id, sizeof(buf_id)/sizeof(buf_id[0]));

	for (i=0; i< schedule_num; i++)
	{
		iRet = schedule_read_info(buf_id[i], &ScheduleFlashData);
		if (iRet < 0)
		{
			//RULES_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_RamData_init] rules_read_info fail.:%d\r\n", iRet);
			continue;
		}
		G_Schedule_Ram.ScheduleRamData[buf_id[i]].active = ScheduleFlashData.ScheduleAttr.active;
		G_Schedule_Ram.ScheduleRamData[buf_id[i]].repeat = ScheduleFlashData.ScheduleAttr.repeat;
		memcpy(&G_Schedule_Ram.ScheduleRamData[buf_id[i]].time, &ScheduleFlashData.ScheduleAttr.time, sizeof(S_ModeTime));
		G_Schedule_Ram.ScheduleRamData[buf_id[i]].outputNum = ScheduleFlashData.ScheduleConfig.OutputNum;
		for (j=0; j<G_Schedule_Ram.ScheduleRamData[buf_id[i]].outputNum; j++)
		{
			G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].DevType = ScheduleFlashData.ScheduleConfig.OutputAction[j].DevType;
			G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].DevId = ScheduleFlashData.ScheduleConfig.OutputAction[j].DevId;
			G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].delay = ScheduleFlashData.ScheduleConfig.OutputAction[j].delay;
			G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].duration = ScheduleFlashData.ScheduleConfig.OutputAction[j].duration;
			G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].ActionType = ScheduleFlashData.ScheduleConfig.OutputAction[j].ActionType;
			memcpy(&G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].ActionValue, &ScheduleFlashData.ScheduleConfig.OutputAction[j].ActionValue, sizeof(U_ScheduleAction));
			G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].quitmethod = ScheduleFlashData.ScheduleConfig.OutputAction[j].quitmethod;
			G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].quitType = ScheduleFlashData.ScheduleConfig.OutputAction[j].quitType;
			memcpy(&G_Schedule_Ram.ScheduleRamData[buf_id[i]].OutPut[j].quitValue, &ScheduleFlashData.ScheduleConfig.OutputAction[j].quitValue, sizeof(U_ScheduleAction));
		}
	}

	return iRet;
}


static int schedule_execute_check(uint8_t ui8Repeat, S_ModeTime *psScheduleTime)
{
	struct tm 	*timep;
	time_t 		now;
	uint32_t	Schedule_time;
	uint32_t	Now_time;

	time(&now);

	timep = localtime(&now);

	Now_time = mktime(timep);
	Schedule_time = mktime(&psScheduleTime->Time);

	if (psScheduleTime->time_mode == 1)
	{
		if (ui8Repeat&(1<<psScheduleTime->Time.tm_wday))
		{
			if (ui8Repeat&(1<<7))
			{
				//SCHEDULE_DYN_PRINT(M_DBG_LEVEL_ERR,"repeat successful\r\n");
				//SCHEDULE_DYN_PRINT(M_DBG_LEVEL_ERR,"time ==%d:%d:%d=======%d:%d:%d\r\n",psScheduleTime->Time.tm_hour, psScheduleTime->Time.tm_min, psScheduleTime->Time.tm_sec, timep->tm_hour, timep->tm_min, timep->tm_sec);
				if (psScheduleTime->Time.tm_hour == timep->tm_hour && psScheduleTime->Time.tm_min == timep->tm_min && psScheduleTime->Time.tm_sec == timep->tm_sec)
				{
					return MUI_SCHEDULE_TIME_CHECK_TRUE;
				}
			}else
			{
				Schedule_time = mktime(&psScheduleTime->Time);
				if ((Now_time-Schedule_time) <= 7*24*3600)
				{
					if (psScheduleTime->Time.tm_hour == timep->tm_hour && psScheduleTime->Time.tm_min == timep->tm_min && psScheduleTime->Time.tm_sec == timep->tm_sec)
					{
						return MUI_SCHEDULE_TIME_CHECK_TRUE;
					}
				}
			}
		}
		return MUI_SCHEDULE_TIME_CHECK_FALSE;
	}
	else if (psScheduleTime->time_mode == 2)
	{
		Schedule_time = mktime(&psScheduleTime->Time);
		if ((Now_time-Schedule_time) == 0 )
		{
			return MUI_SCHEDULE_TIME_CHECK_TRUE;
		}
	}
	return MUI_SCHEDULE_TIME_CHECK_FALSE;
}


static void schedule_output_quene_add(S_ScheduleOutPutState *psScheduleState, uint8_t ui8t)
{
	G_Schedule_OutPut_Quene.sScheduleOutPutAttr[G_Schedule_OutPut_Quene.ScheduleOutPutNum].DevType = psScheduleState->DevType;
	G_Schedule_OutPut_Quene.sScheduleOutPutAttr[G_Schedule_OutPut_Quene.ScheduleOutPutNum].DevId = psScheduleState->DevId;
	if (ui8t == 1)
	{
		G_Schedule_OutPut_Quene.sScheduleOutPutAttr[G_Schedule_OutPut_Quene.ScheduleOutPutNum].ActionType = psScheduleState->ActionType;
		memcpy(&G_Schedule_OutPut_Quene.sScheduleOutPutAttr[G_Schedule_OutPut_Quene.ScheduleOutPutNum].ActionValue, &psScheduleState->ActionValue, sizeof(U_RulesAction));
	}
	else if (ui8t == 2)
	{
		G_Schedule_OutPut_Quene.sScheduleOutPutAttr[G_Schedule_OutPut_Quene.ScheduleOutPutNum].ActionType = psScheduleState->quitType;
		memcpy(&G_Schedule_OutPut_Quene.sScheduleOutPutAttr[G_Schedule_OutPut_Quene.ScheduleOutPutNum].ActionValue, &psScheduleState->quitValue, sizeof(U_RulesAction));
	}
	G_Schedule_OutPut_Quene.ScheduleOutPutNum++;
}


/*
 *
 *
 *
 */
static int schedule_output_active ()
{
	int i, iRet=0, iTmp;

	for (i = 0; i < G_Schedule_OutPut_Quene.ScheduleOutPutNum; i++)
	{
		switch (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionType)
		{
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ON:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_set_state_on(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.on);
				}
				else
				{
					iTmp = set_dev_state_on(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.on);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_BRI:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_light_state_bri(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.bri);
				}
				else
				{
					iTmp = set_dev_state_bri(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.bri);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_SAT:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					//iTmp = group_light_state_sat(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.on);
				}
				else
				{
					iTmp = set_dev_state_sat(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.sat);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUE:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					//iTmp = group_light_state_hue(G_RulesOutPutQuene.sRuelsOutPutAttr[i].DevId, &G_RulesOutPutQuene.sRuelsOutPutAttr[i].action.ActionValue.on);
				}
				else
				{
					iTmp = set_dev_state_hue(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.hue);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_CT:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_light_state_ct(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.ct);
				}
				else
				{
					iTmp = set_dev_state_ct(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.ct);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_XY:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_light_state_xy(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.xy);
				}
				else
				{
					iTmp = set_dev_state_xy(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.xy);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_LEVEL:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_set_state_level(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.level);
				}
				else
				{
					iTmp = set_dev_state_level(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.level);
				}
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_COUNT:
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_TEMP:
			//	iTmp = set_dev_state_temp(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.temp);
			//	break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_HUM:
			//	iTmp = set_dev_state_hum(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.hum);
			//	break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_CONSUMPTION:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_WARN_CFG:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_warn_dev_state_warn_cfg(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.warncfg);
				}
				else
				{
					iTmp = set_dev_state_warn_cfg(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.warncfg);
				}
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTION:
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_GROUP)
				{
					iTmp = group_shade_state_action(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.action);
				}
				else
				{
					iTmp = set_dev_state_action(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.action);
				}
				break;
			//case MUI_SET_DEV_STATE_KEY_WORD_IDX_POWER:
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_PLAY:
				iTmp = set_dev_state_play(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.play);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_VOLUME:
				iTmp = set_dev_state_volume(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.volume);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_URLID:
				iTmp = set_dev_state_url_id(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.urlid);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_EXECUTE:
				iTmp = set_execute_active(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.execute);
				break;
			case MUI_SET_DEV_STATE_KEY_WORD_IDX_ACTIVE :
				if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_RULE)
				{
					iTmp = rules_write_active(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.active);
				}
				else if (G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevType == MUI_DATA_TYPE_SCHED)
				{
					iTmp = schedule_write_active(G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].DevId, &G_Schedule_OutPut_Quene.sScheduleOutPutAttr[i].ActionValue.active);
				}
				break;
			default:
				continue;
		}

	}
	G_Schedule_OutPut_Quene.ScheduleOutPutNum = 0;
	return iRet;
}


//-------------------------------------------------------------------------------------------------*
/*	schedule active处理任务函数
 ;	(功能)
 ;	(参数)
 ;		pArg:	传给函数运行的参数
 ;	(返回)
 ;		NULL
 ;	(注意)
 */
static void* s_schedule_execute_deal_task (void *pArg)
{
	UNUSED(pArg);

	int iRet = 0;
	int i, j;
	G_Schedule_OutPut_Quene.ScheduleOutPutNum = 0;

	iRet = sem_init(&s_tScheduleRamDataSem, 0, 1);	//初始化为1
	if (iRet < 0)
	{
		SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_schedule_execute_deal_task] sem_init fail.:%d\r\n", iRet);
		return ;
	}
	schedule_RamData_init();
	struct tm 	*timep;
	time_t 		now;

	while(1)
	{
		if (0 > sem_wait_ms(&s_tScheduleRamDataSem, 10000))
		{
			SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[s_schedule_execute_deal_task] sem_wait_ms fail.\r\n");
			continue;
		}
		time(&now);
		timep = localtime(&now);
		//SCHEDULE_DYN_PRINT(M_DBG_LEVEL_ERR,"time === %d:%d:%d  schedulenum=%d\r\n", timep->tm_hour, timep->tm_min, timep->tm_sec, G_Schedule_Ram.ScheduleNum);

		for (i=0; i<G_Schedule_Ram.ScheduleNum; i++)
		{
			if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].active == 0)
			{
				continue;
			}
			if (schedule_execute_check(G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].repeat, &G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].time))
			{
				//SCHEDULE_DYN_PRINT(M_DBG_LEVEL_ERR,"ececute check sucessful\r\n");
				if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].execute == 1)
				{
					G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].execute = 2;
				}
				else
				{
					G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].execute = 1;
					G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].finishOutPut = 0;
					for (j=0; j<G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].outputNum; j++)
					{
						G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].delay_count = G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].delay;
						G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration_count = G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration;
					}
				}
			}
			if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].execute > 0)
			{
				for (j=0; j<G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].outputNum; j++)
				{
					if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].execute == 2)
					{
						if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration_count == 0)
						{
							G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].delay_count = G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].delay;
						}
						if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration_count != G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration)
						{
							G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration_count = G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration;
						}
					}
					if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].delay_count == 0)
					{
						if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration_count == 0)
						{
							schedule_output_quene_add(&G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j], 1);
						}
						if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration_count == 0)
						{
							if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].quitmethod == 1)
							{
								schedule_output_quene_add(&G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j], 2);
							}
							G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].delay_count = -1;
							G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration_count = -1;
							G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].finishOutPut++;
							if (G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].finishOutPut == G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].outputNum)
							{
								G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].execute = 0;
							}
						}
						else
						{
							G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].duration_count--;
						}
					}
					else
					{
						G_Schedule_Ram.ScheduleRamData[G_Schedule_Ram.IdList[i]].OutPut[j].delay_count--;
					}
				}
			}
		}
		if (G_Schedule_OutPut_Quene.ScheduleOutPutNum > 0)
		{
			schedule_output_active();
			G_Schedule_OutPut_Quene.ScheduleOutPutNum = 0;
		}

		sem_post(&s_tScheduleRamDataSem);
		msleep(1000);
	}
	return NULL;
}



int schedule_init()
{
	int iRet = 0;

	SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_init] s***********start pthread*************\r\n");
	iRet = pthread_create(&s_Schedule_DealThread, NULL, s_schedule_execute_deal_task, NULL);
	if (iRet < 0)
	{
		SCHEDULE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[rules_init] s**********pthread error*************\r\n");
		return -1;
	}
	return 1;
}
























































































