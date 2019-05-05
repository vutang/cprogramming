/*
* @Author: Vu Tang
* @Date:   2019-05-05 23:36:37
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-05-05 23:41:14
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int findDigits(int n) 
{
    int tmp = n, num_digit = 0, i, div_cnt = 0;
    
    if (n == 0)
        return 0;
    while (tmp) {
        tmp /= 10;
        num_digit++;
    }
    printf("num_digit: %d\n", num_digit);
    int *arr = malloc(num_digit * sizeof(int));
    for (i = 0; i < num_digit; i++) {
      if (n >= pow(10, i))
        arr[i] = (((int)(n / pow(10, i))) % 10);
      else
        arr[i] = 0;
    	printf("%d\n", arr[i]);
    }

    for (i = 0; i < num_digit; i++) {
        if ((arr[i] != 0) && !(n % arr[i]))
            div_cnt++;
    }
    return div_cnt;
}

int main(int argc, char **argv) 
{
	int n = 123;
	if (argc != 1)
		n = atoi(argv[1]);

	printf("%d %d\n", n, findDigits(n));
}