#include "cpu/exec/template-start.h"

#define instr cmov

static void do_execute(){
	uint8_t condition=0;

	switch(ops_decoded.opcode & 0x0ff){
		
		case 0x40: condition = (cpu.OF == 1); break;						//CMOVO
		case 0x41: condition = (cpu.OF == 0); break;						//CMOVNO
		case 0x42: condition = (cpu.CF == 1); break;						//CMOVB, CMOVC, CMOVNAE
		case 0x43: condition = (cpu.CF == 0); break;						//CMOVAE, CMOVNB, CMOVNC
		case 0x44: condition = (cpu.ZF == 1); break;						//CMOVE, CMOVZ
		case 0x45: condition = (cpu.ZF == 0); break;						//CMOVNE, CMOVNZ
		case 0x46: condition = (cpu.CF == 1 || cpu.ZF == 1); break;			//CMOVBE, CMOVNA
		case 0x47: condition = (cpu.CF == 0 && cpu.ZF == 0); break;			//CMOVA, CMOVNBE
		case 0x48: condition = (cpu.SF == 1); break;						//CMOVS
		case 0x49: condition = (cpu.SF == 0); break;						//CMOVNS
		case 0x4a: condition = (cpu.PF == 1); break;						//CMOVP, CMOVpe
		case 0x4b: condition = (cpu.PF == 0); break;						//CMOVNP, CMOVPO
		case 0x4c: condition = (cpu.SF != cpu.OF); break;					//CMOVL, CMOVNGE
		case 0x4d: condition = (cpu.SF == cpu.OF); break;					//CMOVGE, CMOVNL
		case 0x4e: condition = (cpu.ZF == 1 || cpu.SF != cpu.OF); break;	//CMOVLE, CMOVNG
		case 0x4f: condition = (cpu.ZF == 0 && cpu.SF == cpu.OF); break;	//CMOVG, CMOVNLE

	}

	if(condition){
		OPERAND_W(op_dest, op_src->val);
	}

	print_asm_template2();
}

make_instr_helper(rm2r);

#include "cpu/exec/template-end.h"