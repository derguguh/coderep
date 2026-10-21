#include "coderep/data_type/string_slice.h"

struct string_slice string_slice(const struct string *self, size_t from, size_t to)
{
	if (from > to || from > self->length) return (struct string_slice) { self->chars, 0, 0 };
	if (to > self->length) to = self->length;

	return (struct string_slice) { (self->chars) + from, to - from, from };
}

