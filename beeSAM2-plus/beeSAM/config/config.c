#include "common.h"
#include "config.h"
#include "api_deal.h"
#include "at_port_linux.h"
#include "misc.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define CONFIG_DBG

#ifndef CONFIG_PUTC
#ifdef CONFIG_DBG
#define CONFIG_PUTC(Char)			PUTC(Char)
#else
#define CONFIG_PUTC(Char)
#endif
#endif

#ifndef CONFIG_PUTS
#ifdef CONFIG_DBG
#define CONFIG_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define CONFIG_PUTS(Str, Len)
#endif
#endif

#ifndef CONFIG_PUTASCC
#ifdef CONFIG_DBG
#define CONFIG_PUTASCC(Char)			PUTASCC(Char)
#else
#define CONFIG_PUTASCC(Char)
#endif
#endif

#ifndef CONFIG_PUTASCS
#ifdef CONFIG_DBG
#define CONFIG_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define CONFIG_PUTASCS(Str, Len)
#endif
#endif

#ifndef CONFIG_PRINT
#ifdef CONFIG_DBG
#define CONFIG_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define CONFIG_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef CONFIG_DYN_PUTC
#ifdef CONFIG_DBG
#define CONFIG_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_CONFIG|Dbg), Char)
#else
#define CONFIG_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef CONFIG_DYN_PUTS
#ifdef CONFIG_DBG
#define CONFIG_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_CONFIG|Dbg), Str, Len)
#else
#define CONFIG_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef CONFIG_DYN_PUTASCC
#ifdef CONFIG_DBG
#define CONFIG_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_CONFIG|Dbg), Char)
#else
#define CONFIG_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef CONFIG_DYN_PUTASCS
#ifdef CONFIG_DBG
#define CONFIG_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_CONFIG|Dbg), Str, Len)
#else
#define CONFIG_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef CONFIG_DYN_PRINT
#ifdef CONFIG_DBG
#define CONFIG_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_CONFIG|Dbg), Fmt, ##Arg)
#else
#define CONFIG_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#if defined(M_FOR_beeSAM2_plus)
#define MSTR_CONFIG_DEFAULT_NAME				"beeSAM2-plus"
#elif defined(M_FOR_beeSAM3)
#define MSTR_CONFIG_DEFAULT_NAME				"beeSAM3"
#else
#define MSTR_CONFIG_DEFAULT_NAME				"Intercel-HA-GW"
#endif
#define MSTR_CONFIG_DEFAULT_UZIP_VER			"Unknow"
#define MSTR_CONFIG_DEFAULT_UZIP_MAC			"Unknow"
#define MSTR_CONFIG_DEFAULT_UZIP_SN				"Unknow"
#define MSTR_CONFIG_DEFAULT_SW_UPDATA_URL		"sz.intercel.com.cn/beeSAM3/upgrade/beeSAM3_linux"
#define MSTR_CONFIG_DEFAULT_SW_UPDATA_TEXT		"This is a software update"
#define MSTR_CONFIG_DEFAULT_PROXY_ADDR			"ws://sz.intercel.com.cn"
#define MUI_CONFIG_DEFAULT_PROXY_PORT			56788
#define MSTR_CONFIG_DEFAULT_DEV_ID				"20150101000000"
#define MSTR_CONFIG_DEFAULT_PASSWD				"123456"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int s_config_ip_addr (const S_ConfigInfo *psConfigInfo);
static int s_config_verify (const S_ConfigAttrData *psAttrData);
static int s_revise_config_info (S_ConfigInfo *psConfigInfo);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static int s_config_ip_addr (const S_ConfigInfo *psConfigInfo)
{
#ifdef M_FOR_beeSAM3
	char achBuf[256];

	if (1 == psConfigInfo->dhcp)
	{
		system("uci delete network.lan.ipaddr");	//本机IP地址
		system("uci delete network.lan.netmask");	//netmask
		system("uci delete network.lan.gateway");	//gateway设置
		system("uci set network.lan.proto=dhcp");	//协议为dhcp
		system("uci set dhcp.lan.ra_management=1");	//配置为动态IP
	}
	else
	{
		snprintf(achBuf, sizeof(achBuf), "uci set network.lan.ipaddr=%s", psConfigInfo->ipaddr.str);
		system(achBuf);		//本机IP地址
		snprintf(achBuf, sizeof(achBuf), "uci set network.lan.netmask=%s", psConfigInfo->netmask.str);
		system(achBuf);		//netmask设置
		snprintf(achBuf, sizeof(achBuf), "uci set network.lan.gateway=%s", psConfigInfo->gwipaddr.str);
		system(achBuf);		//gateway设置
		system("uci set network.lan.proto=static");	//配置为静态IP
	}

	system("uci commit");					//确认置行uci配置命令
	system("/etc/init.d/network restart");	//重启网络使上述配置生效
#endif
	return 0;
}

static int s_config_verify (const S_ConfigAttrData *psAttrData)
{
	if (crc32_cal(&psAttrData->sConfigInfo, sizeof(psAttrData->sConfigInfo)) != psAttrData->ui32Crc)
	{
		return -1;
	}

	return 0;
}

//-	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -*
static int s_revise_config_info (S_ConfigInfo *psConfigInfo)
{
	S_FactoryInfo sFactoryInfo;

	read_factory_info(&sFactoryInfo);

	memcpy(&psConfigInfo->sn, &sFactoryInfo.sn, sizeof(psConfigInfo->sn));							//S_FactorySn		sn;
	psConfigInfo->databasever = get_database_version();												//uint32_t			databasever;

	//S_CfgName			name;
	get_sw_version(psConfigInfo->swver.str, sizeof(psConfigInfo->swver.str));						//S_CfgSwVer		swver;
	memcpy(&psConfigInfo->wifimac, &sFactoryInfo.wifimac, sizeof(psConfigInfo->wifimac));			//S_StrMacAddr		wifimac;
	memcpy(&psConfigInfo->mac, &sFactoryInfo.mac, sizeof(psConfigInfo->mac));						//S_StrMacAddr		mac;
	strncpy(psConfigInfo->ipaddr.str, net_listen_ip_addr_get(), sizeof(psConfigInfo->ipaddr.str));	//S_StrIpAddr		ipaddr;
	strncpy(psConfigInfo->netmask.str, net_mask_get(), sizeof(psConfigInfo->netmask.str));			//S_StrIpAddr		netmask;
	strncpy(psConfigInfo->gwipaddr.str, net_gw_ip_addr_get(), sizeof(psConfigInfo->gwipaddr.str));	//S_StrIpAddr		gwipaddr;
	//uint8_t				dhcp;

	//S_CfgUzipVer		uzipver;
	//S_CfgUzipMac		uzipmac;
	//S_CfgUzipMac		uzipsn;

	time(&psConfigInfo->utc.time);																	//S_CfgUtc			utc;

	//S_CfgSwUpdate		swupdate;

	//S_CfgProxyAddr		proxyaddr;
	//uint16_t			proxyport;
	//uint8_t				portalservices;

	//S_StrDevId			devid;
	//S_StrPasswd			passwd;

	psConfigInfo->linkbutton = get_linbutton_key();													//uint8_t				linkbutton;

	//uint32_t			whitelist;	//计数

	return 0;
}
//--------------------------------------------------------------
int	read_config_info (S_ConfigInfo *psConfigInfo)
{
	int	iRet;

	ASSERT(NULL	!= psConfigInfo);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), psConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}

	s_revise_config_info(psConfigInfo);

	return iRet;
}
//................................................
int	read_config_info_sn (S_FactorySn *psFactorySn)
{
	ASSERT(NULL	!= psFactorySn);
	return 	read_factory_info_sn(psFactorySn);
}
int	read_config_info_database_ver (uint32_t *pui32DatabaseVer)
{
	ASSERT(NULL	!= pui32DatabaseVer);
#if 1
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.databasever), pui32DatabaseVer, M_SIZEOF(S_ConfigAttrData, sConfigInfo.databasever), M_MEM_TYPE_FLASH);
#else
	*pui32DatabaseVer = get_database_version();
	return 0;
#endif
}
int	read_config_info_name (S_CfgName *psName)
{
	ASSERT(NULL	!= psName);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.name), psName, M_SIZEOF(S_ConfigAttrData, sConfigInfo.name), M_MEM_TYPE_FLASH);
}
int	read_config_info_sw_ver (S_CfgSwVer *psSwVer)
{
	ASSERT(NULL	!= psSwVer);
	get_sw_version(psSwVer->str, sizeof(psSwVer->str));
	return 0;
}
int	read_config_info_wifi_mac (S_StrMacAddr *psWifiMac)
{
	ASSERT(NULL	!= psWifiMac);
	return read_factory_info_wifi_mac(psWifiMac);
}
int	read_config_info_mac (S_StrMacAddr *psMac)
{
	ASSERT(NULL	!= psMac);
	return read_factory_info_mac(psMac);
}
int	read_config_info_ip_addr (S_StrIpAddr *psIpAddr)
{
	ASSERT(NULL	!= psIpAddr);
	strncpy(psIpAddr->str, net_listen_ip_addr_get(), sizeof(psIpAddr->str));
	return 0;
}
int	read_config_info_netmask (S_StrIpAddr *psNetmask)
{
	ASSERT(NULL	!= psNetmask);
	strncpy(psNetmask->str, net_mask_get(), sizeof(psNetmask->str));
	return 0;
}
int	read_config_info_gw_ip_addr (S_StrIpAddr *psGwIpAddr)
{
	ASSERT(NULL	!= psGwIpAddr);
	strncpy(psGwIpAddr->str, net_gw_ip_addr_get(), sizeof(psGwIpAddr->str));
	return 0;
}
int	read_config_info_dhcp (uint8_t *pui8Dhcp)
{
	ASSERT(NULL	!= pui8Dhcp);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.dhcp), pui8Dhcp, M_SIZEOF(S_ConfigAttrData, sConfigInfo.dhcp), M_MEM_TYPE_FLASH);
}
int	read_config_info_uzip_ver (S_CfgUzipVer *psUzipVer)
{
	ASSERT(NULL	!= psUzipVer);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.uzipver), psUzipVer, M_SIZEOF(S_ConfigAttrData, sConfigInfo.uzipver), M_MEM_TYPE_FLASH);
}
int	read_config_info_uzip_mac (S_CfgUzipMac *psUzipMac)
{
	ASSERT(NULL	!= psUzipMac);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.uzipmac), psUzipMac, M_SIZEOF(S_ConfigAttrData, sConfigInfo.uzipmac), M_MEM_TYPE_FLASH);
}
int	read_config_info_uzip_sn (S_CfgUzipSn *psUzipSn)
{
	ASSERT(NULL	!= psUzipSn);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.uzipsn), psUzipSn, M_SIZEOF(S_ConfigAttrData, sConfigInfo.uzipsn), M_MEM_TYPE_FLASH);
}
int	read_config_info_utc (S_CfgUtc *psUtc)
{
	ASSERT(NULL	!= psUtc);
	time(&psUtc->time);
	return 0;
}
int	read_config_info_sw_update (S_CfgUzipVer *psSwUpdate)
{
	ASSERT(NULL	!= psSwUpdate);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.swupdate), psSwUpdate, M_SIZEOF(S_ConfigAttrData, sConfigInfo.swupdate), M_MEM_TYPE_FLASH);
}
int	read_config_info_proxy_addr (S_CfgProxyAddr *psProxyAddr)
{
	ASSERT(NULL	!= psProxyAddr);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.proxyaddr), psProxyAddr, M_SIZEOF(S_ConfigAttrData, sConfigInfo.proxyaddr), M_MEM_TYPE_FLASH);
}
int	read_config_info_proxy_port (uint16_t *pui16ProxyPort)
{
	ASSERT(NULL	!= pui16ProxyPort);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.proxyport), pui16ProxyPort, M_SIZEOF(S_ConfigAttrData, sConfigInfo.proxyport), M_MEM_TYPE_FLASH);
}
int	read_config_info_portal_services (uint8_t *pui8PortalServices)
{
	ASSERT(NULL	!= pui8PortalServices);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.portalservices), pui8PortalServices, M_SIZEOF(S_ConfigAttrData, sConfigInfo.portalservices), M_MEM_TYPE_FLASH);
}
int	read_config_info_dev_id (S_StrDevId *psDevId)
{
	ASSERT(NULL	!= psDevId);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.devid), psDevId, M_SIZEOF(S_ConfigAttrData, sConfigInfo.devid), M_MEM_TYPE_FLASH);
}
int	read_config_info_passwd (S_StrPasswd *psPasswd)
{
	ASSERT(NULL	!= psPasswd);
	return database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo.passwd), psPasswd, M_SIZEOF(S_ConfigAttrData, sConfigInfo.passwd), M_MEM_TYPE_FLASH);
}
int	read_config_info_link_button (uint8_t *pui8LinkButton)
{
	uint8_t ui8Val = get_linbutton_key();

	if (NULL != pui8LinkButton)
	{
		*pui8LinkButton = ui8Val;
	}
	return (int)ui8Val;
}
//uint32_t			whitelist;	//计数

int	write_config_info (const S_ConfigInfo *psConfigInfo)
{
	S_ConfigAttrData sConfigAttrData;

	ASSERT(NULL	!= psConfigInfo);

	memcpy(&sConfigAttrData.sConfigInfo, psConfigInfo, sizeof(sConfigAttrData.sConfigInfo));
	sConfigAttrData.ui32Crc = crc32_cal(&sConfigAttrData.sConfigInfo, sizeof(sConfigAttrData.sConfigInfo));
	return database_write(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, 0, &sConfigAttrData, sizeof(S_ConfigAttrData), M_MEM_TYPE_FLASH);
}
//................................................
int	write_config_info_sn (const S_FactorySn *psFactorySn)
{
	UNUSED(psFactorySn);
	return FILE_S_ERR_UNSUPPORT;
}
int	write_config_info_database_ver (const uint32_t *pui32DatabaseVer)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= pui32DatabaseVer);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.databasever, pui32DatabaseVer, sizeof(sConfigInfo.databasever));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_name (const S_CfgName *psName)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psName);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.name, psName, sizeof(sConfigInfo.name));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_sw_ver (const S_CfgSwVer *psSwVer)
{
	UNUSED(psSwVer);
	return FILE_S_ERR_UNSUPPORT;
}
int	write_config_info_wifi_mac (const S_StrMacAddr *psWifiMac)
{
	UNUSED(psWifiMac);
	return FILE_S_ERR_UNSUPPORT;
}
int	write_config_info_mac (const S_StrMacAddr *psMac)
{
	UNUSED(psMac);
	return FILE_S_ERR_UNSUPPORT;
}
int	write_config_info_ip_addr (const S_StrIpAddr *psIpAddr)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psIpAddr);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 == memcmp(&sConfigInfo.ipaddr, psIpAddr, sizeof(sConfigInfo.ipaddr)))
	{
		return 0;
	}
	memcpy(&sConfigInfo.ipaddr, psIpAddr, sizeof(sConfigInfo.ipaddr));
	iRet = write_config_info(&sConfigInfo);
	if (iRet < 0)
	{
		return iRet;
	}

	if (0 == sConfigInfo.dhcp)
	{
		iRet = s_config_ip_addr(&sConfigInfo);
	}

	return iRet;
}
int	write_config_info_netmask (const S_StrIpAddr *psNetmask)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psNetmask);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 == memcmp(&sConfigInfo.netmask, psNetmask, sizeof(sConfigInfo.netmask)))
	{
		return 0;
	}
	memcpy(&sConfigInfo.netmask, psNetmask, sizeof(sConfigInfo.netmask));
	iRet = write_config_info(&sConfigInfo);
	if (iRet < 0)
	{
		return iRet;
	}

	if (0 == sConfigInfo.dhcp)
	{
		iRet = s_config_ip_addr(&sConfigInfo);
	}

	return iRet;
}
int	write_config_info_gw_ip_addr (const S_StrIpAddr *psGwIpAddr)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psGwIpAddr);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	if (0 == memcmp(&sConfigInfo.gwipaddr, psGwIpAddr, sizeof(sConfigInfo.gwipaddr)))
	{
		return 0;
	}
	memcpy(&sConfigInfo.gwipaddr, psGwIpAddr, sizeof(sConfigInfo.gwipaddr));
	iRet = write_config_info(&sConfigInfo);
	if (iRet < 0)
	{
		return iRet;
	}

	if (0 == sConfigInfo.dhcp)
	{
		iRet = s_config_ip_addr(&sConfigInfo);
	}

	return iRet;
}
int	write_config_info_dhcp (const uint8_t *pui8Dhcp)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= pui8Dhcp);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	if (sConfigInfo.dhcp == *pui8Dhcp)
	{
		return 0;
	}
	sConfigInfo.dhcp = *pui8Dhcp;
	iRet = write_config_info(&sConfigInfo);
	if (iRet < 0)
	{
		return iRet;
	}

	return s_config_ip_addr(&sConfigInfo);
}
int	write_config_info_uzip_ver (const S_CfgUzipVer *psUzipVer)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psUzipVer);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.uzipver, psUzipVer, sizeof(sConfigInfo.uzipver));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_uzip_mac (const S_CfgUzipMac *psUzipMac)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psUzipMac);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.uzipmac, psUzipMac, sizeof(sConfigInfo.uzipmac));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_uzip_sn (const S_CfgUzipSn *psUzipSn)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psUzipSn);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.uzipsn, psUzipSn, sizeof(sConfigInfo.uzipsn));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_utc (const S_CfgUtc *psUtc)
{
	UNUSED(psUtc);
	return FILE_S_ERR_UNSUPPORT;
}
int	write_config_info_sw_update (const S_CfgUzipVer *psSwUpdate)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psSwUpdate);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.swupdate, psSwUpdate, sizeof(sConfigInfo.swupdate));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_proxy_addr (const S_CfgProxyAddr *psProxyAddr)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psProxyAddr);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.proxyaddr, psProxyAddr, sizeof(sConfigInfo.proxyaddr));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_proxy_port (const uint16_t *pui16ProxyPort)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= pui16ProxyPort);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	sConfigInfo.proxyport = *pui16ProxyPort;
	return write_config_info(&sConfigInfo);
}
int	write_config_info_portal_services (const uint8_t *pui8PortalServices)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= pui8PortalServices);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	sConfigInfo.portalservices = *pui8PortalServices;
	return write_config_info(&sConfigInfo);
}
int	write_config_info_dev_id (const S_StrDevId *psDevId)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psDevId);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.devid, psDevId, sizeof(sConfigInfo.devid));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_passwd (const S_StrPasswd *psPasswd)
{
	S_ConfigInfo sConfigInfo;
	int iRet;

	ASSERT(NULL	!= psPasswd);

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, M_OFFSET(S_ConfigAttrData, sConfigInfo), &sConfigInfo, M_SIZEOF(S_ConfigAttrData, sConfigInfo), M_MEM_TYPE_FLASH);
	if (iRet < 0)
	{
		return iRet;
	}
	memcpy(&sConfigInfo.passwd, psPasswd, sizeof(sConfigInfo.passwd));
	return write_config_info(&sConfigInfo);
}
int	write_config_info_link_button (const uint8_t *pui8LinkButton)
{
	UNUSED(pui8LinkButton);
	return FILE_S_ERR_UNSUPPORT;
}
//uint32_t			whitelist;	//计数
//-	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -*

int	config_init	(void)
{
	S_ConfigAttrData sConfigAttrData;
	int iRet, iModify = 0;

	iRet = database_read(MUI_DATA_TYPE_CONFIG, MUI_DATABASE_ID_START, 0, &sConfigAttrData, sizeof(S_ConfigAttrData), M_MEM_TYPE_FLASH);
	if (iRet<0 || s_config_verify(&sConfigAttrData)!=0)
	{
		CONFIG_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_init]Config file is wrong, will initialize this file.\r\n");
		memset(&sConfigAttrData, 0, sizeof(sConfigAttrData));

		//S_FactorySn		sn;
		sConfigAttrData.sConfigInfo.databasever = get_database_version();

		memcpy(sConfigAttrData.sConfigInfo.name.str, MSTR_CONFIG_DEFAULT_NAME, sizeof(MSTR_CONFIG_DEFAULT_NAME)-1);						//S_CfgName			name;
		//S_CfgSwVer		swver;
		//S_StrMacAddr		wifimac;
		//S_StrMacAddr		mac;
		memcpy(sConfigAttrData.sConfigInfo.ipaddr.str, "192.168.1.100", sizeof("192.168.1.100"));
		memcpy(sConfigAttrData.sConfigInfo.netmask.str, "255.255.255.0", sizeof("255.255.255.0"));
		memcpy(sConfigAttrData.sConfigInfo.gwipaddr.str, "192.168.1.1", sizeof("192.168.1.1"));
		sConfigAttrData.sConfigInfo.dhcp = 0;																							//uint8_t			dhcp;

		memcpy(sConfigAttrData.sConfigInfo.uzipver.str, MSTR_CONFIG_DEFAULT_UZIP_VER, sizeof(MSTR_CONFIG_DEFAULT_UZIP_VER)-1);			//S_CfgUzipVer		uzipver;
		memcpy(sConfigAttrData.sConfigInfo.uzipmac.str, MSTR_CONFIG_DEFAULT_UZIP_MAC, sizeof(MSTR_CONFIG_DEFAULT_UZIP_MAC)-1);			//S_CfgUzipMac		uzipmac;
		memcpy(sConfigAttrData.sConfigInfo.uzipsn.str, MSTR_CONFIG_DEFAULT_UZIP_SN, sizeof(MSTR_CONFIG_DEFAULT_UZIP_SN)-1);				//S_CfgUzipMac		uzipsn;

		//S_CfgUtc			utc;

		sConfigAttrData.sConfigInfo.swupdate.updatestate = 0;																			//S_CfgSwUpdate		swupdate;
		memcpy(sConfigAttrData.sConfigInfo.swupdate.url, MSTR_CONFIG_DEFAULT_SW_UPDATA_URL, sizeof(MSTR_CONFIG_DEFAULT_SW_UPDATA_URL)-1);
		memcpy(sConfigAttrData.sConfigInfo.swupdate.text, MSTR_CONFIG_DEFAULT_SW_UPDATA_TEXT, sizeof(MSTR_CONFIG_DEFAULT_SW_UPDATA_TEXT)-1);
		sConfigAttrData.sConfigInfo.swupdate.notify = 0;

		memcpy(sConfigAttrData.sConfigInfo.proxyaddr.str, MSTR_CONFIG_DEFAULT_PROXY_ADDR, sizeof(MSTR_CONFIG_DEFAULT_PROXY_ADDR)-1);	//S_CfgProxyAddr		proxyaddr;
		sConfigAttrData.sConfigInfo.proxyport = MUI_CONFIG_DEFAULT_PROXY_PORT;																						//uint16_t			proxyport;
		sConfigAttrData.sConfigInfo.portalservices = 1;																					//uint8_t			portalservices;

		memcpy(sConfigAttrData.sConfigInfo.devid.str, MSTR_CONFIG_DEFAULT_DEV_ID, sizeof(MSTR_CONFIG_DEFAULT_DEV_ID)-1);				//S_StrDevId			devid;
		memcpy(sConfigAttrData.sConfigInfo.passwd.str, MSTR_CONFIG_DEFAULT_PASSWD, sizeof(MSTR_CONFIG_DEFAULT_PASSWD)-1);				//S_StrPasswd			passwd;

		//uint8_t			linkbutton;

		//uint32_t			whitelist;	//计数

		iRet = write_config_info(&sConfigAttrData.sConfigInfo);
		if (iRet < 0)
		{
			CONFIG_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_init]Call 'write_config_info()' fail.\r\n");
			return iRet;
		}

		iRet = s_config_ip_addr(&sConfigAttrData.sConfigInfo);
		if (iRet < 0)
		{
			CONFIG_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_init]Call 's_config_ip_addr()' fail.\r\n");
			return iRet;
		}

		iModify = 1;
	}
	else
	{
		iRet = read_config_info(&sConfigAttrData.sConfigInfo);
		if (iRet < 0)
		{
			CONFIG_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_init]Call 'read_config_info()' fail.\r\n");
			return iRet;
		}

		if (sConfigAttrData.sConfigInfo.databasever != get_database_version())
		{
			sConfigAttrData.sConfigInfo.databasever = get_database_version();
			iRet = write_config_info_database_ver(&sConfigAttrData.sConfigInfo.databasever);
			if (iRet < 0)
			{
				CONFIG_DYN_PRINT(M_DBG_LEVEL_ERR, "[config_init]Call 'write_config_info_database_ver()' fail.\r\n");
				return iRet;
			}
			iModify = 2;
		}
	}

	return iModify;
}

//-	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -*
int	config_factory_new (void)
{
	uint8_t	aui8Buf[MUI_AT_PORT_CMD_LEN_MAX];
	int	iRet;

	iRet = snprintf((char*)aui8Buf,	sizeof(aui8Buf), AT_CMD_ATZ_S"\r\n");
	at_cmd_query(aui8Buf, iRet,	aui8Buf, sizeof(aui8Buf), 0);
	database_del_all();
	at_port_state_set(0);

	sys_reboot();

	return 0;
}

int	config_info_chk	(void)
{
	uint32_t ui32DatabaseVer;
	int	iRet;
#if	0
	S_ConfigAttrData sSysInfo;
	read_config_info(&sSysInfo);
#endif
	iRet = read_config_info_database_ver(&ui32DatabaseVer);
	if (iRet < 0)
	{
		if (iRet ==	FILE_S_NOFIND_ERR)
		{
			config_factory_new();
		}
		sys_reboot();
	}

	if (ui32DatabaseVer	!= get_database_version())
	{
		config_factory_new();
		sys_reboot();
	}

	return 0;
}

