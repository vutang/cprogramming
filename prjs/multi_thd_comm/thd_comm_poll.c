#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <poll.h>

#include "circle_queue_array.h"

#define TIMEOUT 5

#ifdef DEBUG
#define pr_debug(...) printf("%s", __VA_ARGS__)
#else
#define pr_debug(...)
#endif

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
	int ret, cnt = 0;
	char c;
	char str[STRING_LENGTH];

	plfd.fd = pfd[1][0];
	plfd.events = POLLIN;	


	while (1) {
		ret = poll(&plfd, 1, TIMEOUT * 1000);
		if (ret == -1) {
			perror ("poll");
			return NULL;
		}

		if (!ret) {
			printf ("%d seconds elapsed.\n", TIMEOUT);
			return NULL;
		}

		if (plfd.revents & POLLIN) {
			pr_debug("PCIE got signal\n");
			sprintf(str, "Test string %d", cnt++);
			ret = en_queue(&dl_queue, str);
			if (ret)
				pr_debug("Queue full\n");

			/*Clear Pipe*/
			read(pfd[1][0], &c, 1);
		}
	}
}

static void *l1_thd_func()
{
	struct pollfd plfd;
	int ret;
	char c;
	char str[STRING_LENGTH];

	plfd.fd = pfd[0][0];
	plfd.events = POLLIN;	


	while (1) {
		ret = poll(&plfd, 1, TIMEOUT * 1000);
		if (ret == -1) {
			perror ("poll");
			return NULL;
		}

		if (!ret) {
			printf ("%d seconds elapsed.\n", TIMEOUT);
			return NULL;
		}

		if (plfd.revents & POLLIN) {
			pr_debug("L1 got signal\n");
			ret = de_queue(&dl_queue, str);
			if (ret)
				printf("de_queue fail\n");
			printf("PCIe Got: %s\n", str);
			read(pfd[0][0], &c, 1);
		}
	}
	return NULL;
}

int main() 
{
	pthread_t siggen_thd_id, pcie_thd_id, l1_thd_id;
	int ret;

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