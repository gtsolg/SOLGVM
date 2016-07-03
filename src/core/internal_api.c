#include "..\..\include\core\internal_api.h"

#include <Windows.h>

#include "..\..\include\external_api.h"

__VM_STD_INSTR(load_lib)
{
    char* lib_name = &mem->cells[pop32(__VM_STD_INSTR_ARGS)];
    HMODULE lib = LoadLibraryA(lib_name);
    WREG(0) = lib;
}

__VM_STD_INSTR(get_proc_addr)
{
    HMODULE lib = pop32(__VM_STD_INSTR_ARGS);
    if (!lib)
    {
        WREG(0) = NULL;
        return;
    }
    char* proc_name = &mem->cells[pop32(__VM_STD_INSTR_ARGS)];
    WREG(0) = GetProcAddress(lib, proc_name);
}

__VM_STD_INSTR(invoke)
{

}
