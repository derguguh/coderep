#include "coderep/tool/file_buffer.h"

#include "coderep/allocator/allocator.h"
#include "coderep/macros.h"
#include "coderep/printer/printer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const size_t BUFFER_SIZE = BUFFER_SIZE_;

struct file_buffer *file_buffer_alloc(struct allocator *allocator)
{
	struct file_buffer *self = allocator->allocate(allocator->state, sizeof(struct file_buffer));

	if (!self) ALLOC_ERR_EXIT("struct file_buffer");

	*self = (struct file_buffer) {
		.allocator = allocator,
	};

	return self;
}

bool file_buffer_open(struct file_buffer *self, const char *const file_path)
{
	FILE *file = fopen(file_path, "r");
	if (file == NULL) goto defer;
	self->_file_name = file_path;

	return file_buffer_open_stream(self, &file);
defer:
	if (file) fclose(file);

	printf_err("File \"%s\" don't meet the criteria. It needs to exist, be possible to read, and be not empty", file_path);
	self->_file_name = NULL;
	self->_file = NULL;
	self->char_ = EOF;

	return false;
}

bool file_buffer_open_stream(struct file_buffer *self, FILE **stream)
{
	if (self->_file != NULL) fclose(self->_file);
	self->_file = *stream;
	*stream = NULL;

	self->buffer_cursor = 0;
	self->block = 0;

	self->buffer_length = fread(self->buffer, 1, BUFFER_SIZE, self->_file);
	if (self->buffer_length == 0) goto defer;

	self->char_ = self->buffer[0];

	return true;
defer:
	if (self->_file) fclose(self->_file);

	printf_err("File \"%s\" don't meet the criteria. It needs to exist, be possible to read, and be not empty", self->_file_name == NULL ? "" : self->_file_name);
	self->_file_name = NULL;
	self->_file = NULL;
	self->char_ = EOF;

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
	println("File Buffer: {");

	print("\tCursor: ");
	print_size_t(self->buffer_cursor);
	println("");

	print("\tLength: ");
	print_size_t(self->buffer_length);
	println("");

	print("\tBlock: ");
	print_size_t(self->block);
	println("");

	print("\tBuffer (4096, trimed: ");
	print_bool(trim_spaces);
	println("): [");

	if (trim_spaces) for (size_t i = 0; i < self->buffer_length; i++) print_char(self->buffer[i]);
	else print_cstr(self->buffer, BUFFER_SIZE);

	println("]}");
}

void file_buffer_drop(struct file_buffer **self)
{
	if (!self || !(*self)) return;

	if ((*self)->_file != NULL) fclose((*self)->_file);
	(*self)->_file = NULL;

	(*self)->allocator->deallocate((*self)->allocator->state, *self);
	*self = NULL;
}
