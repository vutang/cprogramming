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

/**
 * ll_create - create linked list
 *
 * return:
 * - NULL if fail
 * - linked_list_t pointer
 */
linked_list_t *ll_create(void);

/**
 * Destroys a linked list. This will free up its memory and the memory taken
 * by its nodes. This also DOES free up the memory taken up by the objects
 * contained within the nodes.
 *
 * list: The list to destroy.
 */
void ll_destroy(linked_list_t *list);
int ll_add_head(linked_list_t *list, void *object, int size);
int ll_add_tail(linked_list_t *list, void *object, int size);
int ll_del_head(linked_list_t *list);

#endif /* LL_DOUBLY_H */