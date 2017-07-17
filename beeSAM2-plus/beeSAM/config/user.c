/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of user module for beeSAM project.
File name:user.c
Description：

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "user.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define USER_DBG

#ifndef USER_PUTC
#ifdef USER_DBG
#define USER_PUTC(Char)			PUTC(Char)
#else
#define USER_PUTC(Char)
#endif
#endif

#ifndef USER_PUTS
#ifdef USER_DBG
#define USER_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define USER_PUTS(Str, Len)
#endif
#endif

#ifndef USER_PUTASCC
#ifdef USER_DBG
#define USER_PUTASCC(Char)			PUTASCC(Char)
#else
#define USER_PUTASCC(Char)
#endif
#endif

#ifndef USER_PUTASCS
#ifdef USER_DBG
#define USER_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define USER_PUTASCS(Str, Len)
#endif
#endif

#ifndef USER_PRINT
#ifdef USER_DBG
#define USER_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define USER_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef USER_DYN_PUTC
#ifdef USER_DBG
#define USER_DYN_PUTC(Dbg, Char)		DYN_PUTC((M_DBG_MODULE_USER|Dbg), Char)
#else
#define USER_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef USER_DYN_PUTS
#ifdef USER_DBG
#define USER_DYN_PUTS(Dbg, Str, Len)	DYN_PUTS((M_DBG_MODULE_USER|Dbg), Str, Len)
#else
#define USER_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef USER_DYN_PUTASCC
#ifdef USER_DBG
#define USER_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_USER|Dbg), Char)
#else
#define USER_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef USER_DYN_PUTASCS
#ifdef USER_DBG
#define USER_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_USER|Dbg), Str, Len)
#else
#define USER_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef USER_DYN_PRINT
#ifdef USER_DBG
#define USER_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_USER|Dbg), Fmt, ##Arg)
#else
#define USER_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define	MSTR_USR_SUPPER_USER_NAME	"IntercelSupperUser-beeSAM"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//-------------------------------------------------------------------------------------------------*
/*	用户总数获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:			成功找到,返回用户总数
;	(注意)
*/
int user_total_get (void)
{
	return user_data_total_get();
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	用户总数获取函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:	成功找到,返回数据长度
;	(注意)
*/
int	user_info_get (uint16_t ui16Idx, S_UserData *psUserData)
{
	return user_data_get_by_idx(ui16Idx, psUserData);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	用户验证函数
;	(功能)
;	(参数)
;		pchUserName:	要找的用户名
;		tLen:			要找的用户名长度
;	(返回)
;		int:			成功找到,返回实际偏移量(以S_User单元计数)
;						异常返回USER_ERR_NO_FIND
;	(注意)
*/
int user_verify (const char *pchUserName, size_t tLen)
{
	if (((sizeof(MSTR_USR_SUPPER_USER_NAME)-1)==tLen) &&
		(0==memcmp(pchUserName, MSTR_USR_SUPPER_USER_NAME, (sizeof(MSTR_USR_SUPPER_USER_NAME)-1))))
	{
		return MUI_USER_NUM_MAX;
	}

	return user_data_verify(pchUserName, tLen);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	用户增加函数
;	(功能)
;	(参数)
;		psUserData:	要增加的用户的信息(结构体的时间信息不用填)
;	(返回)
;		int:		成功加入,返回在用户名在系统中的索引编号
;	(注意)
*/
int user_add (const S_UserData *psUserData)
{
	return user_data_add(psUserData->name, strlen(psUserData->name), psUserData->dev_name, strlen(psUserData->dev_name));
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	用户删除函数
;	(功能)
;	(参数)
;		psUserData:	要删除的用户的信息(结构体的时间信息不用填)
;	(返回)
;		int:		成功删除,返回在用户名原本在系统中的索引编号
;	(注意)
*/
int user_del (const S_UserData *psUserData)
{
	return user_data_del(psUserData->name, strlen(psUserData->name));
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	所有用户删除函数
;	(功能)
;	(参数)
;		NULL
;	(返回)
;		int:		成功删除,返回在用户名原本在系统中的索引编号
;	(注意)
*/
int user_del_all (void)
{
	return user_data_del_all();
}
//-------------------------------------------------------------------------------------------------*
