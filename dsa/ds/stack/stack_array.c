/*
* @Author: Vu Tang
* @Date:   2019-03-31 22:37:02
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-01 18:37:00
*/
#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "stack_array.h"

#ifdef DEBUG
#define pr_dbg(s, ...) 	printf("%s:#%d [DBG] ", __FILE__, __LINE__); \
						printf(s, __VA_ARGS__);
#else
#define pr_dbg(s, ...)
#endif

#define STACK_MAX_SIZE 200

static int stk_cur_pos = -1;
static int *stk_ptr = NULL;

/**
 * stack_init() - Initialize stack.
 * @size: size of prefer stack.
 *
 * Define a stack with @size
 *
 * Return: 
 * 0 - Success
 * < 0 - Fail
 */
int stack_init(int size) 
{
	stk_ptr = malloc(size * sizeof(int));
	if (stk_ptr == NULL) {
		pr_dbg("allocate memory fail: %d(%s)\n", errno, strerror(errno));
		return -1;
	}
	stk_cur_pos = 0;
	return 0;
}

void stack_del()
{
	free(stk_ptr);
}

/**
 * stack_is_empty() - Check if stack is empty or not.
 *
 * Return: 
 * 0 - Not empty
 * 1 - Empty
 * < 0 - Error
 */
int stack_is_empty(void)
{
	if (stk_cur_pos < 0) {
		fprintf(stderr, "Stack is NOT defined\n");
		return -1;
	}
	return (stk_cur_pos == 0);
}

int stack_push(int d_in)
{
	if (stk_ptr == NULL)
		return -1;
	stk_ptr[stk_cur_pos++] = d_in;
	pr_dbg("push d_in %d to %d\n", d_in, stk_cur_pos - 1);
	return 0;
}

int stack_pop(void) {
	pr_dbg("pop %d\n", stk_cur_pos - 1);
	return stk_ptr[--stk_cur_pos];
}