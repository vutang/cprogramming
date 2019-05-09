/*
* @Author: vutang
* @Date:   2019-05-08 09:54:32
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-05-08 22:58:22
*/
#include <stdio.h>
#include "thr_pool.h"

void *counter() 
{
	long long int i = 0;
	while (i < 200 * 10E6)
		i++;
	printf("reach %lld\n", i);
}

int main(int argc, char **argv) 
{
	int i, ret;
	thr_pool_t *main_pool;

	main_pool = thr_pool_create(2, 5, 5, NULL);
	if (main_pool == NULL)
		printf("Create pool fail\n");

	for (i = 0; i < 10; i++) {
		ret = thr_pool_queue(main_pool, counter, NULL);
		if (ret < 0)
			printf("Add job to queue fail\n");
	}

	thr_pool_wait(main_pool);

	thr_pool_destroy(main_pool);
	return 0;
}