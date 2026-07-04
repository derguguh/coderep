#include "coderep/data_type/bimap.h"

#include "coderep/allocator/allocator.h"
#include "coderep/allocator/block.h"
#include "coderep/data_type/frequency.h"
#include "coderep/data_type/node.h"
#include "coderep/macros.h"

struct bimap {
	struct allocator *allocator;
	struct node **buckets;
	size_t buckets_count;
	size_t element_count;
	struct block *block;
	struct block *current;
	size_t block_size;
	size_t block_count;
};

struct bimap *bimap_alloc(struct allocator *allocator, size_t initial_capacity)
{
	if (initial_capacity == 0) initial_capacity = FREQUENCY_DEFAULT_SIZE;

	struct node **buckets = allocator->allocate(allocator->state, sizeof(struct node *) * initial_capacity);
	struct block *block = block_alloc(allocator, sizeof(struct node *) * initial_capacity);
	struct bimap *self = allocator->allocate(allocator->state, sizeof(struct bimap));
	if (!buckets || !block || !self) goto defer;

	*self = (struct bimap) {
		.allocator = allocator,
		.block_size = initial_capacity * sizeof(size_t),
		.buckets_count = initial_capacity,
		.buckets = buckets,
		.block = block,
		.current = block,
	};

	return self;
defer:
	allocator->deallocate(allocator->state, buckets);
	allocator->deallocate(allocator->state, block);
	allocator->deallocate(allocator->state, self);
	MALLOC_ERR_EXIT("\tstruct bimap");
}

void bimap_expand_if_need(struct bimap *self)
{
	if (self->element_count % self->buckets_count == 0) {
		if (self->current->_next == NULL) self->current->_next = block_alloc(self->allocator, self->block_size);

		self->current = self->current->_next;
		self->block_count++;
	}
}

struct node *bimap_bucket_of(const struct bimap *self, const struct string *value)
{
	size_t index = string_hash(value) % self->buckets_count;
	return self->buckets[index];
}

bool bimap_has(const struct bimap *self, const struct string *value)
{
	struct node *bucket = bimap_bucket_of(self, value);

	if (bucket == NULL || node_find(bucket, value) == NULL) return false;
	else return true;
}

bool bimap_length(const struct bimap *self)
{
	return self->element_count;
}

size_t bimap_intern(struct bimap *self, const struct string *value)
{
	size_t index = string_hash(value) % self->buckets_count;
	struct node *bucket = self->buckets[index];
	struct node *last = bucket == NULL ? NULL : node_find(bucket, value);

	if (!(bucket && last)) {
		struct string *cloned = string_clone(value, self->allocator, true);

		if (bucket == NULL) {
			self->buckets[index] = node_alloc(self->allocator, &cloned);
			last = self->buckets[index];
		} else last = node_push(bucket, self->allocator, &cloned);

		last->count = self->element_count;
		self->element_count++;
		bimap_expand_if_need(self);

		struct node **loc = (struct node **)(self->current->mem + sizeof(struct node *) * last->count);
		*loc = last;
	}
	return last->count;
}

size_t bimap_push(struct bimap *self, struct string **value)
{
	struct node *node = bimap_bucket_of(self, *value);

	if (node == NULL) node = node_alloc(self->allocator, value);
	else node = node_push(node, self->allocator, value);

	if (node->count == 1) {
		node->count = self->element_count;
		self->element_count++;
		bimap_expand_if_need(self);

		struct node **loc = (struct node **)(self->current->mem + sizeof(struct node *) * node->count);
		*loc = node;
		*value = NULL;
	} else node->count--;

	return node->count;
}

size_t bimap_get_id_of(struct bimap *self, struct string *value)
{
	struct node *bucket = bimap_bucket_of(self, value);

	return node_find(bucket, value)->count;
}

struct node *bimap_get_node_of(struct bimap *self, size_t id)
{
	size_t what_block = id * sizeof(struct node *) / self->block_size;
	size_t real_id = id % self->buckets_count;
	struct block *block = self->block;
	for (size_t i = 0; i < what_block; i++) {
		if (block->_next == NULL) {
			PRINTF_ERR("Value '%zu' is out of boundary", id);
			if (real_id > self->element_count) real_id = self->element_count;
		} else block = block->_next;
	}
	
	return *(struct node **)(self->current->mem + sizeof(struct node *) * real_id);
}

struct string *bimap_get_value_of(struct bimap *self, size_t id)
{
	return bimap_get_node_of(self, id)->value;
}

void bimap_print(const struct bimap *self)
{
	printf("(struct bimap) {\n");
	printf("\t.buckets:\n");
	for (size_t i = 0; i < self->buckets_count; i++) {
		struct node *node = self->buckets[i];

		printf("\t\t[%zu] = [", i);

		if (node != NULL) node_print(node, true, true);
		else printf("(!)");

		puts("]");
	}
	puts("\t]");
	printf("\t.buckets_count: %zu\n", self->buckets_count);
	printf("\t.element_count: %zu\n", self->element_count);
	printf("\t.block:   %p\n", self->block);
	printf("\t.current: %p\n", self->current);
	printf("\t.block_size:  %zub (%zu elements)\n", self->block_size, self->block_size / sizeof(struct node *));
	printf("\t.block_count: %zu\n", self->block_count);
	puts("}");
}

void bimap_drop(struct bimap **self)
{
	if (!self || !(*self)) return;
	struct bimap *tmp = *self;
	struct allocator *allocator = tmp->allocator;

	for (size_t i = 0; i < tmp->buckets_count; i++) node_drop(&(tmp->buckets[i]), tmp->allocator);
	allocator->deallocate(allocator->state, tmp->buckets);

	block_drop(allocator, &tmp->block);

	allocator->deallocate(allocator->state, *self);

	*self = NULL;
}

