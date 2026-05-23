#ifndef CODEREP_TOOLS_SOURCE_H
#define CODEREP_TOOLS_SOURCE_H

#include "coderep/tools/file_buffer.h"

#include <stdbool.h>

struct source {
	struct file_buffer *file_buffer;
	struct string *line_buffer;
	size_t column;
	size_t line;
};

struct source *source_alloc(struct string *file_path);

void source_advance(struct source *self);

void source_print_error(const struct source *self, const char *const error_text, int error_length);

void source_print(const struct source *self, bool trim_spaces);

void source_drop(struct source **self);

#endif
