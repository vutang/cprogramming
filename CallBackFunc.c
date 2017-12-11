#include <stdio.h>

/*Callback Function Definition*/
typedef int (*PrintFunc) (void *data);

/*Callee 0*/
int PrintInt(void *data) {
	printf("PrintInt: %d\n", *((int *)data));
	return 1;
}

/*Callee 1*/
int PrintFloat(void *data) {
	printf("PrintFloat: %f\n", *(float *)data);
	return 1;
}

/*Callee 2*/
int PrintString(void *data) {
	printf("PrintString: %s\n", (char *)data);
	return 1;
}

/*Caller*/
void Logger(void *CallBack, PrintFunc PriFunc) {
	(*PriFunc)(CallBack);
}

int main () {

	int i = 2;
	float f = 3.1415926535;

	Logger(&i, &PrintInt);
	Logger("It is a string", &PrintString);
	Logger(&f, &PrintFloat);

	return 0;
}