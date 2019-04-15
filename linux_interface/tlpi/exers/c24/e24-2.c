/*
* @Author: Vu Tang
* @Date:   2019-04-15 11:59:10
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-15 13:19:21
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) 
{
	switch (vfork()) {
	case -1: printf("vfork fail\n");
	case 0: 
		printf("in child\n");
		if (close(STDERR_FILENO) == -1)
			printf("close - child fail\n");
		printf("exit child\n");
		sleep(3);
		_exit(EXIT_SUCCESS);
	default: break;
	}

	printf("in parrent\n");
	if (close(STDERR_FILENO) == -1)
		printf("close - parrent fail\n");
	if (close(STDERR_FILENO) == -1)
        printf("close\n");

	exit(EXIT_SUCCESS);
}