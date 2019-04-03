/*
* @Author: vutang
* @Date:   2019-04-03 12:57:08
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-03 16:20:36
*/

#include "stdio.h"

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
}

void selection_sort(int arr[], int n) 
{ 
    int i, j, min_idx; 
  
    // One by one move boundary of unsorted subarray 
    for (i = 0; i < n-1; i++) { 
        // Find the minimum element in unsorted array 
        min_idx = i; 
        for (j = i+1; j < n; j++) 
          if (arr[j] < arr[min_idx]) 
            min_idx = j; 
  
        // Swap the found minimum element with the first element 
        swap(&arr[min_idx], &arr[i]); 
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