#include <stdlib.h>
#include <time.h>

#include <common.h>

#include "TLB.h"

TLB tlb[TLB_NUM];

uint32_t readTLB(uint32_t va) {
	int i;
	va = va & 0xfffff000;
	for(i = 0; i < TLB_NUM; i++){
		if (tlb[i].viaddr == va && tlb[i].valid == true)
			return tlb[i].phaddr;
	}
	return -1;
}

void writeTLB(uint32_t va, uint32_t pa) {
	int i;
	va = va & 0xfffff000;
	pa = pa & 0xfffff000;
	for(i = 0; i < TLB_NUM; i++) {
		if(tlb[i].valid == false) {
			tlb[i].viaddr = va;
			tlb[i].phaddr = pa;
			tlb[i].valid = true;
			return;
		}
	}
	srand(time(0));
	i = rand()%TLB_NUM;
	tlb[i].viaddr = va;
	tlb[i].phaddr = pa;
	tlb[i].valid = true;
}