#include "backend/general/word_tree.h"

#include "coderep/macros.h"

#include <string.h>

struct word_tree {
	struct allocator *allocator;
};

struct word_tree *word_tree_from_table(struct allocator *allocator, const struct word_table *table_array, size_t table_array_l)
{
	struct word_tree *self = allocator->allocate(allocator->state, sizeof(struct word_table));
	if (!self) goto defer;

	return self;
defer:
	MALLOC_ERR_EXIT("\tstruct bimap");
}

ssize_t categorize_word_via_tree(struct string *word, struct word_tree *table_array)
{
	return -1;
}

