#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

void signal_handler() 
{
	printf("Got a signal\n");
	return;
}

static void *l1_thd_func() 
{
	while (1) {
		sleep(1);
	}
	return NULL;
}

static void *pcie_thd_func(void *arg)
{
	sigset_t *set = arg;
	int ret, sig;

	while (1) {
		ret = sigwait(set, &sig);
		if (ret != 0)
			perror("sigwait");
		printf("Thread got signal\n");
	}
	return NULL;
}

int main() 
{
	pthread_t l1_thd_id, pcie_thd_id;
	int ret;
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	ret = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (ret != 0) {
		perror("sigmask");
		exit(EXIT_FAILURE);
	}

	/*init queue*/

	/*create thread 1*/
	ret = pthread_create(&l1_thd_id, NULL, l1_thd_func, NULL);
	if (ret != 0) {
		perror("Fail to create L1 thread");
		exit(EXIT_FAILURE);
	}

	ret = pthread_create(&pcie_thd_id, NULL, pcie_thd_func, (void *) &set);
	if (ret != 0) {
		perror("Fail to create L1 thread");
		exit(EXIT_FAILURE);
	}

	ret = pthread_join(l1_thd_id, NULL);
	exit(EXIT_SUCCESS);
}