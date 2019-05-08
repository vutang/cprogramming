/*
* @Author: vutang
* @Date:   2019-05-08 09:54:32
* @Last Modified by:   vutang
* @Last Modified time: 2019-05-08 10:29:30
*/
#include <stdio.h>
#include "thr_pool.h"

int main(int argc, char **argv) 
{
	thr_pool_t *main_pool;

	main_pool = thr_pool_create(5, 10, 5, NULL);
	if (main_pool == NULL)
		printf("Create pool fail\n");

	thr_pool_destroy(main_pool);
	return 0;
}