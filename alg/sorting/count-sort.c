/*
* @Author: vutang
* @Date:   2019-04-04 15:20:22
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-04 15:36:50
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sort.h"

#define RANGE 11

void count_sort(int arr[], int n)
{
	int output[n];
	int count[RANGE + 1], i;

	memset(count, 0, sizeof(count));

	print_arr(count, RANGE);

	for (i = 0; i < n; i++) 
		count[arr[i]]++;

	print_arr(count, RANGE);

	for (i = 1; i <= RANGE; ++i)  
		count[i] += count[i-1]; 

	print_arr(count, RANGE);

	for (i = 0; i < n; i++) {
		printf("%d ", count[arr[i]] - 1);
	}
	printf("\n");
}