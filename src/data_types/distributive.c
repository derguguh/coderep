#include "coderep/data_types/distributive.h"
#include "coderep/consts.h"

#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_SIZE 16

struct node *node_new(struct string **value)
{
	struct node *node = malloc(sizeof(struct node));
	if (!node) {
		fprintf(stderr, ANSI_ERR "Couldn't alloc memory for a struct node");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
		return NULL;
	} else {
		node->value = *value;
		node->count = 1;

		*value = NULL;

		return node;
	}
}

int node_push(struct node *self, struct string **value)
{
	int deep_level = 1;
	if (string_is_equals(self->value, *value)) {
		self->count++;
		string_drop(value);
	} else {
		if (self->next == NULL) self->next = node_new(value);
		else deep_level = node_push(self->next, value) + 1;
	}

	return deep_level;
}

void node_drop(struct node **self)
{
	if(!self || !(*self)) return;

	node_drop(&(*self)->next);
	string_drop(&(*self)->value);
	free(*self);

	*self = NULL;
}

struct distributive *distributive_new(struct string **name, size_t bucket_size)
{
	if (bucket_size == 0) bucket_size = DEFAULT_SIZE;
	struct distributive *distributive = malloc(sizeof(struct distributive));
	struct node **nodes = calloc(sizeof(struct node*), bucket_size);

	if (distributive && nodes) {
		distributive->buckets_count = bucket_size;
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
	free((*self));
	*self = NULL;
}

