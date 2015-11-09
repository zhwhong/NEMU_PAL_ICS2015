#include "cpu/exec/template-start.h"

#define instr movzx

static void do_execute() {
	uint32_t result = 0;

	// printf("%x\n", ops_decoded.opcode & 0xff);
	switch (ops_decoded.opcode & 0xff){
		case 0xb6:
			result = op_src->val & 0x000000ff;
			// printf("0xb6 %u\n", result);
			break;
		case 0xb7:
			result = op_src->val & 0x0000ffff;
			// printf("0xb7 %u\n", result);
			break;
	}

	//OPERAND_W(op_dest, result);
	write_operand_l(op_dest, result);
	print_asm_template2();
	/*
	if(DATA_BYTE == 1){
		**
		if(op_dest->size == 2){
			uint16_t temp = (uint16_t)(uint8_t)op_src->val;
			OPERAND_W(op_dest, temp);
		}
		else if(op_dest->size == 4){
			uint32_t temp = (uint32_t)(uint8_t)op_src->val;
			OPERAND_W(op_dest, temp);
		}
		**
		uint32_t temp = (uint32_t)(uint8_t)op_src->val;
		OPERAND_W(op_dest, temp);
	}	
	else if(DATA_BYTE == 2){
		uint32_t temp = (uint32_t)(uint16_t)op_src->val;
		OPERAND_W(op_dest, temp);
	}
	print_asm_template2();
	*/
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
