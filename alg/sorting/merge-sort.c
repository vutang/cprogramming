/*
* @Author: vutang
* @Date:   2019-04-04 10:04:02
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-04 14:44:47
*/

#include <stdio.h>
#include "sort.h"

int min(int x, int y) { return (x<y)? x :y; }

void merge_sorted_arrs(int arr[], int l, int m, int r)
{
	int i, j; 

	for (i = m + 1; i <= r; i++) {
		for (j = l; j < i; j++)
			if (arr[j] >= arr[i])
				break;
		insert(arr, j, i);
	}
}

#define TEST

void merge_sort(int arr[], int l, int r)
{
	if (r <= l)
		return;

	int m = (l + r) / 2;

	#ifdef TEST
	printf("[d]-->[%d, %d, %d]: ", l, m, r);
	print_arr(&arr[l], r - l + 1);
	#endif

	merge_sort(arr, l, m);
	merge_sort(arr, m + 1, r);

	merge_sorted_arrs(arr, l, m, r);

	#ifdef TEST
	printf("[m]-->[%d, %d, %d]: ", l, m, r);
	print_arr(&arr[l], r - l + 1);
	#endif
}

void merge_sort_iterative(int arr[], int n)
{
	int curr_size; // For current size of subarrays to be merged 
				// curr_size varies from 1 to n/2 
	int left_start; // For picking starting index of left subarray 
					// to be merged
	int mid, right_end;

	for (curr_size = 1; curr_size <= n-1; curr_size = 2 * curr_size) { 
		printf("curr_size: %d\n", curr_size);
		for (left_start=0; left_start<n-1; left_start += 2*curr_size) {
			mid = left_start + curr_size - 1; 
			right_end = min(left_start + 2*curr_size - 1, n-1); 
			printf("[%d, %d, %d]\n", left_start, \
				mid, right_end);
			merge_sorted_arrs(arr, left_start, mid, right_end);
		}
	}
}
