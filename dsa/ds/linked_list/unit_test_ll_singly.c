/*
* @Author: vutang
* @Date:   2019-04-01 17:41:25
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-01 18:35:33
*/
#include <stdio.h>
#include <stdlib.h>
#include "ll_singly.h"

#define MAX_TEST_SIZE 10

int main() 
{
	int i, ret, *data_ptr;
	linked_list_t *test_list;
	ll_node_t *curr_node;

	test_list = ll_create();
	if (test_list == NULL) {
		printf("Create list fail\n");
		return 0;
	}

	/*Test add head*/
	for (i = 0; i < MAX_TEST_SIZE; i++) {
		data_ptr = malloc(sizeof(int));
		*data_ptr = i + 100;
		ret = ll_add_head(test_list, data_ptr, sizeof(int));
		if (ret) {
			printf("add to list fail at\n");
		}
		free(data_ptr);
	}


	/*Test add tail*/
	for (i = 0; i < MAX_TEST_SIZE; i++) {
		data_ptr = malloc(sizeof(int));
		*data_ptr = i + 200;
		ret = ll_add_tail(test_list, data_ptr, sizeof(int));
		if (ret) {
			printf("add to list fail at\n");
		}
		free(data_ptr);
	}

	curr_node = test_list->head;
	while (curr_node != NULL) {
		printf("%d\n", *(int *)curr_node->object);
		curr_node = curr_node->next;
	}

	ll_destroy(test_list);
	return 0;
}