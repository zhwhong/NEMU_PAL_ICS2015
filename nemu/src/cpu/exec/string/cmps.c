#include "cpu/exec/helper.h"

make_helper(cmps) {
	int DATA_BYTE = 0, IncDec=0;
	uint32_t result = 0;
	uint32_t dest, src;

	if(ops_decoded.opcode == 0xa6) DATA_BYTE = 1;
	else if(ops_decoded.opcode == 0xa7 && ops_decoded.is_data_size_16 == 0)
		DATA_BYTE = 4;
	else DATA_BYTE = 2;

	IncDec = (cpu.DF == 0) ? DATA_BYTE : -DATA_BYTE;

	src = swaddr_read(reg_l(R_ESI), DATA_BYTE);
	dest = swaddr_read(reg_l(R_EDI), DATA_BYTE);

	result =  src - dest;

	reg_l(R_EDI) += IncDec;
	reg_l(R_ESI) += IncDec;

    cpu.ZF = result ? 0 : 1;
	cpu.CF = ((uint32_t)(src) >= (uint32_t)(dest)) ? 0 : 1;
	cpu.AF = ((uint8_t)(src & 0x0f) >= (uint8_t)(dest & 0x0f)) ? 0 : 1;
	uint8_t low_b = result & 0xff;
		cpu.PF = !((low_b & 0x01)^(low_b>>1 & 0x01)^(low_b>>2 & 0x01)^(low_b>>3 & 0x01)^(low_b>>4 & 0x01)^(low_b>>5 & 0x01)^(low_b>>6 & 0x01)^(low_b>>7 & 0x01));
	
	int temp_off = (DATA_BYTE << 3) - 1;

	cpu.SF = result>> temp_off;
	cpu.OF = (dest>>temp_off) ? ((cpu.esi>>temp_off) ? 0 : cpu.SF) : ((cpu.esi>>temp_off) ? !cpu.SF : 0);
	cpu.SF = result >> temp_off;

#ifdef DEBUG
	print_asm_template1();
#endif
	
	return 1;
}

make_helper(cmps_b){
	return cmps(eip);
}

make_helper(cmps_v){
	return cmps(eip);
}
