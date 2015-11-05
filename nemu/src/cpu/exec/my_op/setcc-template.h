#include "cpu/exec/template-start.h"

#define instr setcc

static void do_execute() {

	switch(ops_decoded.opcode & 0xff){
		case 0x90:	// SETO r/m8
			if(cpu.OF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x91:	// SETNO r/m8
			if(cpu.OF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x92:	// SETB r/m8, SETC r/m8, SETNAE r/m8
			if(cpu.CF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x93:	// SETAE r/m8, SETNB r/m8, SETNC r/m8
			if(cpu.CF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x94:	// SETE r/m8, SETZ r/m8
			if(cpu.ZF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x95:	// SETNE r/m8, SETNZ r/m8
			if(cpu.ZF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x96:	// SETBE r/m8, SETNA r/m8
			if(cpu.CF == 1 || cpu.ZF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x97:	// SETA r/m8, SETNBE r/m8
			if(cpu.CF == 0 && cpu.ZF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x98:	// SETS r/m8
			if(cpu.SF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x99:	// SETNS r/m8
			if(cpu.SF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9A:	// SETP r/m8, SETPE r/m8
			if(cpu.PF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9B:	// SETNP r/m8, SETPO r/m8
			if(cpu.PF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9C:	// SETL r/m8, SETNGE r/m8
			if(cpu.SF != cpu.OF)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9D:	// SETGE r/m8, SETNL r/m8
			if(cpu.SF == cpu.OF)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9E:	// SETLE r/m8, SETNG r/m8
			if(cpu.ZF == 1 || cpu.SF != cpu.OF)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9F:	// SETG r/m8. SETNLE r/m8
			if(cpu.ZF == 0 && cpu.SF == cpu.OF)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		default:
			break;
	}

	print_asm_template1();
}

	make_instr_helper(rm)

#include "cpu/exec/template-end.h"
