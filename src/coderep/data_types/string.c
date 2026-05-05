#include "coderep/data_types/string.h"

#include "coderep/consts.h"

#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

struct string *string_alloc()
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

		PRINT_ERR("Couldn't allocate memory for struct string");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
		return NULL;
	}
}

struct string *string_from_cstr(const char *const cstr)
{
	struct string *string = string_alloc();
	string_append_cstr(string, cstr);

	return string;
}

struct string *string_clone(const struct string *self)
{
	struct string *string = string_alloc();
	string_append(string, self);

	return string;
}

void string_resize(struct string *self, size_t new_capacity)
{
	char *tmp = realloc(self->chars, new_capacity);
	if (!tmp) {
		string_drop(&self);

		PRINT_ERR("Couldn't reallocate memory for resizig struct string");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
		return;
	}
	
	if (self->length > new_capacity) self->length = new_capacity;
	self->capacity = new_capacity;
	self->chars = tmp;
}

void string_expand(struct string *self)
{
	string_resize(self, self->capacity * 2);
}

void string_trim(struct string *self)
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
	if (self->length >= self->capacity) string_expand(self);

	self->chars[self->length] = char_;
	self->length++;
}

void string_append(struct string *self, const struct string *str)
{
	if (self->length + str->length > self->capacity) string_resize(self, self->length + str->length);

	memcpy(self->chars + self->length, str->chars, str->length);
	self->length += str->length;
}

void string_append_cstr(struct string *self, const char *const cstr)
{
	int cstr_s = strlen(cstr);
	const struct string str = {
		.chars	  = (char *const) cstr,
		.length	 = cstr_s,
		.capacity   = cstr_s, };

	string_append(self, &str);
}

bool string_is_empty(const struct string *self)
{
	return self->length == 0;
}

bool string_is_equal(const struct string *self, const struct string *str)
{
	if (self->length == str->length) return memcmp(self->chars, str->chars, self->length) == 0;
	else return false;
}

unsigned long string_hash(const struct string *self)
{
	unsigned long hash = 5381;
	for (size_t i = 0; i < self->length; i++)
		hash = ((hash << 5) + hash) + self->chars[i];
	return hash;
}

int string_count(const struct string *self, char char_)
{
	int count = 0;
	for (size_t i = 0; i < self->length; i++) if (self->chars[i] == char_) count++;
	
	return count;
}

size_t string_find(const struct string *self, char char_)
{
	for (size_t i = 0; i < self->length; i++) if (self->chars[i] == char_) return i;
	return -1;
}

struct string_slice string_slice(const struct string *self, size_t from, size_t to)
{
	if (from > to || from > self->length) return (struct string_slice) { self->chars, 0, 0 };
	if (to > self->length) to = self->length;

	return (struct string_slice) { (self->chars) + from, to - from, from };
}

char string_get(const struct string *self, size_t index)
{
	if (self->length > index) return self->chars[index];
	else return '\n';
}

char *string_to_cstr(struct string *self)
{
	if (self->chars[self->length - 1] != '\0') string_push(self, '\0');
	return self->chars;
}

void string_clear(struct string *self)
{
	self->length = 0;
}

void string_print(const struct string *self)
{
	for (int i = 0; i < self->length; i++) putchar(self->chars[i]);
}

void string_drop(struct string **self)
{
	if (!self || !(*self)) return;

	free((*self)->chars);
	free(*self);
	(*self) = NULL;
}

