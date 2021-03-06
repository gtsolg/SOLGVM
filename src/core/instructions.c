#include "..\..\include\core\instructions.h"

#include <intrin.h>

#include "..\..\include\external_api.h"
#include "..\..\include\errors.h"

// unknown register error
#define __CHECK_REG(REGN)   \
    if (REGN > REGSIZE - 1) \
        return UNKNOWN_REG;

// use call/jmp instead or unknown segment
#define __CHECK_SEG(SEG_OP)                      \
    if (SEG_OP == CS_OP || SEG_OP > SEGSIZE - 1) \
    {                                            \
        if (SEG_OP == CS_OP)                     \
            return WRONG_SEG;                    \
        return UNKNOWN_SEG;                      \
    }

// pointer out of memory range 
#define __CHECK_MEM(PTR)     \
    if (PTR > mem->size - 1) \
        return PTR_MEM_RANGE;

// instruction pointer out of memory range
#define __CHECK_IP                \
    if (core->IP > mem->size - 1) \
        return IP_MEM_RANGE;

// stack pointer out of memory range
#define __CHECK_SP                \
    if (core->SP > mem->size - 1) \
        return SP_MEM_RANGE;

#define __CHECK_DIVISOR(DIVISOR) \
    if (DIVISOR == 0)            \
        return DIV_BY_ZERO;

#define __SWAP(A, B, TMP_TYPE) \
    TMP_TYPE TMP = A;          \
    A = B;                     \
    B = TMP

// sets ZF to 1 if result = 0, else zf = 0
#define __SET_ZF(RESULT)     \
    if (RESULT == 0)         \
        SET_FLAG_1(ZF_MASK); \
    else                     \
        SET_FLAG_0(ZF_MASK)

// result = first + second
// if result overflowed then cf is set to 1, else cf = 0
#define __SET_CF_ADD(FIRST, RESULT) \
    if (RESULT < FIRST)             \
        SET_FLAG_1(CF_MASK);        \
    else                            \
        SET_FLAG_0(CF_MASK)

#define __SET_CF_SUB(FIRST, RESULT) \
    if (RESULT > FIRST)             \
        SET_FLAG_1(CF_MASK);        \
    else                            \
        SET_FLAG_0(CF_MASK)

// gets bit at bit_pos position
#define __GET_BIT(UNSIGNED, BIT_POS) (((UNSIGNED) >> (BIT_POS)) & 1)

// gets most left bit of value
#define __SIGN_BIT(UNSIGNED) ((UNSIGNED) >> (sizeof(UNSIGNED) * 8 - 1))

// result = first + second
// sets of = 1 if result is neg while first & sec is pos
// or result is pos while first & sec is neg
#define __SET_OF_ADD(F, S, R)            \
    if (__SIGN_BIT(F) == __SIGN_BIT(S)   \
      && __SIGN_BIT(F) != __SIGN_BIT(R)) \
        SET_FLAG_1(OF_MASK);             \
    else                                 \
        SET_FLAG_0(OF_MASK)

#define __SET_OF_SUB(F, S, R)            \
    if (__SIGN_BIT(F) != __SIGN_BIT(S)   \
      && __SIGN_BIT(F) != __SIGN_BIT(R)) \
        SET_FLAG_1(OF_MASK);             \
    else                                 \
        SET_FLAG_0(OF_MASK)

// sign flag = sign bit of result
#define __SET_SF(RESULT)         \
    if (__SIGN_BIT(RESULT) == 1) \
        SET_FLAG_1(SF_MASK);     \
    else                         \
        SET_FLAG_0(SF_MASK)

// if upper half of the result = 0 then cf/of = 0, else 1
#define __SET_CFOF_MUL(F, S, R)              \
    if (S != 0 && R / S != R)                \
    {                                        \
        SET_FLAG_1(CF_MASK);                 \
        SET_FLAG_1(OF_MASK);                 \
    }                                        \
    else                                     \
    {                                        \
        SET_FLAG_0(CF_MASK);                 \
        SET_FLAG_0(OF_MASK);                 \
    }       

// max shift mask
#define __SH8_MASK  0x7
#define __SH16_MASK 0xF
#define __SH32_MASK 0x1F
#define __SH64_MASK 0x3F

#define __CHECK_SHIFT(SHIFT) \
    if (SHIFT == 0)          \
        return NO_ERR;

// shift = shift & shift_mask
// result = dest shl/shr shift
// cf = last carried out bit of dest
#define __SET_CF_SL(FIRST, SHIFT)                           \
    if (__GET_BIT(FIRST, (sizeof(FIRST) * 8 - SHIFT)) != 0) \
        SET_FLAG_1(CF_MASK);                                \
    else                                                    \
        SET_FLAG_0(CF_MASK)

#define __SET_CF_SR(FIRST, SHIFT)        \
    if(__GET_BIT(FIRST, SHIFT - 1) != 0) \
        SET_FLAG_1(CF_MASK);             \
    else                                 \
        SET_FLAG_0(CF_MASK)

// of is affected only for 1-bits shift
// of = sign_bit(result) xor cf
#define __SET_OF_SL(SHIFT, RESULT)                                     \
    if (SHIFT == 1)                                                    \
        if (((__SIGN_BIT(RESULT)) ^ (GET_FLAG(CF_MASK, CF_POS))) != 0) \
            SET_FLAG_1(OF_MASK);                                       \
        else                                                           \
            SET_FLAG_0(OF_MASK)

#define __SET_OF_SHR(FIRST)     \
    if (__SIGN_BIT(FIRST) == 1) \
        SET_FLAG_1(OF_MASK);    \
    else                        \
        SET_FLAG_0(OF_MASK)

#define __ROL(VAL, SHIFT) (VAL << (SHIFT)) | (VAL >> (sizeof(VAL) * 8 - (SHIFT)))
#define __ROR(VAL, SHIFT) __ROL(VAL, sizeof(VAL) * 8 - SHIFT)

__VM_STD_INSTR(nop)
{
    return NO_ERR;
}

__VM_STD_INSTR(halt)
{
    return HALT;
}

__VM_STD_INSTR(mov8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BREG(dst) = BREG(src);
    return NO_ERR;
}

__VM_STD_INSTR(mov8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BREG(dst) = imm;
    return NO_ERR;
}

__VM_STD_INSTR(mov8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BREG(dst) = __PTR_VAL(ptr, BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(mov8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BREG(dst) = __PTR_VAL(imm, BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(mov8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    __PTR_VAL(imm, BYTE) = BREG(src);
    return NO_ERR;
}

__VM_STD_INSTR(mov8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    __PTR_VAL(dst, BYTE) = imm;
    return NO_ERR;
}

__VM_STD_INSTR(mov8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = BREG(src);
    return NO_ERR;
}

__VM_STD_INSTR(mov8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = imm;
    return NO_ERR;
}

__VM_STD_INSTR(mov8_segimm_reg)
{
    seg_op seg = __READ_IP(seg_op);
    DWORD  off = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_SEG(seg);
#endif

    DWORD ptr = SEGREG(seg) + off;

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = BREG(src);
    return NO_ERR;
}

__VM_STD_INSTR(mov8_segimm_imm)
{
    seg_op seg = __READ_IP(seg_op);
    DWORD  off = __READ_IP(DWORD);
    BYTE   src = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_SEG(seg);
#endif

    DWORD ptr = SEGREG(seg) + off;

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = src;
    return NO_ERR;
}

__VM_STD_INSTR(mov8_reg_segimm)
{
    reg_op dst = __READ_IP(reg_op);
    seg_op seg = __READ_IP(seg_op);
    DWORD  off = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_SEG(seg);
#endif

    DWORD ptr = SEGREG(seg) + off;

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BREG(dst) = __PTR_VAL(ptr, BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(mov8_segreg_reg)
{
    seg_op seg = __READ_IP(seg_op);
    reg_op off = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(off);
    __CHECK_REG(src);
    __CHECK_SEG(seg);
#endif

    DWORD ptr = SEGREG(seg) + BREG(off);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = BREG(src);
    return NO_ERR;
}

__VM_STD_INSTR(mov8_segreg_imm)
{
    seg_op seg = __READ_IP(seg_op);
    reg_op off = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(off);
    __CHECK_SEG(seg);
#endif

    DWORD ptr = SEGREG(seg) + BREG(off);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = imm;
    return NO_ERR;
}

__VM_STD_INSTR(mov8_reg_segreg)
{
    reg_op dst = __READ_IP(reg_op);
    seg_op seg = __READ_IP(seg_op);
    reg_op off = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(off);
    __CHECK_SEG(seg);
#endif

    DWORD ptr = SEGREG(seg) + BREG(off);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BREG(dst) = __PTR_VAL(ptr, BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(mov16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_segimm_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_segimm_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_reg_segimm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_segreg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_segreg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov16_reg_segreg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_segimm_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_segimm_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_reg_segimm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_segreg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_segreg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_reg_segreg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_reg_sreg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov32_sreg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_segimm_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_segimm_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_reg_segimm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_segreg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_segreg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mov64_reg_segreg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    __SWAP(BREG(src), BREG(dst), BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(xchg8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    __SWAP(BREG(dst), __PTR_VAL(imm, BYTE), BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(xchg8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __SWAP(BREG(dst), __PTR_VAL(ptr, BYTE), BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(xchg8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    __SWAP(BREG(src), __PTR_VAL(imm, BYTE), BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(xchg8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __SWAP(BREG(src), __PTR_VAL(ptr, BYTE), BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(xchg16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xchg64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(push8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    __PUSH(BREG(src), BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_SP;
#endif

    return NO_ERR;
}

__VM_STD_INSTR(push8_imm)
{
    BYTE imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
#endif

    __PUSH(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_SP;
#endif

    return NO_ERR;
}

__VM_STD_INSTR(push8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PUSH(__PTR_VAL(ptr, BYTE), BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_SP;
#endif

    return NO_ERR;
}

__VM_STD_INSTR(push8_iptr)
{
    DWORD ptr = __READ_IP(DWORD);
    __CHECK_MEM(ptr);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    __PUSH(__PTR_VAL(ptr, BYTE), BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_SP;
#endif

    return NO_ERR;
}

__VM_STD_INSTR(push16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(push16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(push16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(push16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(push16_flags)
{
    return NO_ERR;
}

__VM_STD_INSTR(push16_eflags)
{
    return NO_ERR;
}

__VM_STD_INSTR(push32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(push32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(push32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(push32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(push32_seg)
{
    return NO_ERR;
}

__VM_STD_INSTR(push64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(push64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(push64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(push64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif
    
    BREG(src) = __POP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_SP;
#endif

    return NO_ERR;
}

__VM_STD_INSTR(pop8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = __POP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_SP;
#endif

    return NO_ERR;
}

__VM_STD_INSTR(pop8_iptr)
{
    DWORD ptr = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = __POP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_SP;
#endif

    return NO_ERR;
}

__VM_STD_INSTR(pop16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop16_flags)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop16_eflags)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop32_seg)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(pop64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(movsx16_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    (int16_t)WREG(dst) = (int8_t)BREG(src);
    return NO_ERR;
}

__VM_STD_INSTR(movsx16_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    (int16_t)WREG(dst) = __PTR_VAL(ptr, int8_t);
    return NO_ERR;
}

__VM_STD_INSTR(movsx16_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    (int16_t)WREG(dst) = __PTR_VAL(imm, int8_t);
    return NO_ERR;
}

__VM_STD_INSTR(movsx32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(movsx32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(movsx32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(movsx64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(movsx64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(movsx64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(movzx16_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    WREG(dst) = BREG(src);
    return NO_ERR;
}

__VM_STD_INSTR(movzx16_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    WREG(dst) = __PTR_VAL(ptr, BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(movzx16_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    WREG(dst) = __PTR_VAL(imm, BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(movzx32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(movzx32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(movzx32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(movzx64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(movzx64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(movzx64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(add8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE first  = BREG(dst);
    BYTE second = BREG(src);
    BYTE result = first + second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_ADD(first, result);
    __SET_OF_ADD(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(add8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE first = BREG(dst);
    BYTE result = first + imm;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_ADD(first, result);
    __SET_OF_ADD(first, imm, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(add8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first  = BREG(dst);
    BYTE second = __PTR_VAL(ptr, BYTE);
    BYTE result = first + second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_ADD(first, result);
    __SET_OF_ADD(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(add8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE first  = BREG(dst);
    BYTE second = __PTR_VAL(imm, BYTE);
    BYTE result = first + second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_ADD(first, result);
    __SET_OF_ADD(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(add8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE first  = __PTR_VAL(imm, BYTE);
    BYTE second = BREG(src);
    BYTE result = first + second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_ADD(first, result);
    __SET_OF_ADD(first, second, result);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(add8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first + imm;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_ADD(first, result);
    __SET_OF_ADD(first, imm, result);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(add8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first  = __PTR_VAL(ptr, BYTE);
    BYTE second = BREG(src);
    BYTE result = first + second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_ADD(first, result);
    __SET_OF_ADD(first, second, result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(add8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first + imm;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_ADD(first, result);
    __SET_OF_ADD(first, imm, result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(add16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(add16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(add16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(add16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(add16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(add32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(add32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(add32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(add32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(add32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(add64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(add64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(add64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(add64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(add64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(add64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE first = BREG(dst);
    BYTE second = BREG(src);
    BYTE result = first - second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_SUB(first, result);
    __SET_OF_SUB(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sub8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE first = BREG(dst);
    BYTE result = first - imm;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_SUB(first, result);
    __SET_OF_SUB(first, imm, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sub8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first = BREG(dst);
    BYTE second = __PTR_VAL(ptr, BYTE);
    BYTE result = first - second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_SUB(first, result);
    __SET_OF_SUB(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sub8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE first = BREG(dst);
    BYTE second = __PTR_VAL(imm, BYTE);
    BYTE result = first - second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_SUB(first, result);
    __SET_OF_SUB(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sub8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE first = __PTR_VAL(imm, BYTE);
    BYTE second = BREG(src);
    BYTE result = first - second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_SUB(first, result);
    __SET_OF_SUB(first, second, result);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sub8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first - imm;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_SUB(first, result);
    __SET_OF_SUB(first, imm, result);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sub8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE second = BREG(src);
    BYTE result = first - second;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_SUB(first, result);
    __SET_OF_SUB(first, second, result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sub8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first - imm;
    __SET_ZF(result);
    __SET_SF(result);
    __SET_CF_SUB(first, result);
    __SET_OF_SUB(first, imm, result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sub16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sub64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE first = BREG(dst);
    BYTE second = BREG(src);
    BYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(mul8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE first = BREG(dst);
    BYTE result = first * imm;
    __SET_CFOF_MUL(first, imm, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(mul8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first = BREG(dst);
    BYTE second = __PTR_VAL(ptr, BYTE);
    BYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(mul8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE first = BREG(dst);
    BYTE second = __PTR_VAL(imm, BYTE);
    BYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(mul8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE first = __PTR_VAL(imm, BYTE);
    BYTE second = BREG(src);
    BYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(mul8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first * imm;
    __SET_CFOF_MUL(first, imm, result);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(mul8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE second = BREG(src);
    BYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(mul8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first * imm;
    __SET_CFOF_MUL(first, imm, result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(mul16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mul64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    SBYTE first = BREG(dst);
    SBYTE second = BREG(src);
    SBYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(imul8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    SBYTE  imm = __READ_IP(SBYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    SBYTE first = BREG(dst);
    SBYTE result = first * imm;
    __SET_CFOF_MUL(first, imm, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(imul8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    SBYTE first = BREG(dst);
    SBYTE second = __PTR_VAL(ptr, SBYTE);
    SBYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(imul8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    SBYTE first = BREG(dst);
    SBYTE second = __PTR_VAL(imm, SBYTE);
    SBYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(imul8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    SBYTE first = __PTR_VAL(imm, SBYTE);
    SBYTE second = BREG(src);
    SBYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    __PTR_VAL(imm, SBYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(imul8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    SBYTE imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    SBYTE first = __PTR_VAL(dst, SBYTE);
    SBYTE result = first * imm;
    __SET_CFOF_MUL(first, imm, result);

    __PTR_VAL(dst, SBYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(imul8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    SBYTE first = __PTR_VAL(ptr, SBYTE);
    SBYTE second = BREG(src);
    SBYTE result = first * second;
    __SET_CFOF_MUL(first, second, result);

    __PTR_VAL(ptr, SBYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(imul8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    SBYTE  imm = __READ_IP(SBYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    SBYTE first = __PTR_VAL(ptr, SBYTE);
    SBYTE result = first * imm;
    __SET_CFOF_MUL(first, imm, result);

    __PTR_VAL(ptr, SBYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(imul16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imul64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    BREG(dst) = BREG(dst) / second;
    return NO_ERR;
}

__VM_STD_INSTR(div8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    __CHECK_DIVISOR(imm);

    BREG(dst) = BREG(dst) / imm;
    return NO_ERR;
}

__VM_STD_INSTR(div8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE second = __PTR_VAL(ptr, BYTE);
    __CHECK_DIVISOR(second);

    BREG(dst) = BREG(dst) / second;
    return NO_ERR;
}

__VM_STD_INSTR(div8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE second = __PTR_VAL(imm, BYTE);
    __CHECK_DIVISOR(second);

    BREG(dst) = BREG(dst) / second;
    return NO_ERR;
}

__VM_STD_INSTR(div8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    __PTR_VAL(imm, BYTE) = __PTR_VAL(imm, BYTE) / second;
    return NO_ERR;
}

__VM_STD_INSTR(div8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    __CHECK_DIVISOR(imm);

    __PTR_VAL(dst, BYTE) = __PTR_VAL(dst, BYTE) / imm;
    return NO_ERR;
}

__VM_STD_INSTR(div8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    __PTR_VAL(ptr, BYTE) = __PTR_VAL(ptr, BYTE) / second;
    return NO_ERR;
}

__VM_STD_INSTR(div8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __CHECK_DIVISOR(imm);

    __PTR_VAL(ptr, BYTE) = __PTR_VAL(ptr, BYTE) / imm;
    return NO_ERR;
}

__VM_STD_INSTR(div16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(div16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(div16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(div32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(div32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(div64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(div64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(div64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(div64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    SBYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    BREG(dst) = (SBYTE)BREG(dst) / second;
    return NO_ERR;
}

__VM_STD_INSTR(idiv8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    SBYTE  imm = __READ_IP(SBYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    __CHECK_DIVISOR(imm);

    BREG(dst) = (SBYTE)BREG(dst) / imm;
    return NO_ERR;
}

__VM_STD_INSTR(idiv8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    SBYTE second = __PTR_VAL(ptr, SBYTE);
    __CHECK_DIVISOR(second);

    BREG(dst) = (SBYTE)BREG(dst) / second;
    return NO_ERR;
}

__VM_STD_INSTR(idiv8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    SBYTE second = __PTR_VAL(imm, SBYTE);
    __CHECK_DIVISOR(second);

    BREG(dst) = (SBYTE)BREG(dst) / second;
    return NO_ERR;
}

__VM_STD_INSTR(idiv8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    SBYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    __PTR_VAL(imm, SBYTE) = __PTR_VAL(imm, SBYTE) / second;
    return NO_ERR;
}

__VM_STD_INSTR(idiv8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    SBYTE imm = __READ_IP(SBYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    __CHECK_DIVISOR(imm);

    __PTR_VAL(dst, SBYTE) = __PTR_VAL(dst, SBYTE) / imm;
    return NO_ERR;
}

__VM_STD_INSTR(idiv8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    SBYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    __PTR_VAL(ptr, SBYTE) = __PTR_VAL(ptr, SBYTE) / second;
    return NO_ERR;
}

__VM_STD_INSTR(idiv8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    SBYTE  imm = __READ_IP(SBYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __CHECK_DIVISOR(imm);

    __PTR_VAL(ptr, SBYTE) = __PTR_VAL(ptr, SBYTE) / imm;
    return NO_ERR;
}

__VM_STD_INSTR(idiv16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(idiv64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    BREG(dst) = BREG(dst) % second;
    return NO_ERR;
}

__VM_STD_INSTR(mod8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    __CHECK_DIVISOR(imm);

    BREG(dst) = BREG(dst) % imm;
    return NO_ERR;
}

__VM_STD_INSTR(mod8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE second = __PTR_VAL(ptr, BYTE);
    __CHECK_DIVISOR(second);

    BREG(dst) = BREG(dst) % second;
    return NO_ERR;
}

__VM_STD_INSTR(mod8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE second = __PTR_VAL(imm, BYTE);
    __CHECK_DIVISOR(second);

    BREG(dst) = BREG(dst) % second;
    return NO_ERR;
}

__VM_STD_INSTR(mod8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    __PTR_VAL(imm, BYTE) = __PTR_VAL(imm, BYTE) % second;
    return NO_ERR;
}

__VM_STD_INSTR(mod8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    __CHECK_DIVISOR(imm);

    __PTR_VAL(dst, BYTE) = __PTR_VAL(dst, BYTE) % imm;
    return NO_ERR;
}

__VM_STD_INSTR(mod8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    __PTR_VAL(ptr, BYTE) = __PTR_VAL(ptr, BYTE) % second;
    return NO_ERR;
}

__VM_STD_INSTR(mod8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __CHECK_DIVISOR(imm);

    __PTR_VAL(ptr, BYTE) = __PTR_VAL(ptr, BYTE) % imm;
    return NO_ERR;
}

__VM_STD_INSTR(mod16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(mod64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    SBYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    BREG(dst) = (SBYTE)BREG(dst) % second;
    return NO_ERR;
}

__VM_STD_INSTR(imod8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    SBYTE  imm = __READ_IP(SBYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    __CHECK_DIVISOR(imm);

    BREG(dst) = (SBYTE)BREG(dst) % imm;
    return NO_ERR;
}

__VM_STD_INSTR(imod8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    SBYTE second = __PTR_VAL(ptr, SBYTE);
    __CHECK_DIVISOR(second);

    BREG(dst) = (SBYTE)BREG(dst) % second;
    return NO_ERR;
}

__VM_STD_INSTR(imod8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    SBYTE second = __PTR_VAL(imm, SBYTE);
    __CHECK_DIVISOR(second);

    BREG(dst) = (SBYTE)BREG(dst) % second;
    return NO_ERR;
}

__VM_STD_INSTR(imod8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    SBYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    __PTR_VAL(imm, SBYTE) = __PTR_VAL(imm, SBYTE) % second;
    return NO_ERR;
}

__VM_STD_INSTR(imod8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    SBYTE imm = __READ_IP(SBYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    __CHECK_DIVISOR(imm);

    __PTR_VAL(dst, SBYTE) = __PTR_VAL(dst, SBYTE) % imm;
    return NO_ERR;
}

__VM_STD_INSTR(imod8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    SBYTE second = BREG(src);
    __CHECK_DIVISOR(second);

    __PTR_VAL(ptr, SBYTE) = __PTR_VAL(ptr, SBYTE) % second;
    return NO_ERR;
}

__VM_STD_INSTR(imod8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    SBYTE  imm = __READ_IP(SBYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __CHECK_DIVISOR(imm);

    __PTR_VAL(ptr, SBYTE) = __PTR_VAL(ptr, SBYTE) % imm;
    return NO_ERR;
}

__VM_STD_INSTR(imod16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(imod64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE result = BREG(dst) & BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(and8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE result = BREG(dst) & imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(and8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = BREG(dst) & __PTR_VAL(ptr, BYTE);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(and8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE result = BREG(dst) & __PTR_VAL(imm, BYTE);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(and8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE result = __PTR_VAL(imm, BYTE) & BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(and8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE result = __PTR_VAL(dst, BYTE) & imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(and8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = __PTR_VAL(ptr, BYTE) & BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(and8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = __PTR_VAL(ptr, BYTE) & imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(and16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(and16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(and16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(and32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(and32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(and64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(and64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(and64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(and64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE result = BREG(dst) | BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(or8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE result = BREG(dst) | imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(or8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = BREG(dst) | __PTR_VAL(ptr, BYTE);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(or8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE result = BREG(dst) | __PTR_VAL(imm, BYTE);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(or8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE result = __PTR_VAL(imm, BYTE) | BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(or8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE result = __PTR_VAL(dst, BYTE) | imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(or8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = __PTR_VAL(ptr, BYTE) | BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(or8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = __PTR_VAL(ptr, BYTE) | imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(or16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(or16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(or16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(or32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(or32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(or64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(or64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(or64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(or64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(not8_reg)
{
    reg_op dst = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BREG(dst) = ~BREG(dst);
    return NO_ERR;
}

__VM_STD_INSTR(not8_iptr)
{
    DWORD ptr = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = ~__PTR_VAL(ptr, BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(not8_rptr)
{
    reg_op dst = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    __PTR_VAL(ptr, BYTE) = ~__PTR_VAL(ptr, BYTE);
    return NO_ERR;
}

__VM_STD_INSTR(not16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(not16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(not16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(not32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(not32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(not32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(not64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(not64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(not64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE result = BREG(dst) ^ BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(xor8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE result = BREG(dst) ^ imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(xor8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = BREG(dst) ^ __PTR_VAL(ptr, BYTE);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(xor8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE result = BREG(dst) ^ __PTR_VAL(imm, BYTE);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(xor8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE result = __PTR_VAL(imm, BYTE) ^ BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(xor8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE result = __PTR_VAL(dst, BYTE) ^ imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(xor8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = __PTR_VAL(ptr, BYTE) ^ BREG(src);
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(xor8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE result = __PTR_VAL(ptr, BYTE) ^ imm;
    SET_FLAG_0(OF_MASK);
    SET_FLAG_0(CF_MASK);
    __SET_ZF(result);
    __SET_SF(result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(xor16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(xor64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shl8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shl8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = __PTR_VAL(ptr, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shl8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE shift = __PTR_VAL(imm, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shl8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(imm, BYTE);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shl8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shl8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shl8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    __PTR_VAL(ptr, BYTE) = imm;
    return NO_ERR;
}

__VM_STD_INSTR(shl16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shl64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    __SET_OF_SHR(first);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shr8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    __SET_OF_SHR(first);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shr8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = __PTR_VAL(ptr, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    __SET_OF_SHR(first);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shr8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE shift = __PTR_VAL(imm, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    __SET_OF_SHR(first);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shr8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(imm, BYTE);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    __SET_OF_SHR(first);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shr8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    __SET_OF_SHR(first);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shr8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    __SET_OF_SHR(first);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shr8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    __SET_OF_SHR(first);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(shr16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(shr64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sal8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sal8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = __PTR_VAL(ptr, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sal8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE shift = __PTR_VAL(imm, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sal8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(imm, BYTE);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sal8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sal8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sal8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first << shift;
    __SET_CF_SL(first, shift);
    __SET_OF_SL(shift, result);

    __PTR_VAL(ptr, BYTE) = imm;
    return NO_ERR;
}

__VM_STD_INSTR(sal16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sal64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    SET_FLAG_0(OF_MASK);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sar8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    SET_FLAG_0(OF_MASK);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sar8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = __PTR_VAL(ptr, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    SET_FLAG_0(OF_MASK);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sar8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE shift = __PTR_VAL(imm, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    SET_FLAG_0(OF_MASK);

    BREG(dst) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sar8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(imm, BYTE);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    SET_FLAG_0(OF_MASK);

    __PTR_VAL(imm, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sar8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    SET_FLAG_0(OF_MASK);

    __PTR_VAL(dst, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sar8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    SET_FLAG_0(OF_MASK);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sar8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first >> shift;
    __SET_CF_SR(first, shift);
    SET_FLAG_0(OF_MASK);

    __PTR_VAL(ptr, BYTE) = result;
    return NO_ERR;
}

__VM_STD_INSTR(sar16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(sar64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);

    BREG(dst) = __ROL(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(rol8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);

    BREG(dst) = __ROL(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(rol8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = __PTR_VAL(ptr, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;

    BREG(dst) = __ROL(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(rol8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE shift = __PTR_VAL(imm, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;

    BREG(dst) = __ROL(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(rol8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(imm, BYTE);
    BYTE result = first << shift;

    __PTR_VAL(imm, BYTE) = __ROL(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(rol8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first << shift;

    __PTR_VAL(dst, BYTE) = __ROL(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(rol8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first << shift;

    __PTR_VAL(ptr, BYTE) = __ROL(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(rol8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first << shift;

    __PTR_VAL(ptr, BYTE) = __ROL(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(rol16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(rol64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror8_reg_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);

    BREG(dst) = __ROR(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(ror8_reg_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);

    BREG(dst) = __ROR(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(ror8_reg_rptr)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = __PTR_VAL(ptr, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;

    BREG(dst) = __ROR(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(ror8_reg_iptr)
{
    reg_op dst = __READ_IP(reg_op);
    DWORD  imm = __READ_IP(DWORD);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_MEM(imm);
#endif

    BYTE shift = __PTR_VAL(imm, BYTE) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = BREG(dst);
    BYTE result = first << shift;

    BREG(dst) = __ROR(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(ror8_iptr_reg)
{
    DWORD  imm = __READ_IP(DWORD);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
    __CHECK_MEM(imm);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(imm, BYTE);
    BYTE result = first << shift;

    __PTR_VAL(imm, BYTE) = __ROR(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(ror8_iptr_imm)
{
    DWORD dst = __READ_IP(DWORD);
    BYTE  imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(dst);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(dst, BYTE);
    BYTE result = first << shift;

    __PTR_VAL(dst, BYTE) = __ROR(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(ror8_rptr_reg)
{
    reg_op dst = __READ_IP(reg_op);
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = BREG(src) & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first << shift;

    __PTR_VAL(ptr, BYTE) = __ROR(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(ror8_rptr_imm)
{
    reg_op dst = __READ_IP(reg_op);
    BYTE   imm = __READ_IP(BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(dst);
#endif

    DWORD ptr = BREG(dst);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    BYTE shift = imm & __SH8_MASK;
    __CHECK_SHIFT(shift);
    BYTE first = __PTR_VAL(ptr, BYTE);
    BYTE result = first << shift;

    __PTR_VAL(ptr, BYTE) = __ROR(first, shift);
    return NO_ERR;
}

__VM_STD_INSTR(ror16_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror16_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror16_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror16_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror16_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror16_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror16_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror16_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror32_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror32_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror32_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror32_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror32_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror32_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror32_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror32_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror64_reg_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror64_reg_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror64_reg_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror64_reg_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror64_iptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror64_iptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror64_rptr_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(ror64_rptr_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);
    
#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    core->IP = ptr;
    return NO_ERR;
}

__VM_STD_INSTR(jmp8_imm)
{
    BYTE ptr = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    core->IP = ptr;
    return NO_ERR;
}

__VM_STD_INSTR(jmp8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = __PTR_VAL(src, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    core->IP = ptr;
    return NO_ERR;
}

__VM_STD_INSTR(jmp8_iptr)
{
    reg_op imm = __READ_IP(reg_op);
    DWORD  ptr = __PTR_VAL(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    core->IP = ptr;
    return NO_ERR;
}

__VM_STD_INSTR(jmp16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jmp64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(je8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) == 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(je8_imm)
{
    BYTE ptr = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) == 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(je8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = __PTR_VAL(src, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) == 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(je8_iptr)
{
    reg_op imm = __READ_IP(reg_op);
    DWORD  ptr = __PTR_VAL(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) == 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(je16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(je16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(je16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(je16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(je32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(je32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(je32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(je32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(je64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(je64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(je64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(je64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) != 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jne8_imm)
{
    BYTE ptr = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) != 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jne8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = __PTR_VAL(src, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) != 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jne8_iptr)
{
    reg_op imm = __READ_IP(reg_op);
    DWORD  ptr = __PTR_VAL(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) != 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jne16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jne64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) == 0
        && GET_FLAG(SF_MASK, SF_POS) == GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jg8_imm)
{
    BYTE ptr = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) == 0
        && GET_FLAG(SF_MASK, SF_POS) == GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jg8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = __PTR_VAL(src, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) == 0
        && GET_FLAG(SF_MASK, SF_POS) == GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jg8_iptr)
{
    reg_op imm = __READ_IP(reg_op);
    DWORD  ptr = __PTR_VAL(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) == 0
        && GET_FLAG(SF_MASK, SF_POS) == GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jg16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jg64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(SF_MASK, SF_POS) == GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jge8_imm)
{
    BYTE ptr = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(SF_MASK, SF_POS) == GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jge8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = __PTR_VAL(src, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(SF_MASK, SF_POS) == GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jge8_iptr)
{
    reg_op imm = __READ_IP(reg_op);
    DWORD  ptr = __PTR_VAL(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(SF_MASK, SF_POS) == GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jge16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jge64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(SF_MASK, SF_POS) != GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jl8_imm)
{
    BYTE ptr = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(SF_MASK, SF_POS) != GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jl8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = __PTR_VAL(src, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(SF_MASK, SF_POS) != GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jl8_iptr)
{
    reg_op imm = __READ_IP(reg_op);
    DWORD  ptr = __PTR_VAL(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(SF_MASK, SF_POS) != GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jl16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jl64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) != 0
        || GET_FLAG(SF_MASK, SF_POS) != GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jle8_imm)
{
    BYTE ptr = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) != 0
        || GET_FLAG(SF_MASK, SF_POS) != GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jle8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = __PTR_VAL(src, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) != 0
        || GET_FLAG(SF_MASK, SF_POS) != GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jle8_iptr)
{
    reg_op imm = __READ_IP(reg_op);
    DWORD  ptr = __PTR_VAL(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(ZF_MASK, ZF_POS) != 0
        || GET_FLAG(SF_MASK, SF_POS) != GET_FLAG(OF_MASK, OF_POS))
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jle16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jle64_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo8_reg)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = BREG(src);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(OF_MASK, OF_POS) != 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jo8_imm)
{
    BYTE ptr = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(OF_MASK, OF_POS) != 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jo8_rptr)
{
    reg_op src = __READ_IP(reg_op);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_REG(src);
#endif

    DWORD ptr = __PTR_VAL(src, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(OF_MASK, OF_POS) != 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jo8_iptr)
{
    reg_op imm = __READ_IP(reg_op);
    DWORD  ptr = __PTR_VAL(imm, BYTE);

#ifndef VM_INSTRUCTIONS_NO_CHECK
    __CHECK_IP;
    __CHECK_MEM(ptr);
#endif

    if (GET_FLAG(OF_MASK, OF_POS) != 0)
    {
        core->IP = ptr;
    }
    return NO_ERR;
}

__VM_STD_INSTR(jo16_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo16_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo16_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo16_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo32_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo32_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo32_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo32_iptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo64_reg)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo64_imm)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo64_rptr)
{
    return NO_ERR;
}

__VM_STD_INSTR(jo64_iptr)
{
    return NO_ERR;
}
