/*
* @Author: Vu Tang
* @Date:   2019-04-25 22:00:34
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-26 16:50:25
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include <time.h>

/*CPU has 8 threads*/
#define MAX_THREAD_NUM 8
#define MAX_THD_MEM 200
#define MAX_PROC_MEM MAX_THD_MEM * MAX_THREAD_NUM

// #define PARTITION_SZ 120
#define FILE_SIZE MAX_PROC_MEM

typedef struct {
	int id;
	int size; 
	int *arr;
} thread_arg_t;

static void *thread_func(void *arg)
{
	thread_arg_t *input = (thread_arg_t *) arg;
	// char file_name[100];
	// int *arr, ret;

	quick_sort(input->arr, input->size);
	printf("[thd%d] sort done\n", input->id);

quit:
	return NULL;
}

int main(int argc, char *argv[]) 
{
	pthread_t tid[MAX_THREAD_NUM];
	int i, j, ret, n;
	void *thd_ret[MAX_THREAD_NUM];

	clock_t start, tick1, tick2;
	char file_name[100];

	thread_arg_t thd_arg[MAX_THREAD_NUM];

	struct timespec start_, finish_;
	double elapsed;

	FILE *fp;

	if (start == -1) {
		printf("clock() fail\n");
		exit(1);
	}

	/*-----Open file----*/
	sprintf(file_name, "random.dat");
	printf("Create %s\n", file_name);
	
	if ((fp = fopen(file_name, "w+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}

	for (j = 0; j < FILE_SIZE * 1024 * 1024 / 4; j++) {
		n = rand();
		fwrite(&n, sizeof(int), 1, fp);
	}

	fclose(fp);

	/*Partitioning...*/
	start = clock();
	clock_gettime(CLOCK_MONOTONIC, &start_);
	if ((fp = fopen(file_name, "r")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}
	for (i = 0; i < MAX_THREAD_NUM; i++) {
		thd_arg[i].arr = (int *) malloc(MAX_THD_MEM * 1024 * 1024 / 4 * \
										sizeof(int));
		
		ret = fread(thd_arg[i].arr, \
			sizeof(int), MAX_THD_MEM * 1024 * 1024 / 4, fp);	
		if (ret <= 0) {
			printf("[ERROR] [%d] fread() fail\n", i);
		}
		else if (ret != MAX_THD_MEM * 1024 * 1024 / 4) 
			printf("[%d] Arr is not full\n", i);

		thd_arg[i].size = ret;
		thd_arg[i].id = i;
		printf("[%d] Array size: %d\n", i, thd_arg[i].size);
	}

	fclose(fp);

	tick1 = clock();
	/*-----Create thread----*/
	for (i = 0; i < MAX_THREAD_NUM; i++) {
		// thd_arg[i] = i;
		ret = pthread_create(&tid[i], NULL, thread_func, &thd_arg[i]);
		if (ret != 0) {
			printf("pthread_create() fail\n");
			exit(1);
		}
	}

	for (i = 0; i < MAX_THREAD_NUM; i++) {
		ret = pthread_join(tid[i], &thd_ret[i]);
		if (ret) {
			printf("pthread_join() fail\n");
			exit(1);
		}
	}

	printf("Process time: %.2f (secs)\n", \
		(double) (clock() - tick1) / CLOCKS_PER_SEC);

	/*Merger*/
	int active_arr_flag = 0;
	int buf_size_remain[MAX_THREAD_NUM], cur_pst[MAX_THREAD_NUM];
	int min_value, min_pos = 0;

	for (i = 0; i < MAX_THREAD_NUM; i++) {
		active_arr_flag |= (1 << i);
		buf_size_remain[i] = thd_arg[i].size;
		cur_pst[i] = 0;
	}
	printf("active_arr_flag: 0x%x\n", active_arr_flag);

	fp = fopen("./output.dat", "w+");
	if (fp == NULL) {
		printf("fopen() fail\n");
		return 0;
	}

	while (active_arr_flag) {
		for (i = 0; i < MAX_THREAD_NUM; i++) {
			if ((buf_size_remain[i] == 0) && 
				(active_arr_flag & (1 << i))) {
				printf("buffer %d is in-active\n", i);
				active_arr_flag &= ~(1 << i);
			}
		}

		if (!active_arr_flag)
			break;

		for (i = 0; i < MAX_THREAD_NUM; i++) {
			if (active_arr_flag & (1 << i)) {
				min_value = thd_arg[i].arr[cur_pst[i]];
				min_pos = i;
				break;
			}
		}

		for (i = 0; i < MAX_THREAD_NUM; i++) {
			if (active_arr_flag & (1 << i)) {
				if (min_value > thd_arg[i].arr[cur_pst[i]]) {
					min_value = thd_arg[i].arr[cur_pst[i]];
					min_pos = i;
				}
			}
		}

		fwrite(&min_value, sizeof(int), 1, fp);
		buf_size_remain[min_pos]--;	
		cur_pst[min_pos]++;	
	}

	fclose(fp);

	for (i = 0; i < MAX_THREAD_NUM; i++) {
		free(thd_arg[i].arr);
	}

	clock_gettime(CLOCK_MONOTONIC, &finish_);
	elapsed = (finish_.tv_sec - start_.tv_sec);
	elapsed += (finish_.tv_nsec - start_.tv_nsec) / 1000000000.0;

	printf("Sorting time: %.2f (secs)\n", \
		(double) (clock() - tick1) / CLOCKS_PER_SEC);

	printf("Muli-threading sorting time: %.2f (secs)\n", \
		(double) (clock() - start) / CLOCKS_PER_SEC);

	printf("Another time estimation %f (secs)\n", elapsed);

	int *arr = malloc(MAX_PROC_MEM * 1024 * 1024 / 4 * sizeof(int));
	if (arr == NULL) {
		printf("malloc() fail\n");
		return 0;
	}

	start = clock();
	if ((fp = fopen(file_name, "r")) == NULL) {
		printf("fopen() fail\n");
		return 0;
	}

	ret = fread(arr, sizeof(int), MAX_PROC_MEM * 1024 * 1024 / 4, fp);
	if (ret <= 0) {
		printf("fread() fail\n");
	}

	printf("Got %d elements (expected %d)\n", \
		ret, MAX_PROC_MEM * 1024 * 1024 / 4);

	fclose(fp); /*Close "input"*/

	quick_sort(arr, ret);

	fp = fopen("output2.dat", "w+");
	for (i = 0; i < ret; i++) {
		fwrite(&arr[i], sizeof(int), 1, fp);
	}
	fclose(fp); /*Close output2.dat*/

	printf("Normal sorting time: %.2f (secs)\n", \
		(double) (clock() - start) / CLOCKS_PER_SEC);
	return 0;
}