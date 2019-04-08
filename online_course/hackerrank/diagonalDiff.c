#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();
char** split_string(char*);

/*
 * Complete the diagonalDifference function below.
 */
int diagonalDifference(int n, int a_columns, int** a) {
    /*
     * Write your code here.
     */
    int i, j, diag_1 = 0, diag_2 = 0;
    
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i == j) {
                diag_1 += a[i][j];
                diag_2 += a[i][n-j-1];
            }
        }
    }
    return (diag_1 > diag_2) ? (diag_1 - diag_2) : (diag_2 - diag_1);
}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    char* n_endptr;
    char* n_str = readline();
    int n = strtol(n_str, &n_endptr, 10);

    if (n_endptr == n_str || *n_endptr != '\0') { exit(EXIT_FAILURE); }

    int** a = malloc(n * sizeof(int*));

    for (int a_row_itr = 0; a_row_itr < n; a_row_itr++) {
        a[a_row_itr] = malloc(n * (sizeof(int)));

        char** a_item_temp = split_string(readline());

        for (int a_column_itr = 0; a_column_itr < n; a_column_itr++) {
            char* a_item_endptr;
            char* a_item_str = a_item_temp[a_column_itr];
            int a_item = strtol(a_item_str, &a_item_endptr, 10);

            if (a_item_endptr == a_item_str || *a_item_endptr != '\0') { exit(EXIT_FAILURE); }

            a[a_row_itr][a_column_itr] = a_item;
        }
    }

    int result = diagonalDifference(n, 0, a);

    fprintf(fptr, "%d\n", result);

    fclose(fptr);

    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;
    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) { break; }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') { break; }

        size_t new_length = alloc_length << 1;
        data = realloc(data, new_length);

        if (!data) { break; }

        alloc_length = new_length;
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';
    }

    data = realloc(data, data_length);

    return data;
}

char** split_string(char* str) {
    char** splits = NULL;
    char* token = strtok(str, " ");

    int spaces = 0;

    while (token) {
        splits = realloc(splits, sizeof(char*) * ++spaces);
        if (!splits) {
            return splits;
        }

        splits[spaces - 1] = token;

        token = strtok(NULL, " ");
    }

    return splits;
}
