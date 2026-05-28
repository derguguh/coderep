#include "backend/lua/lexer.h"

#include "backend/lua/lexical_table.h"
#include "backend/lua/token.h"

#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/file.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

void lua_lexer_skip_comments(struct file *file)
{
	const struct file_buffer *cache = file->buffer;

	if (cache->char_ == '[' || file_buffer_peek_next(cache) == '[') {
		while (cache->char_ != EOF && !(cache->char_ == ']' && file_buffer_peek_next(cache) == ']')) {
			file_advance(file);
		}
	} else while (cache->char_ != '\n' && cache->char_ != EOF) file_advance(file);
}

void lua_lexer_skip_whitespaces(struct file *file)
{
	const struct file_buffer *cache = file->buffer;

	while (isspace(cache->char_) && cache->char_ != EOF) file_advance(file);
}

void lua_lexer_skip_blank(struct file *file)
{
	lua_lexer_skip_whitespaces(file);

	if (file->buffer->char_ == '-' && file_buffer_peek_next(file->buffer) == '-') {

		file_advance(file);
		file_advance(file);
		lua_lexer_skip_comments(file);

		lua_lexer_skip_blank(file);
	}
}

enum lua_token_type lua_lexer_identifier_word(struct string *word)
{
	const char *const cstr = string_to_cstr(word);

	if	(strcmp(cstr, "and") == 0)		return TOKEN_TYPE_KW_AND;
	else if	(strcmp(cstr, "break") == 0)		return TOKEN_TYPE_KW_BREAK;
	else if	(strcmp(cstr, "do") == 0)		return TOKEN_TYPE_KW_DO;
	else if	(strcmp(cstr, "else") == 0)		return TOKEN_TYPE_KW_ELSE;
	else if	(strcmp(cstr, "elseif") == 0)		return TOKEN_TYPE_KW_ELSEIF;
	else if	(strcmp(cstr, "end") == 0)		return TOKEN_TYPE_KW_END;
	else if	(strcmp(cstr, "false") == 0)		return TOKEN_TYPE_KW_FALSE;
	else if	(strcmp(cstr, "for") == 0)		return TOKEN_TYPE_KW_FOR;
	else if	(strcmp(cstr, "function") == 0)		return TOKEN_TYPE_KW_FUNCTION;
	else if	(strcmp(cstr, "goto") == 0)		return TOKEN_TYPE_KW_GOTO;
	else if	(strcmp(cstr, "if") == 0)		return TOKEN_TYPE_KW_IF;
	else if	(strcmp(cstr, "in") == 0)		return TOKEN_TYPE_KW_IN;
	else if	(strcmp(cstr, "local") == 0)		return TOKEN_TYPE_KW_LOCAL;
	else if	(strcmp(cstr, "nil") == 0)		return TOKEN_TYPE_KW_NIL;
	else if	(strcmp(cstr, "not") == 0)		return TOKEN_TYPE_KW_NOT;
	else if	(strcmp(cstr, "or") == 0)		return TOKEN_TYPE_KW_OR;
	else if	(strcmp(cstr, "repeat") == 0)		return TOKEN_TYPE_KW_REPEAT;
	else if	(strcmp(cstr, "return") == 0)		return TOKEN_TYPE_KW_RETURN;
	else if	(strcmp(cstr, "then") == 0)		return TOKEN_TYPE_KW_THEN;
	else if	(strcmp(cstr, "true") == 0)		return TOKEN_TYPE_KW_TRUE;
	else if	(strcmp(cstr, "until") == 0)		return TOKEN_TYPE_KW_UNTIL;
	else if	(strcmp(cstr, "while") == 0)		return TOKEN_TYPE_KW_WHILE;
	else						return TOKEN_TYPE_IDENTIFIER;
}

void lua_lexer_push_and_advance(struct file *file, struct string *string)
{
	string_push(string, file->buffer->char_);

	file_advance(file);
}

struct lua_token lua_lexer_word(struct file *file, struct lua_lexical_table *table)
{
	struct string *word = string_alloc();
	struct lua_token token = { 0 };

	while (isalnum(file->buffer->char_) || file->buffer->char_ == '_') {

		lua_lexer_push_and_advance(file, word);
	}

	token.type = lua_lexer_identifier_word(word);

	if (token.type == TOKEN_TYPE_IDENTIFIER) frequency_push(table->identifier, &word);
	else frequency_push(table->keyword, &word);

	return token;
}

void lua_lexer_read_int(struct file *file, struct string *number)
{
	while (isdigit(file->buffer->char_)) lua_lexer_push_and_advance(file, number);
}

enum lua_token_type lua_lexer_number_hex(struct file *file, struct string *number)
{
	do {
		lua_lexer_push_and_advance(file, number);
	} while (isxdigit(file->buffer->char_));

	if (!isxdigit(string_get(number, number->length - 1))) {
		file_print_error(file,
				"Hexadecimal numbers need to have at least one number after the prefix '0x'/'0X'",
				number->length);
		return TOKEN_TYPE_INVALID;
	} else return TOKEN_TYPE_LIT_HEX;
}

enum lua_token_type lua_lexer_read_scientific_suffix(struct file *file, struct string *number)
{
	lua_lexer_push_and_advance(file, number);
	if (file->buffer->char_ == '+' || file->buffer->char_ == '-') lua_lexer_push_and_advance(file, number);
	lua_lexer_read_int(file, number);

	if (!isdigit(string_get(number, number->length - 1))) {
		file_print_error(file,
				"Scientific numbers need to have at least one number after the prefix 'e', 'e+' or 'e-'",
				number->length);
		return TOKEN_TYPE_INVALID;
	} else return TOKEN_TYPE_LIT_SCIENTIFIC;
}

enum lua_token_type lua_lexer_read_number(struct file *file, struct string *number)
{
	enum lua_token_type type = TOKEN_TYPE_LIT_INTEGER;

	lua_lexer_read_int(file, number);
	if (file->buffer->char_ == '.') {
		lua_lexer_push_and_advance(file, number);
		lua_lexer_read_int(file, number);
		type = TOKEN_TYPE_LIT_DECIMAL;
	}

	if (type == TOKEN_TYPE_LIT_DECIMAL && string_get(number, number->length - 1) == '.') {
		file_print_error(file, "Floating numbers expect at least one digit after '.'", number->length);
		type = TOKEN_TYPE_INVALID;
	}

	return type;
}

struct lua_token lua_lexer_number(struct file *file, struct lua_lexical_table *table)
{
	struct string *number = string_alloc();
	struct lua_token token = { .type=TOKEN_TYPE_LIT_INTEGER };

	if (file->buffer->char_ == '0' && tolower(file_buffer_peek_next(file->buffer)) == 'x') {
		lua_lexer_push_and_advance(file, number);

		token.type = lua_lexer_number_hex(file, number);
	} else token.type = lua_lexer_read_number(file, number);

	if (token.type != TOKEN_TYPE_INVALID && file->buffer->char_ == 'e') token.type = lua_lexer_read_scientific_suffix(file, number);

	if (token.type == TOKEN_TYPE_INVALID) {
		frequency_push(table->invalid, &number);

		struct string *tmp = string_clone(table->template_invalid_number);
		frequency_push(table->what_invalid, &tmp);
	} else frequency_push(table->number, &number);

	return token;
}

struct lua_token lua_lexer_string(struct file *file, struct lua_lexical_table *table, bool double_quotes)
{
	struct string *string = string_alloc();
	struct lua_token token = { .type=TOKEN_TYPE_STRING };
	bool continue_ = false;
	
	do {
		if (file->buffer->char_ == EOF || file->buffer->char_ == '\n') {

			file_print_error(file, "Normal strings needs to be single lined, "
					"and end with the same quotes (single or double) as it started",
					string->length);
			token.type = TOKEN_TYPE_INVALID;
			break;
		}
		if (double_quotes) {
			if (file->buffer->char_ == '"') continue_ = !continue_;
		} else if (file->buffer->char_ == '\'') continue_ = !continue_;

		if (file->buffer->char_ == '\\') {
			char peek = file_buffer_peek_next(file->buffer);
			if (peek == '\'' || peek == '"' || peek == '\\') lua_lexer_push_and_advance(file, string);
		}

		lua_lexer_push_and_advance(file, string);
	} while (continue_);

	if (token.type == TOKEN_TYPE_INVALID) {
		frequency_push(table->invalid, &string);

		struct string *tmp = string_clone(table->template_invalid_string);
		frequency_push(table->what_invalid, &tmp);
	} else frequency_push(table->string, &string);

	return token;
}

struct lua_token lua_lexer_multi_line_string(struct file *file, struct lua_lexical_table *table)
{
	struct string *string = string_alloc();
	struct lua_token token = { .type=TOKEN_TYPE_MULTI_LINE_STRING };
	
	while (!(file->buffer->char_ == ']' && file_buffer_peek_next(file->buffer) == ']')) {
		if (file->buffer->char_ == EOF) {
			file_print_error(file, "Multi lined strings need a clossing ']], "
					"but founded EOF (End Of File)",
					string->length + 1);
			token.type = TOKEN_TYPE_INVALID;
			break;
		} else lua_lexer_push_and_advance(file, string);
	}

	lua_lexer_push_and_advance(file, string);
	lua_lexer_push_and_advance(file, string);

	if (token.type == TOKEN_TYPE_INVALID) {
		frequency_push(table->invalid, &string);

		struct string *tmp = string_clone(table->template_invalid_string);
		frequency_push(table->what_invalid, &tmp);
	} else frequency_push(table->string, &string);

	return token;
}

struct lua_token lua_lexer_characters(struct file *file, struct lua_lexical_table *table)
{
	struct lua_token token = { .type=TOKEN_TYPE_INVALID };
	struct string *symbol = string_alloc();
	char peek = file_buffer_peek_next(file->buffer);

	switch (file->buffer->char_) {
		case '+':	token.type = TOKEN_TYPE_CHAR_PLUS;		break;
		case '-':	token.type = TOKEN_TYPE_CHAR_MINUS;		break;
		case '*':	token.type = TOKEN_TYPE_CHAR_MULT;		break;
		case '%':	token.type = TOKEN_TYPE_CHAR_MODULE;		break;
		case '^':	token.type = TOKEN_TYPE_CHAR_CARET;		break;
		case '#':	token.type = TOKEN_TYPE_CHAR_HASH;		break;
		case '&':	token.type = TOKEN_TYPE_CHAR_AMPERSAND;		break;
		case ',':	token.type = TOKEN_TYPE_CHAR_COMMA;		break;
		case ';':	token.type = TOKEN_TYPE_CHAR_SEMICOLON;		break;
		case '|':	token.type = TOKEN_TYPE_CHAR_VBAR;		break;
		case '(':	token.type = TOKEN_TYPE_CHAR_LPARENTHESIS;	break;
		case ')':	token.type = TOKEN_TYPE_CHAR_RPARENTHESIS;	break;
		case '[':	token.type = TOKEN_TYPE_CHAR_LBRACKET;		break;
		case ']':	token.type = TOKEN_TYPE_CHAR_RBRACKET;		break;
		case '{':	token.type = TOKEN_TYPE_CHAR_LBRACE;		break;
		case '}':	token.type = TOKEN_TYPE_CHAR_RBRACE;		break;
		case '<':
			if (peek == '<') {
				token.type = TOKEN_TYPE_CHAR_SHIFT_LEFT;
				lua_lexer_push_and_advance(file, symbol);
			} else if (peek == '=') {
				token.type = TOKEN_TYPE_CHAR_LESS_OR_EQUALS_THAN;
				lua_lexer_push_and_advance(file, symbol);
			} else token.type = TOKEN_TYPE_CHAR_LESS_THAN;
		break;
		case '>':
			if (peek == '>') {
				token.type = TOKEN_TYPE_CHAR_SHIFT_RIGHT;
				lua_lexer_push_and_advance(file, symbol);
			} else if (peek == '=') {
				token.type = TOKEN_TYPE_CHAR_GREATER_OR_EQUALS_THAN;
				lua_lexer_push_and_advance(file, symbol);
			} else token.type = TOKEN_TYPE_CHAR_GREATER_THAN;
		break;
		case ':':
			if (peek == ':') {
				token.type = TOKEN_TYPE_CHAR_DOUBLE_COLON;
				lua_lexer_push_and_advance(file, symbol);
			}
			else token.type = TOKEN_TYPE_CHAR_COLON;
		break;
		case '/':
			if (peek == '/') {
				token.type = TOKEN_TYPE_CHAR_DOUBLE_SLASH;
				lua_lexer_push_and_advance(file, symbol);
			}
			else token.type = TOKEN_TYPE_CHAR_SLASH;
		break;
		case '=':
			if (peek == '=') {
				token.type = TOKEN_TYPE_CHAR_EQUALS;
				lua_lexer_push_and_advance(file, symbol);
			}
			else token.type = TOKEN_TYPE_CHAR_ASSIGN;
		break;
		case '~':
			if (peek == '=') {
				token.type = TOKEN_TYPE_CHAR_NOT_EQUALS;
				lua_lexer_push_and_advance(file, symbol);
			}
			else token.type = TOKEN_TYPE_CHAR_TILDE;
		break;
		case '.':
			if	(peek == '.') {
				lua_lexer_push_and_advance(file, symbol);
				peek = file_buffer_peek_next(file->buffer);

				if (peek == '.') {
					lua_lexer_push_and_advance(file, symbol);
					token.type = TOKEN_TYPE_CHAR_EPSILON;
				} else token.type = TOKEN_TYPE_CHAR_CONCAT;
			} else token.type = TOKEN_TYPE_CHAR_PERIOD;
		break;
		default:
			file_print_error(file, "Can't identify a character", 1);
			token.type = TOKEN_TYPE_INVALID;
		break;
	};

	lua_lexer_push_and_advance(file, symbol);

	if (token.type == TOKEN_TYPE_INVALID) {
		frequency_push(table->invalid, &symbol);

		struct string *tmp = string_clone(table->template_invalid_char);
		frequency_push(table->what_invalid, &tmp);
	} else frequency_push(table->character, &symbol);

	return token;
}

struct lua_token lua_lexer_next(struct file *file, struct lua_lexical_table *table)
{
	lua_lexer_skip_blank(file);

	if	(file->buffer->char_ == EOF) return (struct lua_token) { .type=TOKEN_TYPE_EOF, .id=0 };
	else if (file->buffer->char_ == '[' && file_buffer_peek_next(file->buffer) == '[') return lua_lexer_multi_line_string(file, table);
	else if (isalpha(file->buffer->char_) || file->buffer->char_ == '_') return lua_lexer_word(file, table);
	else if (isdigit(file->buffer->char_))	return lua_lexer_number(file, table);
	else if (file->buffer->char_ == '\'')	return lua_lexer_string(file, table, false);
	else if (file->buffer->char_ == '"')	return lua_lexer_string(file, table, true);
	else					return lua_lexer_characters(file, table);
}

