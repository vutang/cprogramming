/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

#include "linked_list.h"

#define MAX_LINE        80 /* 80 chars per line, per command */

struct cmd_args {
    char *cmd;
    char *args[MAX_LINE/2 + 1];
};
typedef struct cmd_args cmd_args_t;

char print_history = 0;
char excute_command = 0;

int main(void)
{
    cmd_args_t *cmd_args_1, *cmd_args_2;
    char *args[MAX_LINE/2 + 1]; /* command line (of 80) has max of 40 arguments */
    int should_run = 1;
    
    int j = 0, i = 0;

    char *command;
    char *ptr, c;
    char *str1, *saveptr1, *token, *str2;
    char filename[] = "/bin/";
    // char *const parmList[] =  {"/bin/ls", "-l", "/home/", NULL};
    // char *const parmList[] =  {"/bin/cat", "/home/vutt6/ctc_rru", NULL};
    
    pid_t pid;    

    linked_list_t *command_link;
    ll_node_t *command_node = calloc(sizeof(ll_node_t), 1);

    command_link = ll_create();

    while (should_run){   
        printf("osh>");
        fflush(stdout);
        fflush(stdin);
        command = (char *) calloc(50, sizeof(char));
        ptr = command;

        /*Read command from prompt*/
        scanf("%c", &c);
        while(c != '\n') {
            *ptr = c;
            ptr++;   
            scanf("%c", &c);         
        }

        /*Parsing command and assign this command to args*/
        for (j = 0, str1 = command; ;j++, str1 = NULL) {
            token = strtok_r(str1, " ", &saveptr1);
            if (token == NULL)
                break;
            args[j] = token;
        }

        /*Initilize args*/
        str2 = calloc(50, sizeof(char));
        memcpy(str2, filename, 5);
        
        args[j] = NULL;
        cmd_args_1 = calloc(sizeof(cmd_args_t), 1);
        cmd_args_1->cmd = args[0];
        memcpy(cmd_args_1->args, args, MAX_LINE/2 + 1);

        if (!strcmp(args[0],"exit")) {
            should_run = 0;
        }
        else if (!strcmp(args[0],"!!")) {
            command_node = ll_back(command_link);
            command_node = ll_add(command_link, (void *) command_node->object);
            excute_command = 1;
        }
        else if (!strcmp(args[0],"history")) {
            print_history = 1;
        }
        else { 
            /*Add command to linked list*/
            command_node = ll_add(command_link, (void *) cmd_args_1);
            excute_command = 1;
        }

        if (excute_command == 1) {
            cmd_args_2 = calloc(sizeof(cmd_args_t), 1);
            memcpy(cmd_args_2, (cmd_args_t *) command_node->object, sizeof(cmd_args_t));
            fprintf(stderr, "Excuting cmd: %s\n", cmd_args_2->cmd);
            /*Fork a new process to excute new command*/
            pid = fork();

            if (pid == 0) {
                execvp(cmd_args_2->cmd, cmd_args_2->args);
            }
            else {
                wait(NULL);
            }
            excute_command = 0;
        }
        if (print_history == 1) {
            cmd_args_2 = calloc(sizeof(cmd_args_t), 1);

            command_node = command_link->head;
            while (command_node != NULL) {
                memcpy(cmd_args_2, (cmd_args_t *) command_node->object, sizeof(cmd_args_t));
                j = 0;
                while (cmd_args_2->args[j]) {
                    printf("%s ", cmd_args_2->args[j++]);
                }
                printf("\n");
                command_node = command_node->next;
            }
            free(cmd_args_2);
            print_history = 0;
        }
    }        
    free(command_node);
    free(cmd_args_1);
    free(cmd_args_2);
    ll_destroy(command_link);
    return 0;
}
