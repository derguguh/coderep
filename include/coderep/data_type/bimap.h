#ifndef CODEREP_DATA_TYPE_BIMAP_H
#define CODEREP_DATA_TYPE_BIMAP_H

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/frequency.h"

struct bimap {
	struct frequency *frequency;
	struct block *block;
	struct block *current;
	size_t block_size;
	size_t blocks;
};

struct bimap *bimap_alloc(struct allocator *allocator, struct string **name, size_t buckets);

static inline bool bimap_has(const struct bimap *self, struct string *value)
{
	return frequency_has(self->frequency, value);
}

static inline bool bimap_length(const struct bimap *self)
{
	return self->frequency->element_count;
}

size_t bimap_intern(struct bimap *self, struct string *value);

size_t bimap_push(struct bimap *self, struct string **value);

void bimap_print(const struct bimap *self);

void bimap_drop(struct bimap **self);

#endif
