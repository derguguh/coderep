#include "coderep/data_types/distributive.h"
#include "coderep/consts.h"

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
	size_t index = string_hash(*value);
	struct node *node = self->buckets[index];

	while (node != NULL) {
		if (string_is_equals(node->value, *value)) {

			node->count++;
			break;
		} else node = node->next;
	}
	if (node == NULL) node = node_new(value);

	*value = NULL;
}

void distributive_drop(struct distributive **self)
{
	if (!self || !(*self)) return;

	free((*self));
	*self = NULL;
}

