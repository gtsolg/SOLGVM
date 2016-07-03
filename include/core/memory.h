#ifndef MEMORY_H
#define MEMORY_H

#include "..\cmn_types.h"

typedef BYTE vm_ram_cell;
typedef BYTE* vm_ram_cells_t;

struct vm_ram
{
    vm_ram_cells_t cells;
    size_t size;
};

typedef struct vm_ram* vm_ram_t;


extern vm_ram_t vm_malloc  (size_t size);
extern void     vm_free_mem(vm_ram_t* mem);
extern void     vm_realloc (vm_ram_t  mem, size_t size);
extern void     vm_dump    (vm_ram_t  mem, char* file);

#endif
