#include "back_ends/lua/lexer.h"

#include "back_ends/lua/token.h"
#include "coderep/data_types/frequency.h"
#include "coderep/data_types/string.h"
#include "coderep/macros.h"
#include "coderep/tools/file_buffer.h"
#include "coderep/tools/file.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lua_lexer *lua_lexer_alloc(struct string *file_path)
{
	struct string *tmp_str = string_from_cstr("Lua Symbol Table");
	struct list *symbol_table = list_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.identifiers");
	struct frequency *identifiers = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.keywords");
	struct frequency *keyword = frequency_alloc(&tmp_str, 20);

	tmp_str = string_from_cstr("lexer.symbols.numbers");
	struct frequency *number = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.strings");
	struct frequency *string = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.multi_line_strings");
	struct frequency *multi_line_string = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.symbols");
	struct frequency *character = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.invalid.table");
	struct frequency *invalid = frequency_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.invalid.what");
	struct frequency *what_invalid = frequency_alloc(&tmp_str, 40);

	struct string *template_invalid_number = string_from_cstr("Invalid: Number");
	struct string *template_invalid_string = string_from_cstr("Invalid: String");
	struct string *template_invalid_char = string_from_cstr("Invalid: Char");

	struct file *file = file_alloc(file_path);

	struct lua_lexer *self = malloc(sizeof(struct lua_lexer));
	if (!self) MALLOC_ERR_EXIT("struct lexer (for lua)");

	*self = (struct lua_lexer) {
		.char_ = file->file_buffer->char_,
		.file = file,

		.symbol_table = symbol_table,
		.identifier = identifiers,
		.keyword = keyword,
		.number = number,
		.string = string,
		.multi_line_string = multi_line_string,
		.character = character,
		.invalid = invalid,
		.what_invalid = what_invalid,

		.template_invalid_number = template_invalid_number,
		.template_invalid_string = template_invalid_string,
		.template_invalid_char = template_invalid_char,
	};

	return self;
}

void lua_lexer_skip_comments(struct lua_lexer *self)
{
	const struct file_buffer *cache = self->file->file_buffer;

	if (file_buffer_peek(cache) == '[' || file_buffer_peek_next(cache) == '[') {
		while (file_buffer_peek(cache) != EOF && !(file_buffer_peek(cache) == ']' && file_buffer_peek_next(cache) == ']')) {
			file_advance(self->file);
		}
	} else while (file_buffer_peek(cache) != '\n' && file_buffer_peek(cache) != EOF) file_advance(self->file);
}

void lua_lexer_skip_whitespaces(struct lua_lexer *self)
{
	const struct file_buffer *cache = self->file->file_buffer;

	while (!isgraph(file_buffer_peek(cache)) && file_buffer_peek(cache) != EOF) file_advance(self->file);
}

void lua_lexer_skip_blank(struct lua_lexer *self)
{
	lua_lexer_skip_whitespaces(self);

	if (file_buffer_peek(self->file->file_buffer) == '-' && file_buffer_peek_next(self->file->file_buffer)) {

		file_advance(self->file);
		file_advance(self->file);
		lua_lexer_skip_comments(self);

		lua_lexer_skip_blank(self);
	}

	self->char_ = file_buffer_peek(self->file->file_buffer);
}

enum token_type lua_lexer_identifier_word(const struct lua_lexer *self, struct string *word)
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
	else if	(strcmp(cstr, "global") == 0)		return TOKEN_TYPE_KW_GLOBAL;
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

void lua_lexer_push_and_advance(struct lua_lexer *self, struct string *string)
{
	string_push(string, self->char_);

	file_advance(self->file);
	self->char_ = file_buffer_peek(self->file->file_buffer);
}

struct token lua_lexer_word(struct lua_lexer *self)
{
	struct string *word = string_alloc();
	struct token token = { 0 };

	while (isalnum(self->char_) || self->char_ == '_') {

		lua_lexer_push_and_advance(self, word);
	}

	token.type = lua_lexer_identifier_word(self, word);
	struct string *tmp = string_clone(word);
	token.id = list_push(self->symbol_table, &tmp);

	if (token.type == TOKEN_TYPE_IDENTIFIER) frequency_push(self->identifier, &word);
	else frequency_push(self->keyword, &word);

	return token;
}

void lua_lexer_read_int(struct lua_lexer *self, struct string *number)
{
	while (isdigit(self->char_)) lua_lexer_push_and_advance(self, number);
}

enum token_type lua_lexer_number_hex(struct lua_lexer *self, struct string *number)
{
	do {
		lua_lexer_push_and_advance(self, number);
	} while (isxdigit(self->char_));

	if (!isxdigit(string_get(number, number->length - 1))) {
		file_print_error(self->file,
				"Hexadecial numbers need to have at least one number after the preffix '0x'/'0X'",
				number->length);
		return TOKEN_TYPE_INVALID;
	} else return TOKEN_TYPE_LIT_HEX;
}

enum token_type lua_lexer_read_scientific_suffix(struct lua_lexer *self, struct string *number)
{
	lua_lexer_push_and_advance(self, number);
	if (self->char_ == '+' || self->char_ == '-') lua_lexer_push_and_advance(self, number);
	lua_lexer_read_int(self, number);

	if (!isdigit(string_get(number, number->length - 1))) {
		file_print_error(self->file,
				"Scientific numbers need to have at least one number after the prefix 'e', 'e+' or 'e-'",
				number->length);
		return TOKEN_TYPE_INVALID;
	} else return TOKEN_TYPE_LIT_SCIENTIFIC;
}

enum token_type lua_lexer_read_number(struct lua_lexer *self, struct string *number)
{
	enum token_type type = TOKEN_TYPE_LIT_INTEGER;

	lua_lexer_read_int(self, number);
	if (self->char_ == '.') {
		lua_lexer_push_and_advance(self, number);
		lua_lexer_read_int(self, number);
		type = TOKEN_TYPE_LIT_DECIMAL;
	}

	if (type == TOKEN_TYPE_LIT_DECIMAL && string_get(number, number->length - 1) == '.') {
		file_print_error(self->file, "Floating numbers expect at least one digit after '.'", number->length);
		type = TOKEN_TYPE_INVALID;
	}

	return type;
}

struct token lua_lexer_number(struct lua_lexer *self)
{
	struct string *number = string_alloc();
	struct token token = { .type=TOKEN_TYPE_LIT_INTEGER };

	if (self->char_ == '0' && tolower(file_buffer_peek_next(self->file->file_buffer)) == 'x') {
		lua_lexer_push_and_advance(self, number);

		token.type = lua_lexer_number_hex(self, number);
	} else token.type = lua_lexer_read_number(self, number);

	if (token.type != TOKEN_TYPE_INVALID && self->char_ == 'e') token.type = lua_lexer_read_scientific_suffix(self, number);

	if (token.type == TOKEN_TYPE_INVALID) {
		frequency_push(self->invalid, &number);

		struct string *tmp = string_clone(self->template_invalid_number);
		frequency_push(self->what_invalid, &tmp);
	} else frequency_push(self->number, &number);

	return token;
}

struct token lua_lexer_string(struct lua_lexer *self, bool double_quotes)
{
	struct string *string = string_alloc();
	struct token token = { .type=TOKEN_TYPE_STRING };
	bool continue_ = false;
	
	do {
		if (self->char_ == EOF || self->char_ == '\n') {

			file_print_error(self->file, "Normal strings needs to be single lined, "
					"and end with the same quotes (single or double) as it started",
					string->length);
			token.type = TOKEN_TYPE_INVALID;
			break;
		}
		if (double_quotes) {
			if (self->char_ == '"') continue_ = !continue_;
		} else if (self->char_ == '\'') continue_ = !continue_;

		if (self->char_ == '\\') {
			char peek = file_buffer_peek_next(self->file->file_buffer);
			if (peek == '\'' || peek == '"' || peek == '\\') lua_lexer_push_and_advance(self, string);
		}

		lua_lexer_push_and_advance(self, string);
	} while (continue_);

	if (token.type == TOKEN_TYPE_INVALID) {
		frequency_push(self->invalid, &string);

		struct string *tmp = string_clone(self->template_invalid_string);
		frequency_push(self->what_invalid, &tmp);
	} else frequency_push(self->string, &string);

	return token;
}

struct token lua_lexer_multi_line_string(struct lua_lexer *self)
{
	struct string *string = string_alloc();
	struct token token = { .type=TOKEN_TYPE_MULTI_LINE_STRING };
	
	while (!(self->char_ == ']' && file_buffer_peek_next(self->file->file_buffer) == ']')) {
		if (self->char_ == EOF) {
			file_print_error(self->file, "Multi lined strings need a clossing ']], "
					"but founded EOF (End Of File)",
					string->length + 1);
			token.type = TOKEN_TYPE_INVALID;
			break;
		} else lua_lexer_push_and_advance(self, string);
	}

	lua_lexer_push_and_advance(self, string);
	lua_lexer_push_and_advance(self, string);

	if (token.type == TOKEN_TYPE_INVALID) {
		frequency_push(self->invalid, &string);

		struct string *tmp = string_clone(self->template_invalid_string);
		frequency_push(self->what_invalid, &tmp);
	} else frequency_push(self->string, &string);

	return token;
}

struct token lua_lexer_characters(struct lua_lexer *self)
{
	struct token token = { .type=TOKEN_TYPE_INVALID };
	struct string *symbol = string_alloc();
	char peek = file_buffer_peek_next(self->file->file_buffer);

	switch (self->char_) {
		case '+':	token.type = TOKEN_TYPE_CHAR_PLUS;		break;
		case '-':	token.type = TOKEN_TYPE_CHAR_MINUS;		break;
		case '*':	token.type = TOKEN_TYPE_CHAR_MULT;		break;
		case '%':	token.type = TOKEN_TYPE_CHAR_MODULE;		break;
		case '^':	token.type = TOKEN_TYPE_CHAR_CARET;		break;
		case '#':	token.type = TOKEN_TYPE_CHAR_HASH;		break;
		case '&':	token.type = TOKEN_TYPE_CHAR_AMPERSAND;	break;
		case ',':	token.type = TOKEN_TYPE_CHAR_COMMA;		break;
		case ';':	token.type = TOKEN_TYPE_CHAR_SEMICOLON;	break;
		case '|':	token.type = TOKEN_TYPE_CHAR_VBAR;		break;
		case '(':	token.type = TOKEN_TYPE_CHAR_LPARENTHESIS;	break;
		case ')':	token.type = TOKEN_TYPE_CHAR_RPARENTHESIS;	break;
		case '[':	token.type = TOKEN_TYPE_CHAR_LBRACKET;	break;
		case ']':	token.type = TOKEN_TYPE_CHAR_RBRACKET;	break;
		case '{':	token.type = TOKEN_TYPE_CHAR_LBRACE;		break;
		case '}':	token.type = TOKEN_TYPE_CHAR_RBRACE;		break;
		case '<':
			if (peek == '<') {
				token.type = TOKEN_TYPE_CHAR_SHIFT_LEFT;
				lua_lexer_push_and_advance(self, symbol);
			} else if (peek == '=') {
				token.type = TOKEN_TYPE_CHAR_LESS_OR_EQUALS_THAN;
				lua_lexer_push_and_advance(self, symbol);
			} else token.type = TOKEN_TYPE_CHAR_LESS_THAN;
		break;
		case '>':
			if (peek == '>') {
				token.type = TOKEN_TYPE_CHAR_SHIFT_RIGHT;
				lua_lexer_push_and_advance(self, symbol);
			} else if (peek == '=') {
				token.type = TOKEN_TYPE_CHAR_GREATER_OR_EQUALS_THAN;
				lua_lexer_push_and_advance(self, symbol);
			} else token.type = TOKEN_TYPE_CHAR_GREATER_THAN;
		break;
		case ':':
			if (peek == ':') {
				token.type = TOKEN_TYPE_CHAR_DOUBLE_COLON;
				lua_lexer_push_and_advance(self, symbol);
			}
			else token.type = TOKEN_TYPE_CHAR_COLON;
		break;
		case '/':
			if (peek == '/') {
				token.type = TOKEN_TYPE_CHAR_DOUBLE_SLASH;
				lua_lexer_push_and_advance(self, symbol);
			}
			else token.type = TOKEN_TYPE_CHAR_SLASH;
		break;
		case '=':
			if (peek == '=') {
				token.type = TOKEN_TYPE_CHAR_EQUALS;
				lua_lexer_push_and_advance(self, symbol);
			}
			else token.type = TOKEN_TYPE_CHAR_ASSIGN;
		break;
		case '~':
			if (peek == '=') {
				token.type = TOKEN_TYPE_CHAR_NOT_EQUALS;
				lua_lexer_push_and_advance(self, symbol);
			}
			else token.type = TOKEN_TYPE_CHAR_TILDE;
		break;
		case '.':
			if	(peek == '.') {
				lua_lexer_push_and_advance(self, symbol);
				peek = file_buffer_peek_next(self->file->file_buffer);

				if (peek == '.') {
					lua_lexer_push_and_advance(self, symbol);
					token.type = TOKEN_TYPE_CHAR_EPSILON;
				} else token.type = TOKEN_TYPE_CHAR_CONCAT;
			} else token.type = TOKEN_TYPE_CHAR_PERIOD;
		break;
		default:
			file_print_error(self->file, "Can't identify a character", 1);
			token.type = TOKEN_TYPE_INVALID;
		break;
	};

	lua_lexer_push_and_advance(self, symbol);

	if (token.type == TOKEN_TYPE_INVALID) {
		frequency_push(self->invalid, &symbol);

		struct string *tmp = string_clone(self->template_invalid_char);
		frequency_push(self->what_invalid, &tmp);
	} else frequency_push(self->character, &symbol);

	return token;
}

struct token lua_lexer_next(struct lua_lexer *self)
{
	lua_lexer_skip_blank(self);

	if	(self->char_ == EOF) return (struct token) { .type=TOKEN_TYPE_EOF, .id=0 };
	else if (self->char_ == '[' && file_buffer_peek_next(self->file->file_buffer) == '[') return lua_lexer_multi_line_string(self);
	else if (isalpha(self->char_) || self->char_ == '_') return lua_lexer_word(self);
	else if (isdigit(self->char_))	return lua_lexer_number(self);
	else if (self->char_ == '\'')	return lua_lexer_string(self, false);
	else if (self->char_ == '"')	return lua_lexer_string(self, true);
	else				return lua_lexer_characters(self);
}

void lua_lexer_drop(struct lua_lexer **self)
{
	if (!self || !(*self)) return;

	frequency_drop(&(*self)->identifier);
	frequency_drop(&(*self)->keyword);
	frequency_drop(&(*self)->string);
	frequency_drop(&(*self)->number);
	frequency_drop(&(*self)->multi_line_string);
	frequency_drop(&(*self)->character);
	frequency_drop(&(*self)->invalid);
	frequency_drop(&(*self)->what_invalid);

	string_drop(&(*self)->template_invalid_number);
	string_drop(&(*self)->template_invalid_string);
	string_drop(&(*self)->template_invalid_char);

	file_drop(&(*self)->file);
	list_drop(&(*self)->symbol_table);

	free(*self);
}

