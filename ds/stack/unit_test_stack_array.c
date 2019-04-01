/*
* @Author: Vu Tang
* @Date:   2019-03-31 23:05:04
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-01 18:36:28
*/

#include <stdio.h>
#include "stack_array.h"

#define NUM_TEST 5

int main(void)
{
	int ret, i;
	ret = stack_init(10);
	if (ret != 0)
		return 1;

	printf("Push to stack\n");

	/*Push a increasing by one sequence to stack*/
	for (i = 0; i < NUM_TEST; i++) {
		ret = stack_push(i);
		if (ret)
			printf("Push fail at %d\n", i);
	}

	printf("Flush out stack\n");

	/*Pop all elements in stack*/
	while (!stack_is_empty())
		printf("%d\n", stack_pop());

	// printf("%d %d %d\n", stack_pop(), stack_pop(), stack_pop());
	// Unpredictable Behavior

	stack_del();
	return 0;
}