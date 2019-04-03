/*
* @Author: Vu Tang
* @Date:   2019-04-03 22:09:31
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-03 23:54:08
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort.h"

#define N 20

int main()
{
	int i, *arr_data, check_fail = 0;

	srand(time(NULL));

	arr_data = (int *) malloc(N * sizeof(int));
	if (arr_data == NULL)
		return 1;

	for (i = 0; i < N; i++) {
		arr_data[i] = rand() % 100 + 1;
	}

	insertion_sort(arr_data, N);

	for (i = 0; i < N; i++) {
		if (i < N - 1)
			if (arr_data[i] > arr_data[i + 1]) {
				check_fail = 1;
				break;
			}

	}

	if (!check_fail)
		printf("\nSort pass\n");
	else
		printf("\nSort fail at %d\n", i);

	free(arr_data);
	return 0;
}