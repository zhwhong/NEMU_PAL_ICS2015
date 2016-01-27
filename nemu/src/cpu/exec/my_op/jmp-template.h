#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {

	if (DATA_BYTE == 1)
		op_src->val = (uint32_t)(int32_t)(int8_t)op_src->val;
	else if (DATA_BYTE == 2)
		op_src->val = (uint32_t)(int32_t)(int16_t)op_src->val;

	cpu.eip += op_src->val;
	if (DATA_BYTE == 2)
		cpu.eip &= 0x0000ffff;
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

#if DATA_BYTE == 4
make_helper(ljmp) {
	extern SEG_DES *seg_des;
	SEG_DES seg;
	seg_des = &seg;
	uint32_t op1 = instr_fetch(eip+1, 4)-7;
	uint16_t op2 = instr_fetch(eip +5, 2);
	cpu.eip = op1;
	cpu.cs.selector = op2;
	Assert(((cpu.cs.selector>>3)<<3) <= cpu.gdtr.seg_limit, "segment out limit %d, %d", ((cpu.cs.selector>>3)<<3), cpu.gdtr.seg_limit);
	seg_des->val_part1 = instr_fetch(cpu.gdtr.base_addr + ((cpu.cs.selector>>3)<<3), 4);
	seg_des->val_part2 = instr_fetch(cpu.gdtr.base_addr + ((cpu.cs.selector>>3)<<3)+4, 4);
	Assert(seg_des->P == 1, "segment error");
	cpu.cs.seg_base1 = seg_des->seg_base1;
	cpu.cs.seg_base2 = seg_des->seg_base2;
	cpu.cs.seg_base3 = seg_des->seg_base3;
	cpu.cs.seg_limit1 = seg_des->seg_limit1;
	cpu.cs.seg_limit2 = seg_des->seg_limit2;
	cpu.cs.seg_limit3 = 0xfff;
	print_asm("ljmp %x, %x\n", op2, op1+7);
	return 7;
}
#endif

#include "cpu/exec/template-end.h"
