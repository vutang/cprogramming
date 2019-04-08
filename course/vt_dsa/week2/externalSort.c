#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "heapSort.h"

#define TMP_DIR "./tmp/part"

#define FILE_SIZE 80 		/*In MegaByte*/
#define RAM_SIZE 20	
#define NUM_INT 1024 * 1024 * RAM_SIZE / 4
#define MAX_PART 8
#define BUF_SIZE 32768 		/*In machine word*/

int main  () {
	/*For open file*/
	FILE *fin, *fout, *fp[8];
	char *filename;
	char header[20] = TMP_DIR, part[2] = "0";

	int i, j, n;

	/*For data buffer*/
	int *arr, *ptr, *part_size;
	int buf[MAX_PART + 1][BUF_SIZE]; /*Buffer (128 KB) for mergering*/
	
	/*Buffer status*/
	char buf_active_flag = 0; /*Indicate which buffer is active*/
	char buf_empty_flag = 0xFF; /*Indicate an active buffer is empty and need to load new data*/

	/*For mergering*/
    int min_value = 0, min_pos = 0;
    int cur_pos[9], buf_size_remain[8];

    /*Reset*/
    memset(&cur_pos[0], 0, 9 * sizeof(int));
    memset(&buf_size_remain[0], 0, sizeof(int) * 8);

    /*Histogram for check output
      Because of size of sorting data is big, so the efficient way to check output is using histogram*/
    int histIn[1024], histOut[1024]; 
    memset(&histIn[0], 0, 1024);
    memset(&histOut[0], 0, 1024);

    /*The input data will be breakdown into number_of_part smaller file*/
	int number_of_part = (int) ceil(FILE_SIZE * 1.0 / RAM_SIZE);

	/*Rerun Generator for new test*/
	system("./ranGenerate");
	
	/*Open INPUT file*/
	if ((fin = fopen("./uInput.dat", "r+")) == NULL) {
		printf("Cannot open file!\n");
		return 0;
	}

	/*Storing size of each part, maximum size of part is NUM_INT*/
	part_size = malloc(number_of_part * sizeof(int));
	memset(part_size, 0, sizeof(part_size));

	/*Breakdown INPUT file into a set of smaller file to sorting*/
	for (i = 0; i < number_of_part; i++) {
		filename = strcat(strcpy(header, TMP_DIR), part);
		if ((fout = fopen(filename, "w+")) < 0) {
	        fprintf(stderr, "Cannot open file: %s\n", filename);
	        return 0;   
	    }	

	    for (j = 0; j < NUM_INT; j++) {
		    if (fread(&n, sizeof(int), 1, fin) != 1)  {
		    	printf("EOF or Cannot read data\n");
		    	break;	
		    }
		    else {	
		    	/*Calculate histogram*/
		    	histIn[(int) (n / (RAND_MAX / 1024))]++;
		    	fwrite(&n, sizeof(int), 1, fout);
		    	*(part_size + i) += 1;
		    }
		}

	    fclose(fout);
	    part[0]++;
	}

	fclose(fin);
	printf("FILE BREAKDOWN DONE\n");
	// for (i = 0; i < number_of_part; i++)
	// 	printf("%d, ", *(part_size + i));
	// printf("\n");

	arr = malloc(NUM_INT * sizeof(int));

	/*Open file and sorting*/
	part[0] = '0';
	for (i = 0; i < number_of_part; i++) {
		buf_active_flag |= (char) (1 << i);
		/*Read data for sorting*/
		filename = strcat(strcpy(header, TMP_DIR), part);
		if ((fin = fopen(filename, "r+")) < 0) {
	        fprintf(stderr, "Cannot open file: %s\n", filename);
	        return 0;   
	    }
	    
	    ptr = arr;
	    for (j = 0; j < *(part_size + i); j++) {
	    	fread(ptr++, sizeof(int), 1, fin);
	    }

	    fclose(fin);

		heapSort(arr, *(part_size + i));

		if ((fout = fopen(filename, "w+")) < 0) {
	        fprintf(stderr, "Cannot open file: %s\n", filename);
	        return 0;   
	    }
	    
	    ptr = arr;
	    for (j = 0; j < *(part_size + i); j++) {
	    	fwrite(ptr++, sizeof(int), 1, fin);
	    }

	    fclose(fout);

	    part[0]++;
	}

	min_value = *arr;

	free(arr);
	printf("SORTING FOR EACH PART DONE\n");

	/*Merger*/
	/*Read data for mergering*/
	part[0] = '0';
	for (i = 0; i < number_of_part; i++) {
		filename = strcat(strcpy(header, TMP_DIR), part);
		if ((fp[i] = fopen(filename, "r+")) == NULL) {
	        fprintf(stderr, "Cannot open file: %s\n", filename);
	        return 0;   
	    }
	    part[0]++;
	}

	if ((fout = fopen("sortedUinput.dat", "w+")) < 0) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return 0;   
    }

    min_pos = 0;
	while (buf_active_flag != 0) {
		for (i = 0; i < number_of_part; i++) {
			if ((buf_empty_flag & (1 << i)) && (buf_active_flag & (1 << i))) {
				// printf("Read buffer %d\n", i);
				buf_size_remain[i] = fread(&buf[i], sizeof(int), BUF_SIZE, fp[i]);
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
				min_value = buf[i][cur_pos[i]];
				break;
			}
		}

		/*Find min in active buf*/
		for (i = 0; i < number_of_part; i++) {
			if ((buf_active_flag & (1 << i)) != 0) {
				if (min_value >= buf[i][cur_pos[i]]) {
					min_pos = i;
					min_value = buf[i][cur_pos[i]];
				}
			}
		}

		/*Update status of buff that has minimum*/
		cur_pos[min_pos]++;
		if (cur_pos[min_pos] == buf_size_remain[min_pos]) {
			buf_empty_flag |= (1 << min_pos);
		}
		
		if (cur_pos[8] == BUF_SIZE) {
			fwrite(buf[8], sizeof(int), BUF_SIZE, fout);
			cur_pos[8] = 0;
		}
		buf[8][cur_pos[8]] = min_value;
		cur_pos[8]++;
	}

	for (i = 0; i < number_of_part; i++) {
		fclose(fp[i]);
	}

	/*Check output whether it is in order or not*/
	fseek(fout, 0, SEEK_SET);
	int n1 = 0;	
	i = 0; /*Error position*/
	while(fread(&n, sizeof(int), 1, fout) != 0) {
		histOut[(int) (n / (RAND_MAX / 1024))]++;
		if (n1 > n) {
			printf("ERROR in %d\n", i);		
			break;
		}
		n1 = n;
		i++;
	}
	/*End check*/

	fclose(fout);
	free(part_size);

	system("rm ./tmp/*");

	/*Check data by using histogram, histogram file will be check by using graph tool like Microsoft Excel or LibreOffice Calc*/
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
	for (i = 0; i < 1024; i++)
		fprintf(fout, "%d, ", histOut[i]);

	fclose(fout);
	return 0;
}