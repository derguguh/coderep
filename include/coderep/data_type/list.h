#ifndef CODEREP_DATA_TYPES_LIST_H
#define CODEREP_DATA_TYPES_LIST_H

#include "coderep/data_type/string.h"
#include "coderep/data_type/node.h"

#include <stddef.h>

struct list {
	struct string *name;
	struct node **buckets;
	size_t buckets_count;
	size_t element_count;
};

struct list *list_alloc(struct string **name, size_t initial_capacity);

int list_push(struct list *self, struct string **value);

const struct string *list_peek(struct list *self, size_t index);

void list_print(struct list *self);

void list_drop(struct list **self);

#endif
