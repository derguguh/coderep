#include "coderep/tool/punctuation.h"

#include "coderep/allocator/allocator.h"
#include "coderep/macros.h"
#include "coderep/tool/common_readers.h"
#include "coderep/tool/file.h"
#include "coderep/tool/file_buffer.h"
#include "coderep/tool/word_tree.h"
#include <string.h>

#define CHAR_MAX 0xff

// ------------ Internal structure ------------

struct punctuation_tree {
	char char_;
	size_t id;

	struct punctuation_tree **children;
	size_t children_c;
	size_t children_l;
};

struct punctuation {
	struct allocator *allocator;

	size_t single_chars[CHAR_MAX];
	struct punctuation_tree *_multi_chars[CHAR_MAX];
};

// ------------ Helpers ------------

static inline void print_identation(size_t level)
{
	for (size_t i = 0; i < level; i++) print("\t");
}

// ------------ struct punctuation_tree related ------------

struct punctuation_tree *punctuation_tree_alloc(struct allocator *allocator, const struct word_table for_, size_t char_index, size_t children_n)
{
	struct punctuation_tree *self = allocator->allocate(allocator->state, sizeof(struct punctuation_tree));
	struct punctuation_tree **children = allocator->allocate(allocator->state, children_n * sizeof(struct punctuation_tree *));
	if (!self || !children) goto defer;

	*self = (struct punctuation_tree) {
		.char_ = for_.word[char_index],
		.id = for_.id,
		.children = children,
		.children_c = children_n,
	};

	return self;
defer:
	ALLOC_ERR_EXIT("struct punctuation_tree");
}

void punctuation_tree_init(struct punctuation_tree *self, struct allocator *allocator,
		const char *cstr, size_t cstr_l, const struct word_table tables[], size_t tables_l)
{
	for (size_t i = 0; i < tables_l; i++) {
		if (strlen(tables[i].word) > cstr_l && strncmp(cstr, tables[i].word, cstr_l) == 0) {
			self->children[self->children_l] = punctuation_tree_alloc(allocator, tables[i], cstr_l, self->children_l);
			punctuation_tree_init(self->children[self->children_l], allocator, tables[i].word, cstr_l + 1, tables, tables_l);
			self->children_l++;
		}
	}

}

size_t punctuation_tree_read(struct punctuation_tree *self, struct file *file, size_t peek_level, struct string *scratch_buffer)
{
	char peek = file_buffer_peek_n(file->buffer, peek_level);

	for (size_t i = 0; i < self->children_l; i++) {
		struct punctuation_tree *child = self->children[i];
		if (child->char_ == peek) return punctuation_tree_read(child, file, peek_level + 1, scratch_buffer);
	}

	for (size_t i = 0; i < peek_level; i++) push_and_advance(file, scratch_buffer);
	return self->id;
}

void punctuation_tree_print(struct punctuation_tree *self, size_t identation_level)
{
	print_identation(identation_level);
	println("(struct punctuation_tree) {");

	print_identation(identation_level + 1);
	println(".children = [");
	for (size_t i = 0; i < self->children_l; i++) punctuation_tree_print(self->children[i], identation_level + 2);
	print_identation(identation_level + 1);
	println("],");

	print_identation(identation_level + 1);
	print(".char_ = ");
	print_char(self->char_);
	println(",");

	print_identation(identation_level + 1);
	print(".id = ");
	print_size_t(self->id);
	println(",");

	print_identation(identation_level);
	println("}");
}

void punctuation_tree_drop(struct punctuation_tree **self, struct allocator *allocator)
{
	if (!self || !(*self)) return;

	for (size_t i = 0; i < (*self)->children_l; i++) punctuation_tree_drop(&(*self)->children[i], allocator);

	allocator->deallocate(allocator, (*self)->children);
	(*self)->children = NULL;

	allocator->deallocate(allocator, *self);
	*self = NULL;
}

// ------------ struct punctuation related ------------

void punctuation_init(struct punctuation *self, const struct word_table tables[], size_t tables_l)
{
	size_t multi_char_n = tables_l;
	for (size_t i = 0; i < tables_l; i++) {
		size_t length = strlen(tables[i].word);

		if (length == 1) {
			self->single_chars[(unsigned char)tables[i].word[0]] = tables[i].id;
			multi_char_n--;
		} else if (self->_multi_chars[(unsigned char)tables[i].word[0]] == NULL) {
			unsigned char id = tables[i].word[0];
			self->_multi_chars[id] = punctuation_tree_alloc(self->allocator, tables[i], 0, multi_char_n);
			self->_multi_chars[id]->id = id;
			punctuation_tree_init(self->_multi_chars[id], self->allocator, tables[i].word, 1, tables, tables_l);
		}
	}
}

struct punctuation *punctuation_alloc(struct allocator *allocator, const struct word_table tables[], size_t tables_l)
{
	struct punctuation *self = allocator->allocate(allocator->state, sizeof(struct punctuation));
	if (!self) goto defer;

	*self = (struct punctuation) {
		.allocator = allocator,
	};

	punctuation_init(self, tables, tables_l);

	return self;
defer:
	ALLOC_ERR_EXIT("struct punctuation");
}

size_t punctuation_read(struct punctuation *self, struct file *file, struct string *scratch_buffer)
{
	struct punctuation_tree *tree = self->_multi_chars[(unsigned char)file->buffer->char_];
	if (tree != NULL) {
		size_t cursor = file->buffer->buffer_cursor;
		size_t value = punctuation_tree_read(tree, file, 1, scratch_buffer);
		if (cursor + 1 == file->buffer->buffer_cursor) return self->single_chars[(unsigned char)tree->char_];
		else return value;
	} else {
		size_t value = self->single_chars[(unsigned char)file->buffer->char_];
		push_and_advance(file, scratch_buffer);
		return value;
	}
}

void punctuation_print(struct punctuation *self)
{
	println("struct punctuation {");

	println("\t.single_chars [ (only != 0 are printed)");
	for (size_t i = 0; i < CHAR_MAX; i++) {
		if (self->single_chars[i] != 0) {
			print("\t\t[");
			print_char(i);
			print("] = ");
			print_size_t(self->single_chars[i]);
			println(",");
		}
	}
	println("\t],");

	println("\t._multi_chars [ (only != 0 are printed)");
	for (size_t i = 0; i < CHAR_MAX; i++) {
		if (self->_multi_chars[i] != NULL) punctuation_tree_print(self->_multi_chars[i], 2);
	}
	println("\t],");

	println("}");
}

void punctuation_drop(struct punctuation **self)
{
	if (!self || !(*self)) return;
	struct allocator *allocator = (*self)->allocator;

	for (size_t i = 0; i < CHAR_MAX; i++) punctuation_tree_drop(&(*self)->_multi_chars[i], allocator);

	allocator->deallocate(allocator, *self);
	*self = NULL;
}

