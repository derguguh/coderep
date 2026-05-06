#include "coderep/tools/file_buffer.h"
#include "coderep/data_types/string.h"
#include "coderep/macros.h"

#include <stdio.h>
#include <stdlib.h>

struct file_buffer *file_buffer_alloc(struct string *file_path)
{
	struct file_buffer *self = malloc(sizeof(struct file_buffer));

	if (!self) MALLOC_ERR_EXIT("struct file_buffer for lua");

	FILE *file = fopen(string_to_cstr(file_path), "r");
	if (file == NULL) {
		free(self);

		PRINTF_ERR("Unkown file \"%s\". Does it exists or have permission to open?", string_to_cstr(file_path));
		exit(ERROR_CODE_UNKNOWN_FILE);
	}
	self->buffer_length = fread(self->buffer, 1, BUFFER_SIZE, file);
	if (self->buffer_length == 0) {
		fclose(file);
		free(self);

		PRINTF_ERR("File \"%s\" is empty", string_to_cstr(file_path));
		exit(ERROR_CODE_EMPTY_FILE);
	}

	self->file = file;

	self->char_ = self->buffer[0];
	self->buffer_cursor = 0;
	self->block = 0;

	return self;
}

void file_buffer_advance(struct file_buffer *self)
{
	self->buffer_cursor++;

	if (self->buffer_cursor < self->buffer_length) self->char_ = self->buffer[self->buffer_cursor];
	else if (self->buffer_cursor == BUFFER_SIZE && !feof(self->file)) {
		self->buffer_cursor = 0;

		self->buffer_length = fread(self->buffer, 1, BUFFER_SIZE, self->file);

		if (self->buffer_length == 0) self->char_ = EOF;
		else {
			self->char_ = self->buffer[0];
			self->block++;
		}
	} else {
		LOGF("Attemp to advance in a overflow buffer. Current cursor is %lu, and the buffer length is %lu",
				self->buffer_cursor, self->buffer_length);
		self->char_ = EOF;
	}
}

unsigned char file_buffer_peek(const struct file_buffer *self)
{
	return self->char_;
}

unsigned char file_buffer_peek_next(const struct file_buffer *self)
{
	if (self->buffer_cursor + 1 < self->buffer_length) return self->buffer[self->buffer_cursor + 1];
	else if (self->buffer_cursor + 1 == BUFFER_SIZE && !feof(self->file)) {
		
		unsigned char tmp = 0;
		unsigned char count = fread(&tmp, 1, 1, self->file);
		fseek(self->file, -1, SEEK_CUR);

		if (count == 0) tmp = EOF;
		return tmp;
	} else {

		LOG("Attemp to peek character after buffer");
		return EOF;
	}
}

void file_buffer_print(const struct file_buffer *self, bool trim_spaces)
{
	puts("File Buffer:");
	printf("\tCursor: %lu\n", self->buffer_cursor);
	printf("\tLength: %lu\n", self->buffer_length);
	printf("\tBlock: %lu\n", self->block);
	printf("\tBuffer (4096, trimed: %d): {\n", trim_spaces);

	if (trim_spaces) for (size_t i = 0; i < self->buffer_length; i++) putchar(self->buffer[i]);
	else printf("%4096s", self->buffer);

	puts("}\n");
}

void file_buffer_drop(struct file_buffer **self)
{
	if (!self || !(*self)) return;

	if ((*self)->file != NULL) fclose((*self)->file);
	free(*self);
}
