#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/*References
	Waiting for Input or Output: 
		https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html
	Linux Programmer's Manual
		http://man7.org/linux/man-pages/man2/select.2.html
*/

int main(void) {
	fd_set rfds;
	struct timeval tv;
	int retval;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);

	/* Wait up to five seconds. */
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	retval = select(1, &rfds, NULL, NULL, &tv);
	/* Don't rely on the value of tv now! */

	/*
	FD_SETSIZE is defined in /sys/select.h and refer to /linux/posix_types.h
	FD_SETSIZE is maximum number of file descriptors that a fd_set object 
	(rffs in this example) can hold
	*/
	printf("FD_SETSIZE: %d\n", FD_SETSIZE);

	if (retval == -1)
		perror("select()");
	else if (retval)
		printf("Data is available now.\n");
		/* FD_ISSET(0, &rfds) will be true. */
	else
		printf("No data within five seconds.\n");

	exit(EXIT_SUCCESS);
}
