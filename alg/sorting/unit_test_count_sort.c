/*
* @Author: vutang
* @Date:   2019-04-04 15:24:00
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-04 15:38:05
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "sort.h"

#define SIZE 10

int main() 
{
	int i, *arr_data;
	arr_data = (int *) malloc(SIZE * sizeof(int));
	if (arr_data == NULL)
		return;

	srand(time(NULL));
	for (i = 0; i < SIZE; i++) {
		arr_data[i] = rand() % 10 + 1;
	}
	print_arr(arr_data, SIZE);

	count_sort(arr_data, SIZE);

	free(arr_data);
}