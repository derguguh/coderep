#include "backend/text/text.h"

#include "backend/text/lexer.h"
#include "backend/text/token.h"
#include "coderep/allocator/allocator.h"
#include "coderep/allocator/arena.h"
#include "coderep/data_array.h"
#include "coderep/data_type/frequency.h"

size_t text_request()
{
	return sizeof(struct text_lexer);
}

void text_init(struct backend_data *data)
{
	struct text_lexer *context = (struct text_lexer *)data->context;
	struct allocator *allocator = &data->allocator->abstract;

	*context = (struct text_lexer) {
		.buffer_allocator = default_get_allocator(),
	};

	context->template_unknown_char = string_from_cstr(allocator, "Unknown: character");
	context->string_buffer = string_alloc(&context->buffer_allocator, 256);

	struct string *string = string_from_cstr(allocator, "text.word");
	context->word = frequency_alloc(allocator, &string, 64 * 1024);

	string = string_from_cstr(allocator, "text.character");
	context->character = frequency_alloc(allocator, &string, 4 * 1024);

	string = string_from_cstr(allocator, "text.number");
	context->number = frequency_alloc(allocator, &string, 16 * 1024);

	string = string_from_cstr(allocator, "text.unknown");
	context->unknown = frequency_alloc(allocator, &string, 1024);
}

void text_parse_file(struct backend_data *data, struct arena *ignore, struct file *file)
{
	enum text_token token = TEXT_TOKEN_EOF;
	do {
		token = text_lexer_next((struct text_lexer *)data->context, data->allocator, file);
	} while (token != TEXT_TOKEN_EOF);
}

void text_deinit(struct backend_data *data)
{
	struct text_lexer *context = (struct text_lexer *)data->context;

	data_array_push_frequency(data->array, &context->word);
	data_array_push_frequency(data->array, &context->number);
	data_array_push_frequency(data->array, &context->character);
	data_array_push_frequency(data->array, &context->unknown);

	string_drop(&context->string_buffer);
}

