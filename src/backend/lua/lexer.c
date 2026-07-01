#include "backend/lua/lexer.h"

#include "backend/general/lexer.h"
#include "backend/lua/token.h"

#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"
#include "coderep/macros.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/file.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

const struct word_table LUA_KEYWORD_TABLE[] = {
	{ "and", 	LUA_TOKEN_TYPE_KW_AND },
	{ "break", 	LUA_TOKEN_TYPE_KW_BREAK },
	{ "do", 	LUA_TOKEN_TYPE_KW_DO },
	{ "else", 	LUA_TOKEN_TYPE_KW_ELSE },
	{ "elseif", 	LUA_TOKEN_TYPE_KW_ELSEIF },
	{ "end", 	LUA_TOKEN_TYPE_KW_END },
	{ "false", 	LUA_TOKEN_TYPE_KW_FALSE },
	{ "for", 	LUA_TOKEN_TYPE_KW_FOR },
	{ "function", 	LUA_TOKEN_TYPE_KW_FUNCTION },
	{ "goto", 	LUA_TOKEN_TYPE_KW_GOTO },
	{ "if", 	LUA_TOKEN_TYPE_KW_IF },
	{ "in", 	LUA_TOKEN_TYPE_KW_IN },
	{ "local", 	LUA_TOKEN_TYPE_KW_LOCAL },
	{ "nil", 	LUA_TOKEN_TYPE_KW_NIL },
	{ "not", 	LUA_TOKEN_TYPE_KW_NOT },
	{ "or", 	LUA_TOKEN_TYPE_KW_OR },
	{ "repeat", 	LUA_TOKEN_TYPE_KW_REPEAT },
	{ "return", 	LUA_TOKEN_TYPE_KW_RETURN },
	{ "then", 	LUA_TOKEN_TYPE_KW_THEN },
	{ "true", 	LUA_TOKEN_TYPE_KW_TRUE },
	{ "until", 	LUA_TOKEN_TYPE_KW_UNTIL },
	{ "while", 	LUA_TOKEN_TYPE_KW_WHILE },
};

const size_t LUA_KEYWORD_TABLE_L = ARRAY_SIZE(LUA_KEYWORD_TABLE, struct word_table);

bool lua_skip_block_comment(struct file *file)
{
	file_advance(file);
	size_t n = 0;

	while (file->buffer->char_ == '=') {
		file_advance(file);
		n++;
	}

	if (file->buffer->char_ != '[') return false;

	while (true) {
		if (file->buffer->char_ == ']') {
			file_advance(file);

			size_t n_ = 1;
			while (file->buffer->char_ == '=') {
				file_advance(file);
				n_++;
			}

			if (file->buffer->char_ == ']' && n_ == n) return true;
		} else if (file->buffer->char_ == EOF) return true;
		else file_advance(file);
	}
}

void lua_skip_comments(struct file *file)
{
	const struct file_buffer *cache = file->buffer;
	bool was_a_block_comment = false;

	while (isblank(cache->char_) && cache->char_ != EOF) file_advance(file);

	if (cache->char_ == '[') was_a_block_comment = lua_skip_block_comment(file);

	if (!was_a_block_comment) while (cache->char_ != '\n' && cache->char_ != EOF) file_advance(file);
}

void lua_skip_blank(struct file *file)
{
	skip_whitespaces(file);

	if (file->buffer->char_ == '-' && file_buffer_peek_n(file->buffer, 1) == '-') {

		file_advance(file);
		file_advance(file);
		lua_skip_comments(file);

		lua_skip_blank(file);
	}
}

struct lua_token lua_lexer_word(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
{
	struct lua_token token = { 0 };

	while (isalnum(file->buffer->char_) || file->buffer->char_ == '_') {

		push_and_advance(file, scratch_buffer);
	}

	token.type = word_tree_categorize(self->keyword_tree, scratch_buffer);

	if (token.type == -1) frequency_intern(self->identifier, scratch_buffer);
	else frequency_intern(self->keyword, scratch_buffer);

	return token;
}

struct lua_token lua_block_string(struct file *file, struct string *scratch_buffer, size_t n)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_LIT_MULTI_LINE_STRING };
	bool continue_ = true;

	while (continue_) {
		if (file->buffer->char_ == EOF) {
			file_print_error(file, "Multi lined strings need a clossing ']], "
					"but founded EOF (End Of File)",
					file->column);
			token.type = LUA_TOKEN_TYPE_INVALID;
			continue_ = false;
		} else if (file->buffer->char_ == ']' && file_buffer_peek_n(file->buffer, n) == ']') {
			push_and_advance(file, scratch_buffer);

			size_t n_ = 1;
			while (file->buffer->char_ == '=') {
				push_and_advance(file, scratch_buffer);
				n_++;
			}

			if (file->buffer->char_ == ']' && n_ == n) continue_ = false;
		} else push_and_advance(file, scratch_buffer);
	}

	push_and_advance(file, scratch_buffer);

	return token;
}

struct lua_token lua_lexer_handle_lbracket(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_CHAR_LBRACKET };

	push_and_advance(file, scratch_buffer);

	size_t n = 1;
	while (file->buffer->char_ == '=') {

		push_and_advance(file, scratch_buffer);
		n++;
	}

	if (file->buffer->char_ == '[') token = lua_block_string(file, scratch_buffer, n);

	if (token.type == LUA_TOKEN_TYPE_INVALID) {
		frequency_intern(self->invalid, scratch_buffer);
		frequency_intern(self->what_invalid, self->template_invalid_string);
	} else if (token.type == LUA_TOKEN_TYPE_CHAR_LBRACKET) frequency_intern(self->character, scratch_buffer);
	else frequency_intern(self->block_string, scratch_buffer);

	return token;
}

enum lua_token_type lua_read_number(struct file *file, struct string *scratch_buffer)
{
	enum lua_token_type type = LUA_TOKEN_TYPE_LIT_INTEGER;

	read_int(file, scratch_buffer, false);
	if (file->buffer->char_ == '.') {
		push_and_advance(file, scratch_buffer);
		read_int(file, scratch_buffer, false);
		type = LUA_TOKEN_TYPE_LIT_DECIMAL;
	}

	if (type == LUA_TOKEN_TYPE_LIT_DECIMAL && string_get(scratch_buffer, scratch_buffer->length - 1) == '.') {
		file_print_error(file, "Floating numbers expect at least one digit after '.'", scratch_buffer->length);
		type = LUA_TOKEN_TYPE_INVALID;
	}

	return type;
}

struct lua_token lua_lexer_number(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_LIT_INTEGER };

	if (file->buffer->char_ == '0' && tolower(file_buffer_peek_n(file->buffer, 1)) == 'x') {
		push_and_advance(file, scratch_buffer);

		token.type = read_hex(file, scratch_buffer, false, true) != -1 ? LUA_TOKEN_TYPE_LIT_HEX : LUA_TOKEN_TYPE_INVALID;
	} else token.type = lua_read_number(file, scratch_buffer);

	if (token.type != LUA_TOKEN_TYPE_INVALID && file->buffer->char_ == 'e') {
		token.type = read_scientific_suffix(file, scratch_buffer, false,  true) != -1 ? LUA_TOKEN_TYPE_LIT_SCIENTIFIC : LUA_TOKEN_TYPE_INVALID;
	}

	if (token.type == LUA_TOKEN_TYPE_INVALID) {
		frequency_intern(self->invalid, scratch_buffer);
		frequency_intern(self->what_invalid, self->template_invalid_number);
	} else frequency_intern(self->number, scratch_buffer);

	return token;
}

struct lua_token lua_lexer_string(struct lua_lexer *self, struct file *file, struct string *scratch_buffer, bool double_quotes)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_LIT_STRING };
	push_and_advance(file, scratch_buffer);

	while (!(double_quotes && file->buffer->char_ == '"') && !(!double_quotes && file->buffer->char_ == '\'')) {
		if (file->buffer->char_ == EOF || file->buffer->char_ == '\n') {
			file_print_error(file, "Normal strings needs to be single lined, "
					"and end with the same quotes (single or double) as it started",
					scratch_buffer->length);
			token.type = LUA_TOKEN_TYPE_INVALID;
			break;
		} else if (file->buffer->char_ == '\\') {
			char peek = file_buffer_peek_n(file->buffer, 1);
			if (peek == '\n' || peek == '\'' || peek == '"' || peek == '\\') push_and_advance(file, scratch_buffer);
			else if (peek == 'z') {
				push_and_advance(file, scratch_buffer);
				push_and_advance(file, scratch_buffer);
			}
		}

		push_and_advance(file, scratch_buffer);
	}

	if (token.type == LUA_TOKEN_TYPE_INVALID) {
		frequency_intern(self->invalid, scratch_buffer);
		frequency_intern(self->what_invalid, self->template_invalid_string);
	} else {
		push_and_advance(file, scratch_buffer);
		frequency_intern(self->string, scratch_buffer);
	}
	printf("\n");

	return token;
}

struct lua_token lua_lexer_characters(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_INVALID };
	char peek = file_buffer_peek_n(file->buffer, 1);

	switch (file->buffer->char_) {
		case '+':	token.type = LUA_TOKEN_TYPE_CHAR_PLUS;		break;
		case '-':	token.type = LUA_TOKEN_TYPE_CHAR_MINUS;		break;
		case '*':	token.type = LUA_TOKEN_TYPE_CHAR_MULT;		break;
		case '%':	token.type = LUA_TOKEN_TYPE_CHAR_MODULE;	break;
		case '^':	token.type = LUA_TOKEN_TYPE_CHAR_CARET;		break;
		case '#':	token.type = LUA_TOKEN_TYPE_CHAR_HASH;		break;
		case '&':	token.type = LUA_TOKEN_TYPE_CHAR_AMPERSAND;	break;
		case ',':	token.type = LUA_TOKEN_TYPE_CHAR_COMMA;		break;
		case ';':	token.type = LUA_TOKEN_TYPE_CHAR_SEMICOLON;	break;
		case '|':	token.type = LUA_TOKEN_TYPE_CHAR_VBAR;		break;
		case '(':	token.type = LUA_TOKEN_TYPE_CHAR_LPARENTHESIS;	break;
		case ')':	token.type = LUA_TOKEN_TYPE_CHAR_RPARENTHESIS;	break;
		case '[':	token.type = LUA_TOKEN_TYPE_CHAR_LBRACKET;	break;
		case ']':	token.type = LUA_TOKEN_TYPE_CHAR_RBRACKET;	break;
		case '{':	token.type = LUA_TOKEN_TYPE_CHAR_LBRACE;	break;
		case '}':	token.type = LUA_TOKEN_TYPE_CHAR_RBRACE;	break;
		case '<':
			if (peek == '<') {
				token.type = LUA_TOKEN_TYPE_CHAR_SHIFT_LEFT;
				push_and_advance(file, scratch_buffer);
			} else if (peek == '=') {
				token.type = LUA_TOKEN_TYPE_CHAR_LESS_OR_EQUALS_THAN;
				push_and_advance(file, scratch_buffer);
			} else token.type = LUA_TOKEN_TYPE_CHAR_LESS_THAN;
		break;
		case '>':
			if (peek == '>') {
				token.type = LUA_TOKEN_TYPE_CHAR_SHIFT_RIGHT;
				push_and_advance(file, scratch_buffer);
			} else if (peek == '=') {
				token.type = LUA_TOKEN_TYPE_CHAR_GREATER_OR_EQUALS_THAN;
				push_and_advance(file, scratch_buffer);
			} else token.type = LUA_TOKEN_TYPE_CHAR_GREATER_THAN;
		break;
		case ':':
			if (peek == ':') {
				token.type = LUA_TOKEN_TYPE_CHAR_DOUBLE_COLON;
				push_and_advance(file, scratch_buffer);
			}
			else token.type = LUA_TOKEN_TYPE_CHAR_COLON;
		break;
		case '/':
			if (peek == '/') {
				token.type = LUA_TOKEN_TYPE_CHAR_DOUBLE_SLASH;
				push_and_advance(file, scratch_buffer);
			}
			else token.type = LUA_TOKEN_TYPE_CHAR_SLASH;
		break;
		case '=':
			if (peek == '=') {
				token.type = LUA_TOKEN_TYPE_CHAR_EQUALS;
				push_and_advance(file, scratch_buffer);
			}
			else token.type = LUA_TOKEN_TYPE_CHAR_ASSIGN;
		break;
		case '~':
			if (peek == '=') {
				token.type = LUA_TOKEN_TYPE_CHAR_NOT_EQUALS;
				push_and_advance(file, scratch_buffer);
			}
			else token.type = LUA_TOKEN_TYPE_CHAR_TILDE;
		break;
		case '.':
			
			if	(peek == '.') {
				push_and_advance(file, scratch_buffer);
				peek = file_buffer_peek_n(file->buffer, 1);

				if (peek == '.') {
					push_and_advance(file, scratch_buffer);
					token.type = LUA_TOKEN_TYPE_CHAR_EPSILON;
				} else token.type = LUA_TOKEN_TYPE_CHAR_CONCAT;
			} else token.type = LUA_TOKEN_TYPE_CHAR_PERIOD;
		break;
		default:
			file_print_error(file, "Can't identify a character", 1);
			token.type = LUA_TOKEN_TYPE_INVALID;
		break;
	};

	push_and_advance(file, scratch_buffer);

	if (token.type == LUA_TOKEN_TYPE_INVALID) {
		frequency_intern(self->invalid, scratch_buffer);
		frequency_intern(self->what_invalid, self->template_invalid_string);
	} else frequency_intern(self->character, scratch_buffer);

	return token;
}

struct lua_token lua_lexer_next(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
{
	string_clear(scratch_buffer);
	lua_skip_blank(file);

	if	(file->buffer->char_ == EOF) return (struct lua_token) { .type=LUA_TOKEN_TYPE_EOF, .id=0 };
	else if (file->buffer->char_ == '[') return lua_lexer_handle_lbracket(self, file, scratch_buffer);
	else if (isalpha(file->buffer->char_) || file->buffer->char_ == '_') return lua_lexer_word(self, file, scratch_buffer);
	else if (isdigit(file->buffer->char_))	return lua_lexer_number(self, file, scratch_buffer);
	else if (file->buffer->char_ == '\'')	return lua_lexer_string(self, file, scratch_buffer, false);
	else if (file->buffer->char_ == '"')	return lua_lexer_string(self, file, scratch_buffer, true);
	else					return lua_lexer_characters(self, file, scratch_buffer);
}

