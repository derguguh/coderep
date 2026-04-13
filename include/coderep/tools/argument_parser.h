#ifndef CODEREP_TOOLS_ARGUMENT_PARSER_H
#define CODEREP_TOOLS_ARGUMENT_PARSER_H

#include "data.h"
#include "coderep/data_types/list.h"
#include "coderep/data_types/string.h"

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
