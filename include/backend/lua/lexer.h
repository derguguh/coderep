#ifndef BACKEND_LUA_LEXER_H
#define BACKEND_LUA_LEXER_H

#include "coderep/tool/punctuation.h"
#include "coderep/tool/word_tree.h"

#include "coderep/allocator/arena.h"
#include "coderep/tool/file.h"

struct lua_lexer {
	struct word_tree *keyword_tree;
	struct punctuation *punctuation;
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

extern const struct word_table LUA_PONCTUATION_TABLE[];
extern const size_t LUA_PONCTUATION_TABLE_L;

struct lua_token lua_lexer_next(struct lua_lexer *self, struct file *file, struct string *scratch_buffer, struct arena *tmp_allocator);

#endif
