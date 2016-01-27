#include "nemu.h"

//uint32_t dram_read(hwaddr_t, size_t);
//void dram_write(hwaddr_t, size_t, uint32_t);
uint32_t L1cache_read(hwaddr_t, size_t);
void L1cache_write(hwaddr_t, size_t, uint32_t);
lnaddr_t seg_translate(swaddr_t, size_t, SELECTOR);

/*Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	return L1cache_read(addr, len) & (~0u >> ((4 - len) << 3));
	//return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	L1cache_write(addr, len, data);
	//dram_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, current_sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, current_sreg);
	lnaddr_write(lnaddr, len, data);
}