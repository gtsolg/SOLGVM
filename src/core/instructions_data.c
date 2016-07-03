#include "..\..\include\core\instructions_data.h"

#include <stdarg.h>

static struct instruction_data
__init_data(vm_instr_t instruction, char* instr_name)
{
    struct instruction_data result;
    result.instruction = instruction;
    result.instruction_name = instr_name;
    result.opcode = 0;
    return result;
}

#define __OPCODE_DATA(NAME) __init_data(NAME, #NAME)

static instructions_handle_t
__get_opcodes(size_t size, ...)
{
    instructions_handle_t opcode_handler
        = (instructions_handle_t)get_mem(sizeof(struct instruction_handle));

    opcode_handler->size = size;

    opcode_handler->instructions
        = (instruction_data_t)get_mem(sizeof(struct instruction_data) * size);

    va_list args;

    va_start(args, size);
    struct instruction_data temp;
    for (size_t i = 0; i < size; ++i)
    {
        temp = va_arg(args, struct instruction_data);
        temp.opcode = i;
        opcode_handler->instructions[i] = temp;
    }
    return opcode_handler;
}

instructions_handle_t
get_instructions_data(void)
{
    return NULL;
}
