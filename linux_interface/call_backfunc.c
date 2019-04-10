#include <stdio.h>

/*Callback Function Definition
	Define a function pointer prototype
*/
typedef int (*print_func) (void *data);

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

/*Caller
	Use print_func as a prototype
*/
void logger(void *callback, print_func printfunc) {
	(*printfunc)(callback);
}

int main () {

	int i = 2;
	float f = 3.1415926535;

	logger(&i, &PrintInt);
	logger("It is a string", &PrintString);
	logger(&f, &PrintFloat);

	return 0;
}