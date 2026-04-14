#include "coderep/tools/argument_parser.h"
#include "coderep/consts.h"
#include "coderep/tools/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct opt_table {
	char *cstr;
	int value;
};

const struct opt_table data_flags[] = {
	{ "none",	NONE },
	{ "raw",	RAW },
	{ "min",	MIN },
	{ "avg",	AVG },
	{ "max",	MAX },
	{ "moda",	MODA },
	{ "median",	MEDIAN },
	{ "value",	VALUE },
	{ "frequency",	FREQUENCY },
	{ "count",	COUNT },
};

/*
 * TODO:
 *	 This way is not complete, but at least works.
 *	 We should allow people to do data_type.sub_type1:sub_type2:sub_type3
 *	to reduce usages like data_type.sub_type1, data_type.sub_type2, data_type.sub_type3.
 *	This will need a more advanced reading, so, for now, this is enough.
 *
 *	This functions is a messy...
 */
void args_parse_data(struct args *self, char **argv, int argc, int *index)
{

}

void args_parse(struct args *self, char **argv, int argc)
{
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];

		if	(IS_THIS_ARG(arg, "-d", "--data")) args_parse_data(self, argv, argc, &i);
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
		
		args->brute = DEFAULT_BRUTE;
		args->list = DEFAULT_LIST;
		args->distributive = DEFAULT_DISTRIBUTIVE;

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

