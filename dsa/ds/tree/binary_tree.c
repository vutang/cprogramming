/*
* @Author: vutang
* @Date:   2019-04-03 11:29:55
* @Last Modified by:   vutang
* @Last Modified time: 2019-04-03 11:59:29
*/

#include <stdlib.h>
#include <string.h>

#include "binary_tree.h"

/* create_node() allocates a new node with the given data and NULL left and  
	right pointers. */
struct node* create_node(void *data, int size) 
{ 
	// Allocate memory for new node  
	struct node* node = (struct node*) malloc(sizeof(struct node)); 
	if (node == NULL) 
		return NULL;

	// Assign data to this node 
	node->data = (void *) malloc(size);
	if (node->data == NULL)
		goto free_node;
	memcpy(node->data, data, size); 

	// Initialize left and right children as NULL 
	node->left = NULL; 
	node->right = NULL; 
	return(node); 

free_node:
	free(node);
	return NULL;
}

void traverse_post_order(struct node *node, void (*f)(struct node *node))
{
	if (node == NULL) return;

	traverse_post_order(node->left, f);
	traverse_post_order(node->right, f);

	(*f)(node);
}

void traverse_in_order(struct node *node, void (*f)(struct node *node)) 
{
	if (node == NULL) return;

	traverse_in_order(node->left, f);

	(*f)(node);

	traverse_in_order(node->right, f);
}

void traverse_pre_order(struct node *node, void (*f)(struct node *node)) 
{
	if (node == NULL) return;

	(*f)(node);

	traverse_pre_order(node->left, f);
	traverse_pre_order(node->right, f);
}