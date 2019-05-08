/*
* @Author: vutang
* @Date:   2019-05-08 09:30:39
* @Last Modified by:   vutang
* @Last Modified time: 2019-05-08 09:36:46
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	if (argc < 3)
		exit(0);
	int n = strcmp(argv[1], argv[2]);
	if (n == 0)
		printf("=");
	else if (n < 0)
		printf("<");
	else
		printf(">");
	printf(": %d\n", n);
	exit(1);
}