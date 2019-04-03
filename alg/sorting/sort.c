/*
* @Author: vutang
* @Date:   2019-04-03 12:57:08
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-03 23:18:29
*/

#include <stdio.h>
#include "sort.h"

static void swap(int *xp, int *yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 

// Return minimum index in a[i:j]
int recursion_min_index(int a[], int m, int n) 
{ 
	if (m > n)
		return -1;
	if (m == n) 
		return m; 
	// Find minimum of remaining elements 
	int k = recursion_min_index(a, m + 1, n); 

	// Return minimum of current and remaining. 
	return (a[m] < a[k])? m : k; 
} 

int min_index(int arr[], int m, int n)
{
	int k, i, j;
	if (m > n) return -1;
	k = m;
	for (i = m; i < n; i++) {
		if (arr[m] > arr[i])
			m = i;
	}
	return m;
}

#define TEST

void selection_sort(int arr[], int n) 
{ 
	int i, j, min_idx; 

	// One by one move boundary of unsorted subarray 
	for (i = 0; i < n-1; i++) { 
		#ifdef TEST
		printf("Round %d\n", i);
		print_arr(arr, n);
		#endif

		// Find the minimum element in unsorted array 
		min_idx = i; 
		for (j = i+1; j < n; j++) 
			if (arr[j] < arr[min_idx]) 
				min_idx = j; 
		// Swap the found minimum element with the first element 
		swap(&arr[min_idx], &arr[i]); 

		#ifdef TEST
		print_arr(arr, n);
		#endif
	} 
} 

void stable_selection_sort(int arr[], int n) 
{ 
	// Iterate through array elements 
	int i, j, min, key;
	for (i = 0; i < n - 1; i++) { 
		// Loop invariant : Elements till a[i - 1] 
		// are already sorted. 

		// Find minimum element from  
		// arr[i] to arr[n - 1]. 
		min = i; 
	    for (j = i + 1; j < n; j++) 
			if (arr[min] > arr[j]) 
				min = j; 

	    // Move minimum element at current i. 
		key = arr[min]; 
		while (min > i) { 
			arr[min] = arr[min - 1]; 
			min--; 
		} 
		arr[i] = key; 
	} 
}

void print_arr(int arr[], int n) {
	int i;
	for (i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");
}

void bubble_sort(int arr[], int n)
{ 
    int i, j, k; 
    for (i = 0; i < n-1; i++) {
    	#ifdef TEST
    	printf("i = %d: ", i);
    	print_arr(arr, n);
    	#endif
        // Last i elements are already in place    
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) 
                swap(&arr[j], &arr[j+1]); 

            #ifdef TEST
            printf("    %d: ", j);
        	print_arr(arr, n);
        	#endif
        }
    }
    return;
} 

void bubble_sort_o1(int arr[], int n) 
{ 
    int i, j; 
    int swapped; 
    for (i = 0; i < n-1; i++) { 
        swapped = 0; 
        for (j = 0; j < n-i-1; j++) { 
            if (arr[j] > arr[j+1]) { 
                swap(&arr[j], &arr[j+1]);
                swapped = 1; 
            }
        } 

        // IF no two elements were swapped by inner loop, then break 
    	if (swapped == 0) 
        	break; 
    } 
} 

#define TEST

void bubble_sort_recursion(int arr[], int n)
{
	if (n == 1)
		return;

	#ifdef TEST
	printf("---");
	print_arr(arr, n);
	#endif 

	/*The biggest one is moved to the end of array*/
	for (int i=0; i<n-1; i++) 
		if (arr[i] > arr[i+1]) 
			swap(&arr[i], &arr[i+1]);
	
	#ifdef TEST
	printf("-->");
	print_arr(arr, n);
	#endif

	bubble_sort_recursion(arr, n-1);

	return;
}