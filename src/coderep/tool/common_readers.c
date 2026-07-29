#include "coderep/tool/common_readers.h"

#include "coderep/data_type/string.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

void push_and_advance(struct file *file, struct string *string)
{
	string_push(string, file->buffer->char_);

	file_advance(file);
}

/*
 * Words
 */

size_t read_word(struct file *file, struct string *buffer)
{
	size_t amount = 0;

	while (isalnum(file->buffer->char_) ||file->buffer->char_ == '_') {
		push_and_advance(file, buffer);
		amount++;
	}
	return amount;
}

/*
 * Numbers
 */

size_t read_int(struct file *file, struct string *buffer, bool accept_underscore)
{
	size_t amount = 0;
	
	while (isdigit(file->buffer->char_) || (accept_underscore && file->buffer->char_ == '_')) {
		push_and_advance(file, buffer);
		amount++;
	}
	return amount;
}

ssize_t read_hex(struct file *file, struct string *buffer, bool accept_underscore, bool need_digits)
{
	ssize_t amount = 0;
	while (isxdigit(file->buffer->char_) || (accept_underscore && file->buffer->char_ == '_')) {
		push_and_advance(file, buffer);
		amount++;
	}

	if (need_digits && !isxdigit(string_get(buffer, buffer->length - 1))) {
		file_print_error(file,
				"Hexadecimal numbers need to have at least one digit after the prefix '0x'/'0X'",
				buffer->length);
		return -1;
	} else return amount;
}

ssize_t read_scientific_suffix(struct file *file, struct string *buffer, bool accept_underscore, bool need_digits)
{
	ssize_t amount = 0;
	push_and_advance(file, buffer);
	if (file->buffer->char_ == '+' || file->buffer->char_ == '-') {
		push_and_advance(file, buffer);
		amount++;
	}
	read_int(file, buffer, accept_underscore);

	if (need_digits && !isdigit(string_get(buffer, buffer->length - 1))) {
		file_print_error(file,
				"Scientific numbers need to have at least one digit after the prefix 'e', 'e+' or 'e-'",
				buffer->length);
		return -1;
	} else return amount;
}

ssize_t read_string(struct file *file, struct string *_buffer, struct arena *tmp_allocator, struct string_handler *handler)
{
	struct string *tmp = string_alloc(&tmp_allocator->abstract, handler->end_sequence->length);
	ssize_t amount_read = 0;

	while (true) {
redo:
		string_clear(tmp);
		for (size_t i = 0; i < handler->end_sequence->length; i++) {
			if (string_get(handler->end_sequence, i) == file->buffer->char_) push_and_advance(file, tmp);
			else {
				if (_buffer != NULL) string_append(_buffer, tmp);
				if (string_get(handler->end_sequence, 0) == file->buffer->char_) goto redo;
			}
		}
		if (tmp->length == handler->end_sequence->length) break;

		switch (file->buffer->char_) {
			case EOF:
				file_print_error(file, "Strings need to be finalized with their own end sequence, but EOF (End of File) was founded",
						_buffer != NULL ? _buffer->length : 1);
				return -1;
			break;
			case '\n':
				if (!handler->accept_new_line) {
					file_print_error(file, "This string don't accept new lines",
							_buffer != NULL ? _buffer->length : 1);
					return -1;
				}
			default:
				if (handler->_escape_handler != NULL && file->buffer->char_ == handler->escape_char) {
					if (!handler->_escape_handler(file, _buffer, tmp_allocator)) return -1;
				} else {
					if (_buffer != NULL) string_push(_buffer, file->buffer->char_);
					file_advance(file);
				}
			break;
		}
	}

	return amount_read;
}

/*
 * Skipers
 */

void skip_shebang(struct file *file)
{
	if (file->buffer->char_ == '#' && file_buffer_peek_n(file->buffer, 1)) {
		while (file->buffer->char_ != '\n' && file->buffer->char_ != EOF) file_advance(file);
	}
}

size_t skip_blank(struct file *file)
{
	size_t amount = 0;

	while (isblank(file->buffer->char_) && file->buffer->char_ != EOF) {
		file_advance(file);
		amount++;
	}
	return amount;
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

