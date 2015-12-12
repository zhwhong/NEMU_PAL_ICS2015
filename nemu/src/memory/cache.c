#include "common.h"

#define SIZE_CACHEBLOCK 64
#define WAY_L1CACHE 8
#define SIZE_L1CACHE (1 << 16)
#define ROWS_L1CACHE (SIZE_L1CACHE / SIZE_CACHEBLOCK)
#define WAY_L2CACHE 16
#define SIZE_L2CACHE (1 << 22)
#define ROWS_L2CACHE (SIZE_L2CACHE / SIZE_CACHEBLOCK)

typedef union {
	struct {
		uint32_t b	: 6;
		uint32_t s	: 7;
		uint32_t t	: 27 - 6 - 7;
	};
	uint32_t addr;
} dram_addr;

typedef struct {
	bool valid;
	uint32_t tag;
	uint8_t data[SIZE_CACHEBLOCK];
} CACHE_BLOCK;

struct {
	uint32_t way;
	uint32_t set;
	CACHE_BLOCK buf[ROWS_L1CACHE];
} L1cache;

L1cache.way = WAY_L1CACHE;
L1cache.set = ROWS_L1CACHE / WAY_L1CACHE;

struct {
	uint32_t way;
	uint32_t set;
	CACHE_BLOCK buf[ROWS_L2CACHE];
} L2cache;

L2cache.way = WAY_L2CACHE;
L2cache.set = ROWS_L2CACHE / WAY_L2CACHE;


void init_L1cache() {
	int i;
	for(i = 0; i < ROWS_L1CACHE; i ++) {
		L1cache.buf[i].valid = false;
	}
}

void init_L2cache() {
	int i;
	for(i = 0; i < ROWS_L2CACHE; i ++) {
		L2cache.buf[i].valid = false;
	}
}

bool check_l1cache(hwaddr_t addr, size_t len) {
	int i, j;
	dram_addr temp;
	temp.addr = addr;
	uint32_t b = temp.b;
	uint32_t s = temp.s;
	uint32_t t = temp.t;

	for(i = 0; i < L1cache.way; i++) {
		if(L1cache.buf[s * L1cache.way + i].valid && L1cache.buf[s * L1cache.way + i].tag == t){
			if((b + len) <= SIZE_CACHEBLOCK) {
				return true;
			}
			else {
				temp.addr += SIZE_CACHEBLOCK;
				b = temp.b;
				s = temp.s;
				t = temp.t;
				for(j = 0; j < L1cache.way; j++){
					if(L1cache.buf[s * L1cache.way + j].valid && L1cache.buf[s * L1cache.way + j].tag == t) {
						return true;
				    }
				}// end of for(j)
				return false;
			}
		}
	}// end of for(i)
	return false;
}

uint32_t L1cache_read(hwaddr_t addr, size_t len) {
	int i, j;
	uint8_t result[4];
	dram_addr temp;
	temp.addr = addr;
	uint32_t b = temp.b;
	uint32_t s = temp.s;
	uint32_t t = temp.t;

	for(i = 0; i < L1cache.way; i++) {
		if(L1cache.buf[s * L1cache.way + i].valid && L1cache.buf[s * L1cache.way + i].tag == t){
			if((b + len) <= SIZE_CACHEBLOCK) {
				memcpy(result, L1cache.buf[s * L1cache.way + i].data + b, len);
				break;
			}
			else {
				memcpy(result, L1cache.buf[s * L1cache.way + i].data + b, SIZE_CACHEBLOCK - b);
				temp.addr += SIZE_CACHEBLOCK;
				b = temp.b;
				s = temp.s;
				t = temp.t;
				for(j = 0; j < L1cache.way; j++){
					if(L1cache.buf[s * L1cache.way + j].valid && L1cache.buf[s * L1cache.way + j].tag == t) {
						memcpy(result + SIZE_CACHEBLOCK - b, L1cache.buf[s * L1cache.way + j].data + b, b + len -SIZE_CACHEBLOCK);
						break;
				    }
				}// end of for(j)
				break;
			}
		}
	}// end of for(i)
	return unalign_rw(result, 4);
}

void L1cache_write(hwaddrt_t addr, size_t len, uint32_t data) {
	int i, j;
	uint8_t result[4];
	dram_addr temp;
	temp.addr = addr;
	uint32_t b = temp.b;
	uint32_t s = temp.s;
	uint32_t t = temp.t;

	for(i = 0; i < L1cache.way; i++) {
		if(L1cache.buf[s * L1cache.way + i].valid && L1cache.buf[s * L1cache.way + i].tag == t){
			if((b + len) <= SIZE_CACHEBLOCK) {
				memcpy(L1cache.buf[s * L1cache.way + i].data + b, (uint8_t *)data, len);
				break;
			}
			else {
				memcpy(L1cache.buf[s * L1cache.way + i].data + b, (uint8_t *)data, SIZE_CACHEBLOCK - b);
				temp.addr += SIZE_CACHEBLOCK;
				b = temp.b;
				s = temp.s;
				t = temp.t;
				for(j = 0; j < L1cache.way; j++){
					if(L1cache.buf[s * L1cache.way + j].valid && L1cache.buf[s * L1cache.way + j].tag == t) {
						memcpy(L1cache.buf[s*L1cache.way+j].data+b, (uint8_t *)data + SIZE_CACHEBLOCK - b, b + len -SIZE_CACHEBLOCK);
						break;
				    }
				}// end of for(j)
				break;
			}
		}
	}// end of for(i)
}