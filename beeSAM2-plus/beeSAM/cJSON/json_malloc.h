#ifndef _JASON_MALLOC_H_
#define _JASON_MALLOC_H_
#include <ctype.h>
#include <string.h>
#define MEM_POOL_SIZE  1024*3
void init_m_malloc(void);
void* mjson_malloc(size_t size);
void mjson_free(void* ptr);

#endif
