/*
* @Author: vutang
* @Date:   2019-04-03 12:58:52
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-03 16:20:43
*/
#include <stdio.h>
#include <stdlib.h>

#define DATA_IN "./rand_number.dat"
#define N 80

#ifdef DEBUG
#define pr_dbg(s, ...) printf("%s#%d[DEBUG]", __FILE__, __LINE__); \
					printf("%s\n", __VA_ARGS__);
#else
#define pr_dbg(s, ...)
#endif

/*Load testing data from file to an array*/
int load_test_data(int *arr) 
{
	FILE *fin;
	int i;

	/*Rerun Generator for new test*/
	system("./ran_generate.elf");

	/*Open INPUT file*/
	if ((fin = fopen(DATA_IN, "r+")) == NULL) {
		printf("Cannot open file!\n");
		return -1;
	}

	for (i = 0; i < N; i++) {
		if (fread(&arr[i], sizeof(int), 1, fin) != 1)  {
	    	printf("EOF or Cannot read data\n");
	    	break;	
    	}
	    printf("[%d] %d ", i, arr[i]);
	}
	printf("\n");

	fclose(fin);
	return 0;
}

int main() 
{
	int i, n, *arr_in, index, ret, min;
	int check_fail = 0;

	arr_in = (int*) malloc(sizeof(int) * N);

	ret = load_test_data(arr_in);
	if (ret)
		return ret;

	index = min_index(arr_in, 0, N-1);
	min = arr_in[index];
	printf("Min: %d, %d\n", index, arr_in[index]);
	
	stable_selection_sort(arr_in, N);

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
		printf("\nSort pass\n");
	else
		printf("\nSort fail at %d\n", i);

	if (min == arr_in[0])
		printf("Min pass\n");

	free(arr_in);
	return 0;
}