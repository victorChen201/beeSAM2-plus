#include"uzipupgrade_xmoedm.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define UZIP_UPGRADE_DBG

#ifndef UZIP_UPGRADE_PUTC
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_PUTC(Char)			PUTC(Char)
#else
#define UZIP_UPGRADE_PUTC(Char)
#endif
#endif

#ifndef UZIP_UPGRADE_PUTS
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define UZIP_UPGRADE_PUTS(Str, Len)
#endif
#endif

#ifndef UZIP_UPGRADE_PUTASCC
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_PUTASCC(Char)			PUTASCC(Char)
#else
#define UZIP_UPGRADE_PUTASCC(Char)
#endif
#endif

#ifndef UZIP_UPGRADE_PUTASCS
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define UZIP_UPGRADE_PUTASCS(Str, Len)
#endif
#endif

#ifndef UZIP_UPGRADE_PRINT
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define UZIP_UPGRADE_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef UZIP_UPGRADE_DYN_PUTS
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_UZIP_UPGRADE|Dbg), Str, Len)
#else
#define UZIP_UPGRADE_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef UZIP_UPGRADE_DYN_PUTASCC
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_UZIP_UPGRADE|Dbg), Char)
#else
#define UZIP_UPGRADE_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef UZIP_UPGRADE_DYN_PUTASCS
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_UZIP_UPGRADE|Dbg), Str, Len)
#else
#define UZIP_UPGRADE_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef UZIP_UPGRADE_DYN_PRINT
#ifdef UZIP_UPGRADE_DBG
#define UZIP_UPGRADE_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_UZIP_UPGRADE|Dbg), Fmt, ##Arg)
#else
#define UZIP_UPGRADE_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif



//-----------------------------------------------------------------------------------------------*
/*
*     X-modem ?? ???(values of the X-modem specific charaters)
*/
//-----------------------------------------------------------------------------------------------*
#define UZIP_UPGRADE_XMODEM_START 				'C'
#define UZIP_UPGRADE_XMODEM_SOH 				0x01
#define UZIP_UPGRADE_XMODEM_EOT 				0x04
#define UZIP_UPGRADE_XMODEM_ACK 				0x06
#define UZIP_UPGRADE_XMODEM_NAK 				0x15
#define UZIP_UPGRADE_XMODEM_CAN 				0x18
#define UZIP_UPGRADE_XMODEM_TIMEOUT 			0x54	//???
//-----------------------------------------------------------------------------------------------------*
//  X-modem Frame form: <SOH><block><~block><128 data bytes><CRC hi><CRC lo>  sizemak: 1+1+1+128+2byte4
//-----------------------------------------------------------------------------------------------------*
#define UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE 				128
#define UZIP_UPGRADE_XMODEM_PACKET_TOTAL_LEN 				(UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE+5)

#define UZIP_UPGRADE_XMODEM_AT_ANSWER_LEN  					50   //AT command buf

#define UZIP_UPGRADE_XMODEM_SEND_PACKET_TIMEWAIT_MAX		10000

//-------------------------------------------------------------------------------------------------*


static uint8_t NAK_time = 0;		//<NAK>

char uzipupgradefilepath[50] = "/root/uzip.zigbee";
char deviceupgradefilepath[50] = "";
//????
typedef struct
{
	uint8_t				packetNum;		/*????          ???????? ??? 1 ??255 ??255?0????*/
	uint8_t				start;			/*????????  ????'C' ????1 ?????'C'?0*/
	uint8_t				finsh;			/*????????  ??????? ????1 ????0*/
	uint16_t			crc16;		    /*???CRC16       CRC16??*/
	size_t				offset;			/*??????      ???????? ??? 0*/
	char 				databuf[UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE];
} S_UzipUpgradeInfo;


static S_UzipUpgradeInfo  S_UzipUpgradeDataInfo;    	//Uzip???????
static S_UzipUpgradeInfo  S_UzipUpgradeSavePrvDataInfo;	//Uzip??????????????

//-------------------------------------------------------------------------------------------------*


//-----------------------------------------------------------------------------------------------*
/*	void Uzip_xmodem_filepath(char *file)
*	(??)
*		????????
*	(??)
*		file ???
*
*	(??)
*/
//-----------------------------------------------------------------------------------------------*
void Uzip_xmodem_filepath(char *file)
{
	int iRet = 0;
	iRet = sprintf(&uzipupgradefilepath[iRet],"/root/%s",file);
}

void Device_Upgrade_filepath(char *file)
{
	int iRet = 0;
	iRet = sprintf(&deviceupgradefilepath[iRet],"/root/%s",file);
}

void read_Device_Upgrade_filepath(char *file)
{
	memcpy(file, deviceupgradefilepath, strlen(deviceupgradefilepath));
}

//-----------------------------------------------------------------------------------------------*
/*	void Uzip_xmodem_init_DataInfo()
*	(??)
*		??? ???????
*	(??)
*
*	(??)
*		S_UzipUpgradeDataInfo.start 	= 1; 		//????'C' ????1 ?????'C'?0
*		S_UzipUpgradeDataInfo.finsh 	= 1;		//??????? ????1 ????0
*		S_UzipUpgradeDataInfo.offset	= 0;		//???????? ??? 0
*		S_UzipUpgradeDataInfo.packetNum = 0;		//???????? ??? 1 ??255 ??255?0????
*		uzip_xmoden_state 				= NULL;		//uzip_xmoden_state ?? 'C' <ACK> <NAK> <CAN> timeout
*		S_UzipUpgradeDataInfo.databuf				//??????
*/
//-----------------------------------------------------------------------------------------------*
void Uzip_xmodem_init_DataInfo()
{
	S_UzipUpgradeDataInfo.start 	= 1;
	S_UzipUpgradeDataInfo.finsh 	= 1;
	S_UzipUpgradeDataInfo.offset	= 0;
	S_UzipUpgradeDataInfo.packetNum = 0;
	//uzip_xmoden_state 				= NULL;
	memset(S_UzipUpgradeDataInfo.databuf, 0, UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE);
}

void Uzip_xmodem_save_prv_DataInfo()
{
	S_UzipUpgradeSavePrvDataInfo.start 		= S_UzipUpgradeDataInfo.start;
	S_UzipUpgradeSavePrvDataInfo.finsh 		= S_UzipUpgradeDataInfo.finsh;
	S_UzipUpgradeSavePrvDataInfo.offset		= S_UzipUpgradeDataInfo.offset;
	S_UzipUpgradeSavePrvDataInfo.packetNum	= S_UzipUpgradeDataInfo.packetNum;
}

//-----------------------------------------------------------------------------------------------*
/*	int xmodem_send_packet(char *pdatablock,uint8_t packetNum,uint16_t ui16Crc)
*	(??)
*		??X-modem???????
*		<SOH><block><~block><128 data bytes><CRC hi><CRC lo>  sizemak: 1+1+1+128+2byte4
*	(??)
*		*pdatablock:	128Byte ????
*		packetNum  :  	????? ??? 1 ??255 ??255?0????
*		ui16Crc	   :	CRC-16  x16+x12+x5+1(0x11021) ??X-modem?? CRC???? CRC????
*	(??)
*		iRet  ????  ??133Byte
*	(??)
*/
//-----------------------------------------------------------------------------------------------*
int xmodem_send_packet(S_UzipUpgradeInfo  S_DataInfo)
{
	int iRet;
	char buf[UZIP_UPGRADE_XMODEM_PACKET_TOTAL_LEN];

	memset(buf, 0, UZIP_UPGRADE_XMODEM_PACKET_TOTAL_LEN);

	//[[SOH] [packetNO] [~packetNO] [128byte] [crc-16 H] [crc-16 L]]
	buf[0] = UZIP_UPGRADE_XMODEM_SOH;
	buf[1]= S_DataInfo.packetNum;
	buf[2]= ~S_DataInfo.packetNum;
	memcpy(buf+3,S_DataInfo.databuf,128);
	buf[131]=(uint8_t)(S_DataInfo.crc16>> 8);
	buf[132]=(uint8_t)(S_DataInfo.crc16);

	//??X-modem ??,??133Byte ???????Uzip??(zigbee)
	iRet = at_port_block_wr (buf,UZIP_UPGRADE_XMODEM_PACKET_TOTAL_LEN, UZIP_UPGRADE_XMODEM_SEND_PACKET_TIMEWAIT_MAX);
	if(iRet <= 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_ERR,"[xmodem_send_packet]*******send fail*****.\r\n");
	}
	//UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[xmodem_send_packet]*******[start = %d  ]*****.\r\n",S_DataInfo.start);
	//UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[xmodem_send_packet]*******[finsh = %d  ]*****.\r\n",S_DataInfo.finsh);
	UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[xmodem_send_packet]*******send one block packet finsh*****.\r\n");
	return iRet;
}


//-----------------------------------------------------------------------------------------------*
/*	int Uzip_AT_updata_deal(char *patcommand)
*	(??)
*		??AT ??
*	(??)
*		*patcommand:	AT??
*	(??)
*		iRet ?? ??0
*	(??)
*/
//-----------------------------------------------------------------------------------------------*
int Uzip_AT_updata_deal(char *patcommand)
{
	char achAnswer[UZIP_UPGRADE_XMODEM_AT_ANSWER_LEN];
	int iRet;

	//Delete Zigbee device
 	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), 10000,
							  patcommand);
	if (iRet <= 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[Uzip_AT_updata_deal]$IMAGE fail.\r\n");
		return iRet;
	}
	iRet = at_cmd_answer_verify(achAnswer, iRet);
	if (iRet < 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[Uzip_AT_updata_deal]$IMAGE not answer OK.\r\n");
		return iRet;
	}
	return iRet;

}


//-----------------------------------------------------------------------------------------------*
/*	uzip_upgrade_send_AT_command
*	(??)
*		????????????
*	(??)
*
*	(??)
*  		 -1  ??
*
*	(??)
*/
//-----------------------------------------------------------------------------------------------*
int uzip_xmodem_upgrade_init(char *file)
{
	int iRet;
	S_CfgUzipMac psUzipMac;//Uzig MAC??
	char cATcommand[UZIP_UPGRADE_XMODEM_AT_ANSWER_LEN];//AT ????
	iRet = get_easy_mode();
	if(1 == iRet)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[uzip_upgrade_send_AT_command] send more Uzip upgrade command .\r\n");
		return 0;
	}

	iRet = read_config_info_uzip_mac (&psUzipMac);
	//iRet = read_config_gw_cfg_uzipmac (&psUzipMac);//yjd
	UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_ERR,"[uzip_upgrade_send_AT_command]read_config_gw_cfg_uzipmac = [ %s ].\r\n",psUzipMac.str);
	if(iRet < 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_ERR,"[uzip_upgrade_send_AT_command]read_config_gw_cfg_uzipmac = %d.\r\n",iRet);
		at_cmd_to_easy_mode(0);	//???AT ??
		return -1;
	}

	sprintf(cATcommand,"AT$IMGNTFY=%s,%s\r\n","0006",psUzipMac.str);// Uzip ???? EP: AT$IMAGETY=0006,001BC50340001244\r\n  or 001BC503500201A8
	UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[uzip_upgrade_send_AT_command]AT upgrade comand %s\r\n",cATcommand);

	iRet = Uzip_AT_updata_deal(cATcommand);//????AT??,?????ok
	if(iRet < 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_ERR,"[uzip_upgrade_send_AT_command]Uzip no answer *OK*.\r\n");
		at_cmd_to_easy_mode(0);
		return -1;
	}
	iRet = at_cmd_to_easy_mode(1);//????
	if(iRet != 1)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_ERR,"[uzip_upgrade_send_AT_command]at_cmd_to_easy_mode = %d.\r\n",iRet);
		at_cmd_to_easy_mode(0);	//???AT ??
		return -1;
	}
	Uzip_xmodem_init_DataInfo();
	Uzip_xmodem_filepath(file);//??????
	return iRet;
}


//-----------------------------------------------------------------------------------------------*
/*	int uzip_xmodem_upgrade_readfile()
*	(??)
*		????
*	(??)
*
*	(??)
*
*	(??)
*/
//-----------------------------------------------------------------------------------------------*
int uzip_xmodem_upgrade_readfile()
{
	int iRet,i;
	//iRet = chandler_file_exclusive_read (uzipupgradefilepath, S_UzipUpgradeDataInfo.offset, S_UzipUpgradeDataInfo.databuf,UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE, NULL,0);//yjd
	iRet = file_exclusive_read(uzipupgradefilepath,S_UzipUpgradeDataInfo.offset,S_UzipUpgradeDataInfo.databuf,UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE,NULL,0);
	if(iRet < 0 )//??????
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_ERR,"[uzip_xmodem_upgrade]read Uzipupgrade file fail.\r\n");
		Uzip_xmodem_init_DataInfo();//???
		at_cmd_to_easy_mode(0);	//??????
		at_port_state_set(0);//??UZIP
		return -1;
	}
	else
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[uzip_xmodem_upgrade]===========================================\r\n");
		for (i=0; i<iRet; i++)
		{
			UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"%02hhx ", S_UzipUpgradeDataInfo.databuf[i]);
			if (0 == ((i+1)%16))
			{
				UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
			}
		}
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"\r\n");
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[uzip_xmodem_upgrade]===========================================\r\n");

		if(iRet == UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE)
		{
			S_UzipUpgradeDataInfo.offset 	= S_UzipUpgradeDataInfo.offset + UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE;
			S_UzipUpgradeDataInfo.packetNum = S_UzipUpgradeDataInfo.packetNum + 1;
			S_UzipUpgradeDataInfo.crc16  	= crc16_cal(S_UzipUpgradeDataInfo.databuf, iRet);
		}
		else if(iRet < UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE)
		{
			if(S_UzipUpgradeDataInfo.start == 1)
			{
				UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[uzip_xmodem_upgrade]error!!! uzip upgrade file > 128Byte.\r\n");
				Uzip_xmodem_init_DataInfo();//???
				at_cmd_to_easy_mode(0);	//??????
				at_port_state_set(0);//??UZIP
				return -1;
			}
			else
			{

				S_UzipUpgradeDataInfo.finsh 	= 0;//
				S_UzipUpgradeDataInfo.offset 	= S_UzipUpgradeDataInfo.offset + UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE;
				S_UzipUpgradeDataInfo.packetNum = S_UzipUpgradeDataInfo.packetNum + 1;
				memset(S_UzipUpgradeDataInfo.databuf+iRet,0xff,UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE-iRet);
				S_UzipUpgradeDataInfo.crc16		= crc16_cal(S_UzipUpgradeDataInfo.databuf, UZIP_UPGRADE_XMODEM_BLOCKDATA_BUFSIZE);

			}
		}
	}
	return iRet;
}

//-----------------------------------------------------------------------------------------------*
/*	void* uzip_xmodem_upgrade (void *pArg)
*	(??)
*		Uzip ???X-modem??????
*	(??)
*
*	(??)
*			>0
*	(??)
*/
//-----------------------------------------------------------------------------------------------*
int  uzip_xmodem_upgrade (char uzip_xmoden_state)
{
	int iRet;
	char temp;

	at_cmd_to_easy_mode(1);//??Uzip????,???'C',???????1??,???AT??????????1

	//UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[Uzip_xmodem_answer]uzip_xmoden_state = [  ");
	//UZIP_UPGRADE_DYN_PUTASCS(M_DBG_LEVEL_DEBUG,&uzip_xmoden_state, 1);
	//UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"  ]++++++++++++++++++\r\n");
	if(uzip_xmoden_state == UZIP_UPGRADE_XMODEM_TIMEOUT)//Time Out
	{
		for(iRet=0;iRet<4;iRet++)
		{
			UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[uzip_xmodem_upgrade]Uzip timeout answer *C CAN ACK NAK*.\r\n");
			temp = UZIP_UPGRADE_XMODEM_CAN;
			at_port_block_wr(&temp,1,UZIP_UPGRADE_XMODEM_SEND_PACKET_TIMEWAIT_MAX);//??500ms??"CAN"
			msleep(500);
		}
		at_cmd_to_easy_mode(0);	//??????
		at_port_state_set(0);//??UZIP
		return -1;
	}
	else
	{
		if(uzip_xmoden_state == UZIP_UPGRADE_XMODEM_START)
		{
			NAK_time = 0;
			if(S_UzipUpgradeDataInfo.start == 1)//
			{
				Uzip_xmodem_save_prv_DataInfo();
				S_UzipUpgradeDataInfo.start = 0;//
				iRet = uzip_xmodem_upgrade_readfile();
				if(iRet < 0)
				{
					UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[uzip_xmodem_upgrade]read Uzipupgrade file fail.\r\n");
					Uzip_xmodem_init_DataInfo();//???
					at_cmd_to_easy_mode(0);	//??????
					at_port_state_set(0);//??UZIP
					return -2;
				}
				iRet = xmodem_send_packet(S_UzipUpgradeDataInfo);
				if(iRet < 0)
				{
					at_cmd_to_easy_mode(0);	//??????
					at_port_state_set(0);//??UZIP
					return iRet;
				}
				return iRet;
			}
		}
		else if(uzip_xmoden_state == UZIP_UPGRADE_XMODEM_ACK)
		{
			NAK_time = 0;
			if(S_UzipUpgradeDataInfo.finsh == 1)
			{
				Uzip_xmodem_save_prv_DataInfo();
				iRet = uzip_xmodem_upgrade_readfile();
				if(iRet < 0)
				{
					UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[uzip_xmodem_upgrade]read Uzipupgrade file fail.\r\n");
					Uzip_xmodem_init_DataInfo();//???
					at_cmd_to_easy_mode(0);	//??????
					at_port_state_set(0);//??UZIP
					return -2;
				}
				iRet = xmodem_send_packet(S_UzipUpgradeDataInfo);
				if(iRet < 0)
				{
					at_cmd_to_easy_mode(0);	//??????
					at_port_state_set(0);//??UZIP
					return iRet;
				}
				return iRet;
			}
			else if(S_UzipUpgradeDataInfo.finsh == 0)
			{
				UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[uzip_xmodem_upgrade]**************[<EOT> finsh Uzipupgrade]*************!!!!!!!\r\n");
				temp = UZIP_UPGRADE_XMODEM_EOT;
				at_port_block_wr(&temp,1,UZIP_UPGRADE_XMODEM_SEND_PACKET_TIMEWAIT_MAX);//????,??EOT
				at_cmd_to_easy_mode(0);	//??????
				return 0;
			}
		}
		else if(uzip_xmoden_state == UZIP_UPGRADE_XMODEM_NAK)
		{
			if(NAK_time > 2)//???<NAK>,???????
			{
				UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[uzip_xmodem_upgrade]answer 4 time NAK.\r\n");
				Uzip_xmodem_init_DataInfo();//???
				at_cmd_to_easy_mode(0);	//??????
				at_port_state_set(0);//??UZIP
				return -2;
			}
			NAK_time++;
			S_UzipUpgradeDataInfo.start 	= S_UzipUpgradeSavePrvDataInfo.start;
			S_UzipUpgradeDataInfo.finsh 	= S_UzipUpgradeSavePrvDataInfo.finsh;
			S_UzipUpgradeDataInfo.offset 	= S_UzipUpgradeSavePrvDataInfo.offset;
			S_UzipUpgradeDataInfo.packetNum = S_UzipUpgradeSavePrvDataInfo.packetNum;
			iRet = uzip_xmodem_upgrade_readfile();
			if(iRet < 0)
			{
				UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[uzip_xmodem_upgrade]read Uzipupgrade file fail.\r\n");
				Uzip_xmodem_init_DataInfo();//???
				at_cmd_to_easy_mode(0);	//??????
				at_port_state_set(0);//??UZIP
				return -2;
			}
			iRet = xmodem_send_packet(S_UzipUpgradeDataInfo);
			if(iRet < 0)
			{
				at_cmd_to_easy_mode(0);	//??????
				at_port_state_set(0);//??UZIP
				return iRet;
			}
			return iRet;
		}
		else if(uzip_xmoden_state == UZIP_UPGRADE_XMODEM_CAN)
		{
			at_cmd_to_easy_mode(0);	//??????
			at_port_state_set(0);//??UZIP
			return -3;
		}
	}
	return -4;
}

void find_upgrade_file_by_upgrade_device_type(uint16_t ui16DevType,char *deviceupgradefile)
{
	switch (ui16DevType)
	{
		case 0x0000:
			memcpy(deviceupgradefile, "/root/occupancysensors.zigbee", strlen("/root/occupancysensors.zigbee"));
			break;
		case 0x0001:
			memcpy(deviceupgradefile, "/root/binarysensors.zigbee", strlen("/root/binarysensors.zigbee"));
			break;
		case 0x0002:
			memcpy(deviceupgradefile, "/root/temperaturesensors.zigbee", strlen("/root/temperaturesensors.zigbee"));
			break;
		case 0x0003:
			memcpy(deviceupgradefile, "/root/beeSPv0.2.150101.zigbee", strlen("/root/beeSPv0.2.150101.zigbee"));
			break;
		default:
			return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}

void find_device_by_device_type(uint16_t ui16DevType,char *device)
{

	switch (ui16DevType)
	{
		case MUI_DATA_TYPE_LIGHT:
			memcpy(device, "lights", strlen("lights"));
			break;
		case MUI_DATA_TYPE_OCC_SENSOR:
			memcpy(device, "occupancysensors", strlen("occupancysensors"));
			break;
		case MUI_DATA_TYPE_BIN_SENSOR:
			memcpy(device, "binarysensors", strlen("binarysensors"));
			break;
		case MUI_DATA_TYPE_TEMP_SENSOR:
			memcpy(device, "temperaturesensors", strlen("temperaturesensors"));
			break;
		case MUI_DATA_TYPE_HUM_SENSOR:
			memcpy(device, "humiditysensors", strlen("humiditysensors"));
			break;
		case MUI_DATA_TYPE_BIN_CTRL:
			memcpy(device, "binarycontrollers", strlen("binarycontrollers"));
			break;
		case MUI_DATA_TYPE_LEVEL_CTRL:
			memcpy(device, "levelcontrollers", strlen("levelcontrollers"));
			break;
		case MUI_DATA_TYPE_WARN_DEV:
			memcpy(device, "warningdevices", strlen("warningdevices"));
			break;
		case MUI_DATA_TYPE_PLUG:
			memcpy(device, "plugs", strlen("plugs"));
			break;
		case MUI_DATA_TYPE_SWITCH:
			memcpy(device, "switchs", strlen("switchs"));
			break;
		case MUI_DATA_TYPE_SHADE:
			memcpy(device, "shades", strlen("shades"));
			break;
		case MUI_DATA_TYPE_SONOS:
			memcpy(device, "sonoses", strlen("sonoses"));
			break;
		default:
			break;
	}
}

uint16_t find_upgrade_device_type_by_device_type(uint16_t ui16DevType)
{
	switch (ui16DevType)
	{
		case MUI_DATA_TYPE_OCC_SENSOR:
			return 0x0000;
		case MUI_DATA_TYPE_BIN_SENSOR:
			return 0x0001;
		case MUI_DATA_TYPE_TEMP_SENSOR:
			return 0x0002;
		case MUI_DATA_TYPE_HUM_SENSOR:
			return 0x0002;
		case MUI_DATA_TYPE_PLUG:
			return 0x0003;
		default:
			return DEV_IF_ERR_DEV_TYPE_WRONG;
	}
}

int device_upgrage_init(uint16_t ui16DevType,uint16_t ui16Id,char *file)
{
    int iRet;
	S_AttrModelId psModelId;
	uint8_t deviceupgradestate;
	uint16_t imagtype;
	char achAnswer[UZIP_UPGRADE_XMODEM_AT_ANSWER_LEN];
	char cATcommand[UZIP_UPGRADE_XMODEM_AT_ANSWER_LEN];

	//$IMGABORT:强制打断升级,执行现在的升级
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), 10000,"AT$IMGABORT\r\n");
	if (iRet <= 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[device_upgrage_init]AT$IMGABORT send fail.\r\n");
		return iRet;
	}
	iRet = at_cmd_answer_verify(achAnswer, iRet);
	if (iRet < 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[device_upgrage_init]AT$IMGABORT not answer OK.\r\n");
		return iRet;
	}

/*
	//$IMGSTATUS:00 nothing upgrade ,$IMGSTATUS:01,mac have a device upgrade
	iRet = zigbee_print_query(achAnswer, sizeof(achAnswer), 10000,"AT$IMGSTATUS\r\n");
	if (iRet <= 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[device_upgrage_init]AT$IMFSTATUS send fail.\r\n");
		return iRet;
	}
	UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[device_upgrage_init]device upgrade state %s\r\n",achAnswer);
	iRet = sscanf(achAnswer, "$IMGSTATUS:%02hhx",&deviceupgradestate);
	UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[device_upgrage_init]device upgrade state %02hhx\r\n",deviceupgradestate);
	if(0 != deviceupgradestate)//设备升级状态有三种: 00 没有升级，01 下载程序 02 升级设备中
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[device_upgrage_init]busy , have a device upgrade[%s]\r\n",achAnswer);
		return 0;
	}*/

	//网关的设备类型和uzip的定义的设备类型有差异
	imagtype = find_upgrade_device_type_by_device_type(ui16DevType);
	if(imagtype <0 )
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[device_upgrage_init]this device not support upgrade .\r\n");
		return imagtype;
	}


	//read device mac address
	iRet = read_dev_attr_modelid (ui16DevType,ui16Id,&psModelId);
	UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[deviceupgrageinit]device mac  = [ %s ].\r\n",psModelId.str);
	if(iRet < 0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[uzip_upgrade_send_AT_command]device mac iRet  = %d.\r\n",iRet);
		return -1;
	}

	//upgrade new device
	sprintf(cATcommand,"AT$IMGNTFY=%04hx,%s\r\n",imagtype,psModelId.str);//  EP: AT$IMGNTFY=0003,001BC50340000F3D\r\n  or 001BC503500201A8
	UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[device_upgrage_init]device upgrade send[ %s ]\r\n",cATcommand);
	iRet = Uzip_AT_updata_deal(cATcommand);
	if(iRet < 0)
	{

		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[device_upgrage_init]Uzip no answer *OK*.\r\n");
		return -1;
	}

	//当可以升级设备后,修改升级文件路径
	Device_Upgrade_filepath(file);


}

//-----------------------------------------------------------------------------------------------*
/*	int upgrade_destination_device(char *dest, char *file)
*	(??)
*		????
*	(??)
*		dest ???????: gw zip .....
*		file ???????
*	(??)
*
*	(??)
*/
//-----------------------------------------------------------------------------------------------*

int  upgrade_destination_device(char *dest, char *file,char *address)
{
	int iRet = 0;
	uint16_t ui16Id;
	uint16_t ui16DevType;
	if(strcmp(dest, "uzip")==0)
	{
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[upgrade_destination_device]**************[dest = %s]  [file = %s] [address = %s]*************!!!!!!!\r\n",dest,file,address);
		iRet = uzip_xmodem_upgrade_init(file);
		if(iRet < 0)
		{
			return iRet;
		}
		return 1;
	}
	else if(strcmp(dest, "plugs")==0)
	{
		ui16DevType = MUI_DATA_TYPE_PLUG;
		// read device and device  id
	    iRet = sscanf(address, "%hu",&ui16Id);
	    UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[deviceupgrageinit]device   = [ %hu ].\r\n",ui16Id);
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[upgrade_destination_device]**************[dest = %s]  [file = %s] [address = %s]*************!!!!!!!\r\n",dest,file,address);
		iRet = device_upgrage_init(ui16DevType,ui16Id,file);
		if(iRet < 0)
		{
			return iRet;
		}
		return 1;

	}
	else if(strcmp(dest, "occupancysensor")==0)
	{
		ui16DevType = MUI_DATA_TYPE_OCC_SENSOR;
		// read device and device  id
	    iRet = sscanf(address, "%hu",&ui16Id);
	    UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[deviceupgrageinit]device   = [ %hu ].\r\n",ui16Id);
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[upgrade_destination_device]**************[dest = %s]  [file = %s] [address = %s]*************!!!!!!!\r\n",dest,file,address);
		iRet = device_upgrage_init(ui16DevType,ui16Id,file);
		if(iRet < 0)
		{
			return iRet;
		}
		return 1;
	}
	else if(strcmp(dest, "binarysensor")==0)
	{
		ui16DevType = MUI_DATA_TYPE_BIN_SENSOR;
		// read device and device  id
	    iRet = sscanf(address, "%hu",&ui16Id);
	    UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[deviceupgrageinit]device   = [ %hu ].\r\n",ui16Id);
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[upgrade_destination_device]**************[dest = %s]  [file = %s] [address = %s]*************!!!!!!!\r\n",dest,file,address);
		iRet = device_upgrage_init(ui16DevType,ui16Id,file);
		if(iRet < 0)
		{
			return iRet;
		}
		return 1;
	}
	else if(strcmp(dest, "temperaturesensor")==0)
	{
		ui16DevType = MUI_DATA_TYPE_TEMP_SENSOR;
		// read device and device  id
	    iRet = sscanf(address, "%hu",&ui16Id);
	    UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[deviceupgrageinit]device   = [ %hu ].\r\n",ui16Id);
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[upgrade_destination_device]**************[dest = %s]  [file = %s] [address = %s]*************!!!!!!!\r\n",dest,file,address);
		iRet = device_upgrage_init(ui16DevType,ui16Id,file);
		if(iRet < 0)
		{
			return iRet;
		}
		return 1;
	}
	else if(strcmp(dest, "humiditysensor")==0)
	{
		ui16DevType = MUI_DATA_TYPE_HUM_SENSOR;
		// read device and device  id
	    iRet = sscanf(address, "%hu",&ui16Id);
	    UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_DEBUG,"[deviceupgrageinit]device   = [ %hu ].\r\n",ui16Id);
		UZIP_UPGRADE_DYN_PRINT(M_DBG_LEVEL_NOTICE,"[upgrade_destination_device]**************[dest = %s]  [file = %s] [address = %s]*************!!!!!!!\r\n",dest,file,address);
		iRet = device_upgrage_init(ui16DevType,ui16Id,file);
		if(iRet < 0)
		{
			return iRet;
		}
		return 1;
	}
	/*
	else if(strcmp(dest, "gw")==0)
	{

	}
	else if(strcmp(dest, "light")==0)
	{

	}


	else if(strcmp(dest, "binarycontroller")==0)
	{

	}
	else if(strcmp(dest, "levelcontroller")==0)
	{

	}
	else if(strcmp(dest, "warningdevice")==0)
	{

	}
	else if(strcmp(dest, "switch")==0)
	{

	}
	else if(strcmp(dest, "shade")==0)
	{

	}
*/
	else
	{
		return -1;
	}

}

