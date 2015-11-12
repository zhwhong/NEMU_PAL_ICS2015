#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	int len=concat(decode_i_, SUFFIX)(cpu.eip + 1) + 1;
	if(2 == DATA_BYTE){
		cpu.esp =cpu.esp - 2;
		MEM_W(cpu.esp, (uint16_t)((cpu.eip + len) & 0x0000ffff));

		cpu.eip = (cpu.eip + op_src->val)&0x0000ffff;
	}
	else if(4 == DATA_BYTE){
		cpu.esp = cpu.esp-4;
		MEM_W(cpu.esp,  cpu.eip + len);
		cpu.eip = cpu.eip+ op_src->val;
	}
	print_asm_template1();

	/*
	if(ops_decoded.opcode == 0xE8){
		if(DATA_BYTE == 2){
			cpu.esp -= 2;
			MEM_W(cpu.esp, (uint16_t)(cpu.eip & 0x0000ffff));
			cpu.eip = (cpu.eip + op_src->val) & 0x0000ffff;
		}
		else if(DATA_BYTE == 4){
			cpu.esp -= 4;
			MEM_W(cpu.esp, cpu.eip);
			cpu.eip += op_src->val;
		}
	}
	else if(ops_decoded.opcode == 0xFF){
		if(DATA_BYTE == 2){
			cpu.esp -= 2;
			MEM_W(cpu.esp, (uint16_t)(cpu.eip & 0x0000ffff));
			cpu.eip = (op_src->val & 0x0000ffff) - 2;
		}
		else if(DATA_BYTE == 4){
			cpu.esp -= 4;
			MEM_W(cpu.esp, cpu.eip);
			cpu.eip = op_src->val - 2;
		}
	}
	print_asm_template1();
	*/
}

	make_instr_helper(i);

#if DATA_BYTE ==2 || DATA_BYTE == 4
	
	make_helper(concat(call_rm_, SUFFIX))
	{
		int len=concat(decode_rm_, SUFFIX)(eip + 1) + 1;
		if(2 == DATA_BYTE){
			cpu.esp =cpu.esp - 2;
			MEM_W(cpu.esp, (uint16_t)((cpu.eip+len)& 0x0000ffff));

			cpu.eip = ( op_src->val)&0x0000ffff;
		}
		else if(4 == DATA_BYTE){
			cpu.esp = cpu.esp-4;
			MEM_W(cpu.esp,  cpu.eip + len);
			cpu.eip =  op_src->val;
	}
	print_asm_template1();
	return 0;	
}
#endif

#include "cpu/exec/template-end.h"
