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

WP *new(char *expr, uint32_t result)
{
	if(free_ == NULL)
	{
		printf("The watchpoint is full!!!\n");
		return NULL;
	}
	WP *temp = head;
	if(temp == NULL){
		head = free_;
		free_ = free_->next;
		head->next = NULL;
		head->NO = 1;
		strcpy(head->expr, expr);
		head->result = result;
		return head;
	}
	while(temp->next)
		temp = temp->next;
	temp->next = free_;
	free_->NO = temp->NO + 1;
	free_->result = result;
	strcpy(free_->expr, expr);
	free_ = free_->next;
	temp->next->next = NULL;
	return temp->next;
}

bool free_wp(int num)
{
	if(head == NULL)
	{
		printf("There is no watchpoint!!!\n");
		return false;
	}
	WP *temp = head;
	WP *ptr = head->next;
	if(temp->NO == num)
	{
		if(temp->next == NULL)
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
			head->NO = 1;
			temp = head;
			while(temp->next)
			{
				temp->next->NO = temp->NO + 1;
				temp = temp->next;
			}
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
	temp->next = ptr->next;
	ptr->next = free_;
	free_ = ptr;
	while(temp->next)
	{
		temp->next->NO = temp->NO + 1;
		temp = temp->next;
	}
	return true;
}

WP* GetHead(void)
{
	return head;
}

