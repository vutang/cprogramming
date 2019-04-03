/*
* @Author: Vu Tang
* @Date:   2019-04-03 22:09:31
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-03 23:13:24
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort.h"

#define SIZE 5

int main()
{
	int i, *arr_data;

	srand(time(NULL));

	arr_data = (int *) malloc(SIZE * sizeof(int));
	if (arr_data == NULL)
		return 1;

	for (i = 0; i < SIZE; i++) {
		arr_data[i] = rand() % 100 + 1;
	}

	selection_sort(arr_data, SIZE);

	free(arr_data);
	return 0;
}