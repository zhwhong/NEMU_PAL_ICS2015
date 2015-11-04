#include "cpu/exec/template-start.h"

#define instr jcc

static void do_execute() {

	int32_t rel;
#if DATA_BYTE == 1
	rel = (int32_t)(int8_t)op_src->imm;
#elif DATA_BYTE == 2
	rel = (int32_t)(int16_t)op_src->imm;
#elif DATA_BYTE == 4
	rel = (int32_t)op_src->imm;
#endif

	switch(ops_decoded.opcode & 0xff){
		case 0x70:	// JO rel8
		case 0x80:	// JO rel16/32
			if(cpu.OF == 1)
				cpu.eip += rel;
			break;
		case 0x71:	// JNO rel8
		case 0x81:	// JNO rel16/32
			if(cpu.OF == 0)
				cpu.eip += rel;
			break;
		case 0x72:	// JB rel, JC rel8, JNAE rel8
		case 0x82:	// JB rel16/32, JC rel16/32, JNAE rel16/32
			if(cpu.CF == 1)
				cpu.eip += rel;
			break;
		case 0x73:	// JAE rel8, JNB rel8, JNC rel8
		case 0x83:	// JAE rel16/32, JNB rel16/32 JNC rel16/32
			if(cpu.CF == 0)
				cpu.eip += rel;
			break;
		case 0x74:	// JE rel8, JZ rel8
		case 0x84:	// JE rel16/32, JZ rel16/32
			if(cpu.ZF == 1)
				cpu.eip += rel;
			break;
		case 0x75:	// JNE rel8, JNZ rel8
		case 0x85:	// JNE rel16/32, JNZ rel16/32
			if(cpu.ZF == 0)
				cpu.eip += rel;
			break;
		case 0x76:	// JBE rel8, JNA rel8
		case 0x86:	// JBE rel16/32, JNA rel16/32
			if(cpu.CF == 1 || cpu.ZF == 1)
				cpu.eip += rel;
			break;
		case 0x77:	// JA rel8, JNBE rel8
		case 0x87:	// JA rel16/32, JNBE rel16/32
			if(cpu.CF == 0 && cpu.ZF == 0)
				cpu.eip += rel;
			break;
		case 0x78:	// JS rel8
		case 0x88:	// JS rel16/32
			if(cpu.SF == 1)
				cpu.eip += rel;
			break;
		case 0x79:	// JNS rel8
		case 0x89:	// JNS rel16/32
			if(cpu.SF == 0)
				cpu.eip += rel;
			break;
		case 0x7A:	// JP rel8, JPE rel8
		case 0x8A:	// JP rel16/32, JPE rel16/32
			if(cpu.PF == 1)
				cpu.eip += rel;
			break;
		case 0x7B:	// JNP rel8, JPO rel8
		case 0x8B:	// JNP rel16/32, JPO rel16/32
			if(cpu.PF == 0)
				cpu.eip += rel;
			break;
		case 0x7C:	// JL rel8, JNGE rel8
		case 0x8C:	// JL rel16/32, JNGE rel16/32
			if(cpu.SF != cpu.OF)
				cpu.eip += rel;
			break;
		case 0x7D:	// JGE rel8, JNL rel8
		case 0x8D:	// JGE rel16/32, JNL rel16/32
			if(cpu.SF == cpu.OF)
				cpu.eip += rel;
			break;
		case 0x7E:	// JLE rel8, JNG rel8
		case 0x8E:	// JLE rel16/32, JNG rel16/32
			if(cpu.ZF == 1 || cpu.SF != cpu.OF)
				cpu.eip += rel;
			break;
		case 0x7F:	// JG rel8, JNLE rel8
		case 0x8F:	// JG rel16/32, JNLE rel16/32
			if(cpu.ZF == 0 && cpu.SF == cpu.OF)
				cpu.eip += rel;
			break;
		case 0xE3:	// JCXZ rel8, JECXZ rel8
			if(reg_w(R_CX) == 0 || reg_l(R_ECX) == 0)
				cpu.eip += rel;
			break;
		default:
			break;
	}

	print_asm_template1();
}

	make_instr_helper(i)

#include "cpu/exec/template-end.h"
