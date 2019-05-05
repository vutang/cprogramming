/*
* @Author: Vu Tang
* @Date:   2019-05-05 15:06:32
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-05-05 15:48:21
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int num_of_digit(int n) 
{
	int digit = 0;
	if (n == 0)
		return 1;
    while (n) {
    	n /= 10;
    	digit++;
    }
    return digit;
}

/*n > 0*/
int reverse(int n) 
{
	int m = 0, ret = 0, i;
	
    // printf("m = %d\n", m);
    m = num_of_digit(n);
    int *arr = malloc(m * sizeof(int));

    for (i = 0; i < m; i++) {
    	if (n >= pow(10, i))
            arr[i] = (((int) (n / pow(10, i))) % 10);
        else 
            arr[i] = 0;
    }

    for (i = m - 1; i >= 0; i--)   
        ret += arr[i] * ((int) pow(10, m - i - 1));

    free(arr);
    return ret;
}

int beautifulDays(int i, int j, int k) {

    int count = 0, idx = 0;
    for (idx = i; idx <= j; idx++) {
        if (!(abs(idx - reverse(idx)) % k))
            count ++;
    }
    return count;
}


int main(int argc, char **argv) 
{
	// int n = 123;
	// if (argc != 1)
	// 	n = atoi(argv[1]);
	// printf("%d, %d\n", n, reverse(n));

	printf("beautifulDays: %d\n", beautifulDays(1, 2000000, 23047885));	
}