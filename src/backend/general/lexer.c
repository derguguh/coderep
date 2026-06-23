#include "backend/general/lexer.h"

#include "coderep/data_type/string.h"

#include <ctype.h>
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

	while (isalnum(file->buffer->char_) || file->buffer->char_ == '_') {
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
		string_push(buffer, file->buffer->char_);
		file_advance(file);
		amount++;
	}
	return amount;
}

bool read_hex(struct file *file, struct string *scratch_buffer, bool need_digits, bool accept_underscore)
{
	while (isxdigit(file->buffer->char_) || (accept_underscore && file->buffer->char_ == '_')) push_and_advance(file, scratch_buffer);

	if (need_digits && !isxdigit(string_get(scratch_buffer, scratch_buffer->length - 1))) {
		file_print_error(file,
				"Hexadecimal numbers need to have at least one digit after the prefix '0x'/'0X'",
				scratch_buffer->length);
		return false;
	} else return true;
}

bool read_scientific_suffix(struct file *file, struct string *scratch_buffer, bool need_digits, bool accept_underscore)
{
	push_and_advance(file, scratch_buffer);
	if (file->buffer->char_ == '+' || file->buffer->char_ == '-') push_and_advance(file, scratch_buffer);
	read_int(file, scratch_buffer, accept_underscore);

	if (need_digits && !isdigit(string_get(scratch_buffer, scratch_buffer->length - 1))) {
		file_print_error(file,
				"Scientific numbers need to have at least one digit after the prefix 'e', 'e+' or 'e-'",
				scratch_buffer->length);
		return false;
	} else return true;
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

