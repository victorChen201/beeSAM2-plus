#include "database.h"
#include "config_data.h"
#include "user_data.h"
#include "dev_data.h"
#include "extend_data.h"
#include "group_data.h"
#include "set_data.h"
#include "schedule_data.h"
#include "rule_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define DATABASE_DBG

#ifndef DATABASE_PUTC
#ifdef DATABASE_DBG
#define DATABASE_PUTC(Char)			PUTC(Char)
#else
#define DATABASE_PUTC(Char)
#endif
#endif

#ifndef DATABASE_PUTS
#ifdef DATABASE_DBG
#define DATABASE_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define DATABASE_PUTS(Str, Len)
#endif
#endif

#ifndef DATABASE_PUTASCC
#ifdef DATABASE_DBG
#define DATABASE_PUTASCC(Char)			PUTASCC(Char)
#else
#define DATABASE_PUTASCC(Char)
#endif
#endif

#ifndef DATABASE_PUTASCS
#ifdef DATABASE_DBG
#define DATABASE_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define DATABASE_PUTASCS(Str, Len)
#endif
#endif

#ifndef DATABASE_PRINT
#ifdef DATABASE_DBG
#define DATABASE_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define DATABASE_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef DATABASE_DYN_PUTC
#ifdef DATABASE_DBG
#define DATABASE_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_DATABASE|Dbg), Char)
#else
#define DATABASE_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef DATABASE_DYN_PUTS
#ifdef DATABASE_DBG
#define DATABASE_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_DATABASE|Dbg), Str, Len)
#else
#define DATABASE_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef DATABASE_DYN_PUTASCC
#ifdef DATABASE_DBG
#define DATABASE_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_DATABASE|Dbg), Char)
#else
#define DATABASE_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef DATABASE_DYN_PUTASCS
#ifdef DATABASE_DBG
#define DATABASE_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_DATABASE|Dbg), Str, Len)
#else
#define DATABASE_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef DATABASE_DYN_PRINT
#ifdef DATABASE_DBG
#define DATABASE_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_DATABASE|Dbg), Fmt, ##Arg)
#else
#define DATABASE_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MSTR_FACTORY_DEFAULT_SN				"0000000000000"
#define MSTR_FACTORY_DEFAULT_LAN_MAC		"00:11:22:33:44:55"
#define MSTR_FACTORY_DEFAULT_WIFI_MAC		"00:11:22:33:44:56"
#define MSTR_FACTORY_DEFAULT_WAN_MAC		"00:11:22:33:44:57"

#define MUI_FACTORY_ROM_WAIT_TIME_MS_MAX	3000

#define MSTR_OPENWRT_MTD_TOOL				"/sbin/mtd"
#define MSTR_OPENWRT_MTD0_FILE				"/dev/mtd0"
#define MSTR_OPENWRT_MTD0_TMP_FILE			"/tmp/mtd0"
#define MUI_OPENWRT_MTD0_MAC_OFFSET			0x1FC00

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	S_FactoryInfo		sFactoryInfo;
	uint32_t			ui32Crc;
} S_FactoryAttrData;

#ifdef M_FOR_beeSAM3
typedef struct
{
	S_StrMacAddr		sLanMac;
	S_StrMacAddr		sWifiMac;
	S_StrMacAddr		sWanMac;
} S_Ar9331MacGroup;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static sem_t s_tFactoryRomMutex;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static const char* s_mac_to_str_mac (S_StrMacAddr *psStrMac, const S_MacAddr *psMacAddr);
static int s_factory_mac_cal (S_MacAddr *psDestMacAddr, S_MacAddr *psSrcMacAddr, int iDelta);
static int s_factory_mac_identify (const S_StrMacAddr *psStrMac, S_MacAddr *psMacAddr);
#ifdef M_FOR_beeSAM3
static int s_factory_mac_ext (const S_StrMacAddr *psStrMac, uint8_t ui8MacType, S_Ar9331MacGroup *psMacGroup);
static int s_factory_get_sys_mac (S_StrMacAddr *psStrMac);
static int s_factory_set_sys_mac (const S_StrMacAddr *psStrMac);
#endif
static int s_factory_data_rom_read (size_t tOffset, void *pvBuf, size_t tSize);
static int s_factory_data_rom_write (size_t tOffset, const void *pvBuf, size_t tSize);
static int s_factory_data_verify (const S_FactoryAttrData *psAttrData);

//-------------------------------------------------------------------------------------------------*
static const char* s_mac_to_str_mac (S_StrMacAddr *psStrMac, const S_MacAddr *psMacAddr)
{
	ASSERT(NULL != psStrMac);
	ASSERT(NULL != psMacAddr);

	if (MUI_STR_MAC_ADDR_LEN != snprintf(psStrMac->str, sizeof(psStrMac->str), "%02x:%02x:%02x:%02x:%02x:%02x", psMacAddr->aui8Mac[0], psMacAddr->aui8Mac[1], psMacAddr->aui8Mac[2], psMacAddr->aui8Mac[3], psMacAddr->aui8Mac[4], psMacAddr->aui8Mac[5]))
	{
		return NULL;
	}
	return psStrMac->str;
}

static int s_factory_mac_cal (S_MacAddr *psDestMacAddr, S_MacAddr *psSrcMacAddr, int iDelta)
{
	uint32_t ui32Low4Byte, ui32Delta, ui32Tmp;
	uint16_t ui16High2Byte;

	ASSERT(NULL != psDestMacAddr);
	ASSERT(NULL != psSrcMacAddr);

	if (0 == iDelta)
	{
		memcpy(psDestMacAddr, psSrcMacAddr, sizeof(S_MacAddr));
		return 0;
	}

	ui16High2Byte = (((uint16_t)psSrcMacAddr->aui8Mac[0])<<8) +
				    (((uint16_t)psSrcMacAddr->aui8Mac[1])<<0);
	ui32Low4Byte = (((uint32_t)psSrcMacAddr->aui8Mac[2])<<24) +
				   (((uint32_t)psSrcMacAddr->aui8Mac[3])<<16) +
				   (((uint32_t)psSrcMacAddr->aui8Mac[4])<<8) +
				   (((uint32_t)psSrcMacAddr->aui8Mac[5])<<0);

	if (iDelta > 0)
	{
		ui32Delta = (uint32_t)iDelta;
		ui32Tmp = ui32Low4Byte + ui32Delta;
		if (ui32Tmp < ui32Low4Byte)
		{
			ui16High2Byte++;
		}
		ui32Low4Byte = ui32Tmp;
	}
	else
	{
		ui32Delta = (uint32_t)(0-iDelta);
		ui32Tmp = ui32Low4Byte - ui32Delta;
		if (ui32Tmp > ui32Low4Byte)
		{
			ui16High2Byte--;
		}
		ui32Low4Byte = ui32Tmp;
	}

	psDestMacAddr->aui8Mac[0] = (uint8_t)((ui16High2Byte>>8) & 0x0FF);
	psDestMacAddr->aui8Mac[1] = (uint8_t)((ui16High2Byte>>0) & 0x0FF);
	psDestMacAddr->aui8Mac[2] = (uint8_t)((ui32Low4Byte>>24) & 0x0FF);
	psDestMacAddr->aui8Mac[3] = (uint8_t)((ui32Low4Byte>>16) & 0x0FF);
	psDestMacAddr->aui8Mac[4] = (uint8_t)((ui32Low4Byte>>8) & 0x0FF);
	psDestMacAddr->aui8Mac[5] = (uint8_t)((ui32Low4Byte>>0) & 0x0FF);

	return 0;
}

static int s_factory_mac_identify (const S_StrMacAddr *psStrMac, S_MacAddr *psMacAddr)
{
	int iRet;

	ASSERT(NULL != psStrMac);
	ASSERT(NULL != psMacAddr);

	iRet = sscanf(psStrMac->str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &psMacAddr->aui8Mac[0], &psMacAddr->aui8Mac[1], &psMacAddr->aui8Mac[2], &psMacAddr->aui8Mac[3], &psMacAddr->aui8Mac[4], &psMacAddr->aui8Mac[5]);
	if ((iRet!=6) || (0!=(psMacAddr->aui8Mac[0]&0x01)))
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_factory_mac_identify]MAC address is wrong.\r\n");
		return -1;
	}

	return 0;
}
#ifdef M_FOR_beeSAM3
static int s_factory_mac_ext (const S_StrMacAddr *psStrMac, uint8_t ui8MacType, S_Ar9331MacGroup *psMacGroup)
{
	S_MacAddr sMacAddr0, sMacAddr1, sMacAddr2;
	int iRet;

	ASSERT(NULL != psStrMac);
	ASSERT(ui8MacType <= 2);	//0:LAN; 1:WIFI; 2:WAN
	ASSERT(NULL != psMacGroup);

	if (0 == ui8MacType)
	{
		//LAN MAC
		iRet = s_factory_mac_identify(psStrMac, &sMacAddr0);
		if (iRet < 0)
		{
			return iRet;
		}

		//计算WIFI MAC
		s_factory_mac_cal(&sMacAddr1, &sMacAddr0, 1);
		//计算WAN MAC
		s_factory_mac_cal(&sMacAddr2, &sMacAddr0, 2);
	}
	else if (1 == ui8MacType)
	{
		//WIF MAC
		iRet = s_factory_mac_identify(psStrMac, &sMacAddr1);
		if (iRet < 0)
		{
			return iRet;
		}

		//计算LAN MAC
		s_factory_mac_cal(&sMacAddr0, &sMacAddr1, -1);
		//计算WAN MAC
		s_factory_mac_cal(&sMacAddr2, &sMacAddr1, 1);
	}
	else
	{
		//WAN MAC
		iRet = s_factory_mac_identify(psStrMac, &sMacAddr2);
		if (iRet < 0)
		{
			return iRet;
		}

		//计算LAN MAC
		s_factory_mac_cal(&sMacAddr0, &sMacAddr2, -2);
		//计算WAN MAC
		s_factory_mac_cal(&sMacAddr1, &sMacAddr2, -1);
	}

	if (NULL == s_mac_to_str_mac(&psMacGroup->sLanMac, &sMacAddr0))
	{
		return -1;
	}
	if (NULL == s_mac_to_str_mac(&psMacGroup->sWifiMac, &sMacAddr1))
	{
		return -1;
	}
	if (NULL == s_mac_to_str_mac(&psMacGroup->sWanMac, &sMacAddr2))
	{
		return -1;
	}

	return 0;
}

static int s_factory_get_sys_mac (S_StrMacAddr *psStrMac)
{
	S_MacAddr sMacAddr;
	int iRet;

	ASSERT(NULL != psStrMac);

	file_del(MSTR_OPENWRT_MTD0_TMP_FILE);

	iRet = file_cp(MSTR_OPENWRT_MTD0_TMP_FILE, MSTR_OPENWRT_MTD0_FILE);
	if (iRet < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_factory_get_sys_mac]Call 'file_cp(%s, %s)' fail.\r\n", MSTR_OPENWRT_MTD0_TMP_FILE, MSTR_OPENWRT_MTD0_FILE);
		return iRet;
	}

	iRet = file_read(MSTR_OPENWRT_MTD0_TMP_FILE, MUI_OPENWRT_MTD0_MAC_OFFSET, sMacAddr.aui8Mac, sizeof(sMacAddr.aui8Mac));
	if (iRet < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_factory_get_sys_mac]Call 'file_read(%s, ...)' fail.\r\n", MSTR_OPENWRT_MTD0_TMP_FILE);
		return iRet;
	}

	file_del(MSTR_OPENWRT_MTD0_TMP_FILE);

	if (NULL == s_mac_to_str_mac(psStrMac, &sMacAddr))
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_factory_get_sys_mac]Call 's_mac_to_str_mac()' fail.\r\n");
		return -1;
	}

	DATABASE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_factory_get_sys_mac]MAC=%s\r\n", psStrMac->str);

	return iRet;
}

static int s_factory_set_sys_mac (const S_StrMacAddr *psStrMac)
{
	S_MacAddr sMacAddr;
	int iRet;

	ASSERT(NULL != psStrMac);

	iRet = s_factory_mac_identify(psStrMac, &sMacAddr);
	if (iRet < 0)
	{
		return iRet;
	}

	file_del(MSTR_OPENWRT_MTD0_TMP_FILE);

	iRet = file_cp(MSTR_OPENWRT_MTD0_TMP_FILE, MSTR_OPENWRT_MTD0_FILE);
	if (iRet < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_factory_set_sys_mac]Call 'file_cp(%s, %s)' fail.\r\n", MSTR_OPENWRT_MTD0_TMP_FILE, MSTR_OPENWRT_MTD0_FILE);
		return iRet;
	}

	iRet = file_write(MSTR_OPENWRT_MTD0_TMP_FILE, MUI_OPENWRT_MTD0_MAC_OFFSET, sMacAddr.aui8Mac, sizeof(sMacAddr.aui8Mac));
	if (iRet < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_factory_set_sys_mac]Call 'file_write(%s, ...)' fail.\r\n", MSTR_OPENWRT_MTD0_TMP_FILE);
		return iRet;
	}

	system("sync");	//进行文件保存
	system(MSTR_OPENWRT_MTD_TOOL" -r write "MSTR_OPENWRT_MTD0_TMP_FILE" u-boot");	//System will reboot right now.

	file_del(MSTR_OPENWRT_MTD0_TMP_FILE);

	return 0;
}
#endif

static int s_factory_data_rom_read (size_t tOffset, void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(S_FactoryAttrData));	//不能超出数据的最大边界

	if (NULL == data_factory_file_path_get(achPath, sizeof(achPath)))
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_factory_data_rom_read]Can't get path of factory file.\r\n");
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_read(achPath, tOffset, pvBuf, tSize, &s_tFactoryRomMutex, MUI_FACTORY_ROM_WAIT_TIME_MS_MAX);
}
static int s_factory_data_rom_write (size_t tOffset, const void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(S_FactoryAttrData));	//不能超出数据的最大边界

	if (NULL == data_factory_file_path_get(achPath, sizeof(achPath)))
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_factory_data_rom_read]Can't get path of factory file.\r\n");
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_write(achPath, tOffset, pvBuf, tSize, &s_tFactoryRomMutex, MUI_FACTORY_ROM_WAIT_TIME_MS_MAX);
}
static int s_factory_data_verify (const S_FactoryAttrData *psAttrData)
{
	if (crc32_cal(&psAttrData->sFactoryInfo, sizeof(psAttrData->sFactoryInfo)) != psAttrData->ui32Crc)
	{
		return -1;
	}

	return 0;
}

static int s_write_factory_info (const S_FactoryInfo *psFactoryInfo)
{
	S_FactoryAttrData sAttrData;

	ASSERT(NULL	!= psFactoryInfo);

	memcpy(&sAttrData.sFactoryInfo, psFactoryInfo, sizeof(sAttrData.sFactoryInfo));
	sAttrData.ui32Crc = crc32_cal(&sAttrData.sFactoryInfo, sizeof(sAttrData.sFactoryInfo));
	return s_factory_data_rom_write(0, &sAttrData, sizeof(S_FactoryAttrData));
}
//................................................
int	read_factory_info (S_FactoryInfo *psFactoryInfo)
{
	ASSERT(NULL	!= psFactoryInfo);

	return s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo), psFactoryInfo, M_SIZEOF(S_FactoryAttrData, sFactoryInfo));
}
int	read_factory_info_sn (S_FactorySn *psFactorySn)
{
	ASSERT(NULL	!= psFactorySn);
	return s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo.sn), psFactorySn, M_SIZEOF(S_FactoryAttrData, sFactoryInfo.sn));
}
int	read_factory_info_mac (S_StrMacAddr *psFactoryMac)
{
	ASSERT(NULL	!= psFactoryMac);
	return s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo.mac), psFactoryMac, M_SIZEOF(S_FactoryAttrData, sFactoryInfo.mac));
}
int	read_factory_info_wifi_mac (S_StrMacAddr *psFactoryWifiMac)
{
	ASSERT(NULL	!= psFactoryWifiMac);
	return s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo.wifimac), psFactoryWifiMac, M_SIZEOF(S_FactoryAttrData, sFactoryInfo.wifimac));
}
//................................................
int	write_factory_info_sn (const S_FactorySn *psFactorySn)
{
	S_FactoryInfo sFactoryInfo;
	int iRet;

	ASSERT(NULL	!= psFactorySn);

	iRet = s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo), &sFactoryInfo, sizeof(sFactoryInfo));
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sFactoryInfo.sn, psFactorySn, sizeof(sFactoryInfo.sn));
	return s_write_factory_info(&sFactoryInfo);
}
int	write_factory_info_mac (const S_StrMacAddr *psFactoryMac)
#ifdef M_FOR_beeSAM3
{
	S_Ar9331MacGroup sMacGroup;
	S_FactoryInfo sFactoryInfo;
	int iRet;

	ASSERT(NULL	!= psFactoryMac);

	iRet = s_factory_mac_ext(psFactoryMac, 0, &sMacGroup);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo), &sFactoryInfo, sizeof(sFactoryInfo));
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sFactoryInfo.mac, &sMacGroup.sLanMac, sizeof(sFactoryInfo.mac));
	memcpy(&sFactoryInfo.wifimac, &sMacGroup.sWifiMac, sizeof(sFactoryInfo.wifimac));
	iRet = s_write_factory_info(&sFactoryInfo);
	if (iRet < 0)
	{
		return iRet;
	}

	return s_factory_set_sys_mac(&sMacGroup.sWifiMac);
}
#else
{
	S_FactoryInfo sFactoryInfo;
	int iRet;

	ASSERT(NULL	!= psFactoryMac);

	iRet = s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo), &sFactoryInfo, sizeof(sFactoryInfo));
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sFactoryInfo.mac, psFactoryMac, sizeof(sFactoryInfo.mac));
	return s_write_factory_info(&sFactoryInfo);
}
#endif

int	write_factory_info_wifimac (const S_StrMacAddr *psFactoryWifiMac)
#ifdef M_FOR_beeSAM3
{
	S_Ar9331MacGroup sMacGroup;
	S_FactoryInfo sFactoryInfo;
	int iRet;

	ASSERT(NULL	!= psFactoryWifiMac);

	iRet = s_factory_mac_ext(psFactoryWifiMac, 1, &sMacGroup);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo), &sFactoryInfo, sizeof(sFactoryInfo));
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sFactoryInfo.mac, &sMacGroup.sLanMac, sizeof(sFactoryInfo.mac));
	memcpy(&sFactoryInfo.wifimac, &sMacGroup.sWifiMac, sizeof(sFactoryInfo.wifimac));
	iRet = s_write_factory_info(&sFactoryInfo);
	if (iRet < 0)
	{
		return iRet;
	}

	return s_factory_set_sys_mac(&sMacGroup.sWifiMac);
}
#else
{
	S_FactoryInfo sFactoryInfo;
	int iRet;

	ASSERT(NULL	!= psFactoryWifiMac);

	iRet = s_factory_data_rom_read(M_OFFSET(S_FactoryAttrData, sFactoryInfo), &sFactoryInfo, sizeof(sFactoryInfo));
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sFactoryInfo.wifimac, psFactoryWifiMac, sizeof(sFactoryInfo.wifimac));
	return s_write_factory_info(&sFactoryInfo);
}
#endif

int factory_file_init (void)
{
#ifdef M_FOR_beeSAM3
	S_Ar9331MacGroup sMacGroup;
#endif
	S_FactoryAttrData sAttrData;
	int iRet;

	iRet = sem_init(&s_tFactoryRomMutex, 0, 1);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = s_factory_data_rom_read(0, &sAttrData, sizeof(S_FactoryAttrData));
	if (iRet<0 || s_factory_data_verify(&sAttrData)!=0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[factory_file_init]Factory file is wrong, will initialize this file.\r\n");
		memset(&sAttrData, 0, sizeof(sAttrData));
		memcpy(sAttrData.sFactoryInfo.sn.str, MSTR_FACTORY_DEFAULT_SN, sizeof(sAttrData.sFactoryInfo.sn.str));
		memcpy(sAttrData.sFactoryInfo.mac.str, MSTR_FACTORY_DEFAULT_LAN_MAC, sizeof(sAttrData.sFactoryInfo.mac.str));
		memcpy(sAttrData.sFactoryInfo.wifimac.str, MSTR_FACTORY_DEFAULT_WIFI_MAC, sizeof(sAttrData.sFactoryInfo.wifimac.str));
		iRet = s_write_factory_info(&sAttrData.sFactoryInfo);
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[factory_file_init]Initialize factory file fail.\r\n");
			return iRet;
		}
	}

#ifdef M_FOR_beeSAM3
	iRet = s_factory_get_sys_mac(&sMacGroup.sWifiMac);
	if (iRet < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[factory_file_init]Call 's_factory_get_sys_mac' fail.\r\n");
		return iRet;
	}
	iRet = s_factory_mac_ext(&sMacGroup.sWifiMac, 1, &sMacGroup);
	if (iRet < 0)
	{
		return iRet;
	}

	if ((0!=memcmp(&sMacGroup.sLanMac, &sAttrData.sFactoryInfo.mac, sizeof(sMacGroup.sLanMac))) ||
		(0!=memcmp(&sMacGroup.sWifiMac, &sAttrData.sFactoryInfo.wifimac, sizeof(sMacGroup.sWifiMac))))
	{
		memcpy(&sAttrData.sFactoryInfo.mac, &sMacGroup.sLanMac, sizeof(sAttrData.sFactoryInfo.mac));
		memcpy(&sAttrData.sFactoryInfo.wifimac, &sMacGroup.sWifiMac, sizeof(sAttrData.sFactoryInfo.wifimac));
		iRet = s_write_factory_info(&sAttrData.sFactoryInfo);
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[factory_file_init]Synchronize MAC address fail.\r\n");
			return iRet;
		}
	}
#endif

	return 0;
}

int database_base_file_init (void)
{
	return factory_file_init();
}



int database_init (void)
{
	int iRet;

	do
	{
		iRet = database_base_file_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'database_base_file_init()' fail.\r\n");
			break;
		}

		iRet = config_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'config_data_init()' fail.\r\n");
			break;
		}

		iRet = user_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'user_data_init()' fail.\r\n");
			break;
		}

		iRet = dev_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'dev_data_init()' fail.\r\n");
			break;
		}

		iRet = extend_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'extend_data_init()' fail.\r\n");
			break;
		}

		iRet = group_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'group_data_init()' fail.\r\n");
			break;
		}

		iRet = set_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'set_data_init()' fail.\r\n");
			break;
		}

		iRet = scene_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'scene_data_init()' fail.\r\n");
			break;
		}

		iRet = schedule_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[database_init]Call 'schedule_data_init()' fail.\r\n");
			break;
		}

		iRet = rule_data_init();
		if (iRet < 0)
		{
			DATABASE_DYN_PRINT(M_DBG_LEVEL_INFO, "[database_init]Call 'rule_data_init()' fail.\r\n");
			break;
		}
	} while (0);

	if (iRet >= 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[database_init]Database initialize successfully.\r\n");
	}

	//config_data_test();
	//group_data_test();
	//user_data_test();
	//dev_data_test();

	return iRet;
}

int database_read (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize, uint8_t ui8Mode)
{
	ASSERT((MUI_DATA_TYPE_START<=ui16Type)&&(ui16Type<=MUI_DATA_TYPE_END));
	ASSERT(NULL != pvBuf);
	ASSERT((ui8Mode==M_MEM_TYPE_RAM) || (ui8Mode==M_MEM_TYPE_FLASH));

	switch (ui16Type)
	{
		case MUI_DATA_TYPE_CONFIG:
			if (M_MEM_TYPE_RAM == ui8Mode)
			{
				return config_data_ram_read(tOffset, pvBuf, tSize);
			}
			else //(M_MEM_TYPE_FLASH == ui8Mode)
			{
				return config_data_rom_read(tOffset, pvBuf, tSize);
			}
		case MUI_DATA_TYPE_USER:
			return 0;
		case MUI_DATA_TYPE_GROUP:
			return 0;
		case MUI_DATA_TYPE_SET:
			return 0;
		case MUI_DATA_TYPE_SCHED:
			return 0;
		case MUI_DATA_TYPE_RULE:
			return 0;
		default: //(MUI_DATA_TYPE_DEV_START, MUI_DATA_TYPE_DEV_END)
			if (M_MEM_TYPE_RAM == ui8Mode)
			{
				return dev_data_ram_read(ui16Type, ui16Id, tOffset, pvBuf, tSize);
			}
			else //(M_MEM_TYPE_FLASH == ui8Mode)
			{
				return dev_data_rom_read(ui16Type, ui16Id, tOffset, pvBuf, tSize);
			}
		}
}

int database_write (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize, uint8_t ui8Mode)
{
	ASSERT((MUI_DATA_TYPE_START<=ui16Type)&&(ui16Type<=MUI_DATA_TYPE_END));
	ASSERT(NULL != pvBuf);
	ASSERT((ui8Mode==M_MEM_TYPE_RAM) || (ui8Mode==M_MEM_TYPE_FLASH));

	switch (ui16Type)
	{
		case MUI_DATA_TYPE_CONFIG:
			if (M_MEM_TYPE_RAM == ui8Mode)
			{
				return config_data_ram_write(tOffset, pvBuf, tSize);
			}
			else //(M_MEM_TYPE_FLASH == ui8Mode)
			{
				return config_data_rom_write(tOffset, pvBuf, tSize);
			}
		case MUI_DATA_TYPE_USER:
			return 0;
		case MUI_DATA_TYPE_GROUP:
			return 0;
		case MUI_DATA_TYPE_SET:
			return 0;
		case MUI_DATA_TYPE_SCHED:
			return 0;
		case MUI_DATA_TYPE_RULE:
			return 0;
		default: //(MUI_DATA_TYPE_DEV_START, MUI_DATA_TYPE_DEV_END)
			if (M_MEM_TYPE_RAM == ui8Mode)
			{
				return dev_data_ram_write(ui16Type, ui16Id, tOffset, pvBuf, tSize);
			}
			else //(M_MEM_TYPE_FLASH == ui8Mode)
			{
				return dev_data_rom_write(ui16Type, ui16Id, tOffset, pvBuf, tSize);
			}
		}
}

int database_multi_write (uint16_t ui16Type, uint16_t ui16Id, size_t tRamOffset, const void *pvRamBuf, size_t tRamSize, size_t tRomOffset, const void *pvRomBuf, size_t tRomSize, uint8_t ui8Mode)
{
	ASSERT((MUI_DATA_TYPE_START<=ui16Type)&&(ui16Type<=MUI_DATA_TYPE_END));
	ASSERT(NULL != pvRamBuf);
	ASSERT(NULL != pvRomBuf);

	UNUSED(ui8Mode);

	switch (ui16Type)
	{
		case MUI_DATA_TYPE_CONFIG:
			return config_data_ram_rom_write(tRamOffset, pvRamBuf, tRamSize, tRomOffset, pvRomBuf, tRomSize);
		case MUI_DATA_TYPE_USER:
			return 0;
		case MUI_DATA_TYPE_GROUP:
			return 0;
		case MUI_DATA_TYPE_SET:
			return 0;
		case MUI_DATA_TYPE_SCHED:
			return 0;
		case MUI_DATA_TYPE_RULE:
			return 0;
		default: //(MUI_DATA_TYPE_DEV_START, MUI_DATA_TYPE_DEV_END)
			return dev_data_ram_rom_write(ui16Type, ui16Id, tRamOffset, pvRamBuf, tRamSize, tRomOffset, pvRomBuf, tRomSize);
		}
}

int database_del_all (void)
{
	int iRet, iTmp;

	iRet = 0;
	//config
#ifndef M_FOR_NEWLAND
	//user
	iRet += (iTmp = user_data_del_all());
	if (iTmp < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[database_del_all]Call 'user_data_del_all()' fail.\r\n");
	}
#endif
	//group
	iRet += (iTmp = group_data_del_all());
	if (iTmp < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[database_del_all]Call 'group_data_del_all()' fail.\r\n");
	}
	//set
	iRet += (iTmp = set_data_del_all());
	if (iTmp < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[database_del_all]Call 'set_data_del_all()' fail.\r\n");
	}
	//schedule
	iRet += (iTmp = schedule_data_del_all());
	if (iTmp < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[database_del_all]Call 'schedule_data_del_all()' fail.\r\n");
	}
	//rule
	iRet += (iTmp = rule_data_del_all());
	if (iTmp < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[database_del_all]Call 'rule_data_del_all()' fail.\r\n");
	}
	//device
	iRet += (iTmp = dev_data_del_all());
	if (iTmp < 0)
	{
		DATABASE_DYN_PRINT(M_DBG_LEVEL_ERR, "[database_del_all]Call 'dev_data_del_all()' fail.\r\n");
	}

	return iRet;
}

