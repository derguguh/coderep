#include "coderep/tools/source.h"
#include "coderep/tools/file_buffer.h"
#include "coderep/macros.h"

#include <stdlib.h>

struct source *source_alloc(struct string *file_path)
{
	struct file_buffer *file_buffer = file_buffer_alloc(file_path);
	struct string *line_buffer = string_alloc();
	struct source *self = malloc(sizeof(struct source));
	
	if (!self) MALLOC_ERR_EXIT("struct file_buffer for lua");

	self->file_buffer = file_buffer;
	self->line_buffer = line_buffer;
	self->column = 0;
	self->line = 0;

	return self;
}

void source_advance(struct source *self)
{
	file_buffer_advance(self->file_buffer);

	if (self->file_buffer->char_ == '\n') {
		string_clear(self->line_buffer);
		self->line++;
		self->column = 0;
	} else {
		string_push(self->line_buffer, self->file_buffer->char_);
		self->column++;
	}
}

void source_print_error(const struct source *self, const char *const error_text, int error_length)
{
	if (error_length > self->column) {
		PRINTF_ANSI(stdout, ANSI_LOG, "'error_length' %d passed to 'source_print_error' is bigger than 'source->column' %lu. Will be truncated",
				error_length, self->column);

		error_length = self->column;
	}

	printf(ANSI_ERR "File buffer: %s\n\t~:", error_text);
	string_print(self->line_buffer);
	printf("\n\t  ");

	for (int i = 0; i < self->column - error_length; i++) putchar(' ');
	putchar('^');
	for (int i = 0; i < error_length - 1; i++) putchar('~');
}

void source_print(const struct source *self, bool trim_spaces)
{
	puts("Source: {");
	putchar('\t');
	file_buffer_print(self->file_buffer, trim_spaces);
	printf("\tLine Buffer: ");
	string_print(self->line_buffer);
	printf("\n\tLine: %lu\n", self->line);
	printf("\tColumn: %lu\n", self->column);
	puts("}\n");
}

void source_drop(struct source **self)
{
	if (!self || !(*self)) return;

	file_buffer_drop(&(*self)->file_buffer);
	string_drop(&(*self)->line_buffer);

	free(*self);
	*self = NULL;
}

