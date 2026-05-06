#ifndef BACK_END_LUA_TOKEN_H
#define BACK_END_LUA_TOKEN_H

#include <stddef.h>

enum token_type {
	TOKEN_TYPE_UNKNOWN = -1,
	TOKEN_TYPE_WORD,
	TOKEN_TYPE_KEYWORD,
};

struct token {
	enum token_type type;
	size_t id;
};

void token_print(const struct token self);

#endif
