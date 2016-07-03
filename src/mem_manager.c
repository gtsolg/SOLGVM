#include "..\include\mem_manager.h"

#include <malloc.h>

void*
get_mem(size_t size)
{
    return malloc(size);
}

void
free_mem(void** block)
{
    free(*block);
    *block = NULL;
}

void 
mem_set(void* block, size_t sizeof_el, size_t amount)
{
    size_t total_size = sizeof_el * amount;
    size_t end = total_size / 4;

    for (size_t i = 0; i < end; ++i)
    {
        ((uint32_t*)block)[i] = 0;
    }
    for (size_t i = end * 4; i < total_size; ++i)
    {
        ((uint8_t*)block)[i] = 0;
    }
}

void 
mem_cpy(void* src, void* dest, size_t sizeof_el, size_t amount)
{
    size_t total_size = sizeof_el * amount;
    size_t end = total_size / 4;

    for (size_t i = 0; i < end; ++i)
    {
        ((uint32_t*)dest)[i] = ((uint32_t*)src)[i];
    }
    for (size_t i = end * 4; i < total_size; ++i)
    {
        ((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
    }
}