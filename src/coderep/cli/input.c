#include "coderep/cli/input.h"

#include "coderep/macros.h"

#include <stdlib.h>

struct input *input_alloc()
{
	struct input *self = malloc(sizeof(struct input));
	const char **files = calloc(sizeof(char *), FILES_DEFAULT_CAPACITY);

	if (!self) goto defer;

	*self = (struct input) {
		.files = files,
		.files_c = FILES_DEFAULT_CAPACITY,
	};

	return self;

defer:
	MALLOC_ERR_EXIT("struct input (for CLI)");
}

void input_expand(struct input *self)
{
	self->files_c *= 2;
	const char **tmp = realloc(self->files, sizeof(char *) * self->files_c);

	if (!tmp) goto defer;

	self->files = tmp;

	return;
defer:
	PRINT_ERR("Couldn't reallocate memory to handle all files passed");
	exit(ERROR_CODE_MEMORY_REALLOCATION);
}

void input_push_file(struct input *self, const char *file)
{
	if (self->files_l >= self->files_c) input_expand(self);
	self->files[self->files_l] = file;

	self->files_l++;
}

int input_parse(struct input *self, const char **argv, int argc, int index)
{
	if (index >= argc) {

		PRINT_ERR("'--input' option expects at least one flag");
		return index;
	}

	while (index < argc) {

		const char *arg = argv[index];
		if (arg[0] == '-') break;

		FILE *file = fopen(arg, "r");

		if (!file) {
			PRINT_ERR("'--input' option expects a valid path for every input");
			return index;
		}

		fclose(file);
		input_push_file(self, arg);

		index++;
	}

	return index;
}

void input_drop(struct input **self)
{
	if (!self || !(*self)) return;

	free((*self)->files);
	(*self)->files = NULL;

	free(*self);
	*self = NULL;
}

