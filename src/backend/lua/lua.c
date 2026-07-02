#include "backend/lua/lua.h"

#include "backend/backend.h"
#include "backend/general/lexer.h"
#include "backend/lua/lexer.h"
#include "backend/lua/token.h"
#include "coderep/allocator/allocator.h"
#include "coderep/allocator/arena.h"
#include "coderep/data_array.h"
#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"

size_t lua_request()
{
	return sizeof(struct lua_lexer);
}

void lua_init(struct backend_data *data)
{
	struct lua_lexer *context = (struct lua_lexer *)data->context;
	struct allocator *allocator = &data->allocator->abstract;

	*context = (struct lua_lexer) { 0 };

	context->template_invalid_number = string_from_cstr(allocator, "Invalid: number");
	context->template_invalid_string = string_from_cstr(allocator, "Invalid: string");
	context->template_invalid_char = string_from_cstr(allocator, "Invalid: character");

	struct string *string = string_from_cstr(allocator, "lexer.keyword");
	context->keyword = frequency_alloc(allocator, &string, 23);

	string = string_from_cstr(allocator, "lexer.identifier");
	context->identifier = frequency_alloc(allocator, &string, 16 * 1024);

	string = string_from_cstr(allocator, "lexer.character");
	context->character = frequency_alloc(allocator, &string, 32);

	string = string_from_cstr(allocator, "lexer.number");
	context->number = frequency_alloc(allocator, &string, 4 * 1024);

	string = string_from_cstr(allocator, "lexer.string");
	context->string = frequency_alloc(allocator, &string, 4 * 1024);

	string = string_from_cstr(allocator, "lexer.block_string");
	context->block_string = frequency_alloc(allocator, &string, 4 * 1024);

	string = string_from_cstr(allocator, "lexer.invalid");
	context->invalid = frequency_alloc(allocator, &string, 1024);

	string = string_from_cstr(allocator, "lexer.invalid.what");
	context->what_invalid = frequency_alloc(allocator, &string, 3);

	context->keyword_tree = word_tree_from_table(allocator, LUA_KEYWORD_TABLE, LUA_KEYWORD_TABLE_L);
}

void lua_parse_file(struct backend_data *data, struct arena *tmp_allocator, struct file *file)
{
	skip_shebang(file);
	struct lua_token token = { .type = LUA_TOKEN_TYPE_EOF };
	do {
		token = lua_lexer_next((struct lua_lexer *)data->context, file, data->scratch_buffer, tmp_allocator);
	} while (token.type != LUA_TOKEN_TYPE_EOF);
}

void lua_deinit(struct backend_data *data)
{
	struct lua_lexer *context = (struct lua_lexer *)data->context;

	data_array_push_frequency(data->array, &context->string);
	data_array_push_frequency(data->array, &context->block_string);
	data_array_push_frequency(data->array, &context->identifier);
	data_array_push_frequency(data->array, &context->number);
	data_array_push_frequency(data->array, &context->keyword);
	data_array_push_frequency(data->array, &context->character);
	data_array_push_frequency(data->array, &context->invalid);
	data_array_push_frequency(data->array, &context->what_invalid);
}

