/*
* @Author: vutang
* @Date:   2019-04-04 11:32:32
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-04 13:41:49
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sort.h"

inline void print_arr(int arr[], int n) {
	int i;
	for (i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");
}

inline create_random_array(int arr[], int n)
{
	int i;
	srand(time(NULL));
	for (i = 0; i < n; i++) {
		arr[i] = rand() % 1000 + 1;
	}
}

inline int is_sorted(int arr[], int n) 
{
	int i;
	for (i = 0; i < n; i++)
		if (i < n - 1)
			if (arr[i] > arr[i + 1])
				return 0;
	return 1;
}

void swap(int *xp, int *yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
}

void insert(int arr[], int n, int m) 
{
	int i;
	int tmp = arr[m];

	#ifdef TEST
	print_arr(&arr[n], m - n + 1);
	#endif 

	for (i = m; i > n; i--)
		arr[i] = arr[i-1];
	arr[n] = tmp;

	#ifdef TEST
	print_arr(&arr[n], m - n + 1);
	#endif 
}