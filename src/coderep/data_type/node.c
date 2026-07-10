#include "coderep/data_type/node.h"

#include "coderep/allocator/allocator.h"
#include "coderep/consts.h"
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

bool node_has(const struct node *self, const struct string *value)
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

struct node *node_push(struct node *self, struct allocator *allocator, struct string **value)
{
	if (string_is_equal(self->value, *value)) {
		self->count++;
		string_drop(value);
	} else {
		if (self->_next == NULL) {
			self->_next = node_alloc(allocator, value);
			return self->_next;
		} else return node_push(self->_next, allocator, value);
	}

	return self;
}

struct node *node_find(struct node *self, const struct string *value)
{
	struct node *_node = self;
	while (_node != NULL) {
		if (string_is_equal(_node->value, value)) return _node;
		else _node = _node->_next;
	}

	return NULL;
}

void node_print(const struct node *self, bool show_value, bool show_frequency)
{
	if (show_value) {
		printf(ANSI_CYAN);
		string_print(self->value);
		printf(ANSI_RESET);
	}
	if (show_frequency) printf("(" ANSI_YELLOW "%zu" ANSI_RESET ")", self->count);

	if (self->_next != NULL) {
		printf(", ");
		node_print(self->_next, show_value, show_frequency);
	}
}

void node_drop(struct node **self, struct allocator *allocator)
{
	if(!self || !(*self)) return;

	node_drop(&(*self)->_next, allocator);
	string_drop(&(*self)->value);

	allocator->deallocate(allocator->state, *self);

	*self = NULL;
}
