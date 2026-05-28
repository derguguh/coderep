#ifndef BACKEND_LUA_LEXICAL_TABLE_H
#define BACKEND_LUA_LEXICAL_TABLE_H

#include "coderep/data_array.h"

struct lua_lexical_table {
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

struct lua_lexical_table *lua_lexical_table_alloc();

void lua_lexical_table_append_to_data_array(struct lua_lexical_table *self, struct data_array *data_array);

void lua_lexical_table_drop(struct lua_lexical_table **self);

#endif
