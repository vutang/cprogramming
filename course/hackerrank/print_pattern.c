/*
* @Author: Vu Tang
* @Date:   2019-05-05 23:53:32
* @Last Modified by:   vutang
* @Last Modified time: 2019-05-06 11:22:54
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) 
{
	int n = 4;
	if (argc != 1)
		n = atoi(argv[1]);

	int i, j, ch;

	for (i = 0; i < n; i++) {
		for (j = 0; j < 2 * n - 1; j++) {
			if ((j >= i) && (j < 2 * n - 1 - i))
				printf("%d ", n - i);
			else if (j < i)
				printf("%d ", n - j);
			else
				printf("%d ", j - (n - 2));
		}
		printf("\n");
	}

	for (i = n - 2; i >= 0; i--) {
		for (j = 0; j < 2 * n - 1; j++) {
			if ((j >= i) && (j < 2 * n - 1 - i))
				printf("%d ", n - i);
			else if (j < i)
				printf("%d ", n - j);
			else
				printf("%d ", j - (n - 2));
		}
		printf("\n");
	}
	return 0;
}