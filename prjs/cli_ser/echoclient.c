/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"
#include "unp.h"
#include "cliSer.h"

int main(int argc, char **argv) 
{
    int clientfd, i;
    char *host, *port, line[MAXLINE], buf[MAXLINE];

    /* cmd line (of 80) has max of 40 arguments */
    char *cmd[MAXLINE/2 + 1], cmdCnt;
    char *str1, *saveptr1, *token;
    rio_t rio;

    msg_t *cliMsg = malloc(sizeof(msg_t));
    char *priv_msg;

    if (argc != 3) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
    }
    host = argv[1];
    port = argv[2];

    printf("cli> ");
    fflush(stdout);
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (1) {
    	memset(line, '\0', MAXLINE );
    	fgets(line, MAXLINE, stdin);
    	memcpy(buf, line, strlen(line) + 1);

    	printf("cli> ");
    	fflush(stdout);

    	if((cmd[0] = strtok(buf," \n\t")) == NULL) continue;
    	cmdCnt = 1;
		while((cmd[cmdCnt] = strtok(NULL, " \n\t")) != NULL) cmdCnt++;

        if (!strcmp(cmd[0],"cmd")) {
            if (!strcmp(cmd[1],"cap")) {
            	cliMsg->type = REQUEST;
            	cliMsg->length = strlen(line) + 1;
            	memcpy(cliMsg->buffer, line, strlen(line) + 1);
            	printf("Send cliMsg to Server: %s", cliMsg->buffer);
            	Rio_writen(clientfd, cliMsg, sizeof(msg_t));
            }
        }
    }

    free(cliMsg);
	Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
/* $end echoclientmain */
