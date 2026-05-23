#include "back_ends/lua/lexer.h"

#include "back_ends/lua/token.h"
#include "coderep/data_types/distributive.h"
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
	struct distributive *identifiers = distributive_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.keywords");
	struct distributive *keyword = distributive_alloc(&tmp_str, 20);

	tmp_str = string_from_cstr("lexer.symbols.numbers");
	struct distributive *number = distributive_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.strings");
	struct distributive *string = distributive_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.multi_line_strings");
	struct distributive *multi_line_string = distributive_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.symbols");
	struct distributive *character = distributive_alloc(&tmp_str, 40);

	struct file *file = file_alloc(file_path);

	struct lua_lexer *self = malloc(sizeof(struct lua_lexer));
	if (!self) MALLOC_ERR_EXIT("struct lexer (for lua)");

	self->char_ = file->file_buffer->char_;
	self->file = file;
	self->symbol_table = symbol_table;
	self->identifier = identifiers;
	self->keyword = keyword;
	self->number = number;
	self->string = string;
	self->multi_line_string = multi_line_string;
	self->character = character;

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

	if	(strcmp(cstr, "and") == 0)		return TOKEN_TYPE_AND;
	else if	(strcmp(cstr, "break") == 0)		return TOKEN_TYPE_BREAK;
	else if	(strcmp(cstr, "do") == 0)		return TOKEN_TYPE_DO;
	else if	(strcmp(cstr, "else") == 0)		return TOKEN_TYPE_ELSE;
	else if	(strcmp(cstr, "elseif") == 0)		return TOKEN_TYPE_ELSEIF;
	else if	(strcmp(cstr, "end") == 0)		return TOKEN_TYPE_END;
	else if	(strcmp(cstr, "false") == 0)		return TOKEN_TYPE_FALSE;
	else if	(strcmp(cstr, "for") == 0)		return TOKEN_TYPE_FOR;
	else if	(strcmp(cstr, "function") == 0)		return TOKEN_TYPE_FUNCTION;
	else if	(strcmp(cstr, "global") == 0)		return TOKEN_TYPE_GLOBAL;
	else if	(strcmp(cstr, "goto") == 0)		return TOKEN_TYPE_GOTO;
	else if	(strcmp(cstr, "if") == 0)		return TOKEN_TYPE_IF;
	else if	(strcmp(cstr, "in") == 0)		return TOKEN_TYPE_IN;
	else if	(strcmp(cstr, "local") == 0)		return TOKEN_TYPE_LOCAL;
	else if	(strcmp(cstr, "nil") == 0)		return TOKEN_TYPE_NIL;
	else if	(strcmp(cstr, "not") == 0)		return TOKEN_TYPE_NOT;
	else if	(strcmp(cstr, "or") == 0)		return TOKEN_TYPE_OR;
	else if	(strcmp(cstr, "repeat") == 0)		return TOKEN_TYPE_REPEAT;
	else if	(strcmp(cstr, "return") == 0)		return TOKEN_TYPE_RETURN;
	else if	(strcmp(cstr, "then") == 0)		return TOKEN_TYPE_THEN;
	else if	(strcmp(cstr, "true") == 0)		return TOKEN_TYPE_TRUE;
	else if	(strcmp(cstr, "until") == 0)		return TOKEN_TYPE_UNTIL;
	else if	(strcmp(cstr, "while") == 0)		return TOKEN_TYPE_WHILE;
	else						return TOKEN_TYPE_IDENTIFIER;
}

void lua_lexer_advance_and_push(struct lua_lexer *self, struct string *string)
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

		lua_lexer_advance_and_push(self, word);
	}

	token.type = lua_lexer_identifier_word(self, word);
	struct string *temp = string_clone(word);
	token.id = list_push(self->symbol_table, &temp);

	if (token.type == TOKEN_TYPE_IDENTIFIER) distributive_push(self->identifier, &word);
	else distributive_push(self->keyword, &word);

	return token;
}

struct token lua_lexer_hex(struct lua_lexer *self)
{
	struct string *number = string_alloc();
	struct token token = { .type=TOKEN_TYPE_INTEGER };
	// Skip "0x"
	lua_lexer_advance_and_push(self, number);
	lua_lexer_advance_and_push(self, number);

	while (isdigit(self->char_) || (self->char_ >= 'a' && self->char_ <= 'f') || (self->char_ >= 'A' && self->char_ <= 'F')) {

		lua_lexer_advance_and_push(self, number);
	}

	if (number->length == 2) {

		file_print_error(self->file, "Hexadecial numbers need values, not just the prefix", 2);
	}

	distributive_push(self->number, &number);

	return token;
}

void lua_lexer_scientific_suffix(struct lua_lexer *self, struct string *number, struct token *token)
{
	lua_lexer_advance_and_push(self, number);
	if (self->char_ == '+' || self->char_ == '-') lua_lexer_advance_and_push(self, number);

	size_t length_threshold = number->length;

	while (isdigit(self->char_)) lua_lexer_advance_and_push(self, number);

	if (length_threshold == number->length) {
		file_print_error(self->file, "Scientific numbers need at least one digit after the 'e', 'e+' or 'e-'", length_threshold);
		token->type=TOKEN_TYPE_UNKNOWN;
	} else token->type = TOKEN_TYPE_SCIENTIFIC_NUMBER;
}

struct token lua_lexer_number(struct lua_lexer *self)
{
	struct string *number = string_alloc();
	struct token token = { .type=TOKEN_TYPE_INTEGER };
	bool can_have_dot = true;
	bool is_scientific = false;

	while (isdigit(self->char_) || (can_have_dot && self->char_ == '.')) {
		if (self->char_ == '.') {
			can_have_dot = false;
			token.type = TOKEN_TYPE_DECIMAL;
		}
		
		lua_lexer_advance_and_push(self, number);
	}

	if (self->char_ == 'e' || self->char_ == 'E') lua_lexer_scientific_suffix(self, number, &token);

	if (token.type != TOKEN_TYPE_UNKNOWN) distributive_push(self->number, &number);
	else string_drop(&number);

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
			token.type = TOKEN_TYPE_UNKNOWN;
			string_drop(&string);
			return token;
		}
		if (double_quotes) {
			if (self->char_ == '"') continue_ = !continue_;
		} else if (self->char_ == '\'') continue_ = !continue_;

		if (self->char_ == '\\') {
			char peek = file_buffer_peek_next(self->file->file_buffer);
			if (peek == '\'' || peek == '"' || peek == '\\') lua_lexer_advance_and_push(self, string);
		}

		lua_lexer_advance_and_push(self, string);
	} while (continue_);

	distributive_push(self->string, &string);

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
			token.type = TOKEN_TYPE_UNKNOWN;
			string_drop(&string);
			return token;
		} else lua_lexer_advance_and_push(self, string);
	}

	lua_lexer_advance_and_push(self, string);
	lua_lexer_advance_and_push(self, string);

	distributive_push(self->multi_line_string, &string);

	return token;
}

struct token lua_lexer_characters(struct lua_lexer *self)
{
	struct token token = { .type=TOKEN_TYPE_UNKNOWN };
	struct string *symbol = string_alloc();
	char peek = file_buffer_peek_next(self->file->file_buffer);

	switch (self->char_) {
		case '+':	token.type = TOKEN_TYPE_PLUS;		break;
		case '-':	token.type = TOKEN_TYPE_MINUS;		break;
		case '*':	token.type = TOKEN_TYPE_MULT;		break;
		case '%':	token.type = TOKEN_TYPE_MODULE;		break;
		case '^':	token.type = TOKEN_TYPE_CARET;		break;
		case '#':	token.type = TOKEN_TYPE_HASH;		break;
		case '&':	token.type = TOKEN_TYPE_AMPERSAND;	break;
		case ',':	token.type = TOKEN_TYPE_COMMA;		break;
		case ';':	token.type = TOKEN_TYPE_SEMICOLON;	break;
		case '|':	token.type = TOKEN_TYPE_VBAR;		break;
		case '(':	token.type = TOKEN_TYPE_LPARENTHESIS;	break;
		case ')':	token.type = TOKEN_TYPE_RPARENTHESIS;	break;
		case '[':	token.type = TOKEN_TYPE_LBRACKET;	break;
		case ']':	token.type = TOKEN_TYPE_RBRACKET;	break;
		case '{':	token.type = TOKEN_TYPE_LBRACE;		break;
		case '}':	token.type = TOKEN_TYPE_RBRACE;		break;
		case '<':
			if (peek == '<') {
				token.type = TOKEN_TYPE_SHIFT_LEFT;
				lua_lexer_advance_and_push(self, symbol);
			} else if (peek == '=') {
				token.type = TOKEN_TYPE_LESS_OR_EQUALS_THAN;
				lua_lexer_advance_and_push(self, symbol);
			} else token.type = TOKEN_TYPE_LESS_THAN;
		break;
		case '>':
			if (peek == '>') {
				token.type = TOKEN_TYPE_SHIFT_RIGHT;
				lua_lexer_advance_and_push(self, symbol);
			} else if (peek == '=') {
				token.type = TOKEN_TYPE_GREATER_OR_EQUALS_THAN;
				lua_lexer_advance_and_push(self, symbol);
			} else token.type = TOKEN_TYPE_GREATER_THAN;
		break;
		case ':':
			if (peek == ':') {
				token.type = TOKEN_TYPE_DOUBLE_COLON;
				lua_lexer_advance_and_push(self, symbol);
			}
			else token.type = TOKEN_TYPE_COLON;
		break;
		case '/':
			if (peek == '/') {
				token.type = TOKEN_TYPE_DOUBLE_SLASH;
				lua_lexer_advance_and_push(self, symbol);
			}
			else token.type = TOKEN_TYPE_SLASH;
		break;
		case '=':
			if (peek == '=') {
				token.type = TOKEN_TYPE_EQUALS;
				lua_lexer_advance_and_push(self, symbol);
			}
			else token.type = TOKEN_TYPE_ASSIGN;
		break;
		case '~':
			if (peek == '=') {
				token.type = TOKEN_TYPE_NOT_EQUALS;
				lua_lexer_advance_and_push(self, symbol);
			}
			else token.type = TOKEN_TYPE_TILDE;
		break;
		case '.':
			if	(peek == '.') {
				lua_lexer_advance_and_push(self, symbol);
				peek = file_buffer_peek_next(self->file->file_buffer);

				if (peek == '.') {
					lua_lexer_advance_and_push(self, symbol);
					token.type = TOKEN_TYPE_EPSILON;
				} else token.type = TOKEN_TYPE_CONCAT;
			} else token.type = TOKEN_TYPE_PERIOD;
		break;
		default:
			file_print_error(self->file, "Can't identify a character", 1);

			string_drop(&symbol);

			file_advance(self->file);
			self->char_ = file_buffer_peek(self->file->file_buffer);
			return token;
		break;
	};

	lua_lexer_advance_and_push(self, symbol);
	distributive_push(self->character, &symbol);

	return token;
}

struct token lua_lexer_next(struct lua_lexer *self)
{
	lua_lexer_skip_blank(self);

	if	(self->char_ == EOF)							 return (struct token) { .type=TOKEN_TYPE_EOF, .id=0 };
	else if (self->char_ == '0' && file_buffer_peek_next(self->file->file_buffer)) return lua_lexer_hex(self);
	else if (self->char_ == '[' && file_buffer_peek_next(self->file->file_buffer)) return lua_lexer_multi_line_string(self);
	else if (isalpha(self->char_) || self->char_ == '_')				 return lua_lexer_word(self);
	else if (isdigit(self->char_))	return lua_lexer_number(self);
	else if (self->char_ == '\'')	return lua_lexer_string(self, false);
	else if (self->char_ == '"')	return lua_lexer_string(self, true);
	else				return lua_lexer_characters(self);
}

void lua_lexer_drop(struct lua_lexer **self)
{
	if (!self || !(*self)) return;

	file_drop(&(*self)->file);
	list_drop(&(*self)->symbol_table);
	distributive_drop(&(*self)->identifier);
	distributive_drop(&(*self)->keyword);
	distributive_drop(&(*self)->string);
	distributive_drop(&(*self)->number);
	distributive_drop(&(*self)->multi_line_string);
	distributive_drop(&(*self)->character);
	free(*self);
}

