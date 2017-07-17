#include "common.h"
#include "net_manage.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//#define COMMON_DBG

#ifndef COMMON_PUTC
#ifdef COMMON_DBG
#define COMMON_PUTC(Char)			PUTC(Char)
#else
#define COMMON_PUTC(Char)
#endif
#endif

#ifndef COMMON_PUTS
#ifdef COMMON_DBG
#define COMMON_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define COMMON_PUTS(Str, Len)
#endif
#endif

#ifndef COMMON_PUTASCC
#ifdef COMMON_DBG
#define COMMON_PUTASCC(Char)			PUTASCC(Char)
#else
#define COMMON_PUTASCC(Char)
#endif
#endif

#ifndef COMMON_PUTASCS
#ifdef COMMON_DBG
#define COMMON_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define COMMON_PUTASCS(Str, Len)
#endif
#endif

#ifndef COMMON_PRINT
#ifdef COMMON_DBG
#define COMMON_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define COMMON_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef COMMON_DYN_PUTC
#ifdef COMMON_DBG
#define COMMON_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_COMMON|Dbg), Char)
#else
#define COMMON_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef COMMON_DYN_PUTS
#ifdef COMMON_DBG
#define COMMON_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_COMMON|Dbg), Str, Len)
#else
#define COMMON_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef COMMON_DYN_PUTASCC
#ifdef COMMON_DBG
#define COMMON_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_COMMON|Dbg), Char)
#else
#define COMMON_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef COMMON_DYN_PUTASCS
#ifdef COMMON_DBG
#define COMMON_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_COMMON|Dbg), Str, Len)
#else
#define COMMON_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef COMMON_DYN_PRINT
#ifdef COMMON_DBG
#define COMMON_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_COMMON|Dbg), Fmt, ##Arg)
#else
#define COMMON_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static const uint16_t s_ui16CrcTab[256] = // 余式表(0x8811): x16 + x12 + x5 + 1
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac,	0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861,	0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79,	0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4,	0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2,	0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb,	0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3,	0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8,	0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2,	0x0ed1, 0x1ef0
};

static const uint32_t s_ui32CrcTab[256] = // 余式表(0x04c11db7): x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

static const char s_achSwVer[] = "v0.0.4_00";
static const uint32_t s_ui32DatabaseVer = 0x00010001;	//高16位为ROM版本,低16位为RAM版本.

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

//-------------------------------------------------------------------------------------------------*
/*	CRC16继续计算函数
;	(功能)基于x16 + x12 + x5 + 1余式表以查表法计算CRC码.
;	(参数)
;	(返回)
;	(注意)
*/
uint16_t crc16_cal_continue (const void *pvBuf, size_t tLen, uint16_t ui16Tmp)
{
	uint8_t *pui8Buf = (uint8_t*)pvBuf;

	ASSERT(pvBuf != NULL);

	while (tLen--)
	{
		ui16Tmp = (ui16Tmp<<8) ^ s_ui16CrcTab[((uint8_t)((ui16Tmp)>>8)) ^ *pui8Buf];
		pui8Buf++;
	}

	return (ui16Tmp);
}
//..............................................................................
/*	CRC16计算函数
;	(功能)基于x16 + x12 + x5 + 1余式表以查表法计算CRC码.
;	(参数)
;	(返回)
;	(注意)
*/
uint16_t crc16_cal (const void *pvBuf, size_t tLen)
{
	ASSERT(pvBuf != NULL);

	return crc16_cal_continue(pvBuf, tLen, 0);
}
//..............................................................................
/*	CRC32继续计算函数
;	(功能)基于x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1余式表以查表法计算CRC码.
;	(参数)
;	(返回)
;	(注意)
*/
uint32_t crc32_cal_continue (const void *pvBuf, size_t tLen, uint32_t ui32Tmp)
{
	uint8_t *pui8Buf = (uint8_t*)pvBuf;

	ASSERT(pvBuf != NULL);

	while (tLen--)
	{
		ui32Tmp = (ui32Tmp<<8) ^ s_ui32CrcTab[((uint8_t)((ui32Tmp)>>24)) ^ *pui8Buf];
		pui8Buf++;
	}

	return (ui32Tmp);
}
//..............................................................................
/*	CRC32计算函数
;	(功能)基于x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1余式表以查表法计算CRC码.
;	(参数)
;	(返回)
;	(注意)
*/
uint32_t crc32_cal (const void *pvBuf, size_t tLen)
{
	ASSERT(pvBuf != NULL);

	return crc32_cal_continue(pvBuf, tLen, 0);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	查找子字符串函数
;	(功能)Find the first occurrence of find in s, where the search is limited to the first slen
;		  characters of s.
;	(参数)
;	(返回)
;	(注意)
*/
char* strnstr (const char *pchStr, const char *pchFind, size_t tSLen)
{
	char ch, chS;
	size_t tLen;

	if ((ch = *pchFind++) != '\0')
	{
		tLen = strlen(pchFind);
		do
		{
			do
			{
				if (tSLen-- < 1 || (chS = *pchStr++) == '\0')
				{
					return (NULL);
				}
			} while (chS != ch);
			if (tLen > tSLen)
			{
				return (NULL);
			}
		} while (strncmp(pchStr, pchFind, tLen) != 0);
		pchStr--;
	}

	return ((char *)pchStr);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	内存搜索函数
;	(功能)在指定内存区域搜索特定的内存数据块
;	(参数)
;		pvBuf:		要搜索的缓存
;		tBufLen:	要搜索的缓存长度
;		pvSub:		要匹配的子缓存
;		tSubLen:	要匹配的子缓存长度
;	(返回)
;		void*:		搜索到的匹配缓存的起始地址
;	(注意)
*/
void* mem_search (const void *pvBuf, size_t tBufLen, const void *pvSub, size_t tSubLen)
{
	const uint8_t *pui8BufSearchEnd, *pui8SubEnd, *pui8BufTmp, *pui8BufCmp, *pui8SubTmp;

	ASSERT(pvBuf != NULL);
	ASSERT(pvSub != NULL);

	if ((0==tBufLen) || (0==tSubLen) || (tSubLen>tBufLen))
	{
		return NULL;
	}

	pui8BufSearchEnd = (uint8_t*)pvBuf + (tBufLen - tSubLen + 1);
	pui8SubEnd       = (uint8_t*)pvSub + tSubLen;
	pui8BufTmp       = (uint8_t*)pvBuf;
	while (pui8BufTmp < pui8BufSearchEnd)
	{
		pui8BufCmp = pui8BufTmp;
		pui8SubTmp = pvSub;
		while (pui8SubTmp < pui8SubEnd)
		{
			if (*pui8SubTmp != *pui8BufCmp)
			{
				break;
			}
			pui8SubTmp++;
			pui8BufCmp++;
		}
		if (pui8SubTmp >= pui8SubEnd)
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_search]Have verified sub from buf.\r\n");
			return (void*)pui8BufTmp;
		}
		pui8BufTmp++;
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	内存检测头部函数
;	(功能)检测内存块是否是包含指定头内存块.
;	(参数)
;		pvBuf:		要检测的内存块
;		tBufLen:	要检测的内存块的长度
;		pvHeader:	期望的头字内存块
;		tHeaderLen:	期望的头内存块长度
;		ptLen:		检测到期望头内存块的内存块的长度
;		ptLen:		检测到期望头内存块的长度(从字节到第一个\r\n)
;	(返回)
;		void*:		期望头内存块的指针(成功); NULL(失败)
;	(注意)
*/
void* mem_header_chk (const void *pvBuf, size_t tBufLen, const void *pvHeader, size_t tHeaderLen, size_t *ptLen)
{
	const uint8_t *pui8Search = pvBuf;
	const uint8_t *pui8Tmp;
	const uint8_t *pui8Finish;
	const uint8_t *pui8Finish1;

	ASSERT(pvBuf != NULL);
	ASSERT(pvHeader != NULL);

	if ((0==tBufLen) || (0==tHeaderLen) || (tHeaderLen>tBufLen))
	{
		return NULL;
	}

	COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_header_chk]String & Sub-string:\r\n");
	COMMON_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
	COMMON_PUTASCS((const char*)pvBuf, (int)tBufLen);
	COMMON_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
	COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	COMMON_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
	COMMON_PUTASCS((const char*)pvHeader, (int)tHeaderLen);
	COMMON_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
	COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	while (NULL != (pui8Tmp=mem_search(pui8Search, tBufLen, pvHeader, tHeaderLen)))
	{
		pui8Finish  = mem_search(pui8Tmp, tBufLen-(pui8Tmp-pui8Search), "\r\n", 2);
		pui8Finish1 = mem_search(pui8Tmp, tBufLen-(pui8Tmp-pui8Search), "\n\r", 2);
		if ((NULL == pui8Finish) && (NULL == pui8Finish1))
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_header_chk]Can't find 'FINISH' flag.\r\n");
			break;
		}
		else if ((NULL != pui8Finish) && (NULL == pui8Finish1))
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_header_chk]Have found 'FINISH' flag-0:'\\r\\n'.\r\n");
			pui8Finish += 2;
		}
		else if ((NULL == pui8Finish) && (NULL != pui8Finish1))
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_header_chk]Have found 'FINISH' flag-1:'\\n\\r'.\r\n");
			pui8Finish = pui8Finish1 + 2;
		}
		else
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_header_chk]Have found 'FINISH' flag-2:'\\x\\x'.\r\n");
			pui8Finish = (pui8Finish < pui8Finish1) ? (pui8Finish+2) : (pui8Finish1+2);
		}

		if ((pui8Tmp==pui8Search) || (*(const uint8_t*)(pui8Tmp-1)=='\n'))
		{
			*ptLen = pui8Finish - pui8Tmp;
			return (void*)pui8Tmp;
		}

		tBufLen -= (pui8Search - pui8Finish);
		pui8Search  = pui8Finish;
	}

	COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_header_chk]Can't find header.\r\n");

	return NULL;
}
//..............................................................................
/*	内存检测尾部函数
;	(功能)检测字符串是否是包含指定尾字符串.本函数可以支持通配符'*',表示任意字符.
;		  但目前仅支持一个'*'.
;	(参数)
;		pvBuf:		要检测的内存块
;		tBufLen:	要检测的内存块的长度
;		pvTail:		期望的尾字内存块
;		tTailLen:	期望的尾内存块长度
;		ptLen:		检测到期望尾内存块的内存块的长度
;	(返回)
;		void*:		期望尾内存块的指针(成功); NULL(失败)
;	(注意)
*/
void* mem_tail_chk (const void *pvBuf, size_t tBufLen, const void *pvTail, size_t tTailLen, size_t *ptLen)
{
	const uint8_t *pui8Tail = pvTail;
	size_t tLen, tI;

	ASSERT(pvBuf != NULL);
	ASSERT(pvTail != NULL);

	if ((0==tBufLen) || (0==tTailLen) || (tTailLen>tBufLen))
	{
		return NULL;
	}

	COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_tail_chk]String & Sub-string:\r\n");
	COMMON_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
	COMMON_PUTASCS((const char*)pvBuf, (int)tBufLen);
	COMMON_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
	COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	COMMON_DYN_PUTC(M_DBG_LEVEL_DEBUG, '[');
	COMMON_PUTASCS((const char*)pvTail, (int)tTailLen);
	COMMON_DYN_PUTC(M_DBG_LEVEL_DEBUG, ']');
	COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");

	for (tI=0; tI<tTailLen; tI++)
	{
		if (pui8Tail[tI] == '*')
		{
			break;
		}
	}

	if (tI >= tTailLen)
	{
		if (tTailLen > tBufLen)
		{
			return NULL;
		}

		if (0 == memcmp((uint8_t*)pvBuf+tBufLen-tTailLen, pui8Tail, tTailLen))
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_tail_chk]Have find tail-0.\r\n");
			*ptLen = tBufLen;
			return (void*)pvBuf;
		}
		else
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_tail_chk]Haven't find tail-0.\r\n");
			return NULL;
		}
	}
	else
	{
		if (tTailLen > (tBufLen+1))
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_tail_chk]Haven't find tail-1.\r\n");
			return NULL;
		}

		//匹配'*'之后的部分
		tLen = tTailLen - (tI + 1);
		if (tLen > 0)
		{
			if (0 != memcmp((uint8_t*)pvBuf+tBufLen-tLen, &pui8Tail[tI+1], tLen))
			{
				COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_tail_chk]Haven't find tail-2.\r\n");
				return NULL;
			}
		}
		//匹配'*'之前的部分
		if (tI > 0)
		{
			if (NULL != mem_search((uint8_t*)pvBuf, tBufLen-tLen, pui8Tail, tI))
			{
				COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_tail_chk]Have find tail-1.\r\n");
				*ptLen = tBufLen;
				return (void*)pvBuf;
			}
			else
			{
				COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_tail_chk]Haven't find tail-3.\r\n");
				return NULL;
			}
		}

		COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[mem_tail_chk]Have find tail-2.\r\n");
		*ptLen = tBufLen;
		return (void*)pvBuf;
	}
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	获取子字符串直到\r\n函数
;	(功能)从指定字符串中截取子字符串,直到碰到\r\n.
;	(参数)
;		pchStr:		要截取子字符串的源字符串
;		pchSubStr:	截取的子字符串的回写地址
;		tStrLen:	源字符串的长度
;	(返回)
;		int:		<0  :出错(包括异常及等待超时)
;					其它:子字符串的长度
;	(注意)
*/
int sub_str_get_util_rn (const char *pchStr, char *pchSubStr, size_t tStrLen)
{
	char *pchRN = NULL;
	int iLen;

	ASSERT(pchStr != NULL);
	ASSERT(pchSubStr != NULL);
	ASSERT(tStrLen > 0);

	UNUSED(tStrLen);

	//COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "sub_str_get_util_rn: #####################################\r\n");
	//COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "sub_str_get_util_rn: String is\r\n");
	//COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%s\r\n", pchStr);
	//COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "sub_str_get_util_rn: #####################################\r\n");

	pchRN = strstr(pchStr, "\r\n");
	if (pchRN == NULL)
	{
		return 0;
	}

	iLen = ((size_t)pchRN) - ((size_t)pchStr) + 2;
	//COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "sub_str_get_util_rn: Will copy %d bytes to SubStr.\r\n", iLen);
	memcpy(pchSubStr, pchStr, iLen);

	return iLen;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	获取软件版本号函数
;	(功能)获取本软件的版本号
;	(参数)
;		pchVer:		返回版本号的指针
;	(返回)
;		const char*
;	(注意)
*/
const char* get_sw_version (char *pchVer, size_t tLen)
{
	if (NULL!=pchVer && sizeof(s_achSwVer)<=tLen)
	{
		strcpy(pchVer, s_achSwVer);
		return pchVer;
	}

	return s_achSwVer;
}
//..............................................................................
/*	获取数据库版本号函数
;	(功能)获取数据库的版本号
;	(参数)
;		NULL
;	(返回)
;		uint32_t	数据库的版本号
;	(注意)
*/
uint32_t get_database_version (void)
{
	return s_ui32DatabaseVer;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	等待信号量指定毫秒函数
;	(功能)
;	(参数)
;		ptSem:		信号量指针
;		ui32WaitMs:	毫秒数
;	(返回)
;		int
;			0:		成功
;			-1:		错误或超时(errno指明错误号)
;	(注意)
*/
int sem_wait_ms (sem_t *ptSem, uint32_t ui32WaitMs)
{
	struct timespec sTime;

	clock_gettime(CLOCK_REALTIME, &sTime);
	sTime.tv_sec  += ui32WaitMs/1000;
	ui32WaitMs     = (ui32WaitMs%1000)*1000*1000;
	ui32WaitMs    += sTime.tv_nsec;
	sTime.tv_sec  += ui32WaitMs/(1000*1000*1000);
	sTime.tv_nsec  = ui32WaitMs%(1000*1000*1000);

	return sem_timedwait(ptSem, &sTime);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	获取(已连接Socket)对端的MAC地址函数
;	(功能)
;	(参数)
;		iSocketFd:	Socket句柄
;		pchBuf:		回写MAC地址的指针
;		tSize:		回写缓存的小大
;	(返回)
;		int
;			0:		成功
;			-1:		错误或超时(errno指明错误号)
;	(注意)
*/
int get_peer_mac (int iSocketFd, char* pchBuf, size_t tSize)
{
	struct arpreq sArpReq;
	struct sockaddr_in sSocketAddr;
	socklen_t tLen = sizeof(struct sockaddr_in);
	unsigned char* pch;
	int iRet =0;

	memset(&sArpReq, 0, sizeof(sArpReq));
	memset(&sSocketAddr, 0, sizeof(sSocketAddr));

	if (getpeername(iSocketFd, (struct sockaddr*)&sSocketAddr, &tLen) < 0)
	{
		COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[get_peer_mac]: Call 'getpeername' error:%d.\r\n", errno);
		return -1;
	}
	else
	{
		memcpy(&sArpReq.arp_pa, &sSocketAddr, sizeof(sArpReq.arp_pa));
		strcpy(sArpReq.arp_dev, MSTR_IF_NAME);
		sArpReq.arp_pa.sa_family = AF_INET;
		sArpReq.arp_ha.sa_family = AF_UNSPEC;
		if (ioctl(iSocketFd, SIOCGARP, &sArpReq) < 0)
		{
			COMMON_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[get_peer_mac]: Call 'ioctl' error:%d.\r\n", errno);
			return -1;
		}
		else
		{
			pch = (unsigned char*)sArpReq.arp_ha.sa_data;
			iRet = snprintf(pchBuf,	tSize, "%02x%02x%02x%02x%02x%02x", *pch, *(pch+1), *(pch+2), *(pch+3), *(pch+4), *(pch+5));
		}
	}

	return iRet;
}
//-------------------------------------------------------------------------------------------------*

int sys_reboot (void)
{
	system("sync");	//进行文件保存
	system("reboot -f");	//进行文件保存

	return 0;
}

