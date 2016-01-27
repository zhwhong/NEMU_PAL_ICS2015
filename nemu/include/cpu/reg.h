#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
	union{
		union{
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
		} gpr[8];

		/* Do NOT change the order of the GPRs' definitions. */
		struct{
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};

	swaddr_t eip;

	union{
		uint32_t EFLAGS;	//32位标志寄存器
		struct {
			uint32_t CF:1;
			uint32_t remain1:1;
			uint32_t PF:1;
			uint32_t remain2:1;
			uint32_t AF:1;
			uint32_t remain3:1;
			uint32_t ZF:1;
			uint32_t SF:1;
			uint32_t TF:1;
			uint32_t IF:1;
			uint32_t DF:1;
			uint32_t OF:1;
			uint32_t IOPL:1;
			uint32_t NT:1;
			uint32_t remian4:1;
			uint32_t RF:1;
			uint32_t VM:1;
			uint32_t reserve:15;
		};
	};
	
	struct GDTR{
		uint32_t base_addr;
		uint16_t seg_limit;
	}gdtr;
	CR0 cr0;
	struct {
		uint16_t selector;
		union {
			struct {
				uint32_t seg_base1 	:16;
				uint32_t seg_base2 	:8;
				uint32_t seg_base3 	:8;
			};
			uint32_t seg_base;
		};
		union {
			struct {
				uint32_t seg_limit1 	:16;
				uint32_t seg_limit2 	:4;
				uint32_t seg_limit3 	:12;
			};
			uint32_t seg_limit;
		};
	}cs, ds, es, ss;
} CPU_state;

typedef struct {
	union {
		struct {
			uint32_t seg_limit1	:16;
			uint32_t seg_base1	:16;
		};
		uint32_t val_part1;
	};
	union {
		struct {
			uint32_t seg_base2 	:8;
			uint32_t A 		:1;
			uint32_t TYPE		:3;
			uint32_t one 		:1;
			uint32_t DPL		:2;
			uint32_t P		:1;
			uint32_t seg_limit2	:4;
			uint32_t AVL		:1;
			uint32_t zero		:1;
			uint32_t X		:1;
			uint32_t G 		:1;
			uint32_t seg_base3	:8;
		};
		uint32_t val_part2;
	};
}SEG_DES;

typedef  union {
	struct 
	{
		uint16_t rpl	:2;
		uint16_t ti	:1;
		uint16_t index 	:13;
	};
	uint16_t val;
}SELECTOR;

extern CPU_state cpu;
extern SELECTOR current_sreg;
extern SEG_DES *seg_des;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif
