#include "common.h"

#define SIZE_CACHEBLOCK 64
#define WAY_L1CACHE 8
#define SIZE_L1CACHE (1 << 16)
#define ROWS_L1CACHE (SIZE_L1CACHE / SIZE_CACHEBLOCK)
//#define WAY_L1CACHE 16
//#define SIZE_L2CACHE (1 << 22)
//#define ROWS_L2CACHE (SIZE_L2CACHE / SIZE_CACHEBLOCK)

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

/*
struct {
	uint32_t way;
	uint32_t num;
	CACHE_BLOCK buf[ROW_L2CACHE];
} L2cache;

L2cache.way = 16;
L2cache.num = ROW_L2CACHE / L2cache.way;
*/

void init_L1cache() {
	int i;
	for(i = 0; i < ROWS_L1CACHE; i ++) {
		L1cache.buf[i].valid = false;
	}
}

uint32_t L1cache_read(hwaddr_t addr, size_t len) {
	
}

void L1cache_read(hwaddrt_t, size_t) {
	
}