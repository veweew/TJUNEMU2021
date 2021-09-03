#include "cpu/exec/template-start.h"

#define instr sete

static void do_execute(){
    if(cpu.eflags.ZF){
        OPERAND_W(op_dest, 1);
    }else{
        OPERAND_W(op_dest, 0);
    }
    print_asm_template1();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
