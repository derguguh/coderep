#include "coderep/tool/file_buffer.h"

#include "coderep/macros.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct file_buffer *file_buffer_alloc()
{
	struct file_buffer *self = malloc(sizeof(struct file_buffer));

	if (!self) MALLOC_ERR_EXIT("struct file_buffer for lua");

	*self = (struct file_buffer) {};

	return self;
}

bool file_buffer_open(struct file_buffer *self, const char *const file_path)
{
	if (self->_file != NULL) fclose(self->_file);
	self->_file = NULL;

	self->buffer_cursor = 0;
	self->block = 0;

	FILE *file = fopen(file_path, "r");
	if (file == NULL) goto defer;

	self->buffer_length = fread(self->buffer, 1, BUFFER_SIZE, file);
	if (self->buffer_length == 0) goto defer;

	self->_file = file;
	self->_file_name = file_path;
	self->char_ = self->buffer[0];

	return true;
defer:
	if (file) fclose(file);

	self->_file_name = NULL;
	self->_file = NULL;
	self->char_ = EOF;
	
	PRINTF_ERR("File \"%s\" don't meet the criteria. It needs to exist, be possible to read (permission to read), and be not empty", file_path);

	return false;
}

void file_buffer_advance(struct file_buffer *self)
{
	assert(self->_file != NULL && "struct file_buffer: is not possible to advance in a null file\n");

	self->buffer_cursor++;

	if (self->buffer_cursor < self->buffer_length) self->char_ = self->buffer[self->buffer_cursor];
	else if (self->buffer_cursor == BUFFER_SIZE && !feof(self->_file)) {
		self->buffer_cursor = 0;

		self->buffer_length = fread(self->buffer, 1, BUFFER_SIZE, self->_file);

		if (self->buffer_length == 0) self->char_ = EOF;
		else {
			self->char_ = self->buffer[0];
			self->block++;
		}
	} else {
		if (self->buffer_cursor > self->buffer_length) {
			LOGF("Attemp to advance in a overflow buffer. Current cursor is %lu, and the buffer length is %lu",
					self->buffer_cursor, self->buffer_length);
		}

		self->char_ = EOF;
	}
}

char file_buffer_peek_n(const struct file_buffer *self, size_t n)
{
	assert(self->_file != NULL && "struct file_buffer: is not possible to advance in a null file\n");

	if (self->buffer_cursor + n < self->buffer_length) return self->buffer[self->buffer_cursor + n];
	else if (self->buffer_cursor + n == BUFFER_SIZE && !feof(self->_file)) {
		
		fseek(self->_file, n - 1, SEEK_CUR);
		unsigned char tmp = 0;
		unsigned char count = fread(&tmp, 1, 1, self->_file);
		fseek(self->_file, -n, SEEK_CUR);

		if (count == 0) tmp = EOF;
		return tmp;
	} else {

		LOG("Attemp to peek character after buffer");
		return EOF;
	}
}

void file_buffer_print(const struct file_buffer *self, bool trim_spaces)
{
	puts("File Buffer: {");
	printf("\tCursor: %lu\n", self->buffer_cursor);
	printf("\tLength: %lu\n", self->buffer_length);
	printf("\tBlock: %lu\n", self->block);
	printf("\tBuffer (4096, trimed: %d): [\n", trim_spaces);

	if (trim_spaces) for (size_t i = 0; i < self->buffer_length; i++) putchar(self->buffer[i]);
	else printf("%4096s", self->buffer);

	puts("]}\n");
}

void file_buffer_drop(struct file_buffer **self)
{
	if (!self || !(*self)) return;

	if ((*self)->_file != NULL) fclose((*self)->_file);
	(*self)->_file = NULL;

	free(*self);
	*self = NULL;
}
