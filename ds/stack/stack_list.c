/*
* @Author: vutang
* @Date:   2019-04-01 18:41:19
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-01 23:12:47
*/

#include <stdlib.h>
#include "stack_list.h"
#include "../linked_list/ll_singly.h"

static linked_list_t *stack_list = NULL;

int stack_list_init() 
{
	stack_list = ll_create();
	if (stack_list == NULL)
		return -1;
	return 0;
}

void stack_list_del(void) 
{
	ll_destroy(stack_list);
}

int stack_list_push(int data) 
{
	int ret;
	ret = ll_add_head(stack_list, &data, sizeof(int));
	if (ret)
		return -1;
	return 0;
}

int stack_list_pop(int *dout) 
{
	if(stack_list_is_empty())
		return -1;
	ll_node_t *curr;

	*dout = *(int *)stack_list->head->object;
	
	ll_del_head(stack_list);

	return 0;
}

int stack_list_is_empty(void) 
{
	if (stack_list == NULL)
		return 1;
	return (stack_list->head == NULL);
}