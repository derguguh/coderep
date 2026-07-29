#ifndef CODEREP_TOOL_WORD_TREE_H
#define CODEREP_TOOL_WORD_TREE_H

#include "coderep/data_type/string.h"

#include <stddef.h>

struct word_table {
	const char *const word;
	ssize_t id;
};

struct word_tree;

struct word_tree *word_tree_from_table(struct allocator *allocator, const struct word_table *table_array, size_t table_array_l);

const struct string *word_tree_get_by_id(const struct word_tree *self, ssize_t id);

ssize_t word_tree_categorize(const struct word_tree *self, const struct string *word);

#endif
