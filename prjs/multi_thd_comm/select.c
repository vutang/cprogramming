#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	fd_set rfds;
	struct timeval tv;
	int retval;
	char c;

	/* Watch stdin (fd 0) to see when it has input. */

	FD_ZERO(&rfds);
	FD_SET(STDIN_FILENO, &rfds);

	while (1) {
		/* Wait up to five seconds. */
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		retval = select(1, &rfds, NULL, NULL, &tv);
		/* Don't rely on the value of tv now! */

		printf("(%d:%d)\n", (int) tv.tv_sec, (int) tv.tv_usec);
		if (retval == -1)
			perror("select()");
		else if (retval) {
			printf("Data is available now.\n");
			/* FD_ISSET(0, &rfds) will be true. */
			while ((c = getchar()) != '\n' && c != EOF) { };
		}
		else
			printf("No data within five seconds.\n");
	}

	exit(EXIT_SUCCESS);
}