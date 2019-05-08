/*
* @Author: vutang
* @Date:   2019-05-07 17:56:42
* @Last Modified by:   vutang
* @Last Modified time: 2019-05-08 09:02:04
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*For getopt*/

#include <sys/types.h>
#include <sys/stat.h> /*stat()*/

#include <time.h>
#include <string.h>

int main(int argc, char **argv) 
{
	int n = 50, i, j, tmp, ch_cnt = 0, size;
	char number_str[60];
	if (argc != 1)
		n = atoi(argv[1]);

	srand(time(NULL));

	for (i = 0; i < n; i++) {
		size = rand() % 5 + 1;
		for (j = 0; j < size; j++) {
			tmp = rand();
			ch_cnt += sprintf(number_str + ch_cnt, "%d", tmp);
		}
		*(number_str + ch_cnt + 1) = '\n';
		printf("%s\n", number_str);
		ch_cnt = 0;
	}
	return 0;
}