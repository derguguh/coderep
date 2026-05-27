#include "coderep/cli/input.h"

#include "coderep/data_type/string.h"
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
	if (index >= argc) {

		PRINT_ERR("'--input' option expects at least one flag");
		return index;
	}

	struct string *arg = string_alloc();
	while (index < argc && string_get(arg, 0) != '-') {

		string_append_cstr(arg, argv[index]);
		if (string_get(arg, 0) == '-') break;

		FILE *file = fopen(argv[index], "r");

		if (!file) {
			PRINT_ERR("'--input' option expects a valid path for every input");
			goto defer;
		}

		fclose(file);
		list_push(self->files, &arg);

		string_clear(arg);
		index++;
	}

defer:
	string_drop(&arg);
	return index;
}

void input_drop(struct input **self)
{
	if (!self || !(*self)) return;

	list_drop(&(*self)->files);
	free(*self);
	*self = NULL;
}

