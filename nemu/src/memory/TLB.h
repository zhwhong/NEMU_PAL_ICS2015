#include "common.h"
#define TLB_NUM 64

typedef struct {
	bool valid;
	uint32_t viaddr;
	uint32_t phaddr;
}TLB;

extern TLB tlb[TLB_NUM];