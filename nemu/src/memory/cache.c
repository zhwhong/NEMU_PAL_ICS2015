#include <stdlib.h>
#include <time.h>
#include "common.h"

#define W_WIDTH 6
#define Q_WIDTH 3
#define R_WIDTH 7
#define F_WIDTH (27-W_WIDTH-Q_WIDTH-R_WIDTH)

#define BLOCK_SIZE (1 << W_WIDTH)       //64B
#define BLOCK_NUM (1 << Q_WIDTH)        //8-way set associative
#define GROUP_NUM (1 << R_WIDTH)        //128 groups

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
void update_L1cache(hwaddr_t, void *, size_t);

typedef union {
	struct {
		uint32_t w	:W_WIDTH;
		uint32_t q 	:Q_WIDTH;
		uint32_t r 	:R_WIDTH;
		uint32_t f 	:F_WIDTH;
	};
	uint32_t addr;
}cache_addr;

typedef  struct {
	struct {
		uint32_t q 	:Q_WIDTH;
		uint32_t f 	:F_WIDTH;
		uint32_t valid	:1;
	};
	uint8_t block[BLOCK_SIZE];
}cache_block;

cache_block L1cache[GROUP_NUM][BLOCK_NUM];	

void init_L1cache() {
	int i, j;
	for(i = 0; i < GROUP_NUM; i ++) {
		for(j = 0; j < BLOCK_NUM; j++){
			L1cache[i][j].valid = 0;
		}
	}
}

uint32_t L1cache_read(hwaddr_t addr,  size_t len) {
	int i;
	cache_addr caddr;
	caddr.addr = addr;
	uint32_t temp;
	for(i = 0; i < Q_WIDTH; i++) {
		if (L1cache[caddr.r][i].q == caddr.q && L1cache[caddr.r][i].f == caddr.f && L1cache[caddr.r][i].valid == 1) {
			if (len + caddr.w <= BLOCK_SIZE) {
				memcpy(&temp, &L1cache[caddr.r][i].block[caddr.w], len);
				return temp;
			}
		} 
	}
	for(i = 0;i < Q_WIDTH; i++) {
		if (L1cache[caddr.r][i].valid == 0) {
			L1cache[caddr.r][i].q = caddr.q;
			L1cache[caddr.r][i].f = caddr.f;
			L1cache[caddr.r][i].valid = 1;
			update_L1cache(addr, L1cache[caddr.r][i].block, BLOCK_SIZE);
			return dram_read(addr, len);
		} 
	}
	srand(time(0));
	i = rand()%BLOCK_NUM;
	L1cache[caddr.r][i].q = caddr.q;
	L1cache[caddr.r][i].f = caddr.f;
	L1cache[caddr.r][i].valid = 1;
	update_L1cache(addr, L1cache[caddr.r][i].block, BLOCK_SIZE);
	return dram_read(addr, len);
}

void L1cache_read_debug(hwaddr_t addr, size_t len){
	int i;
	cache_addr caddr;
	caddr.addr = addr;
	uint32_t temp;
	for(i = 0; i < Q_WIDTH; i++) {
		if (L1cache[caddr.r][i].q == caddr.q && L1cache[caddr.r][i].f == caddr.f && L1cache[caddr.r][i].valid == 1) {
			if (len + caddr.w <= BLOCK_SIZE) {
				memcpy(&temp, &L1cache[caddr.r][i].block[caddr.w], len);
				printf("content = %x, f = %d, q = %d\n", temp, caddr.f , caddr.q);
				return ;
			}
		} 
	}
	printf("Can't find in the L1cache.\n");
	return ;
}

void L1cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	int i;
	cache_addr caddr;
	caddr.addr = addr;
	for(i = 0; i < Q_WIDTH; i++)
		if (L1cache[caddr.r][i].q == caddr.q && L1cache[caddr.r][i].f == caddr.f && L1cache[caddr.r][i].valid == 1) 
			memcpy(&L1cache[caddr.r][i].block[caddr.w], &data, len);
	dram_write(addr, len, data);
}