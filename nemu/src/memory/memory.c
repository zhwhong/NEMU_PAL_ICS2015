#include "common.h"
#include "burst.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	uint32_t offset = addr & BURST_MASK;
	uint8_t result[len];
	dram_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t rank = temp.rank;
	uint32_t bank = temp.bank;
	uint32_t row = temp.row;
	uint32_t col = temp.col;
	if(rowbufs[rank][bank].valid && rowbufs[rank][bank].row_idx == row) {
		if(offset + len <= BURST_LEN){
			return unalign_rw(rowbufs[rank][bank].buf + col + offset, len);
		}
		else{
			memcpy(result, rowbufs[rank][bank].buf + col + offset, BURST_LEN - offset);
			temp.addr = (addr + BURST_LEN) & ~BURST_MASK;
			rank = temp.rank;
			bank = temp.bank;
			row = temp.row;
			col = temp.col;
			memcpy(result + BURST_LEN - offset, rowbufs[rank][bank].buf + col, len + offset - BURST_LEN);
			return unalign_rw(result, len);
		}
	}
	else{
		return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	}
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {

	dram_write(addr, len, data);
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
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

