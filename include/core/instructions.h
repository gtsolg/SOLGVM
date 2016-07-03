#ifndef VM_INSTRUCTIONS_H
#define VM_INSTRUCTIONS_H

#include "vm_core.h"

#define __VM_STD_INSTR_PROTO(NAME) extern int NAME(__VM_STD_INSTR_ARGS_PROTO)
#define __VM_STD_INSTR(NAME) inline int NAME(__VM_STD_INSTR_ARGS_PROTO)

typedef int (*vm_instr_t)(__VM_STD_INSTR_ARGS_PROTO);

__VM_STD_INSTR_PROTO(nop);
__VM_STD_INSTR_PROTO(halt);

__VM_STD_INSTR_PROTO(mov8_reg_reg);
__VM_STD_INSTR_PROTO(mov8_reg_imm);
__VM_STD_INSTR_PROTO(mov8_reg_rptr);
__VM_STD_INSTR_PROTO(mov8_reg_iptr);
__VM_STD_INSTR_PROTO(mov8_iptr_reg);
__VM_STD_INSTR_PROTO(mov8_iptr_imm);
__VM_STD_INSTR_PROTO(mov8_rptr_reg);
__VM_STD_INSTR_PROTO(mov8_rptr_imm);
__VM_STD_INSTR_PROTO(mov8_segimm_reg);
__VM_STD_INSTR_PROTO(mov8_segimm_imm);
__VM_STD_INSTR_PROTO(mov8_reg_segimm);
__VM_STD_INSTR_PROTO(mov8_segreg_reg);
__VM_STD_INSTR_PROTO(mov8_segreg_imm);
__VM_STD_INSTR_PROTO(mov8_reg_segreg);
__VM_STD_INSTR_PROTO(mov16_reg_reg);
__VM_STD_INSTR_PROTO(mov16_reg_imm);
__VM_STD_INSTR_PROTO(mov16_reg_rptr);
__VM_STD_INSTR_PROTO(mov16_reg_iptr);
__VM_STD_INSTR_PROTO(mov16_iptr_reg);
__VM_STD_INSTR_PROTO(mov16_iptr_imm);
__VM_STD_INSTR_PROTO(mov16_rptr_reg);
__VM_STD_INSTR_PROTO(mov16_rptr_imm);
__VM_STD_INSTR_PROTO(mov16_segimm_reg);
__VM_STD_INSTR_PROTO(mov16_segimm_imm);
__VM_STD_INSTR_PROTO(mov16_reg_segimm);
__VM_STD_INSTR_PROTO(mov16_segreg_reg);
__VM_STD_INSTR_PROTO(mov16_segreg_imm);
__VM_STD_INSTR_PROTO(mov16_reg_segreg);
__VM_STD_INSTR_PROTO(mov32_reg_reg);
__VM_STD_INSTR_PROTO(mov32_reg_imm);
__VM_STD_INSTR_PROTO(mov32_reg_rptr);
__VM_STD_INSTR_PROTO(mov32_reg_iptr);
__VM_STD_INSTR_PROTO(mov32_iptr_reg);
__VM_STD_INSTR_PROTO(mov32_iptr_imm);
__VM_STD_INSTR_PROTO(mov32_rptr_reg);
__VM_STD_INSTR_PROTO(mov32_rptr_imm);
__VM_STD_INSTR_PROTO(mov32_segimm_reg);
__VM_STD_INSTR_PROTO(mov32_segimm_imm);
__VM_STD_INSTR_PROTO(mov32_reg_segimm);
__VM_STD_INSTR_PROTO(mov32_segreg_reg);
__VM_STD_INSTR_PROTO(mov32_segreg_imm);
__VM_STD_INSTR_PROTO(mov32_reg_segreg);
__VM_STD_INSTR_PROTO(mov32_reg_sreg);
__VM_STD_INSTR_PROTO(mov32_sreg_reg);
__VM_STD_INSTR_PROTO(mov64_reg_reg);
__VM_STD_INSTR_PROTO(mov64_reg_imm);
__VM_STD_INSTR_PROTO(mov64_reg_rptr);
__VM_STD_INSTR_PROTO(mov64_reg_iptr);
__VM_STD_INSTR_PROTO(mov64_iptr_reg);
__VM_STD_INSTR_PROTO(mov64_iptr_imm);
__VM_STD_INSTR_PROTO(mov64_rptr_reg);
__VM_STD_INSTR_PROTO(mov64_rptr_imm);
__VM_STD_INSTR_PROTO(mov64_segimm_reg);
__VM_STD_INSTR_PROTO(mov64_segimm_imm);
__VM_STD_INSTR_PROTO(mov64_reg_segimm);
__VM_STD_INSTR_PROTO(mov64_segreg_reg);
__VM_STD_INSTR_PROTO(mov64_segreg_imm);
__VM_STD_INSTR_PROTO(mov64_reg_segreg);



__VM_STD_INSTR_PROTO(xchg8_reg_reg);
__VM_STD_INSTR_PROTO(xchg8_reg_iptr);
__VM_STD_INSTR_PROTO(xchg8_reg_rptr);
__VM_STD_INSTR_PROTO(xchg8_iptr_reg);
__VM_STD_INSTR_PROTO(xchg8_rptr_reg);
__VM_STD_INSTR_PROTO(xchg16_reg_reg);
__VM_STD_INSTR_PROTO(xchg16_reg_iptr);
__VM_STD_INSTR_PROTO(xchg16_reg_rptr);
__VM_STD_INSTR_PROTO(xchg16_iptr_reg);
__VM_STD_INSTR_PROTO(xchg16_rptr_reg);
__VM_STD_INSTR_PROTO(xchg32_reg_reg);
__VM_STD_INSTR_PROTO(xchg32_reg_iptr);
__VM_STD_INSTR_PROTO(xchg32_reg_rptr);
__VM_STD_INSTR_PROTO(xchg32_iptr_reg);
__VM_STD_INSTR_PROTO(xchg32_rptr_reg);
__VM_STD_INSTR_PROTO(xchg64_reg_reg);
__VM_STD_INSTR_PROTO(xchg64_reg_iptr);
__VM_STD_INSTR_PROTO(xchg64_reg_rptr);
__VM_STD_INSTR_PROTO(xchg64_iptr_reg);
__VM_STD_INSTR_PROTO(xchg64_rptr_reg);



__VM_STD_INSTR_PROTO(push8_reg);
__VM_STD_INSTR_PROTO(push8_imm);
__VM_STD_INSTR_PROTO(push8_rptr);
__VM_STD_INSTR_PROTO(push8_iptr);
__VM_STD_INSTR_PROTO(push16_reg);
__VM_STD_INSTR_PROTO(push16_imm);
__VM_STD_INSTR_PROTO(push16_rptr);
__VM_STD_INSTR_PROTO(push16_iptr);
__VM_STD_INSTR_PROTO(push16_flags);
__VM_STD_INSTR_PROTO(push16_eflags);
__VM_STD_INSTR_PROTO(push32_reg);
__VM_STD_INSTR_PROTO(push32_imm);
__VM_STD_INSTR_PROTO(push32_rptr);
__VM_STD_INSTR_PROTO(push32_iptr);
__VM_STD_INSTR_PROTO(push32_seg);
__VM_STD_INSTR_PROTO(push64_reg);
__VM_STD_INSTR_PROTO(push64_imm);
__VM_STD_INSTR_PROTO(push64_rptr);
__VM_STD_INSTR_PROTO(push64_iptr);



__VM_STD_INSTR_PROTO(pop8_reg);
__VM_STD_INSTR_PROTO(pop8_rptr);
__VM_STD_INSTR_PROTO(pop8_iptr);
__VM_STD_INSTR_PROTO(pop16_reg);
__VM_STD_INSTR_PROTO(pop16_rptr);
__VM_STD_INSTR_PROTO(pop16_iptr);
__VM_STD_INSTR_PROTO(pop16_flags);
__VM_STD_INSTR_PROTO(pop16_eflags);
__VM_STD_INSTR_PROTO(pop32_reg);
__VM_STD_INSTR_PROTO(pop32_rptr);
__VM_STD_INSTR_PROTO(pop32_iptr);
__VM_STD_INSTR_PROTO(pop32_seg);
__VM_STD_INSTR_PROTO(pop64_reg);
__VM_STD_INSTR_PROTO(pop64_rptr);
__VM_STD_INSTR_PROTO(pop64_iptr);



__VM_STD_INSTR_PROTO(movsx16_reg_reg);
__VM_STD_INSTR_PROTO(movsx16_reg_rptr);
__VM_STD_INSTR_PROTO(movsx16_reg_iptr);
__VM_STD_INSTR_PROTO(movsx32_reg_reg);
__VM_STD_INSTR_PROTO(movsx32_reg_rptr);
__VM_STD_INSTR_PROTO(movsx32_reg_iptr);
__VM_STD_INSTR_PROTO(movsx64_reg_reg);
__VM_STD_INSTR_PROTO(movsx64_reg_rptr);
__VM_STD_INSTR_PROTO(movsx64_reg_iptr);



__VM_STD_INSTR_PROTO(movzx16_reg_reg);
__VM_STD_INSTR_PROTO(movzx16_reg_rptr);
__VM_STD_INSTR_PROTO(movzx16_reg_iptr);
__VM_STD_INSTR_PROTO(movzx32_reg_reg);
__VM_STD_INSTR_PROTO(movzx32_reg_rptr);
__VM_STD_INSTR_PROTO(movzx32_reg_iptr);
__VM_STD_INSTR_PROTO(movzx64_reg_reg);
__VM_STD_INSTR_PROTO(movzx64_reg_rptr);
__VM_STD_INSTR_PROTO(movzx64_reg_iptr);



__VM_STD_INSTR_PROTO(add8_reg_reg);
__VM_STD_INSTR_PROTO(add8_reg_imm);
__VM_STD_INSTR_PROTO(add8_reg_rptr);
__VM_STD_INSTR_PROTO(add8_reg_iptr);
__VM_STD_INSTR_PROTO(add8_iptr_reg);
__VM_STD_INSTR_PROTO(add8_iptr_imm);
__VM_STD_INSTR_PROTO(add8_rptr_reg);
__VM_STD_INSTR_PROTO(add8_rptr_imm);
__VM_STD_INSTR_PROTO(add16_reg_reg);
__VM_STD_INSTR_PROTO(add16_reg_imm);
__VM_STD_INSTR_PROTO(add16_reg_rptr);
__VM_STD_INSTR_PROTO(add16_reg_iptr);
__VM_STD_INSTR_PROTO(add16_iptr_reg);
__VM_STD_INSTR_PROTO(add16_iptr_imm);
__VM_STD_INSTR_PROTO(add16_rptr_reg);
__VM_STD_INSTR_PROTO(add16_rptr_imm);
__VM_STD_INSTR_PROTO(add32_reg_reg);
__VM_STD_INSTR_PROTO(add32_reg_imm);
__VM_STD_INSTR_PROTO(add32_reg_rptr);
__VM_STD_INSTR_PROTO(add32_reg_iptr);
__VM_STD_INSTR_PROTO(add32_iptr_reg);
__VM_STD_INSTR_PROTO(add32_iptr_imm);
__VM_STD_INSTR_PROTO(add32_rptr_reg);
__VM_STD_INSTR_PROTO(add32_rptr_imm);
__VM_STD_INSTR_PROTO(add64_reg_reg);
__VM_STD_INSTR_PROTO(add64_reg_imm);
__VM_STD_INSTR_PROTO(add64_reg_rptr);
__VM_STD_INSTR_PROTO(add64_reg_iptr);
__VM_STD_INSTR_PROTO(add64_iptr_reg);
__VM_STD_INSTR_PROTO(add64_iptr_imm);
__VM_STD_INSTR_PROTO(add64_rptr_reg);
__VM_STD_INSTR_PROTO(add64_rptr_imm);



__VM_STD_INSTR_PROTO(sub8_reg_reg);
__VM_STD_INSTR_PROTO(sub8_reg_imm);
__VM_STD_INSTR_PROTO(sub8_reg_rptr);
__VM_STD_INSTR_PROTO(sub8_reg_iptr);
__VM_STD_INSTR_PROTO(sub8_iptr_reg);
__VM_STD_INSTR_PROTO(sub8_iptr_imm);
__VM_STD_INSTR_PROTO(sub8_rptr_reg);
__VM_STD_INSTR_PROTO(sub8_rptr_imm);
__VM_STD_INSTR_PROTO(sub16_reg_reg);
__VM_STD_INSTR_PROTO(sub16_reg_imm);
__VM_STD_INSTR_PROTO(sub16_reg_rptr);
__VM_STD_INSTR_PROTO(sub16_reg_iptr);
__VM_STD_INSTR_PROTO(sub16_iptr_reg);
__VM_STD_INSTR_PROTO(sub16_iptr_imm);
__VM_STD_INSTR_PROTO(sub16_rptr_reg);
__VM_STD_INSTR_PROTO(sub16_rptr_imm);
__VM_STD_INSTR_PROTO(sub32_reg_reg);
__VM_STD_INSTR_PROTO(sub32_reg_imm);
__VM_STD_INSTR_PROTO(sub32_reg_rptr);
__VM_STD_INSTR_PROTO(sub32_reg_iptr);
__VM_STD_INSTR_PROTO(sub32_iptr_reg);
__VM_STD_INSTR_PROTO(sub32_iptr_imm);
__VM_STD_INSTR_PROTO(sub32_rptr_reg);
__VM_STD_INSTR_PROTO(sub32_rptr_imm);
__VM_STD_INSTR_PROTO(sub64_reg_reg);
__VM_STD_INSTR_PROTO(sub64_reg_imm);
__VM_STD_INSTR_PROTO(sub64_reg_rptr);
__VM_STD_INSTR_PROTO(sub64_reg_iptr);
__VM_STD_INSTR_PROTO(sub64_iptr_reg);
__VM_STD_INSTR_PROTO(sub64_iptr_imm);
__VM_STD_INSTR_PROTO(sub64_rptr_reg);
__VM_STD_INSTR_PROTO(sub64_rptr_imm);



__VM_STD_INSTR_PROTO(mul8_reg_reg);
__VM_STD_INSTR_PROTO(mul8_reg_imm);
__VM_STD_INSTR_PROTO(mul8_reg_rptr);
__VM_STD_INSTR_PROTO(mul8_reg_iptr);
__VM_STD_INSTR_PROTO(mul8_iptr_reg);
__VM_STD_INSTR_PROTO(mul8_iptr_imm);
__VM_STD_INSTR_PROTO(mul8_rptr_reg);
__VM_STD_INSTR_PROTO(mul8_rptr_imm);
__VM_STD_INSTR_PROTO(mul16_reg_reg);
__VM_STD_INSTR_PROTO(mul16_reg_imm);
__VM_STD_INSTR_PROTO(mul16_reg_rptr);
__VM_STD_INSTR_PROTO(mul16_reg_iptr);
__VM_STD_INSTR_PROTO(mul16_iptr_reg);
__VM_STD_INSTR_PROTO(mul16_iptr_imm);
__VM_STD_INSTR_PROTO(mul16_rptr_reg);
__VM_STD_INSTR_PROTO(mul16_rptr_imm);
__VM_STD_INSTR_PROTO(mul32_reg_reg);
__VM_STD_INSTR_PROTO(mul32_reg_imm);
__VM_STD_INSTR_PROTO(mul32_reg_rptr);
__VM_STD_INSTR_PROTO(mul32_reg_iptr);
__VM_STD_INSTR_PROTO(mul32_iptr_reg);
__VM_STD_INSTR_PROTO(mul32_iptr_imm);
__VM_STD_INSTR_PROTO(mul32_rptr_reg);
__VM_STD_INSTR_PROTO(mul32_rptr_imm);
__VM_STD_INSTR_PROTO(mul64_reg_reg);
__VM_STD_INSTR_PROTO(mul64_reg_imm);
__VM_STD_INSTR_PROTO(mul64_reg_rptr);
__VM_STD_INSTR_PROTO(mul64_reg_iptr);
__VM_STD_INSTR_PROTO(mul64_iptr_reg);
__VM_STD_INSTR_PROTO(mul64_iptr_imm);
__VM_STD_INSTR_PROTO(mul64_rptr_reg);
__VM_STD_INSTR_PROTO(mul64_rptr_imm);



__VM_STD_INSTR_PROTO(imul8_reg_reg);
__VM_STD_INSTR_PROTO(imul8_reg_imm);
__VM_STD_INSTR_PROTO(imul8_reg_rptr);
__VM_STD_INSTR_PROTO(imul8_reg_iptr);
__VM_STD_INSTR_PROTO(imul8_iptr_reg);
__VM_STD_INSTR_PROTO(imul8_iptr_imm);
__VM_STD_INSTR_PROTO(imul8_rptr_reg);
__VM_STD_INSTR_PROTO(imul8_rptr_imm);
__VM_STD_INSTR_PROTO(imul16_reg_reg);
__VM_STD_INSTR_PROTO(imul16_reg_imm);
__VM_STD_INSTR_PROTO(imul16_reg_rptr);
__VM_STD_INSTR_PROTO(imul16_reg_iptr);
__VM_STD_INSTR_PROTO(imul16_iptr_reg);
__VM_STD_INSTR_PROTO(imul16_iptr_imm);
__VM_STD_INSTR_PROTO(imul16_rptr_reg);
__VM_STD_INSTR_PROTO(imul16_rptr_imm);
__VM_STD_INSTR_PROTO(imul32_reg_reg);
__VM_STD_INSTR_PROTO(imul32_reg_imm);
__VM_STD_INSTR_PROTO(imul32_reg_rptr);
__VM_STD_INSTR_PROTO(imul32_reg_iptr);
__VM_STD_INSTR_PROTO(imul32_iptr_reg);
__VM_STD_INSTR_PROTO(imul32_iptr_imm);
__VM_STD_INSTR_PROTO(imul32_rptr_reg);
__VM_STD_INSTR_PROTO(imul32_rptr_imm);
__VM_STD_INSTR_PROTO(imul64_reg_reg);
__VM_STD_INSTR_PROTO(imul64_reg_imm);
__VM_STD_INSTR_PROTO(imul64_reg_rptr);
__VM_STD_INSTR_PROTO(imul64_reg_iptr);
__VM_STD_INSTR_PROTO(imul64_iptr_reg);
__VM_STD_INSTR_PROTO(imul64_iptr_imm);
__VM_STD_INSTR_PROTO(imul64_rptr_reg);
__VM_STD_INSTR_PROTO(imul64_rptr_imm);



__VM_STD_INSTR_PROTO(div8_reg_reg);
__VM_STD_INSTR_PROTO(div8_reg_imm);
__VM_STD_INSTR_PROTO(div8_reg_rptr);
__VM_STD_INSTR_PROTO(div8_reg_iptr);
__VM_STD_INSTR_PROTO(div8_iptr_reg);
__VM_STD_INSTR_PROTO(div8_iptr_imm);
__VM_STD_INSTR_PROTO(div8_rptr_reg);
__VM_STD_INSTR_PROTO(div8_rptr_imm);
__VM_STD_INSTR_PROTO(div16_reg_reg);
__VM_STD_INSTR_PROTO(div16_reg_imm);
__VM_STD_INSTR_PROTO(div16_reg_rptr);
__VM_STD_INSTR_PROTO(div16_reg_iptr);
__VM_STD_INSTR_PROTO(div16_iptr_reg);
__VM_STD_INSTR_PROTO(div16_iptr_imm);
__VM_STD_INSTR_PROTO(div16_rptr_reg);
__VM_STD_INSTR_PROTO(div16_rptr_imm);
__VM_STD_INSTR_PROTO(div32_reg_reg);
__VM_STD_INSTR_PROTO(div32_reg_imm);
__VM_STD_INSTR_PROTO(div32_reg_rptr);
__VM_STD_INSTR_PROTO(div32_reg_iptr);
__VM_STD_INSTR_PROTO(div32_iptr_reg);
__VM_STD_INSTR_PROTO(div32_iptr_imm);
__VM_STD_INSTR_PROTO(div32_rptr_reg);
__VM_STD_INSTR_PROTO(div32_rptr_imm);
__VM_STD_INSTR_PROTO(div64_reg_reg);
__VM_STD_INSTR_PROTO(div64_reg_imm);
__VM_STD_INSTR_PROTO(div64_reg_rptr);
__VM_STD_INSTR_PROTO(div64_reg_iptr);
__VM_STD_INSTR_PROTO(div64_iptr_reg);
__VM_STD_INSTR_PROTO(div64_iptr_imm);
__VM_STD_INSTR_PROTO(div64_rptr_reg);
__VM_STD_INSTR_PROTO(div64_rptr_imm);
                     
                     
                     
__VM_STD_INSTR_PROTO(idiv8_reg_reg);
__VM_STD_INSTR_PROTO(idiv8_reg_imm);
__VM_STD_INSTR_PROTO(idiv8_reg_rptr);
__VM_STD_INSTR_PROTO(idiv8_reg_iptr);
__VM_STD_INSTR_PROTO(idiv8_iptr_reg);
__VM_STD_INSTR_PROTO(idiv8_iptr_imm);
__VM_STD_INSTR_PROTO(idiv8_rptr_reg);
__VM_STD_INSTR_PROTO(idiv8_rptr_imm);
__VM_STD_INSTR_PROTO(idiv16_reg_reg);
__VM_STD_INSTR_PROTO(idiv16_reg_imm);
__VM_STD_INSTR_PROTO(idiv16_reg_rptr);
__VM_STD_INSTR_PROTO(idiv16_reg_iptr);
__VM_STD_INSTR_PROTO(idiv16_iptr_reg);
__VM_STD_INSTR_PROTO(idiv16_iptr_imm);
__VM_STD_INSTR_PROTO(idiv16_rptr_reg);
__VM_STD_INSTR_PROTO(idiv16_rptr_imm);
__VM_STD_INSTR_PROTO(idiv32_reg_reg);
__VM_STD_INSTR_PROTO(idiv32_reg_imm);
__VM_STD_INSTR_PROTO(idiv32_reg_rptr);
__VM_STD_INSTR_PROTO(idiv32_reg_iptr);
__VM_STD_INSTR_PROTO(idiv32_iptr_reg);
__VM_STD_INSTR_PROTO(idiv32_iptr_imm);
__VM_STD_INSTR_PROTO(idiv32_rptr_reg);
__VM_STD_INSTR_PROTO(idiv32_rptr_imm);
__VM_STD_INSTR_PROTO(idiv64_reg_reg);
__VM_STD_INSTR_PROTO(idiv64_reg_imm);
__VM_STD_INSTR_PROTO(idiv64_reg_rptr);
__VM_STD_INSTR_PROTO(idiv64_reg_iptr);
__VM_STD_INSTR_PROTO(idiv64_iptr_reg);
__VM_STD_INSTR_PROTO(idiv64_iptr_imm);
__VM_STD_INSTR_PROTO(idiv64_rptr_reg);
__VM_STD_INSTR_PROTO(idiv64_rptr_imm);



__VM_STD_INSTR_PROTO(mod8_reg_reg);
__VM_STD_INSTR_PROTO(mod8_reg_imm);
__VM_STD_INSTR_PROTO(mod8_reg_rptr);
__VM_STD_INSTR_PROTO(mod8_reg_iptr);
__VM_STD_INSTR_PROTO(mod8_iptr_reg);
__VM_STD_INSTR_PROTO(mod8_iptr_imm);
__VM_STD_INSTR_PROTO(mod8_rptr_reg);
__VM_STD_INSTR_PROTO(mod8_rptr_imm);
__VM_STD_INSTR_PROTO(mod16_reg_reg);
__VM_STD_INSTR_PROTO(mod16_reg_imm);
__VM_STD_INSTR_PROTO(mod16_reg_rptr);
__VM_STD_INSTR_PROTO(mod16_reg_iptr);
__VM_STD_INSTR_PROTO(mod16_iptr_reg);
__VM_STD_INSTR_PROTO(mod16_iptr_imm);
__VM_STD_INSTR_PROTO(mod16_rptr_reg);
__VM_STD_INSTR_PROTO(mod16_rptr_imm);
__VM_STD_INSTR_PROTO(mod32_reg_reg);
__VM_STD_INSTR_PROTO(mod32_reg_imm);
__VM_STD_INSTR_PROTO(mod32_reg_rptr);
__VM_STD_INSTR_PROTO(mod32_reg_iptr);
__VM_STD_INSTR_PROTO(mod32_iptr_reg);
__VM_STD_INSTR_PROTO(mod32_iptr_imm);
__VM_STD_INSTR_PROTO(mod32_rptr_reg);
__VM_STD_INSTR_PROTO(mod32_rptr_imm);
__VM_STD_INSTR_PROTO(mod64_reg_reg);
__VM_STD_INSTR_PROTO(mod64_reg_imm);
__VM_STD_INSTR_PROTO(mod64_reg_rptr);
__VM_STD_INSTR_PROTO(mod64_reg_iptr);
__VM_STD_INSTR_PROTO(mod64_iptr_reg);
__VM_STD_INSTR_PROTO(mod64_iptr_imm);
__VM_STD_INSTR_PROTO(mod64_rptr_reg);
__VM_STD_INSTR_PROTO(mod64_rptr_imm);



__VM_STD_INSTR_PROTO(imod8_reg_reg);
__VM_STD_INSTR_PROTO(imod8_reg_imm);
__VM_STD_INSTR_PROTO(imod8_reg_rptr);
__VM_STD_INSTR_PROTO(imod8_reg_iptr);
__VM_STD_INSTR_PROTO(imod8_iptr_reg);
__VM_STD_INSTR_PROTO(imod8_iptr_imm);
__VM_STD_INSTR_PROTO(imod8_rptr_reg);
__VM_STD_INSTR_PROTO(imod8_rptr_imm);
__VM_STD_INSTR_PROTO(imod16_reg_reg);
__VM_STD_INSTR_PROTO(imod16_reg_imm);
__VM_STD_INSTR_PROTO(imod16_reg_rptr);
__VM_STD_INSTR_PROTO(imod16_reg_iptr);
__VM_STD_INSTR_PROTO(imod16_iptr_reg);
__VM_STD_INSTR_PROTO(imod16_iptr_imm);
__VM_STD_INSTR_PROTO(imod16_rptr_reg);
__VM_STD_INSTR_PROTO(imod16_rptr_imm);
__VM_STD_INSTR_PROTO(imod32_reg_reg);
__VM_STD_INSTR_PROTO(imod32_reg_imm);
__VM_STD_INSTR_PROTO(imod32_reg_rptr);
__VM_STD_INSTR_PROTO(imod32_reg_iptr);
__VM_STD_INSTR_PROTO(imod32_iptr_reg);
__VM_STD_INSTR_PROTO(imod32_iptr_imm);
__VM_STD_INSTR_PROTO(imod32_rptr_reg);
__VM_STD_INSTR_PROTO(imod32_rptr_imm);
__VM_STD_INSTR_PROTO(imod64_reg_reg);
__VM_STD_INSTR_PROTO(imod64_reg_imm);
__VM_STD_INSTR_PROTO(imod64_reg_rptr);
__VM_STD_INSTR_PROTO(imod64_reg_iptr);
__VM_STD_INSTR_PROTO(imod64_iptr_reg);
__VM_STD_INSTR_PROTO(imod64_iptr_imm);
__VM_STD_INSTR_PROTO(imod64_rptr_reg);
__VM_STD_INSTR_PROTO(imod64_rptr_imm);



__VM_STD_INSTR_PROTO(and8_reg_reg);
__VM_STD_INSTR_PROTO(and8_reg_imm);
__VM_STD_INSTR_PROTO(and8_reg_rptr);
__VM_STD_INSTR_PROTO(and8_reg_iptr);
__VM_STD_INSTR_PROTO(and8_iptr_reg);
__VM_STD_INSTR_PROTO(and8_iptr_imm);
__VM_STD_INSTR_PROTO(and8_rptr_reg);
__VM_STD_INSTR_PROTO(and8_rptr_imm);
__VM_STD_INSTR_PROTO(and16_reg_reg);
__VM_STD_INSTR_PROTO(and16_reg_imm);
__VM_STD_INSTR_PROTO(and16_reg_rptr);
__VM_STD_INSTR_PROTO(and16_reg_iptr);
__VM_STD_INSTR_PROTO(and16_iptr_reg);
__VM_STD_INSTR_PROTO(and16_iptr_imm);
__VM_STD_INSTR_PROTO(and16_rptr_reg);
__VM_STD_INSTR_PROTO(and16_rptr_imm);
__VM_STD_INSTR_PROTO(and32_reg_reg);
__VM_STD_INSTR_PROTO(and32_reg_imm);
__VM_STD_INSTR_PROTO(and32_reg_rptr);
__VM_STD_INSTR_PROTO(and32_reg_iptr);
__VM_STD_INSTR_PROTO(and32_iptr_reg);
__VM_STD_INSTR_PROTO(and32_iptr_imm);
__VM_STD_INSTR_PROTO(and32_rptr_reg);
__VM_STD_INSTR_PROTO(and32_rptr_imm);
__VM_STD_INSTR_PROTO(and64_reg_reg);
__VM_STD_INSTR_PROTO(and64_reg_imm);
__VM_STD_INSTR_PROTO(and64_reg_rptr);
__VM_STD_INSTR_PROTO(and64_reg_iptr);
__VM_STD_INSTR_PROTO(and64_iptr_reg);
__VM_STD_INSTR_PROTO(and64_iptr_imm);
__VM_STD_INSTR_PROTO(and64_rptr_reg);
__VM_STD_INSTR_PROTO(and64_rptr_imm);



__VM_STD_INSTR_PROTO(or8_reg_reg);
__VM_STD_INSTR_PROTO(or8_reg_imm);
__VM_STD_INSTR_PROTO(or8_reg_rptr);
__VM_STD_INSTR_PROTO(or8_reg_iptr);
__VM_STD_INSTR_PROTO(or8_iptr_reg);
__VM_STD_INSTR_PROTO(or8_iptr_imm);
__VM_STD_INSTR_PROTO(or8_rptr_reg);
__VM_STD_INSTR_PROTO(or8_rptr_imm);
__VM_STD_INSTR_PROTO(or16_reg_reg);
__VM_STD_INSTR_PROTO(or16_reg_imm);
__VM_STD_INSTR_PROTO(or16_reg_rptr);
__VM_STD_INSTR_PROTO(or16_reg_iptr);
__VM_STD_INSTR_PROTO(or16_iptr_reg);
__VM_STD_INSTR_PROTO(or16_iptr_imm);
__VM_STD_INSTR_PROTO(or16_rptr_reg);
__VM_STD_INSTR_PROTO(or16_rptr_imm);
__VM_STD_INSTR_PROTO(or32_reg_reg);
__VM_STD_INSTR_PROTO(or32_reg_imm);
__VM_STD_INSTR_PROTO(or32_reg_rptr);
__VM_STD_INSTR_PROTO(or32_reg_iptr);
__VM_STD_INSTR_PROTO(or32_iptr_reg);
__VM_STD_INSTR_PROTO(or32_iptr_imm);
__VM_STD_INSTR_PROTO(or32_rptr_reg);
__VM_STD_INSTR_PROTO(or32_rptr_imm);
__VM_STD_INSTR_PROTO(or64_reg_reg);
__VM_STD_INSTR_PROTO(or64_reg_imm);
__VM_STD_INSTR_PROTO(or64_reg_rptr);
__VM_STD_INSTR_PROTO(or64_reg_iptr);
__VM_STD_INSTR_PROTO(or64_iptr_reg);
__VM_STD_INSTR_PROTO(or64_iptr_imm);
__VM_STD_INSTR_PROTO(or64_rptr_reg);
__VM_STD_INSTR_PROTO(or64_rptr_imm);



__VM_STD_INSTR_PROTO(not8_reg);
__VM_STD_INSTR_PROTO(not8_iptr);
__VM_STD_INSTR_PROTO(not8_rptr);
__VM_STD_INSTR_PROTO(not16_reg);
__VM_STD_INSTR_PROTO(not16_iptr);
__VM_STD_INSTR_PROTO(not16_rptr);
__VM_STD_INSTR_PROTO(not32_reg);
__VM_STD_INSTR_PROTO(not32_iptr);
__VM_STD_INSTR_PROTO(not32_rptr);
__VM_STD_INSTR_PROTO(not64_reg);
__VM_STD_INSTR_PROTO(not64_iptr);
__VM_STD_INSTR_PROTO(not64_rptr);



__VM_STD_INSTR_PROTO(xor8_reg_reg);
__VM_STD_INSTR_PROTO(xor8_reg_imm);
__VM_STD_INSTR_PROTO(xor8_reg_rptr);
__VM_STD_INSTR_PROTO(xor8_reg_iptr);
__VM_STD_INSTR_PROTO(xor8_iptr_reg);
__VM_STD_INSTR_PROTO(xor8_iptr_imm);
__VM_STD_INSTR_PROTO(xor8_rptr_reg);
__VM_STD_INSTR_PROTO(xor8_rptr_imm);
__VM_STD_INSTR_PROTO(xor16_reg_reg);
__VM_STD_INSTR_PROTO(xor16_reg_imm);
__VM_STD_INSTR_PROTO(xor16_reg_rptr);
__VM_STD_INSTR_PROTO(xor16_reg_iptr);
__VM_STD_INSTR_PROTO(xor16_iptr_reg);
__VM_STD_INSTR_PROTO(xor16_iptr_imm);
__VM_STD_INSTR_PROTO(xor16_rptr_reg);
__VM_STD_INSTR_PROTO(xor16_rptr_imm);
__VM_STD_INSTR_PROTO(xor32_reg_reg);
__VM_STD_INSTR_PROTO(xor32_reg_imm);
__VM_STD_INSTR_PROTO(xor32_reg_rptr);
__VM_STD_INSTR_PROTO(xor32_reg_iptr);
__VM_STD_INSTR_PROTO(xor32_iptr_reg);
__VM_STD_INSTR_PROTO(xor32_iptr_imm);
__VM_STD_INSTR_PROTO(xor32_rptr_reg);
__VM_STD_INSTR_PROTO(xor32_rptr_imm);
__VM_STD_INSTR_PROTO(xor64_reg_reg);
__VM_STD_INSTR_PROTO(xor64_reg_imm);
__VM_STD_INSTR_PROTO(xor64_reg_rptr);
__VM_STD_INSTR_PROTO(xor64_reg_iptr);
__VM_STD_INSTR_PROTO(xor64_iptr_reg);
__VM_STD_INSTR_PROTO(xor64_iptr_imm);
__VM_STD_INSTR_PROTO(xor64_rptr_reg);
__VM_STD_INSTR_PROTO(xor64_rptr_imm);



__VM_STD_INSTR_PROTO(shl8_reg_reg);
__VM_STD_INSTR_PROTO(shl8_reg_imm);
__VM_STD_INSTR_PROTO(shl8_reg_rptr);
__VM_STD_INSTR_PROTO(shl8_reg_iptr);
__VM_STD_INSTR_PROTO(shl8_iptr_reg);
__VM_STD_INSTR_PROTO(shl8_iptr_imm);
__VM_STD_INSTR_PROTO(shl8_rptr_reg);
__VM_STD_INSTR_PROTO(shl8_rptr_imm);
__VM_STD_INSTR_PROTO(shl16_reg_reg);
__VM_STD_INSTR_PROTO(shl16_reg_imm);
__VM_STD_INSTR_PROTO(shl16_reg_rptr);
__VM_STD_INSTR_PROTO(shl16_reg_iptr);
__VM_STD_INSTR_PROTO(shl16_iptr_reg);
__VM_STD_INSTR_PROTO(shl16_iptr_imm);
__VM_STD_INSTR_PROTO(shl16_rptr_reg);
__VM_STD_INSTR_PROTO(shl16_rptr_imm);
__VM_STD_INSTR_PROTO(shl32_reg_reg);
__VM_STD_INSTR_PROTO(shl32_reg_imm);
__VM_STD_INSTR_PROTO(shl32_reg_rptr);
__VM_STD_INSTR_PROTO(shl32_reg_iptr);
__VM_STD_INSTR_PROTO(shl32_iptr_reg);
__VM_STD_INSTR_PROTO(shl32_iptr_imm);
__VM_STD_INSTR_PROTO(shl32_rptr_reg);
__VM_STD_INSTR_PROTO(shl32_rptr_imm);
__VM_STD_INSTR_PROTO(shl64_reg_reg);
__VM_STD_INSTR_PROTO(shl64_reg_imm);
__VM_STD_INSTR_PROTO(shl64_reg_rptr);
__VM_STD_INSTR_PROTO(shl64_reg_iptr);
__VM_STD_INSTR_PROTO(shl64_iptr_reg);
__VM_STD_INSTR_PROTO(shl64_iptr_imm);
__VM_STD_INSTR_PROTO(shl64_rptr_reg);
__VM_STD_INSTR_PROTO(shl64_rptr_imm);



__VM_STD_INSTR_PROTO(shr8_reg_reg);
__VM_STD_INSTR_PROTO(shr8_reg_imm);
__VM_STD_INSTR_PROTO(shr8_reg_rptr);
__VM_STD_INSTR_PROTO(shr8_reg_iptr);
__VM_STD_INSTR_PROTO(shr8_iptr_reg);
__VM_STD_INSTR_PROTO(shr8_iptr_imm);
__VM_STD_INSTR_PROTO(shr8_rptr_reg);
__VM_STD_INSTR_PROTO(shr8_rptr_imm);
__VM_STD_INSTR_PROTO(shr16_reg_reg);
__VM_STD_INSTR_PROTO(shr16_reg_imm);
__VM_STD_INSTR_PROTO(shr16_reg_rptr);
__VM_STD_INSTR_PROTO(shr16_reg_iptr);
__VM_STD_INSTR_PROTO(shr16_iptr_reg);
__VM_STD_INSTR_PROTO(shr16_iptr_imm);
__VM_STD_INSTR_PROTO(shr16_rptr_reg);
__VM_STD_INSTR_PROTO(shr16_rptr_imm);
__VM_STD_INSTR_PROTO(shr32_reg_reg);
__VM_STD_INSTR_PROTO(shr32_reg_imm);
__VM_STD_INSTR_PROTO(shr32_reg_rptr);
__VM_STD_INSTR_PROTO(shr32_reg_iptr);
__VM_STD_INSTR_PROTO(shr32_iptr_reg);
__VM_STD_INSTR_PROTO(shr32_iptr_imm);
__VM_STD_INSTR_PROTO(shr32_rptr_reg);
__VM_STD_INSTR_PROTO(shr32_rptr_imm);
__VM_STD_INSTR_PROTO(shr64_reg_reg);
__VM_STD_INSTR_PROTO(shr64_reg_imm);
__VM_STD_INSTR_PROTO(shr64_reg_rptr);
__VM_STD_INSTR_PROTO(shr64_reg_iptr);
__VM_STD_INSTR_PROTO(shr64_iptr_reg);
__VM_STD_INSTR_PROTO(shr64_iptr_imm);
__VM_STD_INSTR_PROTO(shr64_rptr_reg);
__VM_STD_INSTR_PROTO(shr64_rptr_imm);



__VM_STD_INSTR_PROTO(sal8_reg_reg);
__VM_STD_INSTR_PROTO(sal8_reg_imm);
__VM_STD_INSTR_PROTO(sal8_reg_rptr);
__VM_STD_INSTR_PROTO(sal8_reg_iptr);
__VM_STD_INSTR_PROTO(sal8_iptr_reg);
__VM_STD_INSTR_PROTO(sal8_iptr_imm);
__VM_STD_INSTR_PROTO(sal8_rptr_reg);
__VM_STD_INSTR_PROTO(sal8_rptr_imm);
__VM_STD_INSTR_PROTO(sal16_reg_reg);
__VM_STD_INSTR_PROTO(sal16_reg_imm);
__VM_STD_INSTR_PROTO(sal16_reg_rptr);
__VM_STD_INSTR_PROTO(sal16_reg_iptr);
__VM_STD_INSTR_PROTO(sal16_iptr_reg);
__VM_STD_INSTR_PROTO(sal16_iptr_imm);
__VM_STD_INSTR_PROTO(sal16_rptr_reg);
__VM_STD_INSTR_PROTO(sal16_rptr_imm);
__VM_STD_INSTR_PROTO(sal32_reg_reg);
__VM_STD_INSTR_PROTO(sal32_reg_imm);
__VM_STD_INSTR_PROTO(sal32_reg_rptr);
__VM_STD_INSTR_PROTO(sal32_reg_iptr);
__VM_STD_INSTR_PROTO(sal32_iptr_reg);
__VM_STD_INSTR_PROTO(sal32_iptr_imm);
__VM_STD_INSTR_PROTO(sal32_rptr_reg);
__VM_STD_INSTR_PROTO(sal32_rptr_imm);
__VM_STD_INSTR_PROTO(sal64_reg_reg);
__VM_STD_INSTR_PROTO(sal64_reg_imm);
__VM_STD_INSTR_PROTO(sal64_reg_rptr);
__VM_STD_INSTR_PROTO(sal64_reg_iptr);
__VM_STD_INSTR_PROTO(sal64_iptr_reg);
__VM_STD_INSTR_PROTO(sal64_iptr_imm);
__VM_STD_INSTR_PROTO(sal64_rptr_reg);
__VM_STD_INSTR_PROTO(sal64_rptr_imm);



__VM_STD_INSTR_PROTO(sar8_reg_reg);
__VM_STD_INSTR_PROTO(sar8_reg_imm);
__VM_STD_INSTR_PROTO(sar8_reg_rptr);
__VM_STD_INSTR_PROTO(sar8_reg_iptr);
__VM_STD_INSTR_PROTO(sar8_iptr_reg);
__VM_STD_INSTR_PROTO(sar8_iptr_imm);
__VM_STD_INSTR_PROTO(sar8_rptr_reg);
__VM_STD_INSTR_PROTO(sar8_rptr_imm);
__VM_STD_INSTR_PROTO(sar16_reg_reg);
__VM_STD_INSTR_PROTO(sar16_reg_imm);
__VM_STD_INSTR_PROTO(sar16_reg_rptr);
__VM_STD_INSTR_PROTO(sar16_reg_iptr);
__VM_STD_INSTR_PROTO(sar16_iptr_reg);
__VM_STD_INSTR_PROTO(sar16_iptr_imm);
__VM_STD_INSTR_PROTO(sar16_rptr_reg);
__VM_STD_INSTR_PROTO(sar16_rptr_imm);
__VM_STD_INSTR_PROTO(sar32_reg_reg);
__VM_STD_INSTR_PROTO(sar32_reg_imm);
__VM_STD_INSTR_PROTO(sar32_reg_rptr);
__VM_STD_INSTR_PROTO(sar32_reg_iptr);
__VM_STD_INSTR_PROTO(sar32_iptr_reg);
__VM_STD_INSTR_PROTO(sar32_iptr_imm);
__VM_STD_INSTR_PROTO(sar32_rptr_reg);
__VM_STD_INSTR_PROTO(sar32_rptr_imm);
__VM_STD_INSTR_PROTO(sar64_reg_reg);
__VM_STD_INSTR_PROTO(sar64_reg_imm);
__VM_STD_INSTR_PROTO(sar64_reg_rptr);
__VM_STD_INSTR_PROTO(sar64_reg_iptr);
__VM_STD_INSTR_PROTO(sar64_iptr_reg);
__VM_STD_INSTR_PROTO(sar64_iptr_imm);
__VM_STD_INSTR_PROTO(sar64_rptr_reg);
__VM_STD_INSTR_PROTO(sar64_rptr_imm);



__VM_STD_INSTR_PROTO(rol8_reg_reg);
__VM_STD_INSTR_PROTO(rol8_reg_imm);
__VM_STD_INSTR_PROTO(rol8_reg_rptr);
__VM_STD_INSTR_PROTO(rol8_reg_iptr);
__VM_STD_INSTR_PROTO(rol8_iptr_reg);
__VM_STD_INSTR_PROTO(rol8_iptr_imm);
__VM_STD_INSTR_PROTO(rol16_reg_reg);
__VM_STD_INSTR_PROTO(rol16_reg_imm);
__VM_STD_INSTR_PROTO(rol16_reg_rptr);
__VM_STD_INSTR_PROTO(rol16_reg_iptr);
__VM_STD_INSTR_PROTO(rol16_iptr_reg);
__VM_STD_INSTR_PROTO(rol16_iptr_imm);
__VM_STD_INSTR_PROTO(rol32_reg_reg);
__VM_STD_INSTR_PROTO(rol32_reg_imm);
__VM_STD_INSTR_PROTO(rol32_reg_rptr);
__VM_STD_INSTR_PROTO(rol32_reg_iptr);
__VM_STD_INSTR_PROTO(rol32_iptr_reg);
__VM_STD_INSTR_PROTO(rol32_iptr_imm);
__VM_STD_INSTR_PROTO(rol64_reg_reg);
__VM_STD_INSTR_PROTO(rol64_reg_imm);
__VM_STD_INSTR_PROTO(rol64_reg_rptr);
__VM_STD_INSTR_PROTO(rol64_reg_iptr);
__VM_STD_INSTR_PROTO(rol64_iptr_reg);
__VM_STD_INSTR_PROTO(rol64_iptr_imm);



__VM_STD_INSTR_PROTO(ror8_reg_reg);
__VM_STD_INSTR_PROTO(ror8_reg_imm);
__VM_STD_INSTR_PROTO(ror8_reg_rptr);
__VM_STD_INSTR_PROTO(ror8_reg_iptr);
__VM_STD_INSTR_PROTO(ror8_iptr_reg);
__VM_STD_INSTR_PROTO(ror8_iptr_imm);
__VM_STD_INSTR_PROTO(ror16_reg_reg);
__VM_STD_INSTR_PROTO(ror16_reg_imm);
__VM_STD_INSTR_PROTO(ror16_reg_rptr);
__VM_STD_INSTR_PROTO(ror16_reg_iptr);
__VM_STD_INSTR_PROTO(ror16_iptr_reg);
__VM_STD_INSTR_PROTO(ror16_iptr_imm);
__VM_STD_INSTR_PROTO(ror32_reg_reg);
__VM_STD_INSTR_PROTO(ror32_reg_imm);
__VM_STD_INSTR_PROTO(ror32_reg_rptr);
__VM_STD_INSTR_PROTO(ror32_reg_iptr);
__VM_STD_INSTR_PROTO(ror32_iptr_reg);
__VM_STD_INSTR_PROTO(ror32_iptr_imm);
__VM_STD_INSTR_PROTO(ror64_reg_reg);
__VM_STD_INSTR_PROTO(ror64_reg_imm);
__VM_STD_INSTR_PROTO(ror64_reg_rptr);
__VM_STD_INSTR_PROTO(ror64_reg_iptr);
__VM_STD_INSTR_PROTO(ror64_iptr_reg);
__VM_STD_INSTR_PROTO(ror64_iptr_imm);
///////////


__VM_STD_INSTR_PROTO(jmp8_reg);
__VM_STD_INSTR_PROTO(jmp16_reg);
__VM_STD_INSTR_PROTO(jmp32_reg);
__VM_STD_INSTR_PROTO(jmp32_val);
__VM_STD_INSTR_PROTO(jmp64_reg);
__VM_STD_INSTR_PROTO(jmp_bptr);
__VM_STD_INSTR_PROTO(jmp_wptr);
__VM_STD_INSTR_PROTO(jmp_dptr);
__VM_STD_INSTR_PROTO(jmp_qptr);



__VM_STD_INSTR_PROTO(je8_reg);
__VM_STD_INSTR_PROTO(je16_reg);
__VM_STD_INSTR_PROTO(je32_reg);
__VM_STD_INSTR_PROTO(je32_val);
__VM_STD_INSTR_PROTO(je64_reg);
__VM_STD_INSTR_PROTO(je_bptr);
__VM_STD_INSTR_PROTO(je_wptr);
__VM_STD_INSTR_PROTO(je_dptr);
__VM_STD_INSTR_PROTO(je_qptr);



__VM_STD_INSTR_PROTO(jne8_reg);
__VM_STD_INSTR_PROTO(jne16_reg);
__VM_STD_INSTR_PROTO(jne32_reg);
__VM_STD_INSTR_PROTO(jne32_val);
__VM_STD_INSTR_PROTO(jne64_reg);
__VM_STD_INSTR_PROTO(jne_bptr);
__VM_STD_INSTR_PROTO(jne_wptr);
__VM_STD_INSTR_PROTO(jne_dptr);
__VM_STD_INSTR_PROTO(jne_qptr);



__VM_STD_INSTR_PROTO(jge8_reg);
__VM_STD_INSTR_PROTO(jge16_reg);
__VM_STD_INSTR_PROTO(jge32_reg);
__VM_STD_INSTR_PROTO(jge32_val);
__VM_STD_INSTR_PROTO(jge64_reg);
__VM_STD_INSTR_PROTO(jge_bptr);
__VM_STD_INSTR_PROTO(jge_wptr);
__VM_STD_INSTR_PROTO(jge_dptr);
__VM_STD_INSTR_PROTO(jge_qptr);



__VM_STD_INSTR_PROTO(jl8_reg);
__VM_STD_INSTR_PROTO(jl16_reg);
__VM_STD_INSTR_PROTO(jl32_reg);
__VM_STD_INSTR_PROTO(jl32_val);
__VM_STD_INSTR_PROTO(jl64_reg);
__VM_STD_INSTR_PROTO(jl_bptr);
__VM_STD_INSTR_PROTO(jl_wptr);
__VM_STD_INSTR_PROTO(jl_dptr);
__VM_STD_INSTR_PROTO(jl_qptr);



__VM_STD_INSTR_PROTO(jle8_reg);
__VM_STD_INSTR_PROTO(jle16_reg);
__VM_STD_INSTR_PROTO(jle32_reg);
__VM_STD_INSTR_PROTO(jle32_val);
__VM_STD_INSTR_PROTO(jle64_reg);
__VM_STD_INSTR_PROTO(jle_bptr);
__VM_STD_INSTR_PROTO(jle_wptr);
__VM_STD_INSTR_PROTO(jle_dptr);
__VM_STD_INSTR_PROTO(jle_qptr);



__VM_STD_INSTR_PROTO(jo8_reg);
__VM_STD_INSTR_PROTO(jo16_reg);
__VM_STD_INSTR_PROTO(jo32_reg);
__VM_STD_INSTR_PROTO(jo32_val);
__VM_STD_INSTR_PROTO(jo64_reg);
__VM_STD_INSTR_PROTO(jo_bptr);
__VM_STD_INSTR_PROTO(jo_wptr);
__VM_STD_INSTR_PROTO(jo_dptr);
__VM_STD_INSTR_PROTO(jo_qptr);

#endif