#include <stdio.h>
#include <string.h>

#define QUEUE_SIZE 5
#define STRING_LENGTH 100
#define LOGFILE_HEADER "./logcmd/log"

struct queue {
    int front;
    int rear;
    char queue[QUEUE_SIZE][STRING_LENGTH];
};
typedef struct queue queue_t;

/*Initialize Queue*/
void ini_queue(queue_t *queue) {
    queue->front = -1;
    queue->rear = -1;
    return;
}

/*Add an element to Queue*/
int en_queue(queue_t *queue, char* cmd) {
    if ((queue->front == 0 && queue->rear == QUEUE_SIZE - 1) || (queue->front == queue->rear + 1)) {
        return -1; /*Queue is full*/
    }
    else if (queue->rear == -1) {
        queue->rear++;
        queue->front++;
    }    
    else if (queue->rear == QUEUE_SIZE - 1 && queue->front > 0) {
        queue->rear = 0;
    }
    else {
        queue->rear++;
    }

    strcpy(queue->queue[queue->rear], cmd);
    fprintf(stderr,"[INFO] en_queue successfully\n");
    return 1;
}

/*Read an element from Queue*/
int de_queue(queue_t *queue, char *str) {
    if (queue->front ==  -1) {
        fprintf(stderr,"[WARN] Queue is empty\n");
        return -1;
    }
    else if (queue->front == queue->rear) {
        // printf("\n %s deleted", queue->queue[queue->front]);
        strcpy(str, queue->queue[queue->front]);
        queue->front =  -1;
        queue->rear =  -1;

    }
    else {
        // printf("\n %s deleted", queue->queue[queue->front]);
        strcpy(str, queue->queue[queue->front]);
        queue->front++;
    }
    return 1;
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