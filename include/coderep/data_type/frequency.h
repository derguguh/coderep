#ifndef CODEREP_DATA_TYPES_FREQUENCY_H
#define CODEREP_DATA_TYPES_FREQUENCY_H

#include "coderep/data_type/output_mode.h"
#include "coderep/data_type/string.h"
#include "coderep/data_type/node.h"
#include <stdbool.h>

struct frequency {
	struct allocator *allocator;
	struct string *name;
	struct node **buckets;
	size_t buckets_count;
	size_t element_count;
};

extern const size_t FREQUENCY_DEFAULT_SIZE;

struct frequency *frequency_alloc(struct allocator *allocator, struct string **name, size_t initial_capacity);

bool frequency_has(struct frequency *self, const struct string *value);

bool frequency_increment(struct frequency *self, const struct string *value);

void frequency_intern(struct frequency *self, const struct string *value);

void frequency_push(struct frequency *self, struct string **value);

void frequency_print(struct frequency *self, enum output_mode output_mode);

void frequency_drop(struct frequency **self);

#endif
