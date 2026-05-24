#ifndef CODEREP_DATA_TYPES_DISTRIBUTIVE_H
#define CODEREP_DATA_TYPES_DISTRIBUTIVE_H

#include "coderep/data_types/string.h"

struct frequency {
	struct string *name;
	struct node **buckets;
	size_t buckets_count;
};

struct frequency *frequency_alloc(struct string **name, size_t initial_capacity);

void frequency_push(struct frequency *self, struct string **value);

void frequency_print(struct frequency *self);

void frequency_drop(struct frequency **self);

#endif
