#include "coderep/data_types/string.h"

#include "coderep/consts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct string *new_string() {
    struct string *string = malloc(sizeof(struct string));
    char *chars = calloc(1, 1);

    if (string && chars) {
        string->capacity = 1;
        string->length = 0;
        string->chars = chars;

        return string;
    } else {
        free(string);
        free(chars);

        fprintf(stderr, ANSI_ERR "Couldn't allocate memory for struct string");
        exit(MEMORY_ALLOCATION_ERROR);
        return NULL;
    }
}

void string_resize(struct string *self, size_t new_size) {

    char *tmp = realloc(self->chars, new_size);


}

void string_push(struct string *self, char char_) {

    if (self->length + 1 >= self->capacity) string_resize(self, self->capacity * 2);

    self->chars[self->length++] = char_;
    self->chars[self->length] = '\0';
}

void string_append_cstr(struct string *self, const char *const cstr) {

    int cstr_s = strlen(cstr);
    const struct string str = {
        .chars      = (char *const) cstr,
        .length     = cstr_s,
        .capacity   = cstr_s,
    };

    string_append(self, &str);
}

void string_append(struct string *self, const struct string *str) {

    if (self->length + str->length >= self->capacity) string_resize(self, self->length + str->length + 1);

    strncat(self->chars, str->chars, str->length);
    self->length = self->length + str->length;
}

bool string_is_equals(struct string *self, struct string *str) {

    if (self->length == str->length) return strncmp(self->chars, str->chars, self->length) == 0;
    else return false;
}

void string_drop(struct string **self) {
    if (!self || !(*self)) return;

    free((*self)->chars);
    (*self)->chars = NULL;
    (*self)->capacity = 0;
    (*self)->length = 0;

    free((*self));
    (*self) = NULL;
}

