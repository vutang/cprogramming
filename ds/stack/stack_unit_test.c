/*
* @Author: Vu Tang
* @Date:   2019-03-31 23:05:04
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-03-31 23:48:32
*/

#include <stdio.h>
#include "stack.h"

int main(void)
{
	int ret;
	ret = stack_init(10);
	if (ret != 0)
		return 1;

	ret = stack_push(3);
	if (ret)
		printf("Push fail\n");

	ret = stack_push(4);
	if (ret)
		printf("Push fail\n");

	ret = stack_push(5);
	if (ret)
		printf("Push fail\n");

	printf("Flush out stack\n");

	// while (!stack_is_empty())
		// printf("%d\n", stack_pop());

	printf("%d %d %d\n", stack_pop(), stack_pop(), stack_pop());

	stack_del();
	return 0;
}