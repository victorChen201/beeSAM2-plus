#include "at_port.h"
#include "api_deal.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//#define AT_PORT_DBG

#ifndef AT_PORT_PUTC
#ifdef AT_PORT_DBG
#define AT_PORT_PUTC(Char)			PUTC(Char)
#else
#define AT_PORT_PUTC(Char)
#endif
#endif

#ifndef AT_PORT_PUTS
#ifdef AT_PORT_DBG
#define AT_PORT_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define AT_PORT_PUTS(Str, Len)
#endif
#endif

#ifndef AT_PORT_PUTASCC
#ifdef AT_PORT_DBG
#define AT_PORT_PUTASCC(Char)		PUTASCC(Char)
#else
#define AT_PORT_PUTASCC(Char)
#endif
#endif

#ifndef AT_PORT_PUTASCS
#ifdef AT_PORT_DBG
#define AT_PORT_PUTASCS(Str, Len)	PUTASCS(Str, Len)
#else
#define AT_PORT_PUTASCS(Str, Len)
#endif
#endif

#ifndef AT_PORT_PRINT
#ifdef AT_PORT_DBG
#define AT_PORT_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define AT_PORT_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef AT_PORT_DYN_PUTS
#ifdef AT_PORT_DBG
#define AT_PORT_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_AT_PORT|Dbg), Str, Len)
#else
#define AT_PORT_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef AT_PORT_DYN_PUTASCC
#ifdef AT_PORT_DBG
#define AT_PORT_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_AT_PORT|Dbg), Char)
#else
#define AT_PORT_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef AT_PORT_DYN_PUTASCS
#ifdef AT_PORT_DBG
#define AT_PORT_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_AT_PORT|Dbg), Str, Len)
#else
#define AT_PORT_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef AT_PORT_DYN_PRINT
#ifdef AT_PORT_DBG
#define AT_PORT_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_AT_PORT|Dbg), Fmt, ##Arg)
#else
#define AT_PORT_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*


