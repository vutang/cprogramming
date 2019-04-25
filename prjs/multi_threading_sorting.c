/*
* @Author: Vu Tang
* @Date:   2019-04-25 22:00:34
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-25 23:18:32
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include <time.h>

#define MAX_THREAD_NUM 10

static void *thread_func(void *arg)
{
	int input = *((int *) arg), n;
	char file_name[100];
	long int i, size = 80;
	FILE *fp;

	sprintf(file_name, "./tmp/random%02d", input);
	printf("Create %s\n", file_name);
	
	if ((fp = fopen(file_name, "w+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}

	for (i = 0; i < size * 1024 * 1024 / 4; i++) {
		n = rand();
		fwrite(&n, sizeof(int), 1, fp);
	}

	fclose(fp);
	return NULL;
}

int main(int argc, char *argv[]) 
{
	pthread_t tid[MAX_THREAD_NUM];
	int i, ret;
	int thd_arg[MAX_THREAD_NUM];
	void *thd_ret[MAX_THREAD_NUM];

	clock_t start;

	start = clock();
	if (start == -1) {
		printf("clock() fail\n");
		exit(1);
	}

	for (i = 0; i < MAX_THREAD_NUM; i++) {
		thd_arg[i] = i;
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

	for (i = 0; i < MAX_THREAD_NUM; i++) {
		// printf("Thread return: %f\n", (double) thd_ret[i]);
	}

	printf("Run time: %.2f (secs)\n", \
		(double) (clock() - start) / CLOCKS_PER_SEC);
	return 0;
}