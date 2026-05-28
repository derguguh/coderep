#include "backend/general/lexer.h"

#include "backend/general/lexical_table.h"
#include "backend/general/token.h"

#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/file.h"

#include <ctype.h>
#include <stdio.h>

void general_lexer_skip_whitespaces(struct file *file)
{
	const struct file_buffer *cache = file->buffer;

	while (isspace(cache->char_) && cache->char_ != EOF) file_advance(file);
}

void general_lexer_push_and_advance(struct file *file, struct string *string)
{
	string_push(string, file->buffer->char_);

	file_advance(file);
}

enum general_token_type general_lexer_word(struct file *file, struct general_lexical_table *table)
{
	struct string *word = string_alloc();

	while (isalnum(file->buffer->char_) || file->buffer->char_ == '_') {

		general_lexer_push_and_advance(file, word);
	}

	frequency_push(table->word, &word);

	return TOKEN_TYPE_WORD;
}

void general_lexer_read_int(struct file *file, struct string *number)
{
	while (isdigit(file->buffer->char_)) general_lexer_push_and_advance(file, number);
}

enum general_token_type general_lexer_read_number(struct file *file, struct string *number)
{
	enum general_token_type type = TOKEN_TYPE_LIT_INTEGER;

	general_lexer_read_int(file, number);
	if (file->buffer->char_ == '.') {
		general_lexer_push_and_advance(file, number);
		general_lexer_read_int(file, number);
		type = TOKEN_TYPE_LIT_DECIMAL;
	}

	if (type == TOKEN_TYPE_LIT_DECIMAL && string_get(number, number->length - 1) == '.') {
		file_print_error(file, "Floating numbers expect at least one digit after '.'", number->length);
		type = TOKEN_TYPE_UNKNOWN;
	}

	return type;
}

enum general_token_type general_lexer_number(struct file *file, struct general_lexical_table *table)
{
	struct string *number = string_alloc();
	enum general_token_type token = TOKEN_TYPE_LIT_INTEGER;

	general_lexer_read_int(file, number);

	if (file->buffer->char_ == '.') {

		general_lexer_push_and_advance(file, number);
		general_lexer_read_int(file, number);

		token = TOKEN_TYPE_LIT_DECIMAL;
	}

	frequency_push(table->number, &number);

	return token;
}

enum general_token_type general_read_ascii(struct file *file)
{
	switch (file->buffer->char_) {
		case '!':	return TOKEN_TYPE_CHAR_EXCLAMATION;	break;
		case '\"':	return TOKEN_TYPE_CHAR_DOUBLE_QUOTE;	break;
		case '#':	return TOKEN_TYPE_CHAR_HASH;		break;
		case '$':	return TOKEN_TYPE_CHAR_DOLLAR_SIGN;	break;
		case '%':	return TOKEN_TYPE_CHAR_PERCENT;		break;
		case '&':	return TOKEN_TYPE_CHAR_AMPERSAND;	break;
		case '\'':	return TOKEN_TYPE_CHAR_SINGLE_QUOTE;	break;
		case '(':	return TOKEN_TYPE_CHAR_LPARENTHESIS;	break;
		case ')':	return TOKEN_TYPE_CHAR_RPARENTHESIS;	break;
		case '*':	return TOKEN_TYPE_CHAR_STAR;		break;
		case '+':	return TOKEN_TYPE_CHAR_PLUS;		break;
		case ',':	return TOKEN_TYPE_CHAR_COMMA;		break;
		case '-':	return TOKEN_TYPE_CHAR_MINUS;		break;
		case '.':	return TOKEN_TYPE_CHAR_PERIOD;		break;
		case '/':	return TOKEN_TYPE_CHAR_SLASH;		break;
		case ':':	return TOKEN_TYPE_CHAR_COLON;		break;
		case ';':	return TOKEN_TYPE_CHAR_SEMICOLON;	break;
		case '<':	return TOKEN_TYPE_CHAR_LESS_THAN;	break;
		case '=':	return TOKEN_TYPE_CHAR_EQUALS;		break;
		case '>':	return TOKEN_TYPE_CHAR_GREATER_THAN;	break;
		case '?':	return TOKEN_TYPE_CHAR_QUESTION;	break;
		case '@':	return TOKEN_TYPE_CHAR_AT;		break;
		case '[':	return TOKEN_TYPE_CHAR_LBRACKET;	break;
		case '\\':	return TOKEN_TYPE_CHAR_REVERSE_SLASH;	break;
		case ']':	return TOKEN_TYPE_CHAR_RBRACKET;	break;
		case '^':	return TOKEN_TYPE_CHAR_CARET;		break;
		case '`':	return TOKEN_TYPE_CHAR_BACKTICK;	break;
		case '{':	return TOKEN_TYPE_CHAR_RBRACE;		break;
		case '|':	return TOKEN_TYPE_CHAR_VBAR;		break;
		case '}':	return TOKEN_TYPE_CHAR_RBRACE;		break;
		case '~':	return TOKEN_TYPE_CHAR_TILDE;		break;
		default:
			file_print_error(file, "Can't identify a character", 1);
			return TOKEN_TYPE_UNKNOWN;
		break;
	};
}

enum general_token_type general_lexer_characters(struct file *file, struct general_lexical_table *table)
{
	struct string *symbol = string_alloc();
	enum general_token_type token = TOKEN_TYPE_CHAR_UTF8;

	unsigned char b = file->buffer->char_;
	int utf8_len = 1;
	if      ((b & 0x80) == 0x00) utf8_len = 1;
	else if ((b & 0xE0) == 0xC0) utf8_len = 2;
	else if ((b & 0xF0) == 0xE0) utf8_len = 3;
	else if ((b & 0xF8) == 0xF0) utf8_len = 4;

	if (utf8_len == 1) token = general_read_ascii(file);

	for (int i = 0; i < utf8_len; i++) general_lexer_push_and_advance(file, symbol);

	if (token == TOKEN_TYPE_UNKNOWN) frequency_push(table->unknown, &symbol);
	else frequency_push(table->character, &symbol);

	return token;
}

enum general_token_type general_lexer_next(struct file *file, struct general_lexical_table *table)
{
	general_lexer_skip_whitespaces(file);

	if	(file->buffer->char_ == EOF) return TOKEN_TYPE_EOF;
	else if (isalpha(file->buffer->char_) || file->buffer->char_ == '_') return general_lexer_word(file, table);
	else if (isdigit(file->buffer->char_)) return general_lexer_number(file, table);
	else return general_lexer_characters(file, table);
}

