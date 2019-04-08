/*
* @Author: vutang
* @Date:   2018-11-03 08:53:19
* @Last Modified by:   vutang
* @Last Modified time: 2018-11-03 08:58:40
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h> 
#include <fcntl.h>

int main() {
	int fd;

	fd = open("/home/vutt6/err.c", O_RDONLY);
	if (fd < 0) {
		printf("%s\n", strerror(errno));
	}
	return 0;
}
