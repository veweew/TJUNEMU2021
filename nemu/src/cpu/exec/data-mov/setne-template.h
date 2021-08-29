#include "cpu/exec/template-start.h"

#define instr setne

static void do_execute(){
    Log("%d",cpu.eflags.ZF);
    if(!cpu.eflags.ZF){
        OPERAND_W(op_dest, 1);
    }else{
        OPERAND_W(op_dest, 0);
        Log("val:%x",op_src->val);
    }
    print_asm_template1();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
