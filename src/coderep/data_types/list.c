#include "coderep/data_types/list.h"
#include "coderep/consts.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SIZE 20

int node_push_index(struct node *self, struct string **value, size_t element_index)
{
	int deep_level = 1;
	if (string_is_equal(self->value, *value)) string_drop(value);
	else {
		if (self->next == NULL) {
			self->next = node_new(value);
			self->next->count = element_index;
		} else deep_level = node_push(self->next, value) + 1;
	}

	return deep_level;
}

struct list *list_new(struct string **name, size_t initial_capacity)
{
	if (initial_capacity == 0) initial_capacity = DEFAULT_SIZE;
	struct list *list = malloc(sizeof(struct list));
	struct node **nodes = calloc(sizeof(struct node*), initial_capacity);

	if (list && nodes) {
		list->buckets_count = initial_capacity;
		list->element_count = 0;
		list->buckets = nodes;
		list->name = *name;

		*name = NULL;
		
		return list;
	} else {
		string_drop(name);
		free(list);
		free(nodes);

		fprintf(stderr, ANSI_ERR "Couldn't allocate memory for a struct list\n");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
		return NULL;
	}
}

int list_push(struct list *self, struct string **value)
{
	size_t index = string_hash(*value) % self->buckets_count;
	struct node *node = self->buckets[index];

	if (node == NULL) {
		node = node_new(value);
		self->buckets[index] = node;
	} else node_push_index(node, value, self->element_count);

	*value = NULL;

	self->element_count++;
	return self->element_count - 1;
}

const struct string *list_peek(struct list *self, size_t index)
{
	for (int i = 0; i < self->buckets_count; i++) {
		struct node *node = self->buckets[index];
		while (node != NULL) {
			if (node->count == index) return node->value;
			else node = node->next;
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

			node = node->next;
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
