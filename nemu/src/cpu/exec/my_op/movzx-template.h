#include "cpu/exec/template-start.h"

#define instr movzx

static void do_execute() {
	if(DATA_BYTE == 1){
		/*
		if(op_dest->size == 2){
			uint16_t temp = (uint16_t)(uint8_t)op_src->val;
			OPERAND_W(op_dest, temp);
		}
		else if(op_dest->size == 4){
			uint32_t temp = (uint32_t)(uint8_t)op_src->val;
			OPERAND_W(op_dest, temp);
		}
		*/
		uint32_t temp = (uint32_t)(uint8_t)op_src->val;
		OPERAND_W(op_dest, temp);
	}	
	else if(DATA_BYTE == 2){
		uint32_t temp = (uint32_t)(uint16_t)op_src->val;
		OPERAND_W(op_dest, temp);
	}
	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
