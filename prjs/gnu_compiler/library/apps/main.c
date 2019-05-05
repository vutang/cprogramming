/*
* @Author: vutang
* @Date:   2018-10-25 11:18:27
* @Last Modified by:   vutang
* @Last Modified time: 2018-10-25 11:31:43
*/

#include <stdio.h>

/*Use as system header file, can specify by use -I options*/
#include <libvux.h>

/*Use user header file*/
// #include "../lib/libvux.h"

int main() {
	int a = 1, b = 2;

	printf("%d + %d = %d\n", a, b, add(a, b));
	return 0;
}