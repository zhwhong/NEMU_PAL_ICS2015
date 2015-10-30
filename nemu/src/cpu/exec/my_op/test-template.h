#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
	DATA_TYPE result = op_dest->val & op_src->val;

	//set EFLAGS's value
	cpu.CF = 0;
	cpu.OF = 0;
	cpu.ZF = result ? 0 : 1;
	cpu.SF = MSB(result);
	
	uint8_t low_b = result & 0xff;
	if((low_b & 0x01)^(low_b>>1 & 0x01)^(low_b>>2 & 0x01)^(low_b>>3 & 0x01)^(low_b>>4 & 0x01)^(low_b>>5 & 0x01)^(low_b>>6 & 0x01)^(low_b>>7 & 0x01))
		cpu.PF = 0;
	else
		cpu.PF = 1;

	print_asm_template2();
}

	make_instr_helper(i2a)
	make_instr_helper(i2rm)
	make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
