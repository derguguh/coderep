#include "coderep/data_types/list.h"
#include "coderep/macros.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SIZE 20

size_t node_push_index(struct node *self, struct string **value, size_t element_count)
{
	size_t index = self->count;
	if (string_is_equal(self->value, *value)) string_drop(value);
	else {
		if (self->_next == NULL) {
			self->_next = node_alloc(value);
			self->_next->count = element_count;
			index = element_count;
		} else index = node_push_index(self->_next, value, element_count);
	}

	return index;
}

struct list *list_alloc(struct string **name, size_t initial_capacity)
{
	if (initial_capacity == 0) initial_capacity = DEFAULT_SIZE;
	struct list *list = malloc(sizeof(struct list));
	struct node **nodes = calloc(sizeof(struct node*), initial_capacity);

	if (!list || !nodes) goto defer;

	list->buckets_count = initial_capacity;
	list->element_count = 0;
	list->buckets = nodes;
	list->name = *name;

	*name = NULL;
	
	return list;

defer:
	string_drop(name);
	free(list);
	free(nodes);

	MALLOC_ERR_EXIT("struct list");
}

int list_push(struct list *self, struct string **value)
{
	size_t index = string_hash(*value) % self->buckets_count;
	struct node *node = self->buckets[index];

	int element_count = self->element_count;
	if (node == NULL) {
		node = node_alloc(value);
		node->count = element_count;
		self->buckets[index] = node;
	} else element_count = node_push_index(node, value, self->element_count);

	*value = NULL;

	if (element_count == self->element_count) self->element_count++;
	return self->element_count - 1;
}

const struct string *list_peek(struct list *self, size_t index)
{
	for (int i = 0; i < self->buckets_count; i++) {
		struct node *node = self->buckets[index];
		while (node != NULL) {
			if (node->count == index) return node->value;
			else node = node->_next;
		}
	}

	return self->name;
}

void list_print(struct list *self)
{
	string_print(self->name);
	printf(": {");

	bool first = true;
	for (int i = 0; i < self->buckets_count; i++) {
		struct node *node = self->buckets[i];

		while (node != NULL) {	
			if (!first) printf(", ");
			string_print(node->value);

			node = node->_next;
			first = false;
		}
	}
	
	puts("}");
}

void list_drop(struct list **self)
{
	if (!self || !(*self)) return;

	for (int i = 0; i < (*self)->buckets_count; i++) node_drop(&(*self)->buckets[i]);
	free((*self)->buckets);
	(*self)->buckets = NULL;

	string_drop(&(*self)->name);
	free(*self);
}
