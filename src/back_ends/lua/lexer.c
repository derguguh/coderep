#include "back_ends/lua/lexer.h"
#include "back_ends/lua/token.h"
#include "coderep/macros.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

struct lua_lexer *lua_lexer_alloc(struct string *file_path)
{
	struct string *tmp_str = string_from_cstr("Lua Symbol Table");
	struct list *symbol_table = list_alloc(&tmp_str, 40);

	tmp_str = string_from_cstr("lua.words");
	struct distributive *lua_words = distributive_alloc(&tmp_str, 40);

	struct source *source = source_alloc(file_path);

	struct lua_lexer *self = malloc(sizeof(struct lua_lexer));
	if (!self) MALLOC_ERR_EXIT("struct lexer (for lua)");

	self->source = source;
	self->symbol_table = symbol_table;
	self->lua_words = lua_words;

	return self;
}

struct token lua_lexer_next(struct lua_lexer *self)
{
	source_advance(self->source);
	self->char_ = file_buffer_peek(self->source->file_buffer);

	if (isalpha(self->char_)) return lua_lexer_word(self);
	else return (struct token) { .type=TOKEN_TYPE_UNKNOWN, .id=0 };
}

struct token lua_lexer_word(struct lua_lexer *self)
{
	struct string *word = string_alloc();
	struct token token = { .type=TOKEN_TYPE_WORD };

	while (isalnum(self->char_)) {
		string_push(word, self->char_);

		source_advance(self->source);
		self->char_ = file_buffer_peek(self->source->file_buffer);
	}

	string_print(word);
	putchar('\n');
	struct string *temp = string_clone(word);
	token.id = list_push(self->symbol_table, &temp);
	distributive_push(self->lua_words, &word);

	return token;
}

void lua_lexer_drop(struct lua_lexer **self)
{
	if (!self || !(*self)) return;

	source_drop(&(*self)->source);
	list_drop(&(*self)->symbol_table);
	distributive_drop(&(*self)->lua_words);
	free(*self);
}

