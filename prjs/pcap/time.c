#include <stdio.h>
#include <time.h>
#include <netinet/in.h>

int main() {
	time_t rawtime;
	time(&rawtime);
	u_char rawtime_a[4];
	printf("Timestamp: %s\n", ctime(&rawtime));
	rawtime = htonl(rawtime);
	printf("Timestamp: %x\n", rawtime);
	printf("sizeof(time_t): %d\n", (int)sizeof(time_t));
	printf("sizeof(time_t): %d\n", (int)sizeof(char));
	return 0;
}