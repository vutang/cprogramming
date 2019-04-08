/*
* @Author: vutang
* @Date:   2019-03-27 18:37:59
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-03-28 00:03:15
*/

#include <stdio.h>
#include <string.h>
#include <errno.h> 

#include "circle_queue.h"

#define LOGFILE_HEADER "./logcmd/log"
#define INPUT_DATA "./data/Tuan1_input.txt"

int main() {    
	queue_t cmd_queue;
	int i = 0;

	char str[STRING_LENGTH], str_tmp[STRING_LENGTH], logid[2] = "0";
	char header[20] = LOGFILE_HEADER, *filename;

	FILE *fin, *fout;

	if ((fin = fopen(INPUT_DATA, "r")) == NULL) {
		fprintf(stderr, "Cannot open file: %s, errno: %d (%s)\n", INPUT_DATA, \
			errno, strerror(errno));
		return 0;
	}

	/*Init queue*/
	ini_queue(&cmd_queue); 

	while (fgets(str, STRING_LENGTH, fin) != NULL) {
		if (en_queue(&cmd_queue, str) < 0) {
			logid[0] = (char) (i + 'A');
			filename = strcat(strcpy(header, LOGFILE_HEADER), logid);
			if ((fout = fopen(filename, "w")) == NULL) {
				fprintf(stderr, "Cannot open file: %s\n", filename);
				return 0;   
			}
			i++;

			while (de_queue(&cmd_queue, str_tmp) >= 0) {
				fprintf(fout, "%s", str_tmp);
			}
			fclose(fout);
			en_queue(&cmd_queue, str);
		}
	}

	fclose(fin);

	/*Flush queue at the end of working session*/
	if (!is_empty(&cmd_queue)) {
		logid[0] = (char) (i + 'A');

		filename = strcat(strcpy(header, LOGFILE_HEADER), logid);
		printf("%s\n", filename);

		if ((fout = fopen(filename, "w")) < 0) {
			fprintf(stderr, "Cannot open file: %s\n", filename);   
			return 0;
		}
		while (de_queue(&cmd_queue, str_tmp) >= 0) 
			fprintf(fout, "%s", str_tmp);

		fclose(fout);
	}
	
	return 1;
}