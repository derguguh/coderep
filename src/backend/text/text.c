#include "backend/text/text.h"

#include "backend/text/lexer.h"
#include "backend/text/token.h"
#include "coderep/allocator/allocator.h"
#include "coderep/allocator/arena.h"
#include "coderep/report.h"
#include "coderep/data_type/frequency.h"

size_t text_request()
{
	return sizeof(struct text_lexer);
}

void text_init(struct backend_context *data)
{
	struct text_lexer *lexer = (struct text_lexer *)data->requested;
	struct allocator *allocator = &data->allocator->abstract;

	*lexer = (struct text_lexer) { 0 };

	lexer->template_unknown_char = string_from_cstr(allocator, "Unknown: character");

	struct string *string = string_from_cstr(allocator, "text.word");
	lexer->word = frequency_alloc(allocator, &string, data->profile.extreme);

	string = string_from_cstr(allocator, "text.character");
	lexer->character = frequency_alloc(allocator, &string, data->profile.light);

	string = string_from_cstr(allocator, "text.number");
	lexer->number = frequency_alloc(allocator, &string, data->profile.medium);

	string = string_from_cstr(allocator, "text.unknown");
	lexer->unknown = frequency_alloc(allocator, &string, data->profile.baseline);
}

void text_parse_file(struct backend_context *data, struct arena *ignore, struct file *file)
{
	enum text_token token = TEXT_TOKEN_EOF;
	do {
		token = text_lexer_next((struct text_lexer *)data->requested, file, data->scratch_buffer);
	} while (token != TEXT_TOKEN_EOF);
}

void text_deinit(struct backend_context *data)
{
	struct text_lexer *lexer = (struct text_lexer *)data->requested;

	report_push_frequency(data->report, &lexer->word);
	report_push_frequency(data->report, &lexer->number);
	report_push_frequency(data->report, &lexer->character);
	report_push_frequency(data->report, &lexer->unknown);
}

