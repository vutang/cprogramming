#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int s, sig, pid;

	if (argc != 3 || strcmp(argv[1], "--help") == 0) {
		printf("%s pid sig-num\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pid = strtol(argv[1], NULL, 10);
	sig = strtol(argv[2], NULL, 10);
	printf("Got signal, pid: %d, %d\n", sig, pid);

	while (1) {
		s = kill(pid, sig);

		if (sig != 0) {
			if (s == -1)
				printf("kill");

		} else {/*Null signal: process existence check */
			if (s == 0) {
				printf("Process exists and we can send it a signal\n");
			} else {
				if (errno == EPERM)
					printf("Process exists, but we don't have "
						"permission to send it a signal\n");
				else if (errno == ESRCH)
					printf("Process does not exist\n");
				else
					printf("kill");
			}
		}
		sleep(1);
	}

	exit(EXIT_SUCCESS);
}
