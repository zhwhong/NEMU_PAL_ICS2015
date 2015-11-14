#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {

	cpu.eip += op_src->val;
	if(DATA_BYTE == 2){
		cpu.eip &= 0x0000ffff ;
	}
	/*
	switch (ops_decoded.opcode & 0xff){
		case 0xe9:
		case 0xeb:
			if (DATA_BYTE == 1)
				op_src->val = (uint32_t)(int32_t)(int8_t)op_src->val;
			else if (DATA_BYTE == 2)
				op_src->val = (uint32_t)(int32_t)(int16_t)op_src->val;

			cpu.eip += op_src->val;
			if (DATA_BYTE == 2)
				cpu.eip &= 0x0000ffff;
			break;
		case 0xff:
			cpu.eip = op_src->val;
			if (DATA_BYTE == 2)
				cpu.eip &= 0x0000ffff;
			cpu.eip -= 2;
			break;
	}
	*/
	print_asm_template1();
}

	make_instr_helper(i)

#if DATA_BYTE == 2 || DATA_BYTE == 4
	//make_instr_helper(rm)
	make_helper(concat(jmp_rm_, SUFFIX))
	{
		concat(decode_rm_, SUFFIX)(eip + 1);

		if(DATA_BYTE == 2){
			cpu.eip = op_src->val & 0x0000ffff;
		}
		else if(DATA_BYTE == 4){
			cpu.eip =  op_src->val;
		}
		return 0;
	}
	
#endif

#include "cpu/exec/template-end.h"
