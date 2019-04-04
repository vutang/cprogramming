/*
* @Author: vutang
* @Date:   2019-04-04 10:21:43
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-04 14:30:07
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "sort.h"

#define SIZE 13

#ifdef DEBUG
#define pr_dbg(s, ...) printf("%s#%d[DEBUG]", __FILE__, __LINE__); \
					printf("%s\n", __VA_ARGS__);
#else
#define pr_dbg(s, ...)
#endif

void test_insert() 
{
	int i, *arr_data;
	arr_data = (int *) malloc(SIZE * sizeof(int));
	if (arr_data == NULL)
		return;

	create_random_array(arr_data, SIZE);

	print_arr(arr_data, SIZE);

	insert(arr_data, 1, 8);

	print_arr(arr_data, SIZE);

	free(arr_data);
}

#define SIZE1 5 /*sub-array 1*/
#define SIZE2 4 /*sub-array 2*/
void test_merge_sorted_arrs() 
{
	int *arr_data, *arr_tmp, i;
	arr_data = (int *) malloc((SIZE1 + SIZE2) * sizeof(int));
	if (arr_data == NULL) {
		pr_dbg("Allocate memory fail");
		return;
	}

	memset(arr_data, 0, (SIZE1 + SIZE2) * sizeof(int));

	/*Create sub array 1*/
	arr_tmp = (int *) malloc(SIZE1 * sizeof(int));
	if (arr_tmp == NULL) {
		pr_dbg("Allocate memory fail");
		goto free_arr_data;
	}

	create_random_array(arr_tmp, SIZE1);

	selection_sort(arr_tmp, SIZE1);

	for (i = 0; i < SIZE1; i++) {
		arr_data[i] = arr_tmp[i];
	}

	print_arr(arr_tmp, SIZE1);
	print_arr(arr_data, SIZE1 + SIZE2);

	free(arr_tmp);

	/*Create sub array 2*/
	arr_tmp = (int *) malloc(SIZE2 * sizeof(int));
	if (arr_tmp == NULL) {
		pr_dbg("Allocate memory fail");
		goto free_arr_data;
	}

	sleep(1);
	create_random_array(arr_tmp, SIZE2);

	selection_sort(arr_tmp, SIZE2);

	for (i = 0; i < SIZE2; i++) {
		arr_data[i + SIZE1] = arr_tmp[i];
	}

	print_arr(arr_tmp, SIZE2);
	print_arr(arr_data, SIZE1 + SIZE2);

	free(arr_tmp);

	/*Test merge*/
	merge_sorted_arrs(arr_data, 0, SIZE1 - 1, SIZE1 + SIZE2 - 1);
	print_arr(arr_data, SIZE1 + SIZE2);

free_arr_data:
	free(arr_data);
	return;
}

int test_merge_sort() 
{
	int *arr_data, i, check_fail;

	arr_data = (int *) malloc(SIZE * sizeof(int));
	if (arr_data == NULL)
		return;

	create_random_array(arr_data, SIZE);
	print_arr(arr_data, SIZE);

	merge_sort(arr_data, 0, SIZE-1);

	print_arr(arr_data, SIZE);

	if (is_sorted(arr_data, SIZE))
		printf("Pass\n");
	else
		printf("Fail\n");

	free(arr_data);
	return;
}

int test_merge_sort_iterative() 
{
	int *arr_data, i, check_fail;

	arr_data = (int *) malloc(SIZE * sizeof(int));
	if (arr_data == NULL)
		return;

	create_random_array(arr_data, SIZE);
	print_arr(arr_data, SIZE);

	merge_sort_iterative(arr_data, SIZE);

	free(arr_data);
	return;	
}

int main()
{
	test_merge_sort_iterative();
}