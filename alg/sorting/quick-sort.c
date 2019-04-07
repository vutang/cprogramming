/*
* @Author: vutang
* @Date:   2019-04-04 11:39:38
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-04 13:37:27
*/

#include <stdio.h>
#define TEST

int partition(int arr[], int low, int high) 
{
	int pivot = arr[high];
	int i, pos;

	#ifdef TEST
	printf("[p]-->\n");
	print_arr(&arr[low], high - low + 1);
	#endif 
	/*[low, pos] hold element that is lower or equal than pivot*/
	pos = low - 1;
	for (i = low; i < high; i++) {
		if (arr[i] <= pivot) {
			pos++;
			swap(&arr[pos], &arr[i]);
		}
	}

	swap(&arr[pos + 1], &arr[high]);

	#ifdef TEST
	print_arr(&arr[low], high - low + 1);
	#endif 
	return (pos + 1);
}

void quick_sort(int arr[], int low, int high) 
{
	int pivot;
	if (low < high) {
		#ifdef TEST
		printf("[%d, %d]: ", low, high);
		print_arr(arr, high - low + 1);
		#endif 
		pivot = partition(arr, low, high);
		quick_sort(arr, low, pivot - 1);
		quick_sort(arr, pivot + 1, high);
	}
}