#ifndef __M_FILE_UZIPUPGRADE_XMOEDM_H__
#define __M_FILE_UZIPUPGRADE_XMOEDM_H__

//-----------------------------------------------------------------------------------------------*
#include "common.h"
#include "sys_time_linux.h"
#include "dbg_linux.h"
#include "zigbee_manage.h"
#include "at_port_linux.h"
#include "database.h"
#include "at_port_linux.h"
#include "config.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

sem_t s_tUzipupdataSem;//Uzip Éý¼¶ÐÅºÅÁ¿

//-----------------------------------------------------------------------------------------------*
extern int  upgrade_destination_device(char *dest, char *file,char *address);
extern int  uzip_xmodem_upgrade (char uzip_xmoden_state);
extern void Uzip_xmodem_init_DataInfo();
extern void read_Device_Upgrade_filepath(char *file);
extern void find_device_by_device_type(uint16_t ui16DevType,char *device);
extern void find_upgrade_file_by_upgrade_device_type(uint16_t ui16DevType,char *deviceupgradefile);
#endif
