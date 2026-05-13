#include "back_ends/lua/lexer.h"
#include "back_ends/lua/token.h"
#include "coderep/data_types/string.h"
#include "coderep/macros.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lua_lexer *lua_lexer_alloc(struct string *file_path)
{
	struct string *tmp_str = string_from_cstr("Lua Symbol Table");
	struct list *symbol_table = list_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.words");
	struct distributive *lua_words = distributive_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.strings");
	struct distributive *lua_strings = distributive_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lexer.symbols.numbers");
	struct distributive *lua_numbers = distributive_alloc(&tmp_str, 40);

	struct source *source = source_alloc(file_path);

	struct lua_lexer *self = malloc(sizeof(struct lua_lexer));
	if (!self) MALLOC_ERR_EXIT("struct lexer (for lua)");

	self->char_ = source->file_buffer->char_;
	self->source = source;
	self->symbol_table = symbol_table;
	self->words = lua_words;
	self->strings = lua_strings;
	self->numbers = lua_numbers;

	return self;
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

	source_advance(self->source);
	self->char_ = file_buffer_peek(self->source->file_buffer);
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
	distributive_push(self->words, &word);

	return token;
}

struct token lua_lexer_string(struct lua_lexer *self, bool double_quotes)
{
	struct string *string = string_alloc();
	struct token token = { .type=TOKEN_TYPE_STRING };
	bool continue_ = false;
	
	do {
		if (double_quotes) {
			if (self->char_ == '"') continue_ = !continue_;
		} else if (self->char_ == '\'') continue_ = !continue_;
		if (self->char_ == EOF) break;

		if (self->char_ == '\\') {
			char peek = file_buffer_peek_next(self->source->file_buffer);
			if (peek == '\'' || peek == '"' || peek == '\\') lua_lexer_advance_and_push(self, string);
		}

		lua_lexer_advance_and_push(self, string);
	} while (continue_);

	struct string *temp = string_clone(string);
	token.id = list_push(self->symbol_table, &temp);
	distributive_push(self->strings, &string);

	return token;
}

struct token lua_lexer_hex(struct lua_lexer *self)
{
	struct string *number = string_from_cstr("0x");
	struct token token = { .type=TOKEN_TYPE_INTEGER };
	// Skip "0x"
	source_advance(self->source);
	source_advance(self->source);

	while (isdigit(self->char_) || (self->char_ >= 'a' && self->char_ <= 'f') || (self->char_ >= 'A' && self->char_ <= 'F')) {

		lua_lexer_advance_and_push(self, number);
	}

	struct string *temp = string_clone(number);
	token.id = list_push(self->symbol_table, &temp);
	distributive_push(self->numbers, &number);

	return token;
}

void lua_lexer_scientific_suffix(struct lua_lexer *self, struct string *number)
{
	lua_lexer_advance_and_push(self, number);

	while (isdigit(self->char_)) lua_lexer_advance_and_push(self, number);
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

	if (self->char_ == 'e' || self->char_ == 'E') {

		lua_lexer_scientific_suffix(self, number);

		token.type = TOKEN_TYPE_SCIENTIFIC_NUMBER;
	}

	struct string *temp = string_clone(number);
	token.id = list_push(self->symbol_table, &temp);
	distributive_push(self->numbers, &number);

	return token;
}

void lua_lexer_skip_whitespaces(struct lua_lexer *self)
{
	while (isblank(self->char_) && self->char_ != EOF) {
		source_advance(self->source);
		self->char_ = file_buffer_peek(self->source->file_buffer);
	}
}

struct token lua_lexer_symbols(struct lua_lexer *self)
{
	source_advance(self->source);
	self->char_ = file_buffer_peek(self->source->file_buffer);
	return (struct token) { .type=TOKEN_TYPE_UNKNOWN, .id=0 };
}

struct token lua_lexer_next(struct lua_lexer *self)
{
	lua_lexer_skip_whitespaces(self);

	if	(self->char_ == EOF)	return (struct token) { .type=TOKEN_TYPE_EOF, .id=0 };
	else if (isalpha(self->char_))	return lua_lexer_word(self);
	else if (self->char_ == '\'')	return lua_lexer_string(self, false);
	else if (self->char_ == '"')	return lua_lexer_string(self, true);
	else if (self->char_ == '0') {

		char peek = file_buffer_peek_next(self->source->file_buffer);
		if (peek == 'x' || peek == 'X') return lua_lexer_hex(self);

		return lua_lexer_number(self);
	} else if (isdigit(self->char_))	return lua_lexer_number(self);
	else				return lua_lexer_symbols(self);
}

void lua_lexer_drop(struct lua_lexer **self)
{
	if (!self || !(*self)) return;

	source_drop(&(*self)->source);
	list_drop(&(*self)->symbol_table);
	distributive_drop(&(*self)->words);
	distributive_drop(&(*self)->strings);
	distributive_drop(&(*self)->numbers);
	free(*self);
}

