struct node { 
	struct node *left; 
	struct node *right; 

    void *data; 
}; 

struct node* create_node(void *data, int size);
void traverse_post_order(struct node *node, void (*f)(struct node *node));
void traverse_in_order(struct node *node, void (*f)(struct node *node));
void traverse_pre_order(struct node *node, void (*f)(struct node *node));