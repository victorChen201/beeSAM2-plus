/*
 * config.h
 *
 *	Created	on:	Oct	11,	2013
 *		Author:	yangyuantao
 */
//#define	__M_FILE_NAME_config_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_config_H__
#define	__M_FILE_NAME_config_H__

#include "net_manage.h"
#include "user.h"

//-	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -*

//--------------------------------------------------------------
extern int read_config_info (S_ConfigInfo *psConfigInfo);
//................................................
extern int read_config_info_sn (S_FactorySn *psFactorySn);
extern int read_config_info_database_ver (uint32_t *pui32DatabaseVer);
extern int read_config_info_name (S_CfgName *psName);
extern int read_config_info_sw_ver (S_CfgSwVer *psSwVer);
extern int read_config_info_wifi_mac (S_StrMacAddr *psWifiMac);
extern int read_config_info_mac (S_StrMacAddr *psMac);
extern int read_config_info_ip_addr (S_StrIpAddr *psIpAddr);
extern int read_config_info_netmask (S_StrIpAddr *psNetmask);
extern int read_config_info_gw_ip_addr (S_StrIpAddr *psGwIpAddr);
extern int read_config_info_dhcp (uint8_t *pui8Dhcp);
extern int read_config_info_uzip_ver (S_CfgUzipVer *psUzipVer);
extern int read_config_info_uzip_mac (S_CfgUzipMac *psUzipMac);
extern int read_config_info_uzip_sn (S_CfgUzipSn *psUzipSn);
extern int read_config_info_utc (S_CfgUtc *psUtc);
extern int read_config_info_sw_update (S_CfgUzipVer *psSwUpdate);
extern int read_config_info_proxy_addr (S_CfgProxyAddr *psProxyAddr);
extern int read_config_info_proxy_port (uint16_t *pui16ProxyPort);
extern int read_config_info_portal_services (uint8_t *pui8PortalServices);
extern int read_config_info_dev_id (S_StrDevId *psDevId);
extern int read_config_info_passwd (S_StrPasswd *psPasswd);
extern int read_config_info_link_button (uint8_t *pui8LinkButton);
//uint32_t			whitelist;	//计数
//--------------------------------------------------------------
extern int write_config_info (const S_ConfigInfo *psConfigInfo);
//................................................
extern int write_config_info_sn (const S_FactorySn *psFactorySn);
extern int write_config_info_database_ver (const uint32_t *pui32DatabaseVer);
extern int write_config_info_name (const S_CfgName *psName);
extern int write_config_info_sw_ver (const S_CfgSwVer *psSwVer);
extern int write_config_info_wifi_mac (const S_StrMacAddr *psWifiMac);
extern int write_config_info_mac (const S_StrMacAddr *psMac);
extern int write_config_info_ip_addr (const S_StrIpAddr *psIpAddr);
extern int write_config_info_netmask (const S_StrIpAddr *psNetmask);
extern int write_config_info_gw_ip_addr (const S_StrIpAddr *psGwIpAddr);
extern int write_config_info_dhcp (const uint8_t *pui8Dhcp);
extern int write_config_info_uzip_ver (const S_CfgUzipVer *psUzipVer);
extern int write_config_info_uzip_mac (const S_CfgUzipMac *psUzipMac);
extern int write_config_info_uzip_sn (const S_CfgUzipSn *psUzipSn);
extern int write_config_info_utc (const S_CfgUtc *psUtc);
extern int write_config_info_sw_update (const S_CfgUzipVer *psSwUpdate);
extern int write_config_info_proxy_addr (const S_CfgProxyAddr *psProxyAddr);
extern int write_config_info_proxy_port (const uint16_t *pui16ProxyPort);
extern int write_config_info_portal_services (const uint8_t *pui8PortalServices);
extern int write_config_info_dev_id (const S_StrDevId *psDevId);
extern int write_config_info_passwd (const S_StrPasswd *psPasswd);
extern int write_config_info_link_button (const uint8_t *pui8LinkButton);

//-	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -*
extern int config_factory_new (void);
extern int config_info_chk (void);

//-	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -*

#endif	//end of "#ifndef __M_FILE_NAME_config_H__".
