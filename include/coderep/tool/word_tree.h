#ifndef CODEREP_TOOL_WORD_TREE_H
#define CODEREP_TOOL_WORD_TREE_H

#include "coderep/data_type/string.h"

#include <stddef.h>

struct word_table {
	const char *word;
	ssize_t id;
};

struct word_tree;

// ------------ Allocation ------------
struct word_tree *word_tree_alloc(struct allocator *allocator, const struct word_table *table_array, size_t table_array_l);

// ------------ Usage ------------
const struct string *word_tree_get_by_id(const struct word_tree *self, ssize_t id);
ssize_t word_tree_categorize(const struct word_tree *self, const struct string *word);

// ------------ Deallocation ------------
void word_tree_drop(struct word_tree **self);

#endif
