/**
 * VuTang's Simple Shell
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

#define MAX_LINE        100 /* 80 chars per line, per command */
#define QUEUE_SIZE      8   
#define LOGFILE_HEADER "./logcmd/log"

struct cmd_args {
    char *cmd;
    char *args[MAX_LINE/2 + 1];
};
typedef struct cmd_args cmd_args_t;

struct queue {
    int front;
    int rear;
    cmd_args_t queue[QUEUE_SIZE];
};
typedef struct queue queue_t;

/*Global Variables*/
char *currentDirectory;
int log_cnt = 0;

void shellPrompt() { 
    // We print the prompt in the form "<user>@<host> <cwd> >"
    char hostn[1204] = "";
    gethostname(hostn, sizeof(hostn));
    printf("%s@%s:%s> ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}

/*Add an element to Queue*/
int en_queue(queue_t *queue, cmd_args_t* cmd) {
    if ((queue->front == 0 && queue->rear == QUEUE_SIZE - 1) || (queue->front == queue->rear + 1)) {
        fprintf(stderr,"[WARN] Queue is full\n");
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

    queue->queue[queue->rear] = *cmd;
    fprintf(stderr,"[INFO] en_queue successfully\n");
    return 1;
}

/*Read an element from Queue*/
int de_queue(queue_t *queue, cmd_args_t *str) {
    if (queue->front ==  -1) {
        fprintf(stderr,"[INFO] Queue is empty\n");
        return -1;
    }
    else if (queue->front == queue->rear) {
        *str = queue->queue[queue->front];
        queue->front =  -1;
        queue->rear =  -1;

    }
    else {
        *str = queue->queue[queue->front];
        queue->front++;
    }
    return 1;
}

/*Initialize Queue*/
void ini_queue(queue_t *queue) {
    queue->front = -1;
    queue->rear = -1;
    return;
}

/*Store cmd in Queue and write to file when queue is full*/
void cmd_logger(queue_t *cmdQueue, cmd_args_t *cmd_args) {
    FILE *fout;
    char logid[2] = "0", *filename;
    char header[20] = LOGFILE_HEADER;
    cmd_args_t cmd_args_tmp; 
    int i;

    if (en_queue(cmdQueue, cmd_args) < 0) {
        logid[0] = (char) log_cnt + '0';                 
        filename = strcat(strcpy(header, LOGFILE_HEADER), logid);
        if ((fout = fopen(filename, "w")) < 0) {
            fprintf(stderr, "Cannot open file: %s\n", filename);
            return;   
        }
        log_cnt++;
        while (de_queue(cmdQueue, &cmd_args_tmp) > 0) {
            i = 0;
            while (cmd_args_tmp.args[i] != NULL) {
                fprintf(fout, "%s ", cmd_args_tmp.args[i]);
                i++;
            }
            fprintf(fout, "\n");
            fflush(fout);
        }
        en_queue(cmdQueue, cmd_args);
        fclose(fout);
    }
    return;
}

void vsh_handler(cmd_args_t *cmd_args, queue_t *cmd_queue) {
    FILE *vsh_script_file;
    char cur_cmd[100], *token;
    char *args[MAX_LINE/2 + 1], *saveptr1, *str1;
    int i;
    cmd_args_t cmd_args_q, cmd_args_tmp;  

    printf("Handle vsh: %s\n", cmd_args->args[1]);

    if ((vsh_script_file = fopen(cmd_args->args[1], "r")) == NULL) {
        printf("[INFO] File not found\n");
        return;
    }
    else {
        printf("[INFO] vSHELL executes vSH file\n");
        while (fgets(cur_cmd, 100, vsh_script_file) > 0) {
            for (i = 0, str1 = cur_cmd; ;i++, str1 = NULL) {
                // token = strtok_r(str1, " \n\t", &saveptr1);
                token = strtok(str1, " \n\r\t");
                if ((token == NULL) || (strcmp(token, "\n") == 0)) {
                    args[i] = NULL;
                    break;
                }
                args[i] = malloc(strlen(token) * sizeof(char));
                strcpy(args[i], token);
            }

            i = 0;
            while(args[i] != NULL) {
                cmd_args_q.args[i] = args[i];
                i++;
            }
            cmd_args_q.args[i] = args[i];
            cmd_args_q.cmd = args[0];
            cmd_logger(cmd_queue, &cmd_args_q);
        }        
    }

    fclose(vsh_script_file);
    return;
}


int main(void) {
    queue_t cmd_queue;
    char cur_cmd[50], *token;
    char *args[MAX_LINE/2 + 1], *saveptr1, *str1; /* command line (of 80) has max of 40 arguments */
    int i;
    cmd_args_t cmd_args_q, cmd_args_tmp;    

    FILE *fout;
    char logid[2] = "0", *filename;
    char header[20] = LOGFILE_HEADER;

    ini_queue(&cmd_queue); 
    log_cnt = 0;

    while (1) {
        shellPrompt();
        fgets(cur_cmd, 50, stdin);

        /*Parsing command and assign this command to args*/
        for (i = 0, str1 = cur_cmd; ;i++, str1 = NULL) {
            token = strtok_r(str1, " \n\t", &saveptr1);
            if (token == NULL) {
                args[i] = NULL;
                break;
            }
            args[i] = malloc(strlen(token) * sizeof(char));
            strcpy(args[i], token);
        }

        i = 0;
        while(args[i] != NULL) {
            cmd_args_q.args[i] = args[i];
            i++;
        }

        cmd_args_q.args[i] = args[i];
        cmd_args_q.cmd = args[0];

        if (args[0] == NULL) {
            printf("\n");
        }
        else if (strcmp(cmd_args_q.cmd, "vsh") == 0) {
            vsh_handler(&cmd_args_q, &cmd_queue);
        }
        else if (strcmp(cmd_args_q.cmd, "exit") == 0) {
            /*flush out all remaining elements in Queue before exitting shell*/
            logid[0] = (char) log_cnt + '0';                 
            filename = strcat(strcpy(header, LOGFILE_HEADER), logid);
            if ((fout = fopen(filename, "w")) < 0) {
                fprintf(stderr, "Cannot open file: %s\n", filename);
                return 0;   
            }
            while (de_queue(&cmd_queue, &cmd_args_tmp) > 0) {
                fprintf(fout, "%s\n", cmd_args_tmp.cmd);
                fflush(fout);
            }
            fclose(fout);

            fprintf(stderr, "Exit VSH\n");
            exit(0);
        }
        else 
            /*Log cmd to QUEUE*/
            cmd_logger(&cmd_queue, &cmd_args_q);
    }
    return 0;
}
