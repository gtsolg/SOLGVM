#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include "cmn_types.h"

extern void* get_mem(size_t size);
extern void  free_mem(void** ptr);

extern void  mem_set(void* block, size_t sizeof_el, size_t amount);
extern void  mem_cpy(void* src, void* dest, size_t sizeof_el, size_t amount);

#endif