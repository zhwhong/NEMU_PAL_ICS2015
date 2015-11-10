#include "cpu/exec/helper.h"

make_helper(stos) {
	int IncDec = 0;
	if(ops_decoded.opcode == 0xaa)
	{
		swaddr_write(reg_l(R_EDI), 1 , reg_b(R_AL));
		IncDec = (cpu.DF == 0) ? 1 : -1 ;
	} 
	else if(ops_decoded.opcode == 0xab && ops_decoded.is_data_size_16 == 0)
	{
		swaddr_write(reg_l(R_EDI), 4 , reg_l(R_EAX));
		IncDec = (cpu.DF == 0) ? 4 : -4 ;
	}
	else{
		swaddr_write(reg_l(R_EDI), 2 , reg_w(R_AX));
		IncDec = (cpu.DF == 0) ? 2 : -2 ;
	}

	reg_l(R_EDI) += IncDec;

#ifdef DEBUG
	print_asm_template1();
#endif
	return 1;
}

make_helper(stos_b){
	return stos(eip);
}

make_helper(stos_v){
	return stos(eip);
}
