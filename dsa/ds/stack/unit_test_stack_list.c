/*
* @Author: Vu Tang
* @Date:   2019-04-01 22:49:14
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-01 23:12:35
*/

#include <stdio.h>
#include "stack_list.h"

#define NUM_TEST 10

int main() {
	int ret, i;
	ret = stack_list_init();
	if (ret != 0)
		return 1;

	printf("Push to stack\n");

	/*Push a increasing by one sequence to stack*/
	for (i = 0; i < NUM_TEST; i++) {
		ret = stack_list_push(i);
		if (ret)
			printf("Push fail at %d\n", i);
	}

	printf("Flush out stack\n");

	/*Pop all elements in stack*/
	while (!stack_list_is_empty()) {
		stack_list_pop(&i);
		printf("%d\n", i);
	}

	// printf("%d %d %d\n", stack_pop(), stack_pop(), stack_pop());
	// Unpredictable Behavior

	stack_list_del();
	return 0;
	return 0;
}