#ifndef CODEREP_TOOL_COMMON_READERS_H
#define CODEREP_TOOL_COMMON_READERS_H

#include "coderep/tool/common_skips.h"
#include "coderep/data_type/string.h"
#include "coderep/tool/file.h"

#include <stddef.h>

static inline void push_and_advance(struct file *file, struct string *string)
{
	string_push(string, file->buffer->char_);
	file_advance(file);
}

size_t read_while(struct file *file, struct string *buffer, condition_fn condition);

size_t read_word(struct file *file, struct string *buffer);

size_t read_int(struct file *file, struct string *buffer, bool accept_underscore);
ssize_t read_hex(struct file *file, struct string *buffer, bool accept_underscore, bool need_digits);
ssize_t read_scientific_suffix(struct file *file, struct string *buffer, bool accept_underscore, bool need_digits);

#endif
