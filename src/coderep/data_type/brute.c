#include "coderep/data_type/brute.h"

#include "coderep/allocator/allocator.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/macros.h"

struct brute *brute_alloc(struct allocator *allocator, struct string **name, struct string **value)
{
	struct brute *brute = allocator->allocate(allocator->state, sizeof(struct brute));

	if (!brute) goto defer;

	*brute = (struct brute) {
		.allocator = allocator,
		.name = *name,
		.value = *value,
	};

	*name = NULL;
	*value = NULL;

	return brute;

defer:
	string_drop(name);
	string_drop(value);

	MALLOC_ERR_EXIT("struct brute");
}

struct brute *brute_from_size_t(struct allocator *allocator, struct string **name, size_t value)
{
	struct string *tmp_str = string_from_size_t(allocator, value);
	return brute_alloc(allocator, name, &tmp_str);
}

struct brute *brute_from_ssize_t(struct allocator *allocator, struct string **name, ssize_t value)
{
	struct string *tmp_str = string_from_ssize_t(allocator, value);
	return brute_alloc(allocator, name, &tmp_str);
}

void brute_print(const struct brute *self, enum output_mode output_mode)
{
	if ((output_mode & OUTPUT_MODE_RAW) == OUTPUT_MODE_RAW) {
		printf("(struct brute) { .name =");
		string_print(self->name);
		printf(", .value = ");
		string_print(self->value);
		putchar('}');
	} else if ((output_mode & OUTPUT_MODE_VALUE) == OUTPUT_MODE_VALUE) {
		string_print(self->name);
		printf(": ");
		string_print(self->value);
	} else {
		string_print(self->name);
	}
	printf("\n");
}

void brute_drop(struct brute **self)
{
	if (!self || !(*self)) return;

	string_drop(&(*self)->name);
	string_drop(&(*self)->value);
	(*self)->allocator->deallocate((*self)->allocator->state, *self);
	*self = NULL;
}

