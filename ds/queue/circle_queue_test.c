/*
* @Author: vutang
* @Date:   2019-03-27 18:37:59
* @Last Modified by:   vutang
* @Last Modified time: 2019-03-27 18:40:27
*/

#include <stdio.h>
#include <string.h>

#include "circle_queue.h"

int main() {    
	queue_t cmd_queue;
	int i = 0;

	char str[STRING_LENGTH], str_tmp[STRING_LENGTH], logid[2] = "0";
	char header[20] = LOGFILE_HEADER, *filename;

	FILE *fin, *fout;

	if ((fin = fopen("Tuan1_input.txt", "r")) < 0) {
		fprintf(stderr, "Cannot open file: %s\n", "Tuan1_input.txt");
		return 0;
	}
	if ((fout = fopen("./logcmd/log0", "w")) < 0) {
		fprintf(stderr, "Cannot open file: %s\n", "./logcmd/log0");   
		return 0;
	}

	/*Init queue*/
	ini_queue(&cmd_queue); 

	while (fgets(str, STRING_LENGTH, fin) != NULL) {
		if (en_queue(&cmd_queue, str) < 0) {
			if (i > 0) {
				fclose(fout);
				logid[0] = (char) i + '0';                 
				filename = strcat(strcpy(header, LOGFILE_HEADER), logid);
				if ((fout = fopen(filename, "w")) < 0) {
					fprintf(stderr, "Cannot open file: %s\n", filename);
					return 0;   
				}
			}
			i++;

			while (de_queue(&cmd_queue, str_tmp) > 0) {
				fprintf(fout, "%s", str_tmp);
			}
			en_queue(&cmd_queue, str);
		}
	}

	/*Flush queue at the end of working session*/
	fclose(fout);
	logid[0] = (char) i + '0';                 
	filename = strcat(strcpy(header, LOGFILE_HEADER), logid);
	if ((fout = fopen(filename, "w")) < 0) {
	fprintf(stderr, "Cannot open file: %s\n", filename);   
	return 0;
	}
	while (de_queue(&cmd_queue, str_tmp) > 0) {
	fprintf(fout, "%s", str_tmp);
	}

	fclose(fin); fclose(fout);
	return 1;
}