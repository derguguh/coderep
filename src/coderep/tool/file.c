#include "coderep/tool/file.h"

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/macros.h"

#include <stdlib.h>
#include <string.h>

const size_t RING_LINE_BUFFER_SIZE = 40;
const char *const PRINT_PREFIX = "\n\t|";

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
	MALLOC_ERR_EXIT("struct file_buffer");
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
		self->column++;
	}
}

void file_print_line_buffer(const struct file *self)
{
	if (self->line_buffer->length > RING_LINE_BUFFER_SIZE) {
		size_t offset = self->line_buffer->length - RING_LINE_BUFFER_SIZE;

		fprintf(stderr, "...%.*s", (int)RING_LINE_BUFFER_SIZE, self->line_buffer->chars + offset);

	} else string_print(self->line_buffer);
}

void file_print_error(const struct file *self, const char *const error_text, size_t error_length)
{
	if (error_length > self->column) {
		LOGF("'error_length' %zu passed to 'file_print_error' is bigger than 'file->column' %lu. Will be truncated",
				error_length, self->column);
		error_length = self->column;
	} else if (error_length < 1) {
		LOGF("'error_length' %zu passed to 'file_print_error' is less than 1, which means it don't have anything to be printed. Will be set to 1\n",
				error_length);
		error_length = 1;
	}

	PRINTF_ERR("%zu;%zu in %-60s\n\t|%s\n\t|", self->line, self->column, self->buffer->_file_name, error_text);
	file_print_line_buffer(self);

	size_t tab_count = string_count(self->line_buffer, '\t');

	size_t spaces_total;
	if (self->line_buffer->length > RING_LINE_BUFFER_SIZE) spaces_total = RING_LINE_BUFFER_SIZE - tab_count - error_length + strlen(PRINT_PREFIX);
	else spaces_total = self->column - error_length;

	printf(PRINT_PREFIX);
	for (size_t i = 0; i < spaces_total; i++) putchar(' ');
	for (size_t i = 0; i < tab_count; i++) putchar('\t');
	for (size_t i = 0; i < error_length - 1; i++) putchar('~');
	puts("^\n");
}

void file_print(const struct file *self, bool trim_spaces)
{
	puts("Source: {");

	putchar('\t');
	file_buffer_print(self->buffer, trim_spaces);

	printf("\tLine Buffer: ");
	string_print(self->line_buffer);
	putchar('\n');

	printf("\tLine: %zu\n", self->line);
	printf("\tColumn: %zu\n", self->column);

	puts("}\n");
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

