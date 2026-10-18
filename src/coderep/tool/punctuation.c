#include "coderep/tool/punctuation.h"

#include "coderep/macros.h"
#include "coderep/tool/word_tree.h"
#include <string.h>

struct punctuation_tree {
	char char_;
	ssize_t id;

	struct punctuation_tree **children;
	size_t children_l;
};

struct punctuation {
	struct allocator *allocator;

	ssize_t *single_chars;
	struct punctuation_tree **multi_chars;
};

void punctuation_tree_init(struct allocator *allocator, struct word_table for_, size_t index, struct word_table *tables, size_t tables_l)
{
	size_t n = 0;
	for (size_t i = 0; i < tables_l; i++) {
		if (strlen(tables[i].word) > index
			&& tables[i].word[0] == for_.word[0]
			&& tables[i].word[index] == for_.word[index]
			) {
			n++;
		}
	}

	struct punctuation_tree *tree = allocator->allocate(allocator->state, sizeof(struct punctuation_tree));
	struct punctuation_tree **children = allocator->allocate(allocator->state, n * sizeof(struct punctuation_tree *));
}

void punctuation_init(struct punctuation *self, struct word_table *tables, size_t tables_l)
{
	for (size_t i = 0; i < 0xff; i++) self->single_chars[i] = -1;
	memset(self->multi_chars, 0, 0xff * sizeof(ssize_t));

	for (size_t i = 0; i < tables_l; i++) {
		size_t length = strlen(tables[i].word);

		if (length == 1) self->single_chars[(unsigned char)tables[i].word[0]] = tables[i].id;
		else {
			punctuation_tree_init(self->allocator, tables[i], 0, tables, tables_l);
		}
	}
}

struct punctuation *punctuation_alloc(struct allocator *allocator, struct word_table *tables, size_t tables_l)
{
	ssize_t *single_chars = allocator->allocate(allocator->state, 0xff * sizeof(ssize_t));
	struct punctuation_tree **multi_chars = allocator->allocate(allocator->state, 0xff * sizeof(struct punctuation_tree *));
	struct punctuation *self = allocator->allocate(allocator->state, sizeof(struct punctuation));
	if (!self) goto defer;

	*self = (struct punctuation) {
		.allocator = allocator,
		.single_chars = single_chars,
		.multi_chars = multi_chars,
	};

	punctuation_init(self, tables, tables_l);

	return self;
defer:
	allocator->deallocate(allocator->state, self);
	allocator->deallocate(allocator->state, single_chars);
	allocator->deallocate(allocator->state, multi_chars);
	ALLOC_ERR_EXIT("struct punctuation");
}

size_t punctuation_read(struct punctuation *self, struct file *file)
{
}

void punctuation_drop(struct punctuation **self)
{
}

