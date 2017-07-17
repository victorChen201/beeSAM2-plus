/*------------------------------------------------------------------------------
Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
All rights reserved.

Source file of dev_data module for beeSAM project.
File name:dev_data.c
Description��

-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
Author    : Chandler
Version   : 1.0
Date      : 2014/04/01
Time      : 14:43

-------------------------------------------------------------------------------*/
#include "dev_data.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define DEV_DATA_DBG

#ifndef DEV_DATA_PUTC
#ifdef DEV_DATA_DBG
#define DEV_DATA_PUTC(Char)			PUTC(Char)
#else
#define DEV_DATA_PUTC(Char)
#endif
#endif

#ifndef DEV_DATA_PUTS
#ifdef DEV_DATA_DBG
#define DEV_DATA_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define DEV_DATA_PUTS(Str, Len)
#endif
#endif

#ifndef DEV_DATA_PUTASCC
#ifdef DEV_DATA_DBG
#define DEV_DATA_PUTASCC(Char)			PUTASCC(Char)
#else
#define DEV_DATA_PUTASCC(Char)
#endif
#endif

#ifndef DEV_DATA_PUTASCS
#ifdef DEV_DATA_DBG
#define DEV_DATA_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define DEV_DATA_PUTASCS(Str, Len)
#endif
#endif

#ifndef DEV_DATA_PRINT
#ifdef DEV_DATA_DBG
#define DEV_DATA_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define DEV_DATA_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef DEV_DATA_DYN_PUTC
#ifdef DEV_DATA_DBG
#define DEV_DATA_DYN_PUTC(Dbg, Char)		DYN_PUTC((M_DBG_MODULE_DEV_DATA|Dbg), Char)
#else
#define DEV_DATA_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef DEV_DATA_DYN_PUTS
#ifdef DEV_DATA_DBG
#define DEV_DATA_DYN_PUTS(Dbg, Str, Len)	DYN_PUTS((M_DBG_MODULE_DEV_DATA|Dbg), Str, Len)
#else
#define DEV_DATA_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef DEV_DATA_DYN_PUTASCC
#ifdef DEV_DATA_DBG
#define DEV_DATA_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_DEV_DATA|Dbg), Char)
#else
#define DEV_DATA_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef DEV_DATA_DYN_PUTASCS
#ifdef DEV_DATA_DBG
#define DEV_DATA_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_DEV_DATA|Dbg), Str, Len)
#else
#define DEV_DATA_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef DEV_DATA_DYN_PRINT
#ifdef DEV_DATA_DBG
#define DEV_DATA_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_DEV_DATA|Dbg), Fmt, ##Arg)
#else
#define DEV_DATA_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MI_DEV_RAM_SEARCH_NEW_OFFSET	(-50000)
#define MUI_DEV_RAM_WAIT_TIME_MS_MAX	3000
#define MUI_DEV_ROM_WAIT_TIME_MS_MAX	3000

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef struct
{
	uint32_t			tDevIdx;		// Type(High 16Bits)+ID(Low 16Bits)
	U_DevStateSave		uDevState;
} S_DevStateRecord;

typedef struct
{
	uint16_t			ui16DevTotal;		//�豸����
						//ע��,����aui16DevTypeIdx,��ʵ�ʵ����������һ��,���һ������ָ��asRecordʵ�����ݵ�β��+1
	uint16_t			aui16DevTypeIdx[MUI_DEV_TYPE_NUM+1];	//ÿ���豸������ʼƫ����(��S_DevStateRecordΪ��λ)
	S_DevStateRecord	asRecord[MUI_LOGIC_DEV_NUM_MAX];
} S_DevStateData;

typedef struct
{
	S_DevStateData		sDevStateData;
	uint32_t			ui32Crc;
} S_DevStateDataBackup;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static S_DevStateData s_sDevStateData;
static sem_t s_tDevRamMutex, s_tDevRomMutex;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static void s_dev_data_ram_show (void);
static int s_dev_data_total_get (void);
static int s_dev_data_idx_list_get (uint32_t *pui32Buf, uint16_t ui16Capacity);
static int s_dev_data_num_get_by_type (uint16_t ui16Type);
static int s_dev_data_id_list_get_by_type (uint16_t ui16Type, uint16_t *pui16Buf, uint16_t ui16Capacity);
static int s_dev_ram_search (uint16_t ui16Type, uint16_t ui16Id, uint16_t ui16Start, uint16_t ui16End);
static int s_dev_ram_insert_by_type_offset (uint16_t ui16Type, uint16_t ui16Offset, uint16_t ui16Id, const void *pvRamBuf, size_t tRamSize);
static int s_dev_ram_remove_by_type_offset (uint16_t ui16Type, uint16_t ui16Offset);
static int s_dev_data_assign_id_by_type (uint16_t ui16Type);
static int s_dev_rom_data_del (uint16_t ui16Type, uint16_t ui16Id);
static int s_dev_rom_data_del_by_type (uint16_t ui16Type);
static int s_dev_rom_dir_del_by_type (uint16_t ui16Type);
static int s_dev_data_add (uint16_t ui16Type, const void *pvRamBuf, size_t tRamSize, const void *pvRomBuf, size_t tRomSize);
static int s_dev_data_del (uint16_t ui16Type, uint16_t ui16Id);
static int s_dev_data_del_by_type (uint16_t ui16Type);
static int s_dev_data_del_all (void);
static int s_dev_data_swap_id (uint16_t ui16Type, uint16_t ui16DestId, uint16_t ui16SrcId);
static int s_dev_data_set_new_id (uint16_t ui16Type, uint16_t ui16NewId, uint16_t ui16Id);
static int s_dev_data_state_checkout (S_DevStateData *psDevStateData);
static int s_dev_data_state_backup (const S_DevStateData *psDevStateData, uint32_t ui32Crc);
static int s_dev_data_state_sync (const S_DevStateData *psDevStateData);
static int s_dev_data_state_del (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int init_dev_ram_data (uint16_t ui16DevType, uint16_t ui16Id, U_DevStateSave *puState);

//-------------------------------------------------------------------------------------------------*
/*	�豸RAM(���ݱ�)��ʾ����
;	(����)
;	(����)
;		NULL
;	(����)
;		NULL
;	(ע��)
*/
static void s_dev_data_ram_show (void)
{
#if 0
	int i;

	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_ram_show]s_sDevStateData.ui16DevTotal=%u\r\n", s_sDevStateData.ui16DevTotal);
	for (i=0; i<(int)(sizeof(s_sDevStateData.aui16DevTypeIdx)/sizeof(s_sDevStateData.aui16DevTypeIdx[0])); i++)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[%d]%u ", i, s_sDevStateData.aui16DevTypeIdx[i]);
	}
	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	for (i=0; i<s_sDevStateData.ui16DevTotal; i++)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "%d:%08x\r\n", i, s_sDevStateData.asRecord[i].tDevIdx);
	}
	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_ram_show]====================END====================\r\n");
#endif
}
//-------------------------------------------------------------------------------------------------*

static int s_dev_data_total_get (void)
{
	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_total_get]Total device: %u\r\n", s_sDevStateData.ui16DevTotal);
	return s_sDevStateData.ui16DevTotal;
}

static int s_dev_data_idx_list_get (uint32_t *pui32Buf, uint16_t ui16Capacity)
{
	uint16_t ui16i;

	ASSERT(NULL != pui32Buf);

	if (ui16Capacity < s_sDevStateData.ui16DevTotal)
	{
		return FILE_S_BUF_ERR;
	}
	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_idx_list_get]Index list of all device.\r\n");
	for (ui16i=0; ui16i<s_sDevStateData.ui16DevTotal; ui16i++, pui32Buf++)
	{
		*pui32Buf = s_sDevStateData.asRecord[ui16i].tDevIdx;
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_idx_list_get]%08x\r\n", *pui32Buf);
	}

	return (int)s_sDevStateData.ui16DevTotal;
}

static int s_dev_data_num_get_by_type (uint16_t ui16Type)
{
	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_num_get_by_type]There are total %u devices of type: %04hx.\r\n", (s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]-s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START]), ui16Type);
	return (int)(s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START] - s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START]);
}

static int s_dev_data_id_list_get_by_type (uint16_t ui16Type, uint16_t *pui16Buf, uint16_t ui16Capacity)
{
	uint16_t ui16i, ui16End;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(NULL != pui16Buf);

	ui16End = s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START];
	ui16i   = s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START];
	if (ui16Capacity < (ui16End-ui16i))
	{
		return FILE_S_BUF_ERR;
	}
	ui16Capacity = ui16End - ui16i;
	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_id_list_get_by_type]ID list of device type %04hx:\r\n", ui16Type);
	while (ui16i < ui16End)
	{
		*pui16Buf = M_GET_ID_FROM_IDX(s_sDevStateData.asRecord[ui16i].tDevIdx);
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_id_list_get_by_type]%04hx\r\n", *pui16Buf);
		ui16i++;
		pui16Buf++;
	}

	return (int)ui16Capacity;
}

//-------------------------------------------------------------------------------------------------*
/*	�豸RAM(���ݱ�)��������
;	(����)
;	(����)
;		ui16Type:	��Ҫ��Ѱ���豸����
;		ui16Id:		��Ҫ��Ѱ���豸ID
;		ui16Start:	������Χ����ʼ����
;		ui16End:	������Χ�Ľ�������(��ui16Start����ui16End,����Ϊ������ΧΪ��,�����������˵,����
;					��Ϊui16Endָ�������Ӧ��ʵ��������Χ��1)
;	(����)
;		int:		�ɹ��ҵ�,����ʵ��ƫ����
;					���δ�ҵ�,�򷵻ؿɲ���ƫ��λ��+MI_DEV_RAM_SEARCH_NEW_OFFSET
;	(ע��)
*/
static int s_dev_ram_search (uint16_t ui16Type, uint16_t ui16Id, uint16_t ui16Start, uint16_t ui16End)
{
	uint32_t ui32DestIdx;
	uint16_t ui16M, ui16MOld;
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT((MUI_DEV_ID_MIN<=ui16Id) && (ui16Id<=MUI_DEV_ID_MAX));
	ASSERT(ui16Start <= ui16End);	//Start<=End
	ASSERT(MUI_LOGIC_DEV_NUM_MAX >= ui16End);	//��֤�豸������32K����

	//DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_ram_search]Will search %04hx:%04hx from [%u] to [%u].\r\n", ui16Type, ui16Id, ui16Start, ui16End);
	//s_dev_data_ram_show();

	ui32DestIdx = M_GEN_IDX_BY_TYPE_ID(ui16Type, ui16Id);
	ui16MOld = ui16End;
	iRet  = -1;		//��ui16Start==ui16End,ֱ�ӷ���MI_DEV_RAM_SEARCH_NEW_OFFSET+(int)ui16Start

	while (ui16MOld != (ui16M=((ui16Start+ui16End)>>1)))
	{
		ui16MOld = ui16M;
		iRet  = data_idx_cmp(&ui32DestIdx, &s_sDevStateData.asRecord[ui16M].tDevIdx);
		if (iRet > 0)
		{
			ui16Start = ui16M;
		}
		else if (iRet < 0)
		{
			ui16End = ui16M;
		}
		else
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_ram_search]Have find %04hx:%04hx on [%u].\r\n", ui16Type, ui16Id, ui16M);
			return (int)ui16M;
		}
	}
#if 1
	if (iRet > 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_ram_search]----------------------------------------------------------\r\n");
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_ram_search]Can insert %04hx:%04hx to [%u].\r\n", ui16Type, ui16Id, ui16M+1);
		return (MI_DEV_RAM_SEARCH_NEW_OFFSET + 1 + (int)ui16M);
	}
	else
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_ram_search]Can insert %04hx:%04hx to [%u].\r\n", ui16Type, ui16Id, ui16M);
		return (MI_DEV_RAM_SEARCH_NEW_OFFSET + (int)ui16M);
	}
#else
	return FILE_S_NOFIND_ERR;
#endif
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	�豸RAM(���ݱ�)��������ƫ�������뺯��
;	(����)
;	(����)
;		ui16Type:	��Ҫɾ�����豸����
;		ui16Offset:	��Ҫɾ���豸���ݵ�ƫ����(asRecord�е�ƫ��)
;		ui16Id:		��Ҫ�������ݵ�ID
;		pvRamBuf:	��Ҫ�������ݵ�ָ��
;		tRamSize:	��Ҫ�������ݵĳߴ�
;	(����)
;		int:		�ɹ�ɾ��,����0
;					���ʧ��,�򷵻ظ���
;	(ע��)
*/
static int s_dev_ram_insert_by_type_offset (uint16_t ui16Type, uint16_t ui16Offset, uint16_t ui16Id, const void *pvRamBuf, size_t tRamSize)
{
	uint16_t ui16i;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT((s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START]<=ui16Offset) && (ui16Offset<=s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]));
	ASSERT((MUI_DEV_ID_MIN<=ui16Id) && (ui16Id<=MUI_DEV_ID_MAX));
	ASSERT(NULL != pvRamBuf);
	ASSERT(tRamSize <= sizeof(U_DevStateSave));	//���ܳ������ݵ����߽�

	//��1��:���S_DevStateRecord�����ƶ�,֮��������������Ϊ�˼����ڴ��ʹ��
	for (ui16i=s_sDevStateData.ui16DevTotal; ui16i>ui16Offset; ui16i--)
	{
		memcpy(&s_sDevStateData.asRecord[ui16i], &s_sDevStateData.asRecord[ui16i-1], sizeof(s_sDevStateData.asRecord[ui16i]));
	}
	//��2��:����������
	s_sDevStateData.asRecord[ui16i].tDevIdx = M_GEN_IDX_BY_TYPE_ID(ui16Type, ui16Id);
	memcpy(&s_sDevStateData.asRecord[ui16i].uDevState, pvRamBuf, tRamSize);

	//��3��:����s_sDevStateData.aui16DevTypeIdx[]
	for (ui16i=ui16Type+1-MUI_DATA_TYPE_DEV_START; ui16i<(sizeof(s_sDevStateData.aui16DevTypeIdx)/sizeof(s_sDevStateData.aui16DevTypeIdx[0])); ui16i++)
	{
		s_sDevStateData.aui16DevTypeIdx[ui16i]++;
	}
	//��4��:����s_sDevStateData.ui16DevTotal
	s_sDevStateData.ui16DevTotal++;

	return sizeof(S_DevStateRecord);
}
//..............................................................................
/*	�豸RAM(���ݱ�)��������ƫ����ɾ������
;	(����)
;	(����)
;		ui16Type:	��Ҫɾ�����豸����
;		ui16Offset:	��Ҫɾ���豸���ݵ�ƫ����(asRecord�е�ƫ��)
;	(����)
;		int:		�ɹ�ɾ��,����0
;					���ʧ��,�򷵻ظ���
;	(ע��)
*/
static int s_dev_ram_remove_by_type_offset (uint16_t ui16Type, uint16_t ui16Offset)
{
	uint16_t ui16i;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT((s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START]<=ui16Offset) && (ui16Offset<s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]));

	//��1��:���S_DevStateRecord��ǰ�ƶ�,֮��������������Ϊ�˼����ڴ��ʹ��
	for (ui16i=ui16Offset; ui16i<(s_sDevStateData.ui16DevTotal-1); ui16i++)
	{
		memcpy(&s_sDevStateData.asRecord[ui16i], &s_sDevStateData.asRecord[ui16i+1], sizeof(s_sDevStateData.asRecord[ui16i]));
	}
	//��2��:����s_sDevStateData.aui16DevTypeIdx[]
	for (ui16i=ui16Type+1-MUI_DATA_TYPE_DEV_START; ui16i<(sizeof(s_sDevStateData.aui16DevTypeIdx)/sizeof(s_sDevStateData.aui16DevTypeIdx[0])); ui16i++)
	{
		s_sDevStateData.aui16DevTypeIdx[ui16i]--;
	}
	//��3��:����s_sDevStateData.ui16DevTotal
	s_sDevStateData.ui16DevTotal--;

	return 0;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
/*	�������ͷ�����ID����
;	(����)
;	(����)
;		ui16Type:	��Ҫ������ID���豸����
;	(����)
;		int:		�ɹ����䵽,����(RAMλ��ƫ����[��16Bits]+��ID[��16Bits])
;					���δ����,�򷵻ظ���
;	(ע��)			�˴������������ݱ��뱣֤����С��0x8000,����֤�豸������32K����
*/
static int s_dev_data_assign_id_by_type (uint16_t ui16Type)
{
	uint16_t ui16Id, ui16Cmp, ui16Dest;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	//�豸���������ж�
	if (s_sDevStateData.ui16DevTotal >= MUI_LOGIC_DEV_NUM_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[s_dev_data_assign_id_by_type]Can't assigned id for new device, device total is %u\r\n", s_sDevStateData.ui16DevTotal);
		return FILE_S_ERR_DEV_NUM_ULTRALIMIT;
	}
	//�����ͽ����豸�������ж�
	ui16Dest = (uint16_t)s_dev_data_num_get_by_type(ui16Type);
	if ((uint16_t)data_num_max_get_by_type(ui16Type) <= ui16Dest)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[s_dev_data_assign_id_by_type]Can't assigned id for new device, type %04hx device number is %d\r\n", ui16Type, ui16Dest);
		return FILE_S_ERR_DEV_NUM_ULTRALIMIT;
	}

	ui16Id	 = MUI_DEV_ID_MIN;
	ui16Cmp	 = s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START];
	ui16Dest = s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START];
	while (ui16Cmp < ui16Dest)
	{
		if (ui16Id < M_GET_ID_FROM_IDX(s_sDevStateData.asRecord[ui16Cmp].tDevIdx))
		{
			break;
		}
		ui16Id++;
		ui16Cmp++;
	}

	return (int)((((uint32_t)ui16Cmp)<<16) | ((uint32_t)ui16Id));
}
//-------------------------------------------------------------------------------------------------*

static int s_dev_rom_data_del (uint16_t ui16Type, uint16_t ui16Id)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	//ɾ��ROM�е�����
	if (NULL == data_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_rom_data_del]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_del(achPath);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_rom_data_del]Can't delete device data-%s from ROM.\r\n", achPath);
		return iRet;
	}

	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_rom_data_del]Have deleted device %04hx:%04hx.\r\n", ui16Type, ui16Id);

	return 0;
}

static int s_dev_rom_data_del_by_type (uint16_t ui16Type)
{
	uint16_t ui16i;
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	//ɾ��ROM����
	for (ui16i=s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START]; ui16i<s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]; ui16i++)
	{
		iRet = s_dev_rom_data_del(ui16Type, M_GET_ID_FROM_IDX(s_sDevStateData.asRecord[ui16i].tDevIdx));
		if (iRet < 0)
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_rom_data_del_by_type]Call 's_dev_rom_data_del(%04hx, %04hx)' fail.\r\n", ui16Type, M_GET_ID_FROM_IDX(s_sDevStateData.asRecord[ui16i].tDevIdx));
			return iRet;
		}
		else
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_rom_data_del_by_type]Have deleted device %u:%08x.\r\n", ui16i, s_sDevStateData.asRecord[ui16i].tDevIdx);
		}
	}

	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_rom_data_del_by_type]Have deleted %d devices of %04hx.\r\n", (ui16i-s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START]), ui16Type);

	return 0;
}

static int s_dev_rom_dir_del_by_type (uint16_t ui16Type)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	//ɾ�������ݵ�Ŀ¼
	if (NULL == data_dir_path_get_by_type(ui16Type, achPath, sizeof(achPath)))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_dev_rom_dir_del_by_type]Can't get path of device type %04hx.\r\n", ui16Type);
		return FILE_S_ADDR_ERR;
	}
	iRet = del_empty_dir(achPath);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_dev_rom_dir_del_by_type]Can't delete empty dir for device type %04hx.\r\n", ui16Type);
		return iRet;
	}

	return 0;
}

static int s_dev_data_add (uint16_t ui16Type, const void *pvRamBuf, size_t tRamSize, const void *pvRomBuf, size_t tRomSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint16_t ui16Id, ui16Offset;
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(NULL != pvRamBuf);
	ASSERT(NULL != pvRomBuf);
	ASSERT(tRamSize <= sizeof(U_DevStateSave));	//���ܳ������ݵ����߽�
	ASSERT(tRomSize <= sizeof(U_DevAttrSave));	//���ܳ������ݵ����߽�

	do
	{
		//������ID
		iRet = s_dev_data_assign_id_by_type(ui16Type);
		if (iRet < MUI_DEV_ID_MIN)
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_dev_data_add]Can't assign new ID for type %04hx\r\n", ui16Type);
			break;
		}
		ui16Id = (uint16_t)(((uint32_t)iRet)&0x0000FFFF);
		ui16Offset = (uint16_t)(((uint32_t)iRet)>>16);
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_add]New ID=%04hx, will insert s_sDevStateData.asRecord[%u]\r\n", ui16Id, ui16Offset);

		//������ROM����(�Ȳ���ROM�����ٲ���RAM����,��ΪRAM����ֻ���ڴ����û��ʧ�ܿ���,������˳����Ϊ�˷�ֹ���˲���)
		if (NULL == data_dir_path_get_by_type(ui16Type, achPath, sizeof(achPath)))
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_add]Can't get path of device type %04hx\r\n", ui16Type);
			iRet = FILE_S_ADDR_ERR;
			break;
		}
		iRet = mk_multi_dir(achPath, MUI_DATA_CREAT_MODE);
		if (iRet < 0)
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_add]Can't execute mkdir for device type %04hx\r\n", ui16Type);
			iRet = FILE_S_ADDR_ERR;
			break;
		}
		if (NULL == data_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_add]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
			iRet = FILE_S_ADDR_ERR;
			break;
		}
		iRet = file_write(achPath, 0, pvRomBuf, tRomSize);
		if (iRet < 0)
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_add]Can't write new device data-%s to ROM.\r\n", achPath);
			break;
		}

		//������RAM����
		iRet = s_dev_ram_insert_by_type_offset(ui16Type, ui16Offset, ui16Id, pvRamBuf, tRamSize);
		if (iRet < 0)
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_add]Can't insert record to RAM.\r\n");
			break;
		}

		iRet = (int)ui16Id;
	} while (0);

	return iRet;
}

static int s_dev_data_del (uint16_t ui16Type, uint16_t ui16Id)
{
	uint16_t ui16Offset;
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	//����ID
	iRet = s_dev_ram_search(ui16Type, ui16Id, s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START], s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_dev_data_del]Can't find device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_NOFIND_ERR;
	}
	ui16Offset = (uint16_t)iRet;
	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_del]Have find device %04hx:%04hx on s_sDevStateData.asRecord[%u].\r\n", ui16Type, ui16Id, ui16Offset);

	//��ɾ��ROM����(�Ȳ���ROM�����ٲ���RAM����,��ΪRAM����ֻ���ڴ����û��ʧ�ܿ���,������˳����Ϊ�˷�ֹ���˲���)
	iRet = s_dev_rom_data_del(ui16Type, ui16Id);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_del]Call 's_dev_rom_data_del(%04hx, %04hx)' fail.\r\n", ui16Type, ui16Id);
		return iRet;
	}
	//ɾ�������ݵ�Ŀ¼
	if (0 > s_dev_rom_dir_del_by_type(ui16Type))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_del]Call 's_dev_rom_dir_del_by_type(%04hx)' fail.\r\n", ui16Type);
	}

	//��ɾ��RAM����
	iRet = s_dev_ram_remove_by_type_offset(ui16Type, ui16Offset);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_del]Can't remove record from RAM.\r\n");
	}

	return iRet;
}

static int s_dev_data_del_by_type (uint16_t ui16Type)
{
	uint16_t ui16Num, ui16i;
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	iRet = s_dev_data_num_get_by_type(ui16Type);
	if (iRet <= 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[s_dev_data_del_by_type]Can't find any devices of type %04hx.\r\n", ui16Type);
		return FILE_S_NOFIND_ERR;
	}
	ui16Num = (uint16_t)iRet;

	//��ɾ��ROM����(�Ȳ���ROM�����ٲ���RAM����,��ΪRAM����ֻ���ڴ����û��ʧ�ܿ���,������˳����Ϊ�˷�ֹ���˲���)
	iRet = s_dev_rom_data_del_by_type (ui16Type);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_del_by_type]Call 's_dev_rom_data_del_by_type(%04hx)' fail.\r\n", ui16Type);
		return iRet;
	}
	//ɾ�������ݵ�Ŀ¼
	if (0 > s_dev_rom_dir_del_by_type(ui16Type))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_del_by_type]Call 's_dev_rom_dir_del_by_type(%04hx)' fail.\r\n", ui16Type);
	}

	//��ɾ��RAM����
	//��1��:���S_DevStateRecord��ǰ�ƶ�,֮��������������Ϊ�˼����ڴ��ʹ��
	for (ui16i=s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]; ui16i<(s_sDevStateData.ui16DevTotal-1); ui16i++)
	{
		memcpy(&s_sDevStateData.asRecord[ui16i], &s_sDevStateData.asRecord[ui16i+ui16Num], sizeof(s_sDevStateData.asRecord[ui16i]));
	}
	//��2��:����s_sDevStateData.aui16DevTypeIdx[]
	for (ui16i=ui16Type+1-MUI_DATA_TYPE_DEV_START; ui16i<(sizeof(s_sDevStateData.aui16DevTypeIdx)/sizeof(s_sDevStateData.aui16DevTypeIdx[0])); ui16i++)
	{
		s_sDevStateData.aui16DevTypeIdx[ui16i] -= ui16Num;
	}
	//��3��:����s_sDevStateData.ui16DevTotal
	s_sDevStateData.ui16DevTotal -= ui16Num;

	return 0;
}

static int s_dev_data_del_all (void)
{
	uint16_t ui16Type;
	int iRet;

	//��ɾ��ROM����(�Ȳ���ROM�����ٲ���RAM����,��ΪRAM����ֻ���ڴ����û��ʧ�ܿ���,������˳����Ϊ�˷�ֹ���˲���)
	for (ui16Type=MUI_DATA_TYPE_DEV_START; ui16Type<=MUI_DATA_TYPE_DEV_END; ui16Type++)
	{
		iRet = s_dev_rom_data_del_by_type(ui16Type);
		if (iRet < 0)
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_del_all]Call 's_dev_rom_dir_del_by_type(%04hx)' fail.\r\n", ui16Type);
			return iRet;
		}
		//ɾ�������ݵ�Ŀ¼
		if (0 > s_dev_rom_dir_del_by_type(ui16Type))
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_del_all]Call 's_dev_rom_dir_del_by_type(%04hx)' fail.\r\n", ui16Type);
		}
	}

	//��ɾ��RAM����
	memset(&s_sDevStateData, 0, sizeof(s_sDevStateData));
	iRet = s_dev_data_state_del();
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_del_all]Call 's_dev_data_state_del()' fail.\r\n");
		return iRet;
	}

	return 0;
}

static int s_dev_data_swap_id (uint16_t ui16Type, uint16_t ui16DestId, uint16_t ui16SrcId)
{
	char achBuf[MUI_FILE_ABS_PATH_LEN*2];
	char *pchSrcPath, *pchDestPath;
	uint16_t ui16SrcOffset, ui16DestOffset;
	int iRet;

	ASSERT(sizeof(U_DevStateSave) <= sizeof(achBuf));
	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(ui16DestId != ui16SrcId);

	//����ID
	iRet = s_dev_ram_search(ui16Type, ui16SrcId, s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START], s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_swap_id]Can't find device %04hx:%04hx.\r\n", ui16Type, ui16SrcId);
		return FILE_S_NOFIND_ERR;
	}
	ui16SrcOffset = (uint16_t)iRet;
	iRet = s_dev_ram_search(ui16Type, ui16DestId, s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START], s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_swap_id]Can't find device %04hx:%04hx.\r\n", ui16Type, ui16DestId);
		return FILE_S_NOFIND_ERR;
	}
	ui16DestOffset = (uint16_t)iRet;

	pchSrcPath  = &achBuf[0*MUI_FILE_ABS_PATH_LEN];
	pchDestPath = &achBuf[1*MUI_FILE_ABS_PATH_LEN];
	//���޸�ROM����(�Ȳ���ROM�����ٲ���RAM����,��ΪRAM����ֻ���ڴ����û��ʧ�ܿ���,������˳����Ϊ�˷�ֹ���˲���)
	if (NULL == data_file_path_get(ui16Type, ui16SrcId, pchSrcPath, MUI_FILE_ABS_PATH_LEN))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_swap_id]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16SrcId);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_file_path_get(ui16Type, ui16DestId, pchDestPath, MUI_FILE_ABS_PATH_LEN))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_swap_id]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16DestId);
		return FILE_S_ADDR_ERR;
	}

	iRet = file_name_swap(pchDestPath, pchSrcPath);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_swap_id]Call 'file_name_swap(%s, %s)' fail.\r\n", pchDestPath, pchSrcPath);
		return iRet;
	}

	//���޸�RAM����
	//�����豸RAM����
	memcpy(achBuf, &s_sDevStateData.asRecord[ui16SrcOffset].uDevState, sizeof(s_sDevStateData.asRecord[ui16SrcOffset].uDevState));
	memcpy(&s_sDevStateData.asRecord[ui16SrcOffset].uDevState, &s_sDevStateData.asRecord[ui16DestOffset].uDevState, sizeof(s_sDevStateData.asRecord[ui16SrcOffset].uDevState));
	memcpy(&s_sDevStateData.asRecord[ui16DestOffset].uDevState, achBuf, sizeof(s_sDevStateData.asRecord[ui16DestOffset].uDevState));

	return 0;
}

static int s_dev_data_set_new_id (uint16_t ui16Type, uint16_t ui16NewId, uint16_t ui16Id)
{
	char achBuf[MUI_FILE_ABS_PATH_LEN*2];
	char *pchPath, *pchNewPath;
	uint16_t ui16Offset, ui16NewOffset, ui16i;
	int iRet;

	ASSERT(sizeof(U_DevStateSave) > sizeof(achBuf));
	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(ui16NewId != ui16Id);

	pchPath    = &achBuf[MUI_FILE_ABS_PATH_LEN*0];
	pchNewPath = &achBuf[MUI_FILE_ABS_PATH_LEN*1];

	//����ID
	iRet = s_dev_ram_search(ui16Type, ui16Id, s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START], s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_set_new_id]Can't find device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_NOFIND_ERR;
	}
	ui16Offset = (uint16_t)iRet;
	iRet = s_dev_ram_search(ui16Type, ui16NewId, s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START], s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]);
	if (iRet >= 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_set_new_id]Device %04hx:%04hx already exist.\r\n", ui16Type, ui16NewId);
		return FILE_S_NOFIND_ERR;
	}
	ui16NewOffset = (uint16_t)(iRet-MI_DEV_RAM_SEARCH_NEW_OFFSET);

	//���޸�ROM����(�Ȳ���ROM�����ٲ���RAM����,��ΪRAM����ֻ���ڴ����û��ʧ�ܿ���,������˳����Ϊ�˷�ֹ���˲���)
	if (NULL == data_file_path_get(ui16Type, ui16Id, pchPath, MUI_FILE_ABS_PATH_LEN))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_set_new_id]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_ADDR_ERR;
	}
	if (NULL == data_file_path_get(ui16Type, ui16NewId, pchNewPath, MUI_FILE_ABS_PATH_LEN))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_set_new_id]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16NewId);
		return FILE_S_ADDR_ERR;
	}
	iRet = file_rename(pchNewPath, pchPath);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_set_new_id]Call 'file_rename(%s, %s)' fail.\r\n", pchNewPath, pchPath);
		return iRet;
	}

	//���޸�RAM����
	//��1��:���ݵ�ǰID�豸��RAM����
	s_sDevStateData.asRecord[ui16Offset].tDevIdx = M_GEN_IDX_BY_TYPE_ID(ui16Type, ui16NewId);
	memcpy(achBuf, &s_sDevStateData.asRecord[ui16Offset], sizeof(s_sDevStateData.asRecord[ui16Offset]));
	//��2��:����ƶ�S_DevStateRecord�е�����,֮��������������Ϊ�˼����ڴ��ʹ��
	if (ui16Offset > ui16NewOffset)
	{
		for (ui16i=ui16Offset; ui16i>ui16NewOffset; ui16i--)
		{
			memcpy(&s_sDevStateData.asRecord[ui16i], &s_sDevStateData.asRecord[ui16i-1], sizeof(s_sDevStateData.asRecord[ui16i]));
		}
	}
	else
	{
		for (ui16i=ui16Offset; ui16i<ui16NewOffset; ui16i++)
		{
			memcpy(&s_sDevStateData.asRecord[ui16i], &s_sDevStateData.asRecord[ui16i+1], sizeof(s_sDevStateData.asRecord[ui16i]));
		}
	}
	//��3��:���뱸������
	memcpy(&s_sDevStateData.asRecord[ui16NewOffset], achBuf, sizeof(s_sDevStateData.asRecord[ui16NewOffset]));

	return 0;
}

static int s_dev_data_state_checkout (S_DevStateData *psDevStateData)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	uint32_t ui32Crc;
	int iRet, iLen;

	ASSERT(NULL != psDevStateData);

	if (NULL == data_dev_state_backup_file_path_get(achPath, sizeof(achPath)))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_checkout]Can't get path of device state backup file.\r\n");
		return FILE_S_ADDR_ERR;
	}

	iLen = file_read(achPath, M_OFFSET(S_DevStateDataBackup, sDevStateData), psDevStateData, M_SIZEOF(S_DevStateDataBackup, sDevStateData));
	if (iLen < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_checkout]Call 'file_read(%s, sDevStateData)' fail.\r\n", achPath);
		return iLen;
	}

	iRet = file_read(achPath, M_OFFSET(S_DevStateDataBackup, ui32Crc), &ui32Crc, M_SIZEOF(S_DevStateDataBackup, ui32Crc));
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_checkout]Call 'file_read(%s, ui32Crc)' fail.\r\n", achPath);
		return iRet;
	}

	if (crc32_cal(psDevStateData, sizeof(S_DevStateData)) != ui32Crc)
	{
		return -1;
	}

	return iLen;
}

static int s_dev_data_state_backup (const S_DevStateData *psDevStateData, uint32_t ui32Crc)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet, iLen;

	ASSERT(NULL != psDevStateData);

	if (NULL == data_dev_state_backup_file_path_get(achPath, sizeof(achPath)))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_backup]Can't get path of device state backup file.\r\n");
		return FILE_S_ADDR_ERR;
	}

	iLen = file_write(achPath, M_OFFSET(S_DevStateDataBackup, sDevStateData), psDevStateData, M_SIZEOF(S_DevStateDataBackup, sDevStateData));
	if (iLen < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_backup]Call 'file_write(%s, sDevStateData)' fail.\r\n", achPath);
		return iLen;
	}

	iRet = file_write(achPath, M_OFFSET(S_DevStateDataBackup, ui32Crc), &ui32Crc, M_SIZEOF(S_DevStateDataBackup, ui32Crc));
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_backup]Call 'file_write(%s, ui32Crc)' fail.\r\n", achPath);
		return iRet;
	}

	return iLen;
}

static int s_dev_data_state_sync (const S_DevStateData *psDevStateData)
{
	uint8_t aui8Buf[1024];
	char *pchPath = (char*)aui8Buf;
	void *pvRam;
	size_t tRemain, tOffset, tSize, tWrote;
	uint32_t ui32Crc;
	int iFd, iRet, iTmp;

	ASSERT(NULL != psDevStateData);

	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_data_state_sync]Will synchronize all devices state.\r\n");

	if (NULL == data_dev_state_backup_file_path_get(pchPath, sizeof(aui8Buf)))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_sync]Can't get path of device state backup file.\r\n");
		return FILE_S_ADDR_ERR;
	}

	iFd = open(pchPath, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if (iFd < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_sync]Call function 'open(%s)' fail, errno:%d\r\n", pchPath, errno);
		return iFd;
	}

	iRet	= 0;
	iTmp	= 0;
	tWrote	= 0;
	tRemain	= M_SIZEOF(S_DevStateDataBackup, sDevStateData);
	tOffset = M_OFFSET(S_DevStateDataBackup, sDevStateData);
	pvRam	= (void*)psDevStateData;
	ui32Crc = 0;
	while (tRemain > 0)
	{
		if (tRemain > sizeof(aui8Buf))
		{
			tSize = sizeof(aui8Buf);
		}
		else
		{
			tSize = tRemain;
		}

		iRet = file_read_by_fd(iFd, tOffset, aui8Buf, tSize);
		if ((iRet<0) || ((size_t)iRet!=tSize))
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_sync]Call function 'file_read_by_fd(data)' fail.\r\n");
			iRet = -1;
			break;
		}
		if (0 != memcmp(aui8Buf, pvRam, tSize))
		{
			iTmp = -1;
			iRet = file_write_by_fd(iFd, tOffset, pvRam, tSize);
			if ((iRet<0) || ((size_t)iRet!=tSize))
			{
				DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_sync]Call function 'file_write_by_fd(data)' fail.\r\n");
				iRet = -1;
				break;
			}
			tWrote += tSize;
		}

		ui32Crc = crc32_cal_continue(pvRam, tSize, ui32Crc);

		tOffset += tSize;
		pvRam   += tSize;
		tRemain -= tSize;
	}

	if (iRet >= 0)
	{
		if (iTmp < 0)
		{
			iRet = file_write_by_fd(iFd, M_OFFSET(S_DevStateDataBackup, ui32Crc), &ui32Crc, M_SIZEOF(S_DevStateDataBackup, ui32Crc));
			if (iRet < 0)
			{
				DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_sync]Call function 'file_write_by_fd(CRC)' fail.\r\n");
			}
		}
		iRet = (int)tWrote;
	}

	close(iFd);

	return iRet;
}

static int s_dev_data_state_del (void)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iRet, iLen;

	if (NULL == data_dev_state_backup_file_path_get(achPath, sizeof(achPath)))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_del]Can't get path of device state backup file.\r\n");
		return FILE_S_ADDR_ERR;
	}

	iRet = file_del(achPath);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[s_dev_data_state_del]Can't delete device state backup file.\r\n");
		return iRet;
	}

	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[s_dev_rom_data_del]Have deleted device state backup file.\r\n");

	return 0;
}















int dev_data_total_get (void)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_total_get]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_total_get();

	sem_post(&s_tDevRamMutex);

	return iRet;
}

int dev_data_idx_list_get (uint32_t *pui32Buf, uint16_t ui16Capacity)
{
	int iRet;

	ASSERT(NULL != pui32Buf);

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_idx_list_get]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_idx_list_get(pui32Buf, ui16Capacity);

	sem_post(&s_tDevRamMutex);

	return iRet;
}

int dev_data_num_get_by_type (uint16_t ui16Type)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_num_get_by_type]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_num_get_by_type(ui16Type);

	sem_post(&s_tDevRamMutex);

	return iRet;
}

int dev_data_id_list_get_by_type (uint16_t ui16Type, uint16_t *pui16Buf, uint16_t ui16Capacity)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(NULL != pui16Buf);

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_id_list_get_by_type]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_id_list_get_by_type(ui16Type, pui16Buf, ui16Capacity);

	sem_post(&s_tDevRamMutex);

	return iRet;
}

int dev_data_assign_id_by_type (uint16_t ui16Type)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_assign_id_by_type]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_assign_id_by_type(ui16Type);

	sem_post(&s_tDevRamMutex);

	return iRet;
}

int dev_data_ram_read (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT((MUI_DEV_ID_MIN<=ui16Id) && (ui16Id<=MUI_DEV_ID_MAX));
	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(U_DevStateSave));	//���ܳ������ݵ����߽�

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_ram_read]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	//�˴����õ���ui16Type+1�ᳬ��,��Ϊ���ݽṹ���Ѿ���Ԥ����һ����Ԫ,���һ����Ԫָ��ʵ�����ݵ�β��+1
	iRet = s_dev_ram_search(ui16Type, ui16Id, s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START], s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_ram_read]Can't find device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		iRet = FILE_S_NOFIND_ERR;
	}
	else
	{
		memcpy(pvBuf, tOffset+(void*)&s_sDevStateData.asRecord[iRet].uDevState, tSize);
		iRet = (int)tSize;
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[dev_data_ram_read]Have read %d bytes data.\r\n", iRet);
	}

	sem_post(&s_tDevRamMutex);

	return iRet;
}

int dev_data_ram_write (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT((MUI_DEV_ID_MIN<=ui16Id) && (ui16Id<=MUI_DEV_ID_MAX));
	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(U_DevStateSave));	//���ܳ������ݵ����߽�

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_ram_write]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	//�˴����õ���ui16Type+1�ᳬ��,��Ϊ���ݽṹ���Ѿ���Ԥ����һ����Ԫ,���һ����Ԫָ��ʵ�����ݵ�β��+1
	iRet = s_dev_ram_search(ui16Type, ui16Id, s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START], s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]);
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_ram_write]Can't find device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		iRet = FILE_S_NOFIND_ERR;
	}
	else
	{
		memcpy(tOffset+(void*)&s_sDevStateData.asRecord[iRet].uDevState, pvBuf, tSize);
		iRet = (int)tSize;
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[dev_data_ram_write]Have wrote %d bytes data.\r\n", iRet);
	}

	sem_post(&s_tDevRamMutex);

	return iRet;
}

int dev_data_rom_read (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT((MUI_DEV_ID_MIN<=ui16Id) && (ui16Id<=MUI_DEV_ID_MAX));
	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(U_DevAttrSave));	//���ܳ������ݵ����߽�

	if (NULL == data_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_rom_read]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_read(achPath, tOffset, pvBuf, tSize, &s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX);
}

int dev_data_rom_write (uint16_t ui16Type, uint16_t ui16Id, size_t tOffset, const void *pvBuf, size_t tSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT((MUI_DEV_ID_MIN<=ui16Id) && (ui16Id<=MUI_DEV_ID_MAX));
	ASSERT(NULL != pvBuf);
	ASSERT((tOffset+tSize) <= sizeof(U_DevAttrSave));	//���ܳ������ݵ����߽�

	if (NULL == data_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_rom_write]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
		return FILE_S_ADDR_ERR;
	}

	return file_exclusive_write(achPath, tOffset, pvBuf, tSize, &s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX);
}

int dev_data_ram_rom_write (uint16_t ui16Type, uint16_t ui16Id, size_t tRamOffset, const void *pvRamBuf, size_t tRamSize, size_t tRomOffset, const void *pvRomBuf, size_t tRomSize)
{
	char achPath[MUI_FILE_ABS_PATH_LEN];
	int iIdx, iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT((MUI_DEV_ID_MIN<=ui16Id) && (ui16Id<=MUI_DEV_ID_MAX));
	ASSERT(NULL != pvRamBuf);
	ASSERT(NULL != pvRomBuf);
	ASSERT((tRomOffset+tRomSize) <= sizeof(U_DevAttrSave));		//���ܳ������ݵ����߽�
	ASSERT((tRamOffset+tRamSize) <= sizeof(U_DevStateSave));	//���ܳ������ݵ����߽�

	if (0 > sem_wait_ms(&s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_ROM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_ram_rom_write]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_ram_rom_write]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		sem_post(&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	do
	{
		//�˴����õ���ui16Type+1�ᳬ��,��Ϊ���ݽṹ���Ѿ���Ԥ����һ����Ԫ,���һ����Ԫָ��ʵ�����ݵ�β��+1
		iIdx = s_dev_ram_search(ui16Type, ui16Id, s_sDevStateData.aui16DevTypeIdx[ui16Type-MUI_DATA_TYPE_DEV_START], s_sDevStateData.aui16DevTypeIdx[ui16Type+1-MUI_DATA_TYPE_DEV_START]);
		if (iIdx < 0)
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_ram_rom_write]Can't find device %04hx:%04hx.\r\n", ui16Type, ui16Id);
			iRet = FILE_S_NOFIND_ERR;
			break;
		}
		if (NULL == data_file_path_get(ui16Type, ui16Id, achPath, sizeof(achPath)))
		{
			DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_ram_rom_write]Can't get path of device %04hx:%04hx.\r\n", ui16Type, ui16Id);
			iRet = FILE_S_ADDR_ERR;
			break;
		}
		iRet = file_write(achPath, tRomOffset, pvRomBuf, tRomSize);
		if (iRet < 0)
		{
			break;
		}
		memcpy(tRamOffset+(void*)&s_sDevStateData.asRecord[iIdx].uDevState, pvRamBuf, tRamSize);
		iRet += (int)tRamSize;
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[dev_data_ram_rom_write]Have wrote %d bytes data to RAM.\r\n", iRet);
	} while (0);

	sem_post(&s_tDevRamMutex);
	sem_post(&s_tDevRomMutex);

	return iRet;
}

int dev_data_add (uint16_t ui16Type, const void *pvRomBuf, size_t tRomSize, const void *pvRamBuf, size_t tRamSize)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));
	ASSERT(NULL != pvRamBuf);
	ASSERT(NULL != pvRomBuf);
	ASSERT(tRomSize <= sizeof(U_DevAttrSave));	//���ܳ������ݵ����߽�
	ASSERT(tRamSize <= sizeof(U_DevStateSave));	//���ܳ������ݵ����߽�

	if (0 > sem_wait_ms(&s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_ROM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_add]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		sem_post(&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_add(ui16Type, pvRamBuf, tRamSize, pvRomBuf, tRomSize);

	s_dev_data_ram_show();

	sem_post(&s_tDevRamMutex);
	sem_post(&s_tDevRomMutex);

	return iRet;
}

int dev_data_del (uint16_t ui16Type, uint16_t ui16Id)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_ROM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_del]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_del]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		sem_post(&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_del(ui16Type, ui16Id);

	s_dev_data_ram_show();

	sem_post(&s_tDevRamMutex);
	sem_post(&s_tDevRomMutex);

	return iRet;
}

int dev_data_del_by_type (uint16_t ui16Type)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_ROM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_del_by_type]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_del_by_type]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		sem_post(&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_del_by_type(ui16Type);

	s_dev_data_ram_show();

	sem_post(&s_tDevRamMutex);
	sem_post(&s_tDevRomMutex);

	return iRet;
}

int dev_data_del_all (void)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_ROM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_del_all]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_del_all]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		sem_post(&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_del_all();

	s_dev_data_ram_show();

	sem_post(&s_tDevRamMutex);
	sem_post(&s_tDevRomMutex);

	return iRet;
}

int dev_data_swap_id (uint16_t ui16Type, uint16_t ui16DestId, uint16_t ui16SrcId)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_ROM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_swap_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_swap_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		sem_post(&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_swap_id(ui16Type, ui16DestId, ui16SrcId);

	s_dev_data_ram_show();

	sem_post(&s_tDevRamMutex);
	sem_post(&s_tDevRomMutex);

	return iRet;
}


int dev_data_set_new_id (uint16_t ui16Type, uint16_t ui16NewId, uint16_t ui16Id)
{
	int iRet;

	ASSERT((MUI_DATA_TYPE_DEV_START<=ui16Type) && (ui16Type<=MUI_DATA_TYPE_DEV_END));

	if (0 > sem_wait_ms(&s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_ROM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_set_new_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}
	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_set_new_id]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		sem_post(&s_tDevRomMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_set_new_id(ui16Type, ui16NewId, ui16Id);

	s_dev_data_ram_show();

	sem_post(&s_tDevRamMutex);
	sem_post(&s_tDevRomMutex);

	return iRet;
}

int dev_data_state_sync (void)
{
	int iRet;

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_WARNING, "[dev_data_state_sync]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_state_sync(&s_sDevStateData);

	sem_post(&s_tDevRamMutex);

	return iRet;
}

int dev_data_init (void)
{
	U_DevStateSave uDevState;
	uint16_t ui16Type, ui16TmpId, ui16Offset;
	uint16_t aui16IdList[MUI_LOGIC_DEV_NUM_MAX];
	int iRet, iDevTotal, iIdx, iCnt, iOldCnt, i, j;

	iRet = sem_init(&s_tDevRomMutex, 0, 1);	//��ʼ��Ϊ1
	if (iRet < 0)
	{
		return iRet;
	}
	iRet = sem_init(&s_tDevRamMutex, 0, 1);	//��ʼ��Ϊ1
	if (iRet < 0)
	{
		return iRet;
	}

	if (0 > sem_wait_ms(&s_tDevRamMutex, MUI_DEV_RAM_WAIT_TIME_MS_MAX))	//�������(MUI_DEV_RAM_WAIT_TIME_MS_MAX)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_init]Wait semaphore %u over time.\r\n", (uint32_t)&s_tDevRamMutex);
		return FILE_S_ERR_SEM_FAIL;
	}

	iRet = s_dev_data_state_checkout(&s_sDevStateData);
	if (iRet < 0)
	{
		memset(&s_sDevStateData, 0, sizeof(s_sDevStateData));
	}

	s_dev_data_ram_show();

	//��ʼ��RAM
	iDevTotal = 0;
	for (iIdx=0; iIdx<(int)(sizeof(s_sDevStateData.aui16DevTypeIdx)/sizeof(s_sDevStateData.aui16DevTypeIdx[0])); iIdx++)
	{
		ui16Type = (uint16_t)iIdx + MUI_DATA_TYPE_DEV_START;
		if (ui16Type > MUI_DATA_TYPE_DEV_END)
		{
			iCnt = 0;	//�˴�������,��Ϊ�˶Ժ�����������и�ֵ
		}
		else
		{
			iCnt = data_id_exclusive_scan_by_type(ui16Type, aui16IdList, MUI_LOGIC_DEV_NUM_MAX, &s_tDevRomMutex, MUI_DEV_ROM_WAIT_TIME_MS_MAX);
			if (iCnt < 0)
			{
				DEV_DATA_DYN_PRINT(M_DBG_LEVEL_NOTICE, "[dev_data_init]Can't find device type:%04hx\r\n", ui16Type);
				iCnt = 0;	//�˴�������,��Ϊ�˶Ժ�����������и�ֵ
			}
		}
		//��RAM���������
		s_sDevStateData.aui16DevTypeIdx[iIdx] = iDevTotal;	//�������,�������,��˵�������д�,�����޸�
		if (iIdx < MUI_DEV_TYPE_NUM)
		{
			iOldCnt = s_sDevStateData.aui16DevTypeIdx[iIdx+1] - s_sDevStateData.aui16DevTypeIdx[iIdx];
			//��ɾ�����������
			j = 0;
			while (j < iOldCnt)
			{
				ui16Offset = s_sDevStateData.aui16DevTypeIdx[iIdx] + j;
				if (ui16Type != M_GET_TYPE_FROM_IDX(s_sDevStateData.asRecord[ui16Offset].tDevIdx))
				{
					//���Ͳ���,����ɾ������
					iRet = s_dev_ram_remove_by_type_offset(ui16Type, ui16Offset);
					if (iRet < 0)
					{
						DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_init]Can't remove wrong type record from RAM.\r\n");
						sem_post(&s_tDevRamMutex);
						return iRet;
					}
					iOldCnt--;
					continue;
				}
				ui16TmpId = M_GET_ID_FROM_IDX(s_sDevStateData.asRecord[ui16Offset].tDevIdx);
				for (i=0; i<iCnt; i++)
				{
					if (ui16TmpId == aui16IdList[i])
					{
						break;
					}
				}
				if (i >= iCnt)
				{
					//δ�ҵ���Ӧ��ID,����ɾ������
					iRet = s_dev_ram_remove_by_type_offset(ui16Type, ui16Offset);
					if (iRet < 0)
					{
						DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_init]Can't remove noexistent record from RAM.\r\n");
						sem_post(&s_tDevRamMutex);
						return iRet;
					}
					iOldCnt--;
					continue;
				}
				j++;
			}
			//����ȱʧ������
			for (i=0; i<iCnt; i++)
			{
				ui16Offset = s_sDevStateData.aui16DevTypeIdx[iIdx];
				ui16TmpId  = 0;
				for (j=0; j<iOldCnt; j++, ui16Offset++)
				{
					ui16TmpId  = M_GET_ID_FROM_IDX(s_sDevStateData.asRecord[ui16Offset].tDevIdx);
					if (ui16TmpId == aui16IdList[i])
					{
						ui16TmpId = 0xFFFF;
						break;
					}
					else if (ui16TmpId > aui16IdList[i])
					{
						j = iOldCnt;
						break;
					}
				}
				if (0xFFFF == ui16TmpId)
				{
					continue;
				}
				if (j >= iOldCnt)
				{
					//δ�ҵ���Ӧ��ID,������������
					iRet = init_dev_ram_data(ui16Type, aui16IdList[i], &uDevState);
					if (iRet < 0)
					{
						DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_init]Call 'init_dev_ram_data()' fail.\r\n");
						sem_post(&s_tDevRamMutex);
						return iRet;
					}
					iRet = s_dev_ram_insert_by_type_offset(ui16Type, ui16Offset, aui16IdList[i], &uDevState, sizeof(uDevState));
					if (iRet < 0)
					{
						DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_init]Can't insert record to RAM.\r\n");
						sem_post(&s_tDevRamMutex);
						return iRet;
					}
					iOldCnt++;
				}
			}
		}

		iDevTotal += iCnt;
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[dev_data_init]Have found %d devices of type:%04hx.\r\n", iCnt, ui16Type);
	}
	s_sDevStateData.ui16DevTotal = iDevTotal;

	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[dev_data_init]Have found total %u devices.\r\n", s_sDevStateData.ui16DevTotal);

	iRet = s_dev_data_state_backup(&s_sDevStateData, crc32_cal(&s_sDevStateData, sizeof(S_DevStateData)));
	if (iRet < 0)
	{
		DEV_DATA_DYN_PRINT(M_DBG_LEVEL_ERR, "[dev_data_init]Call 's_dev_data_state_backup()' fail.\r\n");
		sem_post(&s_tDevRamMutex);
		return iRet;
	}

	s_dev_data_ram_show();

	sem_post(&s_tDevRamMutex);

	DEV_DATA_DYN_PRINT(M_DBG_LEVEL_INFO, "[dev_data_init]Device data initialize successfully.\r\n");

	return iDevTotal;
}

int dev_data_test (void)
{
	int i;

	U_DevAttrSave uAttr;
	U_DevStateSave uState;
/*
	for (i=0; i<MUI_LOGIC_DEV_NUM_MAX+10; i++)
	{
		dev_data_add(MUI_DATA_TYPE_LIGHT, &uAttr, sizeof(uAttr), &uState, sizeof(uState));
	}

	for (i=100; i<101; i++)
	{
		dev_data_del(MUI_DATA_TYPE_LIGHT, i);
	}

	for (i=1; i<2; i++)
	{
		dev_data_del(MUI_DATA_TYPE_LIGHT, i);
	}

	for (i=10; i<20; i++)
	{
		dev_data_del(MUI_DATA_TYPE_LIGHT, i);
	}

	for (i=50; i<60; i++)
	{
		dev_data_del(MUI_DATA_TYPE_LIGHT, i);
	}


	for (i=0; i<MUI_LOGIC_DEV_NUM_MAX+10; i++)
	{
		dev_data_add(MUI_DATA_TYPE_LIGHT, &uAttr, sizeof(uAttr), &uState, sizeof(uState));
	}

	for (i=0; i<1000; i++)
	{
		dev_data_del(MUI_DATA_TYPE_LIGHT, i);
	}
	for (i=0; i<MUI_LOGIC_DEV_NUM_MAX+10; i++)
	{
		dev_data_add(MUI_DATA_TYPE_LIGHT, &uAttr, sizeof(uAttr), &uState, sizeof(uState));
	}

	dev_data_del_by_type(MUI_DATA_TYPE_LIGHT);
*/
	for (i=0; i<MUI_LOGIC_DEV_NUM_MAX+10; i++)
	{
		dev_data_add(MUI_DATA_TYPE_LIGHT, &uAttr, sizeof(uAttr), &uState, sizeof(uState));
	}

	dev_data_del_all();

	return i;
}

