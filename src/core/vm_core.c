#define _CRT_SECURE_NO_WARNINGS
#include "..\..\include\core\vm_core.h"

#include <stdio.h>

#include "..\..\include\cmn_types.h"
#include "..\..\include\mem_manager.h"
#include "..\..\include\core\instructions_data.h"
#include "..\..\include\external_api.h"
#include "..\..\include\errors.h"

vm_handle_t
create_vm(size_t cores_amount)
{
    vm_handle_t vm = (vm_handle_t)get_mem(sizeof(struct vm_handle));
    vm->cores = (vm_core_t)get_mem(sizeof(struct vm_core) * cores_amount);
    vm->cores_amount = cores_amount;
    for (size_t i = 0; i < cores_amount; ++i)
    {
        mem_set(&vm->cores[i], sizeof(struct vm_core), 1);
    }
    vm->ram = vm_malloc(64 * 1024);
    
    return vm;
}
    
void 
free_vm(vm_handle_t* vm)
{
    vm_free_mem(&(*vm)->ram);
    free_mem(&(*vm)->cores);
    free_mem(vm);
}

void 
load_instructions(vm_handle_t vm, char* file)
{
    FILE* input = fopen(file, "r");
    if (!input)
    {
        return;
    }
    int byte = 0;
    size_t counter = 0;

    while ((byte = fgetc(input)) != EOF)
    {
        vm->ram->cells[counter++] = byte;
    }
}

void 
execute(__VM_STD_INSTR_ARGS_PROTO)
{
    instr_op instruction = 0;
    int err = 0;
    //...
    while (1)
    {
        instruction = __READ_IP(instr_op);
        //if (instruction > arr.size)
        //{
        //    return;
        //}
        //err = arr[instruction](__VM_STD_ARGS);
        if (err != NO_ERR)
        {
            if (err == HALT)
            {
                return;
            }
            // error catch
        }
    }
}