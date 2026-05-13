#ifndef BACK_END_LUA_LEXER_H
#define BACK_END_LUA_LEXER_H

#include "back_ends/lua/token.h"
#include "coderep/data_types/distributive.h"
#include "coderep/data_types/list.h"
#include "coderep/tools/source.h"

#include <stddef.h>

struct lua_lexer {
	struct source *source;
	struct list *symbol_table;
	struct distributive *words;
	struct distributive *strings;
	struct distributive *numbers;
	char char_;
};

struct lua_lexer *lua_lexer_alloc(struct string *file_path);

enum token_type lua_lexer_identifier_word(const struct lua_lexer *self, struct string *word);

struct token lua_lexer_next(struct lua_lexer *self);

void lua_lexer_drop(struct lua_lexer **self);

#endif
