#ifndef BACKEND_TEXT_LEXER_H
#define BACKEND_TEXT_LEXER_H

#include "backend/text/token.h"
#include "coderep/allocator/arena.h"
#include "coderep/tool/file.h"

#include <stddef.h>

struct text_lexer {
	struct frequency *word;
	struct frequency *number;
	struct frequency *character;
	struct frequency *unknown;
	const struct string *template_unknown_char;
	struct string *string_buffer;
	struct allocator buffer_allocator;
};

enum text_token text_lexer_next(struct text_lexer *self, struct arena *allocator, struct file *file);

#endif
