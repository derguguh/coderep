#include "back_ends/lua/lexer.h"
#include "back_ends/lua/token.h"

#include <stdbool.h>
// #include "coderep/tools/cli/data.h"

int main(int argc, const char **argv)
{
	// struct data data = data_create();
	//
	// data_parse(&data, argv, argc, 1);

	struct string *file_path = string_from_cstr("../tests/lua/lexer/keywords.lua");
	struct lua_lexer *lexer = lua_lexer_alloc(file_path);

	for (int i = 0; i < 50; i++) {
		struct token token = lua_lexer_next(lexer);
		if (token.type != TOKEN_TYPE_UNKNOWN) token_print(token);
	}

	lua_lexer_drop(&lexer);
	string_drop(&file_path);
}

