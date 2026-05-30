#include "backend/general/lexer.h"

#include "coderep/data_type/string.h"

#include <ctype.h>

void push_and_advance(struct file *file, struct string *string)
{
	string_push(string, file->buffer->char_);

	file_advance(file);
}

size_t skip_whitespaces(struct file *file)
{
	size_t amount = 0;

	while (isspace(file->buffer->char_) && file->buffer->char_ != EOF) {
		file_advance(file);
		amount++;
	}

	return amount;
}

size_t read_int(struct file *file, struct string *buffer)
{
	size_t amount = 0;
	
	while (isdigit(file->buffer->char_)) {

		string_push(buffer, file->buffer->char_);
		file_advance(file);

		amount++;
	}

	return amount;
}

void skip_shebang(struct file *file)
{
	if (file->buffer->char_ == '#' && file_buffer_peek_n(file->buffer, 1)) {
		while (file->buffer->char_ != '\n' && file->buffer->char_ != EOF) file_advance(file);
	}
}

