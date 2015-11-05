#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	if(ops_decoded.opcode == 0xEB || ops_decoded.opcode == 0xE9){
		cpu.eip += op_src->val;
		if(DATA_BYTE == 2)
			cpu.eip &= 0x0000ffff;
	}
	else if(ops_decoded.opcode == 0xFF){
		if(DATA_BYTE == 2){
			cpu.eip = (op_src->val & 0x0000ffff) - 2;
		}	
		else if(DATA_BYTE == 4){
			cpu.eip = op_src->val - 2;
		}
	}

	print_asm_template1();
}

	make_instr_helper(i)

#if DATA_BYTE == 2 || DATA_BYTE == 4
	make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"
