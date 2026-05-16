#include "back_ends/lua/lexer.h"
#include "back_ends/lua/token.h"

#include <stdbool.h>
// #include "coderep/tools/cli/data.h"

int main(int argc, const char **argv)
{
	// struct data data = data_create();
	//
	// data_parse(&data, argv, argc, 1);

	struct string *file_path = string_from_cstr("../tests/lua/lexer/strings.lua");
	struct lua_lexer *lexer = lua_lexer_alloc(file_path);

	struct token token = lua_lexer_next(lexer);
	while (token.type != TOKEN_TYPE_EOF) {

		token = lua_lexer_next(lexer);
	}

	printf("Symbol Table elements count: %lu\n", lexer->symbol_table->element_count);
	distributive_print(lexer->word);
	distributive_print(lexer->number);
	distributive_print(lexer->string);
	distributive_print(lexer->multi_line_string);
	distributive_print(lexer->character);

	lua_lexer_drop(&lexer);
	string_drop(&file_path);
}

