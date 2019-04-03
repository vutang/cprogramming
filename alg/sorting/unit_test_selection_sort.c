/*
* @Author: vutang
* @Date:   2019-04-03 12:58:52
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-03 13:21:35
*/
#include <stdio.h>
#include <stdlib.h>

#define DATA_IN "./rand_number.dat"
#define N 80

int main() 
{
	FILE *fin, *fout;
	int i, n, *arr_in;
	int check_fail = 0;

	/*Open INPUT file*/
	if ((fin = fopen(DATA_IN, "r+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}

	arr_in = (int*) malloc(sizeof(int) * N);

	for (i = 0; i < N; i++) {
		if (fread(&n, sizeof(int), 1, fin) != 1)  {
	    	printf("EOF or Cannot read data\n");
	    	break;	
	    }
	    arr_in[i] = n;
	    printf("[%d] %d ", i, n);
	}
	printf("\n");

	selection_sort(arr_in, N);

	printf("Sorted:\n");

	for (i = 0; i < N; i++) {
		printf("[%d] %d ", i, arr_in[i]);
		if (i < N - 1)
			if (arr_in[i] > arr_in[i + 1]) {
				check_fail = 1;
				break;
			}

	}
	if (!check_fail)
		printf("\nSuccess\n");
	else
		printf("\nFail at %d\n", i);

	fclose(fin);
	return 0;
}