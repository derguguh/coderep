#ifndef CODEREP_TOOLS_FILE_H
#define CODEREP_TOOLS_FILE_H

#include "coderep/tool/file_buffer.h"

#include <stdbool.h>

struct file {
	struct file_buffer *buffer;
	struct string *line_buffer;
	size_t column;
	size_t line;
};

extern const size_t RING_LINE_BUFFER_SIZE;

struct file *file_alloc(struct allocator *allocator);

bool file_open(struct file *self, const char *const file_path);

void file_advance(struct file *self);

void file_print_error(const struct file *self, const char *const error_text, int error_length);

void file_print(const struct file *self, bool trim_spaces);

void file_drop(struct file **self);

#endif
