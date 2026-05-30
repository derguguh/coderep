#include "backend/text/lexer.h"

#include "backend/general/lexer.h"
#include "backend/text/token.h"

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/file.h"

#include <ctype.h>
#include <stdio.h>

enum text_token text_lexer_word(struct text_lexer *self, struct allocator *allocator, struct file *file)
{
	while (isalnum(file->buffer->char_) || file->buffer->char_ == '_') {

		push_and_advance(file, self->string_buffer);
	}

	frequency_intern(self->word, self->string_buffer);

	return TEXT_TOKEN_WORD;
}

enum text_token text_lexer_number(struct text_lexer *self, struct allocator *allocator, struct file *file)
{
	enum text_token type = TEXT_TOKEN_LIT_INTEGER;

	read_int(file, self->string_buffer);
	if (file->buffer->char_ == '.') {
		push_and_advance(file, self->string_buffer);
		read_int(file, self->string_buffer);
		type = TEXT_TOKEN_LIT_DECIMAL;
	}

	frequency_intern(self->number, self->string_buffer);

	return type;
}

enum text_token text_read_ascii(struct file *file)
{
	switch (file->buffer->char_) {
		case '!':	return TEXT_TOKEN_CHAR_EXCLAMATION;	break;
		case '\"':	return TEXT_TOKEN_CHAR_DOUBLE_QUOTE;	break;
		case '#':	return TEXT_TOKEN_CHAR_HASH;		break;
		case '$':	return TEXT_TOKEN_CHAR_DOLLAR_SIGN;	break;
		case '%':	return TEXT_TOKEN_CHAR_PERCENT;		break;
		case '&':	return TEXT_TOKEN_CHAR_AMPERSAND;	break;
		case '\'':	return TEXT_TOKEN_CHAR_SINGLE_QUOTE;	break;
		case '(':	return TEXT_TOKEN_CHAR_LPARENTHESIS;	break;
		case ')':	return TEXT_TOKEN_CHAR_RPARENTHESIS;	break;
		case '*':	return TEXT_TOKEN_CHAR_STAR;		break;
		case '+':	return TEXT_TOKEN_CHAR_PLUS;		break;
		case ',':	return TEXT_TOKEN_CHAR_COMMA;		break;
		case '-':	return TEXT_TOKEN_CHAR_MINUS;		break;
		case '.':	return TEXT_TOKEN_CHAR_PERIOD;		break;
		case '/':	return TEXT_TOKEN_CHAR_SLASH;		break;
		case ':':	return TEXT_TOKEN_CHAR_COLON;		break;
		case ';':	return TEXT_TOKEN_CHAR_SEMICOLON;	break;
		case '<':	return TEXT_TOKEN_CHAR_LESS_THAN;	break;
		case '=':	return TEXT_TOKEN_CHAR_EQUALS;		break;
		case '>':	return TEXT_TOKEN_CHAR_GREATER_THAN;	break;
		case '?':	return TEXT_TOKEN_CHAR_QUESTION;	break;
		case '@':	return TEXT_TOKEN_CHAR_AT;		break;
		case '[':	return TEXT_TOKEN_CHAR_LBRACKET;	break;
		case '\\':	return TEXT_TOKEN_CHAR_REVERSE_SLASH;	break;
		case ']':	return TEXT_TOKEN_CHAR_RBRACKET;	break;
		case '^':	return TEXT_TOKEN_CHAR_CARET;		break;
		case '`':	return TEXT_TOKEN_CHAR_BACKTICK;	break;
		case '{':	return TEXT_TOKEN_CHAR_RBRACE;		break;
		case '|':	return TEXT_TOKEN_CHAR_VBAR;		break;
		case '}':	return TEXT_TOKEN_CHAR_RBRACE;		break;
		case '~':	return TEXT_TOKEN_CHAR_TILDE;		break;
		default:
			file_print_error(file, "Can't identify a character", 1);
			return TEXT_TOKEN_UNKNOWN;
		break;
	};
}

enum text_token text_lexer_characters(struct text_lexer *self, struct allocator *allocator, struct file *file)
{
	enum text_token token = TEXT_TOKEN_CHAR_UTF8;

	unsigned char b = file->buffer->char_;
	int utf8_len = 1;
	if      ((b & 0x80) == 0x00) utf8_len = 1;
	else if ((b & 0xE0) == 0xC0) utf8_len = 2;
	else if ((b & 0xF0) == 0xE0) utf8_len = 3;
	else if ((b & 0xF8) == 0xF0) utf8_len = 4;

	if (utf8_len == 1) token = text_read_ascii(file);

	for (int i = 0; i < utf8_len; i++) push_and_advance(file, self->string_buffer);

	if (token == TEXT_TOKEN_UNKNOWN) frequency_intern(self->unknown, self->string_buffer);
	else frequency_intern(self->character, self->string_buffer);

	return token;
}

enum text_token text_lexer_next(struct text_lexer *self, struct arena *allocator, struct file *file)
{
	skip_whitespaces(file);
	string_clear(self->string_buffer);

	if	(file->buffer->char_ == EOF) return TEXT_TOKEN_EOF;
	else if (isalpha(file->buffer->char_) || file->buffer->char_ == '_') return text_lexer_word(self, &allocator->abstract, file);
	else if (isdigit(file->buffer->char_)) return text_lexer_number(self, &allocator->abstract, file);
	else return text_lexer_characters(self, &allocator->abstract, file);
}

