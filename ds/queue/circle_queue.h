/*Circle queue using array*/

#include <stdio.h>

#define QUEUE_SIZE 5
#define STRING_LENGTH 100

struct queue {
    int front;
    int rear;
    char queue[QUEUE_SIZE][STRING_LENGTH];
};
typedef struct queue queue_t;

/*Initialize Queue*/
void ini_queue(queue_t *queue);
int en_queue(queue_t *queue, char* cmd);