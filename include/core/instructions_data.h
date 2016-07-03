#ifndef VM_INSTRUCTIONS_DATA_H
#define VM_INSTRUCTIONS_DATA_H

#include "instructions.h"
#include "internal_api.h"
#include "..\cmn_types.h"

struct instruction_data
{
    vm_instr_t instruction;
    instr_op   opcode;
    char*      instruction_name;
};

typedef struct instruction_data* instruction_data_t;

struct instruction_handle
{
    instruction_data_t instructions;
    size_t size;
};

typedef struct instruction_handle* instructions_handle_t;

extern instructions_handle_t get_instructions_data(void);

#endif