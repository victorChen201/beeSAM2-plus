#include "ntp.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define NTP_DBG

#ifndef NTP_PUTC
#ifdef NTP_DBG
#define NTP_PUTC(Char)			PUTC(Char)
#else
#define NTP_PUTC(Char)
#endif
#endif

#ifndef NTP_PUTS
#ifdef NTP_DBG
#define NTP_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define NTP_PUTS(Str, Len)
#endif
#endif

#ifndef NTP_PUTASCC
#ifdef NTP_DBG
#define NTP_PUTASCC(Char)			PUTASCC(Char)
#else
#define NTP_PUTASCC(Char)
#endif
#endif

#ifndef NTP_PUTASCS
#ifdef NTP_DBG
#define NTP_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define NTP_PUTASCS(Str, Len)
#endif
#endif

#ifndef NTP_PRINT
#ifdef NTP_DBG
#define NTP_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define NTP_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef NTP_DYN_PUTC
#ifdef NTP_DBG
#define NTP_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_NTP|Dbg), Char)
#else
#define NTP_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef NTP_DYN_PUTS
#ifdef NTP_DBG
#define NTP_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_NTP|Dbg), Str, Len)
#else
#define NTP_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef NTP_DYN_PUTASCC
#ifdef NTP_DBG
#define NTP_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_NTP|Dbg), Char)
#else
#define NTP_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef NTP_DYN_PUTASCS
#ifdef NTP_DBG
#define NTP_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_NTP|Dbg), Str, Len)
#else
#define NTP_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef NTP_DYN_PRINT
#ifdef NTP_DBG
#define NTP_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_NTP|Dbg), Fmt, ##Arg)
#else
#define NTP_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

/* 以下是NTP 相关宏定义*/
#define NTP_PORT							123

/* NTP 服务器地扯*/
//#define NTP_SERVER_IP "210.72.145.44" //CHINA
//#define NTP_SERVER_IP "132.163.4.102" //USA
//#define NTP_SERVER_IP "192.168.112.102" //fedora8
#define NTP_SERVER_IP						"192.53.103.103"  //网络时间服务器

#define NTP_SERVER_IP_ADDR0					192
#define NTP_SERVER_IP_ADDR1					53
#define NTP_SERVER_IP_ADDR2					103
#define NTP_SERVER_IP_ADDR3					103


/* ntp procotol packet head */
#define LI 									0
#define VN 									3				/* ntp version */
#define MODE 								3				/* client mode */
#define STRATUM 							0				/* NTP 层级 */
#define POLL 								4				/* 轮询时间*/
#define PREC 								-6				/* 精度过*/

/* ntp protocol packet len */
#define NTP_PCK_LEN 						48


#define JAN_1970 							0x83aa7e80
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x)) >> 11))
#define USEC(x)    (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

/* ntp update thread description */
static pthread_t s_sntpupdateThread;


/**************************************************************************
** 函数名: construct_packet
** 描  述: 构建NTP 协议包接口
** 全局变量: 无
** 输  入:
**		  packet-->NTP 协议数据包指针
** 输  出: 无
** 调用模块: ntp_send_packet
** 作  者: pauldeng
** 日  期: 2014-05-15
** 修  改:
**************************************************************************/
static void construct_packet(struct ntp_packet *packet)
{
	//char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	time_t timer;

	ASSERT(packet != NULL);

	packet->leap_ver_mode = (LI << 5)|(VN << 3) |(MODE << 0);
	packet->startum = STRATUM;
	packet->poll = POLL;
	packet->precision = (char)(PREC);

	/*Root Delay.Root Dispersion.Reference Indentifier */
	packet->root_delay = 0x100;
	packet->root_dispersion = 0x100;
	//NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "li_vr_md=0x%x, startun=0x%x, poll=0x%x, prec=0x%x\r\n", packet->leap_ver_mode, packet->startum, packet->poll, packet->precision);

	time(&timer);
	//NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "old timer=0x%lx\r\n", (long)timer);

	packet->transmit_timestamp.coarse = htonl(JAN_1970 + (long)timer);
	packet->transmit_timestamp.fine = htonl((long)NTPFRAC(timer));
	//NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "coarse=0x%lx, fine=0x%lx\r\n", packet->transmit_timestamp.coarse, packet->transmit_timestamp.fine);
	return;

}


/**************************************************************************
** 函数名: set_ntp_time
** 描  述: 设置时间
** 全局变量: 无
** 输  入:
**		packet:	NTP数据包指针
** 输  出: 无
** 调用模块: ntp_packet_handler
** 作  者: pauldeng
** 日  期: 2014-05-15
** 修  改:
**************************************************************************/
static int set_ntp_time(struct ntp_packet *packet)
{
	 struct timeval tv;
	 tv.tv_sec = packet->transmit_timestamp.coarse - JAN_1970;
	 tv.tv_usec = USEC(packet->transmit_timestamp.fine);
	 /* setting system time */
	 return settimeofday(&tv, NULL);
}


static int update_ntp_time_thread(void)
{

	int sockfd, addr_len,result=0;
	fd_set pd;

	struct sockaddr_in addr;
	struct timeval timeout = {5,0};

	char data[NTP_PCK_LEN];
	struct ntp_packet ret_time;


	/* create socket description */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0){
  		printf("[update_ntp_time_thread]socket create fail.\r\n");
  		return -1;;
	}

	addr.sin_family = AF_INET;						/* protocol type */
	addr.sin_port = htons(NTP_PORT);				/* NTP PORT number */
	addr.sin_addr.s_addr=inet_addr(NTP_SERVER_IP);	/* ntp server ip addr */

	bzero(data, NTP_PCK_LEN);
	bzero(&ret_time,sizeof(ret_time));

	addr_len = sizeof(struct sockaddr);

	while(1){

		NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_ntp_time_thread]ntp client run.\r\n");

		sleep(30);

		construct_packet(&ret_time);

		//printf("li_vr_mode=0x%x, startum=0x%x, poll=0x%x, prec=0x%x,  root_delay=0x%x, root_disp=0x%x, tran.coarse0x=%lx, tran.fine=0x%lx\n",ret_time.leap_ver_mode, ret_time.startum, ret_time.poll, ret_time.precision, ret_time.root_delay, ret_time.root_dispersion, ret_time.transmit_timestamp.coarse, ret_time.transmit_timestamp.fine);

		/* send ntp packet to ntp server */
		if((result = sendto(sockfd, &ret_time, NTP_PCK_LEN, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr))) < 0) {
			NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_ntp_time_thread]ntp packet sendto error.\n");
	  		continue;
		}

		/*..select()...........1s*/
		FD_ZERO(&pd);
		FD_SET(sockfd, &pd);

		switch (select(sockfd + 1, &pd, NULL, NULL, &timeout))
		{
			case -1:
	 			NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_ntp_time_thread]Call 'select' error.\n");
	 			break;
			case 0:
	 			NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_ntp_time_thread]Call 'select' time out.\n");
	 			break;
			default:
	  			result = 0;
	  			if ((result = recvfrom(sockfd, data, NTP_PCK_LEN, 0, (struct sockaddr *)&addr, &addr_len)) < 0){
					NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_ntp_time_thread]Call 'recvfrom' error.\n");
					continue;
				}
				bzero(&ret_time,sizeof(ret_time));

				//fill new ntp packet
				//ret_time->leap_ver_mode = ntohl(data[0]);
				ret_time.leap_ver_mode = data[0];
				//ret_time->startum = ntohl(data[1]);
				ret_time.startum = data[1];
				//ret_time->poll = ntohl(data[2]);
				ret_time.poll = data[2];
				ret_time.precision = data[3];
				//ret_time->precision = ntohl(data[3]);

				ret_time.root_delay = ntohl(*(int*)&(data[4]));
				ret_time.root_dispersion = ntohl(*(int*)&(data[8]));
				ret_time.reference_identifier = ntohl(*(int*)&(data[12]));


				ret_time.reference_timestamp.coarse = ntohl(*(int*)&(data[16]));
				ret_time.reference_timestamp.fine = ntohl(*(int*)&(data[20]));

				ret_time.originage_timestamp.coarse = ntohl(*(int*)&(data[24]));
				ret_time.originage_timestamp.fine = ntohl(*(int*)&(data[28]));

				ret_time.receive_timestamp.coarse = ntohl(*(int*)&(data[32]));
				ret_time.receive_timestamp.fine = ntohl(*(int*)&(data[36]));

				ret_time.transmit_timestamp.coarse = ntohl(*(int*)&(data[40]));
				ret_time.transmit_timestamp.fine = ntohl(*(int*)&(data[44]));


				//printf("li_vr_mode=0x%x, startum=0x%x, poll=0x%x, prec=0x%x,  root_delay=0x%x, root_disp=0x%x, tran.coarse0x=%lx, tran.fine=0x%lx\n",ret_time.leap_ver_mode, ret_time.startum, ret_time.poll, ret_time.precision, ret_time.root_delay, ret_time.root_dispersion, ret_time.transmit_timestamp.coarse, ret_time.transmit_timestamp.fine);

				if (!set_ntp_time(&ret_time)){
					NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_ntp_time_thread]NTP client update success!\n");
					system("date");
				} else
					NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[update_ntp_time_thread]NTP clinet update fail!\n");

	  			break;
		}
	}

	return 0;
}

/**************************************************************************
** 函数名: ntp_init
** 描  述:NTP初始化接口
** 全局变量: 无
** 输  入: 无
** 输  出: 无
** 调用模块: main
** 作  者: pauldeng
** 日  期: 2014-05-15
** 修  改:
**************************************************************************/
int ntp_init (void)
{
	int iRet = 0;
	//NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "ntp_init\r\n");

	iRet = pthread_create(&s_sntpupdateThread, NULL, update_ntp_time_thread, NULL);
	if (iRet < 0)
	{
		//NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ntp_init]Create thread 'update_ntp_time_thread' fail, errno:%d.\r\n", iRet);
		NTP_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ntp_init]Create thread 'update_ntp_time_thread' fail, errno:%d.\r\n", iRet);
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*

//-------------------------------------------------------------------------------------------------*
/*	ntp退出等待函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:			 <0:出错(包括异常及等待超时)
;						>=0:正常退出
;	(注意)
*/
int ntp_quit_wait (void)
{
	int iRet;

	iRet = pthread_join(s_sntpupdateThread, NULL);
	if (iRet != 0)
	{
		printf("[ntp_quit_wait]Error joining thread 'update_ntp_time_thread'.\r\n");
		abort();
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*

