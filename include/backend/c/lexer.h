#ifndef BACKEND_C_LEXER_H
#define BACKEND_C_LEXER_H

#include "coderep/tool/file.h"

struct c_lexer {
	struct frequency *string;
	struct frequency *identifier;
	struct frequency *number;
	struct frequency *keyword;
	struct frequency *character;
	struct frequency *invalid;
	struct frequency *what_invalid;
	struct string *template_invalid_number;
	struct string *template_invalid_string;
	struct string *template_invalid_char;
};

struct c_token c_lexer_next(struct c_lexer *self, struct file *file, struct string *scratch_buffer);

#endif
