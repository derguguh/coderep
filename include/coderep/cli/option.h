#ifndef CODEREP_CLI_OPTION_H
#define CODEREP_CLI_OPTION_H

#include "coderep/cli/config.h"

#include <stdbool.h>
#include <stddef.h>

struct argument_list {
	const char **arguments;
	size_t arguments_l;
};

typedef bool (*option_parse_fn) (struct config *config, struct argument_list *_list);

enum entry_type {
	COMPOSABLE,
	ONE_INPUT,
	MULTIPLE_INPUT,
};

struct option {
	const char *flag_name;
	char short_name;
	option_parse_fn parsing;
	enum entry_type type;
};

#endif
