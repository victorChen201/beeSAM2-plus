//#define	__M_FILE_NAME_ring_buffer_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_ring_buffer_H__
#define	__M_FILE_NAME_ring_buffer_H__

#include "common.h"

typedef struct
{
	void		*pvStart;
	void		*pvEnd;
	void		*pvHead;
	void		*pvTail;
} S_RingBuffer;

typedef struct
{
	const void	*pvBuf;
	size_t		tLen;
} S_BufUnit;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern bool ring_buf_init (S_RingBuffer *psRB, void *pvStart, void *pvEnd);
extern void ring_buf_clear (S_RingBuffer *psRB);
extern bool ring_buf_if_empty (const S_RingBuffer *psRB);
extern bool ring_buf_if_full (const S_RingBuffer *psRB);
extern size_t ring_buf_size (const S_RingBuffer *psRB);
extern size_t ring_buf_data_len (const S_RingBuffer *psRB);
extern size_t ring_buf_remain (const S_RingBuffer *psRB);
extern bool ring_buf_put_one (S_RingBuffer *psRB, const void *pvData);
extern bool ring_buf_get_one (S_RingBuffer *psRB, void *pvData);
extern size_t ring_buf_wr (S_RingBuffer *psRB, const void *pvSrcBuf, size_t tLen);
extern size_t ring_buf_asm_wr (S_RingBuffer *psRB, const S_BufUnit *psBufUnitList, int iNum);
extern size_t ring_buf_rd (S_RingBuffer *psRB, void *pvDstBuf, size_t tLen);
extern size_t ring_buf_drop (S_RingBuffer *psRB, size_t tLen);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_ring_buffer_H__".
