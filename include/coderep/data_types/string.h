#ifndef CODEREP_DATA_TYPES_STRING_H
#define CODEREP_DATA_TYPES_STRING_H

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
	char  *chars;
	size_t capacity;
	size_t length;
};

struct string *string_new();

void string_resize(struct string *self, size_t new_size);
void string_expand(struct string *self);
void string_trim(struct string *self);

char string_pop(struct string *self);
void string_push(struct string *self, char char_);
void string_append(struct string *self, const struct string *str);
void string_append_cstr(struct string *self, const char *const cstr);

bool string_is_equals(const struct string *self, const struct string *str);
unsigned long string_hash(const struct string *self);

char *string_to_cstr(struct string *self);

void string_print(const struct string *self);

void string_drop(struct string **self);

#endif

