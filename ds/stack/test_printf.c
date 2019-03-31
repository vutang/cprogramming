/*
* @Author: Vu Tang
* @Date:   2019-03-31 23:51:52
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-01 00:26:46
*/

/*http://www.equestionanswers.com/c/c-printf-scanf-working-principle.php*/

/*https://stackoverflow.com/questions/22616986/order-of-evaluation-of-arguments-in-function-calling*/

#include <stdio.h>

int n = 0;

int foo() {
	return n++;
}

int main() 
{
	printf("%d %d %d\n", n++, n++, n++);
	return 0;
}