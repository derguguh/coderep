#ifndef CODEREP_DATA_TYPES_NODE_H
#define CODEREP_DATA_TYPES_NODE_H

#include "coderep/allocator/allocator.h"
#include <stdbool.h>
struct node {
	struct node *_next;
	struct string *value;
	int index;
	int count;
};

struct node *node_alloc(struct allocator *allocator, struct string **value);

bool node_has(struct node *self, const struct string *value);

bool node_increment(struct node *self, const struct string *value);

int node_push(struct node *self, struct allocator *allocator, struct string **value, int index);

void node_drop(struct node **self, struct allocator *allocator);

#endif
