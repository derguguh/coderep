#ifndef CODEREP_TOOLS_BUFFER_FILE_H
#define CODEREP_TOOLS_BUFFER_FILE_H

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

#define BUFFER_SIZE_ 4096
static const size_t BUFFER_SIZE = BUFFER_SIZE_;

struct file_buffer {
	FILE *_file;
	const char *_file_name;
	char buffer[BUFFER_SIZE_];
	size_t buffer_length;
	size_t buffer_cursor;
	size_t block;
	char char_;
};

struct file_buffer *file_buffer_alloc();

bool file_buffer_open(struct file_buffer *self, const char *const file_path);

void file_buffer_advance(struct file_buffer *self);

char file_buffer_peek(const struct file_buffer *self);
char file_buffer_peek_next(const struct file_buffer *self);

void file_buffer_print(const struct file_buffer *self, bool trim_spaces);

void file_buffer_drop(struct file_buffer **self);

#endif
