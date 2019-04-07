/*
* @Author: vutang
* @Date:   2019-04-04 11:55:49
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-04 13:41:53
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "sort.h"

#define SIZE 200

void test_partition() 
{
	int i, *arr_data;
	arr_data = (int *) malloc(SIZE * sizeof(int));
	if (arr_data == NULL)
		return;
	
	create_random_array(arr_data, SIZE);

	partition(arr_data, 0, SIZE - 1);

	free(arr_data);
}

void test_quick_sort()
{
	int i, *arr_data;
	arr_data = (int *) malloc(SIZE * sizeof(int));
	if (arr_data == NULL)
		return;
	
	create_random_array(arr_data, SIZE);

	print_arr(arr_data, SIZE);

	quick_sort(arr_data, 0, SIZE - 1);

	print_arr(arr_data, SIZE);

	if (is_sorted(arr_data, SIZE))
		printf("Pass\n");
	else
		printf("Fail\n");
	free(arr_data);	
}

int main() 
{
	test_quick_sort();
	return 0;
}