#include "backend/lua/lexer.h"

#include "coderep/tool/common_readers.h"
#include "backend/lua/token.h"

#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"
#include "coderep/macros.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/file.h"
#include "coderep/tool/string_reader.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// ------------ Tables ------------

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

const struct word_table LUA_PONCTUATION_TABLE[] = {
	{ "+",	LUA_TOKEN_TYPE_CHAR_PLUS },
	{ "-",	LUA_TOKEN_TYPE_CHAR_MINUS },
	{ "*",	LUA_TOKEN_TYPE_CHAR_MULT },
	{ "%",	LUA_TOKEN_TYPE_CHAR_MODULE },
	{ "^",	LUA_TOKEN_TYPE_CHAR_CARET },
	{ "#",	LUA_TOKEN_TYPE_CHAR_HASH },
	{ "&",	LUA_TOKEN_TYPE_CHAR_AMPERSAND },
	{ ",",	LUA_TOKEN_TYPE_CHAR_COMMA },
	{ ";",	LUA_TOKEN_TYPE_CHAR_SEMICOLON },
	{ "|",	LUA_TOKEN_TYPE_CHAR_VBAR },
	{ "(",	LUA_TOKEN_TYPE_CHAR_LPARENTHESIS },
	{ ")",	LUA_TOKEN_TYPE_CHAR_RPARENTHESIS },
	{ "[",	LUA_TOKEN_TYPE_CHAR_LBRACKET },
	{ "]",	LUA_TOKEN_TYPE_CHAR_RBRACKET },
	{ "{",	LUA_TOKEN_TYPE_CHAR_LBRACE },
	{ "}",	LUA_TOKEN_TYPE_CHAR_RBRACE },
	{ "<",	LUA_TOKEN_TYPE_CHAR_LESS_THAN },
	{ "<<",	LUA_TOKEN_TYPE_CHAR_SHIFT_LEFT },
	{ "<=",	LUA_TOKEN_TYPE_CHAR_LESS_OR_EQUALS_THAN },
	{ ">",	LUA_TOKEN_TYPE_CHAR_GREATER_THAN },
	{ ">>",	LUA_TOKEN_TYPE_CHAR_SHIFT_RIGHT },
	{ ">=",	LUA_TOKEN_TYPE_CHAR_GREATER_OR_EQUALS_THAN },
	{ ":",	LUA_TOKEN_TYPE_CHAR_COLON },
	{ "::",	LUA_TOKEN_TYPE_CHAR_DOUBLE_COLON },
	{ "/",	LUA_TOKEN_TYPE_CHAR_SLASH },
	{ "//",	LUA_TOKEN_TYPE_CHAR_DOUBLE_SLASH },
	{ "=",	LUA_TOKEN_TYPE_CHAR_ASSIGN },
	{ "==",	LUA_TOKEN_TYPE_CHAR_EQUALS },
	{ "~",	LUA_TOKEN_TYPE_CHAR_TILDE },
	{ "~=",	LUA_TOKEN_TYPE_CHAR_NOT_EQUALS },
	{ ".",	LUA_TOKEN_TYPE_CHAR_PERIOD },
	{ "..",	LUA_TOKEN_TYPE_CHAR_CONCAT },
	{ "...",LUA_TOKEN_TYPE_CHAR_EPSILON },
};

const size_t LUA_PONCTUATION_TABLE_L = ARRAY_SIZE(LUA_PONCTUATION_TABLE, struct word_table);

// ------------ Skips ------------
bool lua_skip_block_comment(struct file *file, struct string *scratch_buffer, struct arena *tmp_allocator)
{
	string_clear(scratch_buffer);
	push_and_advance(file, scratch_buffer);

	while (file->buffer->char_ == '=') push_and_advance(file, scratch_buffer);

	if (file->buffer->char_ != '[') return false;
	else {
		push_and_advance(file, scratch_buffer);
		scratch_buffer->chars[0] = ']';
		scratch_buffer->chars[scratch_buffer->length - 1] = ']';
		struct string_handler handler = (struct string_handler) {
			.end_sequence = string_clone(&tmp_allocator->abstract, scratch_buffer, true),
			._escape_handler = NULL,
			.escape_char = '\0',
			.accept_new_line = true,
			.can_end_at_eof = true,
		};
		string_handler_read(&handler, file, scratch_buffer, NULL);
		return true;
	}
}

void lua_skip_comments(struct file *file, struct string *scratch_buffer, struct arena *tmp_allocator)
{
	const struct file_buffer *cache = file->buffer;
	bool was_a_block_comment = false;

	while (isblank(cache->char_) && cache->char_ != EOF) file_advance(file);

	if (cache->char_ == '[') was_a_block_comment = lua_skip_block_comment(file, scratch_buffer, tmp_allocator);

	if (!was_a_block_comment) while (cache->char_ != '\n' && cache->char_ != EOF) file_advance(file);
}

void lua_skip_blank(struct file *file, struct string *scratch_buffer, struct arena *tmp_allocator)
{
	skip_whitespaces(file);

	if (file->buffer->char_ == '-' && file_buffer_peek_n(file->buffer, 1) == '-') {
		file_advance(file);
		file_advance(file);
		lua_skip_comments(file, scratch_buffer, tmp_allocator);

		lua_skip_blank(file, scratch_buffer, tmp_allocator);
	}
}

// ------------ Word ------------

struct lua_token lua_lexer_word(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
{
	struct lua_token token = { 0 };

	read_word(file, scratch_buffer);

	token.type = word_tree_categorize(self->keyword_tree, scratch_buffer);

	if (token.type == -1) frequency_intern(self->identifier, scratch_buffer);
	else frequency_intern(self->keyword, scratch_buffer);

	return token;
}

// ------------ String ------------

bool lua_escape_handler(struct file *file, struct string *_buffer, struct arena *tmp_allocator)
{
	if (_buffer == NULL) return false;
	push_and_advance(file, _buffer);

	switch (file->buffer->char_) {
	case 'z':
		push_and_advance(file, _buffer);
		while (isspace(file->buffer->char_)) push_and_advance(file, _buffer);
	break;
	default:
		push_and_advance(file, _buffer);
	break;
	}

	return true;
}

struct lua_token lua_lexer_string(struct lua_lexer *self, struct file *file, struct string *scratch_buffer, struct arena *tmp_allocator)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_LIT_STRING };
	push_and_advance(file, scratch_buffer);
	struct string_handler handler = (struct string_handler) {
		.end_sequence = string_clone(&tmp_allocator->abstract, scratch_buffer, true),
		._escape_handler = lua_escape_handler,
		.escape_char = '\\',
		.accept_new_line = false,
		.keep_end_sequence = true,
	};

	if (string_handler_read(&handler, file, scratch_buffer, tmp_allocator) == -1) {
		token.type = LUA_TOKEN_TYPE_INVALID;
		frequency_intern(self->invalid, scratch_buffer);
		frequency_intern(self->what_invalid, self->template_invalid_string);
	} else {
		string_append(scratch_buffer, handler.end_sequence);
		frequency_intern(self->string, scratch_buffer);
	}

	return token;
}

struct lua_token lua_lexer_block_string(struct file *file, struct string *scratch_buffer, struct arena *tmp_allocator, size_t n)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_LIT_MULTI_LINE_STRING };
	for (size_t i = 0; i < n; i++) push_and_advance(file, scratch_buffer);
	push_and_advance(file, scratch_buffer);

	struct string_handler handler = (struct string_handler) {
		.end_sequence = string_clone(&tmp_allocator->abstract, scratch_buffer, true),
		._escape_handler = NULL,
		.escape_char = '\0',
		.accept_new_line = true,
		.keep_end_sequence = true,
	};
	handler.end_sequence->chars[0] = ']';
	handler.end_sequence->chars[handler.end_sequence->length - 1] = ']';

	token.type = string_handler_read(&handler, file, scratch_buffer, tmp_allocator) == -1 ? LUA_TOKEN_TYPE_INVALID : LUA_TOKEN_TYPE_LIT_MULTI_LINE_STRING;

	return token;
}

// ------------ Numbers ------------

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

struct lua_token lua_lexer_maybe_special_number(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
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

// ------------ Characters ------------

struct lua_token lua_lexer_characters(struct lua_lexer *self, struct file *file, struct string *scratch_buffer)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_INVALID };

	token.id = punctuation_read(self->punctuation, file, scratch_buffer);

	if (token.id == 0) {
		frequency_intern(self->invalid, scratch_buffer);
		frequency_intern(self->what_invalid, self->template_invalid_string);
		token.id = LUA_TOKEN_TYPE_INVALID;
	} else frequency_intern(self->character, scratch_buffer);

	return token;
}

// ------------ Ambiguity ------------

struct lua_token lua_lexer_handle_lbracket(struct lua_lexer *self, struct file *file, struct string *scratch_buffer, struct arena *tmp_allocator)
{
	struct lua_token token = { .type = LUA_TOKEN_TYPE_CHAR_LBRACKET };
	push_and_advance(file, scratch_buffer);

	size_t n = 0;
	while (file_buffer_peek_n(file->buffer, n) == '=') n++;
	if (file_buffer_peek_n(file->buffer, n) == '[') {
		token = lua_lexer_block_string(file, scratch_buffer, tmp_allocator, n);
		if (token.type == LUA_TOKEN_TYPE_INVALID) {
			frequency_intern(self->invalid, scratch_buffer);
			frequency_intern(self->what_invalid, self->template_invalid_string);
		} else frequency_intern(self->block_string, scratch_buffer);
	} else frequency_intern(self->character, scratch_buffer);

	return token;
}

// ------------ Dispatcher ------------

struct lua_token lua_lexer_next(struct lua_lexer *self, struct file *file, struct string *scratch_buffer, struct arena *tmp_allocator)
{
	lua_skip_blank(file, scratch_buffer, tmp_allocator);
	string_clear(scratch_buffer);

	if	(file->buffer->char_ == EOF) return (struct lua_token) { .type=LUA_TOKEN_TYPE_EOF, .id=0 };
	else if (file->buffer->char_ == '[') return lua_lexer_handle_lbracket(self, file, scratch_buffer, tmp_allocator);
	else if (file->buffer->char_ == '\'' || file->buffer->char_ == '"') return lua_lexer_string(self, file, scratch_buffer, tmp_allocator);
	else if (isalpha(file->buffer->char_) || file->buffer->char_ == '_') return lua_lexer_word(self, file, scratch_buffer);
	else if (isdigit(file->buffer->char_))	return lua_lexer_maybe_special_number(self, file, scratch_buffer);
	else return lua_lexer_characters(self, file, scratch_buffer);
}

