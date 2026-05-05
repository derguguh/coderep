#include "coderep/data_types/node.h"
#include "coderep/data_types/string.h"
#include "coderep/consts.h"

#include <stdlib.h>

struct node *node_alloc(struct string **value)
{
	struct node *node = malloc(sizeof(struct node));
	if (!node) {
		PRINT_ERR("Couldn't alloc memory for a struct node\n");
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
	if (string_is_equal(self->value, *value)) {
		self->count++;
		string_drop(value);
	} else {
		if (self->next == NULL) self->next = node_alloc(value);
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
