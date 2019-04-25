#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*For getopt*/

#include <sys/types.h>
#include <sys/stat.h> /*stat()*/

#include <time.h>
#include <string.h>

#define DEFAULT_SIZE 80 /*In MegaByte*/
#define OUTPUT "./uInput.dat"

int main(int argc, char *argv[]) 
{
	int n, opt;
	long int i, size = -1;
	char *ptr;
	FILE *fp;
	char file_name[100] = OUTPUT;
	/*Check file*/
	struct stat sb;

	/*Time*/
	clock_t tick1, tick2;

	while ((opt = getopt(argc, argv, "n:o:")) != -1) {
		switch (opt) {
		case 'n':
			size = atoi(optarg);
			break;
		case 'o':
			memcpy(file_name, optarg, strlen(optarg));
			break;
		default:
			break;
		}
	}


	printf("Open %s\n", file_name);
	if ((fp = fopen(file_name, "w+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}

	srand(time(NULL));

	if (size <= 0) {
		printf("Default size: %d\n", DEFAULT_SIZE);
		size = DEFAULT_SIZE;
	}
	else
		printf("Got size: %ld\n", size);
	
	tick1 = clock();
	if (tick1 == -1) {
		printf("clock() fail\n");
		exit(1);
	}

	for (i = 0; i < size * 1024 * 1024 / 4; i++) {
		n = rand();
		fwrite(&n, sizeof(int), 1, fp);
	}
	fclose(fp);

	tick2 = clock();
	if (tick2 == -1) {
		printf("clock() fail\n");
		exit(1);
	}

	printf("Run time: %.2f (secs)\n",
		(double) (tick2 - tick1) / CLOCKS_PER_SEC);

	/*Check file info*/
	if (stat(file_name, &sb) == -1) {
		printf("stat() fail\n");
	}

	printf("Actual file size: %d (KBytes)\n", (int) sb.st_size / 1024);

	return 0;
}