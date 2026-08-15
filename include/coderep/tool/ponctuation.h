#ifndef CODEREP_TOOL_PONCTUATION_H
#define CODEREP_TOOL_PONCTUATION_H

#include "coderep/tool/file.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/word_tree.h"
#include <stddef.h>

struct ponctuation;

struct ponctuation *ponctuation_alloc(struct allocator *allocator, struct word_table *table, size_t table_l);

size_t ponctuation_read(struct ponctuation *self, struct file *file);

void ponctuation_drop(struct ponctuation **self);

#endif
