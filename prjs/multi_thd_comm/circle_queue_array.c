/*
* @Author: vutang
* @Date:   2019-03-27 18:36:14
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-03-28 00:01:16
*/
#include "circle_queue_array.h"
#include "../logger/logger.h"
#include <string.h>

/*Initialize Queue*/
void ini_queue(queue_t *queue) {
    queue->front = -1;
    queue->rear = -1;
    return;
}

/*Add an element to Queue*/
int en_queue(queue_t *queue, struct queue_element *element) {
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

    // strcpy(queue->queue[queue->rear], cmd);
    queue->queue_core[queue->rear].size = element->size;
    memcpy(queue->queue_core[queue->rear].block, element->block, element->size);
    LOG_DEBUG("en_queue: %d, %s\n", queue->queue_core[queue->rear].size, 
        queue->queue_core[queue->rear].block);
    return 0;
}

/*Read an element from Queue*/
int de_queue(queue_t *queue, struct queue_element *element) {
    if (queue->front ==  -1) {
        LOG_WARN("[WARN] Queue is empty\n");
        return -1;
    } 
    // strcpy(str, queue->queue[queue->front]);
    element->size = queue->queue_core[queue->front].size;
    memcpy(element->block, queue->queue_core[queue->rear].block, 
        queue->queue_core[queue->front].size);

    LOG_DEBUG("de_queue: %d\n", queue->queue_core[queue->front].size);
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

int is_empty(queue_t *queue) {
    return (queue->front ==  -1);
}

/*Print all queue element to screen*/
void pri_queue(queue_t *queue) {
    int i;
    printf("\n");
    if (queue->front > queue->rear) {
        for (i = queue->front; i < QUEUE_SIZE; i++) {
            // printf("%s ", queue->queue[i]);
        }
        // for (i = 0; i <= queue->rear; i++)
            // printf("%s ", queue->queue[i]);
    }
    else {
        // for (i = queue->front; i <= queue->rear; i++)
            // printf("%s ", queue->queue[i]);
    }

    return;
}