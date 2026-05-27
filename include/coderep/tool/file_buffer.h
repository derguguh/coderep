#ifndef CODEREP_TOOLS_BUFFER_FILE_H
#define CODEREP_TOOLS_BUFFER_FILE_H

#include "coderep/data_type/string.h"
#include <stdio.h>
#include <stddef.h>

#define BUFFER_SIZE 4096

struct file_buffer {
	FILE *file;
	char buffer[BUFFER_SIZE];
	size_t buffer_length;
	size_t buffer_cursor;
	size_t block;
	char char_;
};

struct file_buffer *file_buffer_alloc(struct string *file_path);

void file_buffer_advance(struct file_buffer *self);

char file_buffer_peek(const struct file_buffer *self);
char file_buffer_peek_next(const struct file_buffer *self);

void file_buffer_print(const struct file_buffer *self, bool trim_spaces);

void file_buffer_drop(struct file_buffer **self);

#endif
