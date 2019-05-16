/*
* @Author: vutang
* @Date:   2019-05-15 15:24:20
* @Last Modified by:   vutang
* @Last Modified time: 2019-05-15 15:42:31
*/
#include <stdio.h>
#include <stdlib.h>

/*
 * This stores the total number of books in each shelf.
 */
int* total_number_of_books;

/*
 * This stores the total number of pages in each book of each shelf.
 * The rows represent the shelves and the columns represent the books.
 */
int** total_number_of_pages;

int main()
{
    int total_number_of_shelves;
    scanf("%d", &total_number_of_shelves);
    
    int total_number_of_queries;
    scanf("%d", &total_number_of_queries);

    total_number_of_books = (int *) malloc(total_number_of_shelves * sizeof(int));
    if (total_number_of_books == NULL)
        return 1;
    
    total_number_of_pages = (int **) malloc(total_number_of_shelves * sizeof(int *));
    if (total_number_of_pages == NULL)
    return 1;
	int j;
	for (j = 0; j < total_number_of_shelves; j++) {
		total_number_of_pages[j] = (int *) malloc(total_number_of_queries * sizeof(int));
	}
    
    while (total_number_of_queries--) {
        int type_of_query;
        scanf("%d", &type_of_query);
        // printf("type_of_query: %d\n", type_of_query);
        if (type_of_query == 1) {
            /*
             * Process the query of first type here.
             */
            int x, y;
            scanf("%d %d", &x, &y);
            // printf("%d %d\n", x, y);
            // total_number_of_pages[x][(int)(*(total_number_of_books + x))] = y;
            // printf("%d\n", total_number_of_pages[x][*(total_number_of_books + x)]);
            *(*(total_number_of_pages + x) + *(total_number_of_books + x)) = y;
            (*(total_number_of_books + x))++;

        } else if (type_of_query == 2) {
            int x, y;
            scanf("%d %d", &x, &y);
            printf("%d\n", *(*(total_number_of_pages + x) + y));
        } else {
            int x;
            scanf("%d", &x);
            printf("%d\n", *(total_number_of_books + x));
        }
    }

    if (total_number_of_books) {
        free(total_number_of_books);
    }
    
    int i;
    for (i = 0; i < total_number_of_shelves; i++) {
        if (*(total_number_of_pages + i)) {
            free(*(total_number_of_pages + i));
        }
    }
    
    if (total_number_of_pages) {
        free(total_number_of_pages);
    }
    
    return 0;
}