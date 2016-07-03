#include "..\include\external_api.h"

inline BYTE   
read8_IP(__VM_STD_INSTR_ARGS_PROTO)
{
    BYTE result = __READ_IP(BYTE);
    return result;
}

inline WORD 
read16_IP(__VM_STD_INSTR_ARGS_PROTO)
{
    WORD result = __READ_IP(WORD);
    return result;
}

inline DWORD
read32_IP(__VM_STD_INSTR_ARGS_PROTO)
{
    DWORD result = __READ_IP(DWORD);
    return result;
}

inline QWORD
read64_IP(__VM_STD_INSTR_ARGS_PROTO)
{
    QWORD result = __READ_IP(QWORD);
    return result;
}

inline BYTE
pop8(__VM_STD_INSTR_ARGS_PROTO)
{
    BYTE result = __POP(BYTE);
    return result;
}

inline WORD  
pop16(__VM_STD_INSTR_ARGS_PROTO)
{
    WORD result = __POP(WORD);
    return result;
}

inline DWORD
pop32(__VM_STD_INSTR_ARGS_PROTO)
{
    DWORD result = __POP(DWORD);
    return result;
}

inline QWORD
pop64(__VM_STD_INSTR_ARGS_PROTO)
{
    QWORD result = __POP(QWORD);
    return result;
}

inline void
push8(__VM_STD_INSTR_ARGS_PROTO, BYTE imm)
{
    __PUSH(imm, BYTE);
}

inline void
push16(__VM_STD_INSTR_ARGS_PROTO, WORD imm)
{
    __PUSH(imm, WORD);
}

inline void
push32(__VM_STD_INSTR_ARGS_PROTO, DWORD imm)
{
    __PUSH(imm, DWORD);
}

inline void
push64(__VM_STD_INSTR_ARGS_PROTO, QWORD imm)
{
    __PUSH(imm, QWORD);
}
