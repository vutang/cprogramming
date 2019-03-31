/*
* @Author: Vu Tang
* @Date:   2019-03-31 23:51:52
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-01 00:08:11
*/

/*http://www.equestionanswers.com/c/c-printf-scanf-working-principle.php*/

#include <stdio.h>

int n = 0;

int foo() {
	return n++;
}

int main() 
{
	printf("%d %d\n", foo(), foo());
	return 0;
}