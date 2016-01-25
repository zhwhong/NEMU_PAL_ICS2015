#include <stdlib.h>
#include <time.h>
#include "common.h"

//L1cache 存储空间64KB, cache行64B, 8-way set associate, 标志位（valid）, 随机替换算法, write through, no write allocate
#define W_WIDTH1 6
#define Q_WIDTH1 3
#define R_WIDTH1 7
#define F_WIDTH1 (27-W_WIDTH1-Q_WIDTH1-R_WIDTH1)

#define BLOCK_SIZE1 (1 << W_WIDTH1)       //64B
#define BLOCK_NUM1 (1 << Q_WIDTH1)        //8-way set associative
#define GROUP_NUM1 (1 << R_WIDTH1)        //128 groups

//L2cache 存储空间4MB, cache行64B, 16-way set associate, 标志位（valid, dirty）, 随机替换算法, write back, write allocate
#define W_WIDTH2 6
#define Q_WIDTH2 4
#define R_WIDTH2 12
#define F_WIDTH2 (27-W_WIDTH2-Q_WIDTH2-R_WIDTH2)

#define BLOCK_SIZE2 (1 << W_WIDTH2)		  //64B
#define BLOCK_NUM2 (1 << Q_WIDTH2) 		  //16-way set associative
#define GROUP_NUM2 (1 << R_WIDTH2)		  //4096 groups

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
void update_cache(hwaddr_t, void *, size_t);
void update_dram(hwaddr_t, void *, size_t);
uint32_t L2cache_read(hwaddr_t addr,  size_t len);
void L2cache_write(hwaddr_t addr, size_t len, uint32_t data);

typedef union {
	struct {
		uint32_t w	:W_WIDTH1;
		uint32_t q 	:Q_WIDTH1;
		uint32_t r 	:R_WIDTH1;
		uint32_t f 	:F_WIDTH1;
	};
	uint32_t addr;
}L1cache_addr;

typedef  struct {
	struct {
		uint32_t q 	:Q_WIDTH1;
		uint32_t f 	:F_WIDTH1;
		uint32_t valid	:1;
	};
	uint8_t block[BLOCK_SIZE1];
}L1cache_block;

typedef union {
	struct {
		uint32_t w	:W_WIDTH2;
		uint32_t q 	:Q_WIDTH2;
		uint32_t r 	:R_WIDTH2;
		uint32_t f 	:F_WIDTH2;
	};
	uint32_t addr;
}L2cache_addr;

typedef  struct {
	struct {
		uint32_t q 	:Q_WIDTH2;
		uint32_t f 	:F_WIDTH2;
		uint32_t valid	:1;
		uint32_t dirty	:1;
	};
	uint8_t block[BLOCK_SIZE2];
}L2cache_block;

L1cache_block L1cache[GROUP_NUM1][BLOCK_NUM1];	
L2cache_block L2cache[GROUP_NUM2][BLOCK_NUM2];

void init_L1cache() {
	int i, j;
	for(i = 0; i < GROUP_NUM1; i ++) {
		for(j = 0; j < BLOCK_NUM1; j++){
			L1cache[i][j].valid = 0;
		}
	}
}

void init_L2cache() {
	int i, j;
	for(i = 0; i < GROUP_NUM2; i ++) {
		for(j = 0; j < BLOCK_NUM2; j ++){
			L2cache[i][j].valid = 0;
			//L2cache[i][j].dirty = 0;
		}
	}
}

uint32_t L1cache_read(hwaddr_t addr,  size_t len) {
	int i;
	L1cache_addr caddr;
	caddr.addr = addr;
	uint32_t temp;
	for(i = 0; i < Q_WIDTH1; i++) {
		if (L1cache[caddr.r][i].q == caddr.q && L1cache[caddr.r][i].f == caddr.f && L1cache[caddr.r][i].valid == 1) {
			if (len + caddr.w <= BLOCK_SIZE1) {
				memcpy(&temp, &L1cache[caddr.r][i].block[caddr.w], len);
				return temp;
			}
		} 
	}
	for(i = 0;i < Q_WIDTH1; i++) {
		if (L1cache[caddr.r][i].valid == 0) {
			L1cache[caddr.r][i].q = caddr.q;
			L1cache[caddr.r][i].f = caddr.f;
			L1cache[caddr.r][i].valid = 1;
			update_cache(addr, L1cache[caddr.r][i].block, BLOCK_SIZE1);
			return L2cache_read(addr, len);
		} 
	}
	srand(time(0));
	i = rand()%BLOCK_NUM1;
	L1cache[caddr.r][i].q = caddr.q;
	L1cache[caddr.r][i].f = caddr.f;
	L1cache[caddr.r][i].valid = 1;
	update_cache(addr, L1cache[caddr.r][i].block, BLOCK_SIZE1);
	return L2cache_read(addr, len);
}

void L1cache_read_debug(hwaddr_t addr, size_t len){
	int i;
	L1cache_addr caddr;
	caddr.addr = addr;
	uint32_t temp;
	for(i = 0; i < Q_WIDTH1; i++) {
		if (L1cache[caddr.r][i].q == caddr.q && L1cache[caddr.r][i].f == caddr.f && L1cache[caddr.r][i].valid == 1) {
			if (len + caddr.w <= BLOCK_SIZE1) {
				memcpy(&temp, &L1cache[caddr.r][i].block[caddr.w], len);
				printf("content = %x, cache组号f = %d, 组内块号q = %d\n", temp, caddr.f , caddr.q);
				return ;
			}
		} 
	}
	printf("Can't find in the L1cache！！！\n");
}

void L1cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	int i;
	L1cache_addr caddr;
	caddr.addr = addr;
	for(i = 0; i < Q_WIDTH1; i++){
		if (L1cache[caddr.r][i].q == caddr.q && L1cache[caddr.r][i].f == caddr.f && L1cache[caddr.r][i].valid == 1) {
			memcpy(&L1cache[caddr.r][i].block[caddr.w], &data, len);
			L2cache_write(addr, len, data);
			dram_write(addr, len, data);
			return ;
		}
	}
	L2cache_write(addr, len, data);
}

uint32_t L2cache_read(hwaddr_t addr,  size_t len) {
	int i;
	L2cache_addr caddr;
	L2cache_addr dram_addr;
	caddr.addr = addr;
	uint32_t temp;
	for(i = 0; i < Q_WIDTH2; i++) {
		if (L2cache[caddr.r][i].q == caddr.q && L2cache[caddr.r][i].f == caddr.f && L2cache[caddr.r][i].valid == 1) {
			if (len + caddr.w <= BLOCK_SIZE2) {
				memcpy(&temp, &L2cache[caddr.r][i].block[caddr.w], len);
				return temp;
			}
		} 
	}
	for(i = 0; i < Q_WIDTH2; i++) {
		if (L2cache[caddr.r][i].valid == 0) {
			L2cache[caddr.r][i].q = caddr.q;
			L2cache[caddr.r][i].f = caddr.f;
			L2cache[caddr.r][i].valid = 1;
			L2cache[caddr.r][i].dirty = 0;
			update_cache(addr, L2cache[caddr.r][i].block, BLOCK_SIZE2);
			return dram_read(addr, len);
		} 
	}
	srand(time(0));
	i = rand()%BLOCK_NUM2;
	if (L2cache[caddr.r][i].dirty == 1) {
		dram_addr.q = L2cache[caddr.r][i].q;
		dram_addr.r = caddr.r;
		dram_addr.f = L2cache[caddr.r][i].f;
		dram_addr.w = 0;
		update_dram(dram_addr.addr, L2cache[caddr.r][i].block, BLOCK_SIZE2);
	}
	L2cache[caddr.r][i].q = caddr.q;
	L2cache[caddr.r][i].f = caddr.f;
	L2cache[caddr.r][i].valid = 1;
	L2cache[caddr.r][i].dirty = 0;
	update_cache(addr, L2cache[caddr.r][i].block, BLOCK_SIZE2);
	return dram_read(addr, len);
}

void L2cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	int i;
	L2cache_addr caddr;
	//L2cache_addr dram_addr;
	caddr.addr = addr;
	for(i = 0; i < Q_WIDTH2; i++) {
		if (L2cache[caddr.r][i].q == caddr.q && L2cache[caddr.r][i].f == caddr.f && L2cache[caddr.r][i].valid == 1)  {
			memcpy(&L2cache[caddr.r][i].block[caddr.w], &data, len);
			L2cache[caddr.r][i].dirty = 1;
			return ;
		}
	}
	dram_write(addr, len, data);
	/*
	 for(i = 0; i < Q_WIDTH2; i++) {
	 	if (L2cache[caddr.r][i].valid == 0) {
	 		L2cache[caddr.r][i].q = caddr.q;
	 		L2cache[caddr.r][i].f = caddr.f;
	 		L2cache[caddr.r][i].valid = 1;
	 		update_cache(addr, L2cache[caddr.r][i].block, BLOCK_SIZE2);
	 		memcpy(&L2cache[caddr.r][i].block[caddr.w], &data, len);
	 		L2cache[caddr.r][i].dirty = 1;
	 		return ;
	 	} 
	 }
	 srand(time(0));
	 i = rand()%BLOCK_NUM2;
	 if (L2cache[caddr.r][i].dirty == 1) {
	 	dram_addr.q = L2cache[caddr.r][i].q;
	 	dram_addr.r = caddr.r;
	 	dram_addr.f = L2cache[caddr.r][i].f;
		dram_addr.w = 0;
	 	update_dram(dram_addr.addr, L2cache[caddr.r][i].block, BLOCK_SIZE2);
	 }
	 L2cache[caddr.r][i].q = caddr.q;
	 L2cache[caddr.r][i].f = caddr.f;
	 L2cache[caddr.r][i].valid = 1;
	 update_cache(addr, L2cache[caddr.r][i].block, BLOCK_SIZE2);
	 memcpy(&L2cache[caddr.r][i].block[caddr.w], &data, len);
	 L2cache[caddr.r][i].dirty = 1;
	 return ;
	 */
}