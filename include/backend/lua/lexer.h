#ifndef BACKEND_LUA_LEXER_H
#define BACKEND_LUA_LEXER_H

#include "backend/general/word_tree.h"

#include "coderep/tool/file.h"

struct lua_lexer {
	struct word_tree *keyword_tree;
	struct word_tree *character_tree;
	struct frequency *string;
	struct frequency *block_string;
	struct frequency *identifier;
	struct frequency *number;
	struct frequency *keyword;
	struct frequency *character;
	struct frequency *invalid;
	struct frequency *what_invalid;
	struct string *template_invalid_number;
	struct string *template_invalid_string;
	struct string *template_invalid_char;
};

extern const struct word_table LUA_KEYWORD_TABLE[];
extern const size_t LUA_KEYWORD_TABLE_L;

struct lua_token lua_lexer_next(struct lua_lexer *self, struct file *file, struct string *scratch_buffer);

#endif
