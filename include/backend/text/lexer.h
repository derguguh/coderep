#ifndef BACKEND_TEXT_LEXER_H
#define BACKEND_TEXT_LEXER_H

#include "backend/text/token.h"
#include "coderep/tool/file.h"

#include <stddef.h>

struct text_lexer {
	struct frequency *word;
	struct frequency *number;
	struct frequency *character;
	struct frequency *unknown;
	const struct string *template_unknown_char;
};

enum text_token text_lexer_next(struct text_lexer *self, struct file *file, struct string *scratch_buffer);

#endif
