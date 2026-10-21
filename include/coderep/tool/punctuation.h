#ifndef CODEREP_TOOL_PONCTUATION_H
#define CODEREP_TOOL_PONCTUATION_H

#include "coderep/data_type/string.h"
#include "coderep/tool/file.h"
#include "coderep/tool/word_tree.h"
#include <stddef.h>

struct punctuation;

struct punctuation *punctuation_alloc(struct allocator *allocator, const struct word_table table[], size_t table_l);

size_t punctuation_read(struct punctuation *self, struct file *file, struct string *scratch_buffer);

void punctuation_print(struct punctuation *self);

void punctuation_drop(struct punctuation **self);

#endif
