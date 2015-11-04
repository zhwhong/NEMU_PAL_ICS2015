#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute() {
	DATA_TYPE result = op_dest->val + op_src->val + cpu.CF;

	//set EFLAGS's value
	cpu.ZF = result ? 0 : 1;
	cpu.CF = (result < op_src->val || result < op_dest->val || result < cpu.CF) ? 1 : 0;
	uint8_t temp = 0x00;
	temp = (uint8_t)(op_dest->val & 0x0f) + (uint8_t)(op_src->val & 0x0f) + cpu.CF;
	cpu.AF = (temp>>4)&0x01;
	
	uint8_t low_b = result & 0xff;
	cpu.PF = !((low_b & 0x01)^(low_b>>1 & 0x01)^(low_b>>2 & 0x01)^(low_b>>3 & 0x01)^(low_b>>4 & 0x01)^(low_b>>5 & 0x01)^(low_b>>6 & 0x01)^(low_b>>7 & 0x01));

	cpu.SF = MSB(result);
	if(cpu.CF == 0)	
		cpu.OF = MSB(op_src->val) ? (MSB(op_dest->val) ? !cpu.SF : 0) : (MSB(op_dest->val) ? 0 : cpu.SF);
	else{
		if(result == 0 || (MSB(op_dest->val + op_src->val) == 0 && MSB(result) == 1))
			cpu.OF = 1;
		else
		    cpu.OF = MSB(op_src->val) ? (MSB(op_dest->val) ? !(MSB(result-1)) : 0) : (MSB(op_dest->val) ? 0 : MSB(result-1));
	}
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
