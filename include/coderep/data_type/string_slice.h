#ifndef CODEREP_DATA_TYPE_STRING_SLICE_H
#define CODEREP_DATA_TYPE_STRING_SLICE_H

#include "coderep/data_type/string.h"

#include <stddef.h>

struct string_slice {
	const char *const chars;
	const size_t length;
	const size_t offset;
};

struct string_slice string_slice(const struct string *self, size_t from, size_t to);

#endif
