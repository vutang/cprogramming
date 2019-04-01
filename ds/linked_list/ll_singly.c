/*
* @Author: vutang
* @Date:   2019-04-01 17:25:00
* @Last Modified by:   Vu Tang
* @Last Modified time: 2019-04-01 23:04:41
*/

#include <stdlib.h>
#include <string.h>
#include "ll_singly.h"

linked_list_t *ll_create() 
{
	linked_list_t *list = calloc(sizeof(linked_list_t), 1);
	if (list == NULL)
		return NULL;
	list->head = NULL;
	list->length = 0;
	return list;
}

void ll_destroy(linked_list_t *list) 
{
	if (list == NULL)
		return;

	ll_node_t *curr = list->head;
	ll_node_t *next = NULL;
	while (curr != NULL) {
		next = curr->next;
		free(curr->object);
		free(curr);
		curr = next;
	}

	free(list);
}

ll_node_t *ll_create_node(void *object, int size) 
{
	ll_node_t *node = calloc(sizeof(ll_node_t), 1);
	if (node == NULL) 
		goto return_exit;

	node->object = malloc(size);
	if (node->object == NULL)
		goto free_node;

	memcpy(node->object, object, size);
	node->next = NULL;

	return node;
free_node:
	free(node);
return_exit:
	return NULL;
}

int ll_add_head(linked_list_t *list, void *object, int size)
{
	if (list == NULL || object == NULL)
		return -1;

	ll_node_t *node = ll_create_node(object, size);
	if (node == NULL)
		return -1;

	if (list->head == NULL) 
		list->head = node;
	else {
		node->next = list->head;
		list->head = node;
	}
	list->length++;
	return 0;
}

int ll_add_tail(linked_list_t *list, void *object, int size)
{
	if (list == NULL || object == NULL)
		return -1;

	ll_node_t *curr_node, *node = ll_create_node(object, size);
	if (node == NULL)
		return -1;

	if (list->head == NULL) 
		list->head = node;
	else {
		curr_node = list->head;
		while (curr_node->next != NULL) 
			curr_node = curr_node->next;
		curr_node->next = node;
	}
	return 0;
}

int ll_del_head(linked_list_t *list)
{
	if (list == NULL)
		return 0;
	ll_node_t *curr_node = list->head->next;
	free(list->head);
	list->head = curr_node;
	list->length--;
	return 0;
}