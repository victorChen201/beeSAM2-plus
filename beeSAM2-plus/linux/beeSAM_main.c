#include "beeSAM_main.h"
#include "wsclient_manager.h"
#include "misc.h"

/*------------------------------------------------------------------------------
 Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
 All rights reserved.

 Source file of main module for beeSAM2 project.
 File name:beeSAM2_main.c
 Description：

 -------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
 Author    : Chandler
 Version   : 1.0
 Date      : 2014/04/01
 Time      : 14:43

 -------------------------------------------------------------------------------*/

#define MSTR_BEESAM_NAME_FIFO		"/tmp/beeSAM3.fifo"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int run_para_verify (int iArgNum, char *pchArg[], S_RunPara *psPara);
static void sys_clean (int iSig);

//-------------------------------------------------------------------------------------------------*

int flg = 1;


int main (int arc, char *arv[])
{
	S_RunPara sRunPara;
	struct timeval sTime;
	fd_set sFd;
	uint16_t ui6Key;
	int iFifoFd, iRet;

	signal(SIGINT, sys_clean);
	memset(&sRunPara, 0, sizeof(sRunPara));

	if (0 > run_para_verify(arc, arv, &sRunPara))
	{
		printf("[main]Parameter error.\r\n");
		return -1;
	}

	printf("[main]Net port dev:%s\r\n", sRunPara.achNetPortDev);
	printf("[main]AT port dev:%s\r\n", sRunPara.achAtPortDev);
	printf("[main]File path:%s\r\n", sRunPara.achFilePath);
	printf("[main]Listen address:%s:%u\r\n", sRunPara.achListenIp, sRunPara.ui16ListenPort);
	printf("[main]Report address:%s:%u\r\n", sRunPara.achReportIp, sRunPara.ui16ReportPort);
	printf("[main]netmask address:%s\r\n", sRunPara.netmask);
	printf("[main]gateway address:%s\r\n", sRunPara.gateway);

	printf("[main]DbgIP address:%s\r\n", sRunPara.achDbgIp);

	printf("[main]mac address:%s\r\n", sRunPara.mac);
#ifdef M_FOR_EGGPLANT
	dbg_sys_init("192.168.1.111");
#else
	dbg_sys_init(sRunPara.achDbgIp);
#endif
	iRet = data_root_path_cfg(sRunPara.achFilePath);
	if (iRet < 0)
	{
		DYN_PRINT(M_DBG_MODULE_ALL|M_DBG_LEVEL_ERR, "[main]Call 'data_root_path_cfg(%s)' fail.\r\n", sRunPara.achFilePath);
		return iRet;
	}

	gpio_init();
/*

	while (1)
	{
		led_light_ctrl(MUI_GPIO_PIN_PWR_LED, 1);
		msleep(1000);
		led_light_ctrl(MUI_GPIO_PIN_NET_LED, 1);
		led_light_ctrl(MUI_GPIO_PIN_ZIGBEE_LED, 1);
	}
*/
	do
	{
		iRet = database_init();
		if (iRet < 0)
		{
			DYN_PRINT(M_DBG_MODULE_ALL|M_DBG_LEVEL_ERR, "[main]Call 'database_init()' fail!\r\n");
			return iRet;
		}
		iRet = config_init();
		if (iRet < 0)
		{
			DYN_PRINT(M_DBG_MODULE_ALL|M_DBG_LEVEL_ERR, "[main]Call 'config_init()' fail!\r\n");
			return iRet;
		}
		if ((iRet==1) || (iRet==2))
		{
			database_del_all();
		}
	} while ((iRet==1) || (iRet==2));

	set_init();
	schedule_init();
	rules_init();

	//extend_data_test();

	config_factory_new_manual_chk();
	//config_info_chk();

	api_deal_init();

	//ntp_init();

	multicast_init();
	remote_api_init();
	//test_atcmd();
	local_report_init();
	auto_rp_init();

	zigbee_manager_init(sRunPara.achAtPortDev);

	//iRet = net_manage_init(sRunPara.achListenIp, sRunPara.ui16ListenPort, sRunPara.achReportIp, sRunPara.ui16ReportPort);
	iRet = net_manage_init(&sRunPara);
	if (iRet < 0)
	{
		DYN_PRINT(M_DBG_MODULE_ALL|M_DBG_LEVEL_ERR, "[main]Call 'socket_manage_init' fail:%d, errno:%d", iRet, errno);
		return iRet;
	}
	led_light_ctrl(MUI_GPIO_PIN_NET_LED, 1);
	sonos_drv_init();
	ssdp_init();

	remote_net_manage_init(10);
/*
	iRet = mkfifo(MSTR_BEESAM_NAME_FIFO, O_CREAT|O_EXCL);
	if (iRet < 0)
	{
		DYN_PRINT(M_DBG_MODULE_ALL|M_DBG_LEVEL_ERR, "[main]Call 'mkfifo' fail, errno:%d", errno);
		return iRet;
	}

	iFifoFd = open(MSTR_BEESAM_NAME_FIFO, O_RDONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	while (iFifoFd > 0)
	{
		sTime.tv_sec  = 10;
		sTime.tv_usec = 0;

		//初始化select文件描述符集合
		FD_ZERO(&sFd);
		FD_SET(iFifoFd, &sFd);

		//等待串口输入数据
		iRet = select(iFifoFd+1, &sFd, NULL, NULL, &sTime);
		if (iRet <= 0)
		{
			continue;
		}
		//AT_PORT_LINUX_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[at_port_listen_task]Have checked input from at port.\r\n");

		//read串口
		iRet = read(iFifoFd, &ui6Key, sizeof(ui6Key));
		if (iRet <= 0)
		{
			continue;
		}

		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@Key=%u\r\n", ui6Key);

	}

	close(iFifoFd);
*/
	at_cmd_sys_quit_wait();
	net_manage_quit_wait();
	//rule_task_wait_quit();
	ssdp_quit_wait();
	//ntp_quit_wait();
	remote_net_quit_wait();

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	运行参数校验函数
 ;	(功能)AT命令查询参数校验
 ;	(参数)
 ;		iArgNum:	参数个数
 ;		pchArg:		参数列表(-n -c -p -l -r -m -g -u)
 ;		psPara:		分析后的参数返回结构体指针
 ;	(返回)
 ;		int:		>=0:参数个数
 ;					 <0:参数错误或异常
 ;	(注意)
 */
static int run_para_verify (int iArgNum, char *pchArg[], S_RunPara *psPara)
{
	int i;
	int iCnt = 0;
	int iTmp, iLen;
	S_SockAddr sIfInfo;

	strcpy(psPara->achNetPortDev, MSTR_IF_NAME);
	strcpy(psPara->achAtPortDev, MSTR_AT_PORT_DEV);
	strcpy(psPara->achFilePath, MSTR_FILE_DEFT_PATH);
	strcpy(psPara->achListenIp, "");	 /* 本地IP */
	psPara->ui16ListenPort = MUI_SOCKET_LISTEN_PORT;
	strcpy(psPara->achReportIp, MSTR_SOCKET_REPORT_IP);
	psPara->ui16ReportPort = MUI_SOCKET_REPORT_PORT;

	strcpy(psPara->netmask, "");
	strcpy(psPara->gateway, "");
	strcpy(psPara->mac, "");

	if (1 == iArgNum)
	{
		printf("[run_para_verify]will get local net interface info.\r\n");
	}
	else if (iArgNum < 18)
	{
		i = 1;
		while (i < iArgNum)
		{
			printf("[run_para_verify]Parameter[%d]:%s\r\n", i, pchArg[i]);
			if (0 == strcmp(pchArg[i], "-n"))
			{
				i++;
				if (i < iArgNum)
				{
					strcpy(psPara->achNetPortDev, pchArg[i]);
					iCnt++;
					printf("[run_para_verify]Net port device:%s\r\n", pchArg[i]);
				}
			}
			else if (0 == strcmp(pchArg[i], "-c"))
			{
				i++;
				if (i < iArgNum)
				{
					strcpy(psPara->achAtPortDev, pchArg[i]);
					iCnt++;
					printf("[run_para_verify]AT port device:%s\r\n", pchArg[i]);
				}
			}
			else if (0 == strcmp(pchArg[i], "-p"))
			{
				i++;
				if (i < iArgNum)
				{
					strcpy(psPara->achFilePath, pchArg[i]);
					iCnt++;
					printf("[run_para_verify]File path:%s\r\n", pchArg[i]);
				}
			}
			else if (0 == strcmp(pchArg[i], "-l"))
			{
				i++;
				if (i < iArgNum)
				{
					strcpy(psPara->achListenIp, pchArg[i]);
					iCnt++;
					printf("[run_para_verify]Listen address:%s\r\n", psPara->achListenIp);
				}
			}
			else if (0 == strcmp(pchArg[i], "-lp"))
			{
				i++;
				if (i < iArgNum)
				{
					iTmp = atoi(pchArg[i]);
					if (iTmp <= 0)
					{
						printf("[run_para_verify]The format of '-lp' is wrong, correct example:-lp 7890\r\n");
						return -1;
					}
					psPara->ui16ListenPort = (uint16_t)iTmp;
					iCnt++;
					printf("[run_para_verify]Listen port:%u\r\n", psPara->ui16ListenPort);
				}
			}
			else if (0 == strcmp(pchArg[i], "-r"))
			{
				i++;
				if (i < iArgNum)
				{
					strcpy(psPara->achReportIp, pchArg[i]);
					iCnt++;
					printf("[run_para_verify]Report address:%s\r\n", psPara->achReportIp);
				}
			}
			else if (0 == strcmp(pchArg[i], "-rp"))
			{
				i++;
				if (i < iArgNum)
				{
					iTmp = atoi(pchArg[i]);
					if (iTmp <= 0)
					{
						printf("[run_para_verify]The format of '-rp' is wrong, correct example:-rp 7891\r\n");
						return -1;
					}
					psPara->ui16ReportPort = (uint16_t)iTmp;
					iCnt++;
					printf("[run_para_verify]Report port:%u\r\n", psPara->ui16ReportPort);
				}
			}
			else if (0 == strcmp(pchArg[i], "-m"))
			{
				i++;
				if (i < iArgNum)
				{
					iLen = strlen(pchArg[i]);
					if (iLen > 15)
					{
						printf("[run_para_verify]The format of '-m' is wrong, correct example:-m 255.255.255.255\r\n");
						return -1;
					}
					strcpy(psPara->netmask, pchArg[i]);
					psPara->netmask[iLen] = '\0';

					iCnt++;
					printf("[run_para_verify]netmask:%s\r\n", psPara->netmask);
				}
			}
			else if (0 == strcmp(pchArg[i], "-g"))
			{
				i++;
				if (i < iArgNum)
				{
					iLen = strlen(pchArg[i]);
					if (iLen > 15)
					{
						printf("[run_para_verify]The format of '-g' is wrong, correct example:-g 192.168.1.1\r\n");
						return -1;
					}
					strcpy(psPara->gateway, pchArg[i]);
					psPara->gateway[iLen] = '\0';

					iCnt++;
					printf("[run_para_verify]gateway:%s\r\n", psPara->gateway);
				}
			}
			else if (0 == strcmp(pchArg[i], "-d"))
			{
				i++;
				if (i < iArgNum)
				{
					iLen = strlen(pchArg[i]);
					if (iLen > 15)
					{
						printf("[run_para_verify]The format of '-d' is wrong, correct example:-d 192.168.1.99\r\n");
						return -1;
					}
					strcpy(psPara->achDbgIp, pchArg[i]);
					psPara->achDbgIp[iLen] = '\0';

					iCnt++;
					printf("[run_para_verify]DbgIP:%s\r\n", psPara->achDbgIp);
				}
			}
			else
			{
				printf("[run_para_verify]The correct parameter example: -n eth0 -c /dev/ttyUSB0 -p ./ -l 127.0.0.1 -lp 7890 -r 127.0.0.1 -rp 7891 -g 192.168.1.1 -d 192.168.1.99\r\n");
				return -1;
			}

			i++;
		}
	}

	if ('\0' == psPara->achListenIp[0])	//如果没有指定监听端口,则本地网信息要自动获取
	{
		//获取本机eth0网口界面信息
		memset(&sIfInfo, 0, sizeof(sIfInfo));
		if (0 != if_info_get(psPara->achNetPortDev, &sIfInfo))
		{
			printf("[run_para_verify]Can not find IP address of '%s'\r\n", psPara->achNetPortDev);
			return -1;
		}

		strcpy(psPara->achListenIp, sIfInfo.s_achListenIP);
		//printf("psPara->achListenIp=%s\r\n", psPara->achListenIp);
		/*if (NULL == inet_ntop(AF_INET, &sIfInfo.sIpAddr.s_addr, psPara->achListenIp, sizeof(psPara->achListenIp)))
		 {
		 printf("[run_para_verify]Can not transmit local IP address to string.\r\n");
		 return -1;
		 }*/

		strcpy(psPara->netmask, sIfInfo.s_achNetMask); /* 子网掩码 */
		//printf("psPara->netmask=%s\r\n", psPara->netmask);
		strcpy(psPara->mac, sIfInfo.mac); /* MAC 地扯 */
		//printf("psPara->mac=%s\r\n", psPara->mac);

		//获取网关地扯
		get_gateway(sIfInfo.s_achGatewayIP, sIfInfo.s_achListenIP);
		strcpy(psPara->gateway, sIfInfo.s_achGatewayIP); /* 网关地扯 */
		//printf("psPara->gateway=%s\r\n", psPara->gateway);
		//以上是扫描本地的网络界面信息
	}
	else
	{
		printf("[run_para_verify]paraNumber err,correct,ig:./beeSAM2_linux -n eth0 -c /dev/tty0 -l 192.168.1.100:80 -r 127.0.0.1:90 -m 255.255.210.0 -d 192.168.1.178\r\n");
		printf("[run_para_verify]will get local net interface info.\r\n");
	}

	return iCnt;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	系统清除函数
 ;	(功能)在退出程序前释放所有的资源
 ;	(参数)
 ;		iSig:	信号值
 ;	(返回)
 ;		NULL
 ;	(注意)
 */
static void sys_clean (int iSig)
{
	PRINT("[sys_clean]Got the signal:%d, will free all of resource.\r\n", iSig);
	//at_port_clean();
	//socket_manage_clean();
	PRINT("[sys_clean]Will exit ...\r\n");
	exit(0);
}
//-------------------------------------------------------------------------------------------------*

