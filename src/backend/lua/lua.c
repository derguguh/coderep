#include "backend/lua/lua.h"

#include "backend/backend.h"
#include "backend/general/lexer.h"
#include "backend/lua/lexer.h"
#include "backend/lua/token.h"
#include "coderep/allocator/allocator.h"
#include "coderep/allocator/arena.h"
#include "coderep/report.h"
#include "coderep/data_type/frequency.h"
#include "coderep/data_type/string.h"

size_t lua_request()
{
	return sizeof(struct lua_lexer);
}

void lua_init(struct backend_context *data)
{
	struct lua_lexer *context = (struct lua_lexer *)data->requested;
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

void lua_parse_file(struct backend_context *data, struct arena *tmp_allocator, struct file *file)
{
	skip_shebang(file);
	struct lua_token token = { .type = LUA_TOKEN_TYPE_EOF };
	do {
		token = lua_lexer_next((struct lua_lexer *)data->requested, file, data->scratch_buffer, tmp_allocator);
	} while (token.type != LUA_TOKEN_TYPE_EOF);
}

void lua_deinit(struct backend_context *data)
{
	struct lua_lexer *context = (struct lua_lexer *)data->requested;

	report_push_frequency(data->report, &context->string);
	report_push_frequency(data->report, &context->block_string);
	report_push_frequency(data->report, &context->identifier);
	report_push_frequency(data->report, &context->number);
	report_push_frequency(data->report, &context->keyword);
	report_push_frequency(data->report, &context->character);
	report_push_frequency(data->report, &context->invalid);
	report_push_frequency(data->report, &context->what_invalid);
}

