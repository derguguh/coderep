#include "coderep/data_type/node.h"

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/string.h"
#include "coderep/macros.h"

struct node *node_alloc(struct allocator *allocator, struct string **value)
{
	struct node *node = allocator->allocate(allocator->state, sizeof(struct node));

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

bool node_has(struct node *self, const struct string *value)
{
	if (string_is_equal(self->value, value)) return true;
	else {
		if (self->_next == NULL) return false;
		else return node_has(self->_next, value);
	}
}

bool node_increment(struct node *self, const struct string *value)
{
	if (string_is_equal(self->value, value)) {
		self->count++;
		return true;
	} else {
		if (self->_next == NULL) return false;
		else return node_increment(self->_next, value);
	}
}

int node_push(struct node *self, struct allocator *allocator, struct string **value, int index)
{
	if (string_is_equal(self->value, *value)) {
		self->count++;
		string_drop(value);
	} else {
		if (self->_next == NULL) {
			self->_next = node_alloc(allocator, value);
			self->index = index;
		} else index = node_push(self->_next, allocator, value, index);
	}

	return index;
}

void node_drop(struct node **self, struct allocator *allocator)
{
	if(!self || !(*self)) return;

	node_drop(&(*self)->_next, allocator);
	string_drop(&(*self)->value);

	allocator->deallocate(allocator->state, *self);

	*self = NULL;
}
