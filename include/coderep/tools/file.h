#ifndef CODEREP_TOOLS_FILE_H
#define CODEREP_TOOLS_FILE_H

#include "coderep/tools/file_buffer.h"

#include <stdbool.h>

struct file {
	struct file_buffer *file_buffer;
	struct string *line_buffer;
	size_t column;
	size_t line;
};

struct file *file_alloc(struct string *file_path);

void file_advance(struct file *self);

void file_print_error(const struct file *self, const char *const error_text, int error_length);

void file_print(const struct file *self, bool trim_spaces);

void file_drop(struct file **self);

#endif
