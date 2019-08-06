/*Circle queue using array*/

#include <stdio.h>

#define QUEUE_SIZE 5
#define STRING_LENGTH 61440 * 4

struct queue_element {
	char block[STRING_LENGTH];
	int size;
};

struct queue {
    int front;
    int rear;
    struct queue_element queue_core[QUEUE_SIZE];
};
typedef struct queue queue_t;

/*Initialize Queue*/
void ini_queue(queue_t *queue);
int en_queue(queue_t *queue, struct queue_element *element);
int de_queue(queue_t *queue, struct queue_element *element);
int is_empty(queue_t *queue);