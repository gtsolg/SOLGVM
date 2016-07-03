#ifndef EXTERNAL_API_H
#define EXTERNAL_API_H

#include "core\vm_core.h"
#include "core\instructions.h"

// reads type from instruction pointer
#define __READ_IP(TYPE)        \
    __PTR_VAL(core->IP, TYPE); \
    core->IP += sizeof(TYPE)

// pops type from instruction pointer
#define __POP(TYPE)            \
    __PTR_VAL(core->SP, TYPE); \
    core->SP += sizeof(TYPE)

// pushes N byte on stack pointer
#define __PUSH(VAL, TYPE)           \
    core->SP -= sizeof(VAL);        \
    __PTR_VAL(core->SP, TYPE) = VAL

// read N bytes from instruction pointer
extern inline BYTE  read8_IP (__VM_STD_INSTR_ARGS_PROTO);
extern inline WORD  read16_IP(__VM_STD_INSTR_ARGS_PROTO);
extern inline DWORD read32_IP(__VM_STD_INSTR_ARGS_PROTO);
extern inline QWORD read64_IP(__VM_STD_INSTR_ARGS_PROTO);

// pop N bytes from stack pointer
extern inline BYTE  pop8 (__VM_STD_INSTR_ARGS_PROTO);
extern inline WORD  pop16(__VM_STD_INSTR_ARGS_PROTO);
extern inline DWORD pop32(__VM_STD_INSTR_ARGS_PROTO);
extern inline QWORD pop64(__VM_STD_INSTR_ARGS_PROTO);

// push N bytes on stack pointer
extern inline void push8 (__VM_STD_INSTR_ARGS_PROTO, BYTE  val);
extern inline void push16(__VM_STD_INSTR_ARGS_PROTO, WORD  val);
extern inline void push32(__VM_STD_INSTR_ARGS_PROTO, DWORD val);
extern inline void push64(__VM_STD_INSTR_ARGS_PROTO, QWORD val);

#endif
