#include "backend/text/lexer.h"

#include "backend/generic/lexer.h"
#include "backend/text/token.h"

#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/file.h"

#include <ctype.h>
#include <stdio.h>

enum text_token text_lexer_word(struct text_lexer *self, struct file *file, struct string *scratch_buffer)
{
	read_word(file, scratch_buffer);
	frequency_intern(self->word, scratch_buffer);

	return TEXT_TOKEN_WORD;
}

enum text_token text_lexer_number(struct text_lexer *self, struct file *file, struct string *scratch_buffer)
{
	enum text_token type = TEXT_TOKEN_LIT_INTEGER;

	read_int(file, scratch_buffer, true);
	if (file->buffer->char_ == '.') {
		push_and_advance(file, scratch_buffer);
		read_int(file, scratch_buffer, true);
		type = TEXT_TOKEN_LIT_DECIMAL;
	}

	frequency_intern(self->number, scratch_buffer);

	return type;
}

enum text_token text_lexer_characters(struct text_lexer *self, struct file *file, struct string *scratch_buffer)
{
	enum text_token token = TEXT_TOKEN_CHAR;

	unsigned char b = file->buffer->char_;
	int utf8_len = 1;
	if      ((b & 0x80) == 0x00) utf8_len = 1;
	else if ((b & 0xE0) == 0xC0) utf8_len = 2;
	else if ((b & 0xF0) == 0xE0) utf8_len = 3;
	else if ((b & 0xF8) == 0xF0) utf8_len = 4;

	for (int i = 0; i < utf8_len; i++) push_and_advance(file, scratch_buffer);

	frequency_intern(self->character, scratch_buffer);

	return token;
}

enum text_token text_lexer_next(struct text_lexer *self, struct file *file, struct string *scratch_buffer)
{
	string_clear(scratch_buffer);
	skip_whitespaces(file);

	if	(file->buffer->char_ == EOF) return TEXT_TOKEN_EOF;
	else if (isalpha(file->buffer->char_) || file->buffer->char_ == '_') return text_lexer_word(self, file, scratch_buffer);
	else if (isdigit(file->buffer->char_)) return text_lexer_number(self, file, scratch_buffer);
	else return text_lexer_characters(self, file, scratch_buffer);
}

