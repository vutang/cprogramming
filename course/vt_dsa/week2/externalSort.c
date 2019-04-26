#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> /*stat()*/

#include <time.h>

#include "heapSort.h"

#define TMP_DIR "./tmp/part"
#define INPUT_FILE "./uInput.dat"

#define DEFAULT_RAM_SIZE 20									/*Internal memory*/
#define MAX_SIZE_EACH_PART (1024 * 1024 * ram_size / 4)		 	/*Integer*/

#define MAX_PART 20
/*In machine word, Buffer (128 KB) for mergering*/
#define BUCKET_SIZE ((int) (1024 * 1024 * ram_size / (MAX_PART * 2)) / 4)
// #define BUCKET_SIZE 1024 * 512

int main (int argc, char *argv[]) 
{
	/*For open file*/
	FILE *fin, *fout, *fp[MAX_PART];
	char file_name[100];

	int i, j, n, size, ram_size = -1, opt, input_size = -1;

	/*For data buffer*/
	int *arr, *ptr, *part_size;
	int *bucket[MAX_PART + 1];
	
	/*Buffer status*/
	int buf_active_flag = 0; /*Indicate which buffer is active*/
	/*Indicate an active buffer is empty and need to load new data*/
	int buf_empty_flag = 0xFFFFFFFF;

	/*For mergering*/
    int min_value = 0, min_pos = 0;
    int cur_pos[MAX_PART + 1], buf_size_remain[MAX_PART + 1];

    /*File*/
    struct stat sb;
   	
   	/*Time*/
	clock_t tick_begin, tick_end, tick1, tick2;

	/*Histogram for check output
	Because of size of sorting data is big, so the efficient way to 
	check output is using histogram*/
    int *histIn, *histOut; 

    histIn = malloc(sizeof(int) * ((int) (RAND_MAX / 1024)));
    histOut = malloc(sizeof(int) * ((int) (RAND_MAX / 1024)));

    memset(&histIn[0], 0, sizeof(int) * ((int) (RAND_MAX / 1024)));
    memset(&histOut[0], 0, sizeof(int) * ((int) (RAND_MAX / 1024)));

	char cmd[30];

	while ((opt = getopt(argc, argv, "m:i:")) != -1) {
		switch (opt) {
		case 'm':
			ram_size = atoi(optarg);
			break;
		case 'i':
			input_size = atoi(optarg);
			break;
		default:
			break;
		}
	}

	if (ram_size <= 0) ram_size = DEFAULT_RAM_SIZE;
	printf("Ram size %d, size each part %d, bucket size %d\n", \
		ram_size, MAX_SIZE_EACH_PART * 4 / 1024 / 1024, BUCKET_SIZE);

	/*Rerun Generator for new test*/
	if (input_size > 0) {
		sprintf(cmd, "./ranGenerate.elf -n %d", input_size);
		printf("Call %s\n", cmd);
		system(cmd);
	}

	tick_begin = clock();
	if (tick_begin == -1) {
		printf("clock() fail\n");
		exit(1);
	}

    /*Reset*/
    memset(&cur_pos[0], 0, (MAX_PART + 1) * sizeof(int));
    memset(&buf_size_remain[0], 0, (MAX_PART + 1) * sizeof(int));

    /*Get input data info*/
    if (stat(INPUT_FILE, &sb) == -1) {
    	printf("stat() fail\n");
    	exit(1);
    }

    printf("[INFO] Input file size %d (MBs)\n", (int) sb.st_size/1024/1024);
    /*The input data will be breakdown into number_of_part smaller file*/
	int number_of_part = (int) ceil(sb.st_size / (1024 * 1024) * 1.0 / ram_size);

	if (number_of_part > MAX_PART) {
		printf("Exceed MAX_PART: %d\n", number_of_part);
		exit(0);
	}
	
	/*Open INPUT file*/
	if ((fin = fopen(INPUT_FILE, "r+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}

	/*Storing size of each part, maximum size of part is MAX_SIZE_EACH_PART*/
	part_size = malloc(number_of_part * sizeof(int));
	memset(part_size, 0, sizeof(part_size));

	/*Breakdown INPUT file into a set of smaller file to sorting*/
	int count = 0;
	printf("Creating part...\n");
	for (i = 0; i < number_of_part; i++) {
		sprintf(file_name, "./tmp/part%02d", i);
		if ((fout = fopen(file_name, "w+")) == NULL) {
			fprintf(stderr, "Cannot open file: %s\n", file_name);
			return 0;   
		}	

		for (j = 0; j < MAX_SIZE_EACH_PART; j++) {
			if (fread(&n, sizeof(int), 1, fin) != 1)  {
				printf("EOF or Cannot read data\n");
				break;	
			}
			else {	
				/*Calculate histogram*/
				histIn[(int) (n / ((int) (RAND_MAX / 1024)))]++;
				count++;
				fwrite(&n, sizeof(int), 1, fout);
				*(part_size + i) += 1;
			}
		}

		printf("%s, %ld (MBs) ", \
			file_name, *(part_size + i) * sizeof(int) / 1024 / 1024);
		fflush(stdout);
	    fclose(fout);
	}

	fclose(fin);
	printf("\nbreakdown file into %d parts\n", number_of_part);
	
	// for (i = 0; i < number_of_part; i++)
	// 	printf("%d, ", *(part_size + i));
	// printf("\n");

	arr = malloc(MAX_SIZE_EACH_PART * sizeof(int));

	/*Open file and sorting*/
	printf("Sorting part...\n");
	for (i = 0; i < number_of_part; i++) {
		buf_active_flag |= (1 << i);
		/*Read data for sorting*/
		sprintf(file_name, "./tmp/part%02d", i);
		if ((fin = fopen(file_name, "r+")) == NULL) {
	        fprintf(stderr, "Cannot open file: %s\n", file_name);
	        return 0;   
	    }
	    
	    ptr = arr;
	    for (j = 0; j < *(part_size + i); j++) {
	    	fread(ptr++, sizeof(int), 1, fin);
	    }

	    // fread(arr, sizeof(int), *(part_size + i), fin);

	    fclose(fin);

	    tick1 = clock();
		if (tick1 == -1) {
			printf("clock() fail\n");
			exit(1);
		}
		// heapSort(arr, *(part_size + i));
		quick_sort(arr, *(part_size + i));
		usleep(1000);
		tick2 = clock();
		if (tick2 == -1) {
			printf("clock() fail\n");
			exit(1);
		}

		if ((fout = fopen(file_name, "w+")) == NULL) {
	        fprintf(stderr, "Cannot open file: %s\n", file_name);
	        return 0;   
	    }
	    
	    ptr = arr;
	    for (j = 0; j < *(part_size + i); j++) {
	    	fwrite(ptr++, sizeof(int), 1, fout);
	    }

	    // fwrite(arr, sizeof(int), *(part_size + i), fout);

	    fclose(fout);
		printf("%d (%0.2f) ", \
			i, (double) (tick2 - tick1) / CLOCKS_PER_SEC);
		fflush(stdout);
	}

	min_value = *arr;

	free(arr);
	printf("\nSorting each part done, active: 0x%x\n", buf_active_flag);

	/*Merger*/
	/*Read data for mergering*/
	int bucket_size = (int) ((ram_size * 1024 * 1024 / (number_of_part + 1)) / 4);
	printf("bucket_size %d\n", bucket_size);
	for (i = 0; i < number_of_part + 1; i++) {
		// printf("allocate %d\n", i);
		bucket[i] = malloc(bucket_size * sizeof(int));
		if (!bucket[i]) {
			printf("malloc() bucket fail\n");
			exit(0);
		}
	}

	printf("Open partition file\n");
	for (i = 0; i < number_of_part; i++) {
		sprintf(file_name, "./tmp/part%02d", i);
		if ((fp[i] = fopen(file_name, "r+")) == NULL) {
	        fprintf(stderr, "Cannot open file: %s\n", file_name);
	        return 0;   
	    }
	}

	printf("Open output file\n");
	if ((fout = fopen("sortedUinput.dat", "w+")) == NULL) {
        fprintf(stderr, "Cannot open file: %s\n", file_name);
        return 0;   
    }

    printf("Merger\n");
    min_pos = 0;
	tick1 = clock();
	if (tick1 == -1) {
		printf("clock() fail\n");
		exit(1);
	}
	while (buf_active_flag != 0) {
		for (i = 0; i < number_of_part; i++) {
			if ((buf_empty_flag & (1 << i)) && (buf_active_flag & (1 << i))) {
				
				buf_size_remain[i] = fread(bucket[i], sizeof(int), bucket_size, fp[i]);
				// printf("Fill bucket %d, %d items\n", i, buf_size_remain[i]);
				if (buf_size_remain[i] == 0) {
					printf("BUF %d is not ACTIVE\n", i);
					buf_active_flag &= ~(1 << i);
				}
				else {
					buf_empty_flag &= ~(1 << i);
					cur_pos[i] = 0;
				}
			}
		}
		
		/*Find temporary min*/
		for (i = 0; i < number_of_part; i++) {
			if ((buf_active_flag & (1 << i))) {
				min_value = bucket[i][cur_pos[i]];
				break;
			}
		}

		/*Find min in active bucket*/
		for (i = 0; i < number_of_part; i++) {
			if (buf_active_flag & (1 << i)) {
				if (min_value >= bucket[i][cur_pos[i]]) {
					min_pos = i;
					min_value = bucket[i][cur_pos[i]];
				}
			}
		}
		
		bucket[number_of_part][cur_pos[number_of_part]] = min_value;
		/*Update status of buff that has minimum*/
		if (cur_pos[min_pos] == buf_size_remain[min_pos] - 1) {
			buf_empty_flag |= (1 << min_pos);
		}
		else
			cur_pos[min_pos]++;

		// printf("%d, %d\n", min_pos, cur_pos[min_pos]);

		if ((cur_pos[number_of_part] == bucket_size - 1) || 
			(buf_active_flag == 0 && cur_pos[number_of_part] !=0)) {
			// printf("flush bucket: %d, 0x%x\n", cur_pos[MAX_PART], buf_active_flag);
			fwrite(bucket[number_of_part], sizeof(int), cur_pos[number_of_part] + 1, fout);
			// for (i = 0; i < cur_pos[MAX_PART] + 1; i++)
				// fwrite(&bucket[MAX_PART][i], sizeof(int), 1, fout);
			cur_pos[number_of_part] = 0;
		}
		else {
			cur_pos[number_of_part]++;
		}
	}

	printf("free\n");
	for (i = 0; i < number_of_part + 1; i++)
		free(bucket[i]);
	for (i = 0; i < number_of_part; i++)
		fclose(fp[i]);
	tick2 = clock();
	if (tick2 == -1) {
		printf("clock() fail\n");
		exit(1);
	}

	printf("Merger time: %0.2f (secs)\n", \
		(double) (tick2 - tick1) / CLOCKS_PER_SEC);
	/*Check output whether it is in order or not*/
	fseek(fout, 0, SEEK_SET);
	int n1 = 0;	
	i = 0; /*Error position*/
	while(!feof(fout)) {
		fread(&n, sizeof(int), 1, fout);
		// histOut[(int) (n / (RAND_MAX / 1024))]++;
		histOut[(int) (n / ((int) (RAND_MAX / 1024)))]++;
		if (n1 > n) {
			printf("ERROR in %d\n", i);		
			// break;
		}
		n1 = n;
		i++;
	}
	/*End check*/

	fclose(fout);
	free(part_size);

	// system("rm ./tmp/*");

	/*Dumping histogram to file: Check data by using histogram, 
	histogram file will be check by using graph tool like Microsoft Excel or 
	LibreOffice Calc*/
	if ((fout = fopen("./histIn.csv", "w+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}
	for (i = 0; i < 1024; i++)
		fprintf(fout, "%d, ", histIn[i]);

	fclose(fout);

	if ((fout = fopen("./histOut.csv", "w+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}
	size = 0;
	int diff = 0;
	for (i = 0; i < 1024; i++) {
		fprintf(fout, "%d, ", histOut[i]);
		diff = histOut[i] - histIn[i];
		if (diff) {
			printf("Diff at %d, %d\n", i, diff);
			size += diff;
		}
	}

	fclose(fout);

	printf("Diff: %d\n", size);

	int sum[2] = {0, 0};
	for (i = 0; i < 1024; i++) {
		sum[0] += histIn[i];
		sum[1] += histOut[i];
	}
	printf("hist sum: %d, %d\n", sum[0], sum[1]);

	tick_end = clock();
	if (tick_end == -1) {
		printf("clock() fail\n");
		exit(1);
	}

	printf("Total run time: %0.2f (secs) \n", \
		(double) (tick_end - tick_begin) / CLOCKS_PER_SEC);
	return 0;
}