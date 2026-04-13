#ifndef CODEREP_TOOLS_ARGUMENT_PARSER_H
#define CODEREP_TOOLS_ARGUMENT_PARSER_H

#include "data.h"
#include "coderep/data_types/list.h"
#include "coderep/data_types/string.h"

/*
 * Code Report have some arguments, besides -h or --help, and these ones are:
 *	-l or --language: set self->language to next arg
 *	-f or --filter: set self->filter to next arg
 *	-s or --source: add the next arg to self->source
 *	-i or --input: add the next arg to self->input
 *	-d or --data: indentifier the next arg and set to some "enum data" of that type
 */
struct args {
	struct list *sources;
	struct list *inputs;
	struct string *lang;
	struct string *filter;
	enum data brute;
	enum data list;
	enum data distributive;
};

struct args *args_new(char *argv[], int argc);

void args_drop(struct args **self);

#endif
