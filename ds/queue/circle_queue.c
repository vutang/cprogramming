/*
* @Author: vutang
* @Date:   2019-03-27 18:36:14
* @Last Modified by:   vutang
* @Last Modified time: 2019-03-27 23:14:54
*/
#include "circle_queue.h"

/*Initialize Queue*/
void ini_queue(queue_t *queue) {
    queue->front = -1;
    queue->rear = -1;
    return;
}

/*Add an element to Queue*/
int en_queue(queue_t *queue, char* cmd) {
    if ((queue->front == 0 && queue->rear == QUEUE_SIZE - 1) || \
        (queue->front == queue->rear + 1)) {
        return -1; /*Queue is full*/
    }
    /*The new elelement is first one in queue*/
    else if (queue->rear == -1) {
        queue->rear++;
        queue->front++;
    }    
    /*Roltate rear when reaching MAX value*/
    else if (queue->rear == QUEUE_SIZE - 1 && queue->front > 0) {
        queue->rear = 0;
    }
    else {
        queue->rear++;
    }

    strcpy(queue->queue[queue->rear], cmd);
    fprintf(stderr,"[INFO] en_queue successfully\n");
    return 0;
}

/*Read an element from Queue*/
int de_queue(queue_t *queue, char *str) {
    if (queue->front ==  -1) {
        fprintf(stderr,"[WARN] Queue is empty\n");
        return -1;
    }
    
    strcpy(str, queue->queue[queue->front]);
    // printf("\n %s deleted", queue->queue[queue->front]);
    if (queue->front == queue->rear) {
        queue->front =  -1;
        queue->rear =  -1;

    }
    else if (queue->front == QUEUE_SIZE - 1)
        queue->front = 0;
    else 
        queue->front++;

    return 0;
}

/*Print all queue element to screen*/
void pri_queue(queue_t *queue) {
    int i;
    printf("\n");
    if (queue->front > queue->rear) {
        for (i = queue->front; i < QUEUE_SIZE; i++) {
            printf("%s ", queue->queue[i]);
        }
        for (i = 0; i <= queue->rear; i++)
            printf("%s ", queue->queue[i]);
    }
    else {
        for (i = queue->front; i <= queue->rear; i++)
            printf("%s ", queue->queue[i]);
    }

    return;
}