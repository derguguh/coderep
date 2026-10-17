#ifndef CODEREP_TOOL_STRING_READER_H
#define CODEREP_TOOL_STRING_READER_H

#include "coderep/allocator/arena.h"
#include "coderep/tool/file.h"

#include <stdbool.h>
#include <sys/types.h>

typedef bool (*handle_escape_sequence_fn)(struct file *file, struct string *_buffer, struct arena *tmp_allocator);

struct string_handler {
	struct string *end_sequence;
	handle_escape_sequence_fn _escape_handler;
	char escape_char;
	bool accept_new_line;
	bool can_end_at_eof;
	bool keep_end_sequence;
};

ssize_t string_handler_read(struct string_handler *self, struct file *file, struct string *scratch_buffer, struct arena *temporary);

#endif
