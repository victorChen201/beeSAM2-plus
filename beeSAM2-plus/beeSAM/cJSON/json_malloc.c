#include "common.h"
#include <string.h>
#include "cJSON.h"
#include "json_malloc.h"

#pragma pack(push)
#pragma pack(4)
static unsigned char g_pool[MEM_POOL_SIZE] =
{ 0 };
#pragma pack(pop)
static unsigned char* g_ptr = NULL;

void init_m_malloc(void)
{
	g_ptr = g_pool;
}

size_t len = 0;

void* mjson_malloc(size_t size)
{
	unsigned char* tmp = g_ptr;
	size = ((size + (sizeof(double) - 1)) / sizeof(double)) * sizeof(double);
	if (g_ptr + size >= &g_pool[MEM_POOL_SIZE])
		return NULL;
	g_ptr += size;
	len += size;

	return (void*) tmp;
}

void mjson_free(void* ptr)
{
	UNUSED(ptr);
}

void mjson_free_all(void)
{
	memset(g_pool, 0, sizeof(g_pool));
	g_ptr = g_pool;
}

