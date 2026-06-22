#ifndef CODEREP_DATA_TYPE_STRING_H
#define CODEREP_DATA_TYPE_STRING_H

#include "coderep/allocator/allocator.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

/*
 * This string struct is NOT supposed to be used in/as:
 *  - C POSIX str* functions
 *  - %s in format functions
 *  - to be used like a normal null terminated C string
 *
 *  It CAN be used as C string with a conversion with string_to_cstr, but only after this
 */
struct string {
	struct allocator *allocator;
	char  *chars;
	size_t capacity;
	size_t length;
};

// A const slice of a string, that you can turn into a new string
struct string_slice {
	const char *const chars;
	const size_t length;
	const size_t offset;
};

extern const size_t STRING_DEFAULT_INITIAL_CAPACITY;

// Allocations
struct string *string_alloc(struct allocator *allocator, size_t initial_capacity);
struct string *string_from_cstr(struct allocator *allocator, const char *const cstr);
struct string *string_clone(const struct string *self, struct allocator *allocator, bool trimmed);

// Resizing
void string_resize(struct string *self, size_t new_capacity);
static inline void string_expand(struct string *self)
{
	string_resize(self, self->capacity * 2);
}

static inline void string_trim(struct string *self)
{
	string_resize(self, self->length);
}

// Adding or removing from
char string_pop(struct string *self);
void string_push(struct string *self, char char_);
void string_append(struct string *self, const struct string *str);
void string_append_cstr(struct string *self, const char *const cstr);

// From it
bool string_is_empty(const struct string *self);
bool string_is_equal(const struct string *self, const struct string *str);
unsigned long string_hash(const struct string *self);
int string_count(const struct string *self, char char_);
size_t string_find(const struct string *self, char char_);

// Slicing
struct string_slice string_slice(const struct string *self, size_t from, size_t to);

// Safe access
char string_get(const struct string *self, size_t index);
char *string_to_cstr(struct string *self);

// No output
void string_clear(struct string *self);
void string_print(const struct string *self);

void string_drop(struct string **self);

#endif

