#ifndef CODEREP_DATA_TYPES_BRUTE_H
#define CODEREP_DATA_TYPES_BRUTE_H

#include "string.h"

struct brute {
	struct string *name;
	struct string *value;
};

struct brute *brute_new(struct string **name, struct string **value);

void brute_print(struct brute *self);

void brute_drop(struct brute **self);

#endif
