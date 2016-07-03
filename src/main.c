#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include "..\include\core\instructions_data.h"
#include "..\include\core\vm_core.h"
#include "..\include\external_api.h"
#include "..\include\core\instructions.h"
#include "..\include\cmn_types.h"

#include <time.h>

unsigned
test_ipt(vm_instr_t instruction, unsigned amount)
{
    vm_handle_t vm = create_vm(1);
    vm->cores[0].IP = 0;
    vm->ram->cells[0] = 2;
    vm->ram->cells[1] = 3;
    vm->cores[0].std_regs[1].B = 148;
    vm->cores[0].std_regs[2].B = 4;
    clock_t start = clock();
    unsigned result = 0;
    for (unsigned i = 0; i < amount; ++i, ++result)
    {
        instruction(&vm->cores[0], vm->ram);
        vm->cores[0].IP = 0;
    }
    clock_t delta = clock() - start;
    result = result / delta;
    free_vm(&vm);
    return result;
}

void test()
{
    vm_handle_t vm = create_vm(1);
    vm->cores[0].IP = 0;
    vm->ram->cells[0] = 1;
    vm->ram->cells[1] = 2;

    vm->cores[0].std_regs[1].B = 148;
    vm->cores[0].std_regs[2].B = 4;
    shl8_reg_reg(&vm->cores[0], vm->ram);
}

int 
main(void)
{
    int a = -100 >> 20;
    //test();
    printf("instructions per tact: %u", test_ipt(shl8_reg_reg, 1000000000)); // rel
   // printf("instructions per tact: %u", test_ipt(mov8_reg_reg, 100000000)); // dbg

    char casd=0;
    scanf("%c", &casd);
    return 0;
}