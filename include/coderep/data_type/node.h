#ifndef CODEREP_DATA_TYPE_LIST_H
#define CODEREP_DATA_TYPE_LIST_H

#include "coderep/allocator/allocator.h"
#include <stdbool.h>
#include <stddef.h>

struct node {
	struct node *_next;
	struct string *value;
	ssize_t count;
};

struct node *node_alloc(struct allocator *allocator, struct string **value);

bool node_has(const struct node *self, const struct string *value);

bool node_increment(struct node *self, const struct string *value);

struct node *node_push(struct node *self, struct allocator *allocator, struct string **value);

struct node *node_find(struct node *_self, const struct string *value);

void node_print(const struct node *self, bool show_value, bool show_frequency, bool do_color);

void node_drop(struct node **self, struct allocator *allocator);

#endif
