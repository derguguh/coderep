#ifndef BACK_END_LUA_LEXER_H
#define BACK_END_LUA_LEXER_H

#include "back_ends/lua/token.h"
#include "coderep/data_types/frequency.h"
#include "coderep/data_types/list.h"
#include "coderep/tools/file.h"

#include <stddef.h>

struct lua_lexer {
	struct file *file;
	struct list *symbol_table;
	struct frequency *identifier;
	struct frequency *keyword;
	struct frequency *number;
	struct frequency *string;
	struct frequency *multi_line_string;
	struct frequency *character;
	char char_;
};

struct lua_lexer *lua_lexer_alloc(struct string *file_path);

enum token_type lua_lexer_identifier_word(const struct lua_lexer *self, struct string *word);

struct token lua_lexer_next(struct lua_lexer *self);

void lua_lexer_drop(struct lua_lexer **self);

#endif
