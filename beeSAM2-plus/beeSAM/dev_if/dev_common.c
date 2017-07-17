#include "data_common.h"
#include "dev_common.h"
#include "dev_if.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define DEV_COMMON_DBG

#ifndef DEV_COMMON_PUTC
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_PUTC(Char)			PUTC(Char)
#else
#define DEV_COMMON_PUTC(Char)
#endif
#endif

#ifndef DEV_COMMON_PUTS
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define DEV_COMMON_PUTS(Str, Len)
#endif
#endif

#ifndef DEV_COMMON_PUTASCC
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_PUTASCC(Char)			PUTASCC(Char)
#else
#define DEV_COMMON_PUTASCC(Char)
#endif
#endif

#ifndef DEV_COMMON_PUTASCS
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define DEV_COMMON_PUTASCS(Str, Len)
#endif
#endif

#ifndef DEV_COMMON_PRINT
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define DEV_COMMON_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef DEV_COMMON_DYN_PUTC
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_DEV_COMMON|Dbg), Char)
#else
#define DEV_COMMON_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef DEV_COMMON_DYN_PUTS
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_DEV_COMMON|Dbg), Str, Len)
#else
#define DEV_COMMON_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef DEV_COMMON_DYN_PUTASCC
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_DEV_COMMON|Dbg), Char)
#else
#define DEV_COMMON_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef DEV_COMMON_DYN_PUTASCS
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_DEV_COMMON|Dbg), Str, Len)
#else
#define DEV_COMMON_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef DEV_COMMON_DYN_PRINT
#ifdef DEV_COMMON_DBG
#define DEV_COMMON_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_DEV_COMMON|Dbg), Fmt, ##Arg)
#else
#define DEV_COMMON_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

