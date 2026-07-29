#ifndef CODEREP_TOOL_COMMON_READERS_H
#define CODEREP_TOOL_COMMON_READERS_H

#include "coderep/allocator/arena.h"
#include "coderep/tool/file.h"

#include <stddef.h>
#include <sys/types.h>

typedef bool (*handle_escape_sequence_fn)(struct file *file, struct string *_buffer, struct arena *tmp_allocator);

struct string_handler {
	struct string *end_sequence;
	handle_escape_sequence_fn _escape_handler;
	char escape_char;
	bool accept_new_line;
};

void push_and_advance(struct file *file, struct string *string);

size_t read_word(struct file *file, struct string *buffer);

size_t read_int(struct file *file, struct string *buffer, bool accept_underscore);
ssize_t read_hex(struct file *file, struct string *buffer, bool accept_underscore, bool need_digits);
ssize_t read_scientific_suffix(struct file *file, struct string *buffer, bool accept_underscore, bool need_digits);

ssize_t read_string(struct file *file, struct string *_buffer, struct arena *tmp, struct string_handler *handler);

void skip_shebang(struct file *file);
size_t skip_blank(struct file *file);
size_t skip_whitespaces(struct file *file);

#endif
