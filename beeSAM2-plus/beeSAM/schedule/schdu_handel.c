#if 0
#include"schdu_data.h"

/*char g_test_buf[] = {1,87,97,107,101,32,117,112,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,77,121,32,119,97,107,101,32,117,112,32,97,108,97,114,109,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,47,104,97,47,76,85,112,70,82,75,88,112,86,50,102,118,99,53,66,120,47,103,114,111,117,112,115,47,49,47,97,99,116,105,111,110,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,85,84,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,50,48,49,49,45,48,51,45,51,48,84,49,52,58,50,52,58,52,48,0,0,0,0,0,0,0,0,0,0,0,0};

 void schdu_get_buf(char* buf)
 {

 memcpy( buf, g_test_buf, sizeof(g_test_buf));
 }*/

unsigned int g_schdu_id = 0;

unsigned int schdu_tick = 0;

#define INT_TO_LSL( _A_ )   ( (((_A_)>>1)&0x7f)|((_A_)&0x80)|(((_A_)<<6)&0x40))
#define LSL_TO_INT( _A_ )   ( (((_A_)<<1)&0x7f)|((_A_)&0x80)|(((_A_)>>6)&0x1 ))

sem_t Schdu_Semaphore;
sem_t Schdu_Mutex;


static void s_schedu_free (s_schdu* s_tmp);
static void s_schedu_get_time_buf (char* buf, uint8_t flag);
static int s_schedu_time_ok (s_schdu* schdu, int mtime, unsigned char week);
static int s_schedu_week_ok (uint8_t* repeat, const uint8_t week);
static void s_schedu_flick (schdu_dev* dev);
static int s_schedu_handel (schdu_dev* dev);
static int s_schedu_parse_dev (const uint8_t* buf, const uint8_t num);
static int s_schedu_parse (const uint8_t* buf);
static int init_schdu_buf (void);


int schdu_task_create (int* id, void* prm, void* task)
{
	return pthread_create((pthread_t*) id, NULL, task, prm);
}

int schdu_task_wait_quit (void)
{
	pthread_join(g_schdu_id, NULL);
	return 1;
}

#define s_malloc( _A )    malloc( _A )
#define s_free( _A )      free( _A )
#define s_sleepms( _A )   usleep( 1000*(_A) )

s_schdu* g_shead = NULL;
s_schdu* g_scurr = NULL;

/*void free_all(void)
 {
 s_schdu* addr = NULL;
 addr = g_scurr = g_shead;
 while (addr)
 {
 addr = addr->m_next;
 s_free(g_scurr->m_buf);
 g_scurr->m_buf = NULL;
 free(g_scurr);
 g_scurr = addr;
 }
 }
 */
/*
 *	释放schedule资源
 *作用:	释放schedule资源
 *
 *参数: 	s_tmp				s_schde结构体指针
 *
 *返回:
 */
static void s_schedu_free (s_schdu* s_tmp)
{
	if (NULL == s_tmp)
	{
		return;
	}
	schdu_dev* buf[32] =
	{ NULL };
	schdu_dev* tmp = s_tmp->dev_head;

	int num = 0;
	while (tmp)
	{
		buf[num++] = tmp;
		tmp = tmp->dev_next;
	}
	int i = 0;
	for (i = 0; i < num; i++)
	{
		free(buf[i]);
		buf[i] = NULL;
	}
}

/*
 *获取系统时间
 *作用:获取系统时间
 *
 *参数: 	buf		返回格式化时间字符串缓存区
 *		flag	时间格式标志
 *
 *返回:
 */
static void s_schedu_get_time_buf (char* buf, uint8_t flag)
{
	struct tm* timep;
	time_t now;
	time(&now);
	timep = localtime(&now);
	switch (flag)
	{
	case S_YY_MM_T:
		sprintf(buf, "%d-%02d-%02dT%02d:%02d:%02d", timep->tm_year + 1900, timep->tm_mon + 1, timep->tm_mday, timep->tm_hour, timep->tm_min,
				timep->tm_sec);
		break;

	case S_HH_SS_T:
		sprintf(buf, "T%02d:%02d:%02d", timep->tm_hour, timep->tm_min, timep->tm_sec);
		break;

	default:
		break;
	}
	PRINT("[s_schedu_get_time_buf]+++++++++++++++++%s\r\n", buf);
}

/*无用函数*/
static int s_schedu_time_ok (s_schdu* schdu, int mtime, unsigned char week)
{
#ifdef NO_TIME
	UNUSED(schdu);
	UNUSED(mtime);
	UNUSED(week);
	return 1;
#else
	unsigned int m_repeat = 0;
	unsigned int m_start = 0;
	unsigned int m_dur = 0;

	memcpy((void*) &m_start, schdu->time, sizeof(int));
	memcpy((void*) &m_dur, schdu->time + S_END_TIME, sizeof(int));
	m_repeat = schdu->repeat;

	if (0 == m_dur)
	{
		return 0;
	}
	if (0xffffffff == m_dur)
	{
		return 1;
	}
	if (0 == schdu->m_time_count)
	{
		unsigned char repeatf = m_repeat;

		if (!(repeatf & (1 << 7)))
		{
			int i = 0;
			for (i = 0; i < week; i++)
				m_repeat = (m_repeat) & (~(1 << i));
		}
		unsigned int start = mtime & 0x00ffffff;
		if (((mtime >> 23) & 0x7f) & (m_repeat & 0x7f))
		{
			if (start == m_start)
			{
				schdu->repeat = m_dur;
				return 1;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		schdu->m_time_count--;
		return 1;
	}
	return 0;
#endif
}

/*
 *判断今天是否执行schedule
 *作用: 判断今天是否执行schedule
 *
 *参数:	repeat		是否重复
 *		week		今天是星期几
 *
 *返回:	成功1，失败0
 */
static int s_schedu_week_ok (uint8_t* repeat, const uint8_t week)
{
	uint8_t repeat_f = INT_TO_LSL(*repeat);
	if (repeat_f & (1 << week))
	{
		if (repeat_f && (1 << 7))
		{
			return 1;
		}
		else
		{
			char t_buf[20] =
			{ 0 };
			s_schedu_get_time_buf(t_buf, S_HH_SS_T);
			if (strcmp(t_buf, "T23:59:59") == 0)
			{
				repeat_f &= ~(1 << week);
				*repeat = LSL_TO_INT(repeat_f);
				return 1;
			}
			else
			{
				return 1;
			}

		}
	}
	else
	{
		return 0;
	}
	return 0;
}

/*无用函数*/
int set_schdu_active (const uint8_t id, const uint8_t on_off)
{
	sem_wait_ms(&Schdu_Mutex, RULE_MAX_DELAY);
	s_schdu* tmp = NULL;

	tmp = g_shead;
	while (tmp)
	{
		if (id == tmp->schid)
		{
			tmp->active = on_off;
			sem_post(&Schdu_Mutex);
			return 1;
		}
	}

	sem_post(&Schdu_Mutex);
	return 0;
}

static void s_schedu_flick (schdu_dev* dev)
{
	unsigned int on_time = 0;
	unsigned int off_time = 0;
	on_time = ((dev->flick & 0x0000ff00) >> 8);
	off_time = (dev->flick & 0x000000ff);
	if ((0 == dev->action_value1) || (0 == dev->action_value2))
	{
		return;
	}

	if (((dev->flick >> 31) & 1))
	{
		dev->flick &= (~0x0000ff00u);
		if (0 == (schdu_tick % I_HZ))
			on_time--;
		dev->flick |= (on_time << 8);
	}
	else
	{
		dev->flick &= ~(0x000000ffu);
		if (0 == (schdu_tick % I_HZ))
			off_time--;
		dev->flick |= off_time;
	}
	if (0 == on_time)
	{
		PRINT("++++ON+++++++\r\n");
		//process_schedules(dev->dev_type, dev->dev_id, dev->action_type, 0, dev->action_value2);
		dev->flick |= ((dev->action_value1) << 8);
		dev->flick &= ~(1 << 31);
	}
	if (0 == off_time)
	{
		PRINT("++++OFF+++++++\r\n");
		//process_schedules(dev->dev_type, dev->dev_id, dev->action_type, 1, dev->action_value2);
		dev->flick |= dev->action_value1;
		dev->flick |= (unsigned int) (1 << 31);
	}

}

static int s_schedu_handel (schdu_dev* dev)
{

	PRINT("[s_schedu_handel]+++++++++dev->s_delaycount = %d\r\n", dev->s_delaycount);
	PRINT("[s_schedu_handel]+++++++++dev->s_counttime = %d\r\n", dev->s_counttime);
	if (1 == dev->tri_flag)
	{

		if ((0 == dev->s_delay) && (0 == dev->s_dur))
		{
			dev->tri_flag = 2;
			//process_schedules(dev->dev_type, dev->dev_id, dev->action_type, dev->action_value1, dev->action_value2);
		}
		else if (0 == dev->s_delay && (0 == dev->s_counttime))
		{
			dev->s_delaycount = 1;
			dev->s_counttime = dev->s_dur;
		}
		else if ((0 == dev->s_delaycount) && (0 == dev->s_counttime))
		{
			dev->s_delaycount = dev->s_delay + 1;
			dev->s_counttime = dev->s_dur;

		}

		if (1 == dev->s_delaycount && (dev->s_counttime == dev->s_dur))
		{
			if (dev->action_type == I_FLICK)
			{
				//tmp->out_flick = ((tmp->out_value1) << 8) | (tmp->out_value2);
				dev->flick = (dev->action_value1 << 8) | (dev->action_value2);
				dev->flick |= (1 << 31);
			}
			else
			{
				//process_schedules(dev->dev_type, dev->dev_id, dev->action_type, dev->action_value1, dev->action_value2);
			}
		}

		if (schdu_tick % I_HZ == 0)
		{
			//PRINT("I_FLICK=%d    dev->action_type= %d\r\n",I_FLICK, dev->action_type);
			if (dev->s_delaycount == 1)
			{
				if (I_FLICK == dev->action_type)
				{
					s_schedu_flick(dev);
				}
				else
				{
					//process_schedules(dev->dev_type, dev->dev_id, dev->action_type, dev->action_value1, dev->action_value2);
				}
				dev->s_counttime--;

			}
			else
			{

				dev->s_delaycount--;

			}
		}

		if (dev->s_counttime == 0)
		{
			//outquitstate

			dev->tri_flag = 2;
			if (0 == dev->quit_type)
			{
				return 1;
			}
			else
			{
				PRINT("EXE QUIT STSTE\r\n");
				//process_schedules(dev->dev_type, dev->dev_id, dev->action_type, dev->quit_value1, dev->quit_value2);
			}
		}
	}
	else
	{
		if (2 == dev->tri_flag)
		{
			dev->tri_flag = 0;
			g_scurr->exe_out_conut++;
		}
	}

	return 0;

}

/*
 *schdu轮询函数
 *作用:	每一秒轮询一次判断是否触发schedule
 *
 *参数:
 *
 *返回:
 */
void skip_schdu_table (void)
{
	sem_wait_ms(&Schdu_Mutex, RULE_MAX_DELAY);

	schdu_tick++;
	g_scurr = g_shead;
	char t_buf[20] =
	{ 0 };

	while (g_scurr)
	{

		if (g_scurr->active)
		{
			if (!('T' == g_scurr->time[S_START_TIME]))
			{
				memset(t_buf, 0, 20);
				s_schedu_get_time_buf(t_buf, S_YY_MM_T);
				if (g_scurr->active)
				{
					if (0 == strcmp(g_scurr->time, t_buf))
					{
						g_scurr->tri_flag = 1;
						g_scurr->exe_out_conut = 0;
						schdu_dev* d_tmp = g_scurr->dev_head;
						while (d_tmp)
						{
							d_tmp->tri_flag = 1;
							s_schedu_handel(d_tmp);
							d_tmp = d_tmp->dev_next;
						}
					}
					else
					{
						schdu_dev* d_tmp = g_scurr->dev_head;

						if (1 == g_scurr->tri_flag)
						{
							while (d_tmp)
							{
								if (g_scurr->config_num != g_scurr->exe_out_conut)
								{
									s_schedu_handel(d_tmp);
								}
								else
								{
									d_tmp->tri_flag = 0;
									if (NULL == d_tmp->dev_next)
									{
										g_scurr->tri_flag = 0;
									}
								}
								d_tmp = d_tmp->dev_next;
							}
						}
					}
				}
			}
			else
			{
				unsigned char week = 0;
				memset(t_buf, 0, 20);
				s_schedu_get_time_buf(t_buf, S_HH_SS_T);

				get_sec_time(&week);

				if ((strcmp(g_scurr->time, t_buf) == 0) && s_schedu_week_ok(&g_scurr->repeat, week))
				{
					g_scurr->tri_flag = 1;
					g_scurr->exe_out_conut = 0;
					schdu_dev* d_tmp = g_scurr->dev_head;
					while (d_tmp)
					{
						d_tmp->tri_flag = 1;
						s_schedu_handel(d_tmp);
						d_tmp = d_tmp->dev_next;
					}
				}
				else
				{
					schdu_dev* d_tmp = g_scurr->dev_head;

					if (1 == g_scurr->tri_flag)
					{
						while (d_tmp)
						{
							//PRINT("[EXE ++++++++++++++++schdu\r\n]");
							PRINT("g_scurr->config_num = %d  g_scurr->exe_out_conut=%d \r\n", g_scurr->config_num, g_scurr->exe_out_conut);
							if (g_scurr->config_num != g_scurr->exe_out_conut)
							{
								s_schedu_handel(d_tmp);

							}
							else
							{
								d_tmp->tri_flag = 0;
								if (NULL == d_tmp->dev_next)
								{
									g_scurr->tri_flag = 0;
								}
							}
							d_tmp = d_tmp->dev_next;

						}
					}
				}

			}
		}
		g_scurr = g_scurr->m_next;
	}
	sem_post(&Schdu_Mutex);
}

void schdu_task (void* prm)
{
	UNUSED(prm);
	//while (1)
	//{
	skip_schdu_table();
	//check_schdu_change();
	//	s_sleepms(500);
	//}
	//free_all();
}

/*初始化schedule_dev数据函数
 *
 *作用:	初始化schedule_dev数据
 *
 *参数:	buf					schedule output数据
 *		num					output数量
 *
 *返回:	成功>0，失败<0
 */

static int s_schedu_parse_dev (const uint8_t* buf, const uint8_t num)
{
	int pos = 0;
	int i = 0;
	schdu_dev* tmp = NULL;
	for (i = 0; i < num; i++)
	{
		if (NULL == g_scurr->dev_head)
		{
			g_scurr->dev_head = tmp = (schdu_dev*) malloc(sizeof(schdu_dev));
			tmp->dev_type = buf[pos++];
			tmp->dev_id = buf[pos++];
			tmp->action_type = buf[pos++];
			tmp->quit_type = buf[pos++];
			tmp->s_delay = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);

			tmp->s_dur = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);

			tmp->quit_value1 = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);

			tmp->quit_value2 = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);

			tmp->action_value1 = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);
			tmp->action_value2 = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);
			tmp->dev_next = NULL;
			tmp->flick = 0;
			tmp->s_counttime = 0;
			tmp->s_delaycount = 0;
			tmp->tri_flag = 0;

		}
		else
		{
			tmp->dev_next = (schdu_dev*) malloc(sizeof(schdu_dev));
			tmp = tmp->dev_next;
			tmp->dev_type = buf[pos++];
			tmp->dev_id = buf[pos++];
			tmp->action_type = buf[pos++];
			tmp->quit_type = buf[pos++];
			tmp->s_delay = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);

			tmp->s_dur = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);

			tmp->quit_value1 = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);

			tmp->quit_value2 = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);

			tmp->action_value1 = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);
			tmp->action_value2 = (buf[pos] << 8) | (buf[pos + 1]);
			pos += sizeof(uint16_t);
			tmp->dev_next = NULL;
			tmp->flick = 0;
			tmp->s_counttime = 0;
			tmp->s_delaycount = 0;
			tmp->tri_flag = 0;

		}

		//PRINT("tmp->s_dur =%d tmp->s_delay=%d\r\n", tmp->s_dur,tmp->s_delay);
	}
	return pos;

}

/*解析schedule数据
 *
 *作用:	解析schedule数据
 *
 *参数:	buf				schedule数据字符串
 *
 *返回:	schedule数据长度
 */

static int s_schedu_parse (const uint8_t* buf)
{
	int pos = 0;
	if (NULL == g_shead)
	{
		g_shead = g_scurr = (s_schdu*) malloc(sizeof(s_schdu));
		g_scurr->schid = buf[pos++];
		g_scurr->config_num = buf[pos++];
		g_scurr->tri_flag = 0;
		g_scurr->exe_out_conut = 0;
		g_scurr->dev_head = NULL;
		int len = s_schedu_parse_dev(buf + pos, g_scurr->config_num);
		pos += len;
		g_scurr->active = buf[pos++];
		g_scurr->repeat = buf[pos++];
		memcpy(g_scurr->time, buf + pos, S_TIME_LEN);
		g_scurr->m_next = NULL;

	}
	else
	{
		g_scurr->m_next = (s_schdu*) malloc(sizeof(s_schdu));

		g_scurr = g_scurr->m_next;
		g_scurr->exe_out_conut = 0;
		g_scurr->tri_flag = 0;
		g_scurr->schid = buf[pos++];
		g_scurr->config_num = buf[pos++];
		g_scurr->dev_head = NULL;
		int len = s_schedu_parse_dev(buf + pos, g_scurr->config_num);
		pos += len;
		g_scurr->active = buf[pos++];
		g_scurr->repeat = buf[pos++];
		memcpy(g_scurr->time, buf + pos, S_TIME_LEN);
		g_scurr->m_next = NULL;
	}
	return pos;
}

/*初始化schedule数据函数
 *
 *作用:	初始化schedule数据
 *
 *参数:
 *
 *返回:	成功>0，失败<0
 */
static int init_schdu_buf (void)
{
	uint8_t id_buf[256] =
	{ 0 };
	uint8_t data_buf[256] =
	{ 0 };
	int num;
	int len;
	num = schedule_data_get_id_list(id_buf, MUI_SCHEDULE_DATA_NUM_MAX);
	int i = 0;
	for (i = 0; i < num; i++)
	{
		//int len = File_Read_Schdu(id_buf[i], data_buf, 0, sizeof(data_buf));
		len = schedule_data_read_info(id_buf[i], 0, data_buf, sizeof(data_buf));
		if (len <= 0)
		{
			return -1;
		}
		s_schedu_parse(data_buf);
		memset(data_buf, 0, sizeof(data_buf));
	}
	sem_init(&Schdu_Semaphore, 0, 1);
	sem_init(&Schdu_Mutex, 0, 1);

	return 1;

}

/*更新schedule数据
 *
 *作用: 更新schedule在ram中的数据
 *
 *参数:	id					schedule id
 *		flag				schedule改变的模式: 0为delete，1为update，2为add
 *
 *返回:	成功1，失败-1
 */
int set_change_schdu (const uint8_t id, const uint8_t flag)
{
	PRINT("[set_change_schdu]+++++++++++++in id = %d\r\n", id);

	sem_wait_ms(&Schdu_Mutex, RULE_MAX_DELAY);

	int len;
	uint8_t data_buf[256] =
	{ 0 };
	g_scurr = g_shead;
	s_schdu* tmp = NULL;
	s_schdu* tmp1 = NULL;
	tmp = g_scurr;
	while (g_scurr)
	{
		if (id == g_scurr->schid)
		{
			break;
		}
		tmp = g_scurr;
		g_scurr = g_scurr->m_next;
	}

	if ( S_ADDTAIL == flag)
	{
		//PRINT("[set_change_schdu]+++++++++++++[S_ADDTAIL]\r\n");
		g_scurr = tmp;

	}
	else
	{
		if (NULL == tmp)
		{
			return FILE_S_NOFIND_ERR;
		}
		switch (flag)
		{
		case S_UPDATE:
			g_scurr = tmp;
			break;
		case S_DELETE:
			//PRINT("[set_change_schdu]+++++++++++++[DELETE]\r\n");

			tmp1 = g_scurr;
			if ((g_shead == tmp) && (g_shead == g_scurr))
			{
				g_shead = tmp->m_next;
			}
			else if ((g_shead == tmp))
			{
				tmp->m_next = g_scurr->m_next;
			}
			else
			{
				tmp->m_next = g_scurr->m_next;
			}
			//  PRINT("[set_change_schdu]+++++++++++++++++++++1\r\n");

			s_schedu_free(tmp1);
			break;
		default:
			sem_post(&Schdu_Mutex);
			return FILE_S_PRM_ERR;

		}
	}

	//int len = File_Read_Schdu(id, data_buf, 0, sizeof(data_buf));
	len = schedule_data_read_info(id, 0, data_buf, sizeof(data_buf));

	//PRINT("[set_change_schdu]+++++++++++++++++++++2\r\n");
	if (len <= 0)
	{
		/* if( NULL != g_shead)
		 {
		 PRINT("[set_change_schdu]++   g_shead->schid = %d \r\n",g_shead->schid);
		 }*/
		sem_post(&Schdu_Mutex);
		return -1;
	}
	else
	{
		s_schedu_parse(data_buf);
	}
	/* if( NULL != g_shead)
	 {
	 PRINT("[set_change_schdu]++   g_shead->schid = %d \r\n",g_shead->schid);
	 }*/
	sem_post(&Schdu_Mutex);
	return 1;

}

/*初始化schedule数据函数
 *
 *作用:	初始化schedule数据
 *
 *参数:
 *
 *返回:	成功>0，失败<0
 */
void init_schdu_task (void)
{
	init_schdu_buf();
}

#ifdef DEBUG
void print_all(void)
{
	g_scurr = g_shead;
	while(g_scurr)
	{
		//PRINT("id = %d\n",g_scurr->m_id);
		//PRINT("crc = %d \n", g_scurr->m_crc);
		//PRINT("time = %s\n",g_scurr->m_buf);
		g_scurr = g_scurr->m_next;
	}

}
#endif

#ifdef DEBUG

int main(void)
{
	init_schdu_task();
	schdu_task_wait_quit();
}

#endif
#endif
