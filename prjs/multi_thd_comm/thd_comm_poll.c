#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "circle_queue_array.h"
#include "../logger/logger.h"

#define TIMEOUT 5

/*Pipes for poll*/
static int pfd[2][2];

/*Data queue*/
queue_t dl_queue;

static void *siggen_thd_func() 
{
	while (1) {
		if (write(pfd[0][1], "a", 1) == -1) {
			perror("write");
			return NULL;
		}
		usleep(500000);

		if (write(pfd[1][1], "a", 1) == -1) {
			perror("write");
			return NULL;
		}
		usleep(500000);
	}
}

static void *pcie_thd_func() 
{
	struct pollfd plfd;
	int ret;
	char c;
	// char buf[STRING_LENGTH];
	struct queue_element q_elm;

	plfd.fd = pfd[1][0];
	plfd.events = POLLIN;	

	int fd = open("./tmp/output.jpg", O_CREAT | O_RDWR);
	if (fd < 0)
		perror("open(pcie_thd_id):");

	while (1) {
		ret = poll(&plfd, 1, TIMEOUT * 1000);
		if (ret == -1) {
			perror ("poll");
			break;
		}

		if (!ret) {
			printf ("%d seconds elapsed.\n", TIMEOUT);
			break;
		}

		if (plfd.revents & POLLIN) {
			LOG_DEBUG("PCIE got signal\n");
			ret = de_queue(&dl_queue, &q_elm);
			if (ret) {
				LOG_DEBUG("de_queue fail\n");
				break;
			}
			/*Write back to file*/
			write(fd, q_elm.block, q_elm.size);
			/*Clear Pipe*/
			read(pfd[1][0], &c, 1);
		}
	}
	close(fd);
	return NULL;
}

static void *l1_thd_func()
{
	struct pollfd plfd;
	int ret;
	// int cnt = 0;
	char c;
	// char buf[STRING_LENGTH];

	/*Init for polling*/
	plfd.fd = pfd[0][0];
	plfd.events = POLLIN;	

	struct queue_element q_elm;

	int fd = open("./tmp/input.jpg", O_RDONLY);
	if (fd == -1) {
		perror("open:");
		return NULL;
	}

	while (1) {
		ret = poll(&plfd, 1, TIMEOUT * 1000);
		if (ret == -1) {
			perror ("poll");
			break;
		}

		if (!ret) {
			printf ("%d seconds elapsed.\n", TIMEOUT);
			break;
		}

		if (plfd.revents & POLLIN) {
			LOG_DEBUG("L1 got signal\n");
			// sprintf(buf, "Test string %d", cnt++);
			ret = read(fd, q_elm.block, STRING_LENGTH);
			if (ret <= 0) {
				LOG_DEBUG("End of file");
				break;
			}
			q_elm.size = ret;

			ret = en_queue(&dl_queue, &q_elm);
			if (ret)
				LOG_WARN("en_queue fail\n");
			read(pfd[0][0], &c, 1);
		}
	}
	return NULL;
}

int main() 
{
	pthread_t siggen_thd_id, pcie_thd_id, l1_thd_id;
	int ret;

	ret = config_log("./thd_comm_poll.log", DEBUG, LOG_TO_SCREEN | LOG_TO_FILE);
	if (ret < 0)
		printf("Configure log fail\n");

	/*Create Pipe*/
	if (pipe(pfd[0]) == -1) {
		perror("pipe 0");
		exit(EXIT_FAILURE);
	}

	if (pipe(pfd[1]) == -1) {
		perror("pipe 1");
		exit(EXIT_FAILURE);
	}

	/*Initialize queue*/
	ini_queue(&dl_queue);

	ret = pthread_create(&siggen_thd_id, NULL, siggen_thd_func, NULL);
	if (ret != 0) {
		perror("pthread_create (siggen):");
		exit(EXIT_FAILURE);
	}

	ret = pthread_create(&pcie_thd_id, NULL, pcie_thd_func, NULL);
	if (ret != 0) {
		perror("pthread_create (pcie):");
	}

	ret = pthread_create(&l1_thd_id, NULL, l1_thd_func, NULL);
	if (ret != 0) {
		perror("pthread_create (l1):");
	}

	pthread_join(pcie_thd_id, NULL);
	pthread_join(siggen_thd_id, NULL);
	exit(EXIT_SUCCESS);
}