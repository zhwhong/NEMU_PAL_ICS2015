#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list(void) 
{
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp(char *expr, uint32_t result)
{
	printf("1\n");

	WP *temp = free_;
	printf("2\n");
	free_ = free_->next;
	printf("3\n");
	if(head == NULL){
		temp->NO = 1;
		printf("4\n");
	}
	else{ 
		temp->NO = head->NO + 1;
		printf("5\n");
	}
	printf("6\n");
	//strcpy(temp->expr, expr);
	printf("7\n");
	temp->result = result;
	printf("8\n");
	temp->next = head;
	printf("9\n");
	head = temp;
	printf("10\n");
	return head;
}

bool free_wp(int num)
{
	if(head == NULL)
	{
		printf("There is no watchpoint!!!\n");
		return false;
	}
	WP *temp = head;
	WP *ptr = temp->next;

	if(temp->NO == num)
	{
		if(ptr == NULL)
		{
			head->next = free_;
			free_ = head;
			head = NULL;
			return true;	
		}
		else
		{
			head = head->next;
			temp->next = free_;
			free_ = temp;
			return true;
		}
	}
	while(ptr && ptr->NO != num)
	{
		temp = ptr;
		ptr = ptr->next;
	}
	if(ptr == NULL)
	{
		printf("No such watchpoint!!!\n");
		return false;
	}
	WP *cur = head;
	while(cur != ptr)
	{
		cur->NO--;
		cur = cur->next;
	}
	temp->next = ptr->next;
	ptr->next = free_;
	free_ = ptr;
	return true;
}

WP* GetHead(void)
{
	return head;
}

