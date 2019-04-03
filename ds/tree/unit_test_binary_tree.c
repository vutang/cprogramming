/*
* @Author: vutang
* @Date:   2019-04-03 11:47:12
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-03 12:00:50
*/

#include <stdio.h>
#include "binary_tree.h"

void print_node(struct node *node)
{
	printf("%d\n", *(int *) node->data);
}

int main()
{
	int i; 
	i = 1;
	struct node *root = create_node((void *) &i, sizeof(int)); 
	i = 2;
	root->left = create_node((void *) &i, sizeof(int));
	i = 3;
	root->right = create_node((void *) &i, sizeof(int));
	i = 4;
	root->left->left = create_node((void *) &i, sizeof(int));
	i = 5;
	root->left->right = create_node((void *) &i, sizeof(int));

	printf("post:\n");
	traverse_post_order(root, &print_node);
	printf("in:\n");
	traverse_in_order(root, &print_node);
	printf("pre:\n");
	traverse_pre_order(root, &print_node);
	
	return 0;
}