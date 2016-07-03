#ifndef CORE_H
#define CORE_H

//#include "instructions.h"
#include "..\cmn_types.h"
#include "memory.h"

#define REGSIZE  16
#define SEGSIZE  3
#define FLAGSIZE 4

struct vm_core
{
    union
    {
        DWORD SEGMENTS[SEGSIZE];
        struct
        {
            DWORD CS; // code segment
            DWORD SS; // stack segment
            DWORD DS; // data segment
        };
    };

    DWORD SP;  // stack pointer
    DWORD IP;  // instruction pointer
    DWORD IHP; // interruption handler pointer

    QWORD IC;  // instruction counter

    union
    {
        WORD FLAGS_REG[FLAGSIZE];
        struct
        {
            WORD  FLAGS;
            WORD  EFLAGS;
            DWORD RFLAGS;
        };
    };

    union
    {
        QWORD Q; // qword register
        BYTE  B; // byte register
        WORD  W; // word register
        DWORD D; // dword register
    }
    std_regs[REGSIZE];
};

typedef struct vm_core* vm_core_t;

// dereference CELLN as PTR_TYPE
#define __PTR_VAL(CELLN, PTR_TYPE) ((PTR_TYPE*)mem->cells)[CELLN]

// reg macro
#define BREG(REGN)  core->std_regs[REGN].B
#define WREG(REGN)  core->std_regs[REGN].W
#define DREG(REGN)  core->std_regs[REGN].D
#define QREG(REGN)  core->std_regs[REGN].Q

// segments
#define CS_OP 0
#define SS_OP 1
#define DS_OP 2
#define SEGREG(REGN) core->SEGMENTS[REGN]

// flags
#define CF_MASK 0x0001
#define ZF_MASK 0x0008
#define SF_MASK 0x0010
#define OF_MASK 0x0020
#define IF_MASK 0x0040
#define CF_POS  0
#define ZF_POS  3
#define SF_POS  4
#define OF_POS  5
#define IF_POS  6
#define FLAGS_OP  0
#define EFLAGS_OP 1
#define RFLAGS_OP 2
#define FLAGSREG(REGN) core->FLAGS_REG[REGN]
#define SET_FLAG_1(FLAG_MASK) (FLAGSREG(0) |= FLAG_MASK)
#define SET_FLAG_0(FLAG_MASK) (FLAGSREG(0) &= ~FLAG_MASK)
#define GET_FLAG(FLAG_MASK, FLAG_POS) ((FLAGSREG(0) & FLAG_MASK) >> FLAG_POS)

struct vm_handle
{
    vm_core_t cores;
    size_t cores_amount;
    vm_ram_t ram;
};

typedef struct vm_handle* vm_handle_t;

#define __VM_STD_INSTR_ARGS_PROTO vm_core_t core, vm_ram_t mem
#define __VM_STD_INSTR_ARGS core, mem

extern vm_handle_t create_vm(size_t cores_amount);
extern void        load_instructions(vm_handle_t vm, char*);
extern void        free_vm(vm_handle_t* vm);
extern void        execute(__VM_STD_INSTR_ARGS_PROTO);

#endif