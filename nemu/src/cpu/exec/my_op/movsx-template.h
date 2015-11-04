#include "cpu/exec/template-start.h"

#define instr movsx

static void do_execute() {
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
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
