#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*Disk files are always ready to read (but the read might return 
0 bytes if you're already at the end of the file), so you can't 
use select() on a disk file to find out when new data is added to the file.

POSIX says:

File descriptors associated with regular files shall always select 
true for ready to read, ready to write, and error conditions.*/

int main(int argc, char **argv) 
{
	fd_set rfds;
	struct timeval tv;
	int fd, ret;

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Fail at open file");
		exit(EXIT_FAILURE);
	}

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	while (1) {
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		ret = select(1, NULL, &rfds, NULL, &tv);

		if (ret == -1)
			perror("select()");
		else if (ret) {
			printf("Got data\n");
		}
		else
			printf("Time out\n");
	}

	close(fd);

	exit(EXIT_SUCCESS);
}