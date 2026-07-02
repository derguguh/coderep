#ifndef CODEREP_DATA_TYPE_BRUTE_H
#define CODEREP_DATA_TYPE_BRUTE_H

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/data_type/string.h"

struct brute {
	struct allocator *allocator;
	struct string *name;
	struct string *value;
};

struct brute *brute_alloc(struct allocator *allocator, struct string **name, struct string **value);
struct brute *brute_from_size_t(struct allocator *allocator, struct string **name, size_t value);
struct brute *brute_from_ssize_t(struct allocator *allocator, struct string **name, ssize_t value);

void brute_print(const struct brute *self, enum output_mode output_mode);

void brute_drop(struct brute **self);

#endif
