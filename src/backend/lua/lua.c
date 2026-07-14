#include "backend/lua/lua.h"

#include "backend/backend.h"
#include "backend/generic/lexer.h"
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

	context->what_invalid = frequency_alloc(allocator, 3);
	context->character = frequency_alloc(allocator, 32);
	context->keyword = frequency_alloc(allocator, LUA_KEYWORD_TABLE_L);
	context->invalid = frequency_alloc(allocator, data->profile.minimal);
	context->string = frequency_alloc(allocator, data->profile.light);
	context->block_string = frequency_alloc(allocator, data->profile.light);
	context->number = frequency_alloc(allocator, data->profile.medium);
	context->identifier = frequency_alloc(allocator, data->profile.extreme);

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

	report_push_frequency(data->report, "lexer.string", &context->string);
	report_push_frequency(data->report, "lexer.block_string", &context->block_string);
	report_push_frequency(data->report, "lexer.character", &context->identifier);
	report_push_frequency(data->report, "lexer.number", &context->number);
	report_push_frequency(data->report, "lexer.string", &context->keyword);
	report_push_frequency(data->report, "lexer.block_string", &context->character);
	report_push_frequency(data->report, "lexer.invalid", &context->invalid);
	report_push_frequency(data->report, "lexer.invalid.what", &context->what_invalid);
}

