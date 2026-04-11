#include "coderep/data_types/brute.h"
#include "coderep/consts.h"

#include <stdlib.h>

struct brute *brute_new(struct string **name, struct string **value)
{
	struct brute *brute = malloc(sizeof(struct brute));
	if (!brute) {

		fprintf(stderr, ANSI_ERR "Couldn't allocate memory for a struct brute\n");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
		return NULL;
	}

	brute->name = *name;
	brute->value = *value;
	*name = NULL;
	*value = NULL;
	return brute;
}

void brute_print(struct brute *self)
{
	string_print(self->name);
	putchar(':');
	putchar(' ');
	string_print(self->value);
	putchar('\n');
}

void brute_drop(struct brute **self)
{
	if (!self || !(*self)) return;

	string_drop(&((*self)->name));
	string_drop(&((*self)->value));
	free((*self));
	*self = NULL;
}

