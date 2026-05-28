#include "coderep/data_type/brute.h"
#include "coderep/data_type/output_mode.h"
#include "coderep/macros.h"

#include <stdlib.h>

struct brute *brute_alloc(struct string **name, struct string **value)
{
	struct brute *brute = malloc(sizeof(struct brute));

	if (!brute) goto defer;

	brute->name = *name;
	brute->value = *value;

	*brute = (struct brute) {
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

void brute_print(const struct brute *self, enum output_mode output_mode)
{
	if ((output_mode & OUTPUT_MODE_RAW) == OUTPUT_MODE_RAW) {

		printf("(struct brute) { .name = %s, .value = %s }\n", string_to_cstr(self->name), string_to_cstr(self->value));
	} else if ((output_mode & OUTPUT_MODE_VALUE) == OUTPUT_MODE_VALUE) {

		printf("%s: %s\n", string_to_cstr(self->name), string_to_cstr(self->value));
	} else {
		printf("%s\n", string_to_cstr(self->name));
	}
}

void brute_drop(struct brute **self)
{
	if (!self || !(*self)) return;

	string_drop(&(*self)->name);
	string_drop(&(*self)->value);
	free(*self);
	*self = NULL;
}

