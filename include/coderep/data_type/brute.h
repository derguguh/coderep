#ifndef CODEREP_DATA_TYPES_BRUTE_H
#define CODEREP_DATA_TYPES_BRUTE_H

#include "coderep/data_type/output_mode.h"
#include "coderep/data_type/string.h"

struct brute {
	struct string *name;
	struct string *value;
};

struct brute *brute_alloc(struct string **name, struct string **value);

void brute_print(const struct brute *self, enum output_mode output_mode);

void brute_drop(struct brute **self);

#endif
