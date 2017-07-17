#include "ring_buffer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
static void s_ring_buf_para_rst (S_RingBuffer *psRB);
static bool s_ring_buf_para_verify (S_RingBuffer *psRB);
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//	RingBuffer(环形缓冲区)的组织结构图如下:
//
//           ┌---------------------------------------┐
//   .End → │             实际缓冲区底              │
//           ├---------------------------------------┤
//           │                                       │
//           ├---------------------------------------┤
//           │                                       │
//           ├---------------------------------------┤
//           │                                       │
//           │...................................... │
//           │                                       │
//           ├---------------------------------------┤
//           │                                       │
//           ├---------------------------------------┤
//  .Head → │      .Head指向的单元必须是空的.       │
//           ├---------------------------------------┤
//           │                                       │
//           ├---------------------------------------┤
//           │                                       │
//           │...................................... │
//           │                                       │
//           ├---------------------------------------┤
//           │                                       │
//           ├---------------------------------------┤
//  .Tail → │ .Tail != .Head 时,指向的单元不能为空. │
//           ├---------------------------------------┤
//           │                                       │
//           ├---------------------------------------┤
//           │                                       │
//           │...................................... │
//           │                                       │
//           ├---------------------------------------┤
//           │                                       │
//           ├---------------------------------------┤
//           │                                       │
//           ├---------------------------------------┤
// .Start → │             实际缓冲区顶              │← 初始的 .Head == .Tail
//           └---------------------------------------┘
//
//	当 .Head 或 .Tail 不断增长时,一旦值增加到大于 .End 或小于 .Start 时,则应调整为 .Start 的值.
//	且如果 .Head 的下一个单元指向 .Tail 的话,则当前 .Head 不能写入数据,因此对于任何类型的
//	RingBuffer ,它的 Size = .End - .Start + 1 ,但在没写入任何数据的前提下,调用 Remain 函数,却可
//	以发现返回值比 Size 少一个单元.因此,从这个意义上说,一个可写入数据的 RingBuffer,它的 Size 至
//	少要有 2 个单元.
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//-------------------------------------------------------------------------------------------------*
static void s_ring_buf_para_rst (S_RingBuffer *psRB)
{
	ASSERT(psRB != NULL);

	psRB->pvStart = NULL;
	psRB->pvEnd   = NULL;
	psRB->pvHead  = NULL;
	psRB->pvTail  = NULL;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
static bool s_ring_buf_para_verify (S_RingBuffer *psRB)
{
	ASSERT(psRB != NULL);

	if (psRB->pvStart!=NULL && psRB->pvStart<psRB->pvEnd && (psRB->pvStart<=psRB->pvHead && psRB->pvHead<=psRB->pvEnd) && (psRB->pvStart<=psRB->pvTail && psRB->pvTail<=psRB->pvEnd))
	{
		return true;
	}
	s_ring_buf_para_rst(psRB);

	return false;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
bool ring_buf_init (S_RingBuffer *psRB, void *pvStart, void *pvEnd)
{
	ASSERT(psRB != NULL);
	ASSERT(pvStart != NULL);
	ASSERT(pvEnd != NULL);

	psRB->pvStart = pvStart;
	psRB->pvEnd   = pvEnd;
	psRB->pvHead  = pvStart;
	psRB->pvTail  = pvStart;

	return s_ring_buf_para_verify(psRB);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
void ring_buf_clear (S_RingBuffer *psRB)
{
	ASSERT(psRB != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	psRB->pvTail = psRB->pvHead;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
bool ring_buf_if_empty (const S_RingBuffer *psRB)
{
	ASSERT(psRB != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	return (psRB->pvHead == psRB->pvTail);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
bool ring_buf_if_full (const S_RingBuffer *psRB)
{
	ASSERT(psRB != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	return (psRB->pvHead==(uint8_t*)psRB->pvTail-1 || (psRB->pvHead==psRB->pvEnd && psRB->pvTail==psRB->pvStart));
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
size_t ring_buf_size (const S_RingBuffer *psRB)
{
	ASSERT(psRB != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	return ((uint8_t*)psRB->pvEnd - (uint8_t*)psRB->pvStart + 1);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
size_t ring_buf_data_len (const S_RingBuffer *psRB)
{
	size_t tFillLen;

	ASSERT(psRB != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	if (psRB->pvHead >= psRB->pvTail)
	{
		tFillLen  = (uint8_t*)psRB->pvHead - (uint8_t*)psRB->pvTail;
	}
	else
	{
		tFillLen  = ((uint8_t*)psRB->pvEnd - (uint8_t*)psRB->pvStart + 1) - ((uint8_t*)psRB->pvTail - (uint8_t*)psRB->pvHead);
	}

	return tFillLen;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
size_t ring_buf_remain (const S_RingBuffer *psRB)
{
	size_t tUnfillLen;

	ASSERT(psRB != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	if (psRB->pvHead >= psRB->pvTail)
	{
		tUnfillLen = ((uint8_t*)psRB->pvEnd - (uint8_t*)psRB->pvStart) - ((uint8_t*)psRB->pvHead - (uint8_t*)psRB->pvTail);
	}
	else
	{
		tUnfillLen = (uint8_t*)psRB->pvTail - (uint8_t*)psRB->pvHead - 1;
	}

	return tUnfillLen;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
bool ring_buf_put_one (S_RingBuffer *psRB, const void *pvData)
{
	ASSERT(psRB != NULL);
	ASSERT(pvData != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	if (ring_buf_if_full(psRB))
	{
		return false;
	}
	*(uint8_t*)psRB->pvHead = *(uint8_t*)pvData;
	psRB->pvHead=(uint8_t*)psRB->pvHead + 1;
	if (psRB->pvHead>psRB->pvEnd || psRB->pvHead<psRB->pvStart)
	{
		psRB->pvHead = psRB->pvStart;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
bool ring_buf_get_one (S_RingBuffer *psRB, void *pvData)
{
	ASSERT(psRB != NULL);
	ASSERT(pvData != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	if (ring_buf_if_empty(psRB))
	{
		return false;
	}
	*(uint8_t*)pvData = *(uint8_t*)psRB->pvTail;
	psRB->pvTail = (uint8_t*)psRB->pvTail + 1;
	if (psRB->pvTail>psRB->pvEnd || psRB->pvTail<psRB->pvStart)
	{
		psRB->pvTail = psRB->pvStart;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
size_t ring_buf_wr (S_RingBuffer *psRB, const void *pvSrcBuf, size_t tLen)
{
	void *pvTmp;
	size_t tTmp, tWrLen = 0;

	ASSERT(psRB != NULL);
	ASSERT(pvSrcBuf != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	pvTmp = psRB->pvHead;
	if (pvTmp >= psRB->pvTail)
	{
		tTmp = (uint8_t*)psRB->pvEnd - (uint8_t*)pvTmp + 1;
		if (psRB->pvTail==psRB->pvStart && tLen>=tTmp)
		{
			tLen = tTmp - 1;
		}
		if (tTmp <= tLen)
		{
			memcpy(pvTmp, pvSrcBuf, tTmp*sizeof(uint8_t));
			pvTmp = psRB->pvStart;
			tWrLen = tTmp;
			tLen -= tTmp;
			tTmp = (uint8_t*)psRB->pvTail - (uint8_t*)pvTmp - 1;
			if (tTmp < tLen)
			{
				tLen = tTmp;
			}
		}
	}
	else
	{
		tTmp = (uint8_t*)psRB->pvTail - (uint8_t*)pvTmp - 1;
		if (tTmp < tLen)
		{
			tLen = tTmp;
		}
	}
	memcpy(pvTmp, ((uint8_t*)pvSrcBuf+tWrLen), tLen*sizeof(uint8_t));
	psRB->pvHead = (uint8_t*)pvTmp + tLen;

	return (tWrLen + tLen);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
size_t ring_buf_asm_wr (S_RingBuffer *psRB, const S_BufUnit *psBufUnitList, int iNum)
{
	S_RingBuffer sRingBuffer;
	size_t tTmp, tTotal;
	int i;

	ASSERT(psRB != NULL);
	ASSERT(psBufUnitList != NULL);
	ASSERT(iNum > 0);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	memcpy(&sRingBuffer, psRB, sizeof(sRingBuffer));
	tTotal = 0;
	for (i=0; i<iNum; i++)
	{
		tTmp = ring_buf_wr(&sRingBuffer, psBufUnitList[i].pvBuf, psBufUnitList[i].tLen);
		tTotal += tTmp;
		sRingBuffer.pvTail = psRB->pvTail;
		if (tTmp < psBufUnitList[i].tLen)
		{
			break;
		}
	}
	psRB->pvHead = sRingBuffer.pvHead;

	return tTotal;
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
size_t ring_buf_rd (S_RingBuffer *psRB, void *pvDstBuf, size_t tLen)
{
	void *pvTmp;
	size_t tTmp, tRdLen = 0;

	ASSERT(psRB != NULL);
	ASSERT(pvDstBuf != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	pvTmp = psRB->pvTail;
	if (pvTmp > psRB->pvHead)
	{
		tTmp = (uint8_t*)psRB->pvEnd - (uint8_t*)pvTmp + 1;
		if (tTmp <= tLen)
		{
			memcpy((void*)pvDstBuf, (void*)pvTmp, tTmp*sizeof(uint8_t));
			pvTmp = psRB->pvStart;
			tRdLen = tTmp;
			tLen -= tTmp;
			tTmp = (uint8_t*)psRB->pvHead - (uint8_t*)pvTmp;
			if (tTmp < tLen)
			{
				tLen = tTmp;
			}
		}
	}
	else
	{
		tTmp = (uint8_t*)psRB->pvHead - (uint8_t*)pvTmp;
		if (tTmp < tLen)
		{
			tLen = tTmp;
		}
	}
	memcpy(((uint8_t*)pvDstBuf+tRdLen), pvTmp, tLen*sizeof(uint8_t));
	psRB->pvTail = (uint8_t*)pvTmp + tLen;

	return (tRdLen + tLen);
}
//-------------------------------------------------------------------------------------------------*
//-------------------------------------------------------------------------------------------------*
size_t ring_buf_drop (S_RingBuffer *psRB, size_t tLen)
{
	void *pvTmp;
	size_t tTmp, tRdLen = 0;

	ASSERT(psRB != NULL);
	ASSERT(psRB->pvStart!=NULL && psRB->pvEnd!=NULL && psRB->pvHead!=NULL && psRB->pvTail!=NULL);

	pvTmp = psRB->pvTail;
	if (pvTmp > psRB->pvHead)
	{
		tTmp = (uint8_t*)psRB->pvEnd - (uint8_t*)pvTmp + 1;
		if (tTmp <= tLen)
		{
			pvTmp = psRB->pvStart;
			tRdLen = tTmp;
			tLen -= tTmp;
			tTmp = (uint8_t*)psRB->pvHead - (uint8_t*)pvTmp;
			if (tTmp < tLen)
			{
				tLen = tTmp;
			}
		}
	}
	else
	{
		tTmp = (uint8_t*)psRB->pvHead - (uint8_t*)pvTmp;
		if (tTmp < tLen)
		{
			tLen = tTmp;
		}
	}
	psRB->pvTail = (uint8_t*)pvTmp + tLen;

	return (tRdLen + tLen);
}
//-------------------------------------------------------------------------------------------------*
