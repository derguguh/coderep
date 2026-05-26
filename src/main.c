#include "back_ends/lua/lexer.h"
#include "back_ends/lua/token.h"

#include "coderep/backend.h"
#include "coderep/data_types/brute.h"
#include "coderep/tools/file_buffer.h"
#include "coderep/cli/cli.h"

#include <stdbool.h>
#include <stdio.h>

void push_random_brute(struct data_array *data_array)
{
	struct string *name = string_from_cstr("test.data_array");
	struct string *string = string_from_cstr("some random data");
	struct brute *brute = brute_alloc(&name, &string);

	data_array_push_brute(data_array, &brute);
}

void push_random_frequency(struct data_array *data_array)
{
	struct string *name = string_from_cstr("test.data_array");
	struct frequency *frequency = frequency_alloc(&name, 1);

	struct string *string = string_from_cstr("some random data");
	frequency_push(frequency, &string);

	data_array_push_frequency(data_array, &frequency);
}

void push_random_list(struct data_array *data_array)
{
	struct string *name = string_from_cstr("test.data_array");
	struct list *list = list_alloc(&name, 1);

	struct string *string = string_from_cstr("some random data");
	list_push(list, &string);

	data_array_push_list(data_array, &list);
}

void put_into_lua_c_latter();

int main(int argc, const char **argv)
{
	struct cli *cli = cli_alloc();

	cli_parse(cli, argv, argc);

	struct data_array *data = data_array_alloc();

	for (int i = 0; i < 0; i++) {

		push_random_brute(data);
		push_random_list(data);
		push_random_frequency(data);
	}

	data_array_print(data, cli->data);
	printf("struct data_array arrays length: %lu\n", data->brutes_l);
	printf("struct data_array arrays capacity: %lu\n", data->brutes_c);

	data_array_drop(&data);

	cli_drop(&cli);
	
	put_into_lua_c_latter();
}

void put_into_lua_c_latter() {
	struct string *file_path = string_from_cstr("../tests/lua/lexer/numbers.lua");
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

	frequency_print(lexer->invalid);
	frequency_print(lexer->what_invalid);

	brute_drop(&byte_count);
	lua_lexer_drop(&lexer);
	string_drop(&file_path);
}

