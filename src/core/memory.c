#define _CRT_SECURE_NO_WARNINGS
#include "..\..\include\core\memory.h"

#include <stdio.h>

#include "..\..\include\mem_manager.h"

static inline vm_ram_cells_t
__alloc_mem_cells(size_t amount)
{
    vm_ram_cells_t cells = (vm_ram_cells_t)get_mem(sizeof(vm_ram_cell) * amount);
    mem_set((void*)cells, sizeof(vm_ram_cell), amount);
    return cells;
}

vm_ram_t
vm_malloc(size_t size)
{
    vm_ram_t mem = (vm_ram_t)get_mem(sizeof(struct vm_ram));
    mem->cells = __alloc_mem_cells(size);
    mem->size = size;
    return mem;
}

void 
vm_free_mem(vm_ram_t* mem)
{
    free_mem(&(*mem)->cells);
    free_mem(mem);
}

void
vm_realloc(vm_ram_t mem, size_t size)
{
    vm_ram_cells_t cells = __alloc_mem_cells(size);
    mem_cpy((void*)mem->cells, (void*)cells, sizeof(vm_ram_cell), mem->size);
    free_mem((void*)mem->cells);
    mem->cells = cells;
}

void
vm_dump(vm_ram_t mem, char* path)
{
    FILE* out = fopen(path, "w");
    if (!out)
    {
        return;
    }
    size_t total_size = sizeof(vm_ram_cell) * mem->size;
    size_t end = total_size / 4;

    for (size_t i = 0; i < end; ++i)
    {
        fprintf(out, "%d", ((uint32_t*)mem->cells)[i]);
    }
    for (size_t i = end * 4; i < total_size; ++i)
    {
        fprintf(out, "%c", ((uint8_t*)mem->cells)[i]);
    }
    fclose(out);
}
