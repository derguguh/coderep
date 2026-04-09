#ifndef CODEREP_DATA_TYPES_STRING_H
#define CODEREP_DATA_TYPES_STRING_H

#include <stddef.h>
#include <stdbool.h>

/*
 * This string struct is supposed to be possible to be used:
 *  - C POSIX str* functions
 *  - %s in format functions
 *  - to be used like a normal null terminated C string
 *
 *  For these reasons, length should NOT be equals or greater than the capacity,
 * because this will leave at least one character to be null terminated.
 *  I recommend making all not used chars null terminated, but is just a recommendation.
 */
struct string {
    char  *chars;
    size_t capacity;
    size_t length;
};

struct string *string_new();

void string_resize(struct string *self, size_t new_size);
void string_trim(struct string *self);

void string_push(struct string *self, char char_);

void string_append_cstr(struct string *self, const char *const cstr);
void string_append(struct string *self, const struct string *str);

bool string_is_equals(struct string *self, struct string *str);

void string_drop(struct string **self);

#endif
