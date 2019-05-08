#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INT 110
#define SIZE_MB 1024*1024*100  // 24 MB * 4byte(int) = 96MB
#define SIZE_10_MB 1024*1024*20
#define MAX_RANDOM 1000000

/** 
 * take_data
 * This function takes data from file "f" into double pointer buf
 * @buf: double pointer stores data
 * @f: file that stores primary data
 * Return: len
*/
int take_data(char **buf,  FILE *f)
{
	int len;
	for (len = 0; len < MAX_INT; len++) {
		if (feof(f)) {
			if (*buf[len - 1] == 0) buf[--len] = NULL;
			else buf[len] = NULL;
			break;
		}
		buf[len] = (char *) malloc(50 * sizeof(char));
		while (!buf[len] ) {
			printf("Reallocating buf[%d]\n", len);
			buf[len] = (char *) malloc(50 * sizeof(char));
		}
		fscanf(f, "%s", buf[len]);
	}
	return len;
}
/** 
 * swap
 * This function swaps two pointer point each their string 
 * @A: double pointer
 * @i, j: index of pointer need to be swaped
 * Return: void
*/
void swap(char **A, int i, int j)
{
	char *temp = A[i];
	A[i] = A[j];
	A[j] = temp;
}

int compare(char *s1, char *s2) 
{
	if (strlen(s1) < strlen(s2)) return -1;
	if (strlen(s1) > strlen(s2)) return 1;
	if (strlen(s1) == strlen(s2)) {
		return strcmp(s1, s2);
	}
}
/** 
 * partion
 * This function sorts at least one data in array to right position
 * @A: array has data need to be sorted
 * @begin: the begin element of array A
 * @end: the end element of array A
 * Return: log - pivot position
*/
int partion(char **A, int begin, int end)
{
	int left = begin, right = end, log = left;
	while (left != right) {
		while (right >= left && compare(A[right], A[log]) >= 0) right--;
			if (right == left - 1) return log; 
			if (right != log) {
				swap(A, right, log);
				// int temp = A[right];
				// A[right] = A[log];
				// A[log] = temp;
				// log = right;
			}
		while (compare(A[left], A[log]) < 0) left ++;
		if (left != log) {
			swap(A, left, log); 

			// int temp = A[left];
			// 	A[left] = A[log];
			// 	A[log] = temp;
			// 	log = left;
		}
 	}
 	return log;
}


/** 
 * quick_sort
 * This function sorts array follow quick sort type
 * @A: array has data need to be sorted
 * @begin: the begin element of array A
 * @end: the end element of array A
 * Return: void
*/
void quick_sort(char **A, int begin, int end)
{
	int log;
	if (begin < end) {
		log = partion(A, begin, end);
		quick_sort(A,begin,log - 1);
		quick_sort(A,log + 1, end);
	}
}


/** 
 * sorting_len
 * This function sorts len of buf[x] follow bubble sort type
 * @A: string has data need to be sorted
 * @len: the length element of array A
 * Return: void
*/
void sorting_len(char **A, int len)
{
	void swap(char **A, int i, int j);
	int i , j, flag = 0, k = 0;
	for (i = 0; i < len; i ++) {
		flag = 0;
		for (j = 0; j < len - i - 1; j++) {
			k++;
			if (strlen(A[j]) > strlen(A[j + 1])) {
				flag = 1;
				swap(A, j + 1, j);
			}
		}
		if (flag == 0) break;
	}
}

/** 
 * check_sort
 * This function checks array if sorted or not
 * @A: array has data need to be sorted
 * @len: length of array A 
 * Return: 0 if sorted or 1 unsorted
*/
int check_sort(char **A) {
	int i = 0;
	while (A[i + 1] != NULL) {
		if (strlen(A[i]) == strlen(A[i + 1])) {
			if (strcmp(A[i], A[i + 1]) > 0) return -1;
		}
		else if (strlen(A[i]) > strlen(A[i + 1])) return -1;
		i++;
	}
	return 0;
}

/** 
 * compare_file
 * This function compares element in 2 file
 * @f_pri, f_sec: files
 * Return: 0 if they have same element or 1 if not
*/
int compare_file(FILE *f_pri, FILE *f_sec) 
{
	int i, j, len, A[11], B[11];
	int *read = (int *) malloc(SIZE_MB * sizeof(int));
	while(!read) {
		printf("waiting *read\n");
		read = (int *) malloc(SIZE_MB * sizeof(int));
	}
	fseek(f_pri, 0, SEEK_SET);
	while(!feof(f_pri)) {
		len = fread(read, sizeof(int), SIZE_MB, f_pri);
		for (i = 0; i < len; i++) {
			for (j = 1; j <= 11; j++) {
				if (read[i] < 200000000 * j) {
					A[j - 1]++;
					break;
				}
			}
		}
	}
	/******************************************/
	fseek(f_sec, 0, SEEK_SET);
	while(!feof(f_sec)) {
		len = fread(read, sizeof(int), SIZE_MB, f_sec);
		for (i = 0; i < len; i++) {
			for (j = 1; j <= 11; j++) {
				if (read[i] < 200000000 * j) {
					B[j - 1]++;
					break;
				}
			}
		}
	}
	fseek(f_pri, 0, SEEK_SET);
	fseek(f_sec, 0, SEEK_SET);
	free(read);
	for (i = 0; i < 11; i++) {
		printf("A[%d] = %d\n", i, A[i]);
		printf("B[%d] = %d\n", i, B[i]);
	}
	for (i = 0; i < 11; i++) if (A[i] != B[i]) return 1;
	return 0;
}



int main(int argc, char const *argv[])
{
	FILE *f = NULL, *f1 = NULL;
	char **buf = NULL;
	int i = 0, len;
	if (argc != 2) {
		printf("You must enter the input file need to be execute\n");
		return - 1;
	}
	f = fopen(argv[1], "r");
	f1 = fopen("output2.txt","w+");
	if (!f) {
		printf("Can not find your input file\n");
		return -1;
	}
	if (!f1) {
		printf("Can not open your output file\n");
		return -1;
	}
	buf = (char **) malloc(MAX_INT * sizeof(char *));
	while (!buf) {
		printf("Reallocating buf ...\n");
		buf = (char **) malloc(MAX_INT * sizeof(char *));
	}
	/* Take data from file input txt */
	len = take_data(buf, f);
	/* sort len */
	printf("quick_sort\n");
	quick_sort(buf, 0, len - 1);
	if (check_sort(buf)) printf("SORTED NOT OK\n");
	else printf("OK\n");
	for (i = 0; buf[i] != NULL; i++) {
		fprintf(f1, "%s\n", buf[i]);
		free(buf[i]);
	}
	free(buf);
	fclose(f1);
	fclose(f);
	return 0;
}