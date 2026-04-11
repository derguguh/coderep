#include "coderep/data_types/string.h"

#include "coderep/consts.h"

#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

struct string *string_new()
{
	struct string *string = malloc(sizeof(struct string));
	char *chars = calloc(1, INITIAL_CAPACITY);

	if (string && chars) {
		string->capacity = INITIAL_CAPACITY;
		string->length = 0;
		string->chars = chars;

		return string;
	} else {
		free(string);
		free(chars);

		fprintf(stderr, ANSI_ERR "Couldn't allocate memory for struct string\n");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
		return NULL;
	}
}

void string_resize(struct string *self, size_t new_size)
{
	char *tmp = realloc(self->chars, new_size);
	if (!tmp) {
		fprintf(stderr, ANSI_ERR "Couldn't reallocate memory for resizig struct string\n");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
	}
	
	if (self->length > new_size) self->length = new_size;
	self->capacity = new_size;
	self->chars = tmp;
}

static inline void string_expand(struct string *self)
{
	string_resize(self, self->capacity * 2);
}

static inline void string_trim(struct string *self)
{
	string_resize(self, self->length);
}

char string_pop(struct string *self)
{
	if (self->length > 0) {
		self->length--;
		return self->chars[self->length];
	} else return '\0';
}

void string_push(struct string *self, char char_)
{
	self->length++;
	if (self->length > self->capacity) string_expand(self);

	self->chars[self->length] = char_;
}

void string_append(struct string *self, const struct string *str)
{
	self->length = self->length + str->length;
	if (self->length > self->capacity) string_resize(self, self->length + str->length);

	memcpy(self->chars, str->chars, str->length);
}

void string_append_cstr(struct string *self, const char *const cstr)
{
	int cstr_s = strlen(cstr);
	const struct string str = {
		.chars	  = (char *const) cstr,
		.length	 = cstr_s,
		.capacity   = cstr_s,
	};

	string_append(self, &str);
}

bool string_is_equals(struct string *self, struct string *str)
{
	if (self->length == str->length) return memcmp(self->chars, str->chars, self->length) == 0;
	else return false;
}

char *string_to_cstr(struct string *self)
{
	if (self->chars[self->length - 1] != '\0') string_push(self, '\0');
	return self->chars;
}

void string_drop(struct string **self)
{
	if (!self || !(*self)) return;

	free((*self)->chars);
	free((*self));
	(*self) = NULL;
}

