#include "coderep/tool/file.h"

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/macros.h"

#include <stdlib.h>

struct file *file_alloc(struct allocator *allocator)
{
	struct file_buffer *buffer = file_buffer_alloc(allocator);
	struct string *line_buffer = string_alloc(allocator, 128);
	struct file *self = malloc(sizeof(struct file));
	
	if (!self) goto defer;

	*self = (struct file) {
		.buffer = buffer,
		.line_buffer = line_buffer,
		.line = 1,
		.column = 1,
	};

	return self;
defer:
	MALLOC_ERR_EXIT("struct file_buffer for lua");
}

bool file_open(struct file *self, const char *const file_path)
{
	string_clear(self->line_buffer);
	self->line = 1;
	self->column = 1;

	return file_buffer_open(self->buffer, file_path);
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
		string_push(self->line_buffer, self->buffer->char_);
		self->column++;
	}
}

void file_print_error(const struct file *self, const char *const error_text, int error_length)
{
	if (error_length > self->column) {
		PRINTF_ANSI(stdout, ANSI_LOG, "'error_length' %d passed to 'file_print_error' is bigger than 'file->column' %lu. Will be truncated",
				error_length, self->column);

		error_length = self->column;
	} else if (error_length < 1) {
		PRINTF_ANSI(stdout, ANSI_LOG, "'error_length' %d passed to 'file_print_error' is less than 1, "
				"which means it don't have anything to be printed. Will be set to 1\n",
				error_length);

		error_length = 1;
	}

	printf(ANSI_ERR "%lu;%lu in %s\n\tError message: %s\n\t|", self->line, self->column, self->buffer->_file_name, error_text);
	string_print(self->line_buffer);
	printf("\n\t ");

	size_t tab_count = string_count(self->line_buffer, '\t');
	size_t tab_as_space_count = tab_count == 0 ? 0 : tab_count * 8 - 2;

	for (int i = 0; i < self->column - error_length + tab_as_space_count; i++) putchar(' ');
	for (int i = 0; i < error_length - 1; i++) putchar('~');
	puts("^\n");
}

void file_print(const struct file *self, bool trim_spaces)
{
	puts("Source: {");
	putchar('\t');
	file_buffer_print(self->buffer, trim_spaces);
	printf("\tLine Buffer: ");
	string_print(self->line_buffer);
	printf("\n\tLine: %lu\n", self->line);
	printf("\tColumn: %lu\n", self->column);
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

