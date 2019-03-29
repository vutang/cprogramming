#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list-kernel.h"

struct demo_node {
	struct list_head list;
	int id;
	char name[50];
};

int main() {
	struct demo_node head;
	struct demo_node *node;

	struct list_head *pos, *q;
	char name[50];

	printf("Init list\n");
	INIT_LIST_HEAD(&head.list);

	int i;
	printf("list_add_tail:\n");
	printf("Head: %d, %s\n", head.id, head.name);
	for (i = 0; i < 10; i++) {
		node = (struct demo_node *) malloc(sizeof(struct demo_node));
		node->id = i; 
		sprintf(name, "%d%d%d", i, i, i);
		strcpy(&node->name[0], &name[0]);
		list_add_tail(&node->list, &head.list);
		printf("\t%d, %s\n", i, node->name);
	}
	printf("Head: %d, %s\n", head.id, head.name);
	printf("list_add:\n");
	for (i = 10; i < 20; i++) {
		node = (struct demo_node *) malloc(sizeof(struct demo_node));
		node->id = i; 
		sprintf(name, "%d%d%d", i, i, i);
		strcpy(&node->name[0], &name[0]);
		list_add(&node->list, &head.list);
		printf("\t%d, %s\n", i, node->name);
	}

	printf("Head: %d, %s\n", head.id, head.name);
	printf("list_for_each:\n");
	list_for_each(pos, &head.list) {
		node = container_of(pos, struct demo_node, list);
		printf("\t%d, %s\n", node->id, node->name);	
	}

	node = list_first_entry(&head.list, struct demo_node, list);
	printf("list_first_entry: %d, %s\n", node->id, node->name);
	list_for_each_safe(pos, q, &head.list) {
		/*list_entry will call container_of*/
		node = list_entry(pos, struct demo_node, list);
		list_del(pos);
		free(node);
	}

	if(list_empty(&head.list)) {
		printf("Remove list successfully\n");
	} 
	return 0;
}