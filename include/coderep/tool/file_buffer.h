#ifndef CODEREP_TOOLS_BUFFER_FILE_H
#define CODEREP_TOOLS_BUFFER_FILE_H

#include "coderep/allocator/allocator.h"

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

#define BUFFER_SIZE_ 4096
extern const size_t BUFFER_SIZE;

struct file_buffer {
	struct allocator *allocator;

	const char *_file_name;
	FILE *_file;

	char buffer[BUFFER_SIZE_];
	size_t buffer_length;
	size_t buffer_cursor;
	char char_;

	size_t block;
};

// ------------ Allocation ------------
struct file_buffer *file_buffer_alloc(struct allocator *allocator);

// ------------ Usage ------------
bool file_buffer_open(struct file_buffer *self, const char *const file_path);
bool file_buffer_open_stream(struct file_buffer *self, FILE **stream);

void file_buffer_advance(struct file_buffer *self);

char file_buffer_peek_n(const struct file_buffer *self, size_t n);

// ------------ Deallocation ------------
void file_buffer_print(const struct file_buffer *self, bool trim_spaces);

// ------------ Deallocation ------------
void file_buffer_drop(struct file_buffer **self);

#endif
