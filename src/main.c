#include "back_ends/lua/lexer.h"
#include "back_ends/lua/token.h"
#include "coderep/data_types/brute.h"
#include "coderep/tools/file_buffer.h"

#include <stdbool.h>
#include <stdio.h>
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

	char buffer[] = "          ";
	snprintf(buffer, sizeof(buffer), "%lu", lexer->source->file_buffer->block * BUFFER_SIZE + lexer->source->file_buffer->buffer_length);
	struct string *name = string_from_cstr("file.byte_count");
	struct string *value = string_from_cstr(buffer);
	struct brute *byte_count = brute_alloc(&name, &value);

	printf("Symbol Table elements count: %lu\n", lexer->symbol_table->element_count);
	list_print(lexer->symbol_table);
	brute_print(byte_count);
	distributive_print(lexer->identifier);
	distributive_print(lexer->keyword);
	distributive_print(lexer->number);
	distributive_print(lexer->string);
	distributive_print(lexer->multi_line_string);
	distributive_print(lexer->character);

	brute_drop(&byte_count);
	lua_lexer_drop(&lexer);
	string_drop(&file_path);
}

