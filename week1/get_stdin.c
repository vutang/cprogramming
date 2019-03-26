#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

int main () {
	char ch[100], buf;
	int pipe1[2];
	FILE *fp = fopen("./cmd.log", "w");

	if (pipe(pipe1) < 0) {
		fprintf(stderr, "Cannot create pipe for this process\n");
	}

	dup2(pipe1[1], STDIN_FILENO);

	while (1) {
		// while (fgets(ch, 100, stdin) > 0) {
		// 	// fprintf(fp, "%s\n", ch);
		// 	// fprintf(stdout, "%s\n", ch);
		// 	// fflush(fp);
		// }

		while (read(pipe1[0], &buf, 1) > 0) {
			fprintf(fp, "%c\n", buf);
			fflush(fp);
		}
		printf("2\n");
		sleep(1);
	}

	return 0;
}