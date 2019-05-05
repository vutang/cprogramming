/*
* @Author: Vu Tang
* @Date:   2019-05-05 23:53:32
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-05-06 00:24:09
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
		for (j = 0; j < n; j++) {
			if (j < i)
				ch = n - j;
			else
				ch = n - i;
			printf("%d ", ch);
		}

		for (j = 0; j < n - 1; j++) {
			if (j < n - i - 1)
				ch = n - i;
			else 
				ch = j + 2;
			printf("%d ", ch);	
		}
		printf("\n");
	}

	for (i = n - 1; i > 0; i--) {
		for (j = 0; j < n; j++) {
			ch = n - j;
			if (j < i)
				ch = i;
			printf("%d ", ch);
		}
		printf("\n");
	}
	return 0;
}