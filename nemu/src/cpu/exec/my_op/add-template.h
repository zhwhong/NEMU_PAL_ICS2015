#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
	DATA_TYPE result = op_dest->val + op_src->val;

	//set EFLAGS's value
	cpu.ZF = result ? 0 : 1;
	cpu.CF = (result < op_src->val || result < op_dest->val) ? 1 : 0;
	
	uint8_t temp = 0x00;
	temp = (uint8_t)(op_dest->val & 0x0f) + (uint8_t)(op_src->val & 0x0f);
	cpu.AF = (temp>>4)&0x01;
	
	uint8_t low_b = result & 0xff;
	cpu.PF = !((low_b & 0x01)^(low_b>>1 & 0x01)^(low_b>>2 & 0x01)^(low_b>>3 & 0x01)^(low_b>>4 & 0x01)^(low_b>>5 & 0x01)^(low_b>>6 & 0x01)^(low_b>>7 & 0x01));

	cpu.SF = MSB(result);
	
	cpu.OF = MSB(op_src->val) ? (MSB(op_dest->val) ? !cpu.SF : 0) : (MSB(op_dest->val) ? 0 : cpu.SF);
	
	OPERAND_W(op_dest, result);

	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
	make_instr_helper(si2rm);
#endif

	make_instr_helper(i2a)
	make_instr_helper(i2rm)
	make_instr_helper(r2rm)
	make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
