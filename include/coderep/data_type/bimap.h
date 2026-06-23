#ifndef CODEREP_DATA_TYPE_BIMAP_H
#define CODEREP_DATA_TYPE_BIMAP_H

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/string.h"

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

struct bimap *bimap_alloc(struct allocator *allocator, size_t inital_capacity);

bool bimap_has(const struct bimap *self, const struct string *value);

static inline bool bimap_length(const struct bimap *self)
{
	return self->element_count;
}

size_t bimap_intern(struct bimap *self, const struct string *value);

size_t bimap_push(struct bimap *self, struct string **value);

size_t bimap_get_id_of(struct bimap *self, struct string *value);
struct node *bimap_get_node_of(struct bimap *self, size_t id);
struct string *bimap_get_value_of(struct bimap *self, size_t id);

void bimap_print(const struct bimap *self);

void bimap_drop(struct bimap **self);

#endif
