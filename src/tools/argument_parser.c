#include "coderep/tools/argument_parser.h"
#include "coderep/consts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_EQUALS_TO_ARGS(arg, short_name, long_name) strcmp(arg, short_name) == 0 || strcmp(arg, long_name) == 0

void args_parse(struct args *self, char **argv, int argc)
{
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		int o = i + 1;
		
		if (arg[0] != '-' || o >= argc) {

			fprintf(stderr, ANSI_ERR "Unknown or invalid argument '%s' passed to coderep\n", arg);
			exit(ERROR_UNKOWN_ARGUMENT);
			break;
		}

		struct string *next_arg = string_new();
		string_append_cstr(next_arg, argv[o]);

		if	(IS_EQUALS_TO_ARGS(arg, "-l", "--language")) string_append(self->lang, next_arg);
		else if (IS_EQUALS_TO_ARGS(arg, "-f", "--filter")) string_append(self->filter, next_arg);
		else if (IS_EQUALS_TO_ARGS(arg, "-s", "--source")) list_push(self->sources, &next_arg);
		else if (IS_EQUALS_TO_ARGS(arg, "-i", "--input")) list_push(self->inputs, &next_arg);
		else fprintf(stderr, ANSI_ERR "Unknown argument '%s'\n", arg);

		string_drop(&next_arg);
		i++;
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
		struct string *string = string_new();
		string_append_cstr(string, "Sources");
		args->sources = list_new(&string, 2);
		string = string_new();
		string_append_cstr(string, "Inputs");
		args->inputs = list_new(&string, 4);

		args->lang = string_new();
		args->filter = string_new();

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

