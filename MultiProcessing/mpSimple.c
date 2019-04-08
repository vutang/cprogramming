#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main () {
	pid_t child_pid;

	printf("The main program process ID is %d\n", (int) getpid());

	child_pid = fork ();

	if (child_pid != 0) {
		printf("Parrent process\n");
		printf("The child process id is %d\n", child_pid);
		system("ps ax| grep mp*");
	}
	else  {
		printf("Child process id %d\n", (int) getpid());
		system("ps ax | grep mp*");
	}

	return 0;
}