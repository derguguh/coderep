#include "coderep/tool/word_tree.h"

#include "coderep/data_type/node.h"
#include "coderep/macros.h"

#include <string.h>

struct word_tree {
	struct allocator *allocator;
	struct node **buckets;
	size_t buckets_count;
	size_t non_unique_hash;
};

struct word_tree *word_tree_from_table(struct allocator *allocator, const struct word_table *table_array, size_t table_array_l)
{
	struct node **buckets = allocator->allocate(allocator->state, sizeof(struct node *) * table_array_l);
	struct word_tree *self = allocator->allocate(allocator->state, sizeof(struct word_tree));
	if (!buckets || !self) goto defer;

	memset(buckets, 0, sizeof(struct node *) * table_array_l);

	*self = (struct word_tree) {
		.allocator = allocator,
		.buckets = buckets,
		.buckets_count = table_array_l,
	};

	for (size_t i = 0; i < table_array_l; i++) {
		struct string *value = string_from_cstr(allocator, table_array[i].word);
		size_t index = string_hash(value) % table_array_l;
		struct node *bucket = self->buckets[index];

		if (bucket == NULL) {
			bucket = node_alloc(self->allocator, &value);
			self->buckets[index] = bucket;
		} else {
			bucket = node_push(bucket, self->allocator, &value);
			self->non_unique_hash++;
		}
		bucket->count = table_array[i].id;
	}

	return self;
defer:
	allocator->deallocate(allocator->state, buckets);
	allocator->deallocate(allocator->state, self);
	ALLOC_ERR_EXIT("struct bimap");
}

const struct string *word_tree_get_by_id(const struct word_tree *self, ssize_t id)
{
	for (size_t i = 0; i < self->buckets_count; i++) {
		struct node *bucket = self->buckets[i];
		while (bucket != NULL) {
			if (bucket->count == id) return bucket->value;
			else bucket = bucket->_next;
		}
	}

	return NULL;
}

ssize_t word_tree_categorize(const struct word_tree *self, const struct string *word)
{
	size_t index = string_hash(word) % self->buckets_count;
	struct node *bucket = node_find(self->buckets[index], word);

	if (bucket == NULL) return -1;
	else return bucket->count;
}

void word_tree_drop(struct word_tree **self)
{
	if (!self || !(*self)) return;
	struct allocator *allocator = (*self)->allocator;

	for (size_t i = 0; i < (*self)->buckets_count; i++) node_drop(&(*self)->buckets[i], allocator);

	allocator->deallocate(allocator->state, (*self)->buckets);
	(*self)->buckets = NULL;

	allocator->deallocate(allocator->state, (*self));
	*self = NULL;
}

