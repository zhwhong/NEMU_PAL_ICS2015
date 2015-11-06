#include "cpu/exec/template-start.h"

#define instr movsx

static void do_execute() {
	uint32_t result = 0;

	// printf("%x\n", ops_decoded.opcode);
	switch (ops_decoded.opcode & 0xff){
		case 0xbe:
			result = (uint32_t)(int32_t)(int8_t)op_src->val;
			// printf("0xbe %x\n", result);
			break;
		case 0xbf:
			result = (uint32_t)(int32_t)(int16_t)op_src->val;
			// printf("0xbf %x\n", result);
			break;
		default:
			// printf("default %u\n", result);
			break;
	}

	OPERAND_W(op_dest, result);
	print_asm_template2();
	/*
	if(DATA_BYTE == 1){
		if(op_dest->size == 2){
			int16_t temp = (int16_t)(int8_t)op_src->val;
			OPERAND_W(op_dest, temp);
		}
		else if(op_dest->size == 4){
			int32_t temp = (int32_t)(int8_t)op_src->val;
			OPERAND_W(op_dest, temp);
		}
	}	
	else if(DATA_BYTE == 2){
		int32_t temp = (int32_t)(int16_t)op_src->val;
		OPERAND_W(op_dest, temp);
	}
	print_asm_template2();
	*/
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
