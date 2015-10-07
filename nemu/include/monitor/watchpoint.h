#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	char *expr;
	uint32_t result;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */


} WP;

void init_wp_list();
WP *new_wp(char *expr, uint32_t result);
WP *GetHead(void);
bool free_wp(int num);

#endif
