#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 80

int main(int argc, char *argv[]) {
	int i, n;
	char *ptr;
	FILE *fp;

	if ((fp = fopen("./rand_number.dat", "w+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}

	srand(time(NULL));

	for (i = 0; i < SIZE; i++) {
		n = rand();
		fwrite(&n, sizeof(int), 1, fp);
	}

	fclose(fp);
	return 0;
}