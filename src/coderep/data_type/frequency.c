#include "coderep/data_type/frequency.h"

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/node.h"
#include "coderep/macros.h"

#include <stdbool.h>
#include <stdlib.h>

const size_t FREQUENCY_DEFAULT_SIZE = 20;

struct frequency *frequency_alloc(struct allocator *allocator, struct string **name, size_t initial_capacity)
{
	if (initial_capacity == 0) initial_capacity = FREQUENCY_DEFAULT_SIZE;

	struct frequency *frequency = allocator->allocate(allocator->state, sizeof(struct frequency));
	struct node **nodes = allocator->allocate(allocator->state, sizeof(struct node*) * initial_capacity);

	if (!frequency || !nodes) goto defer;

	*frequency = (struct frequency) {
		.allocator = allocator,
		.buckets_count = initial_capacity,
		.buckets = nodes,
		.name = *name,
	};

	*name = NULL;
	
	return frequency;

defer:
	string_drop(name);
	allocator->deallocate(allocator->state, frequency);
	allocator->deallocate(allocator->state, nodes);

	MALLOC_ERR_EXIT("struct frequency");
}

void frequency_push(struct frequency *self, struct string **value)
{
	size_t bucket = string_hash(*value) % self->buckets_count;
	struct node *node = self->buckets[bucket];

	size_t count = self->element_count;

	if (node == NULL) {
		node = node_alloc(self->allocator, value);
		self->buckets[bucket] = node;
	} else count = node_push(node, self->allocator, value, count);

	self->element_count = count;

	*value = NULL;
}

void frequency_print(struct frequency *self, enum output_mode output_mode)
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

void frequency_drop(struct frequency **self)
{
	if (!self || !(*self)) return;
	struct frequency *tmp = *self;

	for (int i = 0; i < tmp->buckets_count; i++) node_drop(&(tmp->buckets[i]), tmp->allocator);

	tmp->allocator->deallocate(tmp->allocator->state, tmp->buckets);
	tmp->buckets = NULL;

	string_drop(&tmp->name);
	tmp->allocator->deallocate(tmp->allocator->state, *self);
	*self = NULL;
}

