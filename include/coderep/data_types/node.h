#ifndef CODEREP_DATA_TYPES_NODE_H
#define CODEREP_DATA_TYPES_NODE_H

struct node {
	struct node *next;
	struct string *value;
	int count;
};

struct node *node_alloc(struct string **value);

int node_push(struct node *self, struct string **value);

void node_drop(struct node **self);

#endif
