#include "coderep/tools/argument_parser.h"
#include "coderep/consts.h"
#include "coderep/tools/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_EQUALS_TO(arg, short_name, long_name) strcmp(arg, short_name) == 0 || strcmp(arg, long_name) == 0

void args_parse_data(struct args *self, char **argv, int argc, int *i)
{
	(*i)++;
	char *data = argv[(*i)];

	if	(strcmp(data, "brute.none") == 0)		self->brute	= NONE;
	else if	(strcmp(data, "brute.raw") == 0)		self->list	|= LIST_RAW;
	else if	(strcmp(data, "list.none") == 0)		self->list	= NONE;
	else if	(strcmp(data, "list.raw") == 0)			self->list	|= LIST_RAW;
	else if (strcmp(data, "distributive.none") == 0)	self->distributive = NONE;
	else if (strcmp(data, "distributive.raw") == 0)		self->distributive |= DISTRIBUTIVE_RAW;
	else if (strcmp(data, "distributive.min") == 0)		self->distributive |= DISTRIBUTIVE_MIN;
	else if (strcmp(data, "distributive.max") == 0)		self->distributive |= DISTRIBUTIVE_MAX;
	else if (strcmp(data, "distributive.avg") == 0)		self->distributive |= DISTRIBUTIVE_AVG;
	else if (strcmp(data, "distributive.moda") == 0)	self->distributive |= DISTRIBUTIVE_MODA;
	else if (strcmp(data, "distributive.median") == 0)	self->distributive |= DISTRIBUTIVE_MEDIAN;
	else if (strcmp(data, "distributive.frequency") == 0)	self->distributive |= DISTRIBUTIVE_FREQUENCY;
	else if (strcmp(data, "distributive.value") == 0)	self->distributive |= DISTRIBUTIVE_VALUE;
	else fprintf(stderr, ANSI_ERR "'%s' is not a valid --data argument\n", data);
}

void args_parse(struct args *self, char **argv, int argc)
{
	for (int i = 1; i + 1 < argc; i += 2) {
		char *arg = argv[i];

		if	(IS_EQUALS_TO(arg, "-d", "--data")) args_parse_data(self, argv, argc, &i);
		else fprintf(stderr, ANSI_ERR "'%s' is not a valid argument\n", arg);
	}
}

struct args *args_new(char **argv, int argc)
{
	struct args *args = malloc(sizeof(struct args));

	if (!args) {
		fprintf(stderr, ANSI_ERR "Couldn't allocate memory for struct args\n");
		exit(ERROR_CODE_MEMORY_ALLOCATION);
		return NULL;
	} else {
		struct string *string = string_from_cstr("Sources");
		args->sources = list_new(&string, 2);
		string = string_from_cstr("Inputs");
		args->inputs = list_new(&string, 4);

		args->lang = string_new();
		args->filter = string_new();
		
		args->brute = BRUTE_DEFAULT;
		args->list = LIST_DEFAULT;
		args->distributive = DISTRIBUTIVE_DEFAULT;

		args_parse(args, argv, argc);
		
		return args;
	}
}

void args_drop(struct args **self)
{
	if (!self || !(*self)) return;

	list_drop(&(*self)->inputs);
	list_drop(&(*self)->sources);
	string_drop(&(*self)->lang);
	string_drop(&(*self)->filter);
	free(*self);
	*self = NULL;
}

