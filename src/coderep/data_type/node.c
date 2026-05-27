#include "coderep/data_type/node.h"

#include "coderep/data_type/string.h"
#include "coderep/macros.h"

#include <stdlib.h>

struct node *node_alloc(struct string **value)
{
	struct node *node = malloc(sizeof(struct node));

	if (!node) goto defer;

	*node = (struct node) {
		.value = *value,
		.count = 1,
		._next = NULL,
	};

	*value = NULL;

	return node;

defer:
	MALLOC_ERR_EXIT("struct node");
}

int node_push(struct node *self, struct string **value)
{
	int deep_level = 1;
	if (string_is_equal(self->value, *value)) {
		self->count++;
		string_drop(value);
	} else {
		if (self->_next == NULL) self->_next = node_alloc(value);
		else deep_level = node_push(self->_next, value) + 1;
	}

	return deep_level;
}

void node_drop(struct node **self)
{
	if(!self || !(*self)) return;

	node_drop(&(*self)->_next);
	string_drop(&(*self)->value);
	free(*self);

	*self = NULL;
}
