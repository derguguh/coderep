#include "coderep/data_types/distributive.h"
#include "coderep/data_types/node.h"
#include "coderep/consts.h"

#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_SIZE 16

struct distributive *distributive_new(struct string **name, size_t initial_capacity)
{
	if (initial_capacity == 0) initial_capacity = DEFAULT_SIZE;
	struct distributive *distributive = malloc(sizeof(struct distributive));
	struct node **nodes = calloc(sizeof(struct node*), initial_capacity);

	if (distributive && nodes) {
		distributive->buckets_count = initial_capacity;
		distributive->buckets = nodes;
		distributive->name = *name;

		*name = NULL;
		
		return distributive;
	} else {
		string_drop(name);
		free(distributive);
		free(nodes);

		fprintf(stderr, ANSI_ERR "Couldn't allocate memory for a struct distributive");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
		return NULL;
	}
}

void distributive_push(struct distributive *self, struct string **value)
{
	size_t index = string_hash(*value) % self->buckets_count;
	printf("hash: %lu\n", string_hash(*value));
	printf("bucket index: %lu\n", index);
	struct node *node = self->buckets[index];

	if (node == NULL) {
		node = node_new(value);
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

			node = node->next;
			first = false;
		}
	}
	
	puts("]");
}

void distributive_drop(struct distributive **self)
{
	if (!self || !(*self)) return;

	for (int i = 0; i < (*self)->buckets_count; i++) node_drop(&(*self)->buckets[i]);
	free((*self)->buckets);
	(*self)->buckets = NULL;

	string_drop(&(*self)->name);
	free(*self);
	*self = NULL;
}

