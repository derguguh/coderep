#include "coderep/tool/file.h"

#include "coderep/allocator/allocator.h"
#include "coderep/consts.h"
#include "coderep/data_type/string.h"
#include "coderep/macros.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/printer/printer.h"

#include <stdlib.h>
#include <string.h>

const size_t RING_LINE_BUFFER_SIZE = 40;
#define PRINT_PREFIX_ "\t| "
const char *const PRINT_PREFIX = PRINT_PREFIX_;

struct file *file_alloc(struct allocator *allocator)
{
	struct file_buffer *buffer = file_buffer_alloc(allocator);
	struct string *line_buffer = string_alloc(allocator, 100);
	struct file *self = allocator->allocate(allocator->state, sizeof(struct file));
	
	if (!self) goto defer;

	*self = (struct file) {
		.buffer = buffer,
		.line_buffer = line_buffer,
		.line = 1,
		.column = 1,
	};

	return self;
defer:
	ALLOC_ERR_EXIT("struct file_buffer");
}

bool file_open(struct file *self, const char *const file_path)
{
	string_clear(self->line_buffer);
	self->line = 1;
	self->column = 1;

	return file_buffer_open(self->buffer, file_path);
}

void file_reset_line_buffer(struct file *self)
{
	memmove(self->line_buffer->chars, self->line_buffer->chars + RING_LINE_BUFFER_SIZE, RING_LINE_BUFFER_SIZE);
	self->line_buffer->length = RING_LINE_BUFFER_SIZE;
}

void file_advance(struct file *self)
{
	if (self->buffer->char_ == '\n') {
		string_clear(self->line_buffer);
		self->line++;
		self->column = 0;
	}

	file_buffer_advance(self->buffer);

	if (self->buffer->char_ != '\n') {
		if (self->line_buffer->length > RING_LINE_BUFFER_SIZE * 2) file_reset_line_buffer(self);

		string_push(self->line_buffer, self->buffer->char_);
	} else string_push(self->line_buffer, ' ');
	self->column++;
}

size_t file_get_offset(const struct file *self)
{
	if (self->line_buffer->length > RING_LINE_BUFFER_SIZE) return self->line_buffer->length - RING_LINE_BUFFER_SIZE;
	else return 0;
}

size_t file_print_line_buffer(const struct file *self)
{
	fprintf(stderr, PRINT_PREFIX);
	size_t offset = file_get_offset(self);
	size_t length = self->line_buffer->length;

	if (length > RING_LINE_BUFFER_SIZE) {
		fprintf(stderr, "...");
		length = RING_LINE_BUFFER_SIZE;
	}

	fprintf(stderr, ANSI_GREEN);
	return fprintf(stderr, "%.*s\n", (int)length, self->line_buffer->chars + offset);
}

void file_print_error(const struct file *self, const char *const error_text, size_t error_length)
{
	if (error_length > self->column) {
		LOGF("'error_length' %zu passed to 'file_print_error' is bigger than 'file->column' %zu. Will be truncated",
				error_length, self->column);
		error_length = self->column;
	} else if (error_length > RING_LINE_BUFFER_SIZE) {
		LOGF("'error_length' %zu passed to 'file_print_error' is bigger than 'RING_LINE_BUFFER_SIZE' %zu. Will be truncated",
				error_length, RING_LINE_BUFFER_SIZE);
		error_length = RING_LINE_BUFFER_SIZE;
	} else if (error_length < 1) {
		LOGF("'error_length' %zu passed to 'file_print_error' is less than 1, which means it don't have anything to be printed. Will be set to 1\n",
				error_length);
		error_length = 1;
	}

	printf_err("%zu;%zu in %-60s\n" PRINT_PREFIX_ "%s", self->line, self->column, self->buffer->_file_name, error_text);

	size_t spaces = file_print_line_buffer(self) - error_length;

	fprintf(stderr, ANSI_RESET PRINT_PREFIX_ ANSI_MAGENTA);
	size_t offset = file_get_offset(self);
	if (offset != 0) fprintf(stderr, "   ");
	for (size_t i = 1; i < spaces; i++) {
		if (string_get(self->line_buffer, offset + i - 1) == '\t') putc('\t', stderr);
		else putc(' ', stderr);
	}
	for (size_t i = 1; i < error_length; i++) putc('~', stderr);
	fprintf(stderr, "^" ANSI_RESET "\n\n");
}

void file_print(const struct file *self, bool trim_spaces)
{
	println("Source: {");

	print_char('\t');
	file_buffer_print(self->buffer, trim_spaces);

	print("\tLine Buffer: ");
	print_string(self->line_buffer);
	println("");

	print("\tLine: ");
	print_size_t(self->line);
	println("");

	print("\tColumn: ");
	print_size_t(self->column);
	println("");

	println("}");
}

void file_drop(struct file **self)
{
	if (!self || !(*self)) return;
	struct file *tmp = *self;
	struct allocator *allocator = tmp->buffer->allocator;

	string_drop(&tmp->line_buffer);
	file_buffer_drop(&tmp->buffer);
	allocator->deallocate(allocator->state, *self);

	*self = NULL;
}

