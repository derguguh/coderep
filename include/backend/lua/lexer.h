#ifndef BACKEND_LUA_LEXER_H
#define BACKEND_LUA_LEXER_H

#include "coderep/tool/file.h"
struct lua_lexer {
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

struct lua_token lua_lexer_next(struct lua_lexer *self, struct file *file, struct string *scratch_buffer);

#endif
