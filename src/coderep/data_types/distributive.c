#include "coderep/data_types/distributive.h"
#include "coderep/data_types/node.h"
#include "coderep/macros.h"

#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_SIZE 16

struct distributive *distributive_alloc(struct string **name, size_t initial_capacity)
{
	if (initial_capacity == 0) initial_capacity = DEFAULT_SIZE;

	struct distributive *distributive = malloc(sizeof(struct distributive));
	struct node **nodes = calloc(sizeof(struct node*), initial_capacity);

	if (!distributive || !nodes) goto defer;

	distributive->buckets_count = initial_capacity;
	distributive->buckets = nodes;
	distributive->name = *name;

	*name = NULL;
	
	return distributive;

defer:
	string_drop(name);
	free(distributive);
	free(nodes);

	MALLOC_ERR_EXIT("struct distributive");
}

void distributive_push(struct distributive *self, struct string **value)
{
	size_t index = string_hash(*value) % self->buckets_count;
	struct node *node = self->buckets[index];

	if (node == NULL) {
		node = node_alloc(value);
		self->buckets[index] = node;
	} else node_push(node, value);

	*value = NULL;
}

void distributive_print(struct distributive *self)
{
	string_print(self->name);
	printf(": [");

	bool first = true;
	for (int i = 0; i < self->buckets_count; i++) {
		struct node *node = self->buckets[i];

		while (node != NULL) {	
			if (!first) printf(", ");
			string_print(node->value);
			printf("(%d)", node->count);

			node = node->_next;
			first = false;
		}
	}
	
	puts("]");
}

void distributive_drop(struct distributive **self)
{
	if (!self || !(*self)) return;

	for (int i = 0; i < (*self)->buckets_count; i++) node_drop(&((*self)->buckets[i]));

	free((*self)->buckets);
	(*self)->buckets = NULL;

	string_drop(&(*self)->name);
	free(*self);
	*self = NULL;
}

