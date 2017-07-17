//#define	__M_FILE_NAME_user_H__	//屏蔽本头文件
/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of user module for beeSAM project.
File name:user.h
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#ifndef	__M_FILE_NAME_user_H__
#define	__M_FILE_NAME_user_H__

#include "common.h"
#include "data_common.h"
#include "user_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int user_total_get (void);
extern int user_info_get (uint16_t ui16Id, S_UserData *psUserData);
extern int user_verify (const char *pchUserName, size_t tLen);
extern int user_add (const S_UserData *psUserData);
extern int user_del (const S_UserData *psUserData);
extern int user_del_all (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_user_H__".

