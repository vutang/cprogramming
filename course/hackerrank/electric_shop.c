/*
* @Author: Vu Tang
* @Date:   2019-05-05 13:30:22
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-05-05 13:30:51
*/
/*
 * Complete the getMoneySpent function below.
 */

/*
Monica wants to buy a keyboard and a USB drive from her favorite electronics store. The store has several models of each. Monica wants to spend as much as possible for the  items, given her budget.

Given the price lists for the store's keyboards and USB drives, and Monica's budget, find and print the amount of money Monica will spend. If she doesn't have enough money to both a keyboard and a USB drive, print -1 instead. She will buy only the two required items.

For example, suppose she has  to spend. Three types of keyboards cost . Two USB drives cost . She could purchase a , or a . She chooses the latter. She can't buy more than  items so she can't spend exactly .
*/
int getMoneySpent(int keyboards_count, int* keyboards, int drives_count, int* drives, int b) {
    /*
     * Write your code here.
     */
    int i, j;
    int *spend = malloc(keyboards_count * drives_count * sizeof(int));
    if (spend == NULL)
        return -2;
    for (i = 0; i < keyboards_count; i++) {
        for (j = 0; j < drives_count; j++) {
          *(spend + i * drives_count + j) = -1;
          if (*(keyboards + i) + *(drives + j) <= b)
            *(spend + i * drives_count + j) = *(keyboards + i) + *(drives + j);
        }
    }

    int max = *spend;
    for (i = 0; i < keyboards_count; i++) {
        for (j = 0; j < drives_count; j++) {
          if (*(spend + i * drives_count + j) > max)
            max = *(spend + i * drives_count + j);
        }
    }
    free(spend);
    return max;
}
