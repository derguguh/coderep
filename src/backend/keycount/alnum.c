#include "backend/keycount/alnum.h"

#include "backend/generic/lexer.h"
#include "backend/keycount/keycount.h"
#include "coderep/allocator/arena.h"
#include "coderep/data_type/frequency.h"
#include "coderep/report.h"

#include <ctype.h>

size_t keycount_alnum_request()
{
	return sizeof(struct keycount);
}

void keycount_alnum_init(struct backend_context *context)
{
	struct keycount *keycount = (struct keycount *)context->requested;
	struct allocator *allocator = &context->allocator->abstract;

	*keycount = (struct keycount) { 0 };

	keycount->shift_template = string_from_cstr(allocator, "shift");
	keycount->key = string_alloc(allocator, 128);
	keycount->control = frequency_alloc(allocator, context->profile.minimal);
	keycount->single_keys = frequency_alloc(allocator, context->profile.minimal);
	keycount->digraph = frequency_alloc(allocator, context->profile.heavy);
}

void keycount_alnum_parse_file(struct backend_context *context, struct arena *ignore, struct file *file)
{
	struct keycount *keycount = (struct keycount *)context->requested;

	while (file->buffer->char_ != EOF) {
		switch (file->buffer->char_) {
			case '\n':
			case '\t':
			case '\v':
			case '\f':
			case '\0':
			case ' ':
				string_clear(keycount->key);
				string_clear(context->scratch_buffer);
				file_advance(file);
				continue;
			break;
			default: {
				int utf8_len = 1;
				if      ((file->buffer->char_ & 0x80) == 0x00) {
					if (!isalnum(file->buffer->char_)) {
						string_clear(keycount->key);
						string_clear(context->scratch_buffer);
						file_advance(file);
						continue;
					}
					utf8_len = 1;
				}
				else if ((file->buffer->char_ & 0xE0) == 0xC0) utf8_len = 2;
				else if ((file->buffer->char_ & 0xF0) == 0xE0) utf8_len = 3;
				else if ((file->buffer->char_ & 0xF8) == 0xF0) utf8_len = 4;

				for (int i = 0; i < utf8_len - 1; i++) push_and_advance(file, keycount->key);

				string_push(keycount->key, file->buffer->char_);
				string_append(context->scratch_buffer, keycount->key);
			}
			break;
		}

		if (context->scratch_buffer->length == 2) {
			frequency_intern(keycount->digraph, context->scratch_buffer);
			string_clear(context->scratch_buffer);
			string_push(context->scratch_buffer, file->buffer->char_);
		}

		if (keycount->key->length == 1
			&& isalpha(file->buffer->char_) && isupper(file->buffer->char_)) frequency_intern(keycount->control, keycount->shift_template);
		frequency_intern(keycount->single_keys, keycount->key);
		string_clear(keycount->key);

		file_advance(file);
	}
}

void keycount_alnum_deinit(struct backend_context *context)
{
	struct keycount *keycount = (struct keycount *)context->requested;

	report_push_frequency(context->report, "keycount.digraph", &keycount->digraph);
	report_push_frequency(context->report, "keycount.key", &keycount->single_keys);
	report_push_frequency(context->report, "keycount.control", &keycount->control);
}

