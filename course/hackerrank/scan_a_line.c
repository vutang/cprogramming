/*
* @Author: vutang
* @Date:   2019-05-15 08:24:36
* @Last Modified by:   vutang
* @Last Modified time: 2019-05-15 08:24:37
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MAX_LEN 100

int main() 
{
  char ch;
  char str[MAX_LEN];
  scanf("%c", &ch);
  printf("%c\n", ch);

  scanf("%s", str);
  printf("%s\n", str);

  scanf("\n");

  scanf("%[^\n]%*c", str);
  printf("%s\n", str);
  /* Enter your code here. Read input from STDIN. Print output to STDOUT */
  return 0;
}