#include "coderep/tool/ponctuation.h"

#include "coderep/macros.h"
#include "coderep/tool/word_tree.h"
#include <string.h>

struct char_table {
	char char_;
	size_t id;
};

struct ponctuation {
	struct allocator *allocator;
	struct char_table *single_char;
	size_t single_char_l;
	struct char_table *multi_char;
	size_t multi_char_l;
	struct word_table *tables;
};

// I think I can alloc 256 of each char_table instead, and use the character as index, this would speed up kinda a bit
// I have an idea... Radix Tree can be used here
struct ponctuation *ponctuation_alloc(struct allocator *allocator, struct word_table *tables, size_t tables_l)
{
	struct char_table *single_char = allocator->allocate(allocator->state, sizeof(struct char_table) * tables_l);
	struct char_table *multi_char = allocator->allocate(allocator->state, sizeof(struct char_table) * tables_l);
	struct word_table *multi_char_table = allocator->allocate(allocator->state, sizeof(struct word_table) * tables_l);
	struct ponctuation *self = allocator->allocate(allocator->state, sizeof(struct ponctuation));
	if (!self || !single_char || !multi_char || !multi_char_table) goto defer;

	*self = (struct ponctuation) {
		.allocator = allocator,
		.single_char = single_char,
		.multi_char = multi_char,
		.tables = multi_char_table,
	};

	for (size_t i = 0; i < tables_l; i++) {
		struct word_table table = tables[i];
		size_t len = strlen(table.word);

		if (len == 0) print_err("Founded empty ponctuation passed to struct ponctuation allocation");
		else if (len == 1) {
			self->single_char[self->single_char_l].char_ = table.word[0];
			self->single_char[self->single_char_l].id = table.id;
			self->single_char_l++;
		} else {
			self->multi_char[self->multi_char_l].char_ = table.word[0];
			self->multi_char[self->multi_char_l].id = table.id;
			self->tables[self->multi_char_l] = table;
			self->multi_char_l++;
		}
	}

	return self;
defer:
	ALLOC_ERR_EXIT("struct ponctuation");
}

size_t ponctuation_read(struct ponctuation *self, struct file *file)
{
}

void ponctuation_drop(struct ponctuation **self)
{
	if (!self || !(*self)) return;

	(*self)->allocator->deallocate((*self)->allocator->state, (*self)->single_char);
	(*self)->single_char = NULL;

	(*self)->allocator->deallocate((*self)->allocator->state, (*self)->multi_char);
	(*self)->multi_char = NULL;

	(*self)->allocator->deallocate((*self)->allocator->state, (*self)->tables);
	(*self)->tables = NULL;

	(*self)->allocator->deallocate((*self)->allocator->state, *self);
	*self = NULL;
}

