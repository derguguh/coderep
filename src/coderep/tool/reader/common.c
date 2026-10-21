#include "coderep/tool/reader/common.h"

#include "coderep/data_type/string.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

// ------------ Helpers ------------
static inline bool isoctal(char c) { return c >= '0' && c <= '7'; }
static inline bool isbinary(char c) { return c == '0' || c == '1'; }

// ------------ General ------------
size_t read_while(struct file *file, struct string *buffer, condition_fn condition)
{
	size_t amount = 0;
	while (condition(file->buffer->char_) && file->buffer->char_ != EOF) {
		push_and_advance(file, buffer);
		amount++;
	}
	return amount;
}

// ------------ Others ------------
size_t read_word(struct file *file, struct string *buffer)
{
	size_t amount = 0;
	while (isalnum(file->buffer->char_) || file->buffer->char_ == '_') {
		push_and_advance(file, buffer);
		amount++;
	}
	return amount;
}

// ------------ Numbers ------------
size_t read_int(struct file *file, struct string *buffer, bool accept_underscore)
{
	size_t amount = 0;
	while (isdigit(file->buffer->char_) || (accept_underscore && file->buffer->char_ == '_')) {
		push_and_advance(file, buffer);
		amount++;
	}
	return amount;
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
				buffer->length + 1);
		return -1;
	} else return amount;
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
				"Hexadecimal numbers need to have at least one digit after the prefix",
				buffer->length + 1);
		return -1;
	} else return amount;
}

ssize_t read_octal(struct file *file, struct string *buffer, bool accept_underscore, bool need_digits)
{
	ssize_t amount = 0;
	while (isoctal(file->buffer->char_) || (accept_underscore && file->buffer->char_ == '_')) {
		push_and_advance(file, buffer);
		amount++;
	}

	if (need_digits && !isoctal(string_get(buffer, buffer->length - 1))) {
		file_print_error(file,
				"Octal numbers need to have at least one digit after the prefix",
				buffer->length + 1);
		return -1;
	} else return amount;
}

ssize_t read_binary(struct file *file, struct string *buffer, bool accept_underscore, bool need_digits)
{
	ssize_t amount = 0;
	while (isbinary(file->buffer->char_) || (accept_underscore && file->buffer->char_ == '_')) {
		push_and_advance(file, buffer);
		amount++;
	}

	if (need_digits && !isbinary(string_get(buffer, buffer->length - 1))) {
		file_print_error(file,
				"Binary numbers need to have at least one digit after the prefix",
				buffer->length + 1);
		return -1;
	} else return amount;
}

