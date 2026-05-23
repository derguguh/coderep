#include "coderep/cli/input.h"

#include "coderep/data_types/string.h"
#include "coderep/macros.h"

#include <stdlib.h>

struct input *input_alloc()
{
	struct string *name = string_from_cstr("files passed as --input");
	struct list *files = list_alloc(&name, 5);

	struct input *self = malloc(sizeof(struct input));

	if (!self) goto defer;

	self->files = files;

	return self;

defer:
	MALLOC_ERR_EXIT("struct input (for CLI)");
}

int input_parse(struct input *self, const char **argv, int argc, int index)
{
	if (index >= argc) goto defer;

	struct string *file_path = string_from_cstr(argv[index]);
	FILE *file = fopen(argv[index], "r");

	if (!file) goto defer;

	fclose(file);
	list_push(self->files, &file_path);

	return index + 1;
defer:
	PRINT_ERR("'--input' option expects a single flag, that also should contains a valid file path");
	return index;
}

void input_drop(struct input **self)
{
	if (!self || !(*self)) return;

	list_drop(&(*self)->files);
	free(*self);
	*self = NULL;
}

