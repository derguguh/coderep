#include "back_ends/lua/lexer.h"
#include "back_ends/lua/token.h"

#include "coderep/backend.h"
#include "coderep/data_types/brute.h"
#include "coderep/tools/file_buffer.h"
#include "coderep/cli/cli.h"

#include <stdbool.h>
#include <stdio.h>

void push_random_data(struct data_array *data_array)
{
	struct string *name = string_from_cstr("test.data_array");
	struct string *string = string_from_cstr("some random data");
	struct brute *brute = brute_alloc(&name, &string);

	data_array_push_brute(data_array, &brute);
}

int main(int argc, const char **argv)
{
	struct cli *cli = cli_alloc();

	cli_parse(cli, argv, argc);

	struct data_array *data = data_array_alloc();

	for (int i = 0; i < 32; i++) push_random_data(data);

	data_array_print(data, cli->data);

	printf("Hello!\n");
	data_array_drop(&data);
	printf("Hello!\n");

	cli_drop(&cli);
}

void put_into_lua_c_latter() {
	struct string *file_path = string_from_cstr("../tests/lua/lexer/strings.lua");
	struct lua_lexer *lexer = lua_lexer_alloc(file_path);

	struct token token = lua_lexer_next(lexer);
	while (token.type != TOKEN_TYPE_EOF) {

		token = lua_lexer_next(lexer);
	}

	char buffer[] = "          ";
	snprintf(buffer, sizeof(buffer), "%lu", lexer->file->file_buffer->block * BUFFER_SIZE + lexer->file->file_buffer->buffer_length);
	struct string *name = string_from_cstr("file.byte_count");
	struct string *value = string_from_cstr(buffer);
	struct brute *byte_count = brute_alloc(&name, &value);

	printf("Symbol Table elements count: %lu\n", lexer->symbol_table->element_count);
	list_print(lexer->symbol_table);
	brute_print(byte_count);
	frequency_print(lexer->identifier);
	frequency_print(lexer->keyword);
	frequency_print(lexer->number);
	frequency_print(lexer->string);
	frequency_print(lexer->multi_line_string);
	frequency_print(lexer->character);

	brute_drop(&byte_count);
	lua_lexer_drop(&lexer);
	string_drop(&file_path);
}

