#ifndef CODEREP_TOOLS_SOURCE_H
#define CODEREP_TOOLS_SOURCE_H

#include "coderep/tools/file_buffer.h"

struct source {
	struct file_buffer *file_buffer;
	struct string *line_buffer;
	size_t column;
	size_t line;
	unsigned char char_;
};

struct source *source_alloc(struct string *file_path);

void source_advance(struct source *self);

void source_drop(struct source **self);

#endif
