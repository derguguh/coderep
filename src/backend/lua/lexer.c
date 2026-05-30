#include "backend/lua/lexer.h"

#include "backend/general/lexer.h"
#include "backend/lua/token.h"

#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/file.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

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

enum lua_token_type lua_identifier_word(struct string *scratch_buffer)
{
	const char *const cstr = string_to_cstr(scratch_buffer);

	if	(strcmp(cstr, "and") == 0)		return LUA_TOKEN_TYPE_KW_AND;
	else if	(strcmp(cstr, "break") == 0)		return LUA_TOKEN_TYPE_KW_BREAK;
	else if	(strcmp(cstr, "do") == 0)		return LUA_TOKEN_TYPE_KW_DO;
	else if	(strcmp(cstr, "else") == 0)		return LUA_TOKEN_TYPE_KW_ELSE;
	else if	(strcmp(cstr, "elseif") == 0)		return LUA_TOKEN_TYPE_KW_ELSEIF;
	else if	(strcmp(cstr, "end") == 0)		return LUA_TOKEN_TYPE_KW_END;
	else if	(strcmp(cstr, "false") == 0)		return LUA_TOKEN_TYPE_KW_FALSE;
	else if	(strcmp(cstr, "for") == 0)		return LUA_TOKEN_TYPE_KW_FOR;
	else if	(strcmp(cstr, "function") == 0)		return LUA_TOKEN_TYPE_KW_FUNCTION;
	else if	(strcmp(cstr, "goto") == 0)		return LUA_TOKEN_TYPE_KW_GOTO;
	else if	(strcmp(cstr, "if") == 0)		return LUA_TOKEN_TYPE_KW_IF;
	else if	(strcmp(cstr, "in") == 0)		return LUA_TOKEN_TYPE_KW_IN;
	else if	(strcmp(cstr, "local") == 0)		return LUA_TOKEN_TYPE_KW_LOCAL;
	else if	(strcmp(cstr, "nil") == 0)		return LUA_TOKEN_TYPE_KW_NIL;
	else if	(strcmp(cstr, "not") == 0)		return LUA_TOKEN_TYPE_KW_NOT;
	else if	(strcmp(cstr, "or") == 0)		return LUA_TOKEN_TYPE_KW_OR;
	else if	(strcmp(cstr, "repeat") == 0)		return LUA_TOKEN_TYPE_KW_REPEAT;
	else if	(strcmp(cstr, "return") == 0)		return LUA_TOKEN_TYPE_KW_RETURN;
	else if	(strcmp(cstr, "then") == 0)		return LUA_TOKEN_TYPE_KW_THEN;
	else if	(strcmp(cstr, "true") == 0)		return LUA_TOKEN_TYPE_KW_TRUE;
	else if	(strcmp(cstr, "until") == 0)		return LUA_TOKEN_TYPE_KW_UNTIL;
	else if	(strcmp(cstr, "while") == 0)		return LUA_TOKEN_TYPE_KW_WHILE;
	else						return LUA_TOKEN_TYPE_IDENTIFIER;
}

struct lua_token lua_lexer_word(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
{
	struct lua_token token = { 0 };

	while (isalnum(file->buffer->char_) || file->buffer->char_ == '_') {

		push_and_advance(file, scratch_buffer);
	}

	token.type = lua_identifier_word(scratch_buffer);

	if (token.type == LUA_TOKEN_TYPE_IDENTIFIER) frequency_intern(self->identifier, scratch_buffer);
	else frequency_intern(self->keyword, scratch_buffer);

	return token;
}

struct lua_token lua_block_string(struct file *file, struct string *scratch_buffer, size_t n)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_MULTI_LINE_STRING };
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
	else frequency_intern(self->string, scratch_buffer);

	return token;
}

enum lua_token_type lua_read_number(struct file *file, struct string *scratch_buffer)
{
	enum lua_token_type type = LUA_TOKEN_TYPE_LIT_INTEGER;

	read_int(file, scratch_buffer);
	if (file->buffer->char_ == '.') {
		push_and_advance(file, scratch_buffer);
		read_int(file, scratch_buffer);
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

		token.type = read_hex(file, scratch_buffer, true) ? LUA_TOKEN_TYPE_LIT_HEX : LUA_TOKEN_TYPE_INVALID;
	} else token.type = lua_read_number(file, scratch_buffer);

	if (token.type != LUA_TOKEN_TYPE_INVALID && file->buffer->char_ == 'e') {
		token.type = read_scientific_suffix(file, scratch_buffer, true) ? LUA_TOKEN_TYPE_LIT_SCIENTIFIC : LUA_TOKEN_TYPE_INVALID;
	}

	if (token.type == LUA_TOKEN_TYPE_INVALID) {
		frequency_intern(self->invalid, scratch_buffer);
		frequency_intern(self->what_invalid, self->template_invalid_number);
	} else frequency_intern(self->number, scratch_buffer);

	return token;
}

struct lua_token lua_lexer_string(struct lua_lexer *self, struct file *file, struct string *scratch_buffer, bool double_quotes)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_STRING };
	bool continue_ = false;
	
	do {
		if (file->buffer->char_ == EOF || file->buffer->char_ == '\n') {

			file_print_error(file, "Normal strings needs to be single lined, "
					"and end with the same quotes (single or double) as it started",
					scratch_buffer->length);
			token.type = LUA_TOKEN_TYPE_INVALID;
			break;
		}
		if (double_quotes) {
			if (file->buffer->char_ == '"') continue_ = !continue_;
		} else if (file->buffer->char_ == '\'') continue_ = !continue_;

		if (file->buffer->char_ == '\\') {
			char peek = file_buffer_peek_n(file->buffer, 1);
			if (peek == '\n' || peek == '\'' || peek == '"' || peek == '\\') push_and_advance(file, scratch_buffer);
			else if (peek == 'z') {
				push_and_advance(file, scratch_buffer);
				push_and_advance(file, scratch_buffer);
			}
		}

		push_and_advance(file, scratch_buffer);
	} while (continue_);

	if (token.type == LUA_TOKEN_TYPE_INVALID) {
		frequency_intern(self->invalid, scratch_buffer);
		frequency_intern(self->what_invalid, self->template_invalid_string);
	} else frequency_intern(self->string, scratch_buffer);

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

