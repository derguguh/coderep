#ifndef BACKEND_GENERAL_LEXER_H
#define BACKEND_GENERAL_LEXER_H

#include "coderep/tool/file.h"

#include <stddef.h>

void push_and_advance(struct file *file, struct string *string);

size_t skip_whitespaces(struct file *file);

size_t read_int(struct file *file, struct string *buffer);

void skip_shebang(struct file *file);

#endif
