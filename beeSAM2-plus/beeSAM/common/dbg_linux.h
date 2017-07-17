//#define	__M_FILE_NAME_dbg_linux_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_dbg_linux_H__
#define	__M_FILE_NAME_dbg_linux_H__

#include "common.h"


/*
调试信息等级是一个32bits的数据,将由如下方式组成
(ModuleSelect)28bits+(DebugLevel)4bits
高28位代表不同的模块选择,可任意配置,低4位代表16个等级,数值越大重要级别越高
*/
#define M_DBG_MODULE_MASK		0xFFFFFFF0
#define M_DBG_MODULE_ALL		0xFFFFFFF0
#define M_DBG_MODULE_OFF		0x00000000

#define M_DBG_LEVEL_MASK		0x0000000F
#define M_DBG_LEVEL_DEBUG		0x00000000
#define M_DBG_LEVEL_INFO		0x00000002
#define M_DBG_LEVEL_NOTICE		0x00000004
#define M_DBG_LEVEL_WARNING		0x00000006
#define M_DBG_LEVEL_ERR			0x00000008
#define M_DBG_LEVEL_CRIT		0x0000000A
#define M_DBG_LEVEL_ALERT		0x0000000C
#define M_DBG_LEVEL_EMERG		0x0000000E

#define M_DBG_MODULE_AT_PORT			0x00000010
#define M_DBG_MODULE_AT_PORT_LINUX		0x00000010
#define M_DBG_MODULE_ZIGBEE_MANAGE		0x00000020
#define M_DBG_MODULE_NET_LINUX			0x00000040
#define M_DBG_MODULE_REMOTE_NET_MANAGE	0x00000040

#define M_DBG_MODULE_DATABASE			0x00000080
#define M_DBG_MODULE_FILE_LINUX			0x00000100
#define M_DBG_MODULE_EXTEND_DATA		0x00000100
#define M_DBG_MODULE_CONFIG_DATA		0x00000100
#define M_DBG_MODULE_USER_DATA			0x00000100
#define M_DBG_MODULE_DEV_DATA			0x00000100

#define M_DBG_MODULE_API_DEAL			0x00000200
#define M_DBG_MODULE_JSON_DEAL			0x00000200

#define M_DBG_MODULE_DEV_IF				0x00000400

#define M_DBG_MODULE_LIGHT				0x00000800
#define M_DBG_MODULE_OCC_SENSOR			0x00001000
#define M_DBG_MODULE_BIN_SENSOR			0x00001000
#define M_DBG_MODULE_HUM_SENSOR			0x00002000
#define M_DBG_MODULE_TEMP_SENSOR		0x00002000
#define M_DBG_MODULE_BIN_CTRL			0x00004000
#define M_DBG_MODULE_LEVEL_CTRL			0x00004000
#define M_DBG_MODULE_WARN_DEV			0x00008000
#define M_DBG_MODULE_PLUG				0x00008000
#define M_DBG_MODULE_SWITCH				0x00010000
#define M_DBG_MODULE_SHADE				0x00010000
#define M_DBG_MODULE_SONOS				0x00020000

#define M_DBG_MODULE_CONFIG				0x00040000
#define M_DBG_MODULE_GROUP				0x00040000
#define M_DBG_MODULE_SET				0x00080000
#define M_DBG_MODULE_RULE				0x00100000
#define M_DBG_MODULE_SCHEDULE			0x00300000
#define M_DBG_MODULE_SCENE				0x00400000

#define M_DBG_MODULE_SONOS_DRV	        0x00200000
#define M_DBG_MODULE_SONOS_NET_LISTEN	0x00200000


#define M_DBG_MODULE_REPORT				0x00400000
#define M_DBG_MODULE_AUTO_REPORT		0x00400000
#define M_DBG_MODULE_LOCAL_REPORT		0x00400000
#define M_DBG_MODULE_REMOTE_REPORT		0x00400000

#define M_DBG_MODULE_SSDP				0x00800000
#define M_DBG_MODULE_MULTICAST			0x01000000
#define M_DBG_MODULE_NTP				0x02000000
#define M_DBG_MODULE_FTP_UPDATE			0x02000000

#define M_DBG_MODULE_EXTRA_DATA			0x04000000

#define M_DBG_MODULE_DEBUG_MODE 		0x08000000

#define M_DBG_MODULE_UZIP_UPGRADE		0x10000000

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_DBG_INFO
//===================================================================
#ifndef PUTC
#define PUTC(Char)			DBG_PutC(Char)
#endif
#ifndef PUTS
#define PUTS(Str, Len)		DBG_PutS(Str, Len)
#endif
#ifndef PUTASCC
#define PUTASCC(Char)		DBG_PutAsciiC(Char)
#endif
#ifndef PUTASCS
#define PUTASCS(Str, Len)	DBG_PutAsciiS(Str, Len)
#endif
#ifndef PRINT
#define PRINT(Fmt, Arg...)	\
		do	\
		{	\
			if (0 == dbg_info_mode_get())	\
			{	\
				printf(Fmt, ##Arg);	\
			}	\
			else	\
			{	\
				DBG_Print(Fmt, ##Arg);	\
			}	\
		} while (0)

#endif
//-------------------------------------------------------------------
#ifndef DYN_PUTC
#define DYN_PUTC(Dbg, Char)	\
		do	\
		{	\
			if ((0 != ((Dbg)&dbg_info_mask_get())) && (((Dbg)&(M_DBG_LEVEL_MASK)) >= dbg_info_level_get()))	\
			{	\
				PUTC(Char);	\
			}	\
		} while (0)
#endif
#ifndef DYN_PUTS
#define DYN_PUTS(Dbg, Str, Len)	\
		do	\
		{	\
			if ((0 != ((Dbg)&dbg_info_mask_get())) && (((Dbg)&(M_DBG_LEVEL_MASK)) >= dbg_info_level_get()))	\
			{	\
				PUTS(Str, Len);	\
			}	\
		} while (0)
#endif
#ifndef DYN_PUTASCC
#define DYN_PUTASCC(Dbg, Char)	\
		do	\
		{	\
			if ((0 != ((Dbg)&dbg_info_mask_get())) && (((Dbg)&(M_DBG_LEVEL_MASK)) >= dbg_info_level_get()))	\
			{	\
				PUTASCC(Char);	\
			}	\
		} while (0)
#endif
#ifndef DYN_PUTASCS
#define DYN_PUTASCS(Dbg, Str, Len)	\
		do	\
		{	\
			if ((0 != ((Dbg)&dbg_info_mask_get())) && (((Dbg)&(M_DBG_LEVEL_MASK)) >= dbg_info_level_get()))	\
			{	\
				PUTASCS(Str, Len);	\
			}	\
		} while (0)
#endif
#ifndef DYN_PRINT
#define DYN_PRINT(Dbg, Fmt, Arg...)	\
		do	\
		{	\
			if ((0 != ((Dbg)&dbg_info_mask_get())) && (((Dbg)&(M_DBG_LEVEL_MASK)) >= dbg_info_level_get()))	\
			{	\
				PRINT(Fmt, ##Arg);	\
			}	\
		} while (0)
#endif
//-------------------------------------------------------------------
#ifndef ASSERT
#define ASSERT(Expr)	\
		do	\
		{	\
			if (!(Expr))	\
			{	\
				PRINT("Assert failed *****************************************************************\r\n");	\
				PRINT("File: %s\nLine: %d\r\n", __FILE__, __LINE__);	\
				PRINT("Assert failed: %s\r\n", #Expr);	\
				abort();	\
			}	\
		} while (0)
/*
#define ASSERT(Expr)	\
		do	\
		{	\
			if (!(Expr))	\
			{	\
				while(1);	\
			}	\
		} while (0)
*/
//#define ASSERT(Expr)
#endif
//===================================================================
#else
//===================================================================
#ifndef PUTC
#define PUTC(Char)
#endif
#ifndef PUTS
#define PUTS(Str, Len)
#endif
#ifndef PUTASCC
#define PUTASCC(Char)
#endif
#ifndef PUTASCS
#define PUTASCS(Str, Len)
#endif
#ifndef PRINT
#define PRINT(Fmt, Arg...)
#endif
//-------------------------------------------------------------------
#ifndef DYN_PUTC
#define DYN_PUTC(Dbg, Char)
#endif
#ifndef DYN_PUTS
#define DYN_PUTS(Dbg, Str, Len)
#endif
#ifndef DYN_PUTASCC
#define DYN_PUTASCC(Dbg, Char)
#endif
#ifndef DYN_PUTASCS
#define DYN_PUTASCS(Dbg, Str, Len)
#endif
#ifndef DYN_PRINT
#define DYN_PRINT(Dbg, Fmt, Arg...)
#endif
//-------------------------------------------------------------------
#ifndef ASSERT
#define ASSERT(Expr)
#endif
//===================================================================
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_DBG_INFO
extern unsigned char dbg_info_mode_get (void);
extern int DBG_PutC (int Ch);
extern int DBG_PutS (const char *pchStr, int iLen);
extern int DBG_PutAsciiC (int Ch);
extern int DBG_PutAsciiS (const char *pchStr, int iLen);
extern int DBG_Print (const char *pchFmt, ...);
#else
#define dbg_info_mode_get()			(0)
#define DBG_PutC(Char)				(0)
#define DBG_PutS(Str, Len)			(0)
#define DBG_PutAsciiC(Char)			(0)
#define DBG_PutAsciiS(Str, Len)		(0)
#define DBG_Print(Fmt, Arg...)		(0)
#endif

extern unsigned int dbg_info_mask_get (void);
extern unsigned int dbg_info_mask_set (unsigned int uiMask);
extern unsigned int dbg_info_level_get (void);
extern unsigned int dbg_info_level_set (unsigned int uiLevel);

extern int dbg_sys_init (const char *pchDbgIp);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_dbg_linux_H__".
