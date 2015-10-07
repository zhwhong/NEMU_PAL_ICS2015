#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
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
	printf("hello\n");
	/*
	if(printf("666\n") && NULL == free_)
	{
		printf("The watchpoint is full!!!\n");
		return NULL;
	}
	*/
	printf("555");
	WP *temp = free_;
	printf("4444");
	free_ = free_->next;
	printf("333");
	if(head == NULL)
		temp->NO = 1;
	else 
		temp->NO = head->NO + 1;
	printf("1111");
	strcpy(temp->expr, expr);
	printf("222");
	temp->result = result;
	temp->next = head;
	head = temp;
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

