#include "coderep/data_type/bimap.h"

#include "coderep/allocator/allocator.h"
#include "coderep/allocator/block.h"
#include "coderep/data_type/frequency.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/macros.h"

struct bimap *bimap_alloc(struct allocator *allocator, struct string **name, size_t buckets)
{
	if (buckets == 0) buckets = FREQUENCY_DEFAULT_SIZE;

	struct frequency *frequency = frequency_alloc(allocator, name, buckets);
	struct block *block = block_alloc(allocator, buckets * sizeof(struct node *));
	struct bimap *self = allocator->allocate(allocator->state, sizeof(struct bimap));
	if (!self) goto defer;

	*self = (struct bimap) {
		.frequency = frequency,
		.block_size = buckets * sizeof(size_t),
		.block = block,
		.current = block,
	};

	return self;
defer:
	frequency_drop(&frequency);
	MALLOC_ERR_EXIT("struct bimap");
}

void bimap_expand_if_need(struct bimap *self)
{
	if (self->frequency->element_count % self->frequency->buckets_count == 0) {
		if (self->current->_next == NULL) self->current->_next = block_alloc(self->frequency->allocator, self->block_size);

		self->current = self->current->_next;
		self->block++;
	}
}

size_t bimap_intern(struct bimap *self, struct string *value)
{

	if (node == NULL) node = node_alloc(self->frequency->allocator, value);
	else node = node_push(node, self->frequency->allocator, value);

	if (node->count == 1) {
		bimap_expand_if_need(self);

		struct node **loc = (struct node **)(self->current->mem + sizeof(struct node *) * self->frequency->element_count);
		*loc = node;
		*value = NULL;

		printf("%p\n", node);
		printf("bucket:%zu\n", bucket);
	} else printf("node already exists!");

	size_t index = string_hash(value) % self->frequency->buckets_count;
	struct node *bucket = self->frequency->buckets[index];

	self->frequency->count_total++;
	if (bucket == NULL) {
		struct string *cloned = string_clone(value, self->allocator, true);
		self->buckets[index] = node_alloc(self->allocator, &cloned);
		self->element_count++;
		bimap_expand_if_need(self);
		return true;
	} else {
		struct node *node = node_find(bucket, value);
		if (node == NULL) {
			struct string *cloned = string_clone(value, self->allocator, true);
			node_push(bucket, self->allocator, &cloned);
			self->element_count++;
			bimap_expand_if_need(self);
			return true;
		} else {
			node->count++;
			return false;
		}
	}
}

size_t bimap_push(struct bimap *self, struct string **value)
{
	size_t index = string_hash(*value) % self->frequency->buckets_count;
	struct node *bucket = self->frequency->buckets[index];

	if (bucket == NULL) bucket = node_alloc(self->frequency->allocator, value);
	else bucket = node_push(bucket, self->frequency->allocator, value);

	self->frequency->count_total++;
	if (bucket->count == 1) {
		self->frequency->element_count++;
		bimap_expand_if_need(self);

		struct node **loc = (struct node **)(self->current->mem + sizeof(struct node *) * self->frequency->element_count);
		*loc = bucket;
		*value = NULL;

		printf("%p\n", bucket);
		printf("index:%zu\n", index);
		return self->frequency->element_count - 1;
	} else printf("bucket already exists!");
	
	return id;
}

void bimap_print(const struct bimap *self)
{
	printf("(struct bimap) {");
	frequency_print(self->frequency, OUTPUT_MODE_RAW);
	printf("block: %p\n", self->block);
	printf("block_size: %zu\n", self->block_size);
	printf("blocks: %zu\n", self->blocks);
	puts("}");
	for (size_t i = 0; i < self->frequency->element_count; i++) {
		struct node *node = *(struct node **)(self->current->mem + sizeof(struct node *) * i);
		printf("node: (%p) ", node);
		node_print(node, true, true);
		printf(", ");
	}
}

void bimap_drop(struct bimap **self)
{
	if (!self || !(*self)) return;
	struct bimap *tmp = *self;
	struct allocator *allocator = tmp->frequency->allocator;

	frequency_drop(&tmp->frequency);
	block_drop(allocator, &tmp->block);
	allocator->deallocate(allocator->state, *self);

	*self = NULL;
}

