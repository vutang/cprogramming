#include	"../lib/unpipc.h"

#define	SEM_NAME	"mysem"
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
// #define	FILE_MODE	(S_IRUSR | S_IWUSR)

int		count = 0;

int
main(int argc, char **argv)
{
	int		i, nloop;
	sem_t	*mutex;

	if (argc != 2)
		err_quit("usage: incr1 <#loops>");
	nloop = atoi(argv[1]);

	// Sem_unlink("mytest");
	/* 4create, initialize, and unlink semaphore */
	// mutex = Sem_open(Px_ipc_name(SEM_NAME), O_CREAT | O_EXCL | O_RDWR, FILE_MODE, 1);
	mutex = Sem_open("mytest", O_CREAT | O_EXCL | O_RDWR, FILE_MODE, 1);
	Sem_unlink("mytest");

	setbuf(stdout, NULL);	/* stdout is unbuffered */
	if (Fork() == 0) {		/* child */
		for (i = 0; i < nloop; i++) {
			Sem_wait(mutex);
			printf("child: %d\n", count++);
			Sem_post(mutex);
		}
		exit(0);
	}

		/* 4parent */
	for (i = 0; i < nloop; i++) {
		Sem_wait(mutex);
		printf("parent: %d\n", count++);
		Sem_post(mutex);
	}
	exit(0);
}
