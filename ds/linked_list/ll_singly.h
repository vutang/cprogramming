#ifndef LL_DOUBLY_H
#define LL_DOUBLY_H

/** Node in the linked list. */
struct ll_node {
  struct ll_node *next;
  void *object;
};
typedef struct ll_node ll_node_t;

struct linked_list {
	struct ll_node *head;
	unsigned int length;	
};
typedef struct linked_list linked_list_t;

linked_list_t *ll_create(void);
void ll_destroy(linked_list_t *list);
int ll_add_head(linked_list_t *list, void *object, int size);

#endif /* LL_DOUBLY_H */