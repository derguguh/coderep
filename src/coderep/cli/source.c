#include "coderep/cli/source.h"

#include "coderep/macros.h"

#include <stdio.h>
#include <stdlib.h>

struct source *source_alloc()
{
	struct string *name = string_from_cstr("files passed as --source");
	struct list *files = list_alloc(&name, 5);

	struct source *self = malloc(sizeof(struct source));

	if (!self) goto defer;

	self->files = files;

	return self;

defer:
	MALLOC_ERR_EXIT("struct source (for CLI)");
}

int source_parse(struct source *self, const char **argv, int argc, int index)
{
	printf("Sorry, --source is not implemented yet");

	return index;
}

void source_drop(struct source **self)
{
	if (!self || !(*self)) return;

	list_drop(&(*self)->files);
	free(*self);
	*self = NULL;
}
