#ifndef BACKEND_GENERAL_LEXER_H
#define BACKEND_GENERAL_LEXER_H

#include "coderep/tool/file.h"

#include <stddef.h>
#include <sys/types.h>

void push_and_advance(struct file *file, struct string *string);

size_t read_word(struct file *file, struct string *buffer);

size_t read_int(struct file *file, struct string *buffer, bool accept_underscore);
bool read_hex(struct file *file, struct string *scratch_buffer, bool need_digits, bool accept_underscore);
bool read_scientific_suffix(struct file *file, struct string *scratch_buffer, bool need_digits, bool accept_underscore);

void skip_shebang(struct file *file);
size_t skip_blank(struct file *file);
size_t skip_whitespaces(struct file *file);

#endif
